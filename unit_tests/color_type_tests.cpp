#define BOOST_TEST_MODULE "color_type Unit Tests"
#include <boost/test/unit_test.hpp>
#include <memory>
#include <string>
#include <cstring>

#include "color.h"

using cstring_ptr = std::unique_ptr<char, decltype(free) *>;

BOOST_AUTO_TEST_SUITE(color_type_tests)

BOOST_AUTO_TEST_CASE(default_ctor_test)
{
    color_type color;

    cstring_ptr result{color.key_string(), free};
    BOOST_TEST(result.get() == nullptr);
}

BOOST_AUTO_TEST_CASE(insert_key_c_test)
{
    color_type color;
    char key[] = "Some random key - what is this";
    char c[] = "and maybe this seems like some change value too";

    ////////////////////////// Test Subject //////////////////////////////
    cstring_ptr some_result{color.insert(key, c), free};
    ////////////////////////// Test Subject //////////////////////////////

    // This is a terrible test because Im not sure of the actual inputs yet
    BOOST_REQUIRE_NE(some_result.get(), nullptr);
    BOOST_TEST(std::string(some_result.get()) == std::string(c) + key);
}

BOOST_AUTO_TEST_CASE(insert_NULL_c_test)
{
    color_type color;
    char c[] = "and maybe this seems like some change value too";

    ////////////////////////// Test Subject //////////////////////////////
    cstring_ptr some_result{color.insert(nullptr, c), free};
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(some_result.get() == nullptr);
}

BOOST_AUTO_TEST_CASE(insert_key_NULL_test)
{
    color_type color;
    char key[] = "Some random key - what is this";

    ////////////////////////// Test Subject //////////////////////////////
    cstring_ptr some_result{color.insert(key, nullptr), free};
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(some_result.get() == nullptr);
}

BOOST_AUTO_TEST_CASE(insert_combo_test)
{
    color_type color;
    char combo[] = "apples:oranges";

    ////////////////////////// Test Subject //////////////////////////////
    color.insert(combo);
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_TEST(color.get("apples") == std::string("oranges"));
}

BOOST_AUTO_TEST_CASE(insert_duplicate_key_test)
{
    color_type color;
    char key[] = "apples";
    char value[] = "oranges";

    {
        cstring_ptr rc{color.insert(key, value), free};
        BOOST_TEST(color.get("apples") == std::string(value));
        BOOST_REQUIRE_NE(rc.get(), nullptr);
        BOOST_TEST(std::string(rc.get()) == std::string(value) + key);
    }

    char value2[] = "pears";
    ////////////////////////// Test Subject //////////////////////////////
    cstring_ptr rc{color.insert(key, value2), free};
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(color.get("apples") == std::string(value2));
    BOOST_REQUIRE_NE(rc.get(), nullptr);
    BOOST_TEST(std::string(rc.get()) == std::string(value2) + key);
}

BOOST_AUTO_TEST_CASE(keystring_test)
{
    color_type color;
    cstring_ptr ptr{color.key_string(), free};
    BOOST_TEST(ptr.get() == nullptr);

    {
        char combo[] = "apples:oranges";
        color.insert(combo);

        ////////////////////////// Test Subject //////////////////////////////
        std::unique_ptr<char, decltype(free) *> ptr{color.key_string(), free};
        ////////////////////////// Test Subject //////////////////////////////

        std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
        BOOST_TEST(std::string(ptr.get()) == expected);
    }

    {
        char combo[] = "pears:grapes";
        color.insert(combo);

        ////////////////////////// Test Subject //////////////////////////////
        cstring_ptr ptr{color.key_string(), free};
        ////////////////////////// Test Subject //////////////////////////////

        std::string expected = {"<div class='oranges'>apples                   = oranges</div><br/>"
                                "<div class='grapes'>pears                    = grapes</div><br/><br/>"};
        BOOST_TEST(std::string(ptr.get()) == expected);
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
        cstring_ptr ptr{color.key_string(), free};
        BOOST_REQUIRE_NE(ptr.get(), nullptr);
        BOOST_TEST(std::string(ptr.get()) == expected);
    }
    ////////////////////////// Test Subject //////////////////////////////
    BOOST_TEST(color.remove(key) = 88);
    ////////////////////////// Test Subject //////////////////////////////

    {
        cstring_ptr ptr{color.key_string(), free};
        BOOST_TEST(ptr.get() == nullptr);
    }
}

