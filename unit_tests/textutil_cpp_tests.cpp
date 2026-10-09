#include "essential.h"
#include "textutil.h"

#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * Textutil_CPP_Suite
 */
BOOST_AUTO_TEST_SUITE(Textutil_CPP_Suite)

// fix_old_codes_to_html() returns a str_dup'ed string
static std::string convert(const char *old)
{
    char *p = fix_old_codes_to_html(old);
    std::string result(p);
    FREE(p);
    return result;
}

/**********************************************************************************
 * fix_old_codes_to_html_test - pre v70 ESC + letter colour codes (issue #433)
 */
BOOST_AUTO_TEST_CASE(fix_old_codes_to_html_plain_test)
{
    BOOST_TEST(convert("a black belt") == "a black belt");
}

BOOST_AUTO_TEST_CASE(fix_old_codes_to_html_colour_test)
{
    // Item titles from a real player whose items were converted without this fix
    BOOST_TEST(convert("\x1B" "dan element breast plate\x1B" "h") == "<div class='cy'>an element breast plate</div>");
    BOOST_TEST(convert("\x1B" "pHoly Symbol\x1B" "h") == "<div class='cpw'>Holy Symbol</div>");
    BOOST_TEST(convert("\x1B" "cDragon Sleeves\x1B" "h") == "<div class='cg'>Dragon Sleeves</div>");
    BOOST_TEST(convert("a \x1B" "mpair of blue boots\x1B" "h") == "a <div class='cpb'>pair of blue boots</div>");
    BOOST_TEST(convert("a \x1B" "pmithril\x1B" "Odefault\x1B" "P back plate") == "a <div class='cpw'>mithril</div> back plate");
}

BOOST_AUTO_TEST_CASE(fix_old_codes_to_html_unbalanced_test)
{
    // No reset at the end - the tag is closed anyway
    BOOST_TEST(convert("\x1B" "bRed") == "<div class='cr'>Red</div>");

    // Colour after colour - one tag closes before the next opens
    BOOST_TEST(convert("\x1B" "bRed\x1B" "cGreen\x1B" "h") == "<div class='cr'>Red</div><div class='cg'>Green</div>");

    // Back to back codes don't leave empty tags; fg and bg combine
    // (ESC u is a blue background, class 'bb')
    BOOST_TEST(convert("\x1B" "p\x1B" "uHi\x1B" "E there") == "<div class='cpw bb'>Hi</div> there");

    // A reset with nothing open does nothing
    BOOST_TEST(convert("\x1B" "hplain\x1B" "E") == "plain");
}

BOOST_AUTO_TEST_CASE(fix_old_codes_to_html_other_codes_test)
{
    // Codes with no HTML version are removed, letter and all
    BOOST_TEST(convert("\x1B" "Cbold\x1B" "B") == "bold");

    // Gobbled text was never shown
    BOOST_TEST(convert("a\x1B" "Ihidden\x1B" "Jb") == "ab");

    // A colour name other than default is dropped with its codes
    BOOST_TEST(convert("\x1B" "Ofancy\x1B" "Ptext") == "text");

    // Real ANSI sequences and a lone trailing ESC
    BOOST_TEST(convert("\x1B[1;33mHi") == "Hi");
    BOOST_TEST(convert("abc\x1B") == "abc");
}

BOOST_AUTO_TEST_CASE(fix_old_codes_to_html_escaping_test)
{
    // Text is still escaped; only the generated tags are real HTML
    BOOST_TEST(convert("\x1B" "d<b>&&'\x1B" "h") == "<div class='cy'>&lt;b&gt;&amp;&apos;</div>");
}

BOOST_AUTO_TEST_CASE(fix_old_codes_to_html_too_long_test)
{
    // When the tags would not fit, the colour goes but the text stays
    std::string old;
    std::string expected;

    for (int i = 0; i < 2000; i++)
    {
        old += "\x1B" "bx\x1B" "cy";
        expected += "xy";
    }

    BOOST_TEST(convert(old.c_str()) == expected);
}

BOOST_AUTO_TEST_SUITE_END()
