#include "FixtureBase.h"
#include "OutputCapture.h"
#include "descriptor_data.h"
#include "destruct.h"
#include "hookmud.h"
#include "main_functions.h"
#include "pc_data.h"
#include "sessionlog.h"
#include "system.h"
#include "utils.h"

#include <climits>
#include <cstdio>
#include <memory>
#include <regex>

#include <boost/test/unit_test.hpp>

extern int destructed_idx[3]; // destruct.cpp

BOOST_AUTO_TEST_SUITE(SessionLog_Suite)

BOOST_AUTO_TEST_CASE(no_fields_test)
{
    BOOST_TEST(sessionlog_line(1760000000, "boot", {}) == R"({"t":1760000000,"ev":"boot"})");
}

BOOST_AUTO_TEST_CASE(fields_keep_order_test)
{
    const auto line = sessionlog_line(1760000000, "login", {{"id", 42}, {"lvl", 3}, {"new", 0}, {"web", 1}, {"played", 5400}});
    BOOST_TEST(line == R"({"t":1760000000,"ev":"login","id":42,"lvl":3,"new":0,"web":1,"played":5400})");
}

BOOST_AUTO_TEST_CASE(extreme_values_test)
{
    // Guests have id -1; played is an unsigned 32-bit counter.
    const auto line = sessionlog_line(0, "logout", {{"id", -1}, {"played", 4294967295LL}, {"min", LLONG_MIN}});
    BOOST_TEST(line == R"({"t":0,"ev":"logout","id":-1,"played":4294967295,"min":-9223372036854775808})");
}

/**
 * Gives a PC a connection through an (unhooked) web multiplexer and puts back
 * the globals the descriptor constructor changes. The events come from the
 * captured slog() calls.
 */
struct SessionLogFixture : public unit_tests::FixtureBase
{
    SessionLogFixture()
    {
        saved_list = g_descriptor_list;
        saved_connections = g_no_connections;
        saved_max_connections = g_max_no_connections;
        saved_shutdown = g_mud_shutdown;
        hook.bWebsockets = 1;
        desc = std::make_unique<descriptor_data>(&hook);
        DELETE(unit_data, desc->getCharacter()); // the constructor makes its own PC; use ours
        g_descriptor_list = saved_list;
        g_no_connections = saved_connections;
        g_max_no_connections = saved_max_connections;

        pc = std::unique_ptr<pc_data>(dynamic_cast<pc_data *>(new_unit_data(UNIT_ST_PC, nullptr)));
        desc->setCharacter(pc.get());
        unit_tests::OutputCapture::Instance()->Clear();
    }

    ~SessionLogFixture() override
    {
        UCHAR(pc.get())->setDescriptor(nullptr);
        desc->setCharacter(nullptr);
        g_mud_shutdown = saved_shutdown;
    }

    void connect() { UCHAR(pc.get())->setDescriptor(desc.get()); }

    // The JSON of each session log line so far; each must go to vme.log only (LOG_OFF), and
    // be visible at any wizinv level
    static std::vector<std::string> lines()
    {
        std::vector<std::string> out;
        for (const auto &entry : unit_tests::OutputCapture::Instance()->getSLogDataMulti())
        {
            if (entry.m_message.rfind("SESSION ", 0) == 0)
            {
                BOOST_TEST(entry.m_level == LOG_OFF);
                BOOST_TEST(entry.m_wiz_inv_level == 0);
                out.push_back(entry.m_message.substr(8));
            }
        }
        return out;
    }

    static std::vector<std::string> events()
    {
        std::vector<std::string> evs;
        for (const auto &line : lines())
        {
            auto at = line.find("\"ev\":\"");
            evs.push_back(at == std::string::npos ? line : line.substr(at + 6, line.find('"', at + 6) - at - 6));
        }
        return evs;
    }

    cMultiHook hook; // never hooked; says the connection is a web client
    descriptor_data *saved_list{nullptr};
    int saved_connections{0};
    int saved_max_connections{0};
    int saved_shutdown{0};
    std::unique_ptr<descriptor_data> desc;
    std::unique_ptr<pc_data> pc;
};

using events_t = std::vector<std::string>;

BOOST_FIXTURE_TEST_CASE(session_pairs_test, SessionLogFixture)
{
    BOOST_TEST(!pc->isSessionLogged());

    // No connection (an admin 'load'): nothing
    sessionlog_login(pc.get(), false);
    sessionlog_logout(pc.get());
    BOOST_TEST(events() == events_t{});

    connect();
    sessionlog_login(pc.get(), true);
    BOOST_TEST(pc->isSessionLogged());
    sessionlog_login(pc.get(), false); // e.g. back from the immortal menu: same session
    sessionlog_reconnect(pc.get());
    sessionlog_linkdead(pc.get());
    sessionlog_logout(pc.get());
    BOOST_TEST(!pc->isSessionLogged());
    sessionlog_logout(pc.get()); // already ended
    sessionlog_linkdead(pc.get());
    BOOST_TEST((events() == events_t{"login", "reconnect", "linkdead", "logout"}));
}

