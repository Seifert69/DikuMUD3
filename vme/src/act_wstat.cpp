/*
 $Author: All $
 $RCSfile: act_wstat.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.10 $
 */

#ifdef _WINDOWS
    #include <winsock2.h>
#endif

#include "account.h"
#include "act_wizard.h"
#include "affect.h"
#include "apf_affect.h"
#include "bank.h"
#include "comm.h"
#include "common.h"
#include "constants.h"
#include "db.h"
#include "dilshare.h"
#include "files.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "main_functions.h"
#include "modify.h"
#include "skills.h"
#include "spec_assign.h"
#include "spell_parser.h"
#include "spells.h"
#include "structs.h"
#include "system.h"
#include "textutil.h"
#include "tif_affect.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "utility.h"
#include "utils.h"
#include "vmelimits.h"
#include "weather.h"
#include "zone_reset_cmd.h"

#include <arpa/inet.h>
#include <netinet/in.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <string>

static void stat_world_count(const unit_data *ch, char *arg)
{
    int nMinCount = atoi(arg);
    auto msg = diku::format_to_str("The first 40 units containing at least %d units:<br/>", nMinCount);

    int n = 0;
    for (unit_data *u = g_unit_list; u; u = u->gnext)
    {
        int i = 0;
        for (unit_data *t = UNIT_CONTAINS(u); t; t = t->next)
        { // count top layer
            i++;
        }

        if (i >= nMinCount)
        {
            msg += diku::format_to_str("%s@%s(%s) : %d units <br/>", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u), UNIT_NAME(u), i);
            n++;

            if (n >= 40)
            {
                break;
            }
        }
    }

    msg += "<br/>";

    page_string(CHAR_DESCRIPTOR(ch), msg);
}

static void stat_world_extra(const unit_data *ch)
{
    auto msg = diku::format_to_str("World zones (%d):<br/>", g_zone_info.no_of_zones);
    msg += "<div class='fourcol'>";

    for (auto zp : g_zone_info.mmp)
    {
        msg += diku::format_to_str("<a cmd='goto #'>%s</a><br/>", zp.second->getName());
    }
    msg += "</div><br/>";

    page_string(CHAR_DESCRIPTOR(ch), msg);
}

