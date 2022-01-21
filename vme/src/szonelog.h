#pragma once

#include "config.h"
#include "files.h"
#include "formatter.h"
#include "slog.h"
#include "structs.h"

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
        boost::format formatter(fmt);
        diku::format(formatter, pack...);
        formatted_text = formatter.str();
    }
    else
    {
        formatted_text = fmt;
    }

    if (zone == nullptr)
    {
        slog(LOG_ALL, 0, "%s", formatted_text);
        return;
    }

    slog(LOG_ALL, 0, "%s/%s", zone->name, formatted_text);

    auto filename = diku::format_to_str("%s%s.err", g_cServerConfig.getZoneDir(), zone->filename);

    FILE *f = fopen_cache(filename, "a");
    if (f == nullptr)
    {
        slog(LOG_ALL, 0, "Unable to append to zonelog '%s'", filename);
    }
    else
    {
        time_t now = time(nullptr);
        char *tmstr = ctime(&now);

        tmstr[strlen(tmstr) - 1] = '\0';

        fprintf(f, "%s :: %s\n", tmstr, formatted_text.c_str());
    }
}
