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
 * the globals the descriptor constructor and the player count change. The
 * events come from the captured slog() calls.
 */
struct SessionLogFixture : public unit_tests::FixtureBase
{
    SessionLogFixture()
    {
        saved_list = g_descriptor_list;
        saved_connections = g_no_connections;
        saved_max_connections = g_max_no_connections;
        saved_shutdown = g_mud_shutdown;
        saved_players = g_no_players;
        saved_max_players = g_max_no_players;
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
        g_no_players = saved_players;
        g_max_no_players = saved_max_players;
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
    int saved_players{0};
    int saved_max_players{0};
    std::unique_ptr<descriptor_data> desc;
    std::unique_ptr<pc_data> pc;
};

using events_t = std::vector<std::string>;
using link_e = pc_data::game_link_e;
using leave_e = pc_data::leave_e;

BOOST_FIXTURE_TEST_CASE(pair_test, SessionLogFixture)
{
    g_no_players = 3;
    BOOST_TEST((pc->getGameLink() == link_e::out));

    // No connection (an admin 'load'): not counted, nothing logged
    pc->connect_game(false);
    BOOST_TEST((pc->getGameLink() == link_e::out));
    BOOST_TEST(g_no_players == 3);

    connect();
    pc->connect_game(true);
    BOOST_TEST((pc->getGameLink() == link_e::connected));
    BOOST_TEST(g_no_players == 4);
    pc->disconnect_game(leave_e::link_lost);
    BOOST_TEST((pc->getGameLink() == link_e::link_dead));
    BOOST_TEST(g_no_players == 3);
    pc->disconnect_game(leave_e::link_lost); // already link dead
    BOOST_TEST((pc->getGameLink() == link_e::link_dead));
    BOOST_TEST(g_no_players == 3);
    pc->connect_game(false);
    BOOST_TEST(g_no_players == 4);
    pc->connect_game(false); // thrown out: the new connection takes over
    BOOST_TEST((pc->getGameLink() == link_e::connected));
    BOOST_TEST(g_no_players == 4);
    pc->disconnect_game(leave_e::left);
    BOOST_TEST((pc->getGameLink() == link_e::out));
    BOOST_TEST(g_no_players == 3);
    pc->disconnect_game(leave_e::left); // already out
    pc->disconnect_game(leave_e::link_lost);
    BOOST_TEST((pc->getGameLink() == link_e::out));
    BOOST_TEST(g_no_players == 3);
    BOOST_TEST((events() == events_t{"login", "linkdead", "reconnect", "reconnect", "logout"}));
}

BOOST_FIXTURE_TEST_CASE(connect_twice_counts_once_test, SessionLogFixture)
{
    g_no_players = 0;
    g_max_no_players = 0;
    connect();
    pc->connect_game(false);
    pc->connect_game(false);
    BOOST_TEST(g_no_players == 1);
    BOOST_TEST(g_max_no_players == 1);
    pc->disconnect_game(leave_e::left);
    pc->disconnect_game(leave_e::left);
    BOOST_TEST(g_no_players == 0);
    BOOST_TEST(g_max_no_players == 1);
    BOOST_TEST((events() == events_t{"login", "reconnect", "logout"}));
}

BOOST_FIXTURE_TEST_CASE(never_below_zero_test, SessionLogFixture)
{
    g_no_players = 0;
    pc->disconnect_game(leave_e::link_lost);
    pc->disconnect_game(leave_e::left);
    BOOST_TEST(g_no_players == 0);
    BOOST_TEST(events() == events_t{});
}

BOOST_FIXTURE_TEST_CASE(menu_round_trip_test, SessionLogFixture)
{
    // An immortal going to the menu and back: each time one count and a new session
    g_no_players = 0;
    connect();
    for (int i = 0; i < 3; i++)
    {
        pc->connect_game(false);
        BOOST_TEST(g_no_players == 1);
        pc->disconnect_game(leave_e::left);
        BOOST_TEST(g_no_players == 0);
    }
    BOOST_TEST((events() == events_t{"login", "logout", "login", "logout", "login", "logout"}));
}

BOOST_FIXTURE_TEST_CASE(link_dead_then_removed_test, SessionLogFixture)
{
    // Lost the link, then removed (link_dead@basis, purge): logout, counted down once
    g_no_players = 0;
    connect();
    pc->connect_game(false);
    pc->disconnect_game(leave_e::link_lost);
    UCHAR(pc.get())->setDescriptor(nullptr);
    pc->disconnect_game(leave_e::left);
    BOOST_TEST(g_no_players == 0);
    BOOST_TEST((pc->getGameLink() == link_e::out));
    BOOST_TEST((events() == events_t{"login", "linkdead", "logout"}));
}

BOOST_FIXTURE_TEST_CASE(fields_test, SessionLogFixture)
{
    connect();
    pc->setPlayerUID(42);
    pc->getPCTimeInformation().setTotalTimePlayedInSeconds(5400);
    pc->connect_game(false);
    pc->disconnect_game(leave_e::link_lost);
    hook.bWebsockets = 0;
    pc->connect_game(false);
    pc->disconnect_game(leave_e::left);
    const auto got = lines();
    BOOST_REQUIRE(got.size() == 4U);
    BOOST_TEST(got[0].find(R"("ev":"login","id":42,"lvl":0,"new":0,"web":1,"played":5400})") != std::string::npos);
    BOOST_TEST(got[1].find(R"("ev":"linkdead","id":42,"lvl":0})") != std::string::npos);
    BOOST_TEST(got[2].find(R"("ev":"reconnect","id":42,"lvl":0,"web":0})") != std::string::npos);
    // No played at logout (see sessionlog_logout())
    BOOST_TEST(got[3].find(R"("ev":"logout","id":42,"lvl":0})") != std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(new_player_field_test, SessionLogFixture)
{
    connect();
    pc->connect_game(true);
    const auto got = lines();
    BOOST_REQUIRE(got.size() == 1U);
    BOOST_TEST(got[0].find(R"("ev":"login")") != std::string::npos);
    BOOST_TEST(got[0].find(R"("new":1)") != std::string::npos);
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

BOOST_FIXTURE_TEST_CASE(no_linkdead_at_shutdown_test, SessionLogFixture)
{
    g_no_players = 0;
    connect();
    pc->connect_game(false);
    g_mud_shutdown = 1;
    pc->disconnect_game(leave_e::link_lost);
    sessionlog_shutdown();
    BOOST_TEST(g_no_players == 0);
    BOOST_TEST((events() == events_t{"login", "shutdown"}));
}

BOOST_FIXTURE_TEST_CASE(destructed_connect_test, SessionLogFixture)
{
    // A character extracted while entering the game is not counted or logged
    g_no_players = 0;
    connect();
    // register_destruct() appends the unit to the global destructed list: take that entry
    // out again afterwards, as the fixture deletes the unit
    pc->register_destruct();
    pc->connect_game(true);
    destructed_idx[DR_UNIT]--;
    pc->undo_destruct();
    BOOST_TEST((pc->getGameLink() == link_e::out));
    BOOST_TEST(g_no_players == 0);
    BOOST_TEST(events() == events_t{});
}

BOOST_AUTO_TEST_SUITE_END()
