#include "FixtureBase.h"
#include "OutputCapture.h"
#include "config.h"
#include "pcsave.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * PcSave_LoadPlayer_Suite - load_player() on a player file that does not load.
 * It must log "Corrupted player" and return nullptr; it used to give the nullptr to
 * assign_player_file_index() first, which crashed the server (the login prompt reaches
 * it with any name, before the password).
 */
BOOST_AUTO_TEST_SUITE(PcSave_LoadPlayer_Suite)

/**
 * Boots the server configuration on a new directory in /tmp, so the player files the
 * tests write are never in vme/lib/ply, and removes it afterwards.
 */
struct LoadPlayerFixture : public unit_tests::FixtureBase
{
    LoadPlayerFixture()
    {
        char tmpl[] = "/tmp/pcsave_tests.XXXXXX";
        BOOST_REQUIRE(mkdtemp(tmpl));
        dir = std::string(tmpl) + "/";
        for (const char *sub : {"lib", "etc", "log", "zone", "ply", "dil", "ply/b"})
        {
            std::filesystem::create_directory(dir + sub);
        }
        std::ofstream(dir + "server.cfg") << "libdir = ~" << dir << "lib/~\n"
                                          << "etcdir = ~" << dir << "etc/~\n"
                                          << "logdir = ~" << dir << "log/~\n"
                                          << "zondir = ~" << dir << "zone/~\n"
                                          << "plydir = ~" << dir << "ply/~\n"
                                          << "dil_file_dir = ~" << dir << "dil/~\n"
                                          << "mplex hosts = ~127.0.0.1~\n";
        g_cServerConfig.Boot(dir + "server.cfg");
        unit_tests::OutputCapture::Instance()->Clear();
    }

    ~LoadPlayerFixture() override { std::filesystem::remove_all(dir); }

    // A player file holding these bytes, as PlayerFileName() finds it
    void player_file(const std::string &name, const std::string &bytes) const
    {
        std::ofstream(dir + "ply/b/" + name, std::ios::binary) << bytes;
    }

    static bool logged(const std::string &message)
    {
        for (const auto &entry : unit_tests::OutputCapture::Instance()->getSLogDataMulti())
        {
            if (entry.m_message == message)
            {
                return true;
            }
        }
        return false;
    }

    std::string dir;
};

BOOST_FIXTURE_TEST_CASE(empty_file_is_corrupted, LoadPlayerFixture)
{
    player_file("bempty", "");
    BOOST_TEST(load_player("Bempty") == nullptr);
    BOOST_TEST(logged("Corrupted player Bempty."));
}

// The id is there, the length after it is cut short
BOOST_FIXTURE_TEST_CASE(file_cut_in_its_header_is_corrupted, LoadPlayerFixture)
{
    player_file("bcut", std::string("\x01\x00\x00\x00\x10\x00", 6));
    BOOST_TEST(load_player("Bcut") == nullptr);
    BOOST_TEST(logged("Corrupted player Bcut."));
}

BOOST_FIXTURE_TEST_CASE(missing_file_is_not_corrupted, LoadPlayerFixture)
{
    BOOST_TEST(load_player("Bnobody") == nullptr);
    BOOST_TEST(!logged("Corrupted player Bnobody."));
}

BOOST_AUTO_TEST_SUITE_END()
