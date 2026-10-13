#include "bytestring.h"
#include "intlist.h"

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * cintlist_tests
 *
 * ReadBuffer() reads a count from the file and then that many values. A corrupt
 * count (up to 2^31) made it append, and realloc, that many times after the data
 * had run out, which hung the server.
 */
BOOST_AUTO_TEST_SUITE(cintlist_tests)

BOOST_AUTO_TEST_CASE(reads_a_valid_list)
{
    CByteBuffer buf;
    buf.Append32(3);
    buf.Append32(7);
    buf.Append32(static_cast<ubit32>(-8));
    buf.Append32(9);

    cintlist l;
    BOOST_TEST(l.ReadBuffer(&buf) == 0);
    BOOST_TEST(l.Length() == 3);
    BOOST_TEST(l.Value(0) == 7);
    BOOST_TEST(l.Value(1) == -8);
    BOOST_TEST(l.Value(2) == 9);
}

BOOST_AUTO_TEST_CASE(huge_count_stops_at_the_end_of_the_data)
{
    CByteBuffer buf;
    buf.Append32(1000000000);
    buf.Append32(7);
    buf.Append32(8);

    cintlist l;
    BOOST_TEST(l.ReadBuffer(&buf) == 1);
    BOOST_TEST(l.Length() == 2); // the two values that were there, nothing more
}

BOOST_AUTO_TEST_CASE(negative_count_is_corrupt)
{
    CByteBuffer buf;
    buf.Append32(static_cast<ubit32>(-5));

    cintlist l;
    BOOST_TEST(l.ReadBuffer(&buf) == 1);
    BOOST_TEST(l.Length() == 0);
}

BOOST_AUTO_TEST_CASE(missing_count_is_corrupt)
{
    CByteBuffer buf;
    cintlist l;
    BOOST_TEST(l.ReadBuffer(&buf) == 1);
    BOOST_TEST(l.Length() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
