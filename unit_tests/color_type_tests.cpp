#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define BOOST_TEST_MODULE "color_type Unit Tests"
#include "color.h"

#include <cstring>
#include <memory>
#include <string>

#include <boost/test/unit_test.hpp>

using cstring_ptr = std::unique_ptr<char, decltype(free) *>;

BOOST_AUTO_TEST_SUITE(color_type_tests)

BOOST_AUTO_TEST_CASE(default_ctor_test)
{
    color_type color;

    auto result = color.key_string();
    BOOST_TEST(result.empty());
}

BOOST_AUTO_TEST_CASE(insert_key_c_test_deprecated)
{
    color_type color;
    char key[] = "Some random key - what is this";
    char c[] = "and maybe this seems like some change value too";

    ////////////////////////// Test Subject //////////////////////////////
    auto result = color.insert(key, c);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(result == std::string(c) + key);
}

BOOST_AUTO_TEST_CASE(insert_key_c_test)
{
    color_type color;
    std::string key{"Some random key - what is this"};
    std::string c{"and maybe this seems like some change value too"};

    ////////////////////////// Test Subject //////////////////////////////
    auto result = color.insert(key, c);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(result == std::string(c) + key);
}

BOOST_AUTO_TEST_CASE(insert_NULL_c_test_deprecated)
{
    color_type color;
    char c[] = "and maybe this seems like some change value too";

    ////////////////////////// Test Subject //////////////////////////////
    auto result = color.insert({}, c);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(result.empty());
}

BOOST_AUTO_TEST_CASE(insert_NULL_c_test)
{
    color_type color;
    std::string c{"and maybe this seems like some change value too"};

    ////////////////////////// Test Subject //////////////////////////////
    auto result = color.insert({}, c);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(result.empty());
}

BOOST_AUTO_TEST_CASE(insert_key_NULL_test_deprecated)
{
    color_type color;
    char key[] = "Some random key - what is this";

    ////////////////////////// Test Subject //////////////////////////////
    std::string result = color.insert(key, {});
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(result.empty());
}

BOOST_AUTO_TEST_CASE(insert_key_NULL_test)
{
    color_type color;
    std::string key{"Some random key - what is this"};

    ////////////////////////// Test Subject //////////////////////////////
    auto result = color.insert(key, {});
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(result.empty());
}

BOOST_AUTO_TEST_CASE(insert_combo_test_deprecated)
{
    color_type color;
    char combo[] = "apples:oranges";

    ////////////////////////// Test Subject //////////////////////////////
    color.insert(combo);
    ////////////////////////// Test Subject //////////////////////////////

    std::string expected{"oranges"};
    auto result = color.get("apples");
    BOOST_TEST(result == expected);
}

BOOST_AUTO_TEST_CASE(insert_combo_test)
{
    color_type color;
    std::string combo{"apples:oranges"};

    ////////////////////////// Test Subject //////////////////////////////
    color.insert(combo);
    ////////////////////////// Test Subject //////////////////////////////

    std::string key{"apples"};
    auto result = color.get(key);
    std::string expected{"oranges"};
    BOOST_TEST(result == expected);
}

BOOST_AUTO_TEST_CASE(insert_duplicate_key_test_deprecated)
{
    color_type color;
    char key[] = "apples";
    char value[] = "oranges";

    {
        auto rc = color.insert(key, value);
        BOOST_TEST(color.get("apples") == std::string(value));
        BOOST_TEST(rc == std::string(value) + key);
    }

    char value2[] = "pears";
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = color.insert(key, value2);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(color.get("apples") == std::string(value2));
    BOOST_TEST(rc == std::string(value2) + key);
}

BOOST_AUTO_TEST_CASE(insert_duplicate_key_test)
{
    color_type color;
    std::string key{"apples"};
    std::string value{"oranges"};

    {
        auto rc = color.insert(key, value);
        BOOST_TEST(color.get("apples") == value);
        BOOST_TEST(rc == value + key);
    }

    std::string value2{"pears"};
    ////////////////////////// Test Subject //////////////////////////////
    auto rc = color.insert(key, value2);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(color.get("apples") == value2);
    BOOST_TEST(rc == value2 + key);
}

