#pragma once

#include <cstdio>

namespace unit_tests
{
struct FixtureBase
{
    FixtureBase();
    virtual ~FixtureBase();

private:
    FILE *m_saved_log_file_fd; ///< g_log_file_fd before the fixture, put back after it
};
} // namespace unit_tests
