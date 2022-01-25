#define BOOST_TEST_MODULE "MPlex Unit Tests"
#ifndef DIKU_STATIC
    #define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(SampleTest)
{
    BOOST_TEST_MESSAGE("MPlex Unit Tests");
    BOOST_CHECK(true == true);
}
