#pragma once

#include <boost/format.hpp>

/**
 * Goto last function - that is the one you want to look at
 *
 * For more information on variadic templates Eli Bendersky
 * has a good page on them:
 *
 * https://eli.thegreenplace.net/2014/variadic-templates-in-c/
 */
namespace diku
{

/**
 * For internal use:
 * Users should not use this - it is used by below template. Templates have to be
 * defined before use so unfortunately this has to be here.
 *
 * This is used to end the recursive template calls for the last parameter passed
 * @tparam T Type of the argument to log
 * @param formatter Boost format
 * @param last_value The value of last argument
 */
template<typename T>
void format(boost::format &formatter, T &&last_value)
{
    formatter % last_value;
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
 * @param formatter Boost format
 * @param first_arg The argument value to format
 * @param rest_args Parameter pack of the rest of arguments
 */
template<typename T, typename... ParamPack>
void format(boost::format &formatter, T &&first_arg, ParamPack &&...rest_args)
{
    formatter % first_arg;
    format(formatter, rest_args...);
}

/**
 * For external use:
 *
 * Because this uses boost::format internally std::string
 * can be formatted with %s specifier and user defined types can also be printed
 * see boost page for more: https://www.boost.org/doc/libs/1_78_0/libs/format/doc/format.html#user-defined
 *
 * @tparam ParamPack Type of parameter pack of the rest of the arguments to original call
 * @param fmt_str Printf style format string
 * @param args Parameter pack of all the arguments, just like ellipsis ... from C
 * @return A std::string with the format specifiers replaced with the args values
 */
template<typename... ParamPack>
std::string format_to_str(const char *fmt_str, ParamPack &&...args)
{
    static_assert(sizeof...(args) != 0, "No arguments passed to format in diku::format_to_str");

    boost::format formatter(fmt_str);
    format(formatter, args...);
    return formatter.str();
}

} // namespace diku
