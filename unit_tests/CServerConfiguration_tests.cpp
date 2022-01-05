#define BOOST_TEST_MODULE "CServerConfiguration Unit Tests"
#include <boost/test/unit_test.hpp>
#include <cstring>
#include <fstream>
#include <cstdarg>
#include "config.h"
#include "db.h"

struct CServerConfiguration_Fixture
{
    static CServerConfiguration_Fixture *m_fixptr;
    static const std::string server_config;
    CServerConfiguration_Fixture()
    {
        m_fixptr = this; // has to be first

        // Get temp filename for config file for tests
        std::tmpnam(fake_server_config_filename);
        std::ofstream strm(fake_server_config_filename);
        strm << server_config;
    }

    struct slog
    {
        int m_level{};
        int m_wiz_inv_level{};
        std::string m_fmt{};
        std::string m_message{};
        void compare(int level, int wiz_inv_level, const std::string &fmt, const std::string &message)
        {
            BOOST_TEST(m_level == level);
            BOOST_TEST(m_wiz_inv_level == wiz_inv_level);
            BOOST_TEST(m_fmt == fmt);
            BOOST_TEST(m_message == message);
        }
    } slog;

    char fake_server_config_filename[L_tmpnam]{};
    bool exit_called{false};
};

// Make the current Fixture available to slog et al hijacks
CServerConfiguration_Fixture *CServerConfiguration_Fixture::m_fixptr = nullptr;

// Hijack this function for the unit test
void slog(enum log_level level, ubit8 wizinv_level, const char *fmt, ...)
{
    auto &slog = CServerConfiguration_Fixture::m_fixptr->slog;
    slog.m_level = (int)level;
    slog.m_wiz_inv_level = wizinv_level;
    slog.m_fmt = fmt;

    va_list args;
    va_start(args, fmt);
    char buf[1024 * 1024];
    vsnprintf(buf, sizeof(buf), fmt, args);
    slog.m_message = buf;
    va_end(args);
}

struct hijack_exit_exception
{
};
// Hijack lib exit()
void exit(int)
{
    CServerConfiguration_Fixture::m_fixptr->exit_called = true;
    throw hijack_exit_exception();
}

// Attach fixture to suite so each test case gets a new copy
BOOST_FIXTURE_TEST_SUITE(CServerConfiguration_tests, CServerConfiguration_Fixture)

BOOST_AUTO_TEST_CASE(default_ctor_test)
{
    CServerConfiguration config;

    BOOST_TEST(config.m_nMotherPort == 4999);
    BOOST_TEST(config.m_nRentModifier == 10);
    BOOST_TEST(config.m_bAccounting == 0);
    BOOST_TEST(config.m_bAliasShout == TRUE);
    BOOST_TEST(config.m_bBBS == FALSE);
    BOOST_TEST(config.m_bLawful == FALSE);
    BOOST_TEST(config.m_bNoSpecials == FALSE);
    BOOST_TEST(config.m_bBOB == FALSE);
    BOOST_TEST(config.m_nShout == 1);
    BOOST_TEST(config.m_hReboot == 0);
    // Compile test that color member exists, it has its own unit tests
    (void)config.color;
    {
        auto empty = decltype(CServerConfiguration::m_sSubnetMask){0};
        BOOST_TEST(memcmp(&config.m_sSubnetMask, &empty, sizeof(config.m_sSubnetMask)) == 0);
    }
    {
        auto empty = decltype(CServerConfiguration::m_sLocalhost){0};
        BOOST_TEST(memcmp(&config.m_sLocalhost, &empty, sizeof(config.m_sLocalhost)) == 0);
    }
    {
        auto empty = std::remove_reference<decltype(*CServerConfiguration::m_aMplexHosts)>::type{0};
        auto array_size = sizeof(config.m_aMplexHosts) / sizeof(config.m_aMplexHosts[0]);
        for (auto i = 0; i < array_size; ++i)
        {
            BOOST_TEST(memcmp(&config.m_aMplexHosts[i], &empty, sizeof(config.m_aMplexHosts[0])) == 0);
        }
    }
    (void)config.m_promptstr;
    BOOST_TEST_WARN(false, "m_promptstr is not set in default ctor");
    BOOST_TEST(config.m_libdir == nullptr);
    BOOST_TEST(config.m_plydir == nullptr);
    BOOST_TEST(config.m_etcdir == nullptr);
    BOOST_TEST(config.m_logdir == nullptr);
    BOOST_TEST(config.m_zondir == nullptr);
    (void)config.m_dilfiledir;
    BOOST_TEST_WARN(false, "m_dilfiledir is not set in default ctor");
    BOOST_TEST(config.m_mudname == nullptr);
    BOOST_TEST(config.m_pLogo == nullptr);

    (void)config.m_pColor;
    BOOST_TEST_WARN(false, "m_pColor is not set in default ctor");

    BOOST_TEST(config.m_pImmortName == nullptr);
}

