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
    bool warned(const std::string &part) const
    {
        for (const auto &w : warnings)
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
    f.check.assign(0);
    f.check.wait(true, 10);
    f.check.use(0, true, 11);
    BOOST_TEST(f.warnings.size() == 1);
    BOOST_TEST(f.warned("11: pc is null here: it was set before the wait on line 10"));
}

BOOST_AUTO_TEST_CASE(secured_unit_after_wait)
{
    Fixture f;
    f.check.assign(0);
    f.check.secure(0, 9);
    f.check.wait(true, 10);
    f.check.use(0, true, 11);
    BOOST_TEST(f.warnings.empty());
}

BOOST_AUTO_TEST_CASE(unsecure_then_wait)
{
    Fixture f;
    f.check.assign(0);
    f.check.secure(0, 9);
    f.check.unsecure(0);
    f.check.wait(false, 10);
    f.check.use(0, false, 11);
    BOOST_TEST(f.warned("pc is null here"));
}

BOOST_AUTO_TEST_CASE(reassigned_after_wait)
{
    Fixture f;
    f.check.assign(0);
    f.check.wait(false, 10);
    f.check.assign(0);
    f.check.use(0, true, 12);
    BOOST_TEST(f.warnings.empty());
}

BOOST_AUTO_TEST_CASE(extraptr_cleared_even_when_secured_elsewhere)
{
    Fixture f;
    f.check.assign(1);
    f.check.wait(false, 10);
    f.check.use(1, true, 11);
    BOOST_TEST(f.warned("exd is null here"));
}

BOOST_AUTO_TEST_CASE(other_types_survive_waits)
{
    Fixture f;
    f.check.assign(2);
    f.check.wait(true, 10);
    f.check.use(2, false, 11);
    BOOST_TEST(f.warnings.empty());
}

BOOST_AUTO_TEST_CASE(one_warning_per_variable)
{
    Fixture f;
    f.check.assign(0);
    f.check.wait(true, 10);
    f.check.use(0, true, 11);
    f.check.use(0, true, 12);
    BOOST_TEST(f.warnings.size() == 1);
}

BOOST_AUTO_TEST_CASE(boundary_forgets)
{
    // after a label or block end the code may come from elsewhere: no warning
    Fixture f;
    f.check.assign(0);
    f.check.wait(true, 10);
    f.check.boundary();
    f.check.use(0, true, 12);
    BOOST_TEST(f.warnings.empty());
}

BOOST_AUTO_TEST_CASE(boundary_after_unsecure_is_maybe_secured)
{
    // secured above, unsecured on one path: after a label it may still be secured
    Fixture f;
    f.check.assign(0);
    f.check.secure(0, 8);
    f.check.unsecure(0);
    f.check.boundary();
    f.check.wait(true, 10);
    f.check.use(0, true, 11);
    BOOST_TEST(f.warnings.empty());
}

BOOST_AUTO_TEST_CASE(never_secured_stays_unsecured_after_boundary)
{
    Fixture f;
    f.check.assign(0);
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
    BOOST_TEST(f.warnings.empty());
}

BOOST_AUTO_TEST_CASE(unsecured_unit_in_condition)
{
    Fixture f;
    f.check.assign(0);
    f.check.setCondition(true);
    f.check.use(DilCheck::ACTIVATOR, false, 30);
    f.check.use(0, false, 30);
    f.check.setCondition(false);
    BOOST_TEST(f.warnings.size() == 1);
    BOOST_TEST(f.warned("30: pc is null in this condition"));
}

BOOST_AUTO_TEST_CASE(secured_unit_in_condition)
{
    Fixture f;
    f.check.assign(0);
    f.check.secure(0, 29);
    f.check.setCondition(true);
    f.check.use(0, false, 30);
    f.check.setCondition(false);
    BOOST_TEST(f.warnings.empty());
}

BOOST_AUTO_TEST_CASE(used_before_set)
{
    Fixture f;
    f.check.use(0, false, 40); // comparing with null is fine
    f.check.use(0, true, 41);
    BOOST_TEST(f.warnings.size() == 1);
    BOOST_TEST(f.warned("41: pc is used before it is set"));
}

BOOST_AUTO_TEST_CASE(arguments_are_set)
{
    Fixture f(1);
    f.check.use(0, true, 41);
    BOOST_TEST(f.warnings.empty());
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
    f.check.assign(0);
    f.check.wait(true, 10);
    f.check.setQuiet(true);
    f.check.use(0, false, 11);
    f.check.setQuiet(false);
    BOOST_TEST(f.warnings.empty());
}

BOOST_AUTO_TEST_CASE(act_null_units)
{
    Fixture f;
    f.check.act(false, true, true, 60);  // TO_VICT, null victim
    f.check.act(true, false, false, 61); // TO_CHAR, null character
    f.check.act(true, false, true, 62);  // TO_VICT only needs the victim
    f.check.act(false, true, false, 63); // TO_NOTVICT: the room still sees it
    BOOST_TEST(f.warnings.size() == 2);
    BOOST_TEST(f.warned("60: act() TO_VICT with null as the victim"));
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
    BOOST_TEST(f.warnings.size() == 1);
    BOOST_TEST(f.warned("70: exec(\"kill\"+...) has no space"));
}

BOOST_AUTO_TEST_SUITE_END()
