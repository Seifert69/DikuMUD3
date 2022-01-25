#pragma once

#include "formatter.h"
#include "slog.h"
#include "structs.h"
#include "szonelog_raw.h"

#include <boost/format.hpp>

/**
 * For external use:
 *
 * Public call to error(). Because this uses boost::format internally std::string
 * can be formatted with %s specifier and user defined types can also be printed
 * see boost page for more: https://www.boost.org/doc/libs/1_78_0/libs/format/doc/format.html#user-defined
 *
 * @tparam ParamPack Type of parameter pack of the rest of the arguments to original call
 * @param zone Zone to log message for
 * @param fmt Printf style format string
 * @param pack Parameter pack of all the arguments, just like ellipsis ... from C
 */
template<typename... ParamPack>
void szonelog(zone_type *zone, const char *fmt, ParamPack &&...pack)
{
    std::string formatted_text;
    if constexpr (sizeof...(pack) != 0)
    {
        formatted_text = diku::format_to_str(fmt, pack...);
    }
    else
    {
        formatted_text = fmt;
    }

    szonelog_raw(zone, formatted_text);
}
