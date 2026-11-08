#include "ClientConnector.h"
#include "MUDConnector.h"
#include "hook.h"
#include "mplex.h"
#include "textutil.h"
#include "translate.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * ClientConnector_Suite
 *
 * cConHook::ParseOutput() turns the server's HTML into telnet text:
 * StripHTML() removes the tags and IndentText() wraps the lines.
 *
 * StripHTML_Suite tests StripHTML() on its own: entities, <br/>, <data>
 * and colours.
 *
 * ClientConnector_TLS_Suite (at the end) drives the telnets transport over a
 * real loopback socket.
 */
namespace
{
// Puts back the globals a hook changes, once the hook is gone: in websocket
// mode cConHook() doesn't take a connection, but ~cConHook() gives one back
struct SavedGlobals
{
    // websockets mode (no listening socket needed) and telnet negotiation, as the test wants them
    explicit SavedGlobals(int webSocketsMode = TRUE, int telnetMode = -1)
        : webSockets(std::exchange(mplex::g_mplex_arg.bWebSockets, webSocketsMode)),
          modeTelnet(telnetMode == -1 ? mplex::g_mplex_arg.g_bModeTelnet : std::exchange(mplex::g_mplex_arg.g_bModeTelnet, telnetMode)),
          connectionsLeft(mplex::g_nConnectionsLeft)
    {
    }

    ~SavedGlobals()
    {
        mplex::g_mplex_arg.bWebSockets = webSockets;
        mplex::g_mplex_arg.g_bModeTelnet = modeTelnet;
        mplex::g_nConnectionsLeft = connectionsLeft;
    }

    int webSockets;
    int modeTelnet;
    int connectionsLeft;
};

struct TelnetHook
{
    TelnetHook()
    {
        mplex::translate_init(); // The colour codes, as mplex's main() sets them up
        hook.m_sSetup.emulation = TERM_ANSI;
    }

    std::string Parse(const char *text) { return hook.ParseOutput(text); }

    std::string Strip(const char *html)
    {
        char out[4096];
        hook.StripHTML(out, sizeof(out), html);
        return out;
    }

    // In websocket mode cConHook() needs no listening socket. The hook has no
    // websocket sender, so ParseOutput() takes the telnet path (width 80).
    SavedGlobals saved; // Declared before hook, so restored after it is destroyed
    mplex::cConHook hook;
};

// The text a player sees: without the colour codes (ESC ... letter)
std::string Visible(const std::string &out)
{
    std::string text;

    for (size_t i = 0; i < out.size(); i++)
    {
        if (out[i] == CONTROL_CHAR)
        {
            while (++i < out.size() && !isalpha(static_cast<unsigned char>(out[i])))
            {
                ;
            }
            continue;
        }
        text += out[i];
    }
    return text;
}
} // namespace

BOOST_FIXTURE_TEST_SUITE(ClientConnector_Suite, TelnetHook)

// The help pages' wiki link (helpzon.zon)
BOOST_AUTO_TEST_CASE(web_link_shows_its_address)
{
    BOOST_TEST(Parse("<a href='https://wiki.dikumud.com/wiki/Help/Accuse'>View or Contribute on Wiki</a>") ==
               "View or Contribute on Wiki: https://wiki.dikumud.com/wiki/Help/Accuse");
}

BOOST_AUTO_TEST_CASE(command_link_is_unchanged)
{
    BOOST_TEST(Parse("You see a <a cmd='look statue'>statue</a> here.") == "You see a statue here.");
}

BOOST_AUTO_TEST_CASE(double_quotes_and_upper_case)
{
    BOOST_TEST(Parse("<a rel=\"nofollow\" HREF=\"https://regexr.com/\">RegEx Reference</a>") == "RegEx Reference: https://regexr.com/");
}

// Before: "...wiki/Help/Raiseskeletalcham" and "ion" on the next line, the 'p' lost
BOOST_AUTO_TEST_CASE(long_address_moves_to_the_next_line_whole)
{
    BOOST_TEST(Parse("<a href='https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion'>View or Contribute on Wiki</a>") ==
               "View or Contribute on Wiki:\n\rhttps://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion");
}

BOOST_AUTO_TEST_CASE(address_longer_than_the_line_is_not_cut)
{
    hook.m_sSetup.width = 40;
    BOOST_TEST(Parse("<a href='https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion'>Wiki</a> and more") ==
               "Wiki:\n\rhttps://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion\n\rand more");
}

// Before: a word too long to wrap at a space was cut, and lost the character
// at the cut; words more than 30 characters past the last space were cut too
BOOST_AUTO_TEST_CASE(word_is_never_cut)
{
    std::string word(100, 'x');
    BOOST_TEST(Parse(word.c_str()) == word);
    std::string text = std::string(50, 'a') + " " + std::string(40, 'b');
    BOOST_TEST(Parse(text.c_str()) == std::string(50, 'a') + "\n\r" + std::string(40, 'b'));
}

BOOST_AUTO_TEST_CASE(entities_in_the_address_are_decoded)
{
    BOOST_TEST(Parse("<a href='https://x.org/?a=1&amp;b=2'>Search</a>") == "Search: https://x.org/?a=1&b=2");
    BOOST_TEST(Parse("<a href='https://x.org/?a=1&amp;b=2'>https://x.org/?a=1&amp;b=2</a>") == "https://x.org/?a=1&b=2");
}

BOOST_AUTO_TEST_CASE(text_already_the_address_even_in_colour)
{
    std::string out = Parse("<a href='https://github.com/Seifert69/DikuMUD3'><div class='cpw'>https://github.com/Seifert69/DikuMUD3</div></a>");
    BOOST_TEST(out.find("https://github.com/Seifert69/DikuMUD3") != std::string::npos);
    BOOST_TEST(out.find(": ") == std::string::npos);
    BOOST_TEST(out.find("DikuMUD3", out.find("DikuMUD3") + 1) == std::string::npos);
}

