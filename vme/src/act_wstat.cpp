/*
 $Author: All $
 $RCSfile: act_wstat.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.10 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WINDOWS
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

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
#include "utility.h"
#include "files.h"
#include "common.h"
#include "account.h"
#include "constants.h"
#include "main.h"

/*   external vars  */
extern struct zone_info_type zone_info;

/* external functs */
struct time_info_data age(class unit_data *ch);
struct time_info_data real_time_passed(time_t t2, time_t t1);

extern void stat_bank(const class unit_data *ch, class unit_data *u); /* bank.c */
extern eventqueue events;


static void stat_world_count(const class unit_data *ch, char *arg)
{
    char buf[MAX_STRING_LENGTH];
    std::string mystr;
    class unit_data *u, *t;
    int i;
    int n;

    int nMinCount = atoi(arg);

    sprintf(buf, "The first 40 units containing at least %d units:<br/>", nMinCount);
    mystr = buf;

    n = 0;
    for (u = unit_list; u; u = u->gnext)
    {
        i = 0;
        for (t = UNIT_CONTAINS(u); t; t = t->next) // count top layer
            i++;

        if (i >= nMinCount)
        {
            sprintf(buf, "%s@%s(%s) : %d units <br/>", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u), UNIT_NAME(u), i);
            mystr.append(buf);
            n++;

            if (n >= 40)
                break;
        }

    }

    mystr.append("<br/>");

    page_string(CHAR_DESCRIPTOR(ch), mystr.c_str());
}


static void stat_world_extra(const class unit_data *ch)
{
    char buf[MAX_STRING_LENGTH];
    class zone_type *zp;
    int i;
    std::string mystr;

    mystr = "<div class='fourcol'>";
    sprintf(buf, "World zones (%d):<br/>", zone_info.no_of_zones);
    mystr.append(buf);

    for (i = 1, zp = zone_info.zone_list; zp; zp = zp->next, i++)
    {
        sprintf(buf, "%s<br/>", zp->name);
        mystr.append(buf);
    }
    mystr.append("</div><br/>");

    page_string(CHAR_DESCRIPTOR(ch), mystr.c_str());
}

static void stat_memory(class unit_data *ch)
{
    char buf[MAX_STRING_LENGTH];

    void memory_status(char *buf);
    void system_memory(class unit_data * ch);

    sprintf(buf, "Event queue entries: %d<br/>", events.Count());
    send_to_char(buf, ch);
    sprintf(buf, "Current Tic=%d , Event Queue Tic=%d<br/>", tics, events.NextEventTic());
    send_to_char(buf, ch);
    sprintf(buf, "Event queue GL Stats ( Proceses/Sec ): %d/%1.4f<br/>", events.PCount(), events.PTime());
    send_to_char(buf, ch);
    sprintf(buf, "Event queue Max Time  ( Proceses/Sec ): %d/%1.4f<br/>", events.Max_PTime_Process(), events.Max_PTime());
    send_to_char(buf, ch);
    sprintf(buf, "Event queue Max Process  ( Proceses/Sec ): %d/%1.4f<br/>", events.Max_PCount(), events.Max_PCount_Time());
    send_to_char(buf, ch);
    sprintf(buf, "Event queue Avg Process  ( Proceses/Sec ): %1.1f/%1.4f<br/>", events.Avg_PCount(), events.Avg_PTime());
    send_to_char(buf, ch);
    //  system_memory (ch);
    //  memory_status (buf);
    //  send_to_char (buf, ch);
}

static void stat_world(class unit_data *ch)
{
    extern int world_norooms, world_noobjects, world_nochars, world_nozones;
    extern int world_nonpc, world_nopc;
    extern char world_boottime[64];
    extern int tics;
    //extern int g_nDilPrg;
    char buf[MAX_STRING_LENGTH];
    time_t now = time(0);

    char *p = ctime(&now);
    p[strlen(p) - 1] = '\0';

    sprintf(buf, "Server compiled at %s %s<br/>"
                 "World status (tick %d aka %d hours):<br/>"
                 "#rooms [%4d]  #objects [%4d]  #chars [%4d]  #npcs [%4d] "
                 "  #pcs [%4d]<br/>"
                 "#units [%4d]  #zones   [%4d]<br/>"
                 "#connections [%2d / peak %2d]<br/>"
                 "#players     [%2d / peak %2d]<br/>"
                 "Boottime: %s<br/>Time now: %s<br/>"
                 "DIL programs [%d]   DIL Vals [%d]<br>",
            compile_date, compile_time,
            tics, tics / (PULSE_SEC * 3600),
            world_norooms, world_noobjects, world_nochars,
            world_nonpc, world_nopc,
            world_norooms + world_noobjects + world_nochars,
            world_nozones,
            no_connections, max_no_connections,
            no_players, max_no_players, world_boottime, p, g_nDilPrg, g_nDilVal);

    page_string(CHAR_DESCRIPTOR(ch), buf);
}

static char *stat_buffer, *stat_p;

static void stat_zone_reset(const char *indnt, struct zone_reset_cmd *zrip,
                            class unit_data *ch)
{
    static const char *nums[] = {"max", "zonemax", "local"};

    char buf[100];
    int i;

    *stat_p = 0;

    STRCAT(stat_p, indnt);

    switch (zrip->cmd_no)
    {
    case 0:
        strcpy(stat_p, "Nop");
        break;

    case 1:
        sprintf(stat_p, "Load %s", zrip->fi[0]->name);
        TAIL(stat_p);

        if (zrip->fi[1])
        {
            sprintf(stat_p, " into %s", zrip->fi[1]->name);
            TAIL(stat_p);
        }

        for (i = 0; i < 3; i++)
            if (zrip->num[i])
            {
                sprintf(stat_p, " %s %d", nums[i], zrip->num[i]);
                TAIL(stat_p);
            }

        strcpy(stat_p, zrip->cmpl ? " Complete" : "");
        break;

    case 2:
        sprintf(stat_p, "Equip %s %s max %d %s",
                zrip->fi[0]->name, where[zrip->num[1]],
                zrip->num[0], zrip->cmpl ? "Complete" : "");
        break;

    case 3:
        sprintf(stat_p, "Door at %s : %s : %s",
                zrip->fi[0]->name, dirs[zrip->num[0]],
                sprintbit(buf, zrip->num[1], unit_open_flags));
        break;

    case 4:
        sprintf(stat_p, "Purge %s", zrip->fi[0]->name);
        break;

    case 5:
        sprintf(stat_p, "Remove %s in %s", zrip->fi[0]->name,
                zrip->fi[1]->name);
        break;
    }

