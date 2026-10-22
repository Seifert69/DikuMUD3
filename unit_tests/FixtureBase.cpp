#include "FixtureBase.h"

#include "OutputCapture.h"
#include "utility.h"

namespace unit_tests
{
FixtureBase::FixtureBase()
    : m_saved_log_file_fd(g_log_file_fd)
{
    OutputCapture::EnableOutputCapture();

    // Redirect away application logging to tmp file
    g_log_file_fd = tmpfile();
}

FixtureBase::~FixtureBase()
{
    // Put the log file back: tests after this one that log (slog) wrote into the
    // closed FILE, corrupting the heap
    fclose(g_log_file_fd);
    g_log_file_fd = m_saved_log_file_fd;
    OutputCapture::Instance()->Clear();
}

} // namespace unit_tests