BOOST_AUTO_TEST_CASE(no_dangling_separator)
{
    BOOST_TEST(Parse("<a href='https://x.org/'></a>") == "https://x.org/");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki </a>") == "Wiki https://x.org/");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki<br/></a>") == "Wiki\n\rhttps://x.org/");
}

// A link without text (an image) puts its address after a blank
BOOST_AUTO_TEST_CASE(link_without_text_is_not_glued_on)
{
    BOOST_TEST(Parse("Logo<a href='https://x.org/'><img src='l.png'/></a>") == "Logo https://x.org/");
    BOOST_TEST(Visible(Parse("See<a href='https://x.org/'><div class='cpw'></div></a>")) == "See https://x.org/");
    BOOST_TEST(Parse("See <a href='https://x.org/'></a>") == "See https://x.org/");
}

// Before: a colour code in front of an address hid it, so it was cut
BOOST_AUTO_TEST_CASE(coloured_address_is_not_cut)
{
    hook.m_sSetup.width = 40;
    BOOST_TEST(Visible(Parse("See <div class='cpw'>https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion</div> now")) ==
               "See\n\rhttps://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion\n\rnow");
}

// Before: the line broke at the indent, leaving a line of blanks
BOOST_AUTO_TEST_CASE(indented_address_stays_on_its_line)
{
    hook.m_sSetup.width = 40;
    BOOST_TEST(Parse("   https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion") ==
               "   https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion");
}

BOOST_AUTO_TEST_CASE(link_tags_with_blanks)
{
    BOOST_TEST(Parse("<a href='https://a.example/'>A</a >") == "A: https://a.example/");
    BOOST_TEST(Parse("<a\thref='https://b.example/'>B</a>") == "B: https://b.example/");
    BOOST_TEST(Parse("<abbr title='x'>C</abbr>") == "C");
}

// protocol_send_text() splits text over 32 KB; a link split there loses its
// address, and nothing of it is left for later outputs
BOOST_AUTO_TEST_CASE(link_split_over_two_outputs_loses_only_the_address)
{
    BOOST_TEST(Parse("See the <a href='https://x.org/'>") == "See the ");
    BOOST_TEST(Parse("Wiki</a>") == "Wiki");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki") == "Wiki");
    BOOST_TEST(Parse("Hello there") == "Hello there");
    BOOST_TEST(Parse("Something</a> else") == "Something else");
}

// Before: (https://... or a tab before the address hid it, so it was cut
BOOST_AUTO_TEST_CASE(address_in_brackets_or_after_a_tab_is_not_cut)
{
    hook.m_sSetup.width = 40;
    BOOST_TEST(Parse("See (https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion) more") ==
               "See\n\r(https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion)\n\rmore");
    BOOST_TEST(Parse("Wiki:\thttps://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion") ==
               "Wiki:\thttps://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion");
}

// Before: an address after a line-wide run of blanks counted as nothing on
// the line, so the next address didn't move to a new line
BOOST_AUTO_TEST_CASE(second_address_after_a_long_one_moves_on)
{
    hook.m_sSetup.width = 40;
    std::string blanks(40, ' ');
    BOOST_TEST(Parse((blanks + "https://a.example/aaaaaaaa https://b.example/bbbbbbb").c_str()) ==
               blanks + "https://a.example/aaaaaaaa\n\rhttps://b.example/bbbbbbb");
}

// Before: the colour code's final 'm' counted as the text before the link
BOOST_AUTO_TEST_CASE(colour_before_a_link_without_text)
{
    BOOST_TEST(Visible(Parse("See <div class='cpw'><a href='https://x.org/'></a></div>")) == "See https://x.org/");
}

// A new <a> closes the open link, as in a browser; before, its address was lost
BOOST_AUTO_TEST_CASE(nested_link_keeps_both_addresses)
{
    BOOST_TEST(Parse("<a href='https://one.org/'>One <a href='https://two.org/'>Two</a></a>") ==
               "One https://one.org/ Two: https://two.org/");
    BOOST_TEST(Parse("<a href='https://x.org/'>Go <a cmd='look'>look</a> now</a>") == "Go https://x.org/ look now");
}

// Before: these printed the address a second time
BOOST_AUTO_TEST_CASE(text_showing_the_address_loosely)
{
    BOOST_TEST(Parse("<a href='https://regexr.com/'>https://regexr.com</a>") == "https://regexr.com");
    BOOST_TEST(Parse("<a href='https://x.org/'>https://x.org/ </a>") == "https://x.org/ ");
    BOOST_TEST(Parse("<a href='https://x.org/'>HTTPS://x.org/</a>") == "HTTPS://x.org/");
}

// Before: "One: https://one.org/Two: ..." - the address glued to the next link
BOOST_AUTO_TEST_CASE(back_to_back_links)
{
    BOOST_TEST(Parse("<a href='https://one.org/'>One</a><a href='https://two.org/'>Two</a>") ==
               "One: https://one.org/ Two: https://two.org/");
    BOOST_TEST(Parse("<a href='https://one.org/'>One</a><a cmd='look'>look</a>") == "One: https://one.org/ look");
    BOOST_TEST(Parse("<a href='https://one.org/'>One</a> and <a cmd='look'>look</a>") == "One: https://one.org/ and look");
}

// Before: text differing in the case of the path hid the real address
BOOST_AUTO_TEST_CASE(case_of_the_path_counts)
{
    BOOST_TEST(Parse("<a href='https://wiki.dikumud.net/wiki/Help/Accuse'>https://wiki.dikumud.net/wiki/help/accuse</a>") ==
               "https://wiki.dikumud.net/wiki/help/accuse:\n\rhttps://wiki.dikumud.net/wiki/Help/Accuse");
    BOOST_TEST(Parse("<a href='https://x.org/A'>HTTPS://X.ORG/A</a>") == "HTTPS://X.ORG/A");
}