BOOST_AUTO_TEST_CASE(keystring_test)
{
    color_type color;
    {
        auto result = color.key_string();
        BOOST_TEST(result.empty());
    }

    {
        std::string combo{"apples:oranges"};
        color.insert(combo);

        ////////////////////////// Test Subject //////////////////////////////
        auto result = color.key_string();
        ////////////////////////// Test Subject //////////////////////////////

        std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
        BOOST_TEST(result == expected);
    }

    {
        std::string combo{"pears:grapes"};
        color.insert(combo);

        ////////////////////////// Test Subject //////////////////////////////
        auto result = color.key_string();
        ////////////////////////// Test Subject //////////////////////////////

        std::string expected = {"<div class='oranges'>apples                   = oranges</div><br/>"
                                "<div class='grapes'>pears                    = grapes</div><br/><br/>"};
        BOOST_TEST(result == expected);
    }
}

BOOST_AUTO_TEST_CASE(remove_tests)
{
    color_type color;
    char combo[] = "apples:oranges";
    char key[] = "apples";
    std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
    color.insert(combo);

    {
        auto ptr = color.key_string();
        BOOST_TEST(ptr == expected);
    }
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(color.remove(key) = 88);
    ////////////////////////// Test Subject //////////////////////////////

    {
        auto ptr = color.key_string();
        BOOST_TEST(ptr.empty());
    }
}

BOOST_AUTO_TEST_CASE(remove_empty_list_tests)
{
    color_type color;
    char key[] = "doesn't matter";
    BOOST_TEST(color.remove(key) == 0);
}

BOOST_AUTO_TEST_CASE(remove_null_key_tests_deprecated)
{
    color_type color;
    char combo[] = "apples:oranges";
    std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
    color.insert(combo);

    {
        auto ptr = color.key_string();
        BOOST_TEST(ptr == expected);
    }
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(color.remove(nullptr) == 0);
    ////////////////////////// Test Subject //////////////////////////////

    {
        auto ptr = color.key_string();
        BOOST_TEST(ptr == expected);
    }
}

BOOST_AUTO_TEST_CASE(remove_null_key_tests)
{
    color_type color;
    std::string combo{"apples:oranges"};
    std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
    color.insert(combo);

    {
        auto ptr = color.key_string();
        BOOST_TEST(ptr == expected);
    }
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(color.remove(std::string{}) == 0);
    ////////////////////////// Test Subject //////////////////////////////

    {
        auto ptr = color.key_string();
        BOOST_TEST(ptr == expected);
    }
}

BOOST_AUTO_TEST_CASE(remove_bad_key_tests)
{
    color_type color;
    char combo[] = "apples:oranges";
    std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
    color.insert(combo);

    {
        auto ptr = color.key_string();
        BOOST_TEST(ptr == expected);
    }
    ////////////////////////// Test Subject //////////////////////////////
    char key[] = "not in there";
    BOOST_TEST(color.remove(key) == 0);
    ////////////////////////// Test Subject //////////////////////////////

    {
        auto ptr = color.key_string();
        BOOST_TEST(ptr == expected);
    }
}