static void stat_memory(unit_data *ch)
{
    auto msg = diku::format_to_str("Event queue entries: %d<br/>", g_events.Count());
    send_to_char(msg, ch);

    msg = diku::format_to_str("Current Tic=%d , Event Queue Tic=%d<br/>", g_tics, g_events.NextEventTic());
    send_to_char(msg, ch);

    msg = diku::format_to_str("Event queue GL Stats ( Proceses/Sec ): %d/%1.4f<br/>", g_events.PCount(), g_events.PTime());
    send_to_char(msg, ch);

    msg = diku::format_to_str("Event queue Max Time  ( Proceses/Sec ): %d/%1.4f<br/>", g_events.Max_PTime_Process(), g_events.Max_PTime());
    send_to_char(msg, ch);

    msg =
        diku::format_to_str("Event queue Max Process  ( Proceses/Sec ): %d/%1.4f<br/>", g_events.Max_PCount(), g_events.Max_PCount_Time());
    send_to_char(msg, ch);

    msg = diku::format_to_str("Event queue Avg Process  ( Proceses/Sec ): %1.1f/%1.4f<br/>", g_events.Avg_PCount(), g_events.Avg_PTime());
    send_to_char(msg, ch);
    //  system_memory (ch);
    //  memory_status (buf);
    //  send_to_char (buf, ch);

    // Also do a unit sanity check here, not memory related, just a hack
    //

    for (unit_data *u = g_unit_list; u; u = u->next)
    {
        if (UNIT_TYPE(u) != UNIT_ST_ROOM)
        {
            if (UNIT_IN(u) == nullptr)
            {
                msg = diku::format_to_str("%s@%s is not in a room<br/>", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
                send_to_char(msg, ch);
            }
        }
    }
}

static void stat_world(unit_data *ch)
{
    time_t now = time(nullptr);

    char *p = ctime(&now);
    p[strlen(p) - 1] = '\0';

    auto msg = diku::format_to_str("Server compiled at %s %s<br/>"
                                   "World status (tick %d aka %d hours): %s<br/>"
                                   "#rooms [%4d]  #objects [%4d]  #chars [%4d]  #npcs [%4d] "
                                   "  #pcs [%4d]<br/>"
                                   "#units [%4d]  #zones   [%4d]<br/>"
                                   "#connections [%2d / peak %2d]<br/>"
                                   "#players     [%2d / peak %2d]<br/>"
                                   "Boottime: %s<br/>Time now: %s<br/>"
                                   "DIL programs [%d]   DIL Vals [%d]<br>",
                                   g_compile_date,
                                   g_compile_time,
                                   g_tics,
                                   g_tics / (PULSE_SEC * 3600),
                                   g_nTickUsec < 1 ? "time-warped" : "",
                                   g_world_norooms,
                                   g_world_noobjects,
                                   g_world_nochars,
                                   g_world_nonpc,
                                   g_world_nopc,
                                   g_world_norooms + g_world_noobjects + g_world_nochars,
                                   g_world_nozones,
                                   g_no_connections,
                                   g_max_no_connections,
                                   g_no_players,
                                   g_max_no_players,
                                   g_world_boottime,
                                   p,
                                   g_nDilPrg,
                                   g_nDilVal);

    page_string(CHAR_DESCRIPTOR(ch), msg);
}

static std::string stat_zone_reset(const std::string &indnt, const zone_reset_cmd *zrip, unit_data *ch)
{
    static const char *nums[] = {"max", "zonemax", "local"};
    std::string bits;
    int i = 0;

    std::string stat{indnt};

    switch (zrip->getCommandNum())
    {
        case 0:
            stat = "Nop";
            break;

        case 1:
            stat += diku::format_to_str("Load %s", zrip->getFileIndexType(0)->getName());

            if (zrip->getFileIndexType(1))
            {
                stat += diku::format_to_str(" into %s", zrip->getFileIndexType(1)->getName());
            }

            for (i = 0; i < 3; i++)
            {
                if (zrip->getNum(i))
                {
                    stat += diku::format_to_str(" %s %d", nums[i], zrip->getNum(i));
                }
            }

            stat += zrip->getCompleteFlag() ? " Complete" : "";
            break;

        case 2:
            stat = diku::format_to_str("Equip %s %s max %d %s",
                                       zrip->getFileIndexType(0)->getName(),
                                       g_where[zrip->getNum(1)],
                                       zrip->getNum(0),
                                       zrip->getCompleteFlag() ? "Complete" : "");
            break;

        case 3:
            stat = diku::format_to_str("Door at %s : %s : %s",
                                       zrip->getFileIndexType(0)->getName(),
                                       g_dirs[zrip->getNum(0)],
                                       sprintbit(bits, zrip->getNum(1), g_unit_open_flags));
            break;

        case 4:
            stat = diku::format_to_str("Purge %s", zrip->getFileIndexType(0)->getName());
            break;

        case 5:
            stat = diku::format_to_str("Remove %s in %s", zrip->getFileIndexType(0)->getName(), zrip->getFileIndexType(1)->getName());
            break;
    }

    stat += "<br/>";

    if (zrip->getNested())
    {
        stat += diku::format_to_str("%s{<br/>", indnt);

        std::string whitespace{indnt + "  "};
        stat += stat_zone_reset(whitespace, zrip->getNested(), ch);

        stat += diku::format_to_str("%s}<br/>", indnt);
    }
    if (zrip->getNext())
    {
        stat += stat_zone_reset(indnt, zrip->getNext(), ch);
    }
    return stat;
}

static void stat_zone(unit_data *ch, zone_type *zone)
{
    static const char *reset_modes[] = {"Never Reset", "Reset When Empty", "Reset Always", "UNKNOWN"};
    int reset_mode = zone->getResetMode();

    if (!is_in(reset_mode, 0, 2))
    {
        reset_mode = 3;
    }

    bool errors = file_exists(g_cServerConfig.getZoneDir() + zone->getFilename() + ".err");
    bool info = file_exists(g_cServerConfig.getZoneDir() + zone->getFilename() + ".inf");

    char *cname = zone->getCreators().catnames();

    auto msg = diku::format_to_str("Zone [%s]  File [%s]  Access [%d]<br/>"
                                   "Title: \"%s\"<br/>"
                                   "Load level [%d] Pay only [%d]<br/>"
                                   "Number of Units [%d]    Number of Rooms [%d]<br/>"
                                   "Reset Mode : %s (%d)    Reset Interval [%d]<br/>"
                                   "Pressure [%d] Change [%d] Sky [%d] Base [%d]<br/><br/>"
                                   "Authors Mud Mail: %s<br/><br/>%s<br/><br/>"
                                   "%s<br/>%s<br/>",
                                   zone->getName(),
                                   zone->getFilename(),
                                   zone->getAccessLevel(),
                                   zone->getTitle() ? zone->getTitle() : "",
                                   zone->getLevelRequiredToLoadItems(),
                                   zone->getPayOnly(),
                                   zone->getNumOfFileIndexes(),
                                   zone->getNumOfRooms(),
                                   reset_modes[reset_mode],
                                   reset_mode,
                                   zone->getZoneResetTime(),
                                   zone->cgetWeather().getPressure(),
                                   zone->cgetWeather().getChange(),
                                   zone->cgetWeather().getSky(),
                                   zone->cgetWeather().getBase(),
                                   cname,
                                   zone->getNotes(),
                                   errors ? "Errors in zone (stat zone error)" : "No errors registered in zone.",
                                   info ? "User info feedback in zone (stat zone info)." : "No user info (feedback) in zone.");
    FREE(cname)
    page_string(CHAR_DESCRIPTOR(ch), msg);
}

static void stat_creators(unit_data *ch, char *arg)
{
    char tmp[1024];
    int found = 0;

    if (str_is_empty(arg))
    {
        send_to_char("Requires name of creator.<br/>", ch);
        return;
    }

    arg = one_argument(arg, tmp);

    if (str_ccmp(tmp, "all") == 0)
    {
        std::string msg{"List of all Zones with Creators.<br/><br/>"};
        found = FALSE;

        for (auto it = g_zone_info.mmp.begin(); it != g_zone_info.mmp.end(); it++)
        {
            char *cname = it->second->getCreators().catnames();

            msg += diku::format_to_str("%-15s   %s<br/>", it->second->getName(), cname);
            FREE(cname);
            found = TRUE;
        }

        page_string(CHAR_DESCRIPTOR(ch), msg);
        return;
    }

    auto msg = diku::format_to_str("Zones Created by %s.<br/><br/>", tmp);

    found = FALSE;
    for (auto it = g_zone_info.mmp.begin(); it != g_zone_info.mmp.end(); it++)
    {
        if (it->second->getCreators().IsName(tmp))
        {
            msg += diku::format_to_str("%-15s   File: %s.zon<br/>", it->second->getName(), it->second->getFilename());
            found = TRUE;
        }
    }

    if (!found)
    {
        msg += "None.<br/>";
    }

    send_to_char(msg, ch);
}

static void stat_dil(unit_data *ch, zone_type *zone)
{
    auto msg = diku::format_to_str("<u>List of DIL in zone %s (CPU secs, name, #activations, #instructions):</u><br/>", zone->getName());
    send_to_char(msg, ch);

    msg += "<div class='twocol'>";
    for (auto tmpl = zone->cgetDILTemplate().begin(); tmpl != zone->cgetDILTemplate().end(); tmpl++)
    {
        msg += diku::format_to_str("%.2fs %s [%d t / %d i]<br/>",
                                   tmpl->second->fCPU / 1000.0,
                                   tmpl->second->prgname,
                                   tmpl->second->nTriggers,
                                   tmpl->second->nInstructions);
    }

    msg += "</div><br/>"; // MS2020
    send_to_char(msg, ch);
}

static void stat_global_dil(unit_data *ch, ubit32 nCount)
{
    auto msg = diku::format_to_str("<u>List of global DIL in all zones running for more than %dms:</u><br/>", nCount);
    send_to_char(msg, ch);

    msg += "<div class='twocol'>";

    for (auto z = g_zone_info.mmp.begin(); z != g_zone_info.mmp.end(); z++)
    {
        for (auto tmpl = z->second->cgetDILTemplate().begin(); tmpl != z->second->cgetDILTemplate().end(); tmpl++)
        {
            if (tmpl->second->fCPU >= nCount)
            {
                msg += diku::format_to_str("%.2fs %s@%s [%d t / %d i]<br/>",
                                           tmpl->second->fCPU / 1000.0,
                                           tmpl->second->prgname,
                                           tmpl->second->zone->getName(),
                                           tmpl->second->nTriggers,
                                           tmpl->second->nInstructions);
            }
        }
    }

    msg += "</div><br/>"; // MS2020
    send_to_char(msg, ch);
}

static void extra_stat_zone(unit_data *ch, char *arg, zone_type *zone)
{
    char buf[MAX_STRING_LENGTH];
    int argno = 0;
    int search_type = 0;

    //  void stat_dijkstraa (class unit_data * ch, class zone_type *z);

    static const char *zone_args[] = {"mobiles", "objects", "rooms", "reset", "errors", "info", "path", "dil", nullptr};

    static int search_types[] = {UNIT_ST_NPC, UNIT_ST_OBJ, UNIT_ST_ROOM};

    arg = one_argument(arg, buf);
    argno = search_block(buf, zone_args, 0);

    if (argno == -1) /* Asked for a specific zone? */
    {
        if ((zone = find_zone(buf)) == nullptr)
        {
            send_to_char("Usage: wstat zone [name] "
                         "[mobiles|objects|rooms|reset|info|errors|path|dil]<br/>",
                         ch);
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
        {
            auto msg = diku::format_to_str("List of %s in zone %s:<br/>", zone_args[argno], zone->getName());
            send_to_char(msg, ch);
            search_type = search_types[argno];
        }
        break;

        case 3:
            if (zone->cgetZoneResetCommands())
            {
                auto msg = diku::format_to_str("Reset information for zone %s:<br/>", zone->getName());
                send_to_char(msg, ch);
                auto stat_buffer = stat_zone_reset("", zone->cgetZoneResetCommands(), ch);
                page_string(CHAR_DESCRIPTOR(ch), stat_buffer);
            }
            else
            {
                auto msg = diku::format_to_str("No reset information for zone %s.<br/>", zone->getName());
                send_to_char(msg, ch);
            }
            return;

        case 4:
        case 5:
        {
            /* Errors/Info (Small hack, this :-) ) */
            auto filename = diku::format_to_str("%s%s.%.3s", g_cServerConfig.getZoneDir().c_str(), zone->getFilename(), zone_args[argno]);
            if (!file_exists(filename))
            {
                return;
            }
            file_to_string(filename, buf, MAX_STRING_LENGTH);
            page_string(CHAR_DESCRIPTOR(ch), buf);
            return;
        }

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
    std::string msg{"<div class='threecol'>"};
    for (auto fi = zone->cgetFileIndexMap().begin(); fi != zone->cgetFileIndexMap().end(); fi++)
    {
        if (fi->second->getType() == search_type)
        {
            if ((fi->second->getType() == UNIT_ST_OBJ) || (fi->second->getType() == UNIT_ST_NPC))
            {
                msg += diku::format_to_str("<a cmd='load #'>%s</a><br/>", fi->second->getName());
            }
            else
            {
                msg += diku::format_to_str("%s<br/>", fi->second->getName());
            }
        }
    }

    msg += "</div></br>";
    send_to_char(msg, ch);
}

static void stat_ability(const unit_data *ch, unit_data *u)
{
    int i = 0;

    if (!IS_PC(u))
    {
        send_to_char("Unit is not a PC - use 'data' for NPC's<br/>", ch);
        return;
    }

    std::string buf{"Char Abilities:<br/>"};

    for (i = 0; i < ABIL_TREE_MAX; i++)
    {
        buf += diku::format_to_str("%20s : %3d%% Lvl %3d Racial %3d<br/>",
                                   g_AbiColl.text[i],
                                   CHAR_ABILITY(u, i),
                                   PC_ABI_LVL(u, i),
                                   get_racial_ability(CHAR_RACE(u), i));
    }

    page_string(CHAR_DESCRIPTOR(ch), buf);
}

static void stat_spell(const unit_data *ch, unit_data *u)
{
    char tmpbuf1[100];
    int i = 0;
    int max = 0;

    if (!IS_CHAR(u))
    {
        send_to_char("Unit is not a char<br/>", ch);
        return;
    }

    std::string msg{"Char magic skill<br/><pre>"};

    max = IS_NPC(u) ? SPL_GROUP_MAX : SPL_TREE_MAX;

    for (i = 0; i < max; i++)
    {
        str_next_word(TREE_ISLEAF(g_SplColl.tree, i) ? g_SplColl.text[TREE_PARENT(g_SplColl.tree, i)] : "sphere", tmpbuf1);

        if (TREE_ISLEAF(g_SplColl.tree, i) && strcmp(tmpbuf1, "sphere") == 0)
        {
            if (g_spell_info[i].tmpl == nullptr && g_spell_info[i].spell_pointer == nullptr)
            {
                strcpy(tmpbuf1, "NOT IMPLEMENTED");
            }
        }

        auto tmpbuf2 = diku::format_to_str(
            "%s %s (%s)",
            g_spell_info[i].cast_type == SPLCST_CHECK ? "CHECK " : (g_spell_info[i].cast_type == SPLCST_RESIST ? "RESIST" : "OTHER "),
            g_SplColl.text[i],
            tmpbuf1);

        msg += diku::format_to_str("%c%c%c%c%c] %3d%%/%d %c %-50s [%3d racial]<br/>",
                                   IS_SET(g_spell_info[i].media, MEDIA_SPELL) ? 'C' : '-',
                                   IS_SET(g_spell_info[i].media, MEDIA_SCROLL) ? 'S' : '-',
                                   IS_SET(g_spell_info[i].media, MEDIA_POTION) ? 'P' : '-',
                                   IS_SET(g_spell_info[i].media, MEDIA_WAND) ? 'W' : '-',
                                   IS_SET(g_spell_info[i].media, MEDIA_STAFF) ? 'R' : '-',
                                   IS_NPC(u) ? NPC_SPL_SKILL(u, i) : PC_SPL_SKILL(u, i),
                                   IS_NPC(u) ? 0 : PC_SPL_LVL(u, i),
                                   g_spell_info[i].realm == ABIL_DIV ? 'D' : (g_spell_info[i].realm == ABIL_MAG ? 'M' : '!'),
                                   tmpbuf2,
                                   get_racial_spells(CHAR_RACE(u), i));
    }

    msg += "</pre>";
    page_string(CHAR_DESCRIPTOR(ch), msg);
}

static void stat_skill(const unit_data *ch, unit_data *u)
{
    if (!IS_CHAR(u))
    {
        send_to_char("Unit is not a char<br/>", ch);
    }
    else if (IS_NPC(u))
    {
        send_to_char("NPC's have no skills.<br/>", ch);
    }
    else
    {
        std::string msg{"Other skills:<br/>"};

        for (int i = 0; i < SKI_TREE_MAX; i++)
        {
            msg += diku::format_to_str("%20s: %3d%% Lvl %3d Racial %3d<br/>",
                                       g_SkiColl.text[i],
                                       PC_SKI_SKILL(u, i),
                                       PC_SKI_LVL(u, i),
                                       get_racial_skill(CHAR_RACE(u), i));
        }

        page_string(CHAR_DESCRIPTOR(ch), msg);
    }
}

static void stat_wskill(const unit_data *ch, unit_data *u)
{
    if (!IS_CHAR(u))
    {
        send_to_char("Unit is not a char<br/>", ch);
        return;
    }

    std::string msg{"Char weapon skill:<br/>"};

    int max = IS_NPC(u) ? WPN_GROUP_MAX : WPN_TREE_MAX;

    for (int i = 0; i < max; i++)
    {
        msg += diku::format_to_str("%20s : %3d%% Lvl %3d Racial %3d<br/>",
                                   g_WpnColl.text[i],
                                   IS_NPC(u) ? NPC_WPN_SKILL(u, i) : PC_WPN_SKILL(u, i),
                                   IS_NPC(u) ? 0 : PC_WPN_LVL(u, i),
                                   get_racial_weapon(CHAR_RACE(u), i));
    }
    page_string(CHAR_DESCRIPTOR(ch), msg);
}

static void stat_affect(const unit_data *ch, unit_data *u)
{
    unit_affected_type *af = nullptr;

    if (!UNIT_AFFECTED(u))
    {
        send_to_char("It is not affected by anything.<br/>", ch);
        return;
    }

    send_to_char("Unit affects:<br/>", ch);

    for (af = UNIT_AFFECTED(u); af; af = af->getNext())
    {
        auto msg = diku::format_to_str("----------------------------------------------------<br/>"
                                       "Id [%d]   Duration [%d]   Beat [%d] Data [%d] [%d] [%d]<br/>"
                                       "First f() %s<br/>"
                                       "Tick  f() %s<br/>"
                                       "Last  f() %s<br/>"
                                       "Apply f() %s<br/>"
                                       "%s",
                                       af->getID(),
                                       af->getDuration(),
                                       af->getBeat(),
                                       af->getDataAtIndex(0),
                                       af->getDataAtIndex(1),
                                       af->getDataAtIndex(2),
                                       af->getFirstFI() >= 0 ? STR(g_tif[af->getFirstFI()].descr) : "Not used (-1)",
                                       af->getTickFI() >= 0 ? STR(g_tif[af->getTickFI()].descr) : "Not used (-1)",
                                       af->getLastFI() >= 0 ? STR(g_tif[af->getLastFI()].descr) : "Not used (-1)",
                                       af->getApplyFI() >= 0 ? STR(g_apf[af->getApplyFI()].descr) : "Not used (-1)",
                                       u == af->cgetOwner() ? "" : "Serious error in owner pointer!<br/>");
        send_to_char(msg, ch);
    }
}

static void stat_func(const unit_data *ch, unit_data *u)
{
    std::string bits;
    unit_fptr *f = nullptr;

    if (!UNIT_FUNC(u))
    {
        send_to_char("It has no special routines.<br/>", ch);
        return;
    }

    send_to_char("Unit functions:<br/>", ch);
    for (f = UNIT_FUNC(u); f; f = f->getNext())

    {
        if (f->getFunctionPointerIndex() == SFUN_DIL_INTERNAL)
        {
            dilprg *prg = nullptr;

            if ((prg = (dilprg *)f->getData()))
            {
                auto msg = diku::format_to_str("DIL Name: %s@%s<br/>",
                                               prg->frame[0].tmpl->prgname,
                                               prg->frame[0].tmpl->zone ? prg->frame[0].tmpl->zone->getName() : "IDLE");
                send_to_char(msg, ch);
            }
        }

        auto msg = diku::format_to_str(
            "[%3d] %s Flags [%s] Index [%d] Beat [%d]<br/>"
            "%s<br/><br/>",
            f->getFunctionPriority(),
            g_unit_function_array[f->getFunctionPointerIndex()].name,
            sprintbit(bits, f->getAllActivateOnEventFlags(), g_sfb_flags),
            f->getFunctionPointerIndex(),
            f->getHeartBeat(),
            f->getData() ? g_unit_function_array[f->getFunctionPointerIndex()].save_w_d == SD_ASCII ? (char *)f->getData() : "Has raw data."
                         : "No data.");
        send_to_char(msg, ch);
    }
}

static void stat_normal(unit_data *ch, unit_data *u)
{
    std::string bits1;
    std::string bits2;
    char *cname = nullptr;

    /* Stat on the unit */

    cname = UNIT_NAMES(u).catnames(); /* Get names into tmpbuf1 */

    /* Even though type isn't a flag, we'd better show them all in case
     * more than one is set!
     */
    auto msg = diku::format_to_str("Unit status: %s [%s@%s] %d copies (CRC %lu)<br/>Namelist: %s<br/>"
                                   "Title: \"%s\"<br/>Outside_descr:<br/>\"%s\"<br/>"
                                   "Inside_descr:<br/>\"%s\"<br/>",
                                   sprintbit(bits2, UNIT_TYPE(u), g_unit_status),
                                   UNIT_FI_NAME(u),
                                   UNIT_FI_ZONENAME(u),
                                   UNIT_FILE_INDEX(u) ? UNIT_FILE_INDEX(u)->getNumInMem() : -1,
                                   UNIT_FILE_INDEX(u) ? (unsigned long)UNIT_FILE_INDEX(u)->getCRC() : 0,
                                   cname,
                                   STR(UNIT_TITLE_STRING(u)),
                                   STR(UNIT_OUT_DESCR_STRING(u)),
                                   STR(UNIT_IN_DESCR_STRING(u)));
    send_to_char(msg, ch);
    FREE(cname);
    msg = diku::format_to_str("Lights: [%d]  Bright: [%d]  TrnIllu: [%d]  "
                              "Chars Within [%d] WIZI [%d]<br/>"
                              "Unit is inside: %s -- Unit %s<br/>"
                              "Manipulate: %s<br/>"
                              "Flags: %s<br/>"
                              "Hitpoints/max: [%ld/%ld]  Alignment: [%d]<br/>",
                              UNIT_LIGHTS(u),
                              UNIT_BRIGHT(u),
                              UNIT_ILLUM(u),
                              UNIT_CHARS(u),
                              UNIT_MINV(u),
                              UNIT_IN(u) ? STR(TITLENAME(UNIT_IN(u))) : "Nothing",
                              UNIT_CONTAINS(u) ? "has contents" : "is empty",
                              sprintbit(bits2, UNIT_MANIPULATE(u), g_unit_manipulate),
                              sprintbit(bits1, UNIT_FLAGS(u), g_unit_flags),
                              (signed long)UNIT_HIT(u),
                              (signed long)UNIT_MAX_HIT(u),
                              UNIT_ALIGNMENT(u));
    send_to_char(msg, ch);

    msg = diku::format_to_str("Key name: [%s]  Open flags: %s  Open Diff: %d<br/>"
                              "Base weight : [%d] Weight : [%d] Capacity : [%d] Size [%d]<br/>",
                              UNIT_KEY(u) ? UNIT_KEY(u) : "none",
                              sprintbit(bits1, UNIT_OPEN_FLAGS(u), g_unit_open_flags),
                              UNIT_OPEN_DIFF(u),
                              UNIT_BASE_WEIGHT(u),
                              UNIT_WEIGHT(u),
                              UNIT_CAPACITY(u),
                              UNIT_SIZE(u));
    send_to_char(msg, ch);
}

static void stat_extra(const unit_data *ch, extra_list &elist, char *grp)
{
    bool found = false;
    char buf[4 * MAX_STRING_LENGTH];
    /* MS: We used to do a TAIL here... bad idea as newspaper is VERY HUGE */
    /* This isn't nice either, but it works... */
    char *cname = nullptr;
    std::string str;

    str = "";

    if (!elist.isempty())
    {
        one_argument(grp, buf);

        extra_descr_data *ed = nullptr;
        for (ed = elist.m_pList; ed; ed = ed->next)
        {
            if ((*buf) && ed->names.StrStr(grp))
            {
                if (!found)
                {
                    str.append("Extra descriptions:<br/>-------------------<br/>");
                    found = true;
                }
                str.append("Names ");
                cname = ed->names.catnames();
                str.append(ed->names.catnames());
                FREE(cname);
                str.append("<br/>");
                if (ed->vals.Length() > 0)
                {
                    str.append("Values ");
                    cname = ed->vals.catnames();
                    str.append(cname);
                    FREE(cname);
                    str.append("<br/>\"");
                }
                else
                {
                    str.append("\"");
                }
                str.append(ed->descr);
                str.append("\"<br/>-------------------<br/>");
            }
            else if (!(*buf))
            {
                if (!found)
                {
                    str.append("Extra descriptions:<br/>-------------------<br/>");
                    found = true;
                }

                str.append("Names ");
                cname = ed->names.catnames();
                str.append(cname);
                FREE(cname);
                str.append("<br/>");

                if (ed->vals.Length() > 0)
                {
                    str.append("Values ");
                    cname = ed->vals.catnames();
                    str.append(cname);
                    FREE(cname);
                    str.append("<br/>\"");
                }
                else
                {
                    str.append("\"");
                }
                str.append(ed->descr);
                str.append("\"<br/>-------------------<br/>");
            }
        }
    }

    if (!found)
    {
        send_to_char("None.<br/>", ch);
    }
    else
    {
        send_to_char(str.c_str(), ch);
    }
}

static void stat_extra_descr(const unit_data *ch, unit_data *u, char *grp)
{
    stat_extra(ch, UNIT_EXTRA(u), grp);
}

static void stat_extra_quest(const unit_data *ch, unit_data *u, char *grp)
{
    if (IS_PC(u))
    {
        stat_extra(ch, PC_QUEST(u), grp);
    }
    else
    {
        send_to_char("Quests only on Players.<br/>", ch);
    }
}

static void stat_extra_info(const unit_data *ch, unit_data *u, char *grp)
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
        stat_extra(ch, PC_INFO(u), grp);
    }
    else
    {
        send_to_char("Information is only on Players.<br/>", ch);
    }
}

static void stat_ip(const unit_data *ch, unit_data *u)
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
        sockaddr_in sock;

        for (int i = 0; i < 5; i++)
        {
            sock.sin_addr.s_addr = PC_LASTHOST(u)[i];
            auto msg = diku::format_to_str("IP [%s]<br/>", inet_ntoa(sock.sin_addr));
            send_to_char(msg, ch);
        }
    }
    else
    {
        send_to_char("Information is only on Players.<br/>", ch);
    }
}