// Before: these printed the address a second time, or "::"
BOOST_AUTO_TEST_CASE(text_containing_the_address_or_ending_in_a_colon)
{
    BOOST_TEST(Parse("<a href='https://x.org/'>Go to https://x.org/</a>") == "Go to https://x.org/");
    BOOST_TEST(Parse("<a href='https://x.org/'>see https://x.org/.</a>") == "see https://x.org/.");
    BOOST_TEST(Parse("<a href='https://x.org/'>not https://x.org/other</a>") == "not https://x.org/other: https://x.org/");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki:</a>") == "Wiki: https://x.org/");
}

// Before: the address was cut, or moved down leaving a line of blanks
BOOST_AUTO_TEST_CASE(address_after_punctuation_colour_or_a_tab)
{
    hook.m_sSetup.width = 40;
    BOOST_TEST(Parse("See this,https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion now") ==
               "See\n\rthis,https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion\n\rnow");
    BOOST_TEST(Visible(Parse("Wiki:<div class='cpw'>https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion</div> now")) ==
               "Wiki:https://wiki.dikumud.net/wiki/Help/Raiseskeletalchampion\n\rnow");
    std::string indented = std::string(30, ' ') + "https://x.org/abc\tfoo";
    BOOST_TEST(Parse(indented.c_str()) == indented);
}

// Before: "Wiki: https://x.org/s are fun" - a different, wrong address
BOOST_AUTO_TEST_CASE(text_after_an_address_is_kept_apart)
{
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>s are fun") == "Wiki: https://x.org/ s are fun");
    BOOST_TEST(Parse("<a href='https://x.org/'></a>text") == "https://x.org/ text");
    BOOST_TEST(Visible(Parse("<a href='https://x.org/'>Wiki</a><div class='cpw'>s</div>")) == "Wiki: https://x.org/ s");
    BOOST_TEST(Parse("(see <a href='https://x.org/'>Wiki</a>).") == "(see Wiki: https://x.org/).");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a/>") == "Wiki: https://x.org/");
}

// Before: words were matched in two ways that disagreed, so these printed
// the address again; and a cut text could hide an address it didn't show
BOOST_AUTO_TEST_CASE(text_shows_the_address_as_a_word)
{
    BOOST_TEST(Parse("<a href='https://x.org/'>Go to HTTPS://X.ORG/</a>") == "Go to HTTPS://X.ORG/");
    BOOST_TEST(Parse("<a href='https://x.org/'>see https://x.org now</a>") == "see https://x.org now");
    BOOST_TEST(Parse("<a href='https://regexr.com/'>Go to https://regexr.com</a>") == "Go to https://regexr.com");
    BOOST_TEST(Parse("<a href='https://x.org/'>(https://x.org/)</a>") == "(https://x.org/)");
    std::string text = "<a href='https://x.org/'>" + std::string(497, 'a') + " https://x.org/abc</a>";
    std::string out = Parse(text.c_str());
    BOOST_TEST(out.substr(out.size() - 19) == "abc: https://x.org/");
}

// Before: a line after a newline held one character less than the width,
// a line ending in a blank at the width got an empty line after it, and a
// tab after the break indented the next line
BOOST_AUTO_TEST_CASE(wrap_after_newlines_blanks_and_tabs)
{
    hook.m_sSetup.width = 40;
    std::string full = std::string(35, 'a') + " bbbb"; // 40 characters
    BOOST_TEST(Parse(("x\n" + full).c_str()) == "x\n\r" + full);
    BOOST_TEST(Parse((std::string(40, 'a') + " <br/>more").c_str()) == std::string(40, 'a') + "\n\rmore");
    hook.m_sSetup.width = 6;
    BOOST_TEST(Parse("aaaa \tbbbb") == "aaaa\n\rbbbb");
}

// The address may be glued to other text, end in punctuation of its own,
// or hold a blank, and still count as shown
BOOST_AUTO_TEST_CASE(text_shows_the_address_anywhere)
{
    BOOST_TEST(Parse("<a href='https://en.wikipedia.org/wiki/Mercury_(planet)'>https://en.wikipedia.org/wiki/Mercury_(planet)</a>") ==
               "https://en.wikipedia.org/wiki/Mercury_(planet)");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki:https://x.org/</a>") == "Wiki:https://x.org/");
    BOOST_TEST(Parse("<a href='https://x.org/'>URL=https://x.org/</a>") == "URL=https://x.org/");
    BOOST_TEST(Parse("<a href='https://x.org/a b'>https://x.org/a b</a>") == "https://x.org/a b");
    std::string text = "<a href='https://x.org/'>" + std::string(496, 'a') + " https://x.org/</a>"; // 511 visible chars
    std::string out = Parse(text.c_str());
    BOOST_TEST(out.find("https://x.org/") == out.rfind("https://x.org/"));
}

// Punctuation stays attached to an address only where it ends a sentence,
// judged on the character written, not on how the HTML spells it
BOOST_AUTO_TEST_CASE(punctuation_after_an_address)
{
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>'s page") == "Wiki: https://x.org/ 's page");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>?q") == "Wiki: https://x.org/ ?q");
    BOOST_TEST(Parse("(see &lt;<a href='https://x.org/'>Wiki</a>&gt;)") == "(see <Wiki: https://x.org/>)");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>&nbsp;more") == "Wiki: https://x.org/ more");
    BOOST_TEST(Parse("<a href='https://x.org/'>https://x.org/</a>s") == "https://x.org/ s");
}

