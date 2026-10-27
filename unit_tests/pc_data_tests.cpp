#include "FixtureBase.h"
#include "pc_data.h"
#include "textutil.h"
#include "utils.h"

#if defined __APPLE__ && __MACH__
    #include <unistd.h>
#elif defined LINUX
    #include <crypt.h>
#endif

#include <cstring>
#include <memory>
#include <string>

#include <boost/test/unit_test.hpp>

int check_pwd(descriptor_data *d, char *pwd); // nanny.cpp

/**********************************************************************************
 * PcData_Password_Suite - pc_data::setPassword(), check_pwd() and the login check
 */
BOOST_AUTO_TEST_SUITE(PcData_Password_Suite)

struct PasswordFixture : public unit_tests::FixtureBase
{
    std::unique_ptr<pc_data> pc{dynamic_cast<pc_data *>(new_unit_data(UNIT_ST_PC, nullptr))};

    // The check nanny_existing_pwd() makes at login: the stored hash is the salt
    bool logs_in(const char *typed) const
    {
        const char *stored = pc->getPassword();
        return pwdcompare(crypt(typed, stored), stored, pc_data::PC_MAX_PASSWORD) == 0;
    }

    // check_pwd() cuts its argument in place, as nanny_new_pwd() passes the input line
    static std::string checked(const char *typed, bool &accepted)
    {
        std::string input(typed);
        accepted = check_pwd(nullptr, input.data()) != 0;
        return input.c_str();
    }
};

// Traditional DES crypt() of "password" with the salt "pa" (a player named Papi). DES crypt
// hasn't changed since the 1970s, so player files from the 1990s hold hashes like this one.
static const char *const PAPI_PASSWORD_HASH = "papAq5PwY/QQM";

BOOST_FIXTURE_TEST_CASE(stores_a_hash_as_before, PasswordFixture)
{
    BOOST_TEST(pc->setPassword(PAPI_PASSWORD_HASH));
    BOOST_TEST(std::string(pc->getPassword()) == "papAq5PwY/QQ"); // m_pwd keeps 12 characters
    BOOST_TEST(pc->setPassword(""));                              // nanny clears it for a new connection
    BOOST_TEST(std::string(pc->getPassword()).empty());
}

BOOST_FIXTURE_TEST_CASE(refuses_a_crypt_failure, PasswordFixture)
{
    pc->setPassword(PAPI_PASSWORD_HASH);
    BOOST_TEST(!pc->setPassword(nullptr));
    BOOST_TEST(!pc->setPassword("*0"));
    BOOST_TEST(!pc->setPassword(crypt(std::string(600, 'a').c_str(), "papi"))); // NULL or "*0"
    BOOST_TEST(std::string(pc->getPassword()) == "papAq5PwY/QQ");
    BOOST_TEST(logs_in("password"));
}

// A player file at version 72 or older keeps only the first 10 characters of the hash
// (read_unit_string() calls truncatePassword()), newer ones 12. Both log in as before.
BOOST_FIXTURE_TEST_CASE(existing_players_log_in, PasswordFixture)
{
    for (bool old_file : {true, false})
    {
        pc->setPassword(PAPI_PASSWORD_HASH);
        if (old_file)
        {
            pc->truncatePassword();
        }
        BOOST_TEST(logs_in("password"));
        BOOST_TEST(logs_in("password12")); // DES never read past the 8th character
        BOOST_TEST(!logs_in("passwor"));
        BOOST_TEST(!logs_in("Password"));
        BOOST_TEST(!logs_in(""));
    }
}

// A second connection for a character already online borrows its stored hash
BOOST_FIXTURE_TEST_CASE(borrowed_password_kept, PasswordFixture)
{
    std::unique_ptr<pc_data> online{dynamic_cast<pc_data *>(new_unit_data(UNIT_ST_PC, nullptr))};
    online->setPassword(PAPI_PASSWORD_HASH);
    online->truncatePassword();

    BOOST_TEST(pc->setPassword(online->getPassword()));
    BOOST_TEST(std::string(pc->getPassword()) == online->getPassword());
    BOOST_TEST(logs_in("password"));
}

BOOST_FIXTURE_TEST_CASE(new_password_rules, PasswordFixture)
{
    bool accepted = false;

    BOOST_TEST(checked("abcdefg1", accepted) == "abcdefg1");
    BOOST_TEST(accepted);
    BOOST_TEST(checked("abcdefg1\303\251", accepted) == "abcdefg1"); // é after the 8th is cut
    BOOST_TEST(accepted);
    BOOST_TEST(checked(("abcdefg1" + std::string(512, 'x')).c_str(), accepted) == "abcdefg1");
    BOOST_TEST(accepted);

    checked("password1", accepted); // the digit is 9th: DES only reads "password"
    BOOST_TEST(!accepted);
    checked("abc1", accepted);
    BOOST_TEST(!accepted);
    checked("12345678", accepted);
    BOOST_TEST(!accepted);
    checked("abcd\303\251f12", accepted); // é within the first 8
    BOOST_TEST(!accepted);
}

// nanny_new_pwd() stores crypt() of the cut input with the name as salt, and
// nanny_pwd_confirm() and the next login check the player's uncut input against it
BOOST_FIXTURE_TEST_CASE(new_password_then_login, PasswordFixture)
{
    bool accepted = false;
    const std::string cut = checked("abcdefg1verylong", accepted);
    BOOST_REQUIRE(accepted);
    BOOST_TEST(pc->setPassword(crypt(cut.c_str(), "papi")));

    BOOST_TEST(pwdcompare(crypt("abcdefg1verylong", "papi"), pc->getPassword(), pc_data::PC_MAX_PASSWORD) == 0);
    BOOST_TEST(logs_in("abcdefg1verylong"));
    BOOST_TEST(logs_in("abcdefg1"));
    BOOST_TEST(!logs_in("abcdefg2"));
}

BOOST_AUTO_TEST_SUITE_END()