#define STR_DATA(num)                                                                                                                      \
    (pobjdata[idx].v[num] == 0                                                                                                             \
         ? int_str[num]                                                                                                                    \
         : (pobjdata[idx].v[num] == 1 ? (OBJ_VALUE(u, num) ? sprinttype(NULL, OBJ_VALUE(u, num), g_SplColl.text) : "None")                 \
                                      : (pobjdata[idx].v[num] == 2 ? sprinttype(NULL, OBJ_VALUE(u, num), g_WpnColl.text) : "")))

const char *stat_obj_data(unit_data *u, obj_type_t *pobjdata)
{
    char *special_str = nullptr;
    std::string int_str[5];
    int idx = OBJ_TYPE(u);
    int i = 0;

    switch (idx)
    {
        case ITEM_WEAPON:
            special_str = sprinttype(nullptr, OBJ_VALUE(u, 0), g_WpnColl.text);
            break;

        case ITEM_CONTAINER:
            special_str = affected_by_spell(u, ID_CORPSE) ? (char *)"IS A CORPSE" : (char *)"";
            break;

            /*   case ITEM_DRINKCON:
                special_str = sprinttype(NULL, OBJ_VALUE(u, 2), g_drinks);
                break;*/
    }

    if (idx <= 0 || ITEM_SHIELD < idx)
    {
        idx = ITEM_OTHER;
    }

    for (i = 0; i < 5; ++i)
    { /* Init obj-value strings */
        int_str[i] = diku::format_to_str("%ld", (signed long)OBJ_VALUE(u, i));
    }

    static std::string result;
    result = diku::format_to_str(pobjdata[idx].fmt, STR_DATA(0), STR_DATA(1), STR_DATA(2), STR_DATA(3), STR_DATA(4), special_str);
    return result.c_str();
}