    STRCAT(stat_p, "<br/>");

    /* make sure we get no overflow */
    if (stat_p - stat_buffer + MAX_INPUT_LENGTH > 2 * MAX_STRING_LENGTH)
    {
        STRCAT(stat_p, "Truncated .....<br/>");
        return;
    }

    if (zrip->nested)
    {
        char whitespace[MAX_INPUT_LENGTH];

        sprintf(stat_p, "%s{<br/>", indnt);
        TAIL(stat_p);

        sprintf(whitespace, "%s  ", indnt);
        stat_zone_reset(whitespace, zrip->nested, ch);

        sprintf(stat_p, "%s}<br/>", indnt);
        TAIL(stat_p);
    }
    if (zrip->next)
        stat_zone_reset(indnt, zrip->next, ch);
}

static void stat_zone(class unit_data *ch, class zone_type *zone)
{
    static const char *reset_modes[] = {
        "Never Reset",
        "Reset When Empty",
        "Reset Always",
        "UNKNOWN"};
    char *cname;

    char tmp[128], buf[MAX_STRING_LENGTH];
    bool errors, info;
    int reset_mode = zone->reset_mode;

    if (!is_in(reset_mode, 0, 2))
        reset_mode = 3;

    sprintf(tmp, "%s%s.err", g_cServerConfig.m_zondir, zone->filename);
    errors = file_exists(tmp);

    sprintf(tmp, "%s%s.inf", g_cServerConfig.m_zondir, zone->filename);
    info = file_exists(tmp);

    cname = zone->creators.catnames();

    sprintf(buf, "Zone [%s]  File [%s]  Access [%d]<br/>"
                 "Title: \"%s\"<br/>"
                 "Load level [%d] Pay only [%d]<br/>"
                 "Number of Units [%d]    Number of Rooms [%d]<br/>"
                 "Reset Mode : %s (%d)    Reset Interval [%d]<br/>"
                 "Pressure [%d] Change [%d] Sky [%d] Base [%d]<br/><br/>"
                 "Authors Mud Mail: %s<br/><br/>%s<br/><br/>"
                 "%s<br/>%s<br/>",
            zone->name, zone->filename, zone->access,
            zone->title ? zone->title : "",
            zone->loadlevel, zone->payonly,
            zone->no_of_fi, zone->no_rooms,
            reset_modes[reset_mode], reset_mode, zone->zone_time,
            zone->weather.pressure, zone->weather.change,
            zone->weather.sky, zone->weather.base,
            cname, zone->notes,
            errors ? "Errors in zone (stat zone error)" : "No errors registered in zone.",
            info ? "User info feedback in zone (stat zone info)." : "No user info (feedback) in zone.");
    FREE(cname)
    page_string(CHAR_DESCRIPTOR(ch), buf);
}

static void stat_creators(class unit_data *ch, char *arg)
{
    char buf[4 * MAX_STRING_LENGTH], *b;
    char tmp[1024];
    int found;
    char *cname;
    class zone_type *z;
    if (str_is_empty(arg))

    {
        send_to_char("Requires name of creator.<br/>", ch);
        return;
    }

    b = buf;

    arg = one_argument(arg, tmp);

    if (str_ccmp(tmp, "all") == 0)
    {
        sprintf(b, "List of all Zones with Creators.<br/><br/>");
        TAIL(b);

        found = FALSE;

        for (z = zone_info.zone_list; z; z = z->next)
        {
            cname = z->creators.catnames();

            sprintf(b, "%-15s   %s<br/>", z->name, cname);
            FREE(cname);
            TAIL(b);
            found = TRUE;
        }

        page_string(CHAR_DESCRIPTOR(ch), buf);
        return;
    }

    sprintf(b, "Zones Created by %s.<br/><br/>", tmp);
    TAIL(b);

    found = FALSE;
    for (z = zone_info.zone_list; z; z = z->next)
    {
        if (z->creators.IsName(tmp))
        {
            sprintf(b, "%-15s   File: %s.zon<br/>", z->name, z->filename);
            TAIL(b);
            found = TRUE;
        }
    }

    if (!found)
        sprintf(b, "None.<br/>");

    TAIL(b);

    send_to_char(buf, ch);
}

static void stat_dil(class unit_data *ch, class zone_type *zone)
{
    char buf[MAX_STRING_LENGTH];
    std::string mystr;
    struct diltemplate *tmpl;

    sprintf(buf, "<u>List of DIL in zone %s (CPU secs, name, #activations, #instructions):</u><br/>", zone->name);
    send_to_char(buf, ch);

    mystr = "<div class='twocol'>";
    for (*buf = 0, tmpl = zone->tmpl; tmpl; tmpl = tmpl->next)
    {
        sprintf(buf, "%.2fs %s [%d t / %d i]<br/>", tmpl->fCPU/1000.0, tmpl->prgname, tmpl->nTriggers, tmpl->nInstructions);
        mystr.append(buf);
    }

    mystr.append("</div><br/>"); //MS2020
    send_to_char(mystr.c_str(), ch);
}

static void stat_global_dil(class unit_data *ch, ubit32 nCount)
{
    char buf[MAX_STRING_LENGTH];
    std::string mystr;
    struct diltemplate *tmpl;
    class zone_type *z;

    sprintf(buf, "<u>List of global DIL in all zones running for more than %dms:</u><br/>", nCount);
    send_to_char(buf, ch);

    mystr = "<div class='twocol'>";
    for (z = zone_info.zone_list; z; z = z->next)
    {
        for (*buf = 0, tmpl = z->tmpl; tmpl; tmpl = tmpl->next)
        {
            if (tmpl->fCPU >= nCount){
                sprintf(buf, "%.2fs %s@%s [%d t / %d i]<br/>", tmpl->fCPU/1000.0, tmpl->prgname, tmpl->zone->name, tmpl->nTriggers, tmpl->nInstructions);
                mystr.append(buf);
            }
        }        
    }

    mystr.append("</div><br/>"); //MS2020
    send_to_char(mystr.c_str(), ch);
}

