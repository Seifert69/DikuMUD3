#include "mplex/ClientConnector.h"
#include "mplex/mplex.h"
#include "mplex/translate.h"

#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * MPlex_StripHTML_Suite
 *
 * StripHTML() turns the HTML the MUD sends into telnet text with ANSI colours.
 */

// A telnet connection with ANSI colours. bWebSockets stops the constructor
// from trying to accept() a socket.
struct StripHTMLFixture
{
    StripHTMLFixture()
    {
        mplex::g_mplex_arg.bWebSockets = 1;
        mplex::g_mplex_arg.g_bModeANSI = 1;
        mplex::g_mplex_arg.g_bModeRawHTML = 0;
        mplex::translate_init();
        con = new mplex::cConHook();
    }

    ~StripHTMLFixture() { delete con; }

    std::string strip(const char *html)
    {
        char out[4096];
        con->StripHTML(out, sizeof(out), html);
        return out;
    }

    // Counts how often 'needle' appears in 'haystack'
    static int count(const std::string &haystack, const std::string &needle)
    {
        int n = 0;
        for (auto pos = haystack.find(needle); pos != std::string::npos; pos = haystack.find(needle, pos + needle.size()))
        {
            n++;
        }
        return n;
    }

    mplex::cConHook *con;
};

BOOST_FIXTURE_TEST_SUITE(MPlex_StripHTML_Suite, StripHTMLFixture)

/* ----- Entities ----- */

// &apos; is 6 characters. Skipping only 5 left the ';' behind, so an
// engraving of 'DawnAngel' showed as ';DawnAngel'; on telnet.
BOOST_AUTO_TEST_CASE(apos_has_no_trailing_semicolon)
{
    BOOST_TEST(strip("&apos;") == "'");
    BOOST_TEST(strip("(engraved: &apos;DawnAngel&apos;)") == "(engraved: 'DawnAngel')");
    BOOST_TEST(strip("You can&apos;t do that.") == "You can't do that.");
}

BOOST_AUTO_TEST_CASE(other_entities_unchanged)
{
    BOOST_TEST(strip("&lt;&gt;&amp;&quot;&nbsp;") == "<>&\" ");
    BOOST_TEST(strip("&unknown;") == "&unknown;");
    BOOST_TEST(strip("a & b") == "a & b");
}

/* ----- <br/> ----- */

BOOST_AUTO_TEST_CASE(br_becomes_newline)
{
    BOOST_TEST(strip("a<br/>b") == "a\n\rb");
    BOOST_TEST(strip("a<br>b") == "a\n\rb");
}

// <br/> right before a real newline is dropped so lines aren't doubled
BOOST_AUTO_TEST_CASE(br_before_newline_not_doubled)
{
    BOOST_TEST(strip("a<br/>\n\rb") == "a\n\rb");
}

// The check looked at the character after the next one, even when the next
// one was the end of the string. Here the byte past the terminator is '\n',
// so the old code read it and dropped the line break.
BOOST_AUTO_TEST_CASE(br_at_end_does_not_read_past_string)
{
    const char text[] = "a<br/>\0\n";
    BOOST_TEST(strip(text) == "a\n\r");
}

// The same check dropped a <br/> whenever the line after it was one character
// long, e.g. a single "." line between two breaks.
BOOST_AUTO_TEST_CASE(br_before_one_character_line_kept)
{
    BOOST_TEST(strip("a<br/>.\nb") == "a\n\r.\n\rb");
}

/* ----- </div> and colours ----- */

// </div> resets to white on black. It used to send the background three times.
BOOST_AUTO_TEST_CASE(close_div_sends_background_once)
{
    std::string out = strip("</div>");

    BOOST_TEST(out == "\033[0;37m\033[40m");
    BOOST_TEST(count(out, "\033[40m") == 1);
}

// Non-bold colours start with "0;", which clears the background, so it is
// sent again - once.
BOOST_AUTO_TEST_CASE(colour_keeps_background_once)
{
    BOOST_TEST(strip("<div class='bb'>") == "\033[44m");
    BOOST_TEST(strip("<div class='cr'>") == "\033[0;31m\033[44m");
}

// Bold colours don't clear the background, so it isn't sent again
BOOST_AUTO_TEST_CASE(bold_colour_does_not_resend_background)
{
    BOOST_TEST(strip("<div class='cpr'>") == "\033[1;31m");
    BOOST_TEST(strip("<div class='cpr bb'>x") == "\033[1;31m\033[44mx");
}

BOOST_AUTO_TEST_CASE(close_div_after_background_resets_to_black)
{
    BOOST_TEST(strip("<div class='cg bb'>x</div>y") == "\033[0;32m\033[40m\033[44mx\033[0;37m\033[40my");
    BOOST_TEST(con->m_nBgColor == CONTROL_BG_BLACK_CHAR);
}

BOOST_AUTO_TEST_SUITE_END()