#undef STR_DATA

static void stat_data(const unit_data *ch, unit_data *u)
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
    char *cname = nullptr;
    static obj_type_t wstat_obj_type[] = {
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

    char tmp[512];
    std::string bits1;
    std::string bits2;
    int i = 0;

    if (IS_CHAR(u))
    {
        auto msg = diku::format_to_str("Char data:<br/>"
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
                                       CHAR_LEVEL(u),
                                       sprinttype(nullptr, CHAR_SEX(u), g_char_sex),
                                       sprinttype(nullptr, CHAR_POS(u), g_char_pos),
                                       IS_PC(u) ? sprinttype(nullptr, CHAR_RACE(u), g_pc_races) : itoa(CHAR_RACE(u)),
                                       char_carry_w_limit(u),
                                       char_carry_n_limit(u),
                                       sprintbit(bits1, CHAR_FLAGS(u), g_char_flags),
                                       (signed long)CHAR_EXP(u),
                                       CHAR_OFFENSIVE(u),
                                       CHAR_DEFENSIVE(u),
                                       sprinttype(nullptr, CHAR_ATTACK_TYPE(u), g_WpnColl.text),
                                       CHAR_SPEED(u),
                                       CHAR_NATURAL_ARMOUR(u),
                                       (signed long)UNIT_HIT(u),
                                       hit_limit(u),
                                       hit_gain(u),
                                       CHAR_MANA(u),
                                       mana_limit(u),
                                       mana_gain(u),
                                       CHAR_ENDURANCE(u),
                                       move_limit(u),
                                       move_gain(u),
                                       CHAR_STR(u),
                                       CHAR_DEX(u),
                                       CHAR_CON(u),
                                       CHAR_CHA(u),
                                       CHAR_BRA(u),
                                       CHAR_MAG(u),
                                       CHAR_DIV(u),
                                       CHAR_HPP(u));
        send_to_char(msg, ch);

        if (IS_PC(u))
        {
            /* Stat on a player  */
            time_info_data tid1 = age(u);
            time_info_data tid2 = real_time_passed((time_t)PC_TIME(u).played, 0);

            strcpy(tmp, ctime(&PC_TIME(u).connect));
            auto msg2 = diku::format_to_str("----------------- PLAYER -------------------<br/>"
                                            "Filename [%s]  Unique ID [%ld]  BBS [%3d]  Cracks [%2d]<br/>"
                                            "Skill points: [%ld]  Ability points: [%ld]  CRIMES: [%d]<br/>"
                                            "Hometown: [%s] Prompt: [%s]<br/>VLVL: %d (%d lxp) <br/>"
                                            "Profession: [%d %s]  Guild default: [%s]<br/>"
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
                                            (signed long)PC_ABILITY_POINTS(u),
                                            PC_CRIMES(u),
                                            STR(PC_HOME(u)),
                                            STR(UPC(u)->promptstr),
                                            PC_VIRTUAL_LEVEL(u),
                                            required_xp(PC_VIRTUAL_LEVEL(u) + 1) - required_xp(PC_VIRTUAL_LEVEL(u)),
                                            PC_PROFESSION(u),
                                            PC_PROFESSION(u) >= 0 && PC_PROFESSION(u) <= PROFESSION_MAX ? g_professions[PC_PROFESSION(u)]
                                                                                                        : "Undefined",
                                            STR(PC_GUILD(u)),
                                            PC_COND(u, DRUNK),
                                            PC_COND(u, FULL),
                                            PC_COND(u, THIRST),
                                            PC_ACCOUNT(u).last4 == -1 ? "NONE" : "SET",
                                            sprintbit(bits1, PC_FLAGS(u), g_pc_flags),
                                            tid1.getYear(),
                                            tid1.getMonth(),
                                            tid1.getDay(),
                                            tid1.getHours(),
                                            PC_LIFESPAN(u),
                                            tid2.getDay(),
                                            tid2.getHours(),
                                            PC_TIME(u).played,
                                            tmp,
                                            ctime(&PC_TIME(u).creation));

            send_to_char(msg2, ch);
        }
        else /* Stat on a monster */
        {
            auto msg2 = diku::format_to_str("---------------- NON PLAYER ----------------<br/>"
                                            "Default position: %s<br/>"
                                            "NPC-flags: %s<br/>",
                                            sprinttype(nullptr, NPC_DEFAULT(u), g_char_pos),
                                            sprintbit(bits1, NPC_FLAGS(u), g_npc_flags));
            send_to_char(msg2, ch);
        }
    }
    else if (IS_OBJ(u)) /* Stat on an object */
    {
        auto msg = diku::format_to_str("Object data:<br/>"
                                       "Object type: %s (%d)<br/>"
                                       "Values: [%4ld] [%4ld] [%4ld] [%4ld] [%4ld]<br/>"
                                       "Magic resistance [%d]<br/><br/>"
                                       "%s<br/>"
                                       "Extra flags: %s<br/>"
                                       "Cost: [%lu]  Cost/day: [%lu]  Equipped: %s<br/>",
                                       sprinttype(nullptr, OBJ_TYPE(u), g_obj_types),
                                       OBJ_TYPE(u),
                                       (signed long)OBJ_VALUE(u, 0),
                                       (signed long)OBJ_VALUE(u, 1),
                                       (signed long)OBJ_VALUE(u, 2),
                                       (signed long)OBJ_VALUE(u, 3),
                                       (signed long)OBJ_VALUE(u, 4),
                                       OBJ_RESISTANCE(u),
                                       stat_obj_data(u, wstat_obj_type),
                                       sprintbit(bits1, OBJ_FLAGS(u), g_obj_flags),
                                       (unsigned long)OBJ_PRICE(u),
                                       (unsigned long)OBJ_PRICE_DAY(u),
                                       sprinttype(nullptr, OBJ_EQP_POS(u), g_equip_pos));
        send_to_char(msg, ch);
    }
    else /* Stat on a room */
    {
        auto msg = diku::format_to_str("Room data:<br/>"
                                       "%s [%s@%s]  Sector type: %s<br/>"
                                       "Map (%d,%d) Magic resistance [%d]<br/>Outside Environment: %s<br/>",
                                       UNIT_TITLE_STRING(u),
                                       UNIT_FI_NAME(u),
                                       UNIT_FI_ZONENAME(u),
                                       sprinttype(nullptr, ROOM_LANDSCAPE(u), g_room_landscape),
                                       UROOM(u)->mapx,
                                       UROOM(u)->mapy,
                                       ROOM_RESISTANCE(u),
                                       UNIT_IN(u) ? STR(TITLENAME(UNIT_IN(u))) : "Nothing");
        send_to_char(msg, ch);

        for (i = 0; i <= MAX_EXIT; i++)
        {
            if (ROOM_EXIT(u, i))
            {
                cname = ROOM_EXIT(u, i)->open_name.catnames();
                sprintbit(bits2, ROOM_EXIT(u, i)->exit_info, g_unit_open_flags);

                if (ROOM_EXIT(u, i)->to_room)
                {
                    msg = diku::format_to_str("EXIT %-5s to [%s@%s] (%s)<br/>"
                                              "   Exit Name: [%s]<br/>"
                                              "   Exit Bits: [%s] Difficulty: [%d]<br/>"
                                              "   Key: [%s]<br/>",
                                              g_dirs[i],
                                              UNIT_FI_NAME(ROOM_EXIT(u, i)->to_room),
                                              UNIT_FI_ZONENAME(ROOM_EXIT(u, i)->to_room),
                                              UNIT_TITLE_STRING(ROOM_EXIT(u, i)->to_room),
                                              cname,
                                              &bits2[0],
                                              ROOM_EXIT(u, i)->difficulty,
                                              ROOM_EXIT(u, i)->key ? ROOM_EXIT(u, i)->key : "");
                }
                else
                {
                    msg = diku::format_to_str("EXIT %-5s to [NOWHERE]<br/>"
                                              "   Exit Name: [%s]<br/>"
                                              "   Exit Bits: [%s]<br/>",
                                              g_dirs[i],
                                              cname,
                                              &bits2[0]);
                }
                FREE(cname);
                send_to_char(msg, ch);
            }
        }
    }
}

