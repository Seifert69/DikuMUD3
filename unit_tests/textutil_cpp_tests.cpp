#include "textutil.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Textutil_Suite)

static const time_t now = 2000000000;
static const time_t minute = 60;
static const time_t hour = 60 * minute;
static const time_t day = 24 * hour;
static const time_t year = 365 * day;

BOOST_AUTO_TEST_CASE(time_ago_under_a_minute)
{
    BOOST_TEST(time_ago_string(now, now) == "less than a minute ago");
    BOOST_TEST(time_ago_string(now - 59, now) == "less than a minute ago");
    // A time in the future (clock changed) doesn't give a negative age
    BOOST_TEST(time_ago_string(now + hour, now) == "less than a minute ago");
}

BOOST_AUTO_TEST_CASE(time_ago_singular_and_plural)
{
    BOOST_TEST(time_ago_string(now - minute, now) == "1 minute ago");
    BOOST_TEST(time_ago_string(now - 2 * minute, now) == "2 minutes ago");
    BOOST_TEST(time_ago_string(now - hour, now) == "1 hour ago");
    BOOST_TEST(time_ago_string(now - day, now) == "1 day ago");
    BOOST_TEST(time_ago_string(now - year, now) == "1 year ago");
}

BOOST_AUTO_TEST_CASE(time_ago_two_largest_units)
{
    BOOST_TEST(time_ago_string(now - (3 * day + 4 * hour + 5 * minute), now) == "3 days, 4 hours ago");
    BOOST_TEST(time_ago_string(now - (hour + 30 * minute + 59), now) == "1 hour, 30 minutes ago");
    BOOST_TEST(time_ago_string(now - (2 * year + 10 * day), now) == "2 years, 10 days ago");
}

// The second unit is left out when it is zero, rather than skipping down to a smaller one
BOOST_AUTO_TEST_CASE(time_ago_zero_second_unit_left_out)
{
    BOOST_TEST(time_ago_string(now - (2 * day + 5 * minute), now) == "2 days ago");
    BOOST_TEST(time_ago_string(now - (year + 3 * hour), now) == "1 year ago");
}

// Large gaps still give two units
BOOST_AUTO_TEST_CASE(time_ago_long_ago)
{
    BOOST_TEST(time_ago_string(0, now) == "63 years, 153 days ago");
}

BOOST_AUTO_TEST_SUITE_END()
