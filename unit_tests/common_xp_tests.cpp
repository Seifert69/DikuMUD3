#include "common.h"

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * The experience ladder: required_xp() and level_xp() in common.cpp
 */
BOOST_AUTO_TEST_SUITE(Experience_Ladder_Suite)

BOOST_AUTO_TEST_CASE(levels_1_to_10_unchanged)
{
    // 300 * level + 200 * level * level, as before 2026-11
    BOOST_TEST(required_xp(0) == 0);
    BOOST_TEST(required_xp(1) == 500);
    BOOST_TEST(required_xp(2) == 1400);
    BOOST_TEST(required_xp(10) == 23000);
    BOOST_TEST(level_xp(1) == 900);
    BOOST_TEST(level_xp(9) == 4100);
}

BOOST_AUTO_TEST_CASE(levels_10_to_72_cost_2000_plus_250_a_level)
{
    for (int level = 10; level < 73; level++)
    {
        BOOST_TEST(level_xp(level) == 2000 + 250 * level, "level " << level);
    }
    BOOST_TEST(level_xp(10) == 4500); // the same as the old ladder at level 10
    BOOST_TEST(level_xp(49) == 14250);
    BOOST_TEST(required_xp(50) == 398000);
}

BOOST_AUTO_TEST_CASE(flat_from_level_73)
{
    BOOST_TEST(level_xp(72) == 20000);
    BOOST_TEST(level_xp(73) == 20100);
    BOOST_TEST(level_xp(500) == 20100);
    BOOST_TEST(required_xp(74) == required_xp(73) + 20100);
    BOOST_TEST(required_xp(3000) == required_xp(73) + 20100 * (3000 - 73));
}

BOOST_AUTO_TEST_CASE(every_level_costs_more_or_the_same)
{
    for (int level = 1; level < 5000; level++)
    {
        BOOST_TEST(level_xp(level) >= level_xp(level - 1), "level " << level);
        BOOST_TEST(required_xp(level + 1) > required_xp(level), "level " << level);
    }
}

BOOST_AUTO_TEST_SUITE_END()