BOOST_AUTO_TEST_CASE(remove_middle_tests)
{
    color_type color;

    auto ptr = color.key_string();
    BOOST_TEST(ptr.empty());

    {
        char combo[] = "apples:oranges";
        color.insert(combo);

        auto ptr = color.key_string();

        std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
        BOOST_TEST(ptr == expected);
    }

    {
        char combo[] = "pears:grapes";
        color.insert(combo);

        auto ptr = color.key_string();

        std::string expected = {"<div class='oranges'>apples                   = oranges</div><br/>"
                                "<div class='grapes'>pears                    = grapes</div><br/><br/>"};
        BOOST_TEST(ptr == expected);
    }

    {
        char combo[] = "lemons:limes";
        color.insert(combo);

        auto ptr = color.key_string();

        std::string expected = {"<div class='oranges'>apples                   = oranges</div><br/>"
                                "<div class='limes'>lemons                   = limes</div><br/>"
                                "<div class='grapes'>pears                    = grapes</div><br/><br/>"};
        BOOST_TEST(ptr == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    char key[] = "lemons";
    BOOST_TEST(color.remove(key) == 1);
    ////////////////////////// Test Subject //////////////////////////////

    {
        auto ptr = color.key_string();
        std::string expected = {"<div class='oranges'>apples                   = oranges</div><br/>"
                                "<div class='grapes'>pears                    = grapes</div><br/><br/>"};
        BOOST_TEST(ptr == expected);
    }
}

BOOST_AUTO_TEST_CASE(insert_combo_null_tests)
{
    color_type color;
    ////////////////////////// Test Subject //////////////////////////////
    color.insert(nullptr);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true);
}

BOOST_AUTO_TEST_CASE(insert_combo_missing_colon_tests)
{
    color_type color;
    char combo[] = "key_no_colon";
    ////////////////////////// Test Subject //////////////////////////////
    color.insert(combo);
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(true);
}

BOOST_AUTO_TEST_CASE(remove_all_tests)
{
    color_type color;
    char combo1[] = "apples:oranges";
    char combo2[] = "pears:grapes";
    char combo3[] = "lemons:limes";

    color.insert(combo1);
    color.insert(combo2);
    color.insert(combo3);

    ////////////////////////// Test Subject //////////////////////////////
    color.remove_all();
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(color.get("apples").empty());
    BOOST_TEST(color.get("pears").empty());
    BOOST_TEST(color.get("lemos").empty());
}

BOOST_AUTO_TEST_CASE(save_string_tests)
{
    color_type color;
    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.save_string();
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
    }
    char combo1[] = "apples:oranges";
    color.insert(combo1);
    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.save_string();
        ////////////////////////// Test Subject //////////////////////////////
        std::string expected{"apples:oranges:"};
        BOOST_TEST(rc == expected);
    }

    char combo2[] = "pears:grapes";
    color.insert(combo2);
    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.save_string();
        ////////////////////////// Test Subject //////////////////////////////
        std::string expected{"apples:oranges:pears:grapes:"};
        BOOST_TEST(rc == expected);
    }

    char combo3[] = "lemons:limes";
    color.insert(combo3);
    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.save_string();
        ////////////////////////// Test Subject //////////////////////////////
        std::string expected{"apples:oranges:lemons:limes:pears:grapes:"};
        BOOST_TEST(rc == expected);
    }
}

BOOST_AUTO_TEST_CASE(keystring_color_type_test)
{
    color_type color;
    std::string combo1{"apples:oranges"};
    color.insert(combo1);

    color_type color2;
    std::string combo2{"pears:grapes"};
    color2.insert(combo2);

    ////////////////////////// Test Subject //////////////////////////////
    auto rc = color.key_string(color2);
    ////////////////////////// Test Subject //////////////////////////////

    std::string expected{"<div class='oranges'>apples                   = oranges</div><br/>"
                         "<div class='grapes'>pears                    = grapes</div><br/><br/>"};
    BOOST_TEST(rc == expected);
}

BOOST_AUTO_TEST_CASE(get_key_fullkey_test_deprecated)
{
    color_type color;
    char combo[] = "apples:oranges";
    color.insert(combo);
    char combo2[] = "pears:grapes";
    color.insert(combo2);

    char full_key[10000]{};

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get(nullptr, full_key);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
    }

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("apples", nullptr);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc == "oranges");
    }

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get(nullptr, nullptr);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
    }

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("goldfish", full_key);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
        BOOST_TEST(std::string(full_key).empty());
    }

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("pears", full_key);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc == "grapes");
        BOOST_TEST(std::string(full_key) == "pears");
    }
}