static void extra_stat_zone(class unit_data *ch, char *arg, class zone_type *zone)
{
    char buf[MAX_STRING_LENGTH], filename[128];
    std::string mystr;
    int argno;
    class file_index_type *fi;
    int search_type = 0, i;

    //  void stat_dijkstraa (class unit_data * ch, class zone_type *z);

    static const char *zone_args[] = {
        "mobiles",
        "objects",
        "rooms",
        "reset",
        "errors",
        "info",
        "path",
        "dil",
        NULL};

    static int search_types[] = {
        UNIT_ST_NPC,
        UNIT_ST_OBJ,
        UNIT_ST_ROOM};

    arg = one_argument(arg, buf);
    argno = search_block(buf, zone_args, 0);

    if (argno == -1) /* Asked for a specific zone? */
    {
        if ((zone = find_zone(buf)) == NULL)
        {
            send_to_char("Usage: wstat zone [name] "
                         "[mobiles|objects|rooms|reset|info|errors|path|dil]<br/>", ch);
            return;
        }

        arg = one_argument(arg, buf);
        argno = search_block(buf, zone_args, 0);

        if (argno == -1)
        {
            stat_zone(ch, zone);
            return;
        }
    }

    switch (argno)
    {
    case 0:
    case 1:
    case 2:
        sprintf(buf, "List of %s in zone %s:<br/>", zone_args[argno],
                zone->name);
        send_to_char(buf, ch);
        search_type = search_types[argno];
        break;

    case 3:
        if (zone->zri)
        {
            sprintf(buf, "Reset information for zone %s:<br/>", zone->name);
            send_to_char(buf, ch);
            CREATE(stat_buffer, char, 2 * MAX_STRING_LENGTH);
            stat_p = stat_buffer;
            stat_zone_reset("", zone->zri, ch);
            page_string(CHAR_DESCRIPTOR(ch), stat_buffer);
            FREE(stat_buffer);
        }
        else
        {
            sprintf(buf, "No reset information for zone %s.<br/>", zone->name);
            send_to_char(buf, ch);
        }
        return;

    case 4:
    case 5:
        /* Errors/Info (Small hack, this :-) ) */
        sprintf(filename, "%s%s.%.3s", g_cServerConfig.m_zondir,
                zone->filename, zone_args[argno]);
        if (!file_exists(filename))
            return;
        file_to_string(filename, buf, MAX_STRING_LENGTH);
        page_string(CHAR_DESCRIPTOR(ch), buf);
        return;

    case 6:
        page_string(CHAR_DESCRIPTOR(ch), "Not Implemented");
        //      stat_dijkstraa (ch, zone);
        break;

    case 7:
        stat_dil(ch, zone);
        break;

    default:
        return;
    }

    /* Search for mobs/objs/rooms and line in columns */
    mystr = "";
    for (*buf = 0, fi = zone->fi, i = 0; fi; fi = fi->next)
        if (fi->type == search_type)
        {
            sprintf(buf, "%-20s", fi->name);
            mystr.append(buf); //MS2020

            /* Enough to fill a whole line */
            if (++i == 4)
            {
                mystr.append("<br/>"); // MS2020
                send_to_char(mystr.c_str(), ch);
                *buf = 0;
                mystr = "";
                i = 0;
            }
        }
    if (i)
    {
        mystr.append("<br/>"); // MS2020 quick hack
        send_to_char(mystr.c_str(), ch);
    }
}

static void stat_ability(const class unit_data *ch, class unit_data *u)
{
    char buf[MAX_STRING_LENGTH], *b = buf;
    int i;

    if (!IS_PC(u))
    {
        send_to_char("Unit is not a PC - use 'data' for NPC's<br/>", ch);
        return;
    }

    strcpy(b, "Char Abilities:<br/>");
    TAIL(b);

    for (i = 0; i < ABIL_TREE_MAX; i++)
    {
        sprintf(b, "%20s : %3d%% Lvl %3d Racial %3d<br/>",
                abil_text[i],
                CHAR_ABILITY(u, i), PC_ABI_LVL(u, i), get_racial_ability(CHAR_RACE(u), i));
        TAIL(b);
    }

    page_string(CHAR_DESCRIPTOR(ch), buf);
}

static void stat_spell(const class unit_data *ch, class unit_data *u)
{
    char tmpbuf1[100];
    char tmpbuf2[110];
    char buf[100 * (SPL_TREE_MAX + 1)], *b = buf;
    int i, max;

    extern struct spell_info_type spell_info[SPL_TREE_MAX];

    if (!IS_CHAR(u))
    {
        send_to_char("Unit is not a char<br/>", ch);
        return;
    }

    strcpy(b, "Char magic skill<br/>");
    TAIL(b);

    max = IS_NPC(u) ? SPL_GROUP_MAX : SPL_TREE_MAX;

    for (i = 0; i < max; i++)
    {
        str_next_word(TREE_ISLEAF(spl_tree, i) ? spl_text[TREE_PARENT(spl_tree, i)] : "sphere", tmpbuf1);

        if (TREE_ISLEAF(spl_tree, i) && strcmp(tmpbuf1, "sphere") == 0)
            if (spell_info[i].tmpl == NULL && spell_info[i].spell_pointer == NULL)
                strcpy(tmpbuf1, "NOT IMPLEMENTED");

        sprintf(tmpbuf2, "%s %s (%s)",
                spell_info[i].cast_type == SPLCST_CHECK ? "CHECK " : (spell_info[i].cast_type == SPLCST_RESIST ? "RESIST" : "OTHER "), spl_text[i], tmpbuf1);

        sprintf(b, "%c%c%c%c%c] %3d%%/%d %c %-50s [%3d racial]<br/>",
                IS_SET(spell_info[i].media, MEDIA_SPELL) ? 'C' : '-',
                IS_SET(spell_info[i].media, MEDIA_SCROLL) ? 'S' : '-',
                IS_SET(spell_info[i].media, MEDIA_POTION) ? 'P' : '-',
                IS_SET(spell_info[i].media, MEDIA_WAND) ? 'W' : '-',
                IS_SET(spell_info[i].media, MEDIA_STAFF) ? 'R' : '-',
                IS_NPC(u) ? NPC_SPL_SKILL(u, i) : PC_SPL_SKILL(u, i),
                IS_NPC(u) ? 0 : PC_SPL_LVL(u, i),
                spell_info[i].realm == ABIL_DIV ? 'D' : (spell_info[i].realm == ABIL_MAG ? 'M' : '!'),
                tmpbuf2, get_racial_spells(CHAR_RACE(u), i));
        TAIL(b);
    }

    page_string(CHAR_DESCRIPTOR(ch), buf);
    assert(strlen(buf) < sizeof(buf));
}