BOOST_AUTO_TEST_CASE(remove_empty_list_tests)
{
    color_type color;
    char key[] = "doesn't matter";
    BOOST_TEST(color.remove(key) == 0);
}

// TODO This test causes segfault at the strcmp LOC 251
// BOOST_AUTO_TEST_CASE(remove_null_key_tests)
//{
//    color_type color;
//    char combo[] = "apples:oranges";
//    std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
//    color.insert(combo);
//
//    {
//        std::unique_ptr<char, decltype(free) *> ptr{color.key_string(), free};
//        BOOST_REQUIRE_NE(ptr.get(), nullptr);
//        BOOST_TEST(std::string(ptr.get()) == expected);
//    }
//    ////////////////////////// Test Subject //////////////////////////////
//    BOOST_TEST(color.remove(nullptr)==99);
//    ////////////////////////// Test Subject //////////////////////////////
//
//    {
//        std::unique_ptr<char, decltype(free) *> ptr{color.key_string(), free};
//        BOOST_REQUIRE_NE(ptr.get(), nullptr);
//        BOOST_TEST(std::string(ptr.get()) == expected);
//    }
//}

BOOST_AUTO_TEST_CASE(remove_bad_key_tests)
{
    color_type color;
    char combo[] = "apples:oranges";
    std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
    color.insert(combo);

    {
        cstring_ptr ptr{color.key_string(), free};
        BOOST_REQUIRE_NE(ptr.get(), nullptr);
        BOOST_TEST(std::string(ptr.get()) == expected);
    }
    ////////////////////////// Test Subject //////////////////////////////
    char key[] = "not in there";
    BOOST_TEST(color.remove(key) == 0);
    ////////////////////////// Test Subject //////////////////////////////

    {
        cstring_ptr ptr{color.key_string(), free};
        BOOST_REQUIRE_NE(ptr.get(), nullptr);
        BOOST_TEST(std::string(ptr.get()) == expected);
    }
}

BOOST_AUTO_TEST_CASE(remove_middle_tests)
{
    color_type color;

    cstring_ptr ptr{color.key_string(), free};
    BOOST_TEST(ptr.get() == nullptr);

    {
        char combo[] = "apples:oranges";
        color.insert(combo);

        cstring_ptr ptr{color.key_string(), free};

        std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><br/>"};
        BOOST_TEST(std::string(ptr.get()) == expected);
    }

    {
        char combo[] = "pears:grapes";
        color.insert(combo);

        cstring_ptr ptr{color.key_string(), free};

        std::string expected = {"<div class='oranges'>apples                   = oranges</div><br/>"
                                "<div class='grapes'>pears                    = grapes</div><br/><br/>"};
        BOOST_TEST(std::string(ptr.get()) == expected);
    }

    {
        char combo[] = "lemons:limes";
        color.insert(combo);

        cstring_ptr ptr{color.key_string(), free};

        std::string expected = {"<div class='oranges'>apples                   = oranges</div><br/>"
                                "<div class='limes'>lemons                   = limes</div><br/>"
                                "<div class='grapes'>pears                    = grapes</div><br/><br/>"};
        BOOST_TEST(std::string(ptr.get()) == expected);
    }

    ////////////////////////// Test Subject //////////////////////////////
    char key[] = "lemons";
    BOOST_TEST(color.remove(key) == 1);
    ////////////////////////// Test Subject //////////////////////////////

    {
        cstring_ptr ptr{color.key_string(), free};
        std::string expected = {"<div class='oranges'>apples                   = oranges</div><br/>"
                                "<div class='grapes'>pears                    = grapes</div><br/><br/>"};
        BOOST_TEST(std::string(ptr.get()) == expected);
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
    BOOST_TEST(color.get("apples") == nullptr);
    BOOST_TEST(color.get("pears") == nullptr);
    BOOST_TEST(color.get("lemos") == nullptr);
}

BOOST_AUTO_TEST_CASE(save_string_tests)
{
    color_type color;
    {
        ////////////////////////// Test Subject //////////////////////////////
        cstring_ptr rc{color.save_string(), free};
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc.get() == nullptr);
    }
    char combo1[] = "apples:oranges";
    color.insert(combo1);
    {
        ////////////////////////// Test Subject //////////////////////////////
        cstring_ptr rc{color.save_string(), free};
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_REQUIRE_NE(rc.get(), nullptr);
        std::string expected{"apples:oranges:"};
        BOOST_TEST(std::string(rc.get()) == expected);
    }

    char combo2[] = "pears:grapes";
    color.insert(combo2);
    {
        ////////////////////////// Test Subject //////////////////////////////
        cstring_ptr rc{color.save_string(), free};
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_REQUIRE_NE(rc.get(), nullptr);
        std::string expected{"apples:oranges:pears:grapes:"};
        BOOST_TEST(std::string(rc.get()) == expected);
    }

    char combo3[] = "lemons:limes";
    color.insert(combo3);
    {
        ////////////////////////// Test Subject //////////////////////////////
        cstring_ptr rc{color.save_string(), free};
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_REQUIRE_NE(rc.get(), nullptr);
        std::string expected{"apples:oranges:lemons:limes:pears:grapes:"};
        BOOST_TEST(std::string(rc.get()) == expected);
    }
}

