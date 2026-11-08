/*
 $Author: All $
 $RCSfile: act_wizard.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.9 $
 */
#include "act_wizard.h"
#include "affect.h"
#include "combat.h"
#include "comm.h"
#include "common.h"
#include "db.h"
#include "dbfind.h"
#include "descriptor_data.h"
#include "dilrun.h"
#include "file_index_type.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "main_functions.h"
#include "mobact.h"
#include "money.h"
#include "nanny.h"
#include "pcsave.h"
#include "slog.h"
#include "textutil.h"
#include "unitfind.h"
#include "utils.h"
#include "zon_basis.h"
#include "zone_reset.h"
#include "zone_type.h"

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
        if (CHAR_LEVEL(ch) >= CHAR_ORIGINAL(d->getCharacter())->getLevelOfWizardInvisibility())
        {
            users++;
            if (IS_IMMORTAL(d->cgetCharacter()))
            {
                /* an immortal character */
                msg += diku::format_to_str("&lt;I%3d/%3d&gt; %-16s %-10s [%c %4d %-3s %s]<br/>",
                                           CHAR_LEVEL(CHAR_ORIGINAL(d->getCharacter())),
                                           CHAR_ORIGINAL(d->getCharacter())->getLevelOfWizardInvisibility(),
                                           CHAR_ORIGINAL(d->getCharacter())->getNames().Name(),
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
                                           PC_VIRTUAL_LEVEL(CHAR_ORIGINAL(d->getCharacter())),
                                           CHAR_ORIGINAL(d->getCharacter())->getLevelOfWizardInvisibility() ? '*' : ' ',
                                           CHAR_ORIGINAL(d->getCharacter())->getNames().Name(),
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


void do_at(unit_data *ch, char *argument, const command_info *cmd)
{
    char buf[MAX_INPUT_LENGTH];
    unit_data *target = nullptr;
    unit_data *original_loc = nullptr;
    file_index_type *fi = nullptr;

    if (!ch->isPC())
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
            if (target->getUnitIn())
            {
                target = target->getUnitIn();
            }
        }
    }

    if (!target)
    {
        send_to_char("No such place around.<br/>", ch);
        return;
    }

    if (!target->isRoom())
    {
        send_to_char("Sorry, you may only 'at' at a room!<br/>", ch);
        return;
    }

    original_loc = ch->getUnitIn();
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
        abort(); /* Bye bye, in any build: assert() is compiled out with NDEBUG */
    }
}

void do_execute(unit_data *ch, char *argument, const command_info *cmd)
{
    argument = (char *) skip_spaces(argument);

    if (!system_check(ch, argument))
    {
        return;
    }

    execute_append(ch, argument);
    act("Executing $2t.", eA_ALWAYS, ch, argument, cActParameter(), eTO_CHAR);
}