static void stat_skill(const class unit_data *ch, class unit_data *u)
{
    if (!IS_CHAR(u))
        send_to_char("Unit is not a char<br/>", ch);
    else if (IS_NPC(u))
        send_to_char("NPC's have no skills.<br/>", ch);
    else
    {
        char buf[100 * (SKI_TREE_MAX + 1)], *b = buf;
        int i;

        strcpy(b, "Other skills:<br/>");
        TAIL(b);

        for (i = 0; i < SKI_TREE_MAX; i++)
        {
            sprintf(b, "%20s: %3d%% Lvl %3d Racial %3d<br/>",
                    ski_text[i], PC_SKI_SKILL(u, i),
                    PC_SKI_LVL(u, i), get_racial_skill(CHAR_RACE(u), i));
            TAIL(b);
        }

        page_string(CHAR_DESCRIPTOR(ch), buf);
    }
}

static void stat_wskill(const class unit_data *ch, class unit_data *u)
{
    char buf[100 * (WPN_TREE_MAX + 1)], *b = buf;
    int i, max;

    if (!IS_CHAR(u))
    {
        send_to_char("Unit is not a char<br/>", ch);
        return;
    }

    strcpy(b, "Char weapon skill:<br/>");
    TAIL(b);

    max = IS_NPC(u) ? WPN_GROUP_MAX : WPN_TREE_MAX;

    for (i = 0; i < max; i++)
    {
        sprintf(b, "%20s : %3d%% Lvl %3d Racial %3d<br/>", wpn_text[i],
                IS_NPC(u) ? NPC_WPN_SKILL(u, i) : PC_WPN_SKILL(u, i),
                IS_NPC(u) ? 0 : PC_WPN_LVL(u, i),
                get_racial_weapon(CHAR_RACE(u), i));
        TAIL(b);
    }
    page_string(CHAR_DESCRIPTOR(ch), buf);
}

static void stat_affect(const class unit_data *ch, class unit_data *u)
{
    extern struct tick_function_type tif[];
    extern struct apply_function_type apf[];

    class unit_affected_type *af;
    char buf[1024];

    if (!UNIT_AFFECTED(u))
    {
        send_to_char("It is not affected by anything.<br/>", ch);
        return;
    }

    send_to_char("Unit affects:<br/>", ch);

    for (af = UNIT_AFFECTED(u); af; af = af->next)
    {
        sprintf(buf, "----------------------------------------------------<br/>"
                     "Id [%d]   Duration [%d]   Beat [%d] Data [%d] [%d] [%d]<br/>"
                     "First f() %s<br/>"
                     "Tick  f() %s<br/>"
                     "Last  f() %s<br/>"
                     "Apply f() %s<br/>"
                     "%s",
                af->id, af->duration, af->beat, af->data[0], af->data[1],
                af->data[2],
                af->firstf_i >=
                        0
                    ? STR(tif[af->firstf_i].descr)
                    : "Not used (-1)",
                af->tickf_i >=
                        0
                    ? STR(tif[af->tickf_i].descr)
                    : "Not used (-1)",
                af->lastf_i >=
                        0
                    ? STR(tif[af->lastf_i].descr)
                    : "Not used (-1)",
                af->applyf_i >=
                        0
                    ? STR(apf[af->applyf_i].descr)
                    : "Not used (-1)",
                u == af->owner ? "" : "Serious error in owner pointer!<br/>");

        send_to_char(buf, ch);
    }
}

static void stat_func(const class unit_data *ch, class unit_data *u)
{
    extern struct unit_function_array_type unit_function_array[];

    char buf[4096], buf2[512];
    class unit_fptr *f;

    if (!UNIT_FUNC(u))
    {
        send_to_char("It has no special routines.<br/>", ch);
        return;
    }

    send_to_char("Unit functions:<br/>", ch);
    for (f = UNIT_FUNC(u); f; f = f->next)

    {
        if (f->index == SFUN_DIL_INTERNAL)
        {
            class dilprg *prg;

            if ((prg = (class dilprg *)f->data))
            {
                sprintf(buf, "DIL Name: %s@%s<br/>",
                        prg->frame[0].tmpl->prgname,
                        prg->frame[0].tmpl->zone ? prg->frame[0].tmpl->zone->name : "IDLE");

                send_to_char(buf, ch);
            }
        }

        sprintf(buf, "[%3d] %s Flags [%s] Index [%d] Beat [%d]<br/>"
                     "%s<br/><br/>",
                f->priority,
                unit_function_array[f->index].name,
                sprintbit(buf2, f->flags, sfb_flags), f->index, f->heart_beat,
                f->data ? unit_function_array[f->index].save_w_d == SD_ASCII ? (char *)f->data : "Has raw data." : "No data.");
        send_to_char(buf, ch);
    }
}

static void stat_normal(class unit_data *ch, class unit_data *u)
{
    char buf[MAX_STRING_LENGTH], tmpbuf1[512], tmpbuf2[256];
    char *cname;
    /* Stat on the unit */

    cname = UNIT_NAMES(u).catnames(); /* Get names into tmpbuf1 */

    /* Even though type isn't a flag, we'd better show them all in case
     * more than one is set!
     */
    sprintf(buf,
            "Unit status: %s [%s@%s] %d copies (CRC %lu)<br/>Namelist: %s<br/>"
            "Title: \"%s\"<br/>Outside_descr:<br/>\"%s\"<br/>"
            "Inside_descr:<br/>\"%s\"<br/>",
            sprintbit(tmpbuf2, UNIT_TYPE(u), unit_status),
            UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u),
            UNIT_FILE_INDEX(u) ? UNIT_FILE_INDEX(u)->no_in_mem : -1,
            UNIT_FILE_INDEX(u) ? (unsigned long)UNIT_FILE_INDEX(u)->crc : 0,
            cname, STR(UNIT_TITLE_STRING(u)),
            STR(UNIT_OUT_DESCR_STRING(u)), STR(UNIT_IN_DESCR_STRING(u)));
    send_to_char(buf, ch);
    FREE(cname);
    sprintf(buf, "Lights: [%d]  Bright: [%d]  TrnIllu: [%d]  "
                 "Chars Within [%d] WIZI [%d]<br/>"
                 "Unit is inside: %s -- Unit %s<br/>"
                 "Manipulate: %s<br/>"
                 "Flags: %s<br/>"
                 "Hitpoints/max: [%ld/%ld]  Alignment: [%d]<br/>",
            UNIT_LIGHTS(u), UNIT_BRIGHT(u), UNIT_ILLUM(u), UNIT_CHARS(u),
            UNIT_MINV(u),
            UNIT_IN(u) ? STR(TITLENAME(UNIT_IN(u))) : "Nothing",
            UNIT_CONTAINS(u) ? "has contents" : "is empty",
            sprintbit(tmpbuf2, UNIT_MANIPULATE(u), unit_manipulate),
            sprintbit(tmpbuf1, UNIT_FLAGS(u), unit_flags),
            (signed long)UNIT_HIT(u),
            (signed long)UNIT_MAX_HIT(u), UNIT_ALIGNMENT(u));
    send_to_char(buf, ch);

    sprintf(buf,
            "Key name: [%s]  Open flags: %s  Open Diff: %d<br/>"
            "Base weight : [%d] Weight : [%d] Capacity : [%d] Size [%d]<br/>",
            UNIT_KEY(u) ? UNIT_KEY(u) : "none",
            sprintbit(tmpbuf1, UNIT_OPEN_FLAGS(u), unit_open_flags), UNIT_OPEN_DIFF(u),
            UNIT_BASE_WEIGHT(u), UNIT_WEIGHT(u), UNIT_CAPACITY(u),
            UNIT_SIZE(u));
    send_to_char(buf, ch);
}

