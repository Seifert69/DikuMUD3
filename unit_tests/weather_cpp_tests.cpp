#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include "constants.h"
#include "npc_data.h"
#include "pc_data.h"
#include "weather.h"

#include <ctime>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

// Imported prototypes for testing - as the void version isn't easily testable
time_info_data mud_date(time_t t);

BOOST_TEST_DONT_PRINT_LOG_VALUE(time_info_data)

/**********************************************************************************
 * Weather_CPP_Suite
 */
BOOST_AUTO_TEST_SUITE(Weather_CPP_Suite)
/**
 * Some useful constants for tests
 */
time_t now = time(nullptr);
time_t hour_future = now + (60 * 60);
time_t day_future = now + (60 * 60 * 24);
time_t month_future = now + (60 * 60 * 24 * 30);
time_t year_future = now + (60 * 60 * 24 * 365);

/**********************************************************************************
 * date_defines_test
 */
BOOST_AUTO_TEST_CASE(date_defines_test)
{
    BOOST_TEST(SECS_PER_REAL_MIN == 60);
    BOOST_TEST(SECS_PER_REAL_HOUR == 3600);
    BOOST_TEST(SECS_PER_REAL_DAY == 86400);
    BOOST_TEST(SECS_PER_REAL_YEAR == 31536000);

    BOOST_TEST(MUD_DAY == 24);
    BOOST_TEST(MUD_WEEK == 7);
    BOOST_TEST(MUD_MONTH == 14);
    BOOST_TEST(MUD_YEAR == 9);

    BOOST_TEST(SECS_PER_MUD_HOUR == 300);
    BOOST_TEST(SECS_PER_MUD_DAY == 7200);
    BOOST_TEST(SECS_PER_MUD_MONTH == 100800);
    BOOST_TEST(SECS_PER_MUD_YEAR == 907200);

    BOOST_TEST(g_beginning_of_time == 650336715);
}

/**********************************************************************************
 * weekday_constants_tests
 */
auto weekdays_ds = boost::unit_test::data::make({
    "the Day of the Moon",
    "the Day of the Bull",
    "the Day of the Deception",
    "the Day of Thunder",
    "the Day of Freedom",
    "the Day of the Great Gods",
    "the Day of the Sun",
});
BOOST_DATA_TEST_CASE(weekday_constants_tests, boost::unit_test::data::xrange(MUD_WEEK) ^ weekdays_ds, index, name)
{
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(std::string(g_weekdays[index]) == name);
    ////////////////////////// Test Subject //////////////////////////////
}

/**********************************************************************************
 * months_constants_tests
 */
auto months_ds = boost::unit_test::data::make({
    "Month of Winter",
    "Month of the Winter Wolf",
    "Month of the Frost Giant",
    "Month of Spring",
    "Month of Futility",
    "Month of the Sun",
    "Month of the Heat",
    "Month of the Long Shadows",
    "Month of the Ancient Darkness",
});
BOOST_DATA_TEST_CASE(months_constants_tests, boost::unit_test::data::xrange(MUD_YEAR) ^ months_ds, index, name)
{
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(std::string(g_month_name[index]) == name);
    ////////////////////////// Test Subject //////////////////////////////
}

/**********************************************************************************
 * mud_time_passed_test
 */
auto to_mudtime_dataset = boost::unit_test::data::make({
    now,          // TEST _0
    now,          // TEST _1
    hour_future,  // TEST _2
    now,          // TEST _3
    day_future,   // TEST _4
    now,          // TEST _5
    month_future, // TEST _6
    now,          // TEST _7
    year_future,  // TEST _8
});
auto from_mudtime_dataset = boost::unit_test::data::make({
    now,          // TEST _0
    hour_future,  // TEST _1
    now,          // TEST _2
    day_future,   // TEST _3
    now,          // TEST _4
    month_future, // TEST _5
    now,          // TEST _6
    year_future,  // TEST _7
    now,          // TEST _8
});
auto expected_mudtime_dataset = boost::unit_test::data::make<time_info_data>({
    {0, 0, 0, 0},      // TEST _0
    {-12, 0, 0, 0},    // TEST _1
    {12, 0, 0, 0},     // TEST _2
    {0, -12, 0, 0},    // TEST _3
    {0, 12, 0, 0},     // TEST _4
    {0, -10, -7, -2},  // TEST _5
    {0, 10, 7, 2},     // TEST _6
    {0, -12, -6, -34}, // TEST _7
    {0, 12, 6, 34},    // TEST _8
});

BOOST_DATA_TEST_CASE(mud_time_passed_test, to_mudtime_dataset ^ from_mudtime_dataset ^ expected_mudtime_dataset, to, from, expected_result)
{
    ////////////////////////// Test Subject //////////////////////////////
    auto result = mud_time_passed(to, from);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(result == expected_result);
}

/**********************************************************************************
 * real_time_passed_test
 */
auto to_real_time_dataset = boost::unit_test::data::make({
    now,          // Test _0
    now,          // Test _1
    hour_future,  // Test _2
    now,          // Test _3
    day_future,   // Test _4
    now,          // Test _5
    month_future, // Test _6
    now,          // Test _7
    year_future,  // Test _8
});
auto from_real_time_dataset = boost::unit_test::data::make({
    now,          // Test _0
    hour_future,  // Test _1
    now,          // Test _2
    day_future,   // Test _3
    now,          // Test _4
    month_future, // Test _5
    now,          // Test _6
    year_future,  // Test _7
    now,          // Test _8
});
auto expected_real_time_dataset = boost::unit_test::data::make<time_info_data>({
    {0, 0, -1, 0},   // Test _0
    {-1, 0, -1, 0},  // Test _1
    {1, 0, -1, 0},   // Test _2
    {0, -1, -1, 0},  // Test _3
    {0, 1, -1, 0},   // Test _4
    {0, -30, -1, 0}, // Test _5
    {0, 30, -1, 0},  // Test _6
    {0, 0, -1, -1},  // Test _7
    {0, 0, -1, 1},   // Test _8
});