static void stat_contents(const unit_data *ch, unit_data *u)
{
    int bright = 0;
    int light = 0;
    unit_data *orgu = nullptr;

    orgu = u;

    if (UNIT_CONTAINS(u))
    {
        for (u = UNIT_CONTAINS(u); u; u = u->next)
        {
            if (CHAR_LEVEL(ch) >= UNIT_MINV(u))
            {
                auto msg = diku::format_to_str("[%s@%s] Name '%s', Title '%s'  %s (L%d B%d)<br/>",
                                               UNIT_FI_NAME(u),
                                               UNIT_FI_ZONENAME(u),
                                               UNIT_NAME(u),
                                               UNIT_TITLE_STRING(u),
                                               IS_OBJ(u) && OBJ_EQP_POS(u) ? "Equipped" : "",
                                               UNIT_LIGHTS(u),
                                               UNIT_BRIGHT(u));
                send_to_char(msg, ch);
                bright += UNIT_BRIGHT(u);
                light += UNIT_LIGHTS(u);
            }
        }
        auto msg = diku::format_to_str("Contents lights sum = %d, bright sum = %d<br/>", light, bright);
        send_to_char(msg, ch);
        msg = diku::format_to_str("Parent unit lights total = %d (bright %d)<br/>", UNIT_LIGHTS(orgu), UNIT_BRIGHT(orgu));
        send_to_char(msg, ch);
    }
    else
    {
        send_to_char("It is empty.<br/>", ch);
    }
}

