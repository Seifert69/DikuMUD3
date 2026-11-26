#include "FixtureBase.h"
#include "common.h"

#include <map>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * The builders' BONUS_ values as numbers: bonus_map_a() (quality and magic) and
 * bonus_map_b() (transfers) in common.cpp. A named item's bonus rolls a whole number between
 * the band below's most (not included) and its own band's most, so it keeps its band and
 * varies within it; until 2026-11 each band had one number, now and then 2 (1) more, so an
 * artifact could be +22.
 */
BOOST_AUTO_TEST_SUITE(Bonus_Map_Suite)

const int ROLLS = 2000;

// Every roll of bonus is in lo..hi, and each of those numbers comes up
static void rolls_cover(int (*map)(int), int bonus, int lo, int hi)
{
    std::map<int, int> seen;
    for (int i = 0; i < ROLLS; i++)
    {
        const int v = map(bonus);
        BOOST_TEST((lo <= v && v <= hi), "bonus " << bonus << " gave " << v << ", not " << lo << ".." << hi);
        seen[v]++;
    }
    for (int v = lo; v <= hi; v++)
    {
        BOOST_TEST(seen[v] > 0, "bonus " << bonus << " never gave " << v);
    }
}

BOOST_AUTO_TEST_CASE(quality_rolls_within_its_band)
{
    rolls_cover(bonus_map_a, 1, 1, 2);
    rolls_cover(bonus_map_a, 2, 3, 5);
    rolls_cover(bonus_map_a, 3, 6, 8);
    rolls_cover(bonus_map_a, 4, 9, 11);
    rolls_cover(bonus_map_a, 5, 12, 14);
    rolls_cover(bonus_map_a, 6, 15, 17);
    rolls_cover(bonus_map_a, 7, 18, 20); // BONUS_ARTIFACT: never above 20
}

BOOST_AUTO_TEST_CASE(transfers_roll_within_their_band)
{
    rolls_cover(bonus_map_b, 1, 1, 1);
    rolls_cover(bonus_map_b, 2, 2, 2);
    rolls_cover(bonus_map_b, 3, 3, 4);
    rolls_cover(bonus_map_b, 4, 5, 5);
    rolls_cover(bonus_map_b, 5, 6, 6);
    rolls_cover(bonus_map_b, 6, 7, 8);
    rolls_cover(bonus_map_b, 7, 9, 10);
}

BOOST_AUTO_TEST_CASE(minus_bands_mirror_and_average_is_zero)
{
    rolls_cover(bonus_map_a, -7, -20, -18);
    rolls_cover(bonus_map_a, -1, -2, -1);
    rolls_cover(bonus_map_b, -7, -10, -9);
    rolls_cover(bonus_map_b, -3, -4, -3);
    rolls_cover(bonus_map_a, 0, 0, 0);
    rolls_cover(bonus_map_b, 0, 0, 0);
}

// Out of the scale it is the nearest end of it (and logged, so the log is captured)
BOOST_FIXTURE_TEST_CASE(beyond_artifact_is_artifact, unit_tests::FixtureBase)
{
    rolls_cover(bonus_map_a, 9, 18, 20);
    rolls_cover(bonus_map_b, -12, -10, -9);
}

BOOST_AUTO_TEST_SUITE_END()
