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

BOOST_AUTO_TEST_CASE(boundary_forgets)
{
    // after a label or block end the code may come from elsewhere: no warning
    Fixture f;
    f.check.assign(0, -1);
    f.check.wait(true, 10);
    f.check.boundary();
    f.check.use(0, true, 12);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(boundary_after_unsecure_is_maybe_secured)
{
    // secured above, unsecured on one path: after a label it may still be secured
    Fixture f;
    f.check.assign(0, -1);
    f.check.secure(0, 8);
    f.check.unsecure(0);
    f.check.boundary();
    f.check.wait(true, 10);
    f.check.use(0, true, 11);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(never_secured_stays_unsecured_after_boundary)
{
    Fixture f;
    f.check.assign(0, -1);
    f.check.boundary();
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
    f.check.boundary(true);
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
    f.check.boundary(true);
    f.check.use(DilCheck::ACTIVATOR, true, 121);
    BOOST_TEST(f.all().empty());
}

BOOST_AUTO_TEST_CASE(label_may_be_reached_after_the_variable_is_set)
{
    Fixture f;
    f.check.boundary(true);
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

BOOST_AUTO_TEST_SUITE_END()
