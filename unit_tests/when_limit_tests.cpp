#include "when_limit.h"

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * when_limit_tests
 *
 * cWhenLimit: "when" lookups that read a player file, one per 5 seconds for each
 * player and 5 in any one second for all players together.
 */
BOOST_AUTO_TEST_SUITE(when_limit_tests)

using namespace std::chrono_literals;
using clock_type = cWhenLimit::clock;

BOOST_AUTO_TEST_CASE(one_lookup_per_wait_for_each_key)
{
    cWhenLimit limit(5s, 5);
    const clock_type::time_point t0{};

    BOOST_TEST(limit.Allow(1, t0));
    BOOST_TEST(!limit.Allow(1, t0 + 4999ms));
    BOOST_TEST(limit.Allow(2, t0 + 4999ms)); // another player isn't held up
    BOOST_TEST(limit.Allow(1, t0 + 5s));
}

BOOST_AUTO_TEST_CASE(all_keys_together_get_per_second)
{
    cWhenLimit limit(5s, 5);
    const clock_type::time_point t0{};

    for (long key = 1; key <= 5; key++)
    {
        BOOST_TEST(limit.Allow(key, t0 + key * 100ms));
    }
    // A sixth player who hasn't asked before still waits for the second to pass
    BOOST_TEST(!limit.Allow(6, t0 + 600ms));
    BOOST_TEST(!limit.Allow(6, t0 + 1099ms));
    BOOST_TEST(limit.Allow(6, t0 + 1100ms)); // the first lookup is a second old
}

// A fixed window starting at the first lookup let 5 in at its end and 5 more just
// after it; the last second always holds at most 5
BOOST_AUTO_TEST_CASE(no_burst_across_a_second)
{
    cWhenLimit limit(5s, 5);
    const clock_type::time_point t0{};

    BOOST_TEST(limit.Allow(1, t0));
    for (long key = 2; key <= 5; key++)
    {
        BOOST_TEST(limit.Allow(key, t0 + 990ms));
    }
    BOOST_TEST(limit.Allow(6, t0 + 1s)); // the first one has left the last second
    for (long key = 7; key <= 10; key++)
    {
        BOOST_TEST(!limit.Allow(key, t0 + 1010ms));
    }
}

// A lookup refused by the limit for everyone doesn't count as the player's lookup
BOOST_AUTO_TEST_CASE(refused_lookup_does_not_start_the_wait)
{
    cWhenLimit limit(5s, 1);
    const clock_type::time_point t0{};

    BOOST_TEST(limit.Allow(1, t0));
    BOOST_TEST(!limit.Allow(2, t0 + 500ms));
    BOOST_TEST(limit.Allow(2, t0 + 1s));
}

BOOST_AUTO_TEST_CASE(old_entries_are_dropped_and_still_limit_nothing)
{
    cWhenLimit limit(5s, 2000);
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

BOOST_AUTO_TEST_SUITE_END()
