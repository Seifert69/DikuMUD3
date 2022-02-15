/*
 $Author: All $
 $RCSfile: act_wizard.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.9 $
 */
#include "act_wizard.h"

#include "comm.h"
#include "common.h"
#include "db.h"
#include "dilrun.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "main_functions.h"
#include "money.h"
#include "nanny.h"
#include "pcsave.h"
#include "slog.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"
#include "zon_basis.h"
#include "zone_reset.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

void do_timewarp(unit_data *ch, char *argument, const command_info *cmd)
{
    if (str_is_empty(argument))
    {
        send_to_char("timewrap: How many seconds to you want to wrap time?<br/>", ch);
        return;
    }

    int i = atoi(argument);

    if (i < 1)
    {
        send_to_char("timewrap: You cannot warp for less than a second.<br/>", ch);
        return;
    }

    auto msg = diku::format_to_str("Time warping for %d seconds<br/>", i);
    send_to_char(msg, ch);

    g_events.add(PULSE_SEC * i, timewarp_end, nullptr, nullptr);

    g_nTickUsec = 0; // Warp the time
}

void do_users(unit_data *ch, char *argument, const command_info *cmd)
{
    descriptor_data *d = nullptr;
    int users = 0;

    std::string msg{"<u>Connections:</u><br/>"};
    /*
       <I%3d/%3d> immort lvl / wizi
       < %3d>     mortal vlvl
       < %3d/%3d> mortal vlvl/ wizi
    */

    for (d = g_descriptor_list; d; d = d->getNext())
    {
        assert(d->cgetCharacter());
        if (CHAR_LEVEL(ch) >= UNIT_MINV(CHAR_ORIGINAL(d->cgetCharacter())))
        {
            users++;
            if (IS_IMMORTAL(d->cgetCharacter()))
            {
                /* an immortal character */
                msg += diku::format_to_str("&lt;I%3d/%3d&gt; %-16s %-10s [%c %4d %-3s %s]<br/>",
                                           CHAR_LEVEL(CHAR_ORIGINAL(d->cgetCharacter())),
                                           UNIT_MINV(CHAR_ORIGINAL(d->cgetCharacter())),
                                           UNIT_NAME(CHAR_ORIGINAL(d->cgetCharacter())),
                                           descriptor_is_playing(d) ? "Playing" : "Menu",
                                           g_cServerConfig.FromLAN(d->getHostname()) ? 'L' : 'W',
                                           d->getMplexPortNum(),
                                           d->getSerialLine() == 255 ? "---" : itoa(d->getSerialLine()),
                                           d->getHostname());
            }
            else
            {
                /* a mortal character */
                msg += diku::format_to_str("&lt; %6d%c&gt; %-16s %-10s [%c %4d %-3s %s]<br/>",
                                           PC_VIRTUAL_LEVEL(CHAR_ORIGINAL(d->cgetCharacter())),
                                           UNIT_MINV(CHAR_ORIGINAL(d->cgetCharacter())) ? '*' : ' ',
                                           UNIT_NAME(CHAR_ORIGINAL(d->cgetCharacter())),
                                           descriptor_is_playing(d) ? "Playing" : "Menu",
                                           g_cServerConfig.FromLAN(d->getHostname()) ? 'L' : 'W',
                                           d->getMplexPortNum(),
                                           d->getSerialLine() == 255 ? "---" : itoa(d->getSerialLine()),
                                           d->getHostname());
            }
        }
    }

    msg += diku::format_to_str("<br/>%d visible players connected.<br/>", users);
    page_string(CHAR_DESCRIPTOR(ch), msg);
}

/* Reset the zone in which the char is in! */
void do_reset(unit_data *ch, char *arg, const command_info *cmd)
{
    zone_type *zone = nullptr;

    if (!str_is_empty(arg))
    {
        send_to_char("You can only reset the zone you are in.<br/>", ch);
        return;
    }

    if (!(zone = unit_zone(ch)))
    {
        send_to_char("You are inside no zone to reset!<br/>", ch);
        return;
    }

    send_to_char("Ok.<br/>", ch);
    zone_reset(zone);
}

