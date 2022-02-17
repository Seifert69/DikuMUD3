#include "zone_type.h"

#include "dil.h"
#include "file_index_type.h"
#include "structs.h"
#include "zone_reset_cmd.h"

zone_type::zone_type()
{
    name = nullptr;
    notes = nullptr;
    help = nullptr;
    filename = nullptr;
    rooms = nullptr;
    objects = nullptr;
    npcs = nullptr;

    zri = nullptr;

    spmatrix = nullptr;
    no_rooms = 0;
    no_objs = 0;
    no_npcs = 0;
    access = 255;
}

zone_type::~zone_type()
{
    if (name)
        FREE(name);

    if (title)
        FREE(title);

    if (notes)
        FREE(notes);

    if (help)
        FREE(help);

    if (filename)
        FREE(filename);
    unit_data *ut = nullptr;
    unit_data *nextut = nullptr;

    for (ut = objects; ut; ut = nextut)
    {
        nextut = ut->next;
        ut->next = nullptr;
        ut->gnext = nullptr;
        ut->gprevious = nullptr;
        delete ut;
    }

    for (ut = npcs; ut; ut = nextut)
    {
        nextut = ut->next;
        ut->next = nullptr;
        ut->gnext = nullptr;
        ut->gprevious = nullptr;
        delete ut;
    }

    auto nextfi = mmp_fi.begin();
    for (auto p = mmp_fi.begin(); p != mmp_fi.end(); p = nextfi)
    {
        nextfi = p;
        nextfi++;

        delete p->second;
    }

    zone_reset_cmd *pzri = nullptr;
    zone_reset_cmd *nextzri = nullptr;

    for (pzri = zri; pzri; pzri = nextzri)
    {
        nextzri = pzri->getNext();
        FREE(pzri);
    }

    auto nextpt = mmp_tmpl.begin();

    for (auto pt = mmp_tmpl.begin(); pt != mmp_tmpl.end(); pt = nextpt)
    {
        nextpt = pt;
        nextpt++;

        if (pt->second->prgname)
            FREE(pt->second->prgname);
        if (pt->second->argt)
            FREE(pt->second->argt);
        if (pt->second->core)
            FREE(pt->second->core);
        if (pt->second->vart)
            FREE(pt->second->vart);

        FREE(pt->second);
    }

    // struct bin_search_type *ba;    /* Pointer to binarray of type      */
    // struct diltemplate *tmpl;      /* DIL templates in zone            */
    // struct bin_search_type *tmplba;/* Pointer to binarray of type      */
    if (spmatrix)
        FREE(spmatrix);
}

cNamelist &zone_type::getCreators()
{
    return creators;
}

const char *zone_type::getName() const
{
    return name;
}

char *zone_type::getNamePtr()
{
    return name;
}

void zone_type::setName(char *value)
{
    FREE(name);
    name = value;
}

const char *zone_type::getTitle() const
{
    return title;
}

char **zone_type::getTitlePtrPtr()
{
    return &title;
}

void zone_type::setTitle(char *value)
{
    FREE(title);
    title = value;
}

const char *zone_type::getNotes() const
{
    return notes;
}

char **zone_type::getNotesPtrPtr()
{
    return &notes;
}

void zone_type::setNotes(char *value)
{
    FREE(notes);
    notes = value;
}

char **zone_type::getHelpPtrPtr()
{
    return &help;
}

void zone_type::setHelp(char *value)
{
    FREE(help);
    help = value;
}

const char *zone_type::getFilename() const
{
    return filename;
}

char **zone_type::getFilenamePtrPtr()
{
    return &filename;
}

void zone_type::setFilename(char *value)
{
    FREE(filename);
    filename = value;
}

const std::map<const char *, file_index_type *, cmp_str> &zone_type::cgetFileIndexMap() const
{
    return mmp_fi;
}

std::map<const char *, file_index_type *, cmp_str> &zone_type::getFileIndexMap()
{
    return mmp_fi;
}

const zone_reset_cmd *zone_type::cgetZoneResetCommands() const
{
    return zri;
}

zone_reset_cmd *zone_type::getZoneResetCommands()
{
    return zri;
}

void zone_type::setZoneResetCommands(zone_reset_cmd *value)
{
    FREE(zri);
    zri = value;
}

const std::map<const char *, diltemplate *, cmp_str> &zone_type::cgetDILTemplate() const
{
    return mmp_tmpl;
}

std::map<const char *, diltemplate *, cmp_str> &zone_type::getDILTemplate()
{
    return mmp_tmpl;
}

void zone_type::setZoneNumber(ubit16 value)
{
    zone_no = value;
}

ubit16 zone_type::getNumOfFileIndexes() const
{
    return no_of_fi;
}

void zone_type::incrementNumOfFileIndexes()
{
    ++no_of_fi;
}

void zone_type::setNumOfFileIndexes(ubit16 value)
{
    no_of_fi = value;
}

ubit16 zone_type::getZoneResetTime() const
{
    return zone_time;
}

void zone_type::setZoneResetTime(ubit16 value)
{
    zone_time = value;
}

ubit16 *zone_type::getZoneResetTimePtr()
{
    return &zone_time;
}

ubit16 zone_type::getNumOfRooms() const
{
    return no_rooms;
}

void zone_type::incrementNumOfRooms()
{
    ++no_rooms;
}

void zone_type::setNumOfRooms(ubit16 value)
{
    no_rooms = value;
}

ubit16 zone_type::getNumOfObjects() const
{
    return no_objs;
}

void zone_type::incrementNumOfObjects()
{
    ++no_objs;
}

void zone_type::incrementNumOfNPCs()
{
    ++no_npcs;
}
ubit16 zone_type::getNumOfNPCs() const
{
    return no_npcs;
}

ubit8 zone_type::getResetMode() const
{
    return reset_mode;
}

ubit8 *zone_type::getResetModePtr()
{
    return &reset_mode;
}

void zone_type::setResetMode(ubit8 value)
{
    reset_mode = value;
}

void zone_type::incrementNumOfDILTemplates()
{
    ++no_tmpl;
}

void zone_type::setNumOfDILTemplates(ubit16 value)
{
    no_tmpl = value;
}

ubit8 zone_type::getAccessLevel() const
{
    return access;
}

void zone_type::setAccessLevel(ubit8 value)
{
    access = value;
}

ubit8 zone_type::getLevelRequiredToLoadItems() const
{
    return loadlevel;
}

ubit8 *zone_type::getLevelRequiredToLoadItemsPtr()
{
    return &loadlevel;
}

void zone_type::setLevelRequiredToLoadItems(ubit8 value)
{
    loadlevel = value;
}

ubit8 zone_type::getPayOnly() const
{
    return payonly;
}

ubit8 *zone_type::getPayOnlyPtr()
{
    return &payonly;
}

void zone_type::setPayOnly(ubit8 value)
{
    payonly = value;
}

const char *zone_type::getDILFilePath() const
{
    return dilfilepath;
}

void zone_type::setDILFilePath(char *value)
{
    FREE(dilfilepath);
    dilfilepath = value;
}

const zone_type::Weather &zone_type::cgetWeather() const
{
    return weather;
}

zone_type::Weather &zone_type::getWeather()
{
    return weather;
}
