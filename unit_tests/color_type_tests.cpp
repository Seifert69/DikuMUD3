#define BOOST_TEST_MODULE "color_type Unit Tests"
#include <boost/test/unit_test.hpp>
#include <memory>
#include <string>
#include <cstring>

#include "color.h"

BOOST_AUTO_TEST_SUITE(color_type_tests)

BOOST_AUTO_TEST_CASE(default_ctor_test)
{
    color_type color;

    std::unique_ptr<char, void (*)(void *)> result{color.key_string(), free};
    BOOST_TEST(result.get() == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
