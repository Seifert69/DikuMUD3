/*
 $Author: All $
 $RCSfile: act_wizard.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.9 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "db_file.h"
#include "spells.h"
#include "vmelimits.h"
#include "affect.h"
#include "magic.h"
#include "common.h"
#include "utility.h"
#include "money.h"
#include "system.h"
#include "files.h"
#include "movement.h"
#include "main.h"
#include "dilrun.h"
#include "event.h"
/*   external vars  */

extern struct zone_info_type zone_info;
extern class unit_data *unit_list;
extern class descriptor_data *descriptor_list;

/* external functs */

struct time_info_data age(class unit_data *ch);
struct time_info_data real_time_passed(time_t t2, time_t t1);
class zone_type *find_zone(char *zonename);

class descriptor_data *find_descriptor(const char *,
                                       class descriptor_data *);
int player_exists(const char *pName);
int delete_player(const char *name);

void do_timewarp(class unit_data *ch, char *argument, const struct command_info *cmd)
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

    char buf[256];
    sprintf(buf, "Time warping for %d seconds<br/>", i);
    send_to_char(buf, ch);

    void timewarp_end(void *p1, void *p2);
    extern eventqueue events;

    events.add(PULSE_SEC * i, timewarp_end, 0, 0);

    extern long g_nTickUsec;
    g_nTickUsec = 0; // Warp the time
}

void do_users(class unit_data *ch, char *argument,
              const struct command_info *cmd)
{
    char *buf = NULL;
    int cur_size = 1024;

    int available_connections(void);

    class descriptor_data *d;
    char tmp[256];
    int len, users = 0;

    if (buf == NULL)
        CREATE(buf, char, cur_size);

    strcpy(buf, "<u>Connections:</u><br/>");
    len = strlen(buf);
    /*
       <I%3d/%3d> immort lvl / wizi
       < %3d>     mortal vlvl
       < %3d/%3d> mortal vlvl/ wizi
    */

    for (d = descriptor_list; d; d = d->next)
    {
        assert(d->character);
        if (CHAR_LEVEL(ch) >= UNIT_MINV(CHAR_ORIGINAL(d->character)))
        {
            users++;
            if (IS_IMMORTAL(d->character))
            {
                /* an immortal character */
                sprintf(tmp, "&lt;I%3d/%3d&gt; %-16s %-10s [%c %4d %-3s %s]<br/>",
                        CHAR_LEVEL(CHAR_ORIGINAL(d->character)),
                        UNIT_MINV(CHAR_ORIGINAL(d->character)),
                        UNIT_NAME(CHAR_ORIGINAL(d->character)),
                        descriptor_is_playing(d) ? "Playing" : "Menu",
                        g_cServerConfig.FromLAN(d->host) ? 'L' : 'W',
                        d->nPort, d->nLine == 255 ? "---" : itoa(d->nLine),
                        d->host);
            }
            else
            {
                /* a mortal character */
                sprintf(tmp, "&lt; %6d%c&gt; %-16s %-10s [%c %4d %-3s %s]<br/>",
                        PC_VIRTUAL_LEVEL(CHAR_ORIGINAL(d->character)),
                        UNIT_MINV(CHAR_ORIGINAL(d->character)) ? '*' : ' ',
                        UNIT_NAME(CHAR_ORIGINAL(d->character)),
                        descriptor_is_playing(d) ? "Playing" : "Menu",
                        g_cServerConfig.FromLAN(d->host) ? 'L' : 'W',
                        d->nPort, d->nLine == 255 ? "---" : itoa(d->nLine),
                        d->host);
            }

            len += strlen(tmp);
            if (cur_size < len + 1)
            {
                cur_size *= 2;
                RECREATE(buf, char, cur_size);
            }
            strcat(buf, tmp);
        }
    }

    sprintf(tmp, "<br/>%d visible players connected.<br/>", users);

    len += strlen(tmp);
    if (cur_size < len + 1)
    {
        cur_size *= 2;
        RECREATE(buf, char, cur_size);
    }

    strcat(buf, tmp);

    page_string(CHAR_DESCRIPTOR(ch), buf);
    FREE(buf);
}

