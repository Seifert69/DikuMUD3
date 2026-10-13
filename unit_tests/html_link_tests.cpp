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
    int n = getHTMLLinkTarget(tagText, buf, size);
    BOOST_TEST(n == static_cast<int>(strlen(buf)));
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

BOOST_AUTO_TEST_CASE(malformed_or_too_long)
{
    BOOST_TEST(target("a href='https://example.com/no-closing-quote") == "");
    BOOST_TEST(target("a href=''") == "");
    BOOST_TEST(target("a href=\"https://example.com/double-quotes\"") == ""); // the game only uses single quotes
    BOOST_TEST(target("a href='https://example.com/a-long-address'", 16) == "");
}

BOOST_AUTO_TEST_SUITE_END()