BOOST_DATA_TEST_CASE(real_time_passed_test,
                     to_real_time_dataset ^ from_real_time_dataset ^ expected_real_time_dataset,
                     to,
                     from,
                     expected_result)
{
    ////////////////////////// Test Subject //////////////////////////////
    auto result = real_time_passed(to, from);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(result == expected_result);
}

/**********************************************************************************
 * age_npc_test
 */
BOOST_AUTO_TEST_CASE(age_npc_test)
{
    auto unit = std::unique_ptr<npc_data>(dynamic_cast<npc_data *>(new_unit_data(UNIT_ST_NPC, nullptr)));

    ////////////////////////// Test Subject //////////////////////////////
    auto result = age(unit.get());
    ////////////////////////// Test Subject //////////////////////////////

    time_info_data expected(0, 0, 0, 0);
    BOOST_TEST(result == expected);
}

/**********************************************************************************
 * age_pc_test
 */
BOOST_AUTO_TEST_CASE(age_pc_test)
{
    auto age_time = time(nullptr) - 7654321;
    auto unit = std::unique_ptr<pc_data>(dynamic_cast<pc_data *>(new_unit_data(UNIT_ST_PC, nullptr)));
    unit->getPCTimeInformation().setPlayerBirthday(age_time);

    ////////////////////////// Test Subject //////////////////////////////
    auto result = age(unit.get());
    ////////////////////////// Test Subject //////////////////////////////

    time_info_data expected(2, 13, 3, 8);

    BOOST_TEST(result == expected);
}

/**********************************************************************************
 * mud_date_test
 */
BOOST_AUTO_TEST_CASE(mud_date_test)
{
    time_t test_date = 1640014705; // Mon Dec 20 08:38:25 2021

    ////////////////////////// Test Subject //////////////////////////////
    auto result = mud_date(test_date);
    ////////////////////////// Test Subject //////////////////////////////

    time_info_data expected(6, 3, 8, 1090);
    BOOST_TEST(result == expected);
}

BOOST_AUTO_TEST_SUITE_END()

/**********************************************************************************
 * Time_Ago_Suite - time_ago_string(), for "when"
 */
BOOST_AUTO_TEST_SUITE(Time_Ago_Suite)

const time_t then_base = 2000000000;
const time_t minute = 60;
const time_t hour = 60 * minute;
const time_t day = 24 * hour;
const time_t year = 365 * day;

BOOST_AUTO_TEST_CASE(time_ago_under_a_minute)
{
    BOOST_TEST(time_ago_string(then_base, then_base) == "less than a minute ago");
    BOOST_TEST(time_ago_string(then_base - 59, then_base) == "less than a minute ago");
    // A time in the future (clock set back) doesn't give a negative age
    BOOST_TEST(time_ago_string(then_base + hour, then_base) == "less than a minute ago");
}

BOOST_AUTO_TEST_CASE(time_ago_singular_and_plural)
{
    BOOST_TEST(time_ago_string(then_base - minute, then_base) == "1 minute ago");
    BOOST_TEST(time_ago_string(then_base - 2 * minute, then_base) == "2 minutes ago");
    BOOST_TEST(time_ago_string(then_base - hour, then_base) == "1 hour ago");
    BOOST_TEST(time_ago_string(then_base - day, then_base) == "1 day ago");
    BOOST_TEST(time_ago_string(then_base - year, then_base) == "1 year ago");
}

BOOST_AUTO_TEST_CASE(time_ago_two_largest_units)
{
    BOOST_TEST(time_ago_string(then_base - (3 * day + 4 * hour + 5 * minute), then_base) == "3 days, 4 hours ago");
    BOOST_TEST(time_ago_string(then_base - (hour + 30 * minute + 59), then_base) == "1 hour, 30 minutes ago");
    BOOST_TEST(time_ago_string(then_base - (2 * year + 10 * day), then_base) == "2 years, 10 days ago");
    BOOST_TEST(time_ago_string(then_base - (23 * hour + 59 * minute), then_base) == "23 hours, 59 minutes ago");
    BOOST_TEST(time_ago_string(then_base - (364 * day + 23 * hour), then_base) == "364 days, 23 hours ago");
}

// The second unit is left out when it is zero, rather than skipping down to a smaller one
BOOST_AUTO_TEST_CASE(time_ago_zero_second_unit_left_out)
{
    BOOST_TEST(time_ago_string(then_base - (2 * day + 5 * minute), then_base) == "2 days ago");
    BOOST_TEST(time_ago_string(then_base - (year + 3 * hour), then_base) == "1 year ago");
}

// Large gaps still give two units: a player who last played in 1970
BOOST_AUTO_TEST_CASE(time_ago_long_ago)
{
    BOOST_TEST(time_ago_string(0, then_base) == "63 years, 153 days ago");
}

BOOST_AUTO_TEST_SUITE_END()

#pragma clang diagnostic pop
