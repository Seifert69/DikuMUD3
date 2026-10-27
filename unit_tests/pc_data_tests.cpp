#include "FixtureBase.h"
#include "descriptor_data.h"
#include "nanny.h"
#include "pc_data.h"
#include "textutil.h"
#include "utils.h"

#if defined __APPLE__ && __MACH__
    #include <unistd.h>
#elif defined LINUX
    #include <crypt.h>
#endif

#include <cctype>
#include <memory>
#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * PcData_Password_Suite - pc_data's passwords, check_pwd() and the login check
 */
BOOST_AUTO_TEST_SUITE(PcData_Password_Suite)

struct PasswordFixture : public unit_tests::FixtureBase
{
    PasswordFixture() { pc->setFilename("papi"); }

    std::unique_ptr<pc_data> pc{dynamic_cast<pc_data *>(new_unit_data(UNIT_ST_PC, nullptr))};

    // The check nanny_existing_pwd() makes at login: the stored hash is the salt
    bool logs_in(const std::string &typed) const
    {
        const char *stored = pc->getPassword();
        return pwdcompare(crypt(typed.c_str(), stored), stored, pc_data::PC_MAX_PASSWORD) == 0;
    }

    // What nanny_new_pwd() would hash: the input as check_pwd() cuts it, or "" if refused
    static std::string checked(const std::string &typed)
    {
        std::string input(typed);
        return check_pwd(nullptr, input.data()) ? input.c_str() : "";
    }
};

// The hash is stored as it is: m_pwd keeps 12 characters. nanny also clears it for a new
// connection, and copies the stored hash of a character already online.
BOOST_FIXTURE_TEST_CASE(stores_a_hash_as_it_is, PasswordFixture)
{
    pc->setPassword("papAq5PwY/QQM");
    BOOST_TEST(std::string(pc->getPassword()) == "papAq5PwY/QQ");
    pc->setPassword("");
    BOOST_TEST(std::string(pc->getPassword()).empty());
}

// nanny_new_pwd() stores crypt() of the cut input, and the confirmation and every later
// login check what the player types, uncut, against it
BOOST_FIXTURE_TEST_CASE(new_password_then_login, PasswordFixture)
{
    BOOST_TEST(pc->setNewPassword(checked("abcdefg1verylong").c_str()));
    BOOST_TEST(logs_in("abcdefg1verylong"));
    BOOST_TEST(logs_in("abcdefg1"));
    BOOST_TEST(!logs_in("abcdefg2"));
}

// libxcrypt refuses more than 512 characters (DIL setpassword can pass that many); other
// libraries hash the first 8, so there is nothing to refuse there
BOOST_FIXTURE_TEST_CASE(refuses_a_crypt_failure, PasswordFixture)
{
    const std::string too_long(600, 'a');
    const char *probe = crypt(too_long.c_str(), "pa");
    if (probe == nullptr || probe[0] == '*')
    {
        pc->setPassword("papAq5PwY/QQM");
        BOOST_TEST(!pc->setNewPassword(too_long.c_str()));
        BOOST_TEST(logs_in("password"));
    }
}

BOOST_FIXTURE_TEST_CASE(new_password_rules, PasswordFixture)
{
    BOOST_TEST(checked("abcdefg1") == "abcdefg1");
    BOOST_TEST(checked("abcdefg1\303\251") == "abcdefg1"); // é after the 8th is cut
    BOOST_TEST(checked("abcdefg1" + std::string(512, 'x')) == "abcdefg1");

    BOOST_TEST(checked("password1").empty()); // the digit is 9th: DES only reads "password"
    BOOST_TEST(checked("abc1").empty());
    BOOST_TEST(checked("12345678").empty());
    BOOST_TEST(checked("abcd\303\251f12").empty()); // é within the first 8
}

// Golden vectors: crypt(password, name) as player files store it (traditional DES, which
// hasn't changed since the 1970s; the salt is the first 2 letters of the name).
// Before #449 a new password was cut at 12 characters, so it got crypt(password); now it is
// cut at 8, and gets the same hash, because DES reads only 8. The last rows are passwords
// the old rules accepted and the new ones refuse: letters only in the first 8 (the digits
// after the 8th were never hashed), and a non-ASCII letter (ä from the web client, UTF-8).
// Whatever their rules, stored hashes log in with what the player typed before, from a
// newer player file (12 characters kept) or one at version 72 or older (10 kept).
struct GoldenVector
{
    const char *name;
    const char *password;
    const char *hash;
    bool refused_now;
};

static const GoldenVector GOLDEN[] = {
    {"papi", "abc123", "pap3Dci16rN.k", false},
    {"papi", "abcd1234", "paU/35aOpJ8OA", false},
    {"papi", "abcde12345", "paQUaYazyUhic", false},
    {"seifert", "abc123", "se9mMZZTbypbE", false},
    {"seifert", "abcd1234", "seaVPNbho1KR2", false},
    {"seifert", "abcde12345", "seGL5DbN3qxJc", false},
    {"papi", "password", "papAq5PwY/QQM", true},
    {"papi", "abcdefgh12", "paVOfag4lyqIg", true},
    {"papi", "p\303\244sswort1", "paGvDXFf5FKSo", true},
};

BOOST_FIXTURE_TEST_CASE(golden_vectors_before_and_after, PasswordFixture)
{
    for (const auto &v : GOLDEN)
    {
        BOOST_TEST_CONTEXT(v.name << " / " << v.password)
        {
            const std::string password(v.password);
            const char *hashed = crypt(v.password, v.name);
            BOOST_REQUIRE(hashed != nullptr);
            BOOST_TEST(std::string(hashed) == v.hash); // before: still the same DES

            const std::string cut = checked(password);
            BOOST_TEST(cut.empty() == v.refused_now);
            if (!v.refused_now) // after: the same hash is stored
            {
                pc->setFilename(v.name);
                BOOST_TEST(pc->setNewPassword(cut.c_str()));
                BOOST_TEST(std::string(pc->getPassword()) == std::string(v.hash).substr(0, 12));
            }

            for (bool old_file : {false, true})
            {
                pc->setPassword(v.hash);
                if (old_file)
                {
                    pc->truncatePassword();
                }
                std::string wrong_case(password);
                wrong_case[0] = static_cast<char>(toupper(wrong_case[0]));

                BOOST_TEST(logs_in(password));
                BOOST_TEST(logs_in(password + "9") == (password.size() >= 8)); // DES reads only 8
                BOOST_TEST(logs_in(password.substr(0, password.size() - 1)) == (password.size() > 8));
                BOOST_TEST(!logs_in(wrong_case));
                BOOST_TEST(!logs_in(""));
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
