#ifdef _WINDOWS
    #include <winsock2.h>
#endif

#include "account.h"
#include "act_wizard.h"
#include "affect.h"
#include "apf_affect.h"
#include "bank.h"
#include "char_follow_type.h"
#include "comm.h"
#include "common.h"
#include "constants.h"
#include "db.h"
#include "dilshare.h"
#include "files.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "json_helper.h"
#include "main_functions.h"
#include "modify.h"
#include "skills.h"
#include "spec_assign.h"
#include "spell_parser.h"
#include "spells.h"
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

/**
 * Controls wrapping the JSON response in <data> tags or not basically
 *
 * @param json
 * @param d
 * @param send_in_html_tag
 */
static void send_json_to_client(std::string json, descriptor_data *d, bool send_in_html_tag = true);

static void stat_world_count(const unit_data *ch, char *arg)
{
    int nMinCount = atoi(arg);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        json::write_kvp("first_40_units_containing_at_least", nMinCount, writer);

        writer.String("units");
        writer.StartArray();
        int n = 0;
        for (unit_data *u = g_unit_list; u; u = u->getGlobalNext())
        {
            int i = 0;
            for (unit_data *t = u->getUnitContains(); t; t = t->getNext())
            { // count top layer
                ++i;
            }

            if (i >= nMinCount)
            {
                writer.StartObject();
                {
                    json::write_kvp("file_index_syn_name", u->getFileIndexSymName(), writer);
                    json::write_kvp("name", u->getNames().Name(), writer);
                    json::write_kvp("no_of_units", i, writer);
                }
                writer.EndObject();
                ++n;

                if (n >= 40)
                {
                    break;
                }
            }
        }
        writer.EndArray();
    }
    writer.EndObject();
    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_world_extra(const unit_data *ch)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        json::write_kvp("world_zones", g_zone_info.no_of_zones, writer);
        writer.String("zones");
        writer.StartArray();
        for (auto zp : g_zone_info.mmp)
        {
            writer.String(zp.second->getName().c_str());
        }
        writer.EndArray();
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_memory(unit_data *ch)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        writer.String("memory");
        writer.StartObject();
        {
            writer.String("event_queue");
            writer.StartObject();
            {
                json::write_kvp("entries", g_events.Count(), writer);
                json::write_kvp("tick_1", g_events.CountNextTicks(1), writer);
                json::write_kvp("tick_1-4", g_events.CountNextTicks(4), writer);
                json::write_kvp("tick_1-10", g_events.CountNextTicks(10), writer);
            }
        }
        writer.EndObject();

        json::write_kvp("current_tic", g_tics, writer);
        json::write_kvp("event_queue_tic", g_events.NextEventTic(), writer);

        writer.String("event_queue_stats");
        writer.StartObject();
        {
            writer.String("global");
            writer.StartObject();
            {
                json::write_kvp("processes", g_events.PCount(), writer);
                json::write_kvp("per_second", g_events.PTime(), writer);
            }
            writer.EndObject();

            writer.String("max_time");
            writer.StartObject();
            {
                json::write_kvp("processes", g_events.Max_PTime_Process(), writer);
                json::write_kvp("per_second", g_events.Max_PTime(), writer);
            }
            writer.EndObject();

            writer.String("max_processes");
            writer.StartObject();
            {
                json::write_kvp("processes", g_events.Max_PCount(), writer);
                json::write_kvp("per_second", g_events.Max_PCount_Time(), writer);
            }
            writer.EndObject();

            writer.String("avg_processes");
            writer.StartObject();
            {
                json::write_kvp("processes", g_events.Avg_PCount(), writer);
                json::write_kvp("per_second", g_events.Avg_PTime(), writer);
            }
            writer.EndObject();
        }
        writer.EndObject();
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_world(unit_data *ch)
{
    time_t now = time(nullptr);

    char *p = ctime(&now);
    p[strlen(p) - 1] = '\0';

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        writer.String("world");
        writer.StartObject();
        {
            writer.String("server");
            writer.StartObject();
            {
                json::write_kvp("compiled_date", g_compile_date, writer);
                json::write_kvp("compiled_time", g_compile_time, writer);
            }
            writer.EndObject();
        }
        writer.EndObject();

        writer.String("status");
        writer.StartObject();
        {
            json::write_kvp("ticks", g_tics, writer);
            json::write_kvp("hours", g_tics / (PULSE_SEC * 3600), writer);
            json::write_kvp("timewarped", g_nTickUsec < 1, writer);
            json::write_kvp("number_of_rooms", room_data::g_world_norooms, writer);
            json::write_kvp("number_of_objects", obj_data::g_world_noobjects, writer);
            json::write_kvp("number_of_chars", char_data::g_world_nochars, writer);
            json::write_kvp("number_of_npc", npc_data::g_world_nonpc, writer);
            json::write_kvp("number_of_pc", pc_data::g_world_nopc, writer);
            json::write_kvp("total_units", room_data::g_world_norooms + obj_data::g_world_noobjects + char_data::g_world_nochars, writer);
            json::write_kvp("number_of_zones", zone_info_type::g_world_nozones, writer);
            json::write_kvp("connections", g_no_connections, writer);
            json::write_kvp("max_connections", g_max_no_connections, writer);
            json::write_kvp("players", g_no_players, writer);
            json::write_kvp("max_players", g_max_no_players, writer);

            writer.String("boottime");
            writer.StartObject();
            {
                json::write_kvp("time", g_world_boottime, writer);
                json::write_kvp("time_now", p, writer);
            }
            writer.EndObject();

            writer.String("DIL");
            writer.StartObject();
            {
                json::write_kvp("programs", g_nDilPrg, writer);
                json::write_kvp("values", g_nDilVal, writer);
            }
            writer.EndObject();
        }
        writer.EndObject();
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_zone_reset(const zone_reset_cmd *zrip, unit_data *ch, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    static const char *nums[] = {"max", "zonemax", "local"};
    std::string bits;
    int i = 0;

    writer.StartArray();
    while (zrip)
    {
        writer.StartObject();
        {
            json::write_kvp("command_no", zrip->getCommandNum(), writer);
            switch (zrip->getCommandNum())
            {
                case 1:
                {
                    json::write_kvp("load", zrip->getFileIndexType(0)->getName(), writer);

                    if (zrip->getFileIndexType(1))
                    {
                        json::write_kvp("into", zrip->getFileIndexType(1)->getName(), writer);
                    }

                    writer.String("nums");
                    writer.StartArray();
                    for (i = 0; i < 3; i++)
                    {
                        if (zrip->getNum(i))
                        {
                            writer.StartObject();
                            {
                                json::write_kvp("num", nums[i], writer);
                                json::write_kvp("value", zrip->getNum(i), writer);
                            }
                            writer.EndObject();
                        }
                    }
                    writer.EndArray();

                    json::write_kvp("complete_flag", zrip->getCompleteFlag(), writer);
                }
                break;

                case 2:
                {
                    writer.String("equipment");
                    writer.StartObject();
                    {
                        json::write_kvp("name", zrip->getFileIndexType(0)->getName(), writer);
                        json::write_kvp("where", g_where[zrip->getNum(1)], writer);
                        json::write_kvp("max", zrip->getNum(0), writer);
                        json::write_kvp("complete", zrip->getCompleteFlag(), writer);
                    }
                    writer.EndObject();
                }
                break;

                case 3:
                {
                    writer.String("door");
                    writer.StartObject();
                    {
                        json::write_kvp("name", zrip->getFileIndexType(0)->getName(), writer);
                        json::write_kvp("direction", g_dirs[zrip->getNum(0)], writer);
                        json::write_kvp("flags", sprintbit(bits, zrip->getNum(1), g_unit_open_flags), writer);
                    }
                    writer.EndObject();
                }
                break;

                case 4:
                {
                    writer.String("purge");
                    writer.StartObject();
                    {
                        json::write_kvp("name", zrip->getFileIndexType(0)->getName(), writer);
                    }
                    writer.EndObject();
                }
                break;

                case 5:
                {
                    writer.String("remove");
                    writer.StartObject();
                    {
                        json::write_kvp("name", zrip->getFileIndexType(0)->getName(), writer);
                        json::write_kvp("in", zrip->getFileIndexType(1)->getName(), writer);
                    }
                    writer.EndObject();
                }
                break;
            }
        }
        writer.EndObject();

        if (zrip->getNested())
        {
            writer.String("child");
            stat_zone_reset(zrip->getNested(), ch, writer);
        }
        zrip = zrip->getNext();
    }
    writer.EndArray();
}

static void stat_zone(unit_data *ch, zone_type *zone)
{
    //    static const char *reset_modes[] = {"Never Reset", "Reset When Empty", "Reset Always", "UNKNOWN"};
    int reset_mode = zone->getResetMode();

    if (!is_in(reset_mode, 0, 2))
    {
        reset_mode = 3;
    }

    bool errors = file_exists(g_cServerConfig.getZoneDir() + zone->getName() + ".err");
    bool info = file_exists(g_cServerConfig.getZoneDir() + zone->getName() + ".inf");

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        json::write_object_pointer_kvp("zone", zone, writer);
        json::write_kvp("errors", errors ? "Errors in zone (stat zone error)" : "No errors registered in zone.", writer);
        json::write_kvp("info", info ? "User info feedback in zone (stat zone info)." : "No user info (feedback) in zone.", writer);
    }
    writer.EndObject();
    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_creators(unit_data *ch, char *arg)
{
    if (str_is_empty(arg))
    {
        send_to_char("Requires name of creator.<br/>", ch);
        return;
    }

    char tmp[1024];
    arg = one_argument(arg, tmp);
    const auto all = str_ccmp(tmp, "all") == 0;

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        json::write_kvp("zones_created_by", tmp, writer);

        writer.String("zones");
        writer.StartArray();
        for (auto &it : g_zone_info.mmp)
        {
            if (all || it.second->getCreators().IsName(tmp))
            {
                writer.StartObject();
                {
                    json::write_kvp("zone_name", it.second->getName(), writer);

                    const std::string filename = it.second->getName() + ".zon";
                    json::write_kvp("filename", filename, writer);
                }
                writer.EndObject();
            }
        }
        writer.EndArray();
    }
    writer.EndObject();
    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_dil(unit_data *ch, zone_type *zone)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        writer.String("dil_stats");
        zone->getStatDILJSON(writer);
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_global_dil(unit_data *ch, ubit32 nCount)
{
    ubit64 instructionSum = 0;
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        json::write_kvp("global_dil_running_for_more_than", nCount, writer);

        writer.String("zones");
        writer.StartArray();
        for (auto &z : g_zone_info.mmp)
        {
            json::write_kvp("zone_name", z.first, writer);

            writer.String("dil_global_stat");
            z.second->getStatGlobalDILJSON(nCount, instructionSum, writer);
        }
        writer.EndArray();

        json::write_kvp("total_dil_instructions", instructionSum, writer);
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void extra_stat_zone(unit_data *ch, char *arg, zone_type *zone)
{
    char buf[MAX_STRING_LENGTH];
    int argno = 0;
    //    int search_type = 0;

    //  void stat_dijkstraa (class unit_data * ch, class zone_type *z);

    static const char *zone_args[] = {"mobiles", "objects", "rooms", "reset", "errors", "info", "path", "dil", nullptr};

    static int search_types[] = {UNIT_ST_NPC, UNIT_ST_OBJ, UNIT_ST_ROOM};

    arg = one_argument(arg, buf);
    argno = search_block(buf, zone_args, 0);

    if (argno == -1) /* Asked for a specific zone? */
    {
        if ((zone = find_zone(buf)) == nullptr)
        {
            send_to_char("Usage: jstat zone [name] "
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

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        json::write_kvp("zone", zone->getName(), writer);

        switch (argno)
        {
            case 0:
            case 1:
            case 2:
            {
                writer.String(zone_args[argno]);
                zone->getExtraStatZoneMessageJSON(search_types[argno], writer);
            }
            break;

            case 3:
            {
                if (zone->cgetZoneResetCommands())
                {
                    writer.String("reset_commands");
                    stat_zone_reset(zone->cgetZoneResetCommands(), ch, writer);
                }
            }
            break;

            case 4:
            case 5:
            {
                /* Errors/Info (Small hack, this :-) ) */
                auto filename = diku::format_to_str("%s%s.%.3s", g_cServerConfig.getZoneDir(), zone->getName(), zone_args[argno]);
                if (!file_exists(filename))
                {
                    return;
                }
                file_to_string(filename, buf, MAX_STRING_LENGTH);

                writer.StartObject();
                {
                    json::write_kvp("filename", filename, writer);
                    json::write_kvp("file", buf, writer);
                }
                writer.EndObject();
                return;
            }

            case 6:
            {
                json::write_kvp("error", "Not Implemented", writer);
            }
            break;

            case 7:
            {
                stat_dil(ch, zone);
            }
            break;
        }
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_ability(const unit_data *ch, unit_data *u)
{
    if (!u->isPC())
    {
        send_to_char("Unit is not a PC - use 'data' for NPC's<br/>", ch);
        return;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        // std::string buf{"Char Abilities:<br/>"};

        writer.String("char_abilities");
        writer.StartArray();
        for (int i = 0; i < ABIL_TREE_MAX; i++)
        {
            writer.StartObject();
            {
                json::write_kvp("name", g_AbiColl.text[i], writer);
                json::write_kvp("percentage", CHAR_ABILITY(u, i), writer);
                json::write_kvp("level", PC_ABI_LVL(u, i), writer);
                json::write_kvp("racial", get_racial_ability(CHAR_RACE(u), i), writer);
            }
            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_spell(const unit_data *ch, unit_data *u)
{
    if (!u->isChar())
    {
        send_to_char("Unit is not a char<br/>", ch);
        return;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        writer.String("char_magic_skills");
        const int max = u->isNPC() ? SPL_GROUP_MAX : SPL_TREE_MAX;
        writer.StartArray();
        for (int i = 0; i < max; i++)
        {
            writer.StartObject();
            {
                json::write_kvp("media_spell", IS_SET(g_spell_info[i].media, MEDIA_SPELL), writer);
                json::write_kvp("media_scroll", IS_SET(g_spell_info[i].media, MEDIA_SCROLL), writer);
                json::write_kvp("media_potion", IS_SET(g_spell_info[i].media, MEDIA_POTION), writer);
                json::write_kvp("media_wand", IS_SET(g_spell_info[i].media, MEDIA_WAND), writer);
                json::write_kvp("media_staff", IS_SET(g_spell_info[i].media, MEDIA_STAFF), writer);

                json::write_kvp("percentage", u->isNPC() ? NPC_SPL_SKILL(u, i) : PC_SPL_SKILL(u, i), writer);
                json::write_kvp("level", u->isNPC() ? 0 : PC_SPL_LVL(u, i), writer);
                json::write_kvp("realm", g_spell_info[i].realm == ABIL_DIV ? "D" : (g_spell_info[i].realm == ABIL_MAG ? "M" : "!"), writer);

                json::write_kvp("cast_type",
                                g_spell_info[i].cast_type == SPLCST_CHECK
                                    ? "CHECK "
                                    : (g_spell_info[i].cast_type == SPLCST_RESIST ? "RESIST" : "OTHER "),
                                writer);
                json::write_kvp("name", g_SplColl.text[i], writer);

                char tmpbuf1[100];
                str_next_word(TREE_ISLEAF(g_SplColl.tree, i) ? g_SplColl.text[TREE_PARENT(g_SplColl.tree, i)] : "sphere", tmpbuf1);
                if (TREE_ISLEAF(g_SplColl.tree, i) && strcmp(tmpbuf1, "sphere") == 0)
                {
                    if (g_spell_info[i].tmpl == nullptr && g_spell_info[i].spell_pointer == nullptr)
                    {
                        strcpy(tmpbuf1, "NOT IMPLEMENTED");
                    }
                }
                json::write_kvp("type", tmpbuf1, writer);
                json::write_kvp("racial", get_racial_spells(CHAR_RACE(u), i), writer);
            }
            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_skill(const unit_data *ch, unit_data *u)
{
    if (!u->isChar())
    {
        send_to_char("Unit is not a char<br/>", ch);
        return;
    }

    if (u->isNPC())
    {
        send_to_char("NPC's have no skills.<br/>", ch);
        return;
    }
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        writer.String("other_skills");
        writer.StartArray();
        for (int i = 0; i < SKI_TREE_MAX; i++)
        {
            writer.StartObject();
            {
                json::write_kvp("skill", g_SkiColl.text[i], writer);
                json::write_kvp("percentage", PC_SKI_SKILL(u, i), writer);
                json::write_kvp("level", PC_SKI_LVL(u, i), writer);
                json::write_kvp("racial", get_racial_skill(CHAR_RACE(u), i), writer);
            }
            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_wskill(const unit_data *ch, unit_data *u)
{
    if (!u->isChar())
    {
        send_to_char("Unit is not a char<br/>", ch);
        return;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        const int max = u->isNPC() ? WPN_GROUP_MAX : WPN_TREE_MAX;
        writer.String("char_weapon_skill");
        writer.StartArray();
        for (int i = 0; i < max; i++)
        {
            writer.StartObject();
            {
                const auto wpn_skill = u->isNPC() ? NPC_WPN_SKILL(u, i) : PC_WPN_SKILL(u, i);
                const auto level = u->isNPC() ? 0 : PC_WPN_LVL(u, i);

                json::write_kvp("weapon", g_WpnColl.text[i], writer);
                json::write_kvp("percentage", wpn_skill, writer);
                json::write_kvp("level", level, writer);
                json::write_kvp("racial", get_racial_weapon(CHAR_RACE(u), i), writer);
            }
            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_affect(const unit_data *ch, unit_data *u)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        writer.String("unit_affects");

        writer.StartArray();
        for (unit_affected_type *af = u->getUnitAffected(); af; af = af->getNext())
        {
            af->toJSON(writer);
        }
        writer.EndArray();
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_func(const unit_data *ch, unit_data *u)
{
    std::string bits;
    unit_fptr *f = nullptr;

    if (!u->getFunctionPointer())
    {
        send_to_char("It has no special routines.<br/>", ch);
        return;
    }

    send_to_char("Unit functions:<br/>", ch);
    for (f = u->getFunctionPointer(); f; f = f->getNext())

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
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    std::string bits;
    writer.StartObject();
    {
        writer.String("unit_status");
        writer.StartObject();
        {
            json::write_kvp("type", sprintbit(bits, u->getUnitType(), g_unit_status), writer);
            json::write_kvp("name", u->getFileIndexSymName(), writer);
            json::write_kvp("copies", u->getFileIndex() ? u->getFileIndex()->getNumInMem() : -1, writer);
            json::write_kvp("crc", u->getFileIndex() ? (unsigned long)u->getFileIndex()->getCRC() : 0, writer);
        }
        writer.EndObject();

        json::write_object_value_kvp("namelist", u->getNames(), writer);
        json::write_kvp("title", u->getTitle(), writer);
        json::write_kvp("outside_descr", u->getDescriptionOfOutside(), writer);
        json::write_kvp("inside_descr", u->getDescriptionOfInside(), writer);

        writer.String("light_info");
        writer.StartObject();
        {
            json::write_kvp("lights", u->getNumberOfActiveLightSources(), writer);
            json::write_kvp("bright", u->getLightOutput(), writer);
            json::write_kvp("transparent_illumination", u->getTransparentLightOutput(), writer);
        }
        writer.EndObject();

        json::write_kvp("characters_inside", u->getNumberOfCharactersInsideUnit(), writer);
        json::write_kvp("level_of_wizard_invisibility", u->getLevelOfWizardInvisibility(), writer);
        json::write_kvp("unit_is_inside", u->getUnitIn() ? STR(TITLENAME(u->getUnitIn())) : "Nothing", writer);
        json::write_kvp("unit", u->getUnitContains() ? "has contents" : "is empty", writer);
        json::write_kvp("manipulate", sprintbit(bits, u->getManipulate(), g_unit_manipulate), writer);
        json::write_kvp("flags", sprintbit(bits, u->getUnitFlags(), g_unit_flags), writer);

        writer.String("hitpoints");
        writer.StartObject();
        {
            json::write_kvp("current", u->getCurrentHitpoints(), writer);
            json::write_kvp("max", u->getMaximumHitpoints(), writer);
        }
        writer.EndObject();

        json::write_kvp("alignment", u->getAlignment(), writer);
        json::write_kvp("key_name", u->getKey() ? u->getKey() : "none", writer);
        json::write_kvp("open_flags", sprintbit(bits, u->getOpenFlags(), g_unit_open_flags), writer);
        json::write_kvp("open_difficulty", u->getOpenDifficulty(), writer);
        json::write_kvp("base_weight", u->getBaseWeight(), writer);
        json::write_kvp("weight", u->getWeight(), writer);
        json::write_kvp("capacity", u->getCapacity(), writer);
        json::write_kvp("size", u->getSize(), writer);
    }
    writer.EndObject();
    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_extra(const unit_data *ch, extra_list &elist, char *grp, rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    writer.StartArray();
    for (extra_descr_data *ed = elist.m_pList; ed; ed = ed->next)
    {
        if (ed->names.StrStr(grp))
        {
            writer.String("names");
            ed->names.toJSON(writer);

            if (ed->vals.Length() > 0)
            {
                writer.String("values");
                ed->vals.toJSON(writer);
            }
            json::write_kvp("description", ed->descr, writer);
        }
        else
        {
            writer.String("names");
            ed->names.toJSON(writer);

            if (ed->vals.Length() > 0)
            {
                writer.String("values");
                ed->vals.toJSON(writer);
            }
        }
    }
    writer.EndArray();
}

static void stat_extra_descr(const unit_data *ch, unit_data *u, char *grp)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        stat_extra(ch, u->getExtraList(), grp, writer);
    }
    writer.EndObject();
    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_extra_quest(const unit_data *ch, unit_data *u, char *grp)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    {
        if (u->isPC())
        {
            stat_extra(ch, PC_QUEST(u), grp, writer);
        }
        else
        {
            json::write_kvp("error", "Quests only on Players.", writer);
        }
    }
    writer.EndObject();
    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_extra_info(const unit_data *ch, unit_data *u, char *grp)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        if (!IS_ADMINISTRATOR(ch))
        {
            json::write_kvp("error", "This information is classified for administrators only.", writer);
        }
        else
        {
            if (u->isPC())
            {
                writer.String("extra");
                stat_extra(ch, PC_INFO(u), grp, writer);
            }
            else
            {
                json::write_kvp("error", "Information is only on Players.", writer);
            }
        }
    }
    writer.EndObject();
    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_ip(const unit_data *ch, unit_data *u)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        if (!IS_ADMINISTRATOR(ch))
        {
            json::write_kvp("error", "This information is classified for administrators only.", writer);
        }
        else
        {
            if (u->isPC())
            {
                sockaddr_in sock;

                writer.String("ip");
                writer.StartArray();
                for (int i = 0; i < 5; i++)
                {
                    sock.sin_addr.s_addr = UPC(u)->getLastHostAtIndex(i);
                    writer.String(inet_ntoa(sock.sin_addr));
                }
                writer.EndArray();
            }
            else
            {
                json::write_kvp("error", "Information is only on Players.", writer);
            }
        }
    }
    writer.EndObject();
    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

#define STR_DATA(num)                                                                                                                      \
    (pobjdata[idx].v[num] == 0                                                                                                             \
         ? int_str[num]                                                                                                                    \
         : (pobjdata[idx].v[num] == 1 ? (OBJ_VALUE(u, num) ? sprinttype(nullptr, OBJ_VALUE(u, num), g_SplColl.text) : "None")              \
                                      : (pobjdata[idx].v[num] == 2 ? sprinttype(nullptr, OBJ_VALUE(u, num), g_WpnColl.text) : "")))

const char *stat_obj_jdata(unit_data *u, obj_type_t *pobjdata)
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
     *       2 means print the weapon-type (not used for jstat())
     *       3 means print the empty string (i.e. skip)
     *            A 6th trailing format-argument inserts a special string
     *              as according to the switch following...
     */
    char *cname = nullptr;
    static obj_type_t jstat_obj_type[] = {
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

    std::string bits1;
    std::string bits2;
    int i = 0;
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        u->toJSON(writer);
    }
    writer.EndObject();

    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
    return;
    // TODO finish this

    if (false)
    {
    }
    else if (u->isObj()) /* Stat on an object */
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
                                       stat_obj_jdata(u, jstat_obj_type),
                                       sprintbit(bits1, OBJ_FLAGS(u), g_obj_flags),
                                       (unsigned long)OBJ_PRICE(u),
                                       (unsigned long)OBJ_PRICE_DAY(u),
                                       sprinttype(nullptr, OBJ_EQP_POS(u), g_equip_pos));
        send_to_char(msg, ch);
    }
    else /* Stat on a room */
    {
        auto msg = diku::format_to_str("Room data:<br/>"
                                       "%s [%s]  Sector type: %s<br/>"
                                       "Map (%d,%d) Magic resistance [%d]<br/>Outside Environment: %s<br/>",
                                       u->getTitle(),
                                       u->getFileIndexSymName(),
                                       sprinttype(nullptr, ROOM_LANDSCAPE(u), g_room_landscape),
                                       UROOM(u)->getMapXCoordinate(),
                                       UROOM(u)->getMapYCoordinate(),
                                       ROOM_RESISTANCE(u),
                                       u->getUnitIn() ? STR(TITLENAME(u->getUnitIn())) : "Nothing");
        send_to_char(msg, ch);

        for (i = 0; i <= MAX_EXIT; i++)
        {
            if (ROOM_EXIT(u, i))
            {
                cname = ROOM_EXIT(u, i)->getOpenName().catnames();
                sprintbit(bits2, ROOM_EXIT(u, i)->getDoorFlags(), g_unit_open_flags);

                if (ROOM_EXIT(u, i)->getToRoom())
                {
                    msg = diku::format_to_str("EXIT %-5s to [%s] (%s)<br/>"
                                              "   Exit Name: [%s]<br/>"
                                              "   Exit Bits: [%s] Difficulty: [%d]<br/>"
                                              "   Key: [%s]<br/>",
                                              g_dirs[i],
                                              ROOM_EXIT(u, i)->getToRoom()->getFileIndexSymName(),
                                              ROOM_EXIT(u, i)->getToRoom()->getTitle(),
                                              cname,
                                              &bits2[0],
                                              ROOM_EXIT(u, i)->getSkillDifficulty(),
                                              ROOM_EXIT(u, i)->getKey() ? ROOM_EXIT(u, i)->getKey() : "");
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
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        int bright = 0;
        int light = 0;
        unit_data *orgu = u;

        writer.String("contains");
        if (u->getUnitContains())
        {
            writer.StartObject();
            {
                writer.String("contents");
                writer.StartArray();
                for (u = u->getUnitContains(); u; u = u->getNext())
                {
                    if (CHAR_LEVEL(ch) >= u->getLevelOfWizardInvisibility())
                    {
                        writer.StartObject();
                        {
                            json::write_kvp("id", u->getFileIndexSymName(), writer);
                            json::write_kvp("name", u->getNames().Name(), writer);
                            json::write_kvp("title", u->getTitle(), writer);
                            json::write_kvp("is_equipped", u->isObj() && OBJ_EQP_POS(u) ? "Equipped" : "", writer);
                            json::write_kvp("no_of_active_light_sources", u->getNumberOfActiveLightSources(), writer);
                            json::write_kvp("light_output", u->getLightOutput(), writer);
                        }
                        writer.EndObject();
                        bright += u->getLightOutput();
                        light += u->getNumberOfActiveLightSources();
                    }
                }
                writer.EndArray();

                json::write_kvp("light_sum", light, writer);
                json::write_kvp("bright_sum", bright, writer);
                json::write_kvp("parent_unit_light", orgu->getNumberOfActiveLightSources(), writer);
                json::write_kvp("parent_unit_bright", orgu->getLightOutput(), writer);
            }
            writer.EndObject();
        }
        else
        {
            writer.String("It is empty");
        }
    }
    writer.EndObject();
    send_json_to_client(buffer.GetString(), const_cast<descriptor_data *>(CHAR_DESCRIPTOR(ch)));
}

static void stat_descriptor(const unit_data *ch, unit_data *u)
{
    send_to_char("Is yet to be programmed.<br/>", ch);
}

void do_jstat(unit_data *ch, char *argument, const command_info *cmd)
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
        send_to_char("Usage: See help jstat<br/>[room|zone|memory|account|creators|count [no]|world [dil (ms)|extra|zone]|unit-name]<br/>",
                     ch);
        return;
    }

    if (!str_nccmp("room", argument, 4))
    {
        u = ch->getUnitIn();
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

static void send_json_to_client(std::string json, descriptor_data *d, bool send_in_html_tag)
{
    std::string data;
    if (send_in_html_tag)
    {
        data = "<data type='$json'>";
        data += json;
        data += "</data>";
    }
    else
    {
        data = std::move(json);
    }

    send_to_descriptor(data, d);
}