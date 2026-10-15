#include "combat_packet.h"

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * Combat_Packet_Suite - the '$combat' packet sent to web clients each combat round
 */
BOOST_AUTO_TEST_SUITE(Combat_Packet_Suite)

static combat_packet::Entry entry(const char *id, const char *name, int hp, const char *target, bool me, bool foe)
{
    combat_packet::Entry e;
    e.id = id;
    e.name = name;
    e.hp = hp;
    e.target = target;
    e.me = me;
    e.foe = foe;
    return e;
}

BOOST_AUTO_TEST_CASE(list_json_format)
{
    std::vector<combat_packet::Entry> list;
    list.push_back(entry("a1b2c3", "the mangy mongrel", 62, "you", false, true));
    list.push_back(entry("x9", "Moira", 88, "the mangy mongrel", false, false));
    list.push_back(entry("s1", "you", 80, "the mangy mongrel", true, false));

    BOOST_TEST(combat_packet::listJSON(17, list) ==
               "{\"round\":17,\"list\":["
               "{\"id\":\"a1b2c3\",\"name\":\"the mangy mongrel\",\"hp\":62,\"target\":\"you\",\"me\":false,\"foe\":true},"
               "{\"id\":\"x9\",\"name\":\"Moira\",\"hp\":88,\"target\":\"the mangy mongrel\",\"me\":false,\"foe\":false},"
               "{\"id\":\"s1\",\"name\":\"you\",\"hp\":80,\"target\":\"the mangy mongrel\",\"me\":true,\"foe\":false}]}");
}

BOOST_AUTO_TEST_CASE(empty_target_and_list)
{
    std::vector<combat_packet::Entry> list;
    list.push_back(entry("s1", "you", 100, "", true, false));
    BOOST_TEST(combat_packet::listJSON(1, list) ==
               "{\"round\":1,\"list\":[{\"id\":\"s1\",\"name\":\"you\",\"hp\":100,\"target\":\"\",\"me\":true,\"foe\":false}]}");
    BOOST_TEST(combat_packet::listJSON(2, {}) == "{\"round\":2,\"list\":[]}");
}

BOOST_AUTO_TEST_CASE(names_are_json_escaped)
{
    std::vector<combat_packet::Entry> list;
    list.push_back(entry("i", "Bob \"the\" \\Bad\\\n", 5, "Dain's ghost", false, true));

    BOOST_TEST(combat_packet::listJSON(3, list) ==
               "{\"round\":3,\"list\":[{\"id\":\"i\",\"name\":\"Bob \\\"the\\\" \\\\Bad\\\\\\n\",\"hp\":5,"
               "\"target\":\"Dain's ghost\",\"me\":false,\"foe\":true}]}");
}

BOOST_AUTO_TEST_CASE(control_characters_are_json_escaped)
{
    std::vector<combat_packet::Entry> list;
    list.push_back(entry("i", "a\tb\x01", 5, "", false, true));
    auto json = combat_packet::listJSON(3, list);
    BOOST_TEST(json.find("\"name\":\"a\\tb\\u0001\"") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(end_and_death_json)
{
    BOOST_TEST(combat_packet::endJSON(42) == "{\"round\":42,\"list\":[],\"end\":true}");
    BOOST_TEST(combat_packet::deathJSON("abc123") == "{\"event\":\"death\",\"id\":\"abc123\"}");
}

BOOST_AUTO_TEST_CASE(wrap_in_data_tag)
{
    BOOST_TEST(combat_packet::wrap("{\"event\":\"death\",\"id\":\"x\"}") == "<data type='$combat'>{\"event\":\"death\",\"id\":\"x\"}</data>");
}

BOOST_AUTO_TEST_CASE(wrap_keeps_html_out_of_the_payload)
{
    // A name like "</data><script>" must not end the data element or open a tag
    std::vector<combat_packet::Entry> list;
    list.push_back(entry("i", "</data><b>&amp;", 5, "", false, true));
    auto packet = combat_packet::wrap(combat_packet::listJSON(1, list));

    BOOST_TEST(packet ==
               "<data type='$combat'>{\"round\":1,\"list\":[{\"id\":\"i\","
               "\"name\":\"\\u003c/data\\u003e\\u003cb\\u003e\\u0026amp;\",\"hp\":5,\"target\":\"\",\"me\":false,\"foe\":true}]}</data>");

    auto payload = packet.substr(21, packet.size() - 21 - 7);
    BOOST_TEST(payload.find_first_of("<>&") == std::string::npos);
}

BOOST_AUTO_TEST_CASE(hp_percent)
{
    BOOST_TEST(combat_packet::hpPercent(62, 100) == 62);
    BOOST_TEST(combat_packet::hpPercent(1, 3) == 33);
    BOOST_TEST(combat_packet::hpPercent(150, 100) == 100);
    BOOST_TEST(combat_packet::hpPercent(-10, 100) == 0);
    BOOST_TEST(combat_packet::hpPercent(10, 0) == 0);
    BOOST_TEST(combat_packet::hpPercent(10, -5) == 0);
    BOOST_TEST(combat_packet::hpPercent(2000000000L, 2100000000L) == 95);
}

BOOST_AUTO_TEST_CASE(opaque_id_is_stable_and_not_the_pointer)
{
    int a = 0;
    int b = 0;
    char hex[32];
    snprintf(hex, sizeof(hex), "%llx", static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(&a)));

    auto id = combat_packet::opaqueId(&a, 12345);
    BOOST_TEST(id == combat_packet::opaqueId(&a, 12345));
    BOOST_TEST(id != combat_packet::opaqueId(&b, 12345));
    BOOST_TEST(id != combat_packet::opaqueId(&a, 54321));
    BOOST_TEST(id.size() == 12U);
    BOOST_TEST(id.find_first_not_of("0123456789abcdef") == std::string::npos);
    BOOST_TEST(id.find(hex) == std::string::npos);

    // The salted variant is stable within the process too
    BOOST_TEST(combat_packet::opaqueId(&a) == combat_packet::opaqueId(&a));
    BOOST_TEST(combat_packet::opaqueId(&a) != combat_packet::opaqueId(&b));
}

BOOST_AUTO_TEST_SUITE_END()
