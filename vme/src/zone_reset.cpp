/*
 $Author: All $
 $RCSfile: zone_reset.cpp,v $
 $Date: 2004/03/20 06:13:22 $
 $Revision: 2.6 $
 */
#include "zone_reset.h"

#include "comm.h"
#include "common.h"
#include "db.h"
#include "dilrun.h"
#include "handler.h"
#include "interpreter.h"
#include "main_functions.h"
#include "money.h"
#include "structs.h"
#include "szonelog.h"
#include "utility.h"
#include "utils.h"

class zone_type *g_boot_zone = nullptr; /* Points to the zone currently booted */

/* No Operation */
class unit_data *zone_nop(class unit_data *u, struct zone_reset_cmd *cmd)
{
    /* Return TRUE - NOP always succeedes */

    return (class unit_data *)g_boot_zone; /* dummy */
}

/* Random */
class unit_data *zone_random(class unit_data *u, struct zone_reset_cmd *cmd)
{
    /* Return TRUE if random 0-99 less than given percent  */
    if (number(0, 99) < cmd->num[0])
    {
        return (class unit_data *)g_boot_zone; /* dummy */
    }
    else
    {
        return nullptr;
    }
}

/* Count ->no_in_zone for current 'g_boot_zone' (above) */
void zone_update_no_in_zone(void)
{
    class unit_data *u;

    /* Clear ALL ->no_in_zone */
    for (auto tmp_zone = g_zone_info.mmp.begin(); tmp_zone != g_zone_info.mmp.end(); tmp_zone++)
    {
        for (auto fi = tmp_zone->second->mmp_fi.begin(); fi != tmp_zone->second->mmp_fi.end(); fi++)
        {
            fi->second->no_in_zone = 0;
        }
    }

    for (u = g_unit_list; u; u = u->gnext)
    {
        if (UNIT_FILE_INDEX(u) && (unit_zone(u) == g_boot_zone))
        {
            UNIT_FILE_INDEX(u)->no_in_zone++;
        }
    }
}

/* After loading a unit, call this function to update no_in_zone */
void zone_loaded_a_unit(class unit_data *u)
{
    if (unit_zone(u) == g_boot_zone)
    {
        UNIT_FILE_INDEX(u)->no_in_zone++;
    }
}

