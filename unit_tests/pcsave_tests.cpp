#include "FixtureBase.h"
#include "OutputCapture.h"
#include "config.h"
#include "db_file.h"
#include "handler.h"
#include "interpreter.h"
#include "pc_data.h"
#include "pcsave.h"
#include "reception.h"
#include "spec_assign.h"
#include "unit_fptr.h"
#include "utils.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>

#include <boost/test/unit_test.hpp>

/**
 * Boots the server configuration on a new directory in /tmp, so the player files the
 * tests write are never in vme/lib/ply, and removes it afterwards.
 */
struct PlayerDirFixture : public unit_tests::FixtureBase
{
    PlayerDirFixture()
    {
        char tmpl[] = "/tmp/pcsave_tests.XXXXXX";
        BOOST_REQUIRE(mkdtemp(tmpl));
        dir = std::string(tmpl) + "/";
        // ply/a for the inventory save's temporary file, aaa-inv.tmp
        for (const char *sub : {"lib", "etc", "log", "zone", "ply", "dil", "ply/a", "ply/b"})
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

    ~PlayerDirFixture() override { std::filesystem::remove_all(dir); }

    // A player file holding these bytes, as PlayerFileName() finds it
    void player_file(const std::string &name, const std::string &bytes) const
    {
        std::ofstream(dir + "ply/b/" + name, std::ios::binary) << bytes;
    }

    // What a player file (or with ".inv", its inventory) holds
    std::string player_file(const std::string &name) const
    {
        std::ifstream in(dir + "ply/b/" + name, std::ios::binary);
        return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
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

/**********************************************************************************
 * PcSave_LoadPlayer_Suite - load_player() on a player file that does not load.
 * It must log "Corrupted player" and return nullptr; it used to give the nullptr to
 * assign_player_file_index() first, which crashed the server (the login prompt reaches
 * it with any name, before the password).
 */
BOOST_AUTO_TEST_SUITE(PcSave_LoadPlayer_Suite)

BOOST_FIXTURE_TEST_CASE(empty_file_is_corrupted, PlayerDirFixture)
{
    player_file("bempty", "");
    BOOST_TEST(load_player("Bempty") == nullptr);
    BOOST_TEST(logged("Corrupted player Bempty."));
}

// The id is there, the length after it is cut short
BOOST_FIXTURE_TEST_CASE(file_cut_in_its_header_is_corrupted, PlayerDirFixture)
{
    player_file("bcut", std::string("\x01\x00\x00\x00\x10\x00", 6));
    BOOST_TEST(load_player("Bcut") == nullptr);
    BOOST_TEST(logged("Corrupted player Bcut."));
}

BOOST_FIXTURE_TEST_CASE(missing_file_is_not_corrupted, PlayerDirFixture)
{
    BOOST_TEST(load_player("Bnobody") == nullptr);
    BOOST_TEST(!logged("Corrupted player Bnobody."));
}

BOOST_AUTO_TEST_SUITE_END()

/**********************************************************************************
 * PcSave_SaveHandler_Suite - what a save handler leaves in g_FileBuffer is not saved.
 * A save runs the SFB_SAVE handlers, whose DIL can fill g_FileBuffer: a load() of a unit
 * reads its zone data there, and loading a player file reads that file there. The save
 * used to clear the buffer before the handlers and write it after them, so their bytes
 * were saved as the player or the inventory. Each case saves twice, the second time
 * with a handler that fills the buffer, and the two files must be the same.
 */
BOOST_AUTO_TEST_SUITE(PcSave_SaveHandler_Suite)

const char *const FOREIGN = "the bytes of some other unit";
bool g_fill_on_save = false;

// Stands in for DIL that loads a unit while it is told of the save
int fill_file_buffer(spec_arg *sarg)
{
    if (g_fill_on_save && sarg->cmd == &g_cmd_auto_save)
    {
        g_FileBuffer.Clear();
        g_FileBuffer.AppendString(FOREIGN);
    }
    return SFR_SHARE;
}

/**
 * A player, Bsaved, and fill_file_buffer() as an unused special that is never saved
 * (index 2, SD_NEVER), so the handler is not part of what is saved. The real special
 * is put back afterwards.
 */
struct SaveHandlerFixture : public PlayerDirFixture
{
    static constexpr ubit16 SPECIAL = 2;

    SaveHandlerFixture()
    {
        BOOST_REQUIRE(g_unit_function_array[SPECIAL].save_w_d == SD_NEVER);
        g_unit_function_array[SPECIAL].func = fill_file_buffer;
        g_fill_on_save = false;
        pc->setFilename("bsaved");
        pc->getNames().AppendName("Bsaved");
        pc->setPlayerUID(1);
    }

    ~SaveHandlerFixture() override
    {
        for (unit_data *u : {static_cast<unit_data *>(pc.get()), obj.get()})
        {
            delete u->getFunctionPointer();
            u->setFunctionPointer(nullptr);
        }
        g_unit_function_array[SPECIAL].func = real;
    }

    // Gives u the handler; it must be told of a save that u itself starts, too (SFB_AWARE)
    static void handler_on(unit_data *u)
    {
        auto *fptr = new unit_fptr;
        fptr->setFunctionPointerIndex(SPECIAL);
        fptr->setAllActivateOnEventFlags(SFB_SAVE | SFB_AWARE);
        u->setFunctionPointer(fptr);
    }

    int (*real)(spec_arg *) = g_unit_function_array[SPECIAL].func;
    std::unique_ptr<pc_data> pc{dynamic_cast<pc_data *>(new_unit_data(UNIT_ST_PC, nullptr))};
    std::unique_ptr<unit_data> obj{new_unit_data(UNIT_ST_OBJ, nullptr)};
};

// save_player_disk() keeps the path of its temporary file from its first call, so only
// this case saves a player: another one would have a new directory
BOOST_FIXTURE_TEST_CASE(player_file_is_the_player, SaveHandlerFixture)
{
    handler_on(pc.get());

    save_player_file(pc.get()); // the first save also counts a second played
    save_player_file(pc.get());
    const std::string clean = player_file("bsaved");

    g_fill_on_save = true;
    save_player_file(pc.get());
    const std::string saved = player_file("bsaved");

    BOOST_TEST(saved.find(FOREIGN) == std::string::npos);
    BOOST_TEST(saved == clean);
}

BOOST_FIXTURE_TEST_CASE(inventory_file_is_the_inventory, SaveHandlerFixture)
{
    obj->getNames().AppendName("stone");
    handler_on(obj.get());
    unit_to_unit(obj.get(), pc.get());

    save_contents("bsaved", pc.get(), TRUE, FALSE);
    const std::string clean = player_file("bsaved.inv");

    g_fill_on_save = true;
    save_contents("bsaved", pc.get(), TRUE, FALSE);
    const std::string saved = player_file("bsaved.inv");

    unit_from_unit(obj.get());
    BOOST_TEST(!clean.empty());
    BOOST_TEST(saved.find(FOREIGN) == std::string::npos);
    BOOST_TEST(saved == clean);
}

BOOST_AUTO_TEST_SUITE_END()
