#include "ClientConnector.h"
#include "MUDConnector.h"
#include "mplex.h"
#include "textutil.h"
#include "translate.h"

#include <cctype>
#include <cstring>
#include <string>
#include <utility>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * ClientConnector_Suite
 *
 * cConHook::ParseOutput() turns the server's HTML into telnet text:
 * StripHTML() removes the tags and IndentText() wraps the lines.
 *
 * StripHTML_Suite (at the end) tests StripHTML() on its own: entities,
 * <br/>, <data> and colours.
 */
namespace
{
// Puts back the globals a hook changes, once the hook is gone: in websocket
// mode cConHook() doesn't take a connection, but ~cConHook() gives one back
struct SavedGlobals
{
    ~SavedGlobals()
    {
        mplex::g_mplex_arg.bWebSockets = webSockets;
        mplex::g_nConnectionsLeft = connectionsLeft;
    }

    int webSockets = std::exchange(mplex::g_mplex_arg.bWebSockets, TRUE);
    int connectionsLeft = mplex::g_nConnectionsLeft;
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
