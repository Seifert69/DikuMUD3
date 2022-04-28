/*
 $Author: All $
 $RCSfile: structs.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.8 $
 */

#include "structs.h"

#include "color.h"
#include "handler.h"
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

npc_data::npc_data()
    : char_data(UNIT_ST_NPC)
    , m_weapons{0}
    , m_spells{0}
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
