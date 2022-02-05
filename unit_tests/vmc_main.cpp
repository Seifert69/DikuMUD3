#define BOOST_TEST_MODULE "VMC Unit Tests"
#ifndef DIKU_STATIC
    #define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MAIN
#include "checksum_dataset.h"
#include "sha512.h"

#include <boost/format.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(VMC_tests)

BOOST_AUTO_TEST_CASE(SampleTest)
{
    BOOST_TEST_MESSAGE("VMC Unit Tests");
    BOOST_CHECK(true == true);
}

BOOST_DATA_TEST_CASE(TestChecksums,
                     boost::unit_test::data::make_delayed<checksum_dataset>("../../unit_tests/vmc_checksum_tests.json"),
                     file_info)
{
    sha512 sha;

    ////////////////////////// Test Subject //////////////////////////////
    sha.generate(file_info.full_filename);
    ////////////////////////// Test Subject //////////////////////////////

    auto checksum = sha.getChecksumString();

    boost::format error_message(file_info.error_fmt_str);
    error_message % file_info.full_filename % file_info.sha512_checksum % checksum;

    BOOST_TEST(file_info.sha512_checksum == checksum, error_message.str());
}

BOOST_AUTO_TEST_SUITE_END()