static void stat_descriptor(const unit_data *ch, unit_data *u)
{
    send_to_char("Is yet to be programmed.<br/>", ch);
}

void do_wedit(unit_data *ch, char *argument, const command_info *cmd)
{
    unit_data *u = nullptr;

    if (CHAR_DESCRIPTOR(ch) == nullptr)
    {
        return;
    }

    if (str_ccmp("room", argument) == 0)
    {
        u = UNIT_IN(ch);
    }
    else
    {
        u = find_unit(ch, &argument, nullptr, FIND_UNIT_GLOBAL);

        if (u == nullptr)
        {
            char name[MAX_INPUT_LENGTH + 1];
            char zone[MAX_INPUT_LENGTH + 1];

            split_fi_ref(argument, zone, name);

            if (*name && !*zone)
            {
                strcpy(zone, unit_zone(ch)->getName());
            }

            u = find_symbolic(zone, name);

            if (!u)
            {
                send_to_char("No such thing anywhere.<br/>", ch);
                return;
            }
        }
    }

    std::string t;

    t = "<data type='$json'>";
    t.append(u->json());
    t.append("</data>");
    send_to_char(t.c_str(), ch);

    send_to_char("Sending json edit info to your browser.<br/>", ch);
}

void do_wstat(unit_data *ch, char *argument, const command_info *cmd)
{
    char buf[4 * MAX_STRING_LENGTH];
    unit_data *u = nullptr;
    zone_type *zone = nullptr;

    if (CHAR_DESCRIPTOR(ch) == nullptr)
    {
        return;
    }

    if (str_is_empty(argument))
    {
        send_to_char("Usage: See help wstat<br/>[room|zone|memory|account|creators|count <no>|world [dil (ms)|extra|zone]|unit-name]<br/>",
                     ch);
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
        {
            stat_world_extra(ch);
        }
        return;
    }
    else
    {
        file_index_type *fi = nullptr;

        u = find_unit(ch, &argument, nullptr, FIND_UNIT_GLOBAL);

        if (u == nullptr)
        {
            fi = pc_str_to_file_index(ch, argument);

            if (fi)
            {
                if (fi->getType() == UNIT_ST_ROOM)
                {
                    u = fi->Front(); // Shouldn't be empty
                }
                else
                {
                    if (fi->getNumInMem() == 0)
                    {
                        if (!IS_ADMINISTRATOR(ch))
                        {
                            send_to_char("No instances in memory.<br/>", ch);
                        }
                        else
                        {
                            do_load(ch, argument, cmd);
                        }
                    }
                    u = fi->find_symbolic_instance();
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
        {
            stat_zone(ch, zone);
        }
        else
        {
            extra_stat_zone(ch, argument, zone);
        }

        return;
    }
    argument = one_argument(argument, buf);
    if (strlen(buf) < 1)
    {
        stat_normal(ch, u);
    }
    else if (!strncmp(buf, "data", strlen(buf)))
    {
        stat_data(ch, u);
    }
    else if (!strncmp(buf, "contents", strlen(buf)))
    {
        stat_contents(ch, u);
    }
    else if (!strncmp(buf, "affects", strlen(buf)))
    {
        stat_affect(ch, u);
    }
    else if (!strncmp(buf, "descriptor", strlen(buf)))
    {
        stat_descriptor(ch, u);
    }
    else if (!strncmp(buf, "functions", strlen(buf)))
    {
        stat_func(ch, u);
    }
    else if (!strncmp(buf, "spells", strlen(buf)))
    {
        stat_spell(ch, u);
    }
    else if (!strncmp(buf, "skills", strlen(buf)))
    {
        stat_skill(ch, u);
    }
    else if (!strncmp(buf, "weapons", strlen(buf)))
    {
        stat_wskill(ch, u);
    }
    else if (!strncmp(buf, "extras", strlen(buf)))
    {
        stat_extra_descr(ch, u, argument);
    }
    else if (!strncmp(buf, "quests", strlen(buf)))
    {
        stat_extra_quest(ch, u, argument);
    }
    else if (!strncmp(buf, "ability", strlen(buf)))
    {
        stat_ability(ch, u);
    }
    else if (!strncmp(buf, "account", strlen(buf)))
    {
        account_local_stat(ch, u);
    }
    else if (!strncmp(buf, "bank", strlen(buf)))
    {
        stat_bank(ch, u);
    }
    else if (!strncmp(buf, "combat", strlen(buf)))
    {
        stat_combat(ch, u, argument);
    }
    else if (!strncmp(buf, "splcombat", strlen(buf)))
    {
        stat_spell(ch, u, argument);
    }
    else if (!strncmp(buf, "info", strlen(buf)))
    {
        stat_extra_info(ch, u, argument);
    }
    else if (!strncmp(buf, "ip", strlen(buf)))
    {
        stat_ip(ch, u);
    }
    else
    {
        stat_normal(ch, u);
    }
}
