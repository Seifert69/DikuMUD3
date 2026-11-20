/*
 * vme -j [zone]: the world as the server holds it after the boot, as JSON lines, for tools (zone
 * difficulty, a combat simulator). One line per unit:
 *
 *   {"kind": "zone"|"room"|"npc"|"object"|"prototype", "id": "sym@zone", "room": "sym@zone" or null,
 *    "derived": {...}, "unit": {...}}
 *
 * The first line is the coins, {"kind": "money", "id": "money", "coins": [{"name", "abbrev", "value"}...]}, each
 * coin's value in the server's base unit (etc/money): every worth in the dump is in that unit.
 *
 * - "unit" is the unit's own toJSON(), as jstat shows it: its programs ("func", with each DIL
 *   template's name and arguments), exits, abilities, values...
 * - "derived" is what toJSON() can't name because vmc compiles it too, without the game's tables:
 *   skills by name, the attack, hit points, the object type, and what a unit holds or wears. And
 *   what it is worth: an object's "worth" (the shops' price before their profit; for coins, the
 *   coins' value), and the "money" a character or container holds, coins inside containers too.
 *   A potion, scroll, wand or staff has "magic": its power, the spells it casts by name and, for
 *   a wand or staff, its charges.
 * - "room", "npc" and "object" are what the boot reset placed; "prototype" is every NPC and object
 *   of the zone loaded fresh, including those only DIL loads. A prototype is in no room, and its
 *   DIL isn't started (it may read and write saved data), so its programs show the zone file's
 *   arguments.
 *
 * The values are the server's after loading (vmc's point distribution, races, bonus_setup, affects),
 * but before any tick: what DIL sets later is not in it, and random treasure is still its
 * rnd*@randomt placeholder. With -t <seconds> the game runs that long first (run_offline), so
 * the random treasure has taken its shape and the NPCs have wandered.
 */
#include "json_dump.h"

#include "constants.h"
#include "db.h"
#include "handler.h"
#include "json_helper.h"
#include "modify.h"
#include "money.h"
#include "slog.h"
#include "utility.h"
#include "utils.h"
#include "vmelimits.h"
#include "zone_type.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <fstream>

using PrettyWriter = rapidjson::PrettyWriter<rapidjson::StringBuffer>;

namespace
{

/// A potion's or scroll's spells are values 1-3, a wand's or staff's 2-3 after its charges (wmacros.h)
void write_magic(unit_data *u, PrettyWriter &writer)
{
    const int type = OBJ_TYPE(u);
    const bool charged = type == ITEM_WAND || type == ITEM_STAFF;
    if (!charged && type != ITEM_POTION && type != ITEM_SCROLL)
    {
        return;
    }
    writer.String("magic");
    writer.StartObject();
    json::write_kvp("power", OBJ_VALUE(u, 0), writer);
    if (charged)
    {
        json::write_kvp("charges", OBJ_VALUE(u, 1), writer);
    }
    writer.String("spells");
    writer.StartArray();
    for (int i = charged ? 2 : 1; i <= 3; i++)
    {
        const int spell = OBJ_VALUE(u, i);
        if (spell > 0 && spell < SPL_TREE_MAX)
        {
            writer.String(g_SplColl.text[spell] ? g_SplColl.text[spell] : ("spell" + std::to_string(spell)).c_str());
        }
    }
    writer.EndArray();
    writer.EndObject();
}

void write_derived(unit_data *u, PrettyWriter &writer)
{
    writer.StartObject();

    writer.String("contents");
    writer.StartArray();
    for (unit_data *c = u->getUnitContains(); c; c = c->getNext())
    {
        writer.StartObject();
        json::write_unit_id_kvp("id", c, writer);
        if (c->isObj())
        {
            json::write_kvp("equipped", sprinttype(nullptr, OBJ_EQP_POS(c), g_obj_pos), writer);
        }
        writer.EndObject();
    }
    writer.EndArray();

    if (u->isNPC())
    {
        writer.String("weapons");
        writer.StartObject();
        for (int i = 0; i < WPN_GROUP_MAX; i++)
        {
            json::write_kvp(g_WpnColl.text[i], NPC_WPN_SKILL(u, i), writer);
        }
        writer.EndObject();

        writer.String("spells");
        writer.StartObject();
        for (int i = 0; i < SPL_GROUP_MAX; i++)
        {
            json::write_kvp(g_SplColl.text[i], NPC_SPL_SKILL(u, i), writer);
        }
        writer.EndObject();
    }

    if (u->isChar())
    {
        json::write_kvp("attack", sprinttype(nullptr, CHAR_ATTACK_TYPE(u), g_WpnColl.text), writer);
        json::write_kvp("hit_limit", hit_limit(u), writer);
    }

    if (u->isObj())
    {
        json::write_kvp("type", sprinttype(nullptr, OBJ_TYPE(u), g_obj_types), writer);
        json::write_kvp("worth", IS_MONEY(u) ? MONEY_VALUE(u) : static_cast<sbit32>(OBJ_PRICE(u)), writer);
        write_magic(u, writer);
    }

    if (u->isChar() || (u->isObj() && OBJ_TYPE(u) == ITEM_CONTAINER))
    {
        json::write_kvp("money", unit_holds_total(u, ANY_CURRENCY), writer);
    }
    writer.EndObject();
}

/// Writes the pretty JSON built in buf to out as one compact line (toJSON() only takes a PrettyWriter)
void write_line(const rapidjson::StringBuffer &buf, std::ofstream &out)
{
    rapidjson::Document doc;
    if (doc.Parse(buf.GetString()).HasParseError())
    {
        slog(LOG_OFF, 0, "vme -j: invalid JSON at offset %zu: %.200s", doc.GetErrorOffset(), buf.GetString());
        out.setstate(std::ios::failbit);
        return;
    }
    rapidjson::StringBuffer line;
    rapidjson::Writer<rapidjson::StringBuffer> writer(line);
    doc.Accept(writer);
    out << line.GetString() << '\n';
}

void write_unit(const char *kind, unit_data *u, std::ofstream &out)
{
    rapidjson::StringBuffer buf;
    PrettyWriter writer(buf);
    writer.StartObject();
    json::write_kvp("kind", kind, writer);
    json::write_unit_id_kvp("id", u, writer);
    json::write_unit_id_kvp("room", u->isRoom() || !u->getUnitIn() ? nullptr : unit_room(u), writer);
    writer.String("derived");
    write_derived(u, writer);
    writer.String("unit");
    u->toJSON(writer);
    writer.EndObject();
    write_line(buf, out);
}

} // namespace

