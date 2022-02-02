#pragma once

#include "OutputCapture.h"
#include "formatter.h"
#include "slog_raw.h"
#include "utility.h"

#include <boost/format.hpp>

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
        formatted_text = diku::format_to_str(fmt.c_str(), pack...);
    }
    else
    {
        formatted_text = fmt;
    }

    // If a unit test is being run, send results there too
    unit_tests::OutputCapture::slog(level, wizinv_level, fmt, formatted_text);

    std::string buf;

    if (wizinv_level > 0)
    {
        buf = diku::format_to_str("(%d) ", wizinv_level);
    }
    buf += formatted_text;

    slog_raw(level, wizinv_level, buf);
}