BOOST_AUTO_TEST_CASE(get_key_fullkey_test)
{
    color_type color;
    std::string combo{"apples:oranges"};
    color.insert(combo);
    std::string combo2{"pears:grapes"};
    color.insert(combo2);

    std::string full_key;

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get({}, full_key);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
    }

    //    {
    //        ////////////////////////// Test Subject //////////////////////////////
    //        auto rc = color.get("apples", nullptr);
    //        ////////////////////////// Test Subject //////////////////////////////
    //        BOOST_REQUIRE_NE(rc, nullptr);
    //        BOOST_TEST(std::string(rc) == "oranges");
    //    }

    //    {
    //        ////////////////////////// Test Subject //////////////////////////////
    //        auto rc = color.get(nullptr, nullptr);
    //        ////////////////////////// Test Subject //////////////////////////////
    //        BOOST_TEST(rc == nullptr);
    //    }

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("goldfish", full_key);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
        BOOST_TEST(std::string(full_key).empty());
    }

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("pears", full_key);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(std::string(rc) == "grapes");
        BOOST_TEST(std::string(full_key) == "pears");
    }
}

BOOST_AUTO_TEST_CASE(change_test)
{
    color_type color;
    char combo[] = "apples:oranges";
    color.insert(combo);
    {
        auto rc = color.get("apples");
        BOOST_TEST(std::string(rc) == "oranges");
    }
    ////////////////////////// Test Subject //////////////////////////////
    char key[] = "apples";
    char new_value[] = "speed_boat";
    auto rc = color.change(key, new_value);
    ////////////////////////// Test Subject //////////////////////////////
    {
        std::string expected{"<div class='speed_boat'>apples = speed_boat</div>"};
        BOOST_TEST(rc == expected);
        auto get_rc = color.get("apples");
        BOOST_TEST(std::string(get_rc) == new_value);
    }
}

BOOST_AUTO_TEST_CASE(change_combo_test)
{
    color_type color;
    char combo[] = "apples:oranges";
    color.insert(combo);

    {
        auto rc = color.get("apples");
        BOOST_TEST(std::string(rc) == "oranges");
    }
    ////////////////////////// Test Subject //////////////////////////////
    char combo2[] = "apples:speed_boat";
    color.change(combo2);
    ////////////////////////// Test Subject //////////////////////////////
    {
        auto rc = color.get("apples");
        BOOST_TEST(std::string(rc) == "speed_boat");
    }

    color.change(nullptr);
    BOOST_TEST(true); // Didn't crash

    {
        char bad_combo[] = "apples:";
        color.change(bad_combo);

        BOOST_TEST(true);
    }
}

BOOST_AUTO_TEST_CASE(color_get_full_partial)
{
    color_type color;
    std::string combo1{"deatheater:blue"};
    std::string combo2{"deathpc:purple"};
    std::string combo3{"deaths:green"};
    std::string combo4{"deathsnpc:white"};

    color.insert(combo1);
    color.insert(combo2);
    color.insert(combo3);
    color.insert(combo4);

    {
        auto ptr = color.key_string();

        std::string expected{"<div class='blue'>deatheater               = blue</div><br/>"
                             "<div class='purple'>deathpc                  = purple</div><br/>"
                             "<div class='green'>deaths                   = green</div><br/>"
                             "<div class='white'>deathsnpc                = white</div><br/><br/>"};
        BOOST_TEST(ptr == expected);
    }

    {
        std::string buffer;
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("death", buffer);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(buffer == "deatheater");
        BOOST_TEST(rc == "blue");
    }

    {
        std::string buffer;
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("deatha", buffer);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
        BOOST_TEST(std::string(buffer).empty());
    }

    {
        std::string buffer;
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("deathz", buffer);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
        BOOST_TEST(std::string(buffer).empty());
    }

    {
        std::string buffer;
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("deaths", buffer);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(buffer == "deaths");
        BOOST_TEST(rc == "green");
    }

    {
        std::string buffer;
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("daaaa", buffer);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
        BOOST_TEST(buffer.empty());
    }

    {
        std::string buffer;
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("dzzzzz", buffer);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.empty());
        BOOST_TEST(buffer.empty());
    }
}