void do_at(unit_data *ch, char *argument, const command_info *cmd)
{
    char buf[MAX_INPUT_LENGTH];
    unit_data *target = nullptr;
    unit_data *original_loc = nullptr;
    file_index_type *fi = nullptr;

    if (!IS_PC(ch))
    {
        return;
    }

    if (str_is_empty(argument))
    {
        send_to_char("You must supply a unit name or zone reference.<br/>", ch);
        return;
    }

    if ((fi = pc_str_to_file_index(ch, argument)) && (!fi->Empty()) && (fi->getType() == UNIT_ST_ROOM))
    {
        target = fi->Front();
        argument = one_argument(argument, buf);
    }
    else
    {
        if ((target = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD)))
        {
            if (UNIT_IN(target))
            {
                target = UNIT_IN(target);
            }
        }
    }

    if (!target)
    {
        send_to_char("No such place around.<br/>", ch);
        return;
    }

    if (!IS_ROOM(target))
    {
        send_to_char("Sorry, you may only 'at' at a room!<br/>", ch);
        return;
    }

    original_loc = UNIT_IN(ch);
    unit_from_unit(ch);
    unit_to_unit(ch, target);
    command_interpreter(ch, argument);

    /* check if the guy's still there */
    if (!ch->is_destructed() && !original_loc->is_destructed() && !unit_recursive(ch, original_loc))
    {
        unit_from_unit(ch);
        unit_to_unit(ch, original_loc);
    }
}

void do_crash(unit_data *ch, char *argument, const command_info *cmd)
{
    if (cmd_is_abbrev(ch, cmd))
    {
        send_to_char("If you want to crash the game - say so!<br/>", ch);
        return;
    }

    if (strcmp(argument, "the entire game..."))
    {
        send_to_char("You must type 'crash the entire game...'<br/>", ch);
    }
    else
    {
        assert(FALSE); /* Bye bye */
    }
}

void do_execute(unit_data *ch, char *argument, const command_info *cmd)
{
    argument = skip_spaces(argument);

    if (!system_check(ch, argument))
    {
        return;
    }

    execute_append(ch, argument);
    act("Executing $2t.", A_ALWAYS, ch, argument, cActParameter(), TO_CHAR);
}

void do_shutdown(unit_data *ch, char *argument, const command_info *cmd)
{
    if (!IS_PC(ch))
    {
        return;
    }

    if (cmd_is_abbrev(ch, cmd))
    {
        send_to_char("If you want to shut something down - say so!<br/>", ch);
        return;
    }

    auto msg = diku::format_to_str("Shutdown by %s.<br/>", UNIT_NAME(ch));
    send_to_all(msg);
    g_mud_shutdown = 1;
}

void do_snoop(unit_data *ch, char *argument, const command_info *cmd)
{
    unit_data *victim = nullptr;

    if (!CHAR_DESCRIPTOR(ch))
    {
        return;
    }

    if (str_is_empty(argument))
    {
        victim = ch;
    }
    else
    {
        victim = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD);
    }

    if (!victim)
    {
        send_to_char("No such player around.<br/>", ch);
        return;
    }

    if (!IS_CHAR(victim))
    {
        send_to_char("That's not a player / monster!<br/>", ch);
        return;
    }

    if (!CHAR_DESCRIPTOR(victim))
    {
        act("$3n has no descriptor-link.", A_SOMEONE, ch, cActParameter(), victim, TO_CHAR);
        return;
    }

    if (victim == ch)
    {
        if (CHAR_IS_SNOOPING(ch))
        {
            unsnoop(ch, 0); /* Unsnoop just ch himself */
        }
        else
        {
            send_to_char("You are already snooping yourself.<br/>", ch);
        }
        return;
    }

    if (CHAR_IS_SNOOPED(victim))
    {
        send_to_char("Busy already.<br/>", ch);
        return;
    }

    if (CHAR_LEVEL(CHAR_ORIGINAL(victim)) >= CHAR_LEVEL(CHAR_ORIGINAL(ch)))
    {
        send_to_char("You're not allowed to snoop someone your own status.<br/>", ch);
        return;
    }

    send_to_char("Ok.<br/>", ch);

    if (CHAR_IS_SNOOPING(ch))
    {
        unsnoop(ch, 0); /* Unsnoop just ch himself */
    }

    snoop(ch, victim);
}

/* return -> switch <no_arg> */

void do_switch(unit_data *ch, char *argument, const command_info *cmd)
{
    unit_data *victim = nullptr;

    if (!CHAR_DESCRIPTOR(ch))
    {
        return;
    }

    if (str_is_empty(argument))
    {
        if (CHAR_IS_SWITCHED(ch))
        {
            unswitchbody(ch);
        }
        else
        {
            send_to_char("You are already home in your good old body.<br/>", ch);
        }
        return;
    }

    victim = find_unit(ch, &argument, nullptr, FIND_UNIT_WORLD | FIND_UNIT_SURRO);

    if (!victim || !IS_NPC(victim))
    {
        send_to_char("No such monster around.<br/>", ch);
        return;
    }

    if (ch == victim)
    {
        send_to_char("He he he... We are jolly funny today, eh?<br/>", ch);
        return;
    }

    if (CHAR_DESCRIPTOR(victim))
    {
        act("$3n's body is already in use!", A_ALWAYS, ch, cActParameter(), victim, TO_CHAR);
    }
    else
    {
        send_to_char("Ok.<br/>", ch);
        switchbody(ch, victim);
    }
}