int dump_json(const std::string &zone)
{
    const std::string file = "dump_" + (zone.empty() ? std::string("world") : zone) + ".jsonl";
    std::ofstream out(file);
    if (!out)
    {
        slog(LOG_OFF, 0, "vme -j: can't write %s.", file.c_str());
        return 3;
    }

    auto wanted = [&zone](const char *z) { return zone.empty() || zone == z; };
    int count = 0;

    {
        rapidjson::StringBuffer buf;
        PrettyWriter writer(buf);
        writer.StartObject();
        json::write_kvp("kind", "money", writer);
        json::write_kvp("id", "money", writer);
        writer.String("coins");
        writer.StartArray();
        for (int i = 0; i <= MAX_MONEY; i++)
        {
            writer.StartObject();
            json::write_kvp("name", money_singularis_type(i), writer);
            json::write_kvp("abbrev", g_money_types[i].abbrev, writer);
            json::write_kvp("value", g_money_types[i].relative_value, writer);
            writer.EndObject();
        }
        writer.EndArray();
        writer.EndObject();
        write_line(buf, out);
        count++;
    }

    for (auto &[name, z] : g_zone_info.mmp)
    {
        if (wanted(name.c_str()))
        {
            rapidjson::StringBuffer buf;
            PrettyWriter writer(buf);
            writer.StartObject();
            json::write_kvp("kind", "zone", writer);
            json::write_kvp("id", name, writer);
            writer.String("unit");
            z->toJSON(writer);
            writer.EndObject();
            write_line(buf, out);
            count++;
        }
    }

    // What the boot reset placed: a unit belongs to the zone it comes from or the room it is in
    for (unit_data *u = g_unit_list; u; u = u->getGlobalNext())
    {
        unit_data *room = u->isRoom() || !u->getUnitIn() ? nullptr : unit_room(u);
        if (wanted(u->getFileIndexZoneName()) || (room && wanted(room->getFileIndexZoneName())))
        {
            write_unit(u->isRoom() ? "room" : u->isNPC() ? "npc" : "object", u, out);
            count++;
        }
    }

    // Every NPC and object of the zone loaded fresh: after the instances, as these join the world
    for (auto &[name, z] : g_zone_info.mmp)
    {
        if (!wanted(name.c_str()))
        {
            continue;
        }
        for (auto &[sym, fi] : z->cgetFileIndexMap())
        {
            if (fi->getType() != UNIT_ST_NPC && fi->getType() != UNIT_ST_OBJ)
            {
                continue;
            }
            if (unit_data *u = read_unit(fi.get()))
            {
                write_unit("prototype", u, out);
                count++;
            }
        }
    }

    out.close();
    if (!out)
    {
        slog(LOG_OFF, 0, "vme -j: writing %s failed.", file.c_str());
        return 3;
    }
    slog(LOG_OFF, 0, "vme -j: %d lines written to %s.", count, file.c_str());
    return 0;
}