void do_shutdown(unit_data *ch, char *argument, const command_info *cmd)
{
    if (!ch->isPC())
    {
        return;
    }

    if (cmd_is_abbrev(ch, cmd))
    {
        send_to_char("If you want to shut something down - say so!<br/>", ch);
        return;
    }

    auto msg = diku::format_to_str("Shutdown by %s.<br/>", ch->getNames().Name());
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

    if (!victim->isChar())
    {
        send_to_char("That's not a player / monster!<br/>", ch);
        return;
    }

    if (!CHAR_DESCRIPTOR(victim))
    {
        act("$3n has no descriptor-link.", eA_SOMEONE, ch, cActParameter(), victim, eTO_CHAR);
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

    if (!victim || !victim->isNPC())
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
        act("$3n's body is already in use!", eA_ALWAYS, ch, cActParameter(), victim, eTO_CHAR);
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
        for (tmp = g_unit_list; tmp; tmp = tmp->getGlobalNext())
        {
            if (tmp->isPC() && !str_ccmp(tmp->getNames().Name(), buf))
            {
                send_to_char("A player by that name is linkdead in the game.<br/>", ch);
                return;
            }
        }

        if (player_exists(buf))
        {
            if (CHAR_LEVEL(ch) < PLAYER_ADMIN_LEVEL)
            {
                send_to_char("Loading a player takes a higher level.<br/>", ch);
                return;
            }

            if ((u = load_player(buf)) == nullptr)
            {
                send_to_char("Load error<br/>", ch);
                return;
            }

            enter_game(u, TRUE);

            unit_from_unit(u);
            unit_to_unit(u, ch->getUnitIn());
            send_to_char("You have loaded the player.<br/>", ch);

            if (u->getUnitContains())
            {
                send_to_char("Inventory loaded.<br/>", ch);
            }

            if (CHAR_LEVEL(u) > CHAR_LEVEL(ch))
            {
                slog(LOG_EXTENSIVE,
                     ch->getLevelOfWizardInvisibility(),
                     "LEVEL: %s loaded %s when lower level.",
                     ch->getNames().Name(),
                     u->getNames().Name());
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
        if (!fi->getZone()->getCreators().IsName(ch->getNames().Name()))
        {
            int i = fi->getZone()->getLevelRequiredToLoadItems();

            act("Level $2d is required to load items from this zone.", eA_ALWAYS, ch, &i, cActParameter(), eTO_CHAR);
            return;
        }
    }

    u = read_unit(fi);

    if (u == nullptr)
    {
        send_to_char("No such thing to load.<br/>", ch);
        return;
    }


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

    if (u->isObj() && IS_SET(u->getManipulate(), MANIPULATE_TAKE))
    {
        unit_to_unit(u, ch);
        act("You secretly load $2n.", eA_SOMEONE, ch, u, cActParameter(), eTO_CHAR);
    }
    else
    {
        unit_to_unit(u, ch->getUnitIn());
        act("$1n opens an interdimensional gate and fetches $3n.", eA_SOMEONE, ch, cActParameter(), u, eTO_ROOM);
        act("$1n says, 'Hello World!'", eA_SOMEONE, u, cActParameter(), cActParameter(), eTO_ROOM);
    }
    dil_loadtime_activate(u);
}

/**
 * @returns 0: free access<br>
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

/* ------------------------------------------------------------------ */
/* A game master's rooms: made while the game runs in the virtual      */
/* zone _gm, placed inside another room or linked with exits (set ...  */
/* dir-to), and gone at the next reboot.                               */

#define GM_ROOMS_MAX 100

static int g_gm_rooms_made = 0; // names gm1, gm2, ... are not reused before the reboot

static bool is_gm_room(unit_data *u)
{
    return u && u->isRoom() && u->getFileIndex() && u->getFileIndex()->getZone() == find_zone(g_gm_zone);
}

// "gm3", "gm3@_gm" or "here"
static unit_data *find_gm_room(unit_data *ch, const char *arg)
{
    if (str_ccmp(arg, "here") == 0)
    {
        return is_gm_room(unit_room(ch)) ? unit_room(ch) : nullptr;
    }

    char name[MAX_INPUT_LENGTH + 1];
    char zone[MAX_INPUT_LENGTH + 1];
    split_fi_ref(arg, zone, name);
    unit_data *u = find_symbolic(strchr(arg, '@') ? zone : g_gm_zone, name);
    return is_gm_room(u) ? u : nullptr;
}

// Where everything in a room goes when it's freed: the room it stands in, else where its first exit
// leads, else the crypt in Udgaard
static unit_data *gm_room_way_out(unit_data *room)
{
    if (room->getUnitIn())
    {
        return unit_room(room->getUnitIn());
    }

    for (int i = 0; i <= MAX_EXIT; i++)
    {
        if (ROOM_EXIT(room, i) && ROOM_EXIT(room, i)->getToRoom() && ROOM_EXIT(room, i)->getToRoom() != room)
        {
            return ROOM_EXIT(room, i)->getToRoom();
        }
    }

    return world_room("udgaard", "crypt");
}

static void gm_room_new(unit_data *ch, char *arg)
{
    char keyword[MAX_INPUT_LENGTH + 1];
    char word[MAX_INPUT_LENGTH + 1];

    arg = str_next_word(arg, keyword);
    arg = str_next_word(arg, word);

    if (!*keyword || (*word && str_ccmp(word, "in") != 0))
    {
        send_to_char("Usage: gmroom new <keyword> [in <room@zone>|here]<br/>", ch);
        return;
    }

    unit_data *in = nullptr;
    if (*word)
    {
        arg = str_next_word(arg, word);
        if (str_ccmp(word, "here") == 0)
        {
            in = unit_room(ch);
        }
        else
        {
            char name[MAX_INPUT_LENGTH + 1];
            char zone[MAX_INPUT_LENGTH + 1];
            split_fi_ref(word, zone, name);
            in = find_symbolic(zone, name);
        }

        if (in == nullptr || !in->isRoom())
        {
            send_to_char("No such room (room@zone, or here).<br/>", ch);
            return;
        }
    }

    int n = 0;
    for (unit_data *u = g_room_head; u && u->isRoom(); u = u->getGlobalNext())
    {
        n += is_gm_room(u);
    }
    if (n >= GM_ROOMS_MAX)
    {
        send_to_char(diku::format_to_str("There are %d rooms already; free some first.<br/>", n), ch);
        return;
    }

    zone_type *z = find_zone(g_gm_zone);
    auto name = diku::format_to_str("gm%d", ++g_gm_rooms_made);
    z->insertFileIndex(std::make_unique<file_index_type>(z, name.c_str(), UNIT_ST_ROOM));

    unit_data *room = new_unit_data(UNIT_ST_ROOM, find_file_index(g_gm_zone, name.c_str()));
    room->getNames().AppendName(keyword);
    room->setTitle(diku::format_to_str("Inside the %s", keyword));
    room->setDescriptionOfOutside(diku::format_to_str("A %s is here.", keyword));
    room->setUnitFlag(UNIT_FL_INDOORS);
    room->setCapacity(30000); // what vmc gives a room, so players fit when it's inside another
    room->setBaseWeight(10);
    room->setWeight(10);
    UROOM(room)->setLandscapeTerrain(SECT_INSIDE);
    insert_in_unit_list(room);

    if (in)
    {
        room->setManipulateFlag(MANIPULATE_ENTER);
        unit_to_unit(room, in);
        send_to_char(diku::format_to_str("%s@%s made, inside %s.<br/>", name, g_gm_zone, in->getFileIndexSymName()), ch);
    }
    else
    {
        send_to_char(diku::format_to_str("%s@%s made; give it exits with set room dir-to.<br/>", name, g_gm_zone), ch);
    }
}

static void gm_room_free(unit_data *ch, unit_data *room)
{
    for (unit_data *u = room->getUnitContains(); u; u = u->getNext())
    {
        if (u->isRoom())
        {
            send_to_char(diku::format_to_str("Free %s first: it's inside.<br/>", u->getFileIndexSymName()), ch);
            return;
        }
    }

    unit_data *to = gm_room_way_out(room);
    if (to == nullptr)
    {
        send_to_char("There's no way out to move its contents to.<br/>", ch);
        return;
    }

    // Everyone and everything inside, players' corpses and belongings too, goes to the way out
    while (room->getUnitContains())
    {
        unit_data *u = room->getUnitContains();

        if (u->isChar())
        {
            stop_fighting(u);
            act("The place fades around you, and you are elsewhere.", eA_ALWAYS, u, cActParameter(), cActParameter(), eTO_CHAR);
        }
        unit_from_unit(u);
        unit_to_unit(u, to);
        if (u->isChar())
        {
            act("$1n appears.", eA_HIDEINV, u, cActParameter(), cActParameter(), eTO_ROOM);
            if (u->isPC())
            {
                command_interpreter(u, "look");
            }
        }
    }

    // Nothing may point to it any more: exits, and where characters (also in the menu) last were
    for (unit_data *r = g_room_head; r && r->isRoom(); r = r->getGlobalNext())
    {
        for (int i = 0; i <= MAX_EXIT; i++)
        {
            if (ROOM_EXIT(r, i) && ROOM_EXIT(r, i)->getToRoom() == room)
            {
                delete ROOM_EXIT(r, i);
                UROOM(r)->setRoomDirectionDataForExitTo(i, nullptr);
            }
        }
    }
    for (unit_data *u = g_unit_list; u; u = u->getGlobalNext())
    {
        if (u->isChar() && CHAR_LAST_ROOM(u) == room)
        {
            UCHAR(u)->setLastLocation(nullptr);
        }
    }
    for (descriptor_data *d = g_descriptor_list; d; d = d->getNext())
    {
        if (d->getCharacter() && d->getCharacter()->isChar() && CHAR_LAST_ROOM(d->getCharacter()) == room)
        {
            UCHAR(d->getCharacter())->setLastLocation(nullptr);
        }
    }

    send_to_char(diku::format_to_str("%s freed; what was inside is in %s.<br/>", room->getFileIndexSymName(), to->getFileIndexSymName()), ch);

    DeactivateDil(room);
    stop_all_special(room);
    stop_affect(room);
    room->register_destruct();
    if (room->getUnitIn())
    {
        unit_from_unit(room);
    }
    remove_from_unit_list(room); // it's deleted at the end of this pulse (clear_destructed)
}

static void gm_room_list(unit_data *ch)
{
    std::string msg;

    for (unit_data *r = g_room_head; r && r->isRoom(); r = r->getGlobalNext())
    {
        if (!is_gm_room(r))
        {
            continue;
        }
        int pcs = 0;
        for (unit_data *u = r->getUnitContains(); u; u = u->getNext())
        {
            pcs += u->isPC();
        }
        msg += diku::format_to_str("%-8s %s, %s, %d player%s<br/>",
                                   r->getFileIndexName(),
                                   r->getTitle(),
                                   r->getUnitIn() ? ("inside " + r->getUnitIn()->getFileIndexSymName()).c_str() : "not inside a room",
                                   pcs,
                                   pcs == 1 ? "" : "s");
    }

    send_to_char(msg.empty() ? "No rooms made.<br/>" : msg, ch);
}

void do_gmroom(unit_data *ch, char *arg, const command_info *cmd)
{
    char word[MAX_INPUT_LENGTH + 1];
    arg = str_next_word(arg, word);

    if (str_ccmp(word, "new") == 0)
    {
        gm_room_new(ch, arg);
    }
    else if (str_ccmp(word, "free") == 0)
    {
        str_next_word(arg, word);
        unit_data *room = find_gm_room(ch, word);
        if (room == nullptr)
        {
            send_to_char("No such room: gmroom free <gm3|gm3@_gm|here><br/>", ch);
            return;
        }
        gm_room_free(ch, room);
    }
    else if (str_ccmp(word, "list") == 0)
    {
        gm_room_list(ch);
    }
    else if (str_ccmp(word, "title") == 0 || str_ccmp(word, "descr") == 0 || str_ccmp(word, "outside") == 0)
    {
        // One line each, no editor: the title and what you see inside it, and how it looks from the room around it
        char which[MAX_INPUT_LENGTH + 1];
        strcpy(which, word);
        arg = str_next_word(arg, word);
        unit_data *room = find_gm_room(ch, word);
        if (room == nullptr || str_is_empty(arg))
        {
            send_to_char("Usage: gmroom title|descr|outside <gm3|here> <text><br/>", ch);
            return;
        }
        if (str_ccmp(which, "title") == 0)
        {
            room->setTitle(arg);
        }
        else if (str_ccmp(which, "descr") == 0)
        {
            room->setDescriptionOfInside(arg);
        }
        else
        {
            room->setDescriptionOfOutside(arg);
        }
        send_to_char("Ok.<br/>", ch);
    }
    else
    {
        send_to_char("Usage: gmroom new <keyword> [in <room@zone>|here]<br/>"
                     "       gmroom title|descr|outside <gm3|here> <text><br/>"
                     "       gmroom free <gm3|here><br/>"
                     "       gmroom list<br/>",
                     ch);
    }
}
