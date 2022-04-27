/*
 $Author: All $
 $RCSfile: structs.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.8 $
 */

#include "structs.h"

#include "affect.h"
#include "color.h"
#include "db.h"
#include "db_file.h"
#include "handler.h"
#include "mobact.h"
#include "slog.h"
#include "textutil.h"
#include "utils.h"
#include "zone_reset_cmd.h"

int g_world_norooms = 0;   /* number of rooms in the world   */
int g_world_noobjects = 0; /* number of objects in the world */
int g_world_nochars = 0;   /* number of chars in the world   */
int g_world_nonpc = 0;     /* number of chars in the world   */
int g_world_nopc = 0;      /* number of chars in the world   */
int g_world_nozones = 0;   /* number of zones in the world   */

/* Descriptor stuff is in system.c */

char_data::char_data(ubit8 unit_type)
    : unit_data(unit_type)
    , m_descriptor{nullptr}
    , m_combat{nullptr}
    , m_master{nullptr}
    , m_last_room{nullptr}
    , m_points{}
    , m_followers{nullptr}
    , m_last_attacker{nullptr}
    , m_money{nullptr}
    , m_last_attacker_type{0}
{
    g_world_nochars++;

    // MS2020 memset(&points, 0, sizeof (points));
    // Removed because class constructor already initialize class variables.
}

char_data::~char_data()
{
#ifdef DMSERVER
    if (m_money)
    {
        FREE(m_money);
    }
    if (m_last_attacker)
    {
        FREE(m_last_attacker);
    }
#endif
    g_world_nochars--;
}

pc_data::pc_data()
    : char_data(UNIT_ST_PC)
    , setup{0, 0, 0, 0, 0, 0, 0, 0}
    , guild{nullptr}
    , bank{nullptr}
    , hometown{nullptr}
    , promptstr{nullptr}
    , profession{-1}
    , vlvl{0}
    , id{-1}
    , skill_points{0}
    , ability_points{0}
    , flags{0}
    , nr_of_crimes{0}
    , crack_attempts{0}
    , spells{0}
    , spell_lvl{0}
    , skills{0}
    , skill_lvl{0}
    , weapons{0}
    , weapon_lvl{0}
    , ability_lvl{0}
    , conditions{0}
    , nAccessLevel{0}
    , pwd{0}
    , filename{0}
    , lasthosts{0}
{
    g_world_nopc++;
}

pc_data::~pc_data()
{
    g_world_nopc--;

    if (guild)
        FREE(guild);

    if (hometown)
        FREE(hometown);

    if (bank)
        FREE(bank);

    if (promptstr)
        FREE(promptstr);
}

npc_data::npc_data()
    : char_data(UNIT_ST_NPC)
    , weapons{0}
    , spells{0}
    , default_pos{POSITION_STANDING}
    , flags{0}
{
    g_world_nonpc++;
}

npc_data::~npc_data()
{
    g_world_nonpc--;
}

