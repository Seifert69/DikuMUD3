#pragma once

#include "slog.h"

#define HERE __FILE__, __LINE__

/**
 * Public call to error(). Because this uses boost::format internally std::string
 * can be formatted with %s specifier and user defined types can also be printed
 * see boost page for more: https://www.boost.org/doc/libs/1_78_0/libs/format/doc/format.html#user-defined
 *
 * usage:
 *    error(HERE, "Bad id: %d", id);
 * or
 *    error(HERE, "Something went wrong: %s %d %f", buf, number, 4.12);
 *
 * @tparam ParamPack Type of parameter pack of the rest of the arguments to original call
 * @param file Source file name
 * @param line Source file line number
 * @param fmt Printf style format string
 * @param pack Parameter pack of all the arguments, just like ellipsis ... from C
 */
template<typename... ParamPack>
void error(const char *file, int line, const std::string &fmt, ParamPack &&...pack)
{
    /**
     * MS: Moved szonelog to handler.c to make this module independent.
     * Replacing slog/assert(FALSE)
     *
     */
    std::string str;
    if constexpr (sizeof...(pack) != 0)
    {
        str = diku::format_to_str(fmt.c_str(), pack...);
    }
    else
    {
        str = fmt;
    }
    slog(LOG_OFF, 0, "%s:%d: %s", file, line, str);
    std::terminate();
}
