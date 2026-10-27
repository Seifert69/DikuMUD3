#include "intlist.h"

#include <climits>
#include <initializer_list>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(cintlist_tests)

BOOST_AUTO_TEST_CASE(remove_shifts_the_values_after_idx)
{
    // Remove() moved one value too many, reading past the end of the list.
    // The values were still right, so only a memory checker such as
    // AddressSanitizer shows that bug.
    cintlist l;
    for (int v : {1, 2, 3, 4, 5})
    {
        l.Append(v);
    }

    // One value after the removed one: the smallest shift
    l.Remove(3);
    BOOST_TEST(l.Length() == 4);
    BOOST_TEST(l.Value(0) == 1);
    BOOST_TEST(l.Value(1) == 2);
    BOOST_TEST(l.Value(2) == 3);
    BOOST_TEST(l.Value(3) == 5);

    l.Remove(1);
    BOOST_TEST(l.Length() == 3);
    BOOST_TEST(l.Value(0) == 1);
    BOOST_TEST(l.Value(1) == 3);
    BOOST_TEST(l.Value(2) == 5);

    l.Remove(0);
    BOOST_TEST(l.Length() == 2);
    BOOST_TEST(l.Value(0) == 3);
    BOOST_TEST(l.Value(1) == 5);

    l.Remove(1);
    BOOST_TEST(l.Length() == 1);
    BOOST_TEST(l.Value(0) == 3);

    l.Remove(0);
    BOOST_TEST(l.Length() == 0);

    // The list can be used again after Remove() has emptied it
    l.Append(5);
    BOOST_TEST(l.Length() == 1);
    BOOST_TEST(l.Value(0) == 5);
}

BOOST_AUTO_TEST_CASE(remove_out_of_range_changes_nothing)
{
    cintlist l;
    l.Append(7);
    l.Append(8);

    // Not changed by the fix: DIL passes any 32-bit index straight through
    // (dilfi_remlst), so the bounds check is worth a test of its own
    for (int idx : {-1, 2, 3, INT_MAX, INT_MIN})
    {
        l.Remove(idx);
    }
    BOOST_TEST(l.Length() == 2);
    BOOST_TEST(l.Value(0) == 7);
    BOOST_TEST(l.Value(1) == 8);

    cintlist empty;
    empty.Remove(0);
    BOOST_TEST(empty.Length() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
