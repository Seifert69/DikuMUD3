#include "szonelog_raw.h"

#include "config.h"
#include "files.h"
#include "formatter.h"
#include "slog.h"
#include "structs.h"
#include "zone_type.h"

void szonelog_raw(const zone_type *zone, const std::string &msg)
{
    if (zone == nullptr)
    {
        slog(LOG_ALL, 0, "%s", msg);
        return;
    }

    slog(LOG_ALL, 0, "%s/%s", zone->getName(), msg);

    auto filename = diku::format_to_str("%s%s.err", g_cServerConfig.getZoneDir(), zone->getFilename());

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

        fprintf(f, "%s :: %s\n", tmstr, msg.c_str());
    }
}
