#include "FixtureBase.h"
#include "dil.h"
#include "spec_assign.h"
#include "unit_fptr.h"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include <memory>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * UnitFptr_ToJSON_Suite - toJSON() names the program and gives a DIL program's arguments
 */
BOOST_AUTO_TEST_SUITE(UnitFptr_ToJSON_Suite)

static rapidjson::Document to_json(const unit_fptr &f)
{
    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
    f.toJSON(writer);
    rapidjson::Document doc;
    doc.Parse(buf.GetString());
    return doc;
}

BOOST_FIXTURE_TEST_CASE(names_a_special_function, unit_tests::FixtureBase)
{
    unit_fptr f;
    f.setFunctionPointerIndex(SFUN_DIL_INTERNAL); // without a program it's an ordinary index
    f.setFunctionPriority(25);

    auto doc = to_json(f);
    BOOST_TEST(doc["name"].GetString() == std::string(g_unit_function_array[SFUN_DIL_INTERNAL].name));
    BOOST_TEST(!doc.HasMember("args"));
    BOOST_TEST(doc["priority"].GetInt() == 25);
}

// A dilcopy that hasn't started: its template and arguments as the zone file gives them
BOOST_FIXTURE_TEST_CASE(gives_a_dilcopy_s_arguments, unit_tests::FixtureBase)
{
    char name[] = "function/aggressive"; // no such template loaded here: the name as stored
    char text[] = "ANY";
    char a[] = "$1n attacks!";
    char b[] = "$1n attacks you!";
    char *messages[] = {a, b, nullptr};
    int levels[] = {2, 7, 9}; // [0] is the count

    auto dilargs = std::make_unique<dilargstype>();
    dilargs->name = name;
    dilargs->no = 4;
    dilargs->dilarg[0].type = DILV_INT;
    dilargs->dilarg[0].data.num = static_cast<ubit32>(-1);
    dilargs->dilarg[1].type = DILV_SP;
    dilargs->dilarg[1].data.string = text;
    dilargs->dilarg[2].type = DILV_SLP;
    dilargs->dilarg[2].data.stringlist = messages;
    dilargs->dilarg[3].type = DILV_ILP;
    dilargs->dilarg[3].data.intlist = levels;

    unit_fptr f;
    f.setFunctionPointerIndex(SFUN_DILCOPY_INTERNAL);
    f.setData(dilargs.get());

    auto doc = to_json(f);
    BOOST_TEST(doc["name"].GetString() == std::string("function/aggressive"));
    const auto &args = doc["args"];
    BOOST_REQUIRE(args.Size() == 4);
    BOOST_TEST(args[0].GetInt() == -1);
    BOOST_TEST(args[1].GetString() == std::string("ANY"));
    BOOST_REQUIRE(args[2].Size() == 2);
    BOOST_TEST(args[2][1].GetString() == std::string("$1n attacks you!"));
    BOOST_REQUIRE(args[3].Size() == 2);
    BOOST_TEST(args[3][0].GetInt() == 7);
    BOOST_TEST(args[3][1].GetInt() == 9);
}

BOOST_AUTO_TEST_SUITE_END()
