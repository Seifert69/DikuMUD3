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

BOOST_AUTO_TEST_SUITE_END()