unit_data *file_index_type::find_symbolic_instance_ref(unit_data *ref, ubit16 bitvector)
{
    unit_data *u = nullptr;
    unit_data *uu = nullptr;

    if (ref == nullptr)
    {
        return nullptr;
    }

    if (IS_SET(bitvector, FIND_UNIT_EQUIP))
    {
        for (u = UNIT_CONTAINS(ref); u; u = u->getNext())
        {
            if ((UNIT_FILE_INDEX(u) == this) && UNIT_IS_EQUIPPED(u))
            {
                return u;
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_INVEN))
    {
        for (u = UNIT_CONTAINS(ref); u; u = u->getNext())
        {
            if ((UNIT_FILE_INDEX(u) == this) && !UNIT_IS_EQUIPPED(u))
            {
                return u;
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_SURRO) && UNIT_IN(ref))
    {
        if (this == UNIT_FILE_INDEX(UNIT_IN(ref)))
        {
            return UNIT_IN(ref);
        }

        /* Run through units in local environment */
        for (u = UNIT_CONTAINS(UNIT_IN(ref)); u; u = u->getNext())
        {
            if (UNIT_FILE_INDEX(u) == this)
            {
                return u;
            }

            /* check tranparancy */
            if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
            {
                for (uu = UNIT_CONTAINS(u); uu; uu = uu->getNext())
                {
                    if (UNIT_FILE_INDEX(uu) == this)
                    {
                        return uu;
                    }
                }
            }
        }

        /* Run through units in local environment if upwards transparent */
        if ((u = UNIT_IN(UNIT_IN(ref))) && UNIT_IS_TRANSPARENT(UNIT_IN(ref)))
        {
            for (u = UNIT_CONTAINS(u); u; u = u->getNext())
            {
                if (u != UNIT_IN(ref))
                {
                    if (this == UNIT_FILE_INDEX(u))
                    {
                        return u;
                    }

                    /* check down into transparent unit */
                    if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
                    {
                        for (uu = UNIT_CONTAINS(u); uu; uu = uu->getNext())
                        {
                            if (this == UNIT_FILE_INDEX(uu))
                            {
                                return uu;
                            }
                        }
                    }
                }
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_ZONE))
    {
        if (!fi_unit_list.empty())
        {
            for (auto it = fi_unit_list.begin(); it != fi_unit_list.end(); it++)
            {
                if (UNIT_FILE_INDEX(*it) == this)
                {
                    return u;
                }
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_WORLD))
    {
        if (!fi_unit_list.empty())
        {
            return fi_unit_list.front();
        }
    }

    return nullptr;
}

unit_data *file_index_type::find_symbolic_instance()
{
    if (!fi_unit_list.empty())
    {
        return fi_unit_list.front();
    }

    return nullptr;
}

const char *file_index_type::getName() const
{
    return name.c_str();
}

long file_index_type::getFilepos() const
{
    return filepos;
}

ubit32 file_index_type::getCRC() const
{
    return crc;
}

sbit16 file_index_type::getNumInZone() const
{
    return no_in_zone;
}

ubit16 file_index_type::getRoomNum() const
{
    return room_no;
}

ubit8 file_index_type::getType() const
{
    return type;
}

ubit16 file_index_type::getNumInMem() const
{
    return no_in_mem;
}

ubit32 file_index_type::getLength() const
{
    return length;
}

void file_index_type::IncrementNumInMemory()
{
    ++no_in_mem;
}

zone_type *file_index_type::getZone() const
{
    return zone;
}

void file_index_type::setType(ubit8 value)
{
    type = value;
}

void file_index_type::setZone(zone_type *value)
{
    zone = value;
}

void file_index_type::setName(const char *value, bool to_lower)
{
    if (value)
    {
        name = std::string(value);
        if (to_lower)
        {
            str_lower(name);
        }
    }
}

void file_index_type::DecrementNumInMemory()
{
    /// @todo find out why code is trying to decrement no_in_mem below 0
    /// I tried this but somewhere it goes below zero
    ///    assert(no_in_mem > 0);
    if (no_in_mem > 0)
    {
        --no_in_mem;
    }
}

void file_index_type::IncrementNumInZone()
{
    ++no_in_zone;
}

void file_index_type::setNumInZone(sbit16 value)
{
    no_in_zone = value;
}

void file_index_type::setLength(ubit32 value)
{
    length = value;
}

void file_index_type::setFilepos(long value)
{
    filepos = value;
}

void file_index_type::setRoomNum(ubit16 value)
{
    room_no = value;
}

void file_index_type::setNumInMemory(sbit16 value)
{
    no_in_mem = value;
}

void file_index_type::setCRC(ubit32 value)
{
    crc = value;
}

bool file_index_type::Empty() const
{
    return fi_unit_list.empty();
}

unit_data *file_index_type::Front() const
{
    return fi_unit_list.front();
}

std::forward_list<unit_data *>::iterator file_index_type::Begin()
{
    return fi_unit_list.begin();
}

std::forward_list<unit_data *>::iterator file_index_type::End()
{
    return fi_unit_list.end();
}

void file_index_type::PushFront(unit_data *value)
{
    fi_unit_list.push_front(value);
}

void file_index_type::Remove(unit_data *value)
{
    fi_unit_list.remove(value);
}

unit_data *new_unit_data(ubit8 type)
{
    if (type == UNIT_ST_ROOM)
    {
        return new EMPLACE(room_data) room_data;
    }
    else if (type == UNIT_ST_OBJ)
    {
        return new EMPLACE(obj_data) obj_data;
    }
    else if (type == UNIT_ST_PC)
    {
        return new EMPLACE(pc_data) pc_data;
    }
    else if (type == UNIT_ST_NPC)
    {
        return new EMPLACE(npc_data) npc_data;
    }
    else
    {
        assert(FALSE);
        return nullptr; // Need to avoid warning on Git actions.
    }
}
