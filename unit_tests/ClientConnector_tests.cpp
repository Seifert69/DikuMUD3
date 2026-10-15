#include "ClientConnector.h"
#include "MUDConnector.h"
#include "mplex.h"
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
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>.<br />Next") == "Wiki: https://x.org/ .Next");
    BOOST_TEST(Parse("<a href='https://x.org/'>Wiki</a>.<br/>Next") == "Wiki: https://x.org/.\n\rNext");
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
