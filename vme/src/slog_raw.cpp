#include "slog_raw.h"

#include "utility.h"

#include <exception>

// Per thread: a DIL shell() thread logging meanwhile is not quieted
static thread_local int g_nQuietLogs = 0;       // How many cQuietLog exist
static thread_local unsigned g_nQuietLines = 0; // Lines not written while one did (wraps)

cQuietLog::cQuietLog()
    : m_start(g_nQuietLines)
{
    g_nQuietLogs++;
}

cQuietLog::~cQuietLog()
{
    g_nQuietLogs--;
}

unsigned cQuietLog::Count() const
{
    return g_nQuietLines - m_start;
}

void slog_raw(log_level level, ubit8 wizinv_level, const std::string &msg)
{
    // A LOG_OFF line is always written: error() logs at LOG_OFF, then terminates, and
    // nearly all other LOG_OFF lines are boot or network messages, or come right
    // before an assert.
    if (g_nQuietLogs > 0 && level != LOG_OFF)
    {
        g_nQuietLines++;
        return;
    }

    time_t now = time(nullptr);
    std::string tmstr = ctime(&now);
    tmstr.erase(tmstr.length() - 1);

    /* 5 == " :: \n";  24 == tmstr (Tue Sep 20 18:41:23 1994) */
    static uint32_t log_size = 0;
    log_size += msg.length() + 5 + 24;

    if (log_size > 80'000'000) /* 40 meg is indeed a very big logfile! */
    {
        fprintf(g_log_file_fd, "Log-file insanely big!  Going down.\n");
        std::terminate(); // Dont use error, it calls syslog!!! *grin*
    }

    fprintf(g_log_file_fd, "%s :: %s\n", tmstr.c_str(), msg.c_str());
    fflush(g_log_file_fd);

    static uint8_t idx = 0;
    if (level > LOG_OFF)
    {
        g_log_buf[idx].setLevel(level);
        g_log_buf[idx].setWizInvLevel(wizinv_level);
        g_log_buf[idx].setString(msg);

        idx++;
        idx %= MAXLOG; /* idx = 1 .. MAXLOG-1 */

        g_log_buf[idx].clearString();
    }
}
