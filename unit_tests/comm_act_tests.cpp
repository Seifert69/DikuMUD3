#include "FixtureBase.h"
#include "comm.h"
#include "essential.h"
#include "handler.h"
#include "unit_data.h"

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

// comm.cpp: act_walk(), the walk act() and cact() share, for these tests
void act_recipients(cActParameter arg1, cActParameter arg3, actType_e type, const std::function<void(const unit_data *)> &send);

/**********************************************************************************
 * Act_Generate_Suite - act_generate() keeps what it writes inside its MAX_STRING_LENGTH buffer
 */
BOOST_AUTO_TEST_SUITE(Act_Generate_Suite)

struct ActFixture : public unit_tests::FixtureBase
{
    std::unique_ptr<unit_data> npc{new_unit_data(UNIT_ST_NPC, nullptr)};
    // The buffer act_generate expects, with a guard after it that must stay untouched
    std::vector<char> buf = std::vector<char>(MAX_STRING_LENGTH + 1024, '#');

    bool whole = false; // what act_generate said

    std::string generate(const char *str, const char *text)
    {
        whole = act_generate(buf.data(), str, eA_RAW, npc.get(), text, cActParameter(), eTO_CHAR, npc.get());
        return buf.data();
    }

    bool guard_untouched() const { return std::all_of(buf.begin() + MAX_STRING_LENGTH, buf.end(), [](char c) { return c == '#'; }); }
};

BOOST_FIXTURE_TEST_CASE(short_text, ActFixture)
{
    BOOST_TEST(generate("(LOG: $2t)", "all is well") == "(LOG: all is well)");
    BOOST_TEST(generate("<div class='x'>hello</div>", "") == "<div class='x'>Hello</div>");
}

// A log line far longer than the buffer (it crashed the server in 2022)
BOOST_FIXTURE_TEST_CASE(long_argument_is_cut_short, ActFixture)
{
    std::string text(3 * MAX_STRING_LENGTH, 'x');
    std::string result = generate("(LOG: $2t)", text.c_str());
    BOOST_TEST(result.size() < MAX_STRING_LENGTH);
    BOOST_TEST(result.rfind("(LOG: xxx", 0) == 0);
    BOOST_TEST(result.size() + sizeof("<br/>") <= MAX_STRING_LENGTH); // room left for the <br/>
    BOOST_TEST(guard_untouched());
}

BOOST_FIXTURE_TEST_CASE(long_message_is_cut_short, ActFixture)
{
    std::string str(3 * MAX_STRING_LENGTH, 'y');
    std::string result = generate(str.c_str(), "");
    BOOST_TEST(result.size() < MAX_STRING_LENGTH);
    BOOST_TEST(guard_untouched());
}

// A '<' with no '>' after it used to loop forever
BOOST_FIXTURE_TEST_CASE(unclosed_tag, ActFixture)
{
    BOOST_TEST(generate("<3 $2t", "you") == "<3 you");
}

// Whole when the text got to its end, even blank or empty (act() sends it with a <br/>); not after
// an illegal $-code (act() sends what came before as it is) or with nothing for "to"
BOOST_FIXTURE_TEST_CASE(whole_text, ActFixture)
{
    BOOST_TEST(generate(" ", "") == " ");
    BOOST_TEST(whole);
    BOOST_TEST(generate("$2t", "") == "");
    BOOST_TEST(whole);
    BOOST_TEST(generate("abc $q", "") == "abc "); // cut short before the capital
    BOOST_TEST(!whole);
    BOOST_TEST(!act_generate(buf.data(), "x", eA_RAW, npc.get(), cActParameter(), cActParameter(), eTO_ROOM, npc.get()));
    BOOST_TEST(buf[0] == 0);
}

BOOST_AUTO_TEST_SUITE_END()

/**********************************************************************************
 * Act_Recipients_Suite - act_recipients(), the one walk act() and cact() share: who an act()
 * reaches. A room with the actor A and B, C; a transparent box T with D inside, a closed box O
 * with E inside.
 */
