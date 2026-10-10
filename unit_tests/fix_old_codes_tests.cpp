#include "textutil.h"

#include <cstdlib>
#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * Fix_Old_Codes_Suite
 *
 * fix_old_codes_to_html() converts the titles and descriptions of units saved
 * before version 70. They can hold the old two byte codes, ESC + letter (see
 * mplex/translate.h).
 */
BOOST_AUTO_TEST_SUITE(Fix_Old_Codes_Suite)

static std::string convert(const char *old)
{
    char *html = fix_old_codes_to_html(old);
    std::string result(html);
    free(html);
    return result;
}

// The item names from issue #433. Only the ESC used to be removed, so the
// letter after it showed up in the name, e.g. "pHoly Symbolh".
BOOST_AUTO_TEST_CASE(colour_codes_removed_with_their_letter)
{
    BOOST_TEST(convert("\033pHoly Symbol\033h") == "Holy Symbol");
    BOOST_TEST(convert("\033dan element breast plate\033h") == "an element breast plate");
    BOOST_TEST(convert("\033cDragon Sleeves\033h") == "Dragon Sleeves");
    BOOST_TEST(convert("a \033mpair of blue boots\033h") == "a pair of blue boots");
}

BOOST_AUTO_TEST_CASE(background_and_attribute_codes_removed)
{
    BOOST_TEST(convert("\033u\033CBold on blue\033E") == "Bold on blue");
    BOOST_TEST(convert("x\033By\033Az") == "xyz");
}

// ESC O <name> ESC P picks a named colour. The name was never shown.
BOOST_AUTO_TEST_CASE(colour_name_removed)
{
    BOOST_TEST(convert("a \033pmithril\033Odefault\033P back plate") == "a mithril back plate");
    BOOST_TEST(convert("\033Ksay_self\033Psaid") == "said");
}

// The old protocol hid text between ESC I and ESC J
BOOST_AUTO_TEST_CASE(gobbled_text_removed)
{
    BOOST_TEST(convert("Visible\033Ihidden\033J again") == "Visible again");
    BOOST_TEST(convert("x\033Ihidden \033bred\033Jy") == "xy");
}

// Without a terminator the old protocol hid everything after the code
BOOST_AUTO_TEST_CASE(unterminated_regions_removed_to_the_end)
{
    BOOST_TEST(convert("A sword\033Ihidden") == "A sword");
    BOOST_TEST(convert("A sword\033Odefault") == "A sword");
}

BOOST_AUTO_TEST_CASE(lone_esc_at_end_removed)
{
    BOOST_TEST(convert("a black belt\033") == "a black belt");
}

BOOST_AUTO_TEST_CASE(text_without_codes_unchanged)
{
    BOOST_TEST(convert("a black belt") == "a black belt");
    BOOST_TEST(convert("") == "");
}

// The &-style codes and the escaping work as before
BOOST_AUTO_TEST_CASE(amp_codes_and_escaping_unchanged)
{
    BOOST_TEST(convert("&cra red&cw sword") == "a red sword");
    BOOST_TEST(convert("\033bIt's red\033h") == "It&apos;s red");
    BOOST_TEST(convert("fish && chips") == "fish &amp; chips");
}

BOOST_AUTO_TEST_SUITE_END()