static void stat_extra(const class unit_data *ch, class extra_list &elist, char *grp)
{
    bool found = false;
    char buf[4 * MAX_STRING_LENGTH];
    /* MS: We used to do a TAIL here... bad idea as newspaper is VERY HUGE */
    /* This isn't nice either, but it works... */
    char *cname;

    if (!elist.isempty())
    {
        one_argument(grp, buf);

        class extra_descr_data *ed;
        for (ed = elist.m_pList; ed; ed = ed->next)
        {
            if ((*buf) && ed->names.StrStr(grp))
            {
                if (!found)
                {
                    send_to_char("Extra descriptions:<br/>-------------------<br/>", ch);
                    found = true;
                }
                cname = ed->names.catnames();
                send_to_char("Names ", ch);
                send_to_char(cname, ch);
                FREE(cname);
                send_to_char("<br/>", ch);
                if (ed->vals.Length() > 0)
                {
                    cname = ed->vals.catnames();
                    send_to_char("Values ", ch);
                    send_to_char(cname, ch);
                    send_to_char("<br/>\"", ch);
                    FREE(cname);
                }
                else
                    send_to_char("\"", ch);
                send_to_char(ed->descr.c_str(), ch);
                send_to_char("\"<br/>-------------------<br/>", ch);
            }
            else if (!(*buf))
            {
                if (!found)
                {
                    send_to_char("Extra descriptions:<br/>-------------------<br/>", ch);
                    found = true;
                }
                cname = ed->names.catnames();
                send_to_char("Names ", ch);
                send_to_char(cname, ch);
                FREE(cname);
                send_to_char("<br/>", ch);
                if (ed->vals.Length() > 0)
                {
                    cname = ed->vals.catnames();
                    send_to_char("Values ", ch);
                    send_to_char(cname, ch);
                    send_to_char("<br/>\"", ch);
                    FREE(cname);
                }
                else
                    send_to_char("\"", ch);
                send_to_char(ed->descr.c_str(), ch);
                send_to_char("\"<br/>-------------------<br/>", ch);
            }
        }
    }

    if (!found)
        send_to_char("None.<br/>", ch);
}

static void stat_extra_descr(const class unit_data *ch, class unit_data *u, char *grp)
{
    stat_extra(ch, UNIT_EXTRA(u), grp);
}

static void stat_extra_quest(const class unit_data *ch, class unit_data *u, char *grp)
{
    if (IS_PC(u))
        stat_extra(ch, PC_QUEST(u), grp);
    else
        send_to_char("Quests only on Players.<br/>", ch);
}

static void stat_extra_info(const class unit_data *ch, class unit_data *u, char *grp)
{
    if (!IS_ADMINISTRATOR(ch))
    {
        send_to_char("This information is classified for administrators "
                     "only.<br/>",
                     ch);
        return;
    }

    if (IS_PC(u))
        stat_extra(ch, PC_INFO(u), grp);
    else
        send_to_char("Information is only on Players.<br/>", ch);
}

static void stat_ip(const class unit_data *ch, class unit_data *u)
{
    if (!IS_ADMINISTRATOR(ch))
    {
        send_to_char("This information is classified for administrators "
                     "only.<br/>",
                     ch);
        return;
    }

    if (IS_PC(u))
    {
        char buf[500];
        struct sockaddr_in sock;

        for (int i = 0; i < 5; i++)
        {
            sock.sin_addr.s_addr = PC_LASTHOST(u)[i];
            sprintf(buf, "IP [%s]<br/>", inet_ntoa(sock.sin_addr));
            send_to_char(buf, ch);
        }
    }
    else
        send_to_char("Information is only on Players.<br/>", ch);
}

#define STR_DATA(num) \
    (obj_data[idx].v[num] == 0 ? int_str[num] : (obj_data[idx].v[num] == 1 ? (OBJ_VALUE(u, num) ? sprinttype(NULL, OBJ_VALUE(u, num), spl_text) : "None") : (obj_data[idx].v[num] == 2 ? sprinttype(NULL, OBJ_VALUE(u, num), wpn_text) : "")))

char *stat_obj_data(class unit_data *u, struct obj_type_t *obj_data)
{
    static char result[512];
    char *special_str = NULL, int_str[5][32];
    int idx = OBJ_TYPE(u), i;

    switch (idx)
    {
    case ITEM_WEAPON:
        special_str = sprinttype(NULL, OBJ_VALUE(u, 0), wpn_text);
        break;

    case ITEM_CONTAINER:
        special_str =
            affected_by_spell(u,
                              ID_CORPSE)
                ? (char *)"IS A CORPSE"
                : (char *)"";
        break;

        /*   case ITEM_DRINKCON:
            special_str = sprinttype(NULL, OBJ_VALUE(u, 2), drinks);
            break;*/
    }

    if (idx <= 0 || ITEM_SHIELD < idx)
        idx = ITEM_OTHER;

    for (i = 0; i < 5; ++i) /* Init obj-value strings */
        sprintf(int_str[i], "%ld", (signed long)OBJ_VALUE(u, i));

    sprintf(result, obj_data[idx].fmt,
            STR_DATA(0), STR_DATA(1), STR_DATA(2), STR_DATA(3),
            STR_DATA(4), special_str);

    return result;
}