void do_load(unit_data *ch, char *arg, const command_info *cmd)
{
    char buf[MAX_INPUT_LENGTH];
    file_index_type *fi = nullptr;
    unit_data *u = nullptr;
    unit_data *tmp = nullptr;

    if (str_is_empty(arg))
    {
        send_to_char("Load? Load what?<br/>", ch);
        return;
    }

    arg = one_argument(arg, buf);

    if (find_descriptor(buf, nullptr))
    {
        send_to_char("A player by that name is connected.<br/>", ch);
        return;
    }

    if ((fi = pc_str_to_file_index(ch, buf)) == nullptr)
    {
        for (tmp = g_unit_list; tmp; tmp = tmp->gnext)
        {
            if (IS_PC(tmp) && !str_ccmp(UNIT_NAME(tmp), buf))
            {
                send_to_char("A player by that name is linkdead in the game.<br/>", ch);
                return;
            }
        }

        if (player_exists(buf))
        {
            if ((u = load_player(buf)) == nullptr)
            {
                send_to_char("Load error<br/>", ch);
                return;
            }

            enter_game(u, TRUE);

            unit_from_unit(u);
            unit_to_unit(u, UNIT_IN(ch));
            send_to_char("You have loaded the player.<br/>", ch);

            if (UNIT_CONTAINS(u))
            {
                send_to_char("Inventory loaded.<br/>", ch);
            }

            if (CHAR_LEVEL(u) > CHAR_LEVEL(ch))
            {
                slog(LOG_EXTENSIVE, UNIT_MINV(ch), "LEVEL: %s loaded %s when lower level.", UNIT_NAME(ch), UNIT_NAME(u));
            }
            return;
        }

        send_to_char("No such file index reference found.<br/>", ch);
        return;
    }

    if (fi->getType() == UNIT_ST_ROOM)
    {
        send_to_char("Sorry, you are not allowed to load rooms.<br/>", ch);
        return;
    }

    if (CHAR_LEVEL(ch) < fi->getZone()->getLevelRequiredToLoadItems())
    {
        if (!fi->getZone()->getCreators().IsName(UNIT_NAME(ch)))
        {
            int i = fi->getZone()->getLevelRequiredToLoadItems();

            act("Level $2d is required to load items from this zone.", A_ALWAYS, ch, &i, cActParameter(), TO_CHAR);
            return;
        }
    }

    u = read_unit(fi);

    if (IS_MONEY(u))
    {
        if (!IS_ADMINISTRATOR(ch))
        {
            send_to_char("No you don't.<br/>", ch);
            extract_unit(u);
            return;
        }

        set_money(u, MONEY_AMOUNT(u));
    }

    if (IS_OBJ(u) && IS_SET(UNIT_MANIPULATE(u), MANIPULATE_TAKE))
    {
        unit_to_unit(u, ch);
        act("You secretly load $2n.", A_SOMEONE, ch, u, cActParameter(), TO_CHAR);
    }
    else
    {
        unit_to_unit(u, UNIT_IN(ch));
        act("$1n opens an interdimensional gate and fetches $3n.", A_SOMEONE, ch, cActParameter(), u, TO_ROOM);
        act("$1n says, 'Hello World!'", A_SOMEONE, u, cActParameter(), cActParameter(), TO_ROOM);
    }
    dil_loadtime_activate(u);
}

/*  0: free access
 * >0: locked for below this level
 */
void do_wizlock(unit_data *ch, char *arg, const command_info *cmd)
{
    int lvl = 0;
    char buf[128];

    arg = one_argument(arg, buf);

    if (*buf)
    {
        lvl = atoi(buf) + 1;
    }
    else
    {
        lvl = GOD_LEVEL;
    }

    if (lvl >= CHAR_LEVEL(ch))
    {
        lvl = CHAR_LEVEL(ch);
    }
    if (lvl == 0)
    {
        lvl = 1;
    }

    if (g_wizlock && !*buf)
    {
        send_to_char("Game is no longer wizlocked.<br/>", ch);
        g_wizlock = 0;
    }
    else
    {
        auto msg = diku::format_to_str("Game is now wizlocked for level %d%s.<br/>", lvl - 1, lvl - 1 > 0 ? " and down" : "");
        send_to_char(msg, ch);
        g_wizlock = lvl;
    }
}
