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
