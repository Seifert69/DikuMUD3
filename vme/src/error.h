#pragma once

#include "slog.h"

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
void error(boost::format &format, T &&arg)
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
void error(boost::format &format, T &&arg, ParamPack &&...rest_args)
{
    format % arg;
    error(format, rest_args...);
}

/**
 * For external use:
 *
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
        boost::format format(fmt);
        error(format, pack...);
        str = format.str();
    }
    else
    {
        str = fmt;
    }
    slog(LOG_OFF, 0, "%s:%d: %s", file, line, str);
    std::terminate();
}
