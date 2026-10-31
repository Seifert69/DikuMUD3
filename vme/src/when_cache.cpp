#include "when_cache.h"

#include "utils.h"

cWhenCache &when_cache()
{
    static cWhenCache cache;
    return cache;
}

cWhenEntry when_entry_of(unit_data *pc)
{
    cWhenEntry entry;
    const char *name = pc->getNames().Name();

    entry.last = PC_TIME(pc).getPlayerLastConnectTime();
    entry.found = !str_is_empty(name) && entry.last != 0;
    entry.name = entry.found ? name : "";
    entry.wizinv = pc->getLevelOfWizardInvisibility();
    entry.invisible = IS_SET(pc->getUnitFlags(), UNIT_FL_INVISIBLE);
    return entry;
}

void when_player_saved(unit_data *pc)
{
    // Most saves are of players no one asked about
    if (when_cache().Find(PC_FILENAME(pc)))
    {
        when_cache().Saved(PC_FILENAME(pc), when_entry_of(pc));
    }
}

void when_player_deleted(const char *name)
{
    when_cache().Deleted(name);
}
