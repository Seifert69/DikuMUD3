#define BOOST_TEST_MODULE "CServerConfiguration Unit Tests"
#include "FixtureBase.h"
#include "config.h"
#include "db.h"
#include "diku_exception.h"

#include <cstdarg>
#include <cstring>
#include <fstream>

#include <boost/test/unit_test.hpp>

struct CServerConfiguration_Fixture : public unit_tests::FixtureBase
{
    static const std::string server_config;
    CServerConfiguration_Fixture()
        : FixtureBase()
    {
        // No way to suppress the warning from tempnam because its hardcoded into lib
        // so we'll do it here it's only a unit test
        srand(time(nullptr));
        std::ostringstream sstrm;
        sstrm << "tmp" << std::setfill('0') << std::setw(10) << rand();
        fake_server_config_filename = sstrm.str();

        std::ofstream strm(fake_server_config_filename);
        strm << server_config;
    }
    ~CServerConfiguration_Fixture() { remove(fake_server_config_filename.c_str()); }
    std::string fake_server_config_filename;
};

// Attach fixture to suite so each test case gets a new copy
BOOST_FIXTURE_TEST_SUITE(CServerConfiguration_tests, CServerConfiguration_Fixture)

BOOST_AUTO_TEST_CASE(default_ctor_test)
{
    CServerConfiguration config;

    BOOST_TEST(config.getMotherPort() == 4999);
    BOOST_TEST(config.getRentModifier() == 10);
    BOOST_TEST(config.isAccounting() == false);
    BOOST_TEST(config.isAliasShout() == true);
    BOOST_TEST(config.isBBS() == false);
    BOOST_TEST(config.isLawful() == false);
    BOOST_TEST(config.isNoSpecials() == false);
    BOOST_TEST(config.isBOB() == false);
    BOOST_TEST(config.getShout() == 1);
    BOOST_TEST(config.getReboot() == 0);
    {
        in_addr empty{0};
        BOOST_TEST(config.getSubnetMask().s_addr == empty.s_addr);
    }
    {
        in_addr empty{0};
        BOOST_TEST(config.getLocalhost().s_addr == empty.s_addr);
    }
    {
        in_addr empty{0};
        for (auto i = 0U; i < CServerConfiguration::MAX_MPLEX_HOSTS; ++i)
        {
            BOOST_TEST(config.getMplexHost(i).s_addr == empty.s_addr);
        }
    }
    BOOST_TEST(config.getPromptString().empty());
    BOOST_TEST(config.getLibDir().empty());
    BOOST_TEST(config.getPlyDir().empty());
    BOOST_TEST(config.getEtcDir().empty());
    BOOST_TEST(config.getLogDir().empty());
    BOOST_TEST(config.getZoneDir().empty());
    BOOST_TEST(config.getDILFileDir().empty());
    BOOST_TEST(config.getMudName().empty());
    BOOST_TEST(config.getLogo().empty());
    BOOST_TEST(config.getColorString().empty());
    BOOST_TEST(config.getImmortalName().empty());
}

/**
 * Test the Boot() method
 */
BOOST_AUTO_TEST_CASE(Boot_test)
{
    CServerConfiguration config;
    config.Boot(fake_server_config_filename);

    BOOST_TEST(config.getMotherPort() == 4999);
    BOOST_TEST(config.getRentModifier() == 0);
    BOOST_TEST(config.isAccounting() == false);
    BOOST_TEST(config.isAliasShout() == false);
    BOOST_TEST(config.isBBS() == false);
    BOOST_TEST(config.isLawful() == false);
    BOOST_TEST(config.isNoSpecials() == false);
    BOOST_TEST(config.isBOB() == true);
    BOOST_TEST(config.getShout() == 0);
    BOOST_TEST(config.getReboot() == 0);
    {
        in_addr empty{UINT32_MAX};
        BOOST_TEST(config.getSubnetMask().s_addr == empty.s_addr);
    }
    {
        in_addr empty{0};
        BOOST_TEST(config.getLocalhost().s_addr == empty.s_addr);
    }
    {
        std::vector<in_addr> expected{
            {16777343},  // 127.0.0.1
            {352823488}, // 192.168.7.21
            {352823488}, // The last IP gets copied to the rest of the slots
            {352823488},
            {352823488},
            {352823488},
            {352823488},
            {352823488},
            {352823488},
            {352823488},
        };

        size_t i = 0;
        for (auto host : config.getMplexHosts())
        {
            BOOST_TEST(host.s_addr == expected[i].s_addr,
                       "index " << i << ": mplex:" << host.s_addr << " = " << expected[i].s_addr << ":expected");
            ++i;
        }
    }
    BOOST_TEST(config.getPromptString() == "%mana%m/%e%e/%hp%h> ");
    BOOST_TEST(config.getLibDir() == "../lib/");
    BOOST_TEST(config.getPlyDir() == "../lib/ply/");
    BOOST_TEST(config.getEtcDir() == "../etc/");
    BOOST_TEST(config.getLogDir() == "../log/");
    BOOST_TEST(config.getZoneDir() == "../zone/");
    BOOST_TEST(config.getDILFileDir() == "../lib/dilfiles/");
    BOOST_TEST(config.getMudName() == "DikuMUD III Git Vanilla");

    {
        // Don't like doing it this way TODO Update to pass in logo file
        std::ifstream in(config.getFileInEtcDir(LOGO_FILE), std::ios_base::binary);
        std::string expected{(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()};
        BOOST_TEST(config.getLogo() == expected);
    }

    {
        // Don't like doing it this way TODO Update to pass in color file
        std::string filename{config.getEtcDir()};
        filename += COLOR_FILE;
        std::ifstream in(filename, std::ios_base::binary);
        std::string expected{(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()};
        BOOST_TEST(config.getColorString() == expected);
    }
    BOOST_TEST(config.getImmortalName() == "Papi");
}

BOOST_AUTO_TEST_CASE(Boot_no_file_test)
{
    CServerConfiguration config;
    char bad_file[] = "Bet this file doesn't exist!";

    BOOST_CHECK_THROW(config.Boot(bad_file), diku_exception);
}

/**
 * Test FromLAN() Method
 */
BOOST_AUTO_TEST_CASE(FromLAN_test)
{
    CServerConfiguration config;
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
    config.Boot(fake_server_config_filename);

    sockaddr_in ip{};
    ip.sin_addr.s_addr = 134744072; // 8.8.8.8
    BOOST_TEST(config.ValidMplex(&ip) == 0);

    ip.sin_addr.s_addr = 16777343; // 127.0.0.1
    BOOST_TEST(config.ValidMplex(&ip) == 1);

    ip.sin_addr.s_addr = 352823488; // 192.168.7.21
    BOOST_TEST(config.ValidMplex(&ip) == 1);
}

BOOST_FIXTURE_TEST_CASE(get_file_in_dir_throw_tests, CServerConfiguration_Fixture)
{
    CServerConfiguration config;
    config.Boot(fake_server_config_filename);

    BOOST_CHECK_THROW(config.getFileInEtcDir(nullptr), diku_exception);
    BOOST_CHECK_THROW(config.getFileInLibDir(nullptr), diku_exception);
    BOOST_CHECK_THROW(config.getFileInLogDir(nullptr), diku_exception);
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
