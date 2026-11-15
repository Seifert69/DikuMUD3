#include "dilcheck.h"

#include <boost/test/unit_test.hpp>

namespace
{
/// A DilCheck with variables pc (unitptr), exd (extraptr), s (string), and the warnings it gives.
struct Fixture
{
    std::vector<std::string> warnings;
    DilCheck check{[this](int line, const std::string &msg) { warnings.push_back(std::to_string(line) + ": " + msg); }};

    explicit Fixture(int argc = 0)
    {
        check.begin({{"pc", DilCheck::VarKind::Unit}, {"exd", DilCheck::VarKind::Extra}, {"s", DilCheck::VarKind::Other}}, argc);
    }
    explicit Fixture(const std::vector<DilCheck::Var> &vars) { check.begin(vars, 0); }
    /// The warnings, once the template is done.
    const std::vector<std::string> &all()
    {
        check.end();
        return warnings;
    }
    bool warned(const std::string &part)
    {
        for (const auto &w : all())
        {
            if (w.find(part) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    }
};
} // namespace

BOOST_AUTO_TEST_SUITE(DilCheck_tests)

BOOST_AUTO_TEST_CASE(unsecured_unit_after_wait)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.wait(true, 10);
    f.check.use(0, true, 11);
    BOOST_TEST(f.all().size() == 1);
    BOOST_TEST(f.warned("11: pc is null here: it was set before the wait on line 10"));
}

BOOST_AUTO_TEST_CASE(secured_unit_after_wait)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 9);
    f.check.wait(true, 10);
    f.check.use(0, true, 11);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(unsecure_then_wait)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 9);
    f.check.unsecure(0);
    f.check.wait(false, 10);
    f.check.use(0, false, 11);
    BOOST_TEST(f.warned("pc is null here"));
}

BOOST_AUTO_TEST_CASE(reassigned_after_wait)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.wait(false, 10);
    f.check.assign(0, -1);
    f.check.use(0, true, 12);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(extraptr_cleared_even_when_secured_elsewhere)
{
    Fixture f;
    f.check.assign(1, -1);
    f.check.wait(false, 10);
    f.check.use(1, true, 11);
    BOOST_TEST(f.warned("exd is null here"));
}

BOOST_AUTO_TEST_CASE(other_types_survive_waits)
{
    Fixture f;
    f.check.assign(2, -1);
    f.check.wait(true, 10);
    f.check.use(2, false, 11);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(one_warning_per_variable)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.wait(true, 10);
    f.check.use(0, true, 11);
    f.check.use(0, true, 12);
    BOOST_TEST(f.all().size() == 1);
}

BOOST_AUTO_TEST_CASE(label_forgets)
{
    // after a label or block end the code may come from elsewhere: no warning
    Fixture f;
    f.check.assign(0, -1);
    f.check.wait(true, 10);
    f.check.label();
    f.check.use(0, true, 12);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(label_after_unsecure_is_maybe_secured)
{
    // secured above, unsecured on one path: after a label it may still be secured
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 8);
    f.check.unsecure(0);
    f.check.label();
    f.check.wait(true, 10);
    f.check.use(0, true, 11);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(never_secured_stays_unsecured_after_label)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.label();
    f.check.wait(true, 10);
    f.check.use(0, false, 11);
    BOOST_TEST(f.warned("pc is null here"));
}

BOOST_AUTO_TEST_CASE(event_variables_after_pause)
{
    Fixture f;
    f.check.wait(true, 20);
    f.check.use(DilCheck::ACTIVATOR, true, 21);
    f.check.use(DilCheck::ARGUMENT, false, 22);
    BOOST_TEST(f.warned("21: activator is null here: after the pause on line 20"));
    BOOST_TEST(f.warned("22: argument is empty here"));
}

BOOST_AUTO_TEST_CASE(event_variables_after_event_wait)
{
    Fixture f;
    f.check.wait(true, 20);
    f.check.wait(false, 21);
    f.check.use(DilCheck::ACTIVATOR, true, 22);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(unsecured_unit_in_condition)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.setCondition(true);
    f.check.use(DilCheck::ACTIVATOR, false, 30);
    f.check.use(0, false, 30);
    f.check.setCondition(false);
    f.check.wait(false, 30);
    BOOST_TEST(f.all().size() == 1);
    BOOST_TEST(f.warned("30: pc is null in this condition"));
}

