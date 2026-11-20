#include "FixtureBase.h"
#include "json_helper.h"
#include "unit_data.h"

#include <rapidjson/document.h>

#include <memory>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * JsonHelper_Suite - the shared JSON writers
 */
BOOST_AUTO_TEST_SUITE(JsonHelper_Suite)

template<typename Write>
static rapidjson::Document to_json(Write write)
{
    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
    writer.StartObject();
    write(writer);
    writer.EndObject();
    rapidjson::Document doc;
    doc.Parse(buf.GetString());
    return doc;
}

BOOST_FIXTURE_TEST_CASE(bits_are_a_list_of_names, unit_tests::FixtureBase)
{
    const char *names[] = {"open-close", "closed", "locked", nullptr};
    auto doc = to_json([&](auto &writer) { json::write_bits_kvp("flags", 0x06 | 0x80, names, writer); });

    const auto &flags = doc["flags"];
    BOOST_REQUIRE(flags.Size() == 3);
    BOOST_TEST(flags[0].GetString() == std::string("closed"));
    BOOST_TEST(flags[1].GetString() == std::string("locked"));
    BOOST_TEST(flags[2].GetString() == std::string("bit7")); // past the names
}

BOOST_FIXTURE_TEST_CASE(no_bits_are_an_empty_list, unit_tests::FixtureBase)
{
    const char *names[] = {"open-close", nullptr};
    auto doc = to_json([&](auto &writer) { json::write_bits_kvp("flags", 0, names, writer); });
    BOOST_TEST(doc["flags"].Size() == 0);
}

// A unit without a file index: getID() used to dereference the missing index
BOOST_FIXTURE_TEST_CASE(unit_id_without_a_file_index, unit_tests::FixtureBase)
{
    std::unique_ptr<unit_data> npc{new_unit_data(UNIT_ST_NPC, nullptr)};
    auto doc = to_json([&](auto &writer) { json::write_unit_id_kvp("id", npc.get(), writer); });
    BOOST_TEST(doc["id"].GetString() == std::string("NO-NAME@NO-ZONE"));
}

BOOST_AUTO_TEST_SUITE_END()