BOOST_AUTO_TEST_CASE(keystring_color_type_test)
{
    color_type color;
    char combo1[] = "apples:oranges";
    color.insert(combo1);

    color_type color2;
    char combo2[] = "pears:grapes";
    color2.insert(combo2);

    ////////////////////////// Test Subject //////////////////////////////
    cstring_ptr rc{color.key_string(color2), free};
    ////////////////////////// Test Subject //////////////////////////////

    BOOST_REQUIRE_NE(rc.get(), nullptr);
    std::string expected{"<div class='oranges'>apples                   = oranges</div><br/><div class='grapes'>pears                    = "
                         "grapes</div><br/><br/>"};
    BOOST_TEST(std::string(rc.get()) == expected);
}

BOOST_AUTO_TEST_CASE(get_key_fullkey_test)
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
        BOOST_TEST(rc == nullptr);
    }

    {
        // TODO - full_key isn't checked for null so code needs fixing before this test can run
        //        ////////////////////////// Test Subject //////////////////////////////
        //        auto rc = color.get("apples", nullptr);
        //        ////////////////////////// Test Subject //////////////////////////////
        //        BOOST_REQUIRE_NE(rc, nullptr);
        //        BOOST_TEST(std::string(rc) == "oranges");
        //        BOOST_TEST(std::string(full_key)=="nope");
    }

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get(nullptr, nullptr);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc == nullptr);
    }

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("goldfish", full_key);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_TEST(rc == nullptr);
        BOOST_TEST(std::string(full_key).empty());
    }

    {
        ////////////////////////// Test Subject //////////////////////////////
        auto rc = color.get("pears", full_key);
        ////////////////////////// Test Subject //////////////////////////////
        BOOST_REQUIRE_NE(rc, nullptr);
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
        BOOST_REQUIRE_NE(rc, nullptr);
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
        BOOST_REQUIRE_NE(get_rc, nullptr);
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
        BOOST_REQUIRE_NE(rc, nullptr);
        BOOST_TEST(std::string(rc) == "oranges");
    }
    ////////////////////////// Test Subject //////////////////////////////
    char combo2[] = "apples:speed_boat";
    color.change(combo2);
    ////////////////////////// Test Subject //////////////////////////////
    {
        auto rc = color.get("apples");
        BOOST_REQUIRE_NE(rc, nullptr);
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
BOOST_AUTO_TEST_SUITE_END()
