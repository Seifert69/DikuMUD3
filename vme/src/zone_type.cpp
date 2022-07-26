#include "zone_type.h"

#include "db.h"
#include "dil.h"
#include "file_index_type.h"
#include "formatter.h"
#include "szonelog.h"
#include "textutil.h"
#include "unit_data.h"
#include "zone_reset_cmd.h"

zone_type::zone_type(std::string name, std::string filename)
    : m_name(std::move(name))
    , m_filename(std::move(filename))
{
    str_lower(m_name);
}

zone_type::~zone_type()
{
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

    zone_reset_cmd *pzri = nullptr;
    zone_reset_cmd *nextzri = nullptr;

    for (pzri = m_zri; pzri; pzri = nextzri)
    {
        nextzri = pzri->getNext();
        DELETE(zone_reset_cmd, pzri);
    }

    for (auto &[name, pt] : m_mmp_tmpl)
    {
        if (pt->prgname)
        {
            FREE(pt->prgname);
        }
        if (pt->argt)
        {
            FREE(pt->argt);
        }
        if (pt->core)
        {
            FREE(pt->core);
        }
        if (pt->vart)
        {
            FREE(pt->vart);
        }
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

const std::string &zone_type::getName() const
{
    return m_name;
}

char *zone_type::getNamePtr()
{
    return m_name.data();
}

const std::string &zone_type::getTitle() const
{
    return m_title.getValue();
}

char **zone_type::getTitlePtrPtr()
{
    return m_title.getValuePtr();
}

void zone_type::setTitle(std::string value)
{
    m_title = std::move(value);
}

const std::string &zone_type::getNotes() const
{
    return m_notes.getValue();
}

char **zone_type::getNotesPtrPtr()
{
    return m_notes.getValuePtr();
}

void zone_type::setNotes(std::string value)
{
    m_notes = std::move(value);
}

char **zone_type::getHelpPtrPtr()
{
    return m_help.getValuePtr();
}

void zone_type::setHelp(std::string value)
{
    m_help = std::move(value);
}

const std::string &zone_type::getFilename() const
{
    return m_filename.getValue();
}

char **zone_type::getFilenamePtrPtr()
{
    return m_filename.getValuePtr();
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

size_t zone_type::getNumOfFileIndexes() const
{
    return m_mmp_fi.size();
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

const std::optional<std::string> &zone_type::getDILFilePath() const
{
    return m_dilfilepath;
}

void zone_type::setDILFilePath(std::string value)
{
    m_dilfilepath = std::move(value);
}

const Weather &zone_type::cgetWeather() const
{
    return m_weather;
}

Weather &zone_type::getWeather()
{
    return m_weather;
}

std::string zone_type::getExtraStatZoneMessage(int search_type) const
{
    std::string msg;
    for (const auto &[name, fi] : m_mmp_fi)
    {
        if (fi->getType() == search_type)
        {
            if ((fi->getType() == UNIT_ST_OBJ) || (fi->getType() == UNIT_ST_NPC))
            {
                msg += diku::format_to_str("<a cmd='load #'>%s</a><br/>", name);
            }
            else
            {
                msg += diku::format_to_str("%s<br/>", name);
            }
        }
    }
    return msg;
}

void zone_type::readAllUnitRooms()
{
    for (auto &[name, fi] : m_mmp_fi)
    {
        if (fi->getType() == UNIT_ST_ROOM)
        {
            read_unit(fi.get());
        }
    }
}

unit_data *zone_type::findFirstRoom()
{
    return findFirstUnitOfType(UNIT_ST_ROOM);
}

unit_data *zone_type::findFirstNPC()
{
    return findFirstUnitOfType(UNIT_ST_NPC);
}

unit_data *zone_type::findFirstObj()
{
    return findFirstUnitOfType(UNIT_ST_OBJ);
}

unit_data *zone_type::findFirstUnitOfType(int type)
{
    for (auto &[name, fi] : m_mmp_fi)
    {
        if (fi->getType() == type)
        {
            if (fi->Empty())
            {
                return nullptr;
            }
            else
            {
                return fi->Front();
            }
        }
    }
    return nullptr;
}

file_index_type *zone_type::findOrCreateFileIndex(const char *name)
{
    if (auto it = m_mmp_fi.find(name); it != m_mmp_fi.end())
    {
        return it->second.get();
    }

    auto fi = std::make_unique<file_index_type>();
    fi->setName(str_dup(name));
    fi->setZone(this);
    fi->setType(UNIT_ST_PC);

    auto result = m_mmp_fi.insert(std::make_pair(name, std::move(fi)));

    return result.first->second.get();
}

void zone_type::updateNumberInZone()
{
    for (auto &[name, fi] : m_mmp_fi)
    {
        fi->setNumInZone(0);
    }
}

void zone_type::insertFileIndex(std::unique_ptr<file_index_type> &&value)
{
    m_mmp_fi.insert(std::make_pair(value->getName(), std::move(value)));
}

file_index_type *zone_type::findFileIndex(const std::string &name)
{
    if (auto it = m_mmp_fi.find(name); it != m_mmp_fi.end())
    {
        return it->second.get();
    }
    return nullptr;
}

std::string zone_type::getStatDIL() const
{
    std::string msg;
    for (auto &[name, dil_template] : m_mmp_tmpl)
    {
        msg += diku::format_to_str("%.2fs %s [%d t / %d i]<br/>",
                                   dil_template->fCPU / 1000.0,
                                   dil_template->prgname,
                                   dil_template->nTriggers,
                                   dil_template->nInstructions);
    }
    return msg;
}

std::string zone_type::getStatGlobalDIL(ubit32 nCount, ubit64 &instructionSum) const
{
    std::string msg;
    for (auto &[name, dil_template] : m_mmp_tmpl)
    {
        instructionSum += dil_template->nInstructions;
        if (dil_template->fCPU >= nCount)
        {
            msg += diku::format_to_str("%.2fs %s@%s [%d t / %d i]<br/>",
                                       dil_template->fCPU / 1000.0,
                                       dil_template->prgname,
                                       dil_template->zone->getName(),
                                       dil_template->nTriggers,
                                       dil_template->nInstructions);
        }
    }
    return msg;
}

void zone_type::resolveZoneTemplates()
{
    for (auto &[name, tmpl] : m_mmp_tmpl)
    {
        /* all external references */
        for (int i = 0; i < tmpl->xrefcount; i++)
        {
            bool valid = true;
            tmpl->extprg[i] = find_dil_template(tmpl->xrefs[i].name);

            if (tmpl->extprg[i])
            {
                /* check argument count and types */
                if ((tmpl->xrefs[i].rtnt != tmpl->extprg[i]->rtnt) || (tmpl->xrefs[i].argc != tmpl->extprg[i]->argc))
                {
                    valid = false;
                }
                for (int j = 0; j < tmpl->xrefs[i].argc; j++)
                {
                    if (tmpl->xrefs[i].argt[j] != tmpl->extprg[i]->argt[j])
                    {
                        valid = false;
                    }
                }
            }
            else
            {
                /* ERROR MESSAGE HERE */
                szonelog(this, "Cannot resolve external reference '%s'", tmpl->xrefs[i].name);
            }
            /* Typecheck error ! */
            if (!valid)
            {
                tmpl->extprg[i] = nullptr;
                /* ERROR MESSAGE HERE */
                szonelog(this, "Error typechecking reference to '%s'", tmpl->xrefs[i].name);
            }
        }
    }
}

void zone_type::insertDILTemplate(std::unique_ptr<diltemplate> &&value)
{
    m_mmp_tmpl.insert(std::make_pair(value->prgname, std::move(value)));
}

diltemplate *zone_type::findDILTemplate(const std::string &name)
{
    if (auto it = m_mmp_tmpl.find(name); it != m_mmp_tmpl.end())
    {
        return it->second.get();
    }

    return nullptr;
}