// Before: a colour code between the break and a newline or tab hid them;
// and a break at the end of the text dropped the line break
BOOST_AUTO_TEST_CASE(wrap_looks_past_colour_codes)
{
    hook.m_sSetup.width = 40;
    std::string a40(40, 'a');
    BOOST_TEST(Visible(Parse((a40 + " <div class='cpw'></div><br/>more").c_str())) == a40 + "\n\rmore");
    BOOST_TEST(Parse((a40 + " ").c_str()) == a40 + "\n\r");
    hook.m_sSetup.width = 6;
    BOOST_TEST(Visible(Parse("aaaa <div class='cpw'>\tbbbb")) == "aaaa\n\rbbbb");
}

// Before: a break before a long run of blanks and colour codes copied the
// whole run at once, past the room IndentText() keeps free in dest
BOOST_AUTO_TEST_CASE(break_before_many_colour_codes_stays_in_the_buffer)
{
    std::string in(39, 'a');
    for (int i = 0; i < 40; i++)
    {
        in += " \x1b[0;37m";
    }
    in += "b";
    char out[400];
    hook.IndentText(in.c_str(), out, 160, 40);
    BOOST_TEST(strlen(out) < 160);
}

// A tag that prints nothing doesn't end the address; a line break does
BOOST_AUTO_TEST_CASE(tags_after_punctuation_after_an_address)
{
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>,<b>more</b>") == "Wiki: https://x.org/ ,more");
    BOOST_TEST(Parse("(see <a href='https://x.org/'>Wiki</a>).<br/>Next") == "(see Wiki: https://x.org/).\n\rNext");
}

// What follows punctuation after an address is judged on what StripHTML()
// writes: entities decoded, skipped <script>, and only real line breaks
BOOST_AUTO_TEST_CASE(punctuation_judged_on_the_output)
{
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>.<b>&nbsp;</b>more") == "Wiki: https://x.org/. more");
    BOOST_TEST(Visible(Parse("<a href='https://x.org/'>Wiki</a>.<script>PasswordOn()</script>")).find("https://x.org/.") != std::string::npos);
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>.<br/>Next") == "Wiki: https://x.org/.\n\rNext");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>.<br />Next") == "Wiki: https://x.org/.\n\rNext");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>.<a href='https://y.org/'>Two</a>") ==
               "Wiki: https://x.org/ .Two: https://y.org/");
}

BOOST_AUTO_TEST_CASE(fixture_puts_globals_back)
{
    // A second hook on top of the fixture's, as each test case makes one
    int left = mplex::g_nConnectionsLeft;
    {
        SavedGlobals saved;
        mplex::cConHook other;
    }
    BOOST_TEST(mplex::g_nConnectionsLeft == left);
    BOOST_TEST(mplex::g_mplex_arg.bWebSockets == TRUE); // Still the fixture's
}

BOOST_AUTO_TEST_SUITE_END()

/**********************************************************************************
 * StripHTML_Suite
 */
BOOST_FIXTURE_TEST_SUITE(StripHTML_Suite, TelnetHook)

/* ----- Entities ----- */

// &apos; is 6 characters. Skipping only 5 left the ';' behind, so an
// engraving of 'DawnAngel' showed as ';DawnAngel'; on telnet.
BOOST_AUTO_TEST_CASE(apos_has_no_trailing_semicolon)
{
    BOOST_TEST(Strip("&apos;") == "'");
    BOOST_TEST(Strip("(engraved: &apos;DawnAngel&apos;)") == "(engraved: 'DawnAngel')");
    BOOST_TEST(Strip("You can&apos;t do that.") == "You can't do that.");
}

BOOST_AUTO_TEST_CASE(other_entities_unchanged)
{
    BOOST_TEST(Strip("&lt;&gt;&amp;&quot;&nbsp;") == "<>&\" ");
    BOOST_TEST(Strip("&LT;&Amp;") == "<&");
    BOOST_TEST(Strip("&unknown;") == "&unknown;");
    BOOST_TEST(Strip("a & b") == "a & b");
}

// The help pages from the wiki use &#160; and the server escapes ' in
// names as &#39;; both showed as they were
BOOST_AUTO_TEST_CASE(numeric_entities_decoded)
{
    BOOST_TEST(Strip("&lt;value&gt;&#160;: a value") == "<value> : a value");
    BOOST_TEST(Strip("It&#39;s") == "It's");
    BOOST_TEST(Strip("It&#x27;s &#X27;") == "It's '");
    BOOST_TEST(Strip("&#xa0;&#xA0;") == "  ");
}

// Past ASCII the character is sent as UTF-8, like the rest of the text
BOOST_AUTO_TEST_CASE(numeric_entities_past_ascii_become_utf8)
{
    BOOST_TEST(Strip("&#233;") == "\xc3\xa9");            // é
    BOOST_TEST(Strip("&#8217;") == "\xe2\x80\x99");       // ’
    BOOST_TEST(Strip("&#x1F600;") == "\xf0\x9f\x98\x80"); // 😀
}

// Control characters would reach the terminal as they are, and what isn't a
// character at all can't be decoded: these stay as they were sent
BOOST_AUTO_TEST_CASE(numeric_entities_that_are_not_text_kept)
{
    BOOST_TEST(Strip("&#27;[2J") == "&#27;[2J");   // ESC
    BOOST_TEST(Strip("&#0;&#10;") == "&#0;&#10;"); // NUL, newline
    BOOST_TEST(Strip("&#127;&#155;") == "&#127;&#155;");
    BOOST_TEST(Strip("&#xD800;") == "&#xD800;");     // Half a UTF-16 pair
    BOOST_TEST(Strip("&#x110000;") == "&#x110000;"); // Past Unicode
    BOOST_TEST(Strip("&#99999999;") == "&#99999999;");
    BOOST_TEST(Strip("&#39") == "&#39"); // No ';'
    BOOST_TEST(Strip("&#;&#x;") == "&#;&#x;");
}

/* ----- <br/> ----- */