/**
 * Test the Boot() method
 */
BOOST_AUTO_TEST_CASE(Boot_test)
{
    CServerConfiguration config;
    config.Boot(fake_server_config_filename);

    BOOST_TEST(config.m_nMotherPort == 4999);
    BOOST_TEST(config.m_nRentModifier == 0);
    BOOST_TEST(config.m_bAccounting == 0);
    BOOST_TEST(config.m_bAliasShout == 0);
    BOOST_TEST(config.m_bBBS == FALSE);
    BOOST_TEST(config.m_bLawful == FALSE);
    BOOST_TEST(config.m_bNoSpecials == FALSE);
    BOOST_TEST(config.m_bBOB == 1);
    BOOST_TEST(config.m_nShout == 0);
    BOOST_TEST(config.m_hReboot == 0);
    // Compile test that color member exists, it has its own unit tests
    (void)config.color;
    {
        auto expected = decltype(CServerConfiguration::m_sSubnetMask){UINT32_MAX};
        BOOST_TEST(config.m_sSubnetMask.s_addr == expected.s_addr);
    }
    {
        auto expected = decltype(CServerConfiguration::m_sLocalhost){0};
        BOOST_TEST(memcmp(&config.m_sLocalhost, &expected, sizeof(config.m_sLocalhost)) == 0);
    }
    {
        constexpr auto array_size = sizeof(config.m_aMplexHosts) / sizeof(config.m_aMplexHosts[0]);
        std::remove_reference<decltype(*CServerConfiguration::m_aMplexHosts)>::type expected[array_size]{
            16777343,  // 127.0.0.1
            352823488, // 192.168.7.21
            352823488, // This looks weird - its like the last IP gets copied to the rest of the slots
            352823488,
            352823488,
            352823488,
            352823488,
            352823488,
            352823488,
            352823488,
        };

        for (auto i = 0; i < array_size; ++i)
        {
            BOOST_TEST(config.m_aMplexHosts[i].s_addr == expected[i].s_addr);
        }
    }
    BOOST_TEST(config.m_promptstr);
    BOOST_TEST(std::string(config.m_promptstr) == "%mana%m/%e%e/%hp%h> ");
    BOOST_TEST(config.m_libdir);
    BOOST_TEST(std::string(config.m_libdir) == "../lib/");
    BOOST_TEST(config.m_plydir);
    BOOST_TEST(std::string(config.m_plydir) == "../lib/ply/");
    BOOST_TEST(config.m_etcdir);
    BOOST_TEST(std::string(config.m_etcdir) == "../etc/");
    BOOST_TEST(config.m_logdir);
    BOOST_TEST(std::string(config.m_logdir) == "../log/");
    BOOST_TEST(config.m_zondir);
    BOOST_TEST(std::string(config.m_zondir) == "../zone/");
    BOOST_TEST(config.m_dilfiledir);
    BOOST_TEST(std::string(config.m_dilfiledir) == "../lib/dilfiles/");
    BOOST_TEST(config.m_mudname);
    BOOST_TEST(std::string(config.m_mudname) == "DikuMUD III Git Vanilla");

    {
        BOOST_TEST(config.m_pLogo);
        // Don't like doing it this way TODO Update to pass in logo file
        std::string filename{config.m_etcdir};
        filename += LOGO_FILE;
        std::ifstream in(filename, std::ios_base::binary);
        std::string expected{(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()};
        BOOST_TEST(std::string(config.m_pLogo) == expected);
    }

    {
        BOOST_TEST(config.m_pColor);
        // Don't like doing it this way TODO Update to pass in color file
        std::string filename{config.m_etcdir};
        filename += COLOR_FILE;
        std::ifstream in(filename, std::ios_base::binary);
        std::string expected{(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()};
        BOOST_TEST(std::string(config.m_pColor) == expected);
    }
    BOOST_TEST(config.m_pImmortName);
    BOOST_TEST(std::string(config.m_pImmortName) == "Papi");

    remove(fake_server_config_filename);
}

// TODO I don't like this - it needs to be done better after refactor
// BOOST_AUTO_TEST_CASE(Boot_no_file_test)
//{
//    CServerConfiguration config;
//    char bad_file[] = "Bet this file doesn't exist!";
//
//    // While Boot doesn't throw - our hijacked exit() does
//    BOOST_CHECK_THROW(config.Boot(bad_file), hijack_exit_exception);
//
//    slog.compare(LOG_ALL,
//                 0,
//                 "Could not find server configuration file. %s",
//                 "Could not find server configuration file. Bet this file doesn't exist!");
//    BOOST_TEST(exit_called);
//}

/**
 * Test FromLAN() Method
 */
BOOST_AUTO_TEST_CASE(FromLAN_test)
{
    CServerConfiguration config;
    BOOST_TEST_WARN(false, "m_pColor isn't set in Ctor - and I'm writing tests not fixing bugs so hack it here only!!");
    config.m_pColor = nullptr;
    config.Boot(fake_server_config_filename);

    char bad_input[] = "No way this is a dotted IP";
    BOOST_TEST(config.FromLAN(bad_input) == 0);

    char good_input[] = "8.8.8.8";
    BOOST_TEST(config.FromLAN(good_input) == 0);
}

/**
 * Test ValidMplex() Method
 */
BOOST_FIXTURE_TEST_CASE(ValidMplex, CServerConfiguration_Fixture)
{
    CServerConfiguration config;
    BOOST_TEST_WARN(false, "m_pColor isn't set in Ctor - and I'm writing tests not fixing bugs so hack it here only!!");
    config.m_pColor = nullptr;
    config.Boot(fake_server_config_filename);

    sockaddr_in ip{};
    ip.sin_addr.s_addr = 134744072; // 8.8.8.8
    BOOST_TEST(config.ValidMplex(&ip) == 0);

    ip.sin_addr.s_addr = 16777343; // 127.0.0.1
    BOOST_TEST(config.ValidMplex(&ip) == 1);

    ip.sin_addr.s_addr = 352823488; // 192.168.7.21
    BOOST_TEST(config.ValidMplex(&ip) == 1);
}
BOOST_AUTO_TEST_SUITE_END()

const std::string CServerConfiguration_Fixture::server_config = R"(
#
# The server configuration file
#
# This file is line order sensitive. Do not move around the order of the
# configuration lines. Comments and blank lines can be inserted and remove
# at your leisure.
#

#
# This is the name of the mud, it will appear when the players login,
# ie (1) Enter The VME server
#

mudname = ~DikuMUD III Git Vanilla~
immortal_name = ~Papi~
#
# This is the location of the lib, etc, log, and zone directories
#

libdir = ~../lib/~
etcdir = ~../etc/~
logdir = ~../log/~
zondir = ~../zone/~
plydir = ~../lib/ply/~
dil_file_dir = ~../lib/dilfiles/~

#
# This is the mother port used by the server.
#
#
Port = 4999


#
# The hour of the day at which an automatic reboot will occur.
# 0 disables, 1 - 24 are the hours in military time.  This will reboot
# the mud closely to the hour as possible.

auto_reboot_hour = 0
#
# The rent field determines how much rent is charged. 0 means disabeled,
# 100 means the same amount as specified in the zone files. Any number
# in between is a percentage of the rent specified per object.
#
rent = 0

#
# The Book of Blood. Use "1" to let the server support the Book of
# Blood, use 0 if not. If you choose not to use the Book of Blood
# you must remember to remove the book from the zones as well as
# check that there is no BOB DIL code hanging around to disturb you.
# (Easilly done with a grep after PC_PK_RELAXED and "$BOB").
#
bob = 1

#
# When Alias Shout equals 0, then players are not allowed to make
# shout statements in alias'es. When it equals 1 players can make
# shouts in aliases.
#
Alias Shout = 0

#
# Mainly for debugging. If set to one, no special routines will be activated.
# You should keep this at zero.
#
No Specials = 0

#
# This is an oldie. Don't set to anything but zero. Used on obsure systems
# where you have to shutdown at high loads, etc. etc.
#
Lawful = 0

#
# When BBS is set to TRUE to server will assume that the game is operating
# on a BBS system.
#
# BBS use assumes that all players
# connect via some terminal program instead of telnet. This has to do with
# colour & hidden characters.
#
BBS = 0

#
# Use 0 to disable accounting. All game play is free. Use 1 to enable
# accounting.
#
accounting = 0

#
# Here you can set at which level a newbie can shout, I've set it to
# level 2 for now...
#
shout = 0

#
# The subnetmask and localhost ip numbers are used to specify which Internet
# addresses are from the LAN. If you are not on the Internet, please omit
# these or set them to "255.255.255.255" and "0.0.0.0" respectively.
#
# If you are on the Internet, any connection which is from the LAN will
# bypass the regular accounting system and have all such players play
# for free. If you do not wish this to happen, set the addresses as
# shown above.
#

subnetmask = ~255.255.255.255~
localhost  = ~0.0.0.0~

#
# The multiplexors can theoretically connect from any host (and that's
# intentional). To avoid hacking attempts, here you specify a list of
# different hosts that are trusted as "multiplexor" safe.
#
# More are specified like usual namelists, e.g. ~127.0.0.1~ ~0.0.0.0~
#
mplex hosts = ~127.0.0.1~ ~192.168.7.21~

#
# Color specifications.
#
# Some of these are interpreted as complete strings to be shown directly,
# and some are merely "prefixes" like for example the color room title.
#
#
#

promptstr = ~%mana%m/%e%e/%hp%h> ~
diag_prompt = ~healthy;bruised;scraped;bleeding;gushing~

########################################################################
#
#  Startup script variables only past here
#
#  The VME server does not read anything below this, these are used
#  by the startvme script
#
#  WARNING: The ~ is not a valid in path names
#
########################################################################

# Turn on the startup script debuging if > 0
debug = 0

#  Define as many multiplexors as you need (up to 16).
#    mplex[ < Port # > ]=" <Options> "
#  There are several options that can be passed to a multiplexor
#    -c = color enabled port
#    -?

mplex[4242] = "-c"
mplex[5242]
mplex[4280] = "-w"

###################
#  Logging options
###################

# Number of log files to keep on a cyclic basis
log_num_keep = 8

# Define a compress program to compress the logfiles and any options
#
#   COMMENT OUT compress_program and compres_extention
#             TO DISABLE LOG COMPRESSION
#
compress_program = /usr/bin/gzip

# The extention the compression program will add to a filename ie .gz
compress_extention = .gz

############################
#  Compiler script variables
############################

# VME Home or root install Directory
vmehome = /mnt/c/data/build/vme

# Full path to the vme zone directory
zonedir = /mnt/c/data/build/vme/zone

# Full path to the location that newly compiled zones wait for next reboot
nextzon = /mnt/c/data/build/vme/zone/nxtzon

#############################
#  External program control
#############################

#  Start the ftp compiler
#  0 = no    1 = yes
ftpcompiler = 0

#  Start the executer.perl
#  0 = no    1 = yes
)";
