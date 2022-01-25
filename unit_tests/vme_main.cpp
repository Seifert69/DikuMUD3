#define BOOST_TEST_MODULE "VME Unit Tests"
#ifndef DIKU_STATIC
    #define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(SampleTest)
{
    BOOST_TEST_MESSAGE("VMC Unit Tests");
    BOOST_CHECK(true == true);
}