// The help pages from the wiki use <br />, which was dropped
BOOST_AUTO_TEST_CASE(br_becomes_newline)
{
    BOOST_TEST(Strip("a<br/>b") == "a\n\rb");
    BOOST_TEST(Strip("a<br>b") == "a\n\rb");
    BOOST_TEST(Strip("a<br />b") == "a\n\rb");
    BOOST_TEST(Strip("a<BR />b") == "a\n\rb");
    BOOST_TEST(Strip("a<brick>b") == "ab"); // Not a <br>
}

// <br/> right before a real newline is dropped so lines aren't doubled
BOOST_AUTO_TEST_CASE(br_before_newline_not_doubled)
{
    BOOST_TEST(Strip("a<br/>\n\rb") == "a\n\rb");
    BOOST_TEST(Strip("<p><br />\nb") == "\n\rb");
}

// The check looked at the character after the next one, even when the next
// one was the end of the string. Here the byte past the terminator is '\n',
// so the old code read it and dropped the line break.
BOOST_AUTO_TEST_CASE(br_at_end_does_not_read_past_string)
{
    const char text[] = "a<br/>\0\n";
    BOOST_TEST(Strip(text) == "a\n\r");
}

// The same check dropped a <br/> whenever the line after it was one character
// long, e.g. a single "." line between two breaks.
BOOST_AUTO_TEST_CASE(br_before_one_character_line_kept)
{
    BOOST_TEST(Strip("a<br/>.\nb") == "a\n\r.\n\rb");
}

// Zone text often has a blank or tab after <br/> before the line ends, e.g.
// the newspaper articles in offices.zon. That is still the end of the line.
BOOST_AUTO_TEST_CASE(br_before_trailing_blanks_and_newline_not_doubled)
{
    BOOST_TEST(Strip("a<br/> \nb") == "a \n\rb");
    BOOST_TEST(Strip("a<br/>\t\nb") == "a\t\n\rb");
    BOOST_TEST(Strip("a<br/>   \nb") == "a   \n\rb");
}

// Blanks followed by more text are not a line end, so the break stays
BOOST_AUTO_TEST_CASE(br_before_blanks_and_text_kept)
{
    BOOST_TEST(Strip("a<br/> b") == "a\n\r b");
}

/* ----- <data> ----- */

// <data> carries data for the web client. The server drops it for telnet;
// what reaches mplex anyway must not show either. Telnet used to drop the
// tags but show what was between them.
BOOST_AUTO_TEST_CASE(data_not_shown)
{
    BOOST_TEST(Strip("<data type='$json'>{\"name\":\"bob\"}</data>Sending json edit info to your browser.<br/>") ==
               "Sending json edit info to your browser.\n\r");
    BOOST_TEST(Strip("a<data type='$alias'><li>n north</li><li>s &amp; south<br/></li></data>b"
                     "<data type='$combat'>{\"round\":17,\"list\":[]}</data>c") == "abc");
    BOOST_TEST(Parse("a<data type='$json'>{\"name\":\"bob\"}</data>b") == "ab"); // All of the telnet path
}

// Without a </data> everything after <data> is data: it can't be told apart
BOOST_AUTO_TEST_CASE(unclosed_data_not_shown)
{
    BOOST_TEST(Strip("a<data type='$json'>{\"name\":\"bob\"") == "a");
}

// A self-closed <data/> holds nothing, and <database> is not <data>
BOOST_AUTO_TEST_CASE(only_data_with_contents_hidden)
{
    BOOST_TEST(Strip("a<data type='$json'/>b") == "ab");
    BOOST_TEST(Strip("a<database>b</database>c") == "abc");
}

// Tags are read as a browser reads them. Before, these left the rest of the
// text hidden, or showed the data.
BOOST_AUTO_TEST_CASE(data_tags_in_any_form)
{
    BOOST_TEST(Strip("a<DATA type='$json'>{}</DATA>b") == "ab");
    BOOST_TEST(Strip("a<data type='$json'>{}</data >b") == "ab");
    BOOST_TEST(Strip("a<data type='$json'>{}</Data\t>b") == "ab");
    BOOST_TEST(Strip("a<data\ttype='$json'>{}</data>b") == "ab");
    BOOST_TEST(Strip("a<data>{}</data>b") == "ab");
}

BOOST_AUTO_TEST_CASE(self_closed_data_in_any_form)
{
    BOOST_TEST(Strip("a<data type='$json' / >b") == "ab");
    BOOST_TEST(Strip("a<data/>b") == "ab");
    std::string html = "a<data type='" + std::string(300, 'x') + "'/>b"; // Longer than getHTMLTag() reads
    BOOST_TEST(Strip(html.c_str()) == "ab");
}

BOOST_AUTO_TEST_CASE(data_edge_cases)
{
    BOOST_TEST(Strip("a<datum>b</datum><datalist>c</datalist>") == "abc");
    BOOST_TEST(Strip("a<data type='$json'") == "a");
    BOOST_TEST(Strip("a<data>{}</datum>b") == "a");
    BOOST_TEST(Strip("a<data>1</data>b<data>2</data>c") == "abc");
}

// The server splits text over 32 KB into pieces that mplex reads one by
// one, but drops <data> for telnet before that. A piece here can't be told
// from the rest of the output, so the next output is shown.
BOOST_AUTO_TEST_CASE(unclosed_data_hides_only_the_rest_of_its_output)
{
    BOOST_TEST(Strip("a<data type='$json'>{\"name\":") == "a");
    BOOST_TEST(Strip("Hello there") == "Hello there");
}

// jstat sends telnet its JSON escaped, which shows as it was
BOOST_AUTO_TEST_CASE(escaped_json_shows_as_it_was)
{
    const char *json = "{\"title\":\"Dain's <b>\\\"axe\\\"</b> &amp; co<br/>\",\"n\":1}";
    BOOST_TEST(Strip(str_html_attr_escape(json).c_str()) == json);
}

/* ----- </div> and colours ----- */

