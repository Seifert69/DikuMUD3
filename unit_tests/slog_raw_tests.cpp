#include "FixtureBase.h"
#include "slog_raw.h"
#include "utility.h"

#include <cstdio>
#include <thread>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * slog_raw_tests
 *
 * cQuietLog: while one exists, log lines are counted instead of written ("when"
 * reading a corrupt player file, which could be asked about again and again).
 * A LOG_OFF line is always written: error() logs at LOG_OFF, then terminates.
 */
BOOST_AUTO_TEST_SUITE(slog_raw_tests)

BOOST_AUTO_TEST_CASE(quiet_log_counts_the_lines_not_written)
{
    cQuietLog quiet;
    BOOST_TEST(quiet.Count() == 0u);
    slog_raw(LOG_ALL, 0, "not written");
    slog_raw(LOG_BRIEF, 0, "nor this");
    BOOST_TEST(quiet.Count() == 2u);
}

// "when" can run inside something else that is quiet: each counts its own lines,
// and the outer one stays quiet after the inner one goes
BOOST_AUTO_TEST_CASE(quiet_logs_nest)
{
    cQuietLog outer;
    slog_raw(LOG_ALL, 0, "one");
    {
        cQuietLog inner;
        slog_raw(LOG_ALL, 0, "two");
        BOOST_TEST(inner.Count() == 1u);
    }
    slog_raw(LOG_ALL, 0, "three");
    BOOST_TEST(outer.Count() == 3u);
}

BOOST_AUTO_TEST_CASE(a_new_quiet_log_starts_at_zero)
{
    {
        cQuietLog first;
        slog_raw(LOG_ALL, 0, "counted by the first");
    }
    cQuietLog second;
    BOOST_TEST(second.Count() == 0u);
}

static long log_bytes()
{
    fflush(g_log_file_fd);
    return ftell(g_log_file_fd);
}

// Nothing reaches the log file while quiet, and writing starts again after
BOOST_FIXTURE_TEST_CASE(quiet_log_writes_nothing_until_it_goes, unit_tests::FixtureBase)
{
    {
        cQuietLog quiet;
        slog_raw(LOG_ALL, 0, "not written");
        BOOST_TEST(log_bytes() == 0);
    }
    slog_raw(LOG_ALL, 0, "written");
    BOOST_TEST(log_bytes() > 0);
}

// As error() logs before the server stops: the reason must be in the log
BOOST_FIXTURE_TEST_CASE(quiet_log_writes_a_log_off_line, unit_tests::FixtureBase)
{
    cQuietLog quiet;
    slog_raw(LOG_OFF, 0, "fatal");
    BOOST_TEST(log_bytes() > 0);
    BOOST_TEST(quiet.Count() == 0u);
}

// Only the thread that is quiet: a DIL shell() thread logging meanwhile is written
BOOST_FIXTURE_TEST_CASE(quiet_log_is_per_thread, unit_tests::FixtureBase)
{
    cQuietLog quiet;
    std::thread other([] { slog_raw(LOG_ALL, 0, "from another thread"); });
    other.join();
    BOOST_TEST(log_bytes() > 0);
    BOOST_TEST(quiet.Count() == 0u);
}

BOOST_AUTO_TEST_SUITE_END()
