#include "FixtureBase.h"

#include "OutputCapture.h"
#include "utility.h"

namespace unit_tests
{
FixtureBase::FixtureBase()
{
    OutputCapture::EnableOutputCapture();

    // Redirect away application logging to tmp file
    g_log_file_fd = tmpfile();
}

FixtureBase::~FixtureBase()
{
    fclose(g_log_file_fd);
    OutputCapture::Instance()->Clear();
}

} // namespace unit_tests
