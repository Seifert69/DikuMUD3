#define BOOST_TEST_MODULE "VMC Unit Tests"
#ifndef DIKU_STATIC
    #define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MAIN
#include "ChecksumDataset.h"
#include "sha512.h"

#include <boost/format.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(VMC_tests)

BOOST_AUTO_TEST_CASE(Heading)
{
    BOOST_TEST_MESSAGE("VMC Unit Tests");
}

std::vector<std::string> config_files{"def_file_hashes.json", "zon_file_hashes.json"};

// Sharing the code with decomp_main.cpp
#include "checksum_test_case.tcc"

BOOST_AUTO_TEST_SUITE_END()
