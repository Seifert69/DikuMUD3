#include "combat_packet.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <cstdio>
#include <random>

namespace combat_packet
{
std::string listJSON(int round, const std::vector<Entry> &list, bool end)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    writer.Key("round");
    writer.Int(round);
    writer.Key("list");
    writer.StartArray();
    for (const auto &e : list)
    {
        writer.StartObject();
        writer.Key("id");
        writer.String(e.id.c_str(), static_cast<rapidjson::SizeType>(e.id.size()));
        writer.Key("name");
        writer.String(e.name.c_str(), static_cast<rapidjson::SizeType>(e.name.size()));
        writer.Key("hp");
        writer.Int(e.hp);
        writer.Key("target");
        writer.String(e.target.c_str(), static_cast<rapidjson::SizeType>(e.target.size()));
        writer.Key("me");
        writer.Bool(e.me);
        writer.Key("foe");
        writer.Bool(e.foe);
        writer.EndObject();
    }
    writer.EndArray();
    if (end)
    {
        writer.Key("end");
        writer.Bool(true);
    }
    writer.EndObject();

    return buffer.GetString();
}

std::string endJSON(int round)
{
    return listJSON(round, {}, true);
}

std::string deathJSON(const std::string &id)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    writer.Key("event");
    writer.String("death");
    writer.Key("id");
    writer.String(id.c_str(), static_cast<rapidjson::SizeType>(id.size()));
    writer.EndObject();

    return buffer.GetString();
}

std::string wrap(const std::string &json)
{
    // The payload travels as the text content of an HTML element, so keep
    // it free of HTML-significant characters: < > & only ever occur inside
    // JSON strings, where \u escapes are equivalent.
    std::string out = "<data type='$combat'>";
    for (char c : json)
    {
        switch (c)
        {
            case '<':
                out.append("\\u003c");
                break;
            case '>':
                out.append("\\u003e");
                break;
            case '&':
                out.append("\\u0026");
                break;
            default:
                out.push_back(c);
        }
    }
    out.append("</data>");
    return out;
}

int hpPercent(long hp, long max)
{
    if (max <= 0 || hp <= 0)
    {
        return 0;
    }
    if (hp >= max)
    {
        return 100;
    }
    return static_cast<int>((hp * 100) / max);
}

std::string opaqueId(const void *p, uint64_t salt)
{
    // splitmix64 finaliser over address ^ salt
    uint64_t z = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(p)) ^ salt;
    z += 0x9e3779b97f4a7c15ULL;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    z = z ^ (z >> 31);

    char buf[17];
    snprintf(buf, sizeof(buf), "%012llx", static_cast<unsigned long long>(z & 0xffffffffffffULL));
    return buf;
}

std::string opaqueId(const void *p)
{
    static const uint64_t salt = []() {
        std::random_device rd;
        return (static_cast<uint64_t>(rd()) << 32) ^ static_cast<uint64_t>(rd());
    }();
    return opaqueId(p, salt);
}
} // namespace combat_packet
