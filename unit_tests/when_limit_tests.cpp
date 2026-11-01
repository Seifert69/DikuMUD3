#include "when_limit.h"

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * when_limit_tests
 *
 * cWhenLimit: "when" lookups that read a player file, one per 5 seconds for each
 * player.
 */
BOOST_AUTO_TEST_SUITE(when_limit_tests)

using namespace std::chrono_literals;
using clock_type = cWhenLimit::clock;

BOOST_AUTO_TEST_CASE(one_lookup_per_wait_for_each_key)
{
    cWhenLimit limit(5s);
    const clock_type::time_point t0{};

    BOOST_TEST(limit.Allow(1, t0));
    BOOST_TEST(!limit.Allow(1, t0 + 4999ms));
    BOOST_TEST(limit.Allow(2, t0 + 4999ms)); // another player isn't held up
    BOOST_TEST(limit.Allow(1, t0 + 5s));
}

BOOST_AUTO_TEST_CASE(many_keys_may_ask_again_after_the_wait)
{
    cWhenLimit limit(5s);
    const clock_type::time_point t0{};

    for (long key = 0; key < 1500; key++)
    {
        BOOST_TEST(limit.Allow(key, t0 + key * 1ms));
    }
    // Long after: every one of them may ask again
    for (long key = 0; key < 1500; key++)
    {
        BOOST_TEST(limit.Allow(key, t0 + 10s + key * 1ms));
    }
}

// Over 1000 keys, those whose wait is over are forgotten; the others are kept,
// and still wait
BOOST_AUTO_TEST_CASE(over_1000_keys_old_ones_are_dropped_recent_ones_kept)
{
    cWhenLimit limit(5s);
    const clock_type::time_point t0{};

    for (long key = 0; key < 600; key++)
    {
        BOOST_TEST(limit.Allow(key, t0));
    }
    for (long key = 600; key < 1001; key++)
    {
        BOOST_TEST(limit.Allow(key, t0 + 5s));
    }
    BOOST_TEST(limit.Size() == 1001u);

    // At 6 s the first 600 have waited long enough, the other 401 haven't
    BOOST_TEST(limit.Allow(5000, t0 + 6s));
    BOOST_TEST(limit.Size() == 402u);
    BOOST_TEST(!limit.Allow(1000, t0 + 6s));
    BOOST_TEST(limit.Allow(0, t0 + 6s));
}

// At 1000 keys or fewer nothing is dropped
BOOST_AUTO_TEST_CASE(up_to_1000_keys_nothing_is_dropped)
{
    cWhenLimit limit(5s);
    const clock_type::time_point t0{};

    for (long key = 0; key < 1000; key++)
    {
        BOOST_TEST(limit.Allow(key, t0));
    }
    BOOST_TEST(limit.Allow(5000, t0 + 10s));
    BOOST_TEST(limit.Size() == 1001u);
}

BOOST_AUTO_TEST_SUITE_END()
