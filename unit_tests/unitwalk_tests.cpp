#include "FixtureBase.h"
#include "handler.h"
#include "unit_data.h"
#include "unitfind.h"
#include "unitwalk.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * UnitWalk_Suite - walk_contents() and walk_surroundings(): the units around a unit, as act(),
 * the command scan and unit finding see them. A room R holds A, B, a transparent box T (with C
 * inside) and a closed box O (with D inside); the world W around it holds R itself when R is
 * transparent, and E and a transparent box U (with F inside).
 */
BOOST_AUTO_TEST_SUITE(UnitWalk_Suite)

struct WalkFixture : public unit_tests::FixtureBase
{
    std::map<std::string, std::unique_ptr<unit_data>> u;
    std::vector<std::string> linked; // in the order linked, unlinked backwards

    WalkFixture()
    {
        make("W", UNIT_ST_ROOM);
        make("R", UNIT_ST_OBJ);
        for (const char *c : {"T", "O", "U"})
        {
            make(c, UNIT_ST_OBJ);
        }
        for (const char *c : {"A", "B", "C", "D", "E", "F"})
        {
            make(c, UNIT_ST_NPC);
        }
        u["T"]->setUnitFlag(UNIT_FL_TRANS);
        u["U"]->setUnitFlag(UNIT_FL_TRANS);
        // contents are in reverse order of linking: a unit goes first in its container's list
        put("O", "R");
        put("T", "R");
        put("B", "R");
        put("A", "R");
        put("C", "T");
        put("D", "O");
        put("U", "W");
        put("E", "W");
        put("R", "W");
        put("F", "U");
    }

    ~WalkFixture() override
    {
        for (auto it = linked.rbegin(); it != linked.rend(); ++it)
        {
            if (u[*it]->getUnitIn())
            {
                unit_from_unit(u[*it].get());
            }
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

    static std::string name(const unit_data *x) { return x->getNames().Name(); }
};

BOOST_FIXTURE_TEST_CASE(contents_in_order_and_inside_transparent_units, WalkFixture)
{
    std::string s;
    walk_contents(u["R"].get(), nullptr, [&](unit_data *x, unit_data *inside) {
        s += inside ? "(" + name(x) + ")" : name(x);
        return eWalk::on;
    });
    BOOST_TEST(s == "ABT(C)O"); // not inside the closed box
}

BOOST_FIXTURE_TEST_CASE(skip_and_stop, WalkFixture)
{
    std::string s;
    walk_contents(u["R"].get(), u["T"].get(), [&](unit_data *x, unit_data *) {
        s += name(x);
        return eWalk::on;
    });
    BOOST_TEST(s == "ABTO"); // skip: not inside T

    s = "";
    walk_contents(u["R"].get(), nullptr, [&](unit_data *x, unit_data *) {
        s += name(x);
        return (x == u["T"].get()) ? eWalk::skip : eWalk::on;
    });
    BOOST_TEST(s == "ABTO"); // the visit said skip

    s = "";
    unit_data *at = walk_contents(u["R"].get(), nullptr, [&](unit_data *x, unit_data *) {
        s += name(x);
        return (x == u["C"].get()) ? eWalk::stop : eWalk::on;
    });
    BOOST_TEST(s == "ABTC");
    BOOST_TEST(at == u["C"].get());
}

BOOST_FIXTURE_TEST_CASE(surroundings_from_a_closed_room_and_from_a_transparent_one, WalkFixture)
{
    std::string s;
    auto walk = [&](const char *from) {
        s = "";
        return walk_surroundings(u[from].get(), [&](unit_data *x, unit_data *inside, bool outer) {
            s += std::string(outer ? "^" : "") + (inside ? "(" + name(x) + ")" : name(x));
            return eWalk::on;
        });
    };

    BOOST_TEST(walk("A") == nullptr);
    BOOST_TEST(s == "ABT(C)O"); // R isn't transparent: nothing around it

    u["R"]->setUnitFlag(UNIT_FL_TRANS);
    walk("A");
    BOOST_TEST(s == "ABT(C)O^R^E^U^(F)"); // R open now: the world around it too, not inside R again

    BOOST_TEST(walk("W") == nullptr); // in nothing: no surroundings
    BOOST_TEST(s == "");
}

BOOST_FIXTURE_TEST_CASE(a_visit_may_move_the_unit_it_is_at, WalkFixture)
{
    std::string s;
    walk_contents(u["R"].get(), nullptr, [&](unit_data *x, unit_data *) {
        s += name(x);
        if (x == u["A"].get())
        {
            unit_from_unit(x); // A leaves the room: the walk goes on with B
            unit_to_unit(x, u["W"].get());
        }
        return eWalk::on;
    });
    BOOST_TEST(s == "ABTCO");
}

BOOST_FIXTURE_TEST_CASE(skip_in_the_room_around, WalkFixture)
{
    u["R"]->setUnitFlag(UNIT_FL_TRANS);
    std::string s;
    walk_surroundings(u["A"].get(), [&](unit_data *x, unit_data *inside, bool outer) {
        s += inside ? "(" + name(x) + ")" : name(x);
        return (outer && (x == u["U"].get())) ? eWalk::skip : eWalk::on;
    });
    BOOST_TEST(s == "ABT(C)ORE" "U"); // not inside U, which said skip
}

// scan4_unit, which DIL's foreach and the justice code use: the units around ch, not ch itself,
// of the types asked for, in walk order
BOOST_FIXTURE_TEST_CASE(scan4_unit_lists_the_surroundings, WalkFixture)
{
    auto listed = [&]() {
        std::string s;
        for (int i = 0; i < UVITOP; i++)
        {
            s += name(UVI(i));
        }
        return s;
    };

    scan4_unit(u["A"].get(), UNIT_ST_NPC | UNIT_ST_OBJ);
    BOOST_TEST(listed() == "BTCO");
    scan4_unit(u["A"].get(), UNIT_ST_NPC);
    BOOST_TEST(listed() == "BC");

    u["R"]->setUnitFlag(UNIT_FL_TRANS);
    scan4_unit(u["A"].get(), UNIT_ST_NPC | UNIT_ST_OBJ);
    BOOST_TEST(listed() == "BTCORE" "UF"); // the room around too, R itself listed, not inside it again

    scan4_unit(u["C"].get(), UNIT_ST_NPC); // from inside the box T
    BOOST_TEST(listed() == "AB");
}

BOOST_AUTO_TEST_SUITE_END()