BOOST_AUTO_TEST_CASE(secured_unit_in_condition)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 29);
    f.check.setCondition(true);
    f.check.use(0, false, 30);
    f.check.setCondition(false);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(used_before_set)
{
    Fixture f;
    f.check.use(0, false, 40); // comparing with null is fine
    f.check.use(0, true, 41);
    BOOST_TEST(f.all().size() == 1);
    BOOST_TEST(f.warned("41: pc is used before it is set"));
}

BOOST_AUTO_TEST_CASE(arguments_are_set)
{
    Fixture f(1);
    f.check.use(0, true, 41);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(secure_before_set)
{
    Fixture f;
    f.check.secure(0, 50);
    BOOST_TEST(f.warned("50: secure(pc) comes before pc is set"));
}

BOOST_AUTO_TEST_CASE(quiet_unsecure_argument)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.wait(true, 10);
    f.check.setQuiet(true);
    f.check.use(0, false, 11);
    f.check.setQuiet(false);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(act_null_units)
{
    Fixture f;
    f.check.act(false, true, true, true, 60);  // TO_VICT or TO_NOTVICT, null victim
    f.check.act(true, false, true, false, 61); // TO_CHAR, null character
    f.check.act(true, false, true, true, 62);  // TO_VICT only needs the victim
    f.check.act(true, true, false, false, 63); // the type is a variable: can't tell
    f.check.end();
    BOOST_TEST(f.all().size() == 2);
    BOOST_TEST(f.warned("60: act() TO_VICT/TO_NOTVICT with null as the victim"));
    BOOST_TEST(f.warned("61: act() with null as the character"));
}

BOOST_AUTO_TEST_CASE(exec_without_space)
{
    Fixture f;
    f.check.exec("kill", true, 70);
    f.check.exec("kill ", true, 71);
    f.check.exec("smile", false, 72);
    f.check.exec("say ", true, 73);
    f.check.exec("'", true, 74);
    BOOST_TEST(f.all().size() == 1);
    BOOST_TEST(f.warned("70: exec(\"kill\"+...) has no space"));
}

BOOST_AUTO_TEST_CASE(interrupt_condition_checked_at_the_wait)
{
    // on_activation(pc ...) then secure(pc): pc is secured whenever the condition is tested
    Fixture f;
    f.check.assign(0, -1);
    f.check.setCondition(true);
    f.check.use(0, true, 80);
    f.check.setCondition(false);
    f.check.secure(0, 81);
    f.check.wait(false, 82);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(condition_on_loop_back_secured_further_down)
{
    // :start: wait(..., activator == pc); pc := activator; secure(pc, lost); goto start;
    Fixture f;
    f.check.label();
    f.check.setCondition(true);
    f.check.use(0, false, 90);
    f.check.setCondition(false);
    f.check.wait(false, 90);
    f.check.assign(0, -1);
    f.check.secure(0, 92);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(copy_of_secured_variable_survives)
{
    Fixture f({{"u", DilCheck::VarKind::Unit}, {"v", DilCheck::VarKind::Unit}});
    f.check.assign(0, -1);
    f.check.secure(0, 100);
    f.check.assign(1, 0);
    f.check.wait(true, 101);
    f.check.use(1, true, 102);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(reassigned_secured_variable_is_cleared)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 110);
    f.check.assign(0, -1);
    f.check.wait(true, 111);
    f.check.use(0, true, 112);
    BOOST_TEST(f.warned("112: pc is null here"));
}

BOOST_AUTO_TEST_CASE(loop_start_forgets_the_pause)
{
    // pause; while (activator ...) { wait(SFB_CMD, TRUE); }
    Fixture f;
    f.check.wait(true, 120);
    f.check.label();
    f.check.use(DilCheck::ACTIVATOR, true, 121);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(label_may_be_reached_after_the_variable_is_set)
{
    Fixture f;
    f.check.label();
    f.check.use(0, true, 130);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(secure_of_cleared_variable)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.wait(true, 140);
    f.check.secure(0, 141);
    BOOST_TEST(f.warned("141: secure(pc) secures nothing: pc was cleared by the wait on line 140"));
}

BOOST_AUTO_TEST_CASE(else_starts_from_the_state_before_the_if)
{
    // if (...) { ...; unsecure(pc); } else { pause; pc.name }: pc is secured in the else
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 150);
    f.check.ifBegin();
    f.check.unsecure(0);
    f.check.elseBegin();
    f.check.wait(true, 152);
    f.check.use(0, true, 153);
    f.check.ifEnd();
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(conditional_pause_is_not_certain)
{
    // u := ...; if (busy) pause; u.name
    Fixture f;
    f.check.assign(0, -1);
    f.check.ifBegin();
    f.check.wait(true, 160);
    f.check.ifEnd();
    f.check.use(0, true, 161);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(branch_that_jumps_away_does_not_reach_below)
{
    // u := ...; if (c) { pause; goto x; } pause; u.name: the branch doesn't reach below, u is still set
    Fixture f;
    f.check.assign(0, -1);
    f.check.ifBegin();
    f.check.wait(true, 170);
    f.check.jump();
    f.check.ifEnd();
    f.check.wait(true, 172);
    f.check.use(0, true, 173);
    BOOST_TEST(f.warned("173: pc is null here: it was set before the wait on line 172"));
}

BOOST_AUTO_TEST_CASE(secured_further_down_drops_a_vague_warning)
{
    // :loop: (pc set somewhere above or below) pause; pc.name ... secure(pc, x) further down
    Fixture f;
    f.check.label();
    f.check.wait(true, 180);
    f.check.use(0, true, 181);
    f.check.assign(0, -1);
    f.check.secure(0, 183);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(reassigned_from_activator_stays_secured)
{
    // wait(..., activator == pc); pc := activator;
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 190);
    f.check.setCondition(true, true);
    f.check.use(0, false, 191);
    f.check.equal(DilCheck::ACTIVATOR, 0);
    f.check.setCondition(false);
    f.check.wait(false, 191);
    f.check.assign(0, DilCheck::ACTIVATOR);
    f.check.wait(true, 192);
    f.check.use(0, true, 193);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(reassigned_from_activator_without_condition_is_cleared)
{
    // pc secured, then pc := activator after a wait that didn't look at pc: a new, unsecured unit
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 200);
    f.check.wait(false, 201);
    f.check.assign(0, DilCheck::ACTIVATOR);
    f.check.wait(true, 202);
    f.check.use(0, true, 203);
    BOOST_TEST(f.warned("203: pc is null here"));
}

BOOST_AUTO_TEST_CASE(code_after_a_jump_is_not_checked)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.wait(true, 210);
    f.check.jump();
    f.check.use(0, true, 211);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(unsecure_then_jump_in_a_branch)
{
    // secure(pc); if (c) { unsecure(pc); goto start; } pause; pc.name
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 220);
    f.check.ifBegin();
    f.check.unsecure(0);
    f.check.jump();
    f.check.ifEnd();
    f.check.wait(true, 223);
    f.check.use(0, true, 224);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(activator_read_without_equality_does_not_keep_secured)
{
    // wait(SFB_CMD, activator != pc); pc := activator: a different unit
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 230);
    f.check.setCondition(true, true);
    f.check.use(0, false, 231);
    f.check.setCondition(false);
    f.check.wait(false, 231);
    f.check.assign(0, DilCheck::ACTIVATOR);
    f.check.wait(true, 233);
    f.check.use(0, true, 234);
    BOOST_TEST(f.warned("234: pc is null here"));
}

BOOST_AUTO_TEST_CASE(set_on_both_ways_then_wait_is_definite)
{
    // pc := ...; if (c) { pause; } pause; pc.name (with a secure(pc) elsewhere further down)
    Fixture f;
    f.check.assign(0, -1);
    f.check.ifBegin();
    f.check.wait(true, 240);
    f.check.ifEnd();
    f.check.wait(true, 242);
    f.check.use(0, true, 243);
    f.check.label();
    f.check.assign(0, -1);
    f.check.secure(0, 245);
    BOOST_TEST(f.warned("243: pc is null here"));
}

BOOST_AUTO_TEST_CASE(every_way_paused)
{
    // if (x) { pause; } else { pause; } activator.name
    Fixture f;
    f.check.ifBegin();
    f.check.wait(true, 250);
    f.check.elseBegin();
    f.check.wait(true, 252);
    f.check.ifEnd();
    f.check.use(DilCheck::ACTIVATOR, true, 254);
    BOOST_TEST(f.warned("254: activator is null here"));
}

BOOST_AUTO_TEST_CASE(nested_if_in_else)
{
    // secure(pc); if (a) { unsecure(pc); } else if (b) { pause; pc.name }
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 260);
    f.check.ifBegin();
    f.check.unsecure(0);
    f.check.elseBegin();
    f.check.ifBegin();
    f.check.wait(true, 263);
    f.check.use(0, true, 264);
    f.check.ifEnd();
    f.check.ifEnd();
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(loop_in_dead_code_is_not_checked)
{
    // goto done; while (c) { pause; pc.name }
    Fixture f;
    f.check.assign(0, -1);
    f.check.jump();
    f.check.loopBegin();
    f.check.wait(true, 270);
    f.check.use(0, true, 271);
    f.check.loopEnd();
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_SUITE_END()