/* Reset the zone in which the char is in! */
void do_reset(class unit_data *ch, char *arg, const struct command_info *cmd)
{
    class zone_type *zone;

    int zone_reset(class zone_type *);

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

void do_at(class unit_data *ch, char *argument, const struct command_info *cmd)
{
    char buf[MAX_INPUT_LENGTH];
    class unit_data *target, *original_loc;
    class file_index_type *fi;

    if (!IS_PC(ch))
        return;

    if (str_is_empty(argument))
    {
        send_to_char("You must supply a unit name or zone reference.<br/>", ch);
        return;
    }

    if ((fi = pc_str_to_file_index(ch, argument)) && (!fi->fi_unit_list.empty()) && (fi->type == UNIT_ST_ROOM))
    {
        target = fi->fi_unit_list.front();
        argument = one_argument(argument, buf);
    }
    else
    {
        if ((target = find_unit(ch, &argument, 0, FIND_UNIT_WORLD)))
            if (UNIT_IN(target))
                target = UNIT_IN(target);
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

void do_crash(class unit_data *ch, char *argument,
              const struct command_info *cmd)
{
    if (cmd_is_abbrev(ch, cmd))
    {
        send_to_char("If you want to crash the game - say so!<br/>", ch);
        return;
    }

    if (strcmp(argument, "the entire game..."))
        send_to_char("You must type 'crash the entire game...'<br/>", ch);
    else
        assert(FALSE); /* Bye bye */
}

void do_execute(class unit_data *ch, char *argument,
                const struct command_info *cmd)
{
    int system_check(class unit_data * pc, char *buf);
    void execute_append(class unit_data * pc, char *str);

    argument = skip_spaces(argument);

    if (!system_check(ch, argument))
        return;

    execute_append(ch, argument);
    act("Executing $2t.", A_ALWAYS, ch, argument, cActParameter(), TO_CHAR);
}

void do_shutdown(class unit_data *ch, char *argument,
                 const struct command_info *cmd)
{
    char buf[100];
    extern int mud_shutdown;

    if (!IS_PC(ch))
        return;

    if (cmd_is_abbrev(ch, cmd))
    {
        send_to_char("If you want to shut something down - say so!<br/>", ch);
        return;
    }

    sprintf(buf, "Shutdown by %s.<br/>", UNIT_NAME(ch));
    send_to_all(buf);
    mud_shutdown = 1;
}

void do_snoop(class unit_data *ch, char *argument,
              const struct command_info *cmd)
{
    class unit_data *victim;

    void unsnoop(class unit_data * ch, int mode);
    void snoop(class unit_data * ch, class unit_data * victim);

    if (!CHAR_DESCRIPTOR(ch))
        return;

    if (str_is_empty(argument))
        victim = ch;
    else
        victim = find_unit(ch, &argument, 0, FIND_UNIT_WORLD);

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
            unsnoop(ch, 0); /* Unsnoop just ch himself */
        else
            send_to_char("You are already snooping yourself.<br/>", ch);
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
        unsnoop(ch, 0); /* Unsnoop just ch himself */

    snoop(ch, victim);
}

/* return -> switch <no_arg> */

void do_switch(class unit_data *ch, char *argument, const struct command_info *cmd)
{
    class unit_data *victim;

    void switchbody(class unit_data * ch, class unit_data * victim);
    void unswitchbody(class unit_data * npc);

    if (!CHAR_DESCRIPTOR(ch))
        return;

    if (str_is_empty(argument))
    {
        if (CHAR_IS_SWITCHED(ch))
            unswitchbody(ch);
        else
            send_to_char("You are already home in your good old body.<br/>", ch);
        return;
    }

    victim = find_unit(ch, &argument, 0, FIND_UNIT_WORLD | FIND_UNIT_SURRO);

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
        act("$3n's body is already in use!", A_ALWAYS, ch, cActParameter(), victim, TO_CHAR);
    else
    {
        send_to_char("Ok.<br/>", ch);
        switchbody(ch, victim);
    }
}

void do_load(class unit_data *ch, char *arg, const struct command_info *cmd)
{
    char buf[MAX_INPUT_LENGTH];
    class file_index_type *fi;
    class unit_data *u, *tmp;

    void reset_char(class unit_data * ch);
    void enter_game(class unit_data * ch, int dilway = FALSE);

    if (str_is_empty(arg))
    {
        send_to_char("Load? Load what?<br/>", ch);
        return;
    }

    arg = one_argument(arg, buf);

    if (find_descriptor(buf, NULL))
    {
        send_to_char("A player by that name is connected.<br/>", ch);
        return;
    }

    if ((fi = pc_str_to_file_index(ch, buf)) == NULL)
    {
        for (tmp = unit_list; tmp; tmp = tmp->gnext)
            if (IS_PC(tmp) && !str_ccmp(UNIT_NAME(tmp), buf))
            {
                send_to_char("A player by that name is linkdead in the game.<br/>", ch);
                return;
            }

        if (player_exists(buf))
        {
            if ((u = load_player(buf)) == NULL)
            {
                send_to_char("Load error<br/>", ch);
                return;
            }

            enter_game(u, TRUE);

            unit_from_unit(u);
            unit_to_unit(u, UNIT_IN(ch));
            send_to_char("You have loaded the player.<br/>", ch);

            if (UNIT_CONTAINS(u))
                send_to_char("Inventory loaded.<br/>", ch);

            if (CHAR_LEVEL(u) > CHAR_LEVEL(ch))
                slog(LOG_EXTENSIVE, UNIT_MINV(ch),
                     "LEVEL: %s loaded %s when lower level.",
                     UNIT_NAME(ch), UNIT_NAME(u));
            return;
        }

        send_to_char("No such file index reference found.<br/>", ch);
        return;
    }

    if (fi->type == UNIT_ST_ROOM)
    {
        send_to_char("Sorry, you are not allowed to load rooms.<br/>", ch);
        return;
    }

    if (CHAR_LEVEL(ch) < fi->zone->loadlevel)
    {
        if (!fi->zone->creators.IsName(UNIT_NAME(ch)))
        {
            int i = fi->zone->loadlevel;

            act("Level $2d is required to load items from this zone.",
                A_ALWAYS, ch, &i, cActParameter(), TO_CHAR);
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
        act("$1n opens an interdimensional gate and fetches $3n.",
            A_SOMEONE, ch, cActParameter(), u, TO_ROOM);
        act("$1n says, 'Hello World!'", A_SOMEONE, u, cActParameter(), cActParameter(), TO_ROOM);
    }
    dil_loadtime_activate(u);
}

/*  0: free access
 * >0: locked for below this level
 */
void do_wizlock(class unit_data *ch, char *arg, const struct command_info *cmd)
{
    extern int wizlock;
    int lvl;
    char buf[128];

    arg = one_argument(arg, buf);

    if (*buf)
        lvl = atoi(buf) + 1;
    else
        lvl = GOD_LEVEL;

    if (lvl >= CHAR_LEVEL(ch))
        lvl = CHAR_LEVEL(ch);
    if (lvl == 0)
        lvl = 1;

    if (wizlock && !*buf)
    {
        send_to_char("Game is no longer wizlocked.<br/>", ch);
        wizlock = 0;
    }
    else
    {
        sprintf(buf, "Game is now wizlocked for level %d%s.<br/>", lvl - 1,
                lvl - 1 > 0 ? " and down" : "");
        send_to_char(buf, ch);
        wizlock = lvl;
    }
}
