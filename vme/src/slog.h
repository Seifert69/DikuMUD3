#pragma once

#include "formatter.h"
#include "utility.h"

#include <boost/format.hpp>

/**
 * Goto last function - that is the one you want to look at
 */

/**
 * For internal use:
 * Users should not use this - it is used by below template. Templates have to be
 * defined before use so unfortunately this has to be here.
 *
 * This is used to end the recursive template calls for the last parameter passed
 *
 * @tparam T Type of the argument to log
 * @param format Boost format
 * @param arg The value of last argument
 */
template<typename T>
void slog(boost::format &format, T &&arg)
{
    format % arg;
}

/**
 * For internal use:
 * Users should not use this - it is used by below template.  Templates have to be
 * defined before use so unfortunately this has to be here.
 *
 * This is used to recursively pull 1 parameter from the param pack and format it
 * then call itself or the above template if its the last parameter.
 *
 * @tparam T Type of the argument to log
 * @tparam ParamPack Type of parameter pack of the rest of the arguments to original call
 * @param format Boost format
 * @param arg The argument value to format
 * @param rest_args Parameter pack of the rest of arguments
 */
template<typename T, typename... ParamPack>
void slog(boost::format &format, T &&arg, ParamPack &&...rest_args)
{
    format % arg;
    slog(format, rest_args...);
}

/**
 * For external use:
 *
 * Public call to error(). Because this uses boost::format internally std::string
 * can be formatted with %s specifier and user defined types can also be printed
 * see boost page for more: https://www.boost.org/doc/libs/1_78_0/libs/format/doc/format.html#user-defined
 *
 * @tparam ParamPack Type of parameter pack of the rest of the arguments to original call
 * @param level log_level enum
 * @param wizinv_level
 * @param fmt Printf style format string
 * @param pack Parameter pack of all the arguments, just like ellipsis ... from C
 */
template<typename... ParamPack>
void slog(log_level level, ubit8 wizinv_level, const std::string &fmt, ParamPack &&...pack)
{
    std::string formatted_text;
    if constexpr (sizeof...(pack) != 0)
    {
        boost::format format(fmt);
        slog(format, pack...);
        formatted_text = format.str();
    }
    else
    {
        formatted_text = fmt;
    }

    std::string buf;
    time_t now = time(nullptr);
    std::string tmstr = ctime(&now);
    tmstr.erase(tmstr.length() - 1);

    if (wizinv_level > 0)
    {
        buf = diku::format_to_str("(%d) ", wizinv_level);
    }
    buf += formatted_text;

    /* 5 == " :: \n";  24 == tmstr (Tue Sep 20 18:41:23 1994) */
    static uint32_t log_size = 0;
    log_size += buf.length() + 5 + 24;

    if (log_size > 40'000'000) /* 40 meg is indeed a very big logfile! */
    {
        fprintf(g_log_file_fd, "Log-file insanely big!  Going down.\n");
        std::terminate(); // Dont use error, it calls syslog!!! *grin*
    }

    fprintf(g_log_file_fd, "%s :: %s\n", tmstr.c_str(), buf.c_str());
    fflush(g_log_file_fd);

    static uint8_t idx = 0;
    if (level > LOG_OFF)
    {
        g_log_buf[idx].setLevel(level);
        g_log_buf[idx].setWizInvLevel(wizinv_level);
        g_log_buf[idx].setString(buf);

        idx++;
        idx %= MAXLOG; /* idx = 1 .. MAXLOG-1 */

        g_log_buf[idx].clearString();
    }
}