/* num[0] is the max allowed existing in world              */
/* num[1] is the max allowed existing in zone.              */
/* num[2] is the max allowed existing in room (object)      */
/* Return TRUE if conditions are met, FALSE otherwise       */
bool zone_limit(class unit_data *u, class file_index_type *fi, struct zone_reset_cmd *cmd)
{
    class unit_data *tmp;
    sbit16 i;

    if (fi->type == UNIT_ST_NPC)
    {
        /* If no maxima on mobiles, set it to default of one global. */
        if (cmd->num[0] == 0 && cmd->num[1] == 0 && cmd->num[2] == 0)
        {
            cmd->num[0] = 1;
        }
    }
    else
    {
        /* If no maxima on objects, set it to default of one local! */
        if (cmd->num[0] == 0 && cmd->num[1] == 0 && cmd->num[2] == 0)
        {
            cmd->num[2] = 1;
        }
    }

    /* Check for global maxima */
    if (cmd->num[0] && (fi->no_in_mem) >= (ubit16)(cmd->num[0]))
    {
        return FALSE;
    }

    /* Check for zone maximum */
    if (cmd->num[1] && fi->no_in_zone >= cmd->num[1] && unit_zone(u) == g_boot_zone)
    {
        return FALSE;
    }

    /* Check for local maxima */
    if ((i = cmd->num[2]))
    {
        for (tmp = UNIT_CONTAINS(u); tmp; tmp = tmp->next)
        {
            if (UNIT_FILE_INDEX(tmp) == fi)
            {
                --i;
            }
        }

        if (i <= 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

/* fi[0] is unit to be loaded                                     */
/* fi[1] is room to place loaded unit in or 0 if a PUT command    */
/* num[0] is the max allowed existing number (0 ignores) in world */
/* num[1] is the max allowed locally existing number              */
class unit_data *zone_load(class unit_data *u, struct zone_reset_cmd *cmd)
{
    class unit_data *loaded = nullptr;

    /* Destination */
    if (cmd->fi[1] && !cmd->fi[1]->fi_unit_list.empty() && cmd->fi[1]->type == UNIT_ST_ROOM)
    {
        u = cmd->fi[1]->fi_unit_list.front();
    }

    /* Does the destination room exist */
    if (u == nullptr)
    {
        szonelog(g_boot_zone, "Reset Error: Don't know where to put %s@%s", cmd->fi[0]->name, cmd->fi[0]->zone->name);
    }
    else if (cmd->fi[0]->type != UNIT_ST_OBJ && cmd->fi[0]->type != UNIT_ST_NPC)
    {
        szonelog(g_boot_zone, "Reset Error: %s@%s loaded object is neither an obj nor npc.", cmd->fi[0]->name, cmd->fi[0]->zone->name);
    }
    else if (zone_limit(u, cmd->fi[0], cmd))
    {
        loaded = read_unit(cmd->fi[0]);
        if (!loaded->is_destructed())
        {
            if (IS_MONEY(loaded))
            {
                set_money(loaded, MONEY_AMOUNT(loaded));
            }
            if (!UNIT_IN(loaded))
            {
                unit_to_unit(loaded, u);
            }
            zone_loaded_a_unit(loaded);
            dil_loadtime_activate(loaded);
            if (IS_CHAR(loaded))
            {
                act("$1n has arrived.", A_HIDEINV, loaded, cActParameter(), cActParameter(), TO_ROOM);
                UNIT_SIZE(loaded) += (UNIT_SIZE(loaded) * (55 - dice(10, 10))) / 300;
            }
        }
        else
        {
            loaded = nullptr;
        }
    }

    return loaded;
}

/* fi[0] is unit to be loaded and equipped on parent unit.  */
/* num[0] is the max allowed existing number (0 ignores)    */
/* num[1] is equipment position                             */
class unit_data *zone_equip(class unit_data *u, struct zone_reset_cmd *cmd)
{
    class unit_data *loaded = nullptr;

    /* Does the destination unit exist */
    if (u == nullptr)
    {
        szonelog(g_boot_zone, "Reset error: %s@%s has no parent in equip.", cmd->fi[0]->name, cmd->fi[0]->zone->name);
    }
    else if (!IS_CHAR(u))
    {
        szonelog(g_boot_zone,
                 "Reset Error: %s@%s is not a char in equip.",
                 UNIT_FI_NAME(u),
                 UNIT_FI_ZONENAME(u)); // cmd->fi[0]->name, cmd->fi[0]->zone->name);
    }
    else if (cmd->fi[0]->type != UNIT_ST_OBJ)
    {
        szonelog(g_boot_zone,
                 "Reset Error: %s@%s equipping %s@%s is not an object.",
                 UNIT_FI_NAME(u),
                 UNIT_FI_ZONENAME(u),
                 cmd->fi[0]->name,
                 cmd->fi[0]->zone->name);
    }
    else if (cmd->num[1] <= 0)
    {
        szonelog(g_boot_zone,
                 "Reset Error: %s@%s equipping %s@%s doesn't have a legal equip position.",
                 UNIT_FI_NAME(u),
                 UNIT_FI_ZONENAME(u),
                 cmd->fi[0]->name,
                 cmd->fi[0]->zone->name);
    }
    else if (!equipment(u, cmd->num[1]) && !(cmd->num[0] && (cmd->fi[0]->no_in_mem) >= (ubit16)(cmd->num[0])))
    {
        loaded = read_unit(cmd->fi[0]);

        unit_to_unit(loaded, u);
        zone_loaded_a_unit(loaded);
        if (loaded)
        {
            dil_loadtime_activate(loaded);
        }

        if (IS_OBJ(loaded))
        {
#ifdef SUSPEKT
            if ((cmd->num[1] == WEAR_WIELD) && (OBJ_TYPE(loaded) == ITEM_WEAPON))
            {
                int max = 0, i;
                for (i = 0; i < WPN_GROUP_MAX; i++)
                    if (NPC_WPN_SKILL(u, i) > max)
                        max = NPC_WPN_SKILL(u, i);

                if (weapon_defense_skill(u, OBJ_VALUE(loaded, 0)) < max)
                    szonelog(UNIT_FI_ZONE(u),
                             "%s@%s: Weapon NOT equipped "
                             "on best skill",
                             UNIT_FI_NAME(u),
                             UNIT_FI_ZONENAME(u));
            }
#endif
            equip_char(u, loaded, cmd->num[1]);
            UNIT_SIZE(loaded) = UNIT_SIZE(u); /* Autofit */
        }
    }

    return loaded;
}

/* fi[0] is room in which the door is located.              */
/* num[0] is the exit number (0..5)                         */
/* num[1] is the new state                                  */
class unit_data *zone_door(class unit_data *u, struct zone_reset_cmd *cmd)
{
    if (!cmd->fi[0] || (cmd->fi[0]->type != UNIT_ST_ROOM))
    {
        szonelog(g_boot_zone, "Zone Reset Error: Not a room in door reference!");
    }
    else if (!ROOM_EXIT(cmd->fi[0]->fi_unit_list.front(), cmd->num[0]))
    {
        szonelog(g_boot_zone, "Zone Reset Error: No %s direction from room %s in door.", g_dirs[cmd->num[0]], cmd->fi[0]->name);
    }
    else
    {
        ROOM_EXIT(cmd->fi[0]->fi_unit_list.front(), cmd->num[0])->exit_info = cmd->num[1];
    }

    return nullptr;
}

/* fi[0] is the room to be purged.                          */
class unit_data *zone_purge(class unit_data *u, struct zone_reset_cmd *cmd)
{
    class unit_data *next;

    if (cmd->fi[0]->type != UNIT_ST_ROOM)
    {
        szonelog(g_boot_zone, "Reset Error : No room in purge reference!");
    }
    else
    {
        for (u = UNIT_CONTAINS(cmd->fi[0]->fi_unit_list.front()); u; u = next)
        {
            next = u->next;
            if (!IS_PC(u) && !IS_ROOM(u))
            {
                extract_unit(u);
            }
        }
    }

    return nullptr;
}

/* fi[0] is the thing(s) to be removed.                          */
/* fi[1] is the room to remove from.                             */
class unit_data *zone_remove(class unit_data *u, struct zone_reset_cmd *cmd)
{
    class unit_data *next;

    if (cmd->fi[1]->type != UNIT_ST_ROOM)
    {
        szonelog(g_boot_zone, "Reset Error: No room in remove reference!");
    }
    else
    {
        for (u = UNIT_CONTAINS(cmd->fi[1]->fi_unit_list.front()); u; u = next)
        {
            next = u->next;
            if (UNIT_FILE_INDEX(u) == cmd->fi[0] && !IS_ROOM(u))
            {
                extract_unit(u);
            }
        }
    }

    return nullptr;
}

/* 'u' is the 'master' that will be followed                      */
/* fi[0] is char to be loaded to  follow 'u'                      */
/* fi[1] -                                                        */
/* num[0] is the max allowed existing number (0 ignores) in world */
/* num[1] is the max allowed locally existing number              */
class unit_data *zone_follow(class unit_data *u, struct zone_reset_cmd *cmd)
{
    class unit_data *loaded = nullptr;

    /* Does the master exist */
    if (u == nullptr)
    {
        szonelog(g_boot_zone, "Reset Error: Non Existant destination-unit in follow");
    }
    else if (!IS_CHAR(u))
    {
        szonelog(g_boot_zone, "Reset Error: Master to follow is not a mobile.");
    }
    else if (cmd->fi[0]->type != UNIT_ST_NPC)
    {
        szonelog(g_boot_zone, "Reset Error: Follower %s is not a mobile.", cmd->fi[0]->name);
    }
    else if (zone_limit(u, cmd->fi[0], cmd))
    {
        loaded = read_unit(cmd->fi[0]);

        unit_to_unit(loaded, UNIT_IN(u));
        start_following(loaded, u);
        zone_loaded_a_unit(loaded);

        act("$1n has arrived.", A_HIDEINV, loaded, cActParameter(), cActParameter(), TO_ROOM);
        if (loaded)
        {
            dil_loadtime_activate(loaded);
        }
    }

    return loaded;
}

class unit_data *(*exec_zone_cmd[])(class unit_data *, struct zone_reset_cmd *) =
    {zone_nop, zone_load, zone_equip, zone_door, zone_purge, zone_remove, zone_follow, zone_random};

bool low_reset_zone(class unit_data *u, struct zone_reset_cmd *cmd)
{
    class unit_data *success;
    bool ok = TRUE;

    for (; cmd; cmd = cmd->next)
    {
        success = (*exec_zone_cmd[cmd->cmd_no])(u, cmd);
        if (success && cmd->nested && !low_reset_zone(success, cmd->nested) && cmd->cmpl)
        {
            extract_unit(success);
            success = nullptr;
        }

        ok = ok && success;
    }
    return ok;
}

void zone_reset(class zone_type *zone)
{
    /* extern int memory_total_alloc;
           int i = memory_total_alloc; */
    g_boot_zone = zone;

    zone_update_no_in_zone(); /* Reset the fi->no_in_zone */

    low_reset_zone(nullptr, zone->zri);

    /* Far too much LOG:
       slog(LOG_OFF, 0, "Zone reset of '%s' done (%d bytes used).",
       zone->name, g_memory_total_alloc - i); */

    g_boot_zone = nullptr;
}

/* MS: Changed this to queue reset events at boot such that game comes up
   really fast */

/* Changed back to boot all before players login */
void reset_all_zones(void)
{
    int j;

    for (j = 0; j <= 255; j++)
    {
        for (auto zone = g_zone_info.mmp.begin(); zone != g_zone_info.mmp.end(); zone++)
        {
            if (j == 0)
            {
                g_world_nozones++;
            }

            if (zone->second->access != j)
            {
                continue;
            }

            if (zone->second->zone_time > 0)
            {
                zone_event((void *)zone->second, (void *)nullptr);
            }
        }
    }
}

bool zone_is_empty(class zone_type *zone)
{
    class descriptor_data *d;

    for (d = g_descriptor_list; d; d = d->next)
    {
        if (descriptor_is_playing(d))
        {
            if (unit_zone(d->character) == zone)
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

/* Check if any zones needs updating */
void zone_event(void *p1, void *p2)
{
    class zone_type *zone = (class zone_type *)p1;

    if (zone->reset_mode != RESET_IFEMPTY || zone_is_empty(zone))
    {
        zone_reset(zone);

        /* Papi: Did a little random boogie to prevent reset all at the
         *       same time (causes lags!)
         */

        if (zone->reset_mode != RESET_NOT)
        {
            g_events.add(zone->zone_time * PULSE_ZONE + number(0, WAIT_SEC * 180), zone_event, zone, nullptr);
        }
    }
    else
    {
        g_events.add(1 * PULSE_ZONE, zone_event, zone, nullptr);
    }
}