// New characters use the TTY emulation. </div> must still reset the
// background to black there, or text after a coloured block keeps the
// terminal's own background (white on white on a light terminal).
BOOST_AUTO_TEST_CASE(close_div_sends_black_background_in_tty_mode)
{
    hook.m_sSetup.emulation = TERM_TTY;
    std::string out = Strip("<div class='cg bb'>x</div>y");

    auto close = out.find('x');
    BOOST_TEST(out.find("\033[40m", close) != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

/**********************************************************************************
 * ClientConnector_TLS_Suite
 *
 * The telnets transport (mplex -z). Each test accepts a cConHook from a real
 * listening socket on 127.0.0.1, puts a non-blocking OpenSSL client on the
 * other end, and drives the server side with g_CaptainHook.Wait(), as
 * mplex's main loop does. No mud is connected, so every hook stays at
 * m_nId == 0: the state in which Close() alone does nothing.
 */
namespace
{
// A self-signed certificate and its key in one PEM, the way mplex reads server.pem
struct SelfSigned
{
    std::string pem;

    explicit SelfSigned(long serial)
    {
        EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
        BOOST_REQUIRE(pctx);
        BOOST_REQUIRE(EVP_PKEY_keygen_init(pctx) == 1);
        BOOST_REQUIRE(EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1) == 1);
        EVP_PKEY *key = nullptr;
        BOOST_REQUIRE(EVP_PKEY_keygen(pctx, &key) == 1);
        EVP_PKEY_CTX_free(pctx);

        X509 *x = X509_new();
        X509_set_version(x, 2);
        ASN1_INTEGER_set(X509_get_serialNumber(x), serial);
        X509_gmtime_adj(X509_getm_notBefore(x), -60);
        X509_gmtime_adj(X509_getm_notAfter(x), 3600);
        X509_set_pubkey(x, key);
        X509_NAME *name = X509_get_subject_name(x);
        X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char *)"mplex unit test", -1, -1, 0);
        X509_set_issuer_name(x, name);
        BOOST_REQUIRE(X509_sign(x, key, EVP_sha256()) > 0);

        BIO *bio = BIO_new(BIO_s_mem());
        PEM_write_bio_PrivateKey(bio, key, nullptr, nullptr, 0, nullptr, nullptr);
        PEM_write_bio_X509(bio, x);
        char *p = nullptr;
        long n = BIO_get_mem_data(bio, &p);
        pem.assign(p, n);

        BIO_free(bio);
        X509_free(x);
        EVP_PKEY_free(key);
    }
};

long PeerSerial(SSL *ssl)
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    X509 *x = SSL_get1_peer_certificate(ssl);
#else
    X509 *x = SSL_get_peer_certificate(ssl);
#endif
    BOOST_REQUIRE(x);
    long serial = ASN1_INTEGER_get(X509_get_serialNumber(x));
    X509_free(x);
    return serial;
}

// What another connection's failure leaves on this thread's OpenSSL error queue
void LeaveAnErrorOnTheQueue()
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    ERR_raise(ERR_LIB_SSL, SSL_R_PACKET_LENGTH_TOO_LONG);
#else
    ERR_put_error(ERR_LIB_SSL, 0, SSL_R_PACKET_LENGTH_TOO_LONG, __FILE__, __LINE__);
#endif
}

std::string g_lastCommand; // what the hook handed its state function

void RecordCommand(mplex::cConHook *, const char *cmd)
{
    g_lastCommand = cmd;
}

// A non-blocking TLS client on an already connected socket
struct TlsClient
{
    SSL_CTX *ctx;
    SSL *ssl;

    explicit TlsClient(int fd)
    {
        ctx = SSL_CTX_new(TLS_client_method());
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, fd);
        SSL_set_connect_state(ssl);
    }

    ~TlsClient()
    {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
    }

    // One step; true when complete
    bool Handshake()
    {
        ERR_clear_error();
        int r = SSL_do_handshake(ssl);
        if (r == 1)
        {
            return true;
        }
        int e = SSL_get_error(ssl, r);
        BOOST_REQUIRE_MESSAGE(e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE, "client handshake failed: " << e);
        return false;
    }

    // Appends what is available; -1 once the server has closed
    int Read(std::string &out)
    {
        char buf[4096];
        ERR_clear_error();
        int r = SSL_read(ssl, buf, sizeof(buf));
        if (r > 0)
        {
            out.append(buf, r);
            return r;
        }
        int e = SSL_get_error(ssl, r);
        return (e == SSL_ERROR_WANT_READ || e == SSL_ERROR_WANT_WRITE) ? 0 : -1;
    }

    void Write(const std::string &s)
    {
        ERR_clear_error();
        BOOST_REQUIRE_EQUAL(SSL_write(ssl, s.data(), (int)s.size()), (int)s.size());
    }
};

struct TlsFixture
{
    SavedGlobals saved{FALSE, TRUE}; // real sockets, telnet negotiation on; put back when the test is over
    std::string path;                // the PEM file tls_init_telnet() reads
    int listenFd = -1;
    int port = 0;
    std::vector<mplex::cConHook *> hooks;
    std::vector<int> clients;

    TlsFixture()
    {
        mplex::translate_init(); // the colour codes, as mplex's main() sets them up

        char tmpl[] = "/tmp/mplex-tls-test-XXXXXX";
        int fd = mkstemp(tmpl);
        BOOST_REQUIRE(fd >= 0);
        close(fd);
        path = tmpl;
        WritePem(SelfSigned(1));

        listenFd = socket(AF_INET, SOCK_STREAM, 0);
        BOOST_REQUIRE(listenFd >= 0);
        sockaddr_in a{};
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        BOOST_REQUIRE_EQUAL(bind(listenFd, (sockaddr *)&a, sizeof(a)), 0);
        BOOST_REQUIRE_EQUAL(listen(listenFd, 5), 0);
        socklen_t len = sizeof(a);
        BOOST_REQUIRE_EQUAL(getsockname(listenFd, (sockaddr *)&a, &len), 0);
        port = ntohs(a.sin_port);
        fcntl(listenFd, F_SETFL, O_NONBLOCK);
        mplex::g_MotherHook.Hook(listenFd); // cConHook() accepts from g_MotherHook.tfd()
    }

