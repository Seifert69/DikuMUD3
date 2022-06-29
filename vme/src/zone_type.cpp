#include "zone_type.h"

#include "dil.h"
#include "file_index_type.h"
#include "unit_data.h"
#include "zone_reset_cmd.h"

zone_type::zone_type()
{
    m_name = nullptr;
    m_notes = nullptr;
    m_help = nullptr;
    m_filename = nullptr;
    m_rooms = nullptr;
    m_objects = nullptr;
    m_npcs = nullptr;

    m_zri = nullptr;

    m_spmatrix = nullptr;
    m_no_rooms = 0;
    m_no_objs = 0;
    m_no_npcs = 0;
    m_access = 255;
}

zone_type::~zone_type()
{
    if (m_name)
    {
        FREE(m_name);
    }

    if (m_title)
    {
        FREE(m_title);
    }

    if (m_notes)
    {
        FREE(m_notes);
    }

    if (m_help)
    {
        FREE(m_help);
    }

    if (m_filename)
    {
        FREE(m_filename);
    }
    unit_data *ut = nullptr;
    unit_data *nextut = nullptr;

    for (ut = m_objects; ut; ut = nextut)
    {
        nextut = ut->getNext();
        ut->setNext(nullptr);
        ut->setGlobalNext(nullptr);
        ut->setGlobalPrevious(nullptr);
        delete ut;
    }

    for (ut = m_npcs; ut; ut = nextut)
    {
        nextut = ut->getNext();
        ut->setNext(nullptr);
        ut->setGlobalNext(nullptr);
        ut->setGlobalPrevious(nullptr);
        delete ut;
    }

    auto nextfi = m_mmp_fi.begin();
    for (auto p = m_mmp_fi.begin(); p != m_mmp_fi.end(); p = nextfi)
    {
        nextfi = p;
        nextfi++;

        delete p->second;
    }

    zone_reset_cmd *pzri = nullptr;
    zone_reset_cmd *nextzri = nullptr;

    for (pzri = m_zri; pzri; pzri = nextzri)
    {
        nextzri = pzri->getNext();
        DELETE(zone_reset_cmd, pzri);
    }

    auto nextpt = m_mmp_tmpl.begin();

    for (auto pt = m_mmp_tmpl.begin(); pt != m_mmp_tmpl.end(); pt = nextpt)
    {
        nextpt = pt;
        nextpt++;

        if (pt->second->prgname)
        {
            FREE(pt->second->prgname);
        }
        if (pt->second->argt)
        {
            FREE(pt->second->argt);
        }
        if (pt->second->core)
        {
            FREE(pt->second->core);
        }
        if (pt->second->vart)
        {
            FREE(pt->second->vart);
        }

        FREE(pt->second);
    }

    // struct bin_search_type *ba;    /* Pointer to binarray of type      */
    // struct diltemplate *tmpl;      /* DIL templates in zone            */
    // struct bin_search_type *tmplba;/* Pointer to binarray of type      */
    if (m_spmatrix)
    {
        FREE(m_spmatrix);
    }
}

cNamelist &zone_type::getCreators()
{
    return m_creators;
}

const char *zone_type::getName() const
{
    return m_name;
}

char *zone_type::getNamePtr()
{
    return m_name;
}

void zone_type::setName(char *value)
{
    FREE(m_name);
    m_name = value;
}

const char *zone_type::getTitle() const
{
    return m_title;
}

char **zone_type::getTitlePtrPtr()
{
    return &m_title;
}

void zone_type::setTitle(char *value)
{
    FREE(m_title);
    m_title = value;
}

const char *zone_type::getNotes() const
{
    return m_notes;
}

char **zone_type::getNotesPtrPtr()
{
    return &m_notes;
}

void zone_type::setNotes(char *value)
{
    FREE(m_notes);
    m_notes = value;
}

char **zone_type::getHelpPtrPtr()
{
    return &m_help;
}

void zone_type::setHelp(char *value)
{
    FREE(m_help);
    m_help = value;
}

const char *zone_type::getFilename() const
{
    return m_filename;
}

