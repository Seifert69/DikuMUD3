#include "db.h"
#include "file_index_type.h"
#include "zone_type.h"

#include <boost/test/unit_test.hpp>

#include <cstring>

/**********************************************************************************
 * zone_type_tests
 *
 * zone_type: the player zone's file indexes.
 */
BOOST_AUTO_TEST_SUITE(zone_type_tests)

// A player file may hold the name as it was typed at the login ("Papi"), and every lookup
// (find_file_index) is in lower case: the player zone keeps its keys in lower case.
BOOST_AUTO_TEST_CASE(player_file_index_ignores_case)
{
    zone_type players(g_player_zone);
    g_zone_info.mmp[g_player_zone] = &players;

    file_index_type *typed = players.findOrCreatePlayerFileIndex("Papi");
    file_index_type *lower = players.findOrCreatePlayerFileIndex("papi");
    BOOST_TEST(typed == lower);
    BOOST_TEST(std::strcmp(typed->getName(), "papi") == 0);
    BOOST_TEST(players.findFileIndex("papi") == typed);
    BOOST_TEST(players.getNumOfFileIndexes() == 1u);

    g_zone_info.mmp.erase(g_player_zone);
}

BOOST_AUTO_TEST_CASE(bonusband_defaults_to_not_applicable)
{
    zone_type town("town");
    BOOST_TEST(town.getBonusBand() == BONUS_NOT_APPLICABLE);
    town.setBonusBand(BONUS_EXCELLENT);
    BOOST_TEST(town.getBonusBand() == BONUS_EXCELLENT);
}

BOOST_AUTO_TEST_SUITE_END()