BOOST_AUTO_TEST_CASE(create_test)
{
    std::string color_string{
        "default:cw bn:death:cc bn:exit:cpc bn:hit_me:cpy bn:hit_opponent:cpr bn:hit_other:cpc bn:miss_me:cpb bn:miss_opponent:cpb "
        "bn:miss_other:cpb bn:shield_me:cpw bn:shield_opponent:cpw bn:shield_other:cpw bn:nodam_me:cpg bn:nodam_opponent:cg "
        "bn:nodam_other:cg bn:group:cw bn:immort_descr:cc bn:immort_title:cc bn:log:cw bn:npc_descr:cc bn:npc_title:cc bn:obj_descr:cw "
        "bn:obj_title:cw bn:pc_descr:cc bn:pc_title:cc bn:prompt:cw bn:respond:cpw bn:room_descr:cw bn:room_title:cpb bn:say_self:cw "
        "bn:say_other:cpw bn:shout_self:cw bn:shout_other:cpw bn:social_self:cw bn:social_other:cw bn:spells:cw bn:tell_self:cw "
        "bn:tell_other:cpw bn:time:cw bn:weather:cw bn:whisper:cw bn:wiz:cpw bn:xpgain:cw bn:who:cc bn:who_name:cc bn:who_guild:cpw "
        "bn:who_title:cc bn:who_inv:cpw bn:clan:cc bn:mail:cpr bw:score:cpw bn:cr:cr bn:cpr:cpr bn:br:br:cn:cn bn:cpn:cpn "
        "bn:bn:bn:cg:cg "
        "bn:cpg:cpg bn:bg:bg:cy:cy bn:cpy:cpy bn:by:by:cb:cb bn:cpb:cpb bn:bb:bb:cm:cm bn:cpm:cpm bn:bm:bm:cc:cc bn:cpc:cpc "
        "bn:bc:bc:cw:cw "
        "bn:cpw:cpw bn:bw:bw:ca:ca bn"};
    color_type color;
    color.create(color_string.data());
    BOOST_TEST(std::string(color.get("default")) == "cw bn");
    BOOST_TEST(std::string(color.get("death")) == "cc bn");
    BOOST_TEST(std::string(color.get("exit")) == "cpc bn");
    BOOST_TEST(std::string(color.get("hit_me")) == "cpy bn");
    BOOST_TEST(std::string(color.get("hit_opponent")) == "cpr bn");
    BOOST_TEST(std::string(color.get("hit_other")) == "cpc bn");
    BOOST_TEST(std::string(color.get("miss_me")) == "cpb bn");
    BOOST_TEST(std::string(color.get("miss_opponent")) == "cpb bn");
    BOOST_TEST(std::string(color.get("miss_other")) == "cpb bn");
    BOOST_TEST(std::string(color.get("shield_me")) == "cpw bn");
    BOOST_TEST(std::string(color.get("shield_opponent")) == "cpw bn");
    BOOST_TEST(std::string(color.get("shield_other")) == "cpw bn");
    BOOST_TEST(std::string(color.get("nodam_me")) == "cpg bn");
    BOOST_TEST(std::string(color.get("nodam_opponent")) == "cg bn");
    BOOST_TEST(std::string(color.get("nodam_other")) == "cg bn");
    BOOST_TEST(std::string(color.get("group")) == "cw bn");
    BOOST_TEST(std::string(color.get("immort_descr")) == "cc bn");
    BOOST_TEST(std::string(color.get("immort_title")) == "cc bn");
    BOOST_TEST(std::string(color.get("log")) == "cw bn");
    BOOST_TEST(std::string(color.get("npc_descr")) == "cc bn");
    BOOST_TEST(std::string(color.get("npc_title")) == "cc bn");
    BOOST_TEST(std::string(color.get("obj_descr")) == "cw bn");
    BOOST_TEST(std::string(color.get("obj_title")) == "cw bn");
    BOOST_TEST(std::string(color.get("pc_descr")) == "cc bn");
    BOOST_TEST(std::string(color.get("pc_title")) == "cc bn");
    BOOST_TEST(std::string(color.get("prompt")) == "cw bn");
    BOOST_TEST(std::string(color.get("respond")) == "cpw bn");
    BOOST_TEST(std::string(color.get("room_descr")) == "cw bn");
    BOOST_TEST(std::string(color.get("room_title")) == "cpb bn");
    BOOST_TEST(std::string(color.get("say_self")) == "cw bn");
    BOOST_TEST(std::string(color.get("say_other")) == "cpw bn");
    BOOST_TEST(std::string(color.get("shout_self")) == "cw bn");
    BOOST_TEST(std::string(color.get("shout_other")) == "cpw bn");
    BOOST_TEST(std::string(color.get("social_self")) == "cw bn");
    BOOST_TEST(std::string(color.get("social_other")) == "cw bn");
    BOOST_TEST(std::string(color.get("spells")) == "cw bn");
    BOOST_TEST(std::string(color.get("tell_self")) == "cw bn");
    BOOST_TEST(std::string(color.get("tell_other")) == "cpw bn");
    BOOST_TEST(std::string(color.get("time")) == "cw bn");
    BOOST_TEST(std::string(color.get("weather")) == "cw bn");
    BOOST_TEST(std::string(color.get("whisper")) == "cw bn");
    BOOST_TEST(std::string(color.get("wiz")) == "cpw bn");
    BOOST_TEST(std::string(color.get("xpgain")) == "cw bn");
    BOOST_TEST(std::string(color.get("who")) == "cc bn");
    BOOST_TEST(std::string(color.get("who_name")) == "cc bn");
    BOOST_TEST(std::string(color.get("who_guild")) == "cpw bn");
    BOOST_TEST(std::string(color.get("who_title")) == "cc bn");
    BOOST_TEST(std::string(color.get("who_inv")) == "cpw bn");
    BOOST_TEST(std::string(color.get("clan")) == "cc bn");
    BOOST_TEST(std::string(color.get("mail")) == "cpr bw");
    BOOST_TEST(std::string(color.get("score")) == "cpw bn");
    BOOST_TEST(std::string(color.get("cr")) == "cr bn");
    BOOST_TEST(std::string(color.get("cpr")) == "cpr bn");
    BOOST_TEST(std::string(color.get("br")) == "br");
    BOOST_TEST(std::string(color.get("cn")) == "cn bn");
    BOOST_TEST(std::string(color.get("cpn")) == "cpn bn");
    BOOST_TEST(std::string(color.get("bn")) == "bn");
    BOOST_TEST(std::string(color.get("cg")) == "cg bn");
    BOOST_TEST(std::string(color.get("cpg")) == "cpg bn");
    BOOST_TEST(std::string(color.get("bg")) == "bg");
    BOOST_TEST(std::string(color.get("cy")) == "cy bn");
    BOOST_TEST(std::string(color.get("cpy")) == "cpy bn");
    BOOST_TEST(std::string(color.get("by")) == "by");
    BOOST_TEST(std::string(color.get("cb")) == "cb bn");
    BOOST_TEST(std::string(color.get("cpb")) == "cpb bn");
    BOOST_TEST(std::string(color.get("bb")) == "bb");
    BOOST_TEST(std::string(color.get("cm")) == "cm bn");
    BOOST_TEST(std::string(color.get("cpm")) == "cpm bn");
    BOOST_TEST(std::string(color.get("bm")) == "bm");
    BOOST_TEST(std::string(color.get("cc")) == "cc bn");
    BOOST_TEST(std::string(color.get("cpc")) == "cpc bn");
    BOOST_TEST(std::string(color.get("bc")) == "bc");
    BOOST_TEST(std::string(color.get("cw")) == "cw bn");
    BOOST_TEST(std::string(color.get("cpw")) == "cpw bn");
    BOOST_TEST(std::string(color.get("bw")) == "bw");
    BOOST_TEST(std::string(color.get("ca")) == "ca bn");
}

BOOST_AUTO_TEST_SUITE_END()
#pragma GCC diagnostic pop