    ~TlsFixture()
    {
        for (auto *h : hooks)
        {
            h->Unhook(); // out of g_CaptainHook's table before it is gone
            delete h;
        }
        for (int c : clients)
        {
            if (c >= 0)
            {
                close(c);
            }
        }
        mplex::g_MotherHook.Hook(-1); // ours to close, not the hook's
        close(listenFd);
        mplex::tls_free_telnet();
        unlink(path.c_str());
        g_lastCommand.clear();
    }

    void WritePem(const SelfSigned &c, int mtimeOffset = 0)
    {
        FILE *f = fopen(path.c_str(), "w");
        BOOST_REQUIRE(f);
        fwrite(c.pem.data(), 1, c.pem.size(), f);
        fclose(f);

        if (mtimeOffset)
        {
            // So the change shows even on a coarse clock
            struct stat st = {};
            BOOST_REQUIRE_EQUAL(stat(path.c_str(), &st), 0);
            timespec ts[2] = {st.st_atim, st.st_mtim};
            ts[1].tv_sec += mtimeOffset;
            BOOST_REQUIRE_EQUAL(utimensat(AT_FDCWD, path.c_str(), ts, 0), 0);
        }
    }

    bool Init() { return mplex::tls_init_telnet(path.c_str()); }

    // A connected, non-blocking client socket
    int Connect()
    {
        int c = socket(AF_INET, SOCK_STREAM, 0);
        BOOST_REQUIRE(c >= 0);
        sockaddr_in a{};
        a.sin_family = AF_INET;
        a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        a.sin_port = htons(port);
        BOOST_REQUIRE_EQUAL(connect(c, (sockaddr *)&a, sizeof(a)), 0);
        fcntl(c, F_SETFL, O_NONBLOCK);
        clients.push_back(c);
        return c;
    }

    void Forget(int c) // the test closed it itself
    {
        for (int &x : clients)
        {
            if (x == c)
            {
                x = -1;
            }
        }
    }

    // What cMotherHook::Input() does: accept, then write to the client
    // before anything has come back from it (there: "Requesting an ID")
    mplex::cConHook *Accept(const char *greeting = "Welcome<br/>")
    {
        auto *h = new mplex::cConHook();
        hooks.push_back(h);
        h->m_pFptr = RecordCommand; // not MenuSelect(): there is no mud to ask
        h->SendCon(greeting);
        return h;
    }

    // One pass of mplex's main loop: select() with a timeout, then Input()
    // and PushWrite() on whatever is ready. Returns what select() returned.
    int Loop(int ms = 10)
    {
        timeval tv{0, ms * 1000};
        return g_CaptainHook.Wait(&tv);
    }

    // Server passes and client steps, alternating, until step() is true
    template <class Step>
    bool Until(Step step, int passes = 200)
    {
        for (int i = 0; i < passes; i++)
        {
            Loop();
            if (step())
            {
                return true;
            }
        }
        return false;
    }

    // Handshake to completion and the server's queue flushed to the client
    void Connected(TlsClient &client, mplex::cConHook *h, std::string &got)
    {
        BOOST_REQUIRE(Until([&] { return client.Handshake(); }));
        BOOST_REQUIRE(Until([&] {
            client.Read(got);
            return got.find("Welcome") != std::string::npos;
        }));
        BOOST_REQUIRE(h->IsHooked());
    }
};
} // namespace

BOOST_AUTO_TEST_SUITE(ClientConnector_TLS_Suite)

BOOST_FIXTURE_TEST_CASE(a_missing_pem_file_is_a_failure_not_a_plain_port, TlsFixture)
{
    BOOST_TEST(!mplex::tls_init_telnet("/nonexistent/server.pem"));
    BOOST_TEST(mplex::tls_ctx_for_accept() == nullptr);
    BOOST_TEST(Init());
    BOOST_TEST(mplex::tls_ctx_for_accept() != nullptr);
}

BOOST_FIXTURE_TEST_CASE(a_silent_client_costs_nothing_while_its_handshake_is_pending, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept(); // queues the telnet negotiation and the greeting

    // Nothing can happen until the client speaks: select() must sleep the
    // whole timeout, not return at once because the socket is writable
    auto t0 = std::chrono::steady_clock::now();
    int n = Loop(100);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
    BOOST_TEST(n == 0);
    BOOST_TEST(ms >= 90);
    BOOST_TEST(h->IsHooked());

    // and nothing went to the client in clear
    char buf[64];
    int r = recv(c, buf, sizeof(buf), MSG_DONTWAIT);
    int e = errno; // before any logging can touch it
    BOOST_TEST(r == -1);
    BOOST_TEST(e == EAGAIN);
}

BOOST_FIXTURE_TEST_CASE(a_plain_telnet_client_is_dropped_even_without_an_id, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();

    BOOST_REQUIRE_EQUAL(send(c, "hello\r\n", 7, 0), 7);
    BOOST_TEST(Until([&] { return !h->IsHooked(); }));

    // The client gets the alert and then EOF, and the socket is out of the
    // table: the loop has nothing left to do with it
    char buf[256];
    int r = -1;
    for (int i = 0; i < 50 && (r = recv(c, buf, sizeof(buf), MSG_DONTWAIT)) != 0; i++)
    {
        if (r < 0)
        {
            usleep(1000);
        }
    }
    BOOST_TEST(r == 0);
    BOOST_TEST(Loop(20) == 0);
}

