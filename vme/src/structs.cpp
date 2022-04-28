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

pc_data::pc_data()
    : char_data(UNIT_ST_PC)
    , m_setup{0, 0, 0, 0, 0, 0, 0, 0}
    , m_guild{nullptr}
    , m_bank{nullptr}
    , m_hometown{nullptr}
    , m_promptstr{nullptr}
    , m_profession{-1}
    , m_vlvl{0}
    , m_id{-1}
    , m_skill_points{0}
    , m_ability_points{0}
    , m_flags{0}
    , m_nr_of_crimes{0}
    , m_crack_attempts{0}
    , m_lifespan{0}
    , m_spells{0}
    , m_spell_lvl{0}
    , m_skills{0}
    , m_skill_lvl{0}
    , m_weapons{0}
    , m_weapon_lvl{0}
    , m_ability_lvl{0}
    , m_conditions{0}
    , m_nAccessLevel{0}
    , m_pwd{0}
    , m_filename{0}
    , m_lasthosts{0}
{
    g_world_nopc++;
}

pc_data::~pc_data()
{
    g_world_nopc--;

    if (m_guild)
        FREE(m_guild);

    if (m_hometown)
        FREE(m_hometown);

    if (m_bank)
        FREE(m_bank);

    if (m_promptstr)
        FREE(m_promptstr);
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
