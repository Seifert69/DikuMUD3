#include "textutil.h"

#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * Html_Link_Suite
 *
 * getHTMLLinkTarget() gives mplex the web address of an <a ...> tag so telnet
 * users, whose clients can't follow HTML links, see the address itself.
 */
BOOST_AUTO_TEST_SUITE(Html_Link_Suite)

static std::string target(const char *tagText, int size = 256)
{
    char buf[256] = "unchanged";
    getHTMLLinkTarget(tagText, buf, size);
    return buf;
}

// The help pages' wiki link, as helpzon.zon sends it
BOOST_AUTO_TEST_CASE(wiki_link_keeps_its_case)
{
    BOOST_TEST(target("a href='https://wiki.dikumud.com/wiki/Help/Accuse'") == "https://wiki.dikumud.com/wiki/Help/Accuse");
    BOOST_TEST(target("a href='https://wiki.dikumud.com/wiki/Wizhelp/Set'") == "https://wiki.dikumud.com/wiki/Wizhelp/Set");
}

BOOST_AUTO_TEST_CASE(http_and_any_case_scheme)
{
    BOOST_TEST(target("a href='http://example.com/x'") == "http://example.com/x");
    BOOST_TEST(target("a href='HTTPS://Example.com/Y'") == "HTTPS://Example.com/Y");
}

BOOST_AUTO_TEST_CASE(blanks_around_equals)
{
    BOOST_TEST(target("a  href = 'https://example.com/'") == "https://example.com/");
}

// Command links (clickable commands in the web client) and non-web links give nothing
BOOST_AUTO_TEST_CASE(not_a_web_address)
{
    BOOST_TEST(target("a cmd='look'") == "");
    BOOST_TEST(target("a cmd='help #'") == "");
    BOOST_TEST(target("a href='mailto:someone@example.com'") == "");
    BOOST_TEST(target("a href='javascript:alert(1)'") == "");
    BOOST_TEST(target("a href='/wiki/Help/Who'") == "");
}

// The wiki-scraped help files (e.g. trigger.hlp) use double quotes and more attributes
BOOST_AUTO_TEST_CASE(any_quotes_or_none)
{
    BOOST_TEST(target("a rel=\"nofollow\" class=\"external text\" href=\"https://regexr.com/\"") == "https://regexr.com/");
    BOOST_TEST(target("a href=https://example.com/x title='y'") == "https://example.com/x");
}

// Unlike getHTMLValue(), "href" must be a whole attribute name, in any case
BOOST_AUTO_TEST_CASE(href_is_a_whole_attribute)
{
    BOOST_TEST(target("A HREF='https://example.com/Who'") == "https://example.com/Who");
    BOOST_TEST(target("a data-href='https://old.example/' href='https://new.example/'") == "https://new.example/");
    BOOST_TEST(target("a cmd='help href' href='https://example.com/'") == "https://example.com/");
    BOOST_TEST(target("a title='href=https://no.example/'") == "");
}

// tagText is the source text after '<'; the tag ends at the first '>'
BOOST_AUTO_TEST_CASE(stops_at_the_end_of_the_tag)
{
    BOOST_TEST(target("a href='https://example.com/'>text</a>") == "https://example.com/");
    BOOST_TEST(target("a cmd='look'>see <a href='https://example.com/'>") == "");
}

// Browsers ignore blanks around the address
BOOST_AUTO_TEST_CASE(blanks_around_the_address)
{
    BOOST_TEST(target("a href=\" https://regexr.com/ \"") == "https://regexr.com/");
    BOOST_TEST(target("a href='\thttps://x.org/\n'") == "https://x.org/");
    BOOST_TEST(target("a href='   '") == "");
}

// Before: a newline or ESC went to the terminal as it is
BOOST_AUTO_TEST_CASE(control_characters_are_dropped)
{
    BOOST_TEST(target("a href='https://x.org/a\nb'") == "https://x.org/ab");
    BOOST_TEST(target("a href='https://x.org/a\x1b[2Jb'") == "https://x.org/a[2Jb");
    BOOST_TEST(target("a href='https://x.org/a\tb\x7f'") == "https://x.org/ab");
    BOOST_TEST(target("a href='https://x.org/a\xff" "b'") == "https://x.org/ab"); // Telnet's IAC
}

BOOST_AUTO_TEST_CASE(entities_are_decoded)
{
    BOOST_TEST(target("a href='https://x.org/?a=1&amp;b=2&#39;'") == "https://x.org/?a=1&b=2'");
    BOOST_TEST(target("a href='https://x.org/?a=1&b=2'") == "https://x.org/?a=1&b=2");
    BOOST_TEST(target("a href='https://x.org/a&#x2F;b&#47;c'") == "https://x.org/a/b/c");
    BOOST_TEST(target("a href='https://x.org/caf&#233;'") == "https://x.org/caf\xc3\xa9");
}

// Numeric entities can spell the scheme too. Before, the address was
// checked before decoding, so these gave nothing.
BOOST_AUTO_TEST_CASE(entities_in_the_scheme_are_decoded)
{
    BOOST_TEST(target("a href='https&#58;//x.org/'") == "https://x.org/");
    BOOST_TEST(target("a href='&#104;ttps://x.org/'") == "https://x.org/");
    BOOST_TEST(target("a href='&#x6A;avascript:alert(1)'") == "");
}