BOOST_FIXTURE_TEST_CASE(negotiation_goes_first_then_what_was_queued_then_input_flows, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();
    TlsClient client(c);
    std::string got;
    Connected(client, h, got);

    BOOST_REQUIRE(got.size() >= 3);
    BOOST_TEST((unsigned char)got[0] == 255); // IAC
    BOOST_TEST((unsigned char)got[1] == 253); // DO
    BOOST_TEST((unsigned char)got[2] == 31);  // NAWS
    auto welcome = got.find("Welcome");
    BOOST_TEST(welcome != std::string::npos);
    BOOST_TEST(welcome >= 3);

    client.Write("look\r\n");
    BOOST_TEST(Until([&] { return g_lastCommand == "look"; }));
    BOOST_TEST(h->IsHooked());
}

BOOST_FIXTURE_TEST_CASE(a_socket_error_while_sending_drops_the_connection, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();
    TlsClient client(c);
    std::string got;
    Connected(client, h, got); // handshake done, queue flushed

    // The next write to the socket fails hard (EPIPE), in the middle of an ordinary send
    BOOST_REQUIRE_EQUAL(shutdown(h->tfd(), SHUT_WR), 0);
    h->SendCon("one more line<br/>");

    BOOST_TEST(!h->IsHooked());
    BOOST_TEST(Loop(20) == 0);
}

BOOST_FIXTURE_TEST_CASE(output_waits_while_the_client_is_not_reading, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();
    TlsClient client(c);
    std::string got;
    Connected(client, h, got);

    // Small buffers, so that a little output fills the pipe to a client that does not read
    int size = 4096;
    setsockopt(h->tfd(), SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    setsockopt(c, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    std::string line(500, 'x');
    line += "<br/>";
    for (int i = 0; i < 400; i++)
    {
        h->SendCon(line.c_str()); // 200 KB, far more than the pipe takes
    }
    BOOST_TEST(h->IsHooked()); // back-pressure is not an error

    // and the client keeps typing meanwhile: read and handled as usual
    for (int i = 0; i < 50; i++)
    {
        client.Write("look\r\n");
    }
    BOOST_TEST(Until([&] { return g_lastCommand == "look"; }));
    BOOST_TEST(h->IsHooked());

    // when the client reads, everything arrives
    auto xs = [&] { return std::count(got.begin(), got.end(), 'x'); };
    BOOST_TEST(Until(
        [&] {
            client.Read(got);
            return xs() >= 200000;
        },
        3000));
    BOOST_TEST(xs() == 200000);
    BOOST_TEST(h->IsHooked());
}

BOOST_FIXTURE_TEST_CASE(a_client_that_resets_after_the_handshake_is_dropped, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();
    TlsClient client(c);
    std::string got;
    Connected(client, h, got);

    linger l{1, 0}; // RST: no FIN, no close_notify
    setsockopt(c, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
    close(c);
    Forget(c);

    BOOST_TEST(Until([&] { return !h->IsHooked(); }));
    BOOST_TEST(Loop(20) == 0);
}

BOOST_FIXTURE_TEST_CASE(a_stale_entry_on_the_error_queue_does_not_cost_a_connection, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c = Connect();
    auto *h = Accept();
    TlsClient client(c);
    std::string got;
    Connected(client, h, got);

    LeaveAnErrorOnTheQueue();
    h->Input(SELECT_READ); // nothing to read: must come back as "nothing yet", not as a TLS error
    BOOST_TEST(h->IsHooked());

    LeaveAnErrorOnTheQueue();
    h->SendCon("still here<br/>");
    BOOST_TEST(h->IsHooked());
    BOOST_TEST(Until([&] {
        client.Read(got);
        return got.find("still here") != std::string::npos;
    }));
}

BOOST_FIXTURE_TEST_CASE(a_renewed_certificate_is_used_for_the_next_connection, TlsFixture)
{
    BOOST_REQUIRE(Init());
    int c1 = Connect();
    auto *h1 = Accept();
    TlsClient client1(c1);
    std::string got1;
    Connected(client1, h1, got1);
    BOOST_TEST(PeerSerial(client1.ssl) == 1);

    WritePem(SelfSigned(2), 2);

    int c2 = Connect();
    auto *h2 = Accept();
    TlsClient client2(c2);
    std::string got2;
    Connected(client2, h2, got2);
    BOOST_TEST(PeerSerial(client2.ssl) == 2);

    // and the first connection is unaffected
    h1->SendCon("still here<br/>");
    BOOST_TEST(Until([&] {
        client1.Read(got1);
        return got1.find("still here") != std::string::npos;
    }));
}

BOOST_FIXTURE_TEST_CASE(a_plain_client_that_leaves_before_it_has_an_id_is_dropped, TlsFixture)
{
    // No TLS. The mud has not assigned an id (it is down, say), and the
    // client goes away: the socket must leave the table instead of sitting
    // there with EOF readable, which would make select() return at once on
    // every pass once the mud is back.
    int c = Connect();
    auto *h = Accept();
    Loop(20);
    char buf[256];
    BOOST_REQUIRE(recv(c, buf, sizeof(buf), MSG_DONTWAIT) > 0); // take what was sent, so that closing sends FIN, not RST
    close(c);
    Forget(c);

    BOOST_TEST(Until([&] { return !h->IsHooked(); }));
    BOOST_TEST(Loop(20) == 0);
}

BOOST_FIXTURE_TEST_CASE(the_plain_port_behaves_as_before, TlsFixture)
{
    // No tls_init_telnet(): a plain hook, negotiation out at once, in clear
    int c = Connect();
    auto *h = Accept();
    Loop(20);

    char buf[256];
    int r = recv(c, buf, sizeof(buf), MSG_DONTWAIT);
    BOOST_REQUIRE(r >= 3);
    BOOST_TEST((unsigned char)buf[0] == 255);
    BOOST_TEST((unsigned char)buf[1] == 253);
    BOOST_TEST(std::string(buf, r).find("Welcome") != std::string::npos);
    BOOST_TEST(h->IsHooked());
}

BOOST_AUTO_TEST_SUITE_END()
