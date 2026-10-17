#include "FixtureBase.h"
#include "dil.h"
#include "dilrun.h"
#include "namelist.h"
#include "textutil.h"

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * dilrun_tests
 *
 * dil_free_var() returned before freeing the name of a global variable, so every
 * program with a global variable leaked its names when it was freed (with "when",
 * once per lookup of such a player).
 *
 * dil_bind_global_var() binds a variable to the shared value of a global, for
 * new programs, call frames and programs read from a file alike.
 */
BOOST_AUTO_TEST_SUITE(dilrun_tests)

BOOST_AUTO_TEST_CASE(dil_free_var_frees_a_global_variables_name_not_its_value)
{
    // The value is shared with the global index; the name is the variable's own copy
    cNamelist shared;
    dilvar v{};
    v.type = DilVarType_e::DILV_SLP;
    v.itype = DilIType_e::Global;
    v.name = str_dup("xmas_done@holiday22");
    v.val.namelist = &shared;

    dil_free_var(&v);

    BOOST_TEST(v.name == nullptr);
    BOOST_TEST(v.val.namelist == &shared); // left to the global index
}

BOOST_AUTO_TEST_CASE(dil_free_var_frees_a_regular_variables_name_and_value)
{
    dilvar v{};
    v.type = DilVarType_e::DILV_SLP;
    v.itype = DilIType_e::Regular;
    v.name = str_dup("list");
    v.val.namelist = new cNamelist;

    dil_free_var(&v);

    BOOST_TEST(v.name == nullptr);
    BOOST_TEST(v.val.namelist == nullptr);
}

BOOST_AUTO_TEST_CASE(dil_bind_global_var_shares_one_value)
{
    dilvar a{};
    dilvar b{};
    a.type = b.type = DilVarType_e::DILV_SLP;

    BOOST_TEST(dil_bind_global_var(&a, "shared@dilrun_tests"));
    BOOST_TEST(dil_bind_global_var(&b, "shared@dilrun_tests"));

    BOOST_TEST((a.itype == DilIType_e::Global && b.itype == DilIType_e::Global));
    BOOST_TEST(a.val.namelist != nullptr);
    BOOST_TEST(a.val.namelist == b.val.namelist);
}

// The global index has it as a stringlist: an intlist of the same name can't share it
BOOST_FIXTURE_TEST_CASE(dil_bind_global_var_type_conflict_leaves_it_invalid, unit_tests::FixtureBase)
{
    dilvar a{};
    a.type = DilVarType_e::DILV_SLP;
    BOOST_TEST(dil_bind_global_var(&a, "conflict@dilrun_tests"));

    dilvar b{};
    b.type = DilVarType_e::DILV_ILP;
    BOOST_TEST(!dil_bind_global_var(&b, "conflict@dilrun_tests"));

    BOOST_TEST((b.type == DilVarType_e::DILV_INVALID));
    BOOST_TEST((b.itype == DilIType_e::Regular));
    BOOST_TEST(b.val.string == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
