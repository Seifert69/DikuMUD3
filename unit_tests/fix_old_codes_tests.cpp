#include "color.h"
#include "textutil.h"

#include <string>
#include <utility>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * Fix_Old_Codes_Suite - old_codes_to_html(), for units saved before v70
 */
BOOST_AUTO_TEST_SUITE(Fix_Old_Codes_Suite)

static std::string convert(const char *old)
{
    color_type colours; // A few names from etc/color.dat
    colours.create("default:cw bn:say_self:cw bn:time:cw bn");
    return old_codes_to_html(old, &colours);
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

// Without a terminator only the code goes. The old protocol hid the rest
// when showing it, but never deleted it, and the converted text is saved.
BOOST_AUTO_TEST_CASE(unterminated_regions_keep_the_text)
{
    BOOST_TEST(convert("A sword\033I of fire") == "A sword of fire");
    BOOST_TEST(convert("A sword\033O of fire") == "A sword of fire");
}

// ESC O is always followed by a colour name, so a known name there goes even
// without its ESC P. It must be the whole word, and unknown names stay.
BOOST_AUTO_TEST_CASE(known_colour_name_without_terminator_removed)
{
    BOOST_TEST(convert("A sword\033Odefault") == "A sword");
    BOOST_TEST(convert("\033Osay_self said hi") == " said hi");
    BOOST_TEST(convert("\033Otimeless blade") == "timeless blade");
    BOOST_TEST(convert("\033Otim blade") == "tim blade");
    BOOST_TEST(convert("\033Ofoo bar") == "foo bar");
}

// An ESC before something that isn't a letter isn't a code. Only the ESC
// goes, so line breaks and spaces survive.
BOOST_AUTO_TEST_CASE(esc_before_non_letter_keeps_the_character)
{
    BOOST_TEST(convert("line\033\n\rnext") == "line<br/>\n\rnext");
    BOOST_TEST(convert("a\033 b") == "a b");
}

BOOST_AUTO_TEST_CASE(raw_ansi_sequences_removed)
{
    BOOST_TEST(convert("\033[1;31mRed\033[0m") == "Red");
    BOOST_TEST(convert("\033[mplain") == "plain");
}

// Bytes from 0x80 up were taken for control codes and deleted
BOOST_AUTO_TEST_CASE(non_ascii_letters_kept)
{
    BOOST_TEST(convert("Bl\345 kappe") == "Bl\345 kappe");
    BOOST_TEST(convert("\033bCaf\303\251\033h") == "Caf\303\251");
}

BOOST_AUTO_TEST_CASE(text_without_codes_unchanged)
{
    BOOST_TEST(convert("a black belt") == "a black belt");
    BOOST_TEST(convert("a black belt\033") == "a black belt"); // A lone ESC goes
    BOOST_TEST(convert("a\001black\tbelt") == "ablackbelt");      // Other control codes go
    BOOST_TEST(convert("") == "");
}

// The &-style codes and the escaping work as before
BOOST_AUTO_TEST_CASE(amp_codes_and_escaping_unchanged)
{
    BOOST_TEST(convert("&cra red&cw sword") == "a red sword");
    BOOST_TEST(convert("\033bIt's red\033h") == "It&apos;s red");
    BOOST_TEST(convert("fish && chips") == "fish &amp; chips");
}

// What master's fix_old_codes_to_html(), which this replaces, gives for these:
// codes that touch, the order the codes are removed in, && and &l
BOOST_AUTO_TEST_CASE(old_codes_to_html_converts_as_before)
{
    const std::pair<const char *, const char *> cases[] = {
        {"&c&cwwx", "x"},
        {"&&&n", "&amp;&lt;br/&gt;"},
        {"&&cw", "&amp;cw"},
        {"&c&c+ww&n", "&amp;cw&lt;br/&gt;"},
        {"&l&&l", "&lt;pre&gt;&amp;l</pre>"},
        {"a&&&&b", "a&amp;&amp;b"},
        {"&[default]x&s9", "&amp;[default]x&amp;s9"},
        {"\033Odefault\033P&c+&cwnx", "x"},
        {"&x\n\r&n", "\n<br/>\n\r&lt;br/&gt;"},
        {"it's <b>&h&p&f&bb&bn", "it&apos;s &lt;b&gt;"},
        {"&b&bbn", ""},
        {"&&&&&&", "&amp;&amp;&amp;"},
    };
    for (const auto &c : cases)
    {
        BOOST_TEST(convert(c.first) == c.second, c.first);
    }
}

// The conversion used two stack buffers of 2 * MAX_STRING_LENGTH and asserted on
// text of MAX_STRING_LENGTH or more. A long old text, e.g. in a dormant player's
// file read by "when", converts like any other.
BOOST_AUTO_TEST_CASE(old_codes_to_html_converts_text_of_any_length)
{
    std::string text(3 * MAX_STRING_LENGTH, 'a');
    text.replace(0, 9, "\033pX&cr<b>");
    BOOST_TEST(old_codes_to_html(text.c_str()) == "X&lt;b&gt;" + std::string(3 * MAX_STRING_LENGTH - 9, 'a'));
}

// Text that grows past the old buffers as it converts: each ' becomes &apos;, &n
// &lt;br/&gt; and \n\r <br/>\n\r
BOOST_AUTO_TEST_CASE(old_codes_to_html_converts_text_that_grows)
{
    std::string quotes(5000, '\'');
    std::string breaks;
    std::string lines;
    std::string quotes_html;
    std::string breaks_html;
    std::string lines_html;
    for (int i = 0; i < 5000; i++)
    {
        quotes_html += "&apos;";
        breaks += "&n";
        breaks_html += "&lt;br/&gt;";
        lines += "\n\r";
        lines_html += "<br/>\n\r";
    }
    BOOST_TEST(old_codes_to_html(quotes.c_str()) == quotes_html);
    BOOST_TEST(old_codes_to_html(breaks.c_str()) == breaks_html);
    BOOST_TEST(old_codes_to_html(lines.c_str()) == lines_html);
}

// After each replacement the conversion looks again from the start, as
// str_substitute() does: a colour code that a removal joins together goes too
BOOST_AUTO_TEST_CASE(old_codes_to_html_codes_joined_by_a_removal_go)
{
    BOOST_TEST(old_codes_to_html("&c&cwwx") == "x");
    BOOST_TEST(old_codes_to_html("&&&n") == "&amp;&lt;br/&gt;");
}

BOOST_AUTO_TEST_CASE(old_codes_to_html_null_is_empty)
{
    BOOST_TEST(old_codes_to_html(nullptr).empty());
}

BOOST_AUTO_TEST_SUITE_END()