// A blank from an entity is written as a browser sends it, %20. Before,
// &nbsp; became a blank, which split the printed address in two.
BOOST_AUTO_TEST_CASE(blanks_from_entities_become_percent_20)
{
    BOOST_TEST(target("a href='https://x.org/a&nbsp;b&#160;c&#32;d&#xA0;e'") == "https://x.org/a%20b%20c%20d%20e");
    BOOST_TEST(target("a href='https://x.org/a&#32;'", 19) == "https://x.org/a%20"); // Exactly fits with the \0
    BOOST_TEST(target("a href='https://x.org/a&#32;'", 18) == "");
}

// A blank in the address as sent stays as it is
BOOST_AUTO_TEST_CASE(blank_as_sent_is_kept)
{
    BOOST_TEST(target("a href='https://x.org/a b'") == "https://x.org/a b");
}

// Before: more than 7 digits, leading zeros included, weren't decoded
BOOST_AUTO_TEST_CASE(entities_with_leading_zeros)
{
    BOOST_TEST(target("a href='https://x.org/&#00000039;'") == "https://x.org/'");
}

// Decoding can't sneak in what the address may not hold
BOOST_AUTO_TEST_CASE(entities_for_control_characters_are_not_decoded)
{
    BOOST_TEST(target("a href='https://x.org/a&#10;b&#27;c'") == "https://x.org/a&#10;b&#27;c");
}

BOOST_AUTO_TEST_CASE(malformed_or_too_long)
{
    BOOST_TEST(target("a href='https://example.com/no-closing-quote") == "");
    BOOST_TEST(target("a href='https://example.com/closed-after-the-tag>'") == "");
    BOOST_TEST(target("a href=''") == "");
    BOOST_TEST(target("a href") == "");
    BOOST_TEST(target("a href='https://example.com/a-long-address'", 16) == "");
    BOOST_TEST(target("a href='https://x.org/'", 15) == "https://x.org/"); // Exactly fits with the \0
}

BOOST_AUTO_TEST_SUITE_END()

/**********************************************************************************
 * Html_Entity_Suite
 *
 * decodeHTMLEntity() decodes one entity for clients that show text as it is
 * (StripHTML() in mplex, and getHTMLLinkTarget()).
 */
BOOST_AUTO_TEST_SUITE(Html_Entity_Suite)

// What the entity at the start of text decodes to, then '|' and the text after it
static std::string decode(const char *text)
{
    const char *p = text;
    char out[4];
    int n = decodeHTMLEntity(p, out);
    return std::string(out, n) + "|" + p;
}

BOOST_AUTO_TEST_CASE(named_entities_in_any_case)
{
    BOOST_TEST(decode("&amp;x") == "&|x");
    BOOST_TEST(decode("&LT;") == "<|");
    BOOST_TEST(decode("&gt;&quot;") == ">|&quot;");
    BOOST_TEST(decode("&quot;") == "\"|");
    BOOST_TEST(decode("&Apos;") == "'|");
    BOOST_TEST(decode("&nbsp;") == " |");
}

BOOST_AUTO_TEST_CASE(numeric_entities)
{
    BOOST_TEST(decode("&#39;s") == "'|s");
    BOOST_TEST(decode("&#x27;") == "'|");
    BOOST_TEST(decode("&#X27;") == "'|");
    BOOST_TEST(decode("&#32;") == " |");
    BOOST_TEST(decode("&#160;") == " |"); // No-break space
    BOOST_TEST(decode("&#xa0;") == " |");
}

// UTF-8 at the edges of each length
BOOST_AUTO_TEST_CASE(utf8_lengths)
{
    BOOST_TEST(decode("&#x7E;") == "~|");
    BOOST_TEST(decode("&#xA1;") == "\xc2\xa1" "|");
    BOOST_TEST(decode("&#x7FF;") == "\xdf\xbf" "|");
    BOOST_TEST(decode("&#x800;") == "\xe0\xa0\x80" "|");
    BOOST_TEST(decode("&#xFFFF;") == "\xef\xbf\xbf" "|");
    BOOST_TEST(decode("&#x10000;") == "\xf0\x90\x80\x80" "|");
    BOOST_TEST(decode("&#x10FFFF;") == "\xf4\x8f\xbf\xbf" "|");
}

// Before: more than 7 digits, leading zeros included, weren't decoded
BOOST_AUTO_TEST_CASE(leading_zeros)
{
    BOOST_TEST(decode("&#0000000039;") == "'|");
    BOOST_TEST(decode("&#x00000000041;") == "A|");
}

// Not an entity, or not a character to show: nothing written, p not moved
BOOST_AUTO_TEST_CASE(not_decoded)
{
    for (const char *text : {"x", "&", "&amp", "&ampx;", "&unknown;", "&#;", "&#x;", "&#39", "&#3 9;", "&#x1G;", "&#0;", "&#9;",
                             "&#10;", "&#27;", "&#31;", "&#127;", "&#x80;", "&#159;", "&#xD800;", "&#xDFFF;", "&#x110000;",
                             "&#1114112;", "&#99999999999999999999999999;", "&#xFFFFFFFFFFFFFFFFFFFF;"})
    {
        BOOST_TEST_CONTEXT(text)
        {
            BOOST_TEST(decode(text) == std::string("|") + text);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
