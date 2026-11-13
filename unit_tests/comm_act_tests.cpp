#include "FixtureBase.h"
#include "comm.h"
#include "essential.h"
#include "unit_data.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * Act_Generate_Suite - act_generate() keeps what it writes inside its MAX_STRING_LENGTH buffer
 */
BOOST_AUTO_TEST_SUITE(Act_Generate_Suite)

struct ActFixture : public unit_tests::FixtureBase
{
    std::unique_ptr<unit_data> npc{new_unit_data(UNIT_ST_NPC, nullptr)};
    // The buffer act_generate expects, with a guard after it that must stay untouched
    std::vector<char> buf = std::vector<char>(MAX_STRING_LENGTH + 1024, '#');

    std::string generate(const char *str, const char *text, int bNewline = 1)
    {
        act_generate(buf.data(), str, eA_RAW, npc.get(), text, cActParameter(), eTO_CHAR, npc.get(), bNewline);
        return buf.data();
    }

    bool guard_untouched() const { return std::all_of(buf.begin() + MAX_STRING_LENGTH, buf.end(), [](char c) { return c == '#'; }); }
};

BOOST_FIXTURE_TEST_CASE(short_text, ActFixture)
{
    BOOST_TEST(generate("(LOG: $2t)", "all is well") == "(LOG: all is well)<br/>");
    BOOST_TEST(generate("<div class='x'>hello</div>", "") == "<div class='x'>Hello</div><br/>");
}

// A log line far longer than the buffer (it crashed the server in 2022)
BOOST_FIXTURE_TEST_CASE(long_argument_is_cut_short, ActFixture)
{
    std::string text(3 * MAX_STRING_LENGTH, 'x');
    std::string result = generate("(LOG: $2t)", text.c_str());
    BOOST_TEST(result.size() < MAX_STRING_LENGTH);
    BOOST_TEST(result.rfind("(LOG: xxx", 0) == 0);
    BOOST_TEST(result.substr(result.size() - 5) == "<br/>");
    BOOST_TEST(guard_untouched());
}

BOOST_FIXTURE_TEST_CASE(long_message_is_cut_short, ActFixture)
{
    std::string str(3 * MAX_STRING_LENGTH, 'y');
    std::string result = generate(str.c_str(), "");
    BOOST_TEST(result.size() < MAX_STRING_LENGTH);
    BOOST_TEST(guard_untouched());
}

// A '<' with no '>' after it (no <br/> either, as from sact) used to loop forever
BOOST_FIXTURE_TEST_CASE(unclosed_tag, ActFixture)
{
    BOOST_TEST(generate("<3 $2t", "you", 0) == "<3 you");
}

BOOST_AUTO_TEST_SUITE_END()