BOOST_AUTO_TEST_SUITE(Act_Recipients_Suite)

struct RecipientsFixture : public unit_tests::FixtureBase
{
    std::map<std::string, std::unique_ptr<unit_data>> u;
    std::vector<std::string> linked; // in the order linked, unlinked backwards

    RecipientsFixture()
    {
        make("R", UNIT_ST_ROOM);
        make("T", UNIT_ST_OBJ);
        make("O", UNIT_ST_OBJ);
        for (const char *c : {"A", "B", "C", "D", "E"})
        {
            make(c, UNIT_ST_NPC);
        }
        u["T"]->setUnitFlag(UNIT_FL_TRANS);
        put("A", "R");
        put("B", "R");
        put("C", "R");
        put("T", "R");
        put("O", "R");
        put("D", "T");
        put("E", "O");
    }

    ~RecipientsFixture() override
    {
        for (auto it = linked.rbegin(); it != linked.rend(); ++it)
        {
            unit_from_unit(u[*it].get());
        }
    }

    void make(const std::string &name, ubit8 type)
    {
        u[name].reset(new_unit_data(type, nullptr));
        u[name]->getNames().AppendName(name.c_str());
    }

    void put(const std::string &what, const std::string &where)
    {
        unit_to_unit(u[what].get(), u[where].get());
        linked.push_back(what);
    }

    void move(const std::string &what, const std::string &where)
    {
        unit_from_unit(u[what].get());
        unit_to_unit(u[what].get(), u[where].get());
    }

    // Who act_recipients() gives, as their names in order
    std::string who(const std::string &actor, const std::string &victim, actType_e type)
    {
        std::string s;
        act_recipients(cActParameter(actor.empty() ? nullptr : u[actor].get()),
                       cActParameter(victim.empty() ? nullptr : u[victim].get()),
                       type,
                       [&](const unit_data *ch) { s += ch->getNames().Name(); });
        return s;
    }
};

BOOST_FIXTURE_TEST_CASE(the_room_and_transparent_units_in_it, RecipientsFixture)
{
    // Everyone in the room and in the transparent box, not in the closed one; act_generate()
    // leaves the actor (TO_ROOM) or the victim (TO_NOTVICT) out
    for (actType_e type : {eTO_ROOM, eTO_ALL, eTO_REST, eTO_NOTVICT})
    {
        std::string s = who("A", "B", type);
        std::sort(s.begin(), s.end());
        BOOST_TEST(s == "ABCD");
    }
}

BOOST_FIXTURE_TEST_CASE(one_character_for_char_and_vict, RecipientsFixture)
{
    BOOST_TEST(who("A", "B", eTO_CHAR) == "A");
    BOOST_TEST(who("A", "B", eTO_VICT) == "B");
    BOOST_TEST(who("", "B", eTO_CHAR) == "");

    // Without the victim (or the actor), nobody; this walked on to the room around a
    // transparent unit (or crashed on the null actor)
    move("A", "T");
    BOOST_TEST(who("A", "", eTO_VICT) == "");
    BOOST_TEST(who("", "B", eTO_CHAR) == "");
}

BOOST_FIXTURE_TEST_CASE(from_inside_a_transparent_unit, RecipientsFixture)
{
    // The actor in the transparent box: those in it, and the room around it with its other
    // transparent units (the box itself not twice)
    move("A", "T");
    std::string s = who("A", "", eTO_ROOM);
    std::sort(s.begin(), s.end());
    BOOST_TEST(s == "ABCD");

    // From inside the closed box only those in it
    move("A", "O");
    s = who("A", "", eTO_ROOM);
    std::sort(s.begin(), s.end());
    BOOST_TEST(s == "AE");
}

BOOST_FIXTURE_TEST_CASE(nowhere, RecipientsFixture)
{
    BOOST_TEST(who("R", "", eTO_ROOM) == ""); // a room in nothing reaches nobody
}

BOOST_AUTO_TEST_SUITE_END()