BOOST_FIXTURE_TEST_CASE(fields_test, SessionLogFixture)
{
    connect();
    pc->setPlayerUID(42);
    pc->getPCTimeInformation().setTotalTimePlayedInSeconds(5400);
    sessionlog_login(pc.get(), false);
    hook.bWebsockets = 0;
    sessionlog_reconnect(pc.get());
    sessionlog_linkdead(pc.get());
    sessionlog_logout(pc.get());
    const auto got = lines();
    BOOST_REQUIRE(got.size() == 4U);
    BOOST_TEST(got[0].find(R"("ev":"login","id":42,"lvl":0,"new":0,"web":1,"played":5400})") != std::string::npos);
    BOOST_TEST(got[1].find(R"("ev":"reconnect","id":42,"lvl":0,"web":0})") != std::string::npos);
    BOOST_TEST(got[2].find(R"("ev":"linkdead","id":42,"lvl":0})") != std::string::npos);
    // No played at logout: an unsaved character still has the time from its player file
    BOOST_TEST(got[3].find(R"("ev":"logout","id":42,"lvl":0})") != std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(vme_log_line_test, SessionLogFixture)
{
    // The line in vme.log is "<24-character time> :: SESSION <json>", and the extraction
    // documented in sessionlog.h (grep -E '^.{24} :: SESSION [{]', sed 's/^.\{24\} :: SESSION //')
    // gives back exactly the JSON
    sessionlog_boot();
    fflush(g_log_file_fd);
    rewind(g_log_file_fd);
    char buf[512];
    std::string found;
    while (fgets(buf, sizeof(buf), g_log_file_fd))
    {
        std::string line(buf);
        if (line.find(":: SESSION ") != std::string::npos)
        {
            found = line;
        }
    }
    BOOST_TEST(std::regex_match(found, std::regex(R"(\w{3} \w{3} [ \d]\d \d\d:\d\d:\d\d \d{4} :: SESSION \{.*\}\n)")));
    BOOST_TEST(std::regex_search(found, std::regex("^.{24} :: SESSION [{]")));
    const auto json = std::regex_replace(found, std::regex("^.{24} :: SESSION "), "");
    BOOST_TEST(json == lines().at(0) + "\n");
    BOOST_TEST(json.find(R"("ev":"boot"})") != std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(reconnect_without_session_is_login_test, SessionLogFixture)
{
    connect();
    sessionlog_reconnect(pc.get()); // connecting to a character an admin loaded
    BOOST_TEST(pc->isSessionLogged());
    BOOST_TEST((events() == events_t{"login"}));
}

BOOST_FIXTURE_TEST_CASE(no_linkdead_at_shutdown_test, SessionLogFixture)
{
    connect();
    sessionlog_login(pc.get(), false);
    g_mud_shutdown = 1;
    sessionlog_linkdead(pc.get());
    sessionlog_shutdown();
    BOOST_TEST((events() == events_t{"login", "shutdown"}));
}

BOOST_FIXTURE_TEST_CASE(destructed_login_test, SessionLogFixture)
{
    // A character extracted while entering the game is not logged
    connect();
    // register_destruct() appends the unit to the global destructed list: take that entry
    // out again afterwards, as the fixture deletes the unit
    pc->register_destruct();
    sessionlog_login(pc.get(), true);
    destructed_idx[DR_UNIT]--;
    pc->undo_destruct();
    BOOST_TEST(!pc->isSessionLogged());
    BOOST_TEST(events() == events_t{});
}

BOOST_FIXTURE_TEST_CASE(reconnect_without_descriptor_test, SessionLogFixture)
{
    connect();
    sessionlog_login(pc.get(), false);
    UCHAR(pc.get())->setDescriptor(nullptr);
    sessionlog_reconnect(pc.get());
    BOOST_TEST((events() == events_t{"login"}));
}

BOOST_FIXTURE_TEST_CASE(reconnect_as_login_keeps_new_test, SessionLogFixture)
{
    // A reconnect logged as a login: a never-saved character is still new
    connect();
    sessionlog_reconnect(pc.get());
    const auto got = lines();
    BOOST_REQUIRE(got.size() == 1U);
    BOOST_TEST(got[0].find(R"("ev":"login")") != std::string::npos);
    BOOST_TEST(got[0].find(R"("new":1)") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
