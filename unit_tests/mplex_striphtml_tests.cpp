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

// Zone text often has a blank or tab after <br/> before the line ends, e.g.
// the newspaper articles in offices.zon. That is still the end of the line.
BOOST_AUTO_TEST_CASE(br_before_trailing_blanks_and_newline_not_doubled)
{
    BOOST_TEST(strip("a<br/> \nb") == "a \n\rb");
    BOOST_TEST(strip("a<br/>\t\nb") == "a\t\n\rb");
    BOOST_TEST(strip("a<br/>   \nb") == "a   \n\rb");
}

// Blanks followed by more text are not a line end, so the break stays
BOOST_AUTO_TEST_CASE(br_before_blanks_and_text_kept)
{
    BOOST_TEST(strip("a<br/> b") == "a\n\r b");
}

/* ----- </div> and colours ----- */

// New characters use the TTY emulation. </div> must still reset the
// background to black there, or text after a coloured block keeps the
// terminal's own background (white on white on a light terminal).
BOOST_AUTO_TEST_CASE(close_div_sends_black_background_in_tty_mode)
{
    con->m_sSetup.emulation = TERM_TTY;
    std::string out = strip("<div class='cg bb'>x</div>y");

    auto close = out.find('x');
    BOOST_TEST(out.find("\033[40m", close) != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