char **zone_type::getFilenamePtrPtr()
{
    return &m_filename;
}

void zone_type::setFilename(char *value)
{
    FREE(m_filename);
    m_filename = value;
}

const std::map<const char *, file_index_type *, cmp_str> &zone_type::cgetFileIndexMap() const
{
    return m_mmp_fi;
}

std::map<const char *, file_index_type *, cmp_str> &zone_type::getFileIndexMap()
{
    return m_mmp_fi;
}

const zone_reset_cmd *zone_type::cgetZoneResetCommands() const
{
    return m_zri;
}

zone_reset_cmd *zone_type::getZoneResetCommands()
{
    return m_zri;
}

void zone_type::setZoneResetCommands(zone_reset_cmd *value)
{
    DELETE(zone_reset_cmd, m_zri);
    m_zri = value;
}

const std::map<const char *, diltemplate *, cmp_str> &zone_type::cgetDILTemplate() const
{
    return m_mmp_tmpl;
}

std::map<const char *, diltemplate *, cmp_str> &zone_type::getDILTemplate()
{
    return m_mmp_tmpl;
}

void zone_type::setZoneNumber(ubit16 value)
{
    m_zone_no = value;
}

ubit16 zone_type::getNumOfFileIndexes() const
{
    return m_no_of_fi;
}

void zone_type::incrementNumOfFileIndexes()
{
    ++m_no_of_fi;
}

void zone_type::setNumOfFileIndexes(ubit16 value)
{
    m_no_of_fi = value;
}

ubit16 zone_type::getZoneResetTime() const
{
    return m_zone_time;
}

void zone_type::setZoneResetTime(ubit16 value)
{
    m_zone_time = value;
}

ubit16 *zone_type::getZoneResetTimePtr()
{
    return &m_zone_time;
}

ubit16 zone_type::getNumOfRooms() const
{
    return m_no_rooms;
}

void zone_type::incrementNumOfRooms()
{
    ++m_no_rooms;
}

void zone_type::setNumOfRooms(ubit16 value)
{
    m_no_rooms = value;
}

ubit16 zone_type::getNumOfObjects() const
{
    return m_no_objs;
}

void zone_type::incrementNumOfObjects()
{
    ++m_no_objs;
}

void zone_type::incrementNumOfNPCs()
{
    ++m_no_npcs;
}
ubit16 zone_type::getNumOfNPCs() const
{
    return m_no_npcs;
}

ubit8 zone_type::getResetMode() const
{
    return m_reset_mode;
}

ubit8 *zone_type::getResetModePtr()
{
    return &m_reset_mode;
}

void zone_type::setResetMode(ubit8 value)
{
    m_reset_mode = value;
}

void zone_type::incrementNumOfDILTemplates()
{
    ++m_no_tmpl;
}

void zone_type::setNumOfDILTemplates(ubit16 value)
{
    m_no_tmpl = value;
}

ubit8 zone_type::getAccessLevel() const
{
    return m_access;
}

void zone_type::setAccessLevel(ubit8 value)
{
    m_access = value;
}

ubit8 zone_type::getLevelRequiredToLoadItems() const
{
    return m_loadlevel;
}

ubit8 *zone_type::getLevelRequiredToLoadItemsPtr()
{
    return &m_loadlevel;
}

void zone_type::setLevelRequiredToLoadItems(ubit8 value)
{
    m_loadlevel = value;
}

ubit8 zone_type::getPayOnly() const
{
    return m_payonly;
}

ubit8 *zone_type::getPayOnlyPtr()
{
    return &m_payonly;
}

void zone_type::setPayOnly(ubit8 value)
{
    m_payonly = value;
}

const char *zone_type::getDILFilePath() const
{
    return m_dilfilepath;
}

void zone_type::setDILFilePath(char *value)
{
    FREE(m_dilfilepath);
    m_dilfilepath = value;
}

const zone_type::Weather &zone_type::cgetWeather() const
{
    return m_weather;
}

zone_type::Weather &zone_type::getWeather()
{
    return m_weather;
}