#undef STR_DATA

static void stat_data(const class unit_data *ch, class unit_data *u)
{
    /*  This is a bit tricky:
     *    1: format for the sprintf, where all arguments are %s's.
     *  2-6: correspons to data[0] - data[4]
     *       0 means print the integer-value
     *       1 means print the spell associated
     *       2 means print the weapon-type (not used for wstat())
     *       3 means print the empty string (i.e. skip)
     *            A 6th trailing format-argument inserts a special string
     *              as according to the switch following...
     */
    char *cname;
    static struct obj_type_t wstat_obj_type[] = {
        {"Unused", {0, 0, 0, 0, 0}},                                 /*UNUSED    */
        {"Hours Left %s   Light Sources %s", {0, 0, 0, 0, 0}},       /*LIGHT     */
        {"Power %s<br/>Spells : '%s', '%s', '%s'", {0, 1, 1, 1, 0}}, /*SCROLL    */
        {"[0] Power : %s<br/>"
         "[1] Charges : %s<br/>"
         "[2] Spells1 : '%s'<br/>"
         "[3] Spells2 : '%s'<br/>"
         "[4] Max Charge: '%s'",
         {0, 0, 1, 1, 0}}, /*WAND      */
        {"[0] Power : %s<br/>"
         "[1] Charges : %s<br/>"
         "[2] Spells1 : '%s'<br/>"
         "[3] Spells2 : '%s'<br/>"
         "[4] Max Charge: '%s'",
         {0, 0, 1, 1, 0}}, /*STAFF     */
        {"[0] Category (%s)<br/>"
         "[1] Craftsmanship %s<br/>"
         "[2] Magical Bonus %s<br/>"
         "[3] Slaying Race %s<br/>",
         {2, 0, 0, 0, 0}},                                         /*WEAPON    */
        {"Tohit : %s  Todam : %sD%s  Type : %s", {0, 0, 0, 0, 0}}, /*FIREWEAPON*/
        {"Tohit : %s  Todam : %s%s  Type : %s", {0, 0, 3, 0, 0}},  /*MISSILE   */
        {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*TREASURE  */
        {"[0] Armour Category %s<br/>"
         "[1] Craftsmanship %s<br/>"
         "[2] Magical Bonus %s<br/>",
         {0, 0, 0, 0, 0}},                                           /*ARMOR     */
        {"Power: %s  Spells: '%s', '%s', '%s'", {0, 1, 1, 1, 0}},    /*POTION    */
        {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},        /*WORN      */
        {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},        /*OTHER     */
        {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},        /*TRASH     */
        {"Spell : %s  - Hitpoints : %s", {1, 0, 0, 0, 0}},           /*TRAP      */
        {"Data : [%s] [%s] [%s] [%s] [%s]<br/>%s", {0, 0, 0, 0, 0}}, /*CONTAINER */
        {"Tounge : %s", {0, 0, 0, 0, 0}},                            /*NOTE      */
        {"Max-contains %s  Contains %s  "
         "%sPoison Factor: %s  %sLiquid : %s",
         {0, 0, 3, 0, 3}},                                         /*DRINKCON  */
        {"Keytype : %s", {0, 0, 0, 0, 0}},                         /*KEY       */
        {"Makes full %s - %s%sPoison Factor %s", {0, 3, 3, 0, 0}}, /*FOOD      */
        {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*MONEY     */
        {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*PEN       */
        {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*BOAT      */
        {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*SPELL     */
        {"Data : [%s] [%s] [%s] [%s] [%s]", {0, 0, 0, 0, 0}},      /*BOOK      */
        {"[0] Shield Category %s<br/>"
         "[1] Craftsmanship %s<br/>"
         "[2] Magical Bonus %s<br/>",
         {0, 0, 0, 0, 0}} /*SHIELD    */
    };

    char buf[4096], tmpbuf1[256], tmpbuf2[256];
    int i;

    if (IS_CHAR(u))
    {
        sprintf(buf,
                "Char data:<br/>"
                "Descriptor: %s  Fighting: '%s'<br/>"
                "Master: '%s'  First Follower: '%s'<br/>"
                "Last room: '%s'<br/>"
                "Level [%d]  Sex [%s]  Position [%s]<br/>"
                "Race [%s]  Can Carry W [%d]  Can Carry N [%d]<br/>"
                "Char Flags [%s]<br/>"
                "Exp: [%ld] Offensive Bonus [%d] Defensive Bonus [%d]<br/>"
                "Attack [%s]   Speed [%d]   Natural-Armour [%d]<br/>"
                "HP [H%ld/L%d/G%d]   Mana [H%d/L%d/G%d]  END [H%d/L%d/G%d]<br/>"
                "STR [%d]  DEX [%d]  CON [%d]  CHA [%d]<br/>"
                "BRA [%d]  MAG [%d]  DIV [%d]  HP  [%d]<br/>",
                CHAR_DESCRIPTOR(u) ? "Yes" : "No",
                CHAR_FIGHTING(u) ? STR(UNIT_NAME(CHAR_FIGHTING(u))) : "Nobody",
                CHAR_MASTER(u) ? STR(UNIT_NAME(CHAR_MASTER(u))) : "Nobody",
                CHAR_FOLLOWERS(u) ? STR(UNIT_NAME(CHAR_FOLLOWERS(u)->follower)) : "Nobody",
                CHAR_LAST_ROOM(u) ? STR(UNIT_TITLE_STRING(CHAR_LAST_ROOM(u))) : "Nowhere",
                CHAR_LEVEL(u), sprinttype(NULL, CHAR_SEX(u), char_sex),
                sprinttype(NULL, CHAR_POS(u), char_pos),
                IS_PC(u) ? sprinttype(NULL, CHAR_RACE(u),
                                      pc_races)
                         : itoa(CHAR_RACE(u)),
                char_carry_w_limit(u), char_carry_n_limit(u),
                sprintbit(tmpbuf1, CHAR_FLAGS(u), char_flags),
                (signed long)CHAR_EXP(u), CHAR_OFFENSIVE(u),
                CHAR_DEFENSIVE(u), sprinttype(NULL, CHAR_ATTACK_TYPE(u), wpn_text), CHAR_SPEED(u),
                CHAR_NATURAL_ARMOUR(u), (signed long)UNIT_HIT(u),
                hit_limit(u), hit_gain(u), CHAR_MANA(u), mana_limit(u),
                mana_gain(u), CHAR_ENDURANCE(u), move_limit(u),
                move_gain(u), CHAR_STR(u), CHAR_DEX(u), CHAR_CON(u),
                CHAR_CHA(u), CHAR_BRA(u), CHAR_MAG(u), CHAR_DIV(u),
                CHAR_HPP(u));
        send_to_char(buf, ch);

        if (IS_PC(u))
        {
            /* Stat on a player  */
            struct time_info_data tid1, tid2;

            tid1 = age(u);
            tid2 = real_time_passed((time_t)PC_TIME(u).played, 0);

            strcpy(tmpbuf2, ctime(&PC_TIME(u).connect));
            sprintf(buf, "----------------- PLAYER -------------------<br/>"
                         "Filename [%s]  Unique ID [%ld]  BBS [%3d]  Cracks [%2d]<br/>"
                         "Skill points: [%ld]  Ability points: [%ld]  CRIMES: [%d]<br/>"
                         "Hometown: [%s] Prompt: [%s]<br/>VLVL: %d (%d lxp) <br/>"
                         "Profession: [%d %s]  Guild: [%s (%d member)]<br/>"
                         "Drunk: [%d]  Full: [%d]  Thirst: [%d]<br/>"
                         "CCInfo: %s     Setup: %s<br/>"
                         "Age: %ldY %ldM %ldD %ldH   Lifespan : %dY   Played: %ldDays %ldHours (%d)<br/>"
                         "Last connected at: %s<br/>"
                         "Created at       : %s<br/>",
                    PC_FILENAME(u),
                    (signed long)PC_ID(u),
                    PC_ACCESS_LEVEL(u),
                    PC_CRACK_ATTEMPTS(u),
                    (signed long)PC_SKILL_POINTS(u),
                    (signed long)PC_ABILITY_POINTS(u), PC_CRIMES(u),
                    STR(PC_HOME(u)), STR(UPC(u)->promptstr),
                    PC_VIRTUAL_LEVEL(u),
                    required_xp(PC_VIRTUAL_LEVEL(u) + 1) -
                        required_xp(PC_VIRTUAL_LEVEL(u)),
                    PC_PROFESSION(u), PC_PROFESSION(u) >= 0 && PC_PROFESSION(u) <=PROFESSION_MAX ? professions[PC_PROFESSION(u)] : "Undefined",
                    STR(PC_GUILD(u)),
                    PC_TIME(u).played - PC_GUILD_TIME(u),
                    PC_COND(u, DRUNK), PC_COND(u, FULL), PC_COND(u, THIRST),
                    PC_ACCOUNT(u).last4 == -1 ? "NONE" : "SET",
                    sprintbit(tmpbuf1, PC_FLAGS(u), pc_flags),
                    tid1.year, tid1.month, tid1.day, tid1.hours,
                    PC_LIFESPAN(u),
                    tid2.day, tid2.hours, PC_TIME(u).played,
                    tmpbuf2, ctime(&PC_TIME(u).creation));

            send_to_char(buf, ch);
        }
        else /* Stat on a monster */
        {
            sprintf(buf, "---------------- NON PLAYER ----------------<br/>"
                         "Default position: %s<br/>"
                         "NPC-flags: %s<br/>",
                    sprinttype(NULL, NPC_DEFAULT(u), char_pos),
                    sprintbit(tmpbuf1, NPC_FLAGS(u), npc_flags));
            send_to_char(buf, ch);
        }
    }
    else if (IS_OBJ(u)) /* Stat on an object */
    {
        sprintf(buf, "Object data:<br/>"
                     "Object type: %s (%d)<br/>"
                     "Values: [%4ld] [%4ld] [%4ld] [%4ld] [%4ld]<br/>"
                     "Magic resistance [%d]<br/><br/>"
                     "%s<br/>"
                     "Extra flags: %s<br/>"
                     "Cost: [%lu]  Cost/day: [%lu]  Equipped: %s<br/>",
                sprinttype(NULL, OBJ_TYPE(u), obj_types),
                OBJ_TYPE(u),
                (signed long)OBJ_VALUE(u, 0),
                (signed long)OBJ_VALUE(u, 1),
                (signed long)OBJ_VALUE(u, 2),
                (signed long)OBJ_VALUE(u, 3),
                (signed long)OBJ_VALUE(u, 4),
                OBJ_RESISTANCE(u),
                stat_obj_data(u, wstat_obj_type),
                sprintbit(tmpbuf1, OBJ_FLAGS(u), obj_flags),
                (unsigned long)OBJ_PRICE(u),
                (unsigned long)OBJ_PRICE_DAY(u),
                sprinttype(NULL, OBJ_EQP_POS(u), equip_pos));
        send_to_char(buf, ch);
    }
    else /* Stat on a room */
    {
        sprintf(buf, "Room data:<br/>"
                     "%s [%s@%s]  Sector type: %s<br/>"
                     "Map (%d,%d) Magic resistance [%d]<br/>Outside Environment: %s<br/>",
                UNIT_TITLE_STRING(u), UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u),
                sprinttype(NULL, ROOM_LANDSCAPE(u), room_landscape),
                UROOM(u)->mapx, UROOM(u)->mapy, ROOM_RESISTANCE(u),
                UNIT_IN(u) ? STR(TITLENAME(UNIT_IN(u))) : "Nothing");
        send_to_char(buf, ch);

        for (i = 0; i <= MAX_EXIT; i++)
            if (ROOM_EXIT(u, i))
            {
                cname = ROOM_EXIT(u, i)->open_name.catnames();
                sprintbit(tmpbuf2, ROOM_EXIT(u, i)->exit_info, unit_open_flags);

                if (ROOM_EXIT(u, i)->to_room)
                {
                    sprintf(buf, "EXIT %-5s to [%s@%s] (%s)<br/>"
                                 "   Exit Name: [%s]<br/>"
                                 "   Exit Bits: [%s] Difficulty: [%d]<br/>"
                                 "   Key: [%s]<br/>",
                            dirs[i],
                            UNIT_FI_NAME(ROOM_EXIT(u, i)->to_room),
                            UNIT_FI_ZONENAME(ROOM_EXIT(u, i)->to_room),
                            UNIT_TITLE_STRING(ROOM_EXIT(u, i)->to_room),
                            cname, tmpbuf2, ROOM_EXIT(u, i)->difficulty, 
                            ROOM_EXIT(u, i)->key ? ROOM_EXIT(u, i)->key : "");
                }
                else
                {
                    sprintf(buf, "EXIT %-5s to [NOWHERE]<br/>"
                                 "   Exit Name: [%s]<br/>"
                                 "   Exit Bits: [%s]<br/>",
                            dirs[i], cname, tmpbuf2);
                }
                FREE(cname);
                send_to_char(buf, ch);
            }
    }
}

static void stat_contents(const class unit_data *ch, class unit_data *u)
{
    char buf[MAX_INPUT_LENGTH];
    int bright = 0, light = 0;
    class unit_data *orgu;

    orgu = u;

    if (UNIT_CONTAINS(u))
    {
        for (u = UNIT_CONTAINS(u); u; u = u->next)
        {
            if (CHAR_LEVEL(ch) >= UNIT_MINV(u))
            {
                sprintf(buf, "[%s@%s] Name '%s', Title '%s'  %s (L%d B%d)<br/>",
                        UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u),
                        UNIT_NAME(u), UNIT_TITLE_STRING(u),
                        IS_OBJ(u) && OBJ_EQP_POS(u) ? "Equipped" : "",
                        UNIT_LIGHTS(u), UNIT_BRIGHT(u));
                send_to_char(buf, ch);
                bright += UNIT_BRIGHT(u);
                light += UNIT_LIGHTS(u);
            }
        }
        sprintf(buf, "Contents lights sum = %d, bright sum = %d<br/>", light, bright);
        send_to_char(buf, ch);
        sprintf(buf, "Parent unit lights total = %d (bright %d)<br/>", UNIT_LIGHTS(orgu), UNIT_BRIGHT(orgu));
        send_to_char(buf, ch);
    }
    else
        send_to_char("It is empty.<br/>", ch);
}

static void stat_descriptor(const class unit_data *ch, class unit_data *u)
{
    send_to_char("Is yet to be programmed.<br/>", ch);
}

void do_wstat(class unit_data *ch, char *argument,
              const struct command_info *cmd)
{
    char buf[4 * MAX_STRING_LENGTH];
    class unit_data *u = NULL;
    class zone_type *zone = NULL;

    if (CHAR_DESCRIPTOR(ch) == NULL)
        return;

    if (str_is_empty(argument))
    {
        send_to_char("Usage: See help wstat<br/>[room|zone|memory|account|creators|count <no>|world [dil (ms)|extra|zone]|unit-name]<br/>", ch);
        return;
    }

    if (!str_nccmp("room", argument, 4))
    {
        u = UNIT_IN(ch);
        argument += 4;
    }
    else if (!strncmp("zone", argument, 4))
    {
        zone = unit_zone(ch);
        argument += 4;
    }
    else if (!strncmp("memory", argument, 6))
    {
        stat_memory(ch);
        return;
    }
    else if (!strncmp("account", argument, 7))
    {
        account_global_stat(ch);
        return;
    }
    else if (!strncmp("creators", argument, 8))
    {
        argument += 8;
        stat_creators(ch, argument);
        return;
    }
    else if (!strncmp("count", argument, 5))
    {
        argument += 5;
        stat_world_count(ch, argument);
        return;
    }
    else if (!strncmp("world", argument, 5))
    {
        argument += 5;
        if (str_is_empty(argument))
        {
            stat_world(ch);
            return;
        }

        argument++;
        if (!strncmp("dil", argument, 3))
        {
            int i = atoi(argument + 4);
            stat_global_dil(ch, i);
            return;
        }
        else
            stat_world_extra(ch);
        return;
    }
    else
    {
        class file_index_type *fi;

        u = find_unit(ch, &argument, 0, FIND_UNIT_GLOBAL);

        if (u == NULL)
        {
            fi = pc_str_to_file_index(ch, argument);

            if (fi)
            {
                if (fi->unit)
                    u = fi->unit;
                else
                {
                    if (fi->no_in_mem == 0)
                    {
                        if (!IS_ADMINISTRATOR(ch))
                            send_to_char("No instances in memory.<br/>", ch);
                        else
                            do_load(ch, argument, cmd);
                    }
                    u = find_symbolic_instance(fi);
                }
            }
        }
    }

    if (!u && !zone)
    {
        send_to_char("No such thing anywhere.<br/>", ch);
        return;
    }

    if (zone)
    {
        if (str_is_empty(argument))
            stat_zone(ch, zone);
        else
            extra_stat_zone(ch, argument, zone);

        return;
    }
    argument = one_argument(argument, buf);
    if (strlen(buf) < 1)
        stat_normal(ch, u);
    else if (!strncmp(buf, "data", strlen(buf)))
        stat_data(ch, u);
    else if (!strncmp(buf, "contents", strlen(buf)))
        stat_contents(ch, u);
    else if (!strncmp(buf, "affects", strlen(buf)))
        stat_affect(ch, u);
    else if (!strncmp(buf, "descriptor", strlen(buf)))
        stat_descriptor(ch, u);
    else if (!strncmp(buf, "functions", strlen(buf)))
        stat_func(ch, u);
    else if (!strncmp(buf, "spells", strlen(buf)))
        stat_spell(ch, u);
    else if (!strncmp(buf, "skills", strlen(buf)))
        stat_skill(ch, u);
    else if (!strncmp(buf, "weapons", strlen(buf)))
        stat_wskill(ch, u);
    else if (!strncmp(buf, "extras", strlen(buf)))
        stat_extra_descr(ch, u, argument);
    else if (!strncmp(buf, "quests", strlen(buf)))
        stat_extra_quest(ch, u, argument);
    else if (!strncmp(buf, "ability", strlen(buf)))
        stat_ability(ch, u);
    else if (!strncmp(buf, "account", strlen(buf)))
        account_local_stat(ch, u);
    else if (!strncmp(buf, "bank", strlen(buf)))
        stat_bank(ch, u);
    else if (!strncmp(buf, "combat", strlen(buf)))
        stat_combat(ch, u);
    else if (!strncmp(buf, "info", strlen(buf)))
        stat_extra_info(ch, u, argument);
    else if (!strncmp(buf, "ip", strlen(buf)))
        stat_ip(ch, u);
    else
        stat_normal(ch, u);
}
