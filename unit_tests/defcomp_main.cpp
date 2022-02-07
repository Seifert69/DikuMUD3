#define BOOST_TEST_MODULE "DEFCOMP Unit Tests"
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

BOOST_AUTO_TEST_SUITE(Defcomp_tests)

BOOST_AUTO_TEST_CASE(Heading)
{
    BOOST_TEST_MESSAGE("DEFCOMP Unit Tests");
}

std::vector<std::string> config_files{"color_file_hashes.json"};

// Sharing the code with vmc_main.cpp
#include "checksum_test_case.tcc"

BOOST_AUTO_TEST_SUITE_END()
