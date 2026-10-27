#include "textutil.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>

#include <boost/test/unit_test.hpp>

namespace
{
/// Runs str_substitute on a heap buffer that is exactly as big as the expected result,
/// so AddressSanitizer sees any read or write past it
std::string substituted(const char *old, const char *newstr, const std::string &input, const std::string &expected)
{
    const size_t size = std::max(input.size(), expected.size()) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    memcpy(buf.get(), input.c_str(), input.size() + 1);
    str_substitute(old, newstr, buf.get());
    return buf.get();
}
} // namespace

/**********************************************************************************
 * Textutil_Substitute_Suite - the char * str_substitute. The growing branch used to
 * move olen - 1 bytes too many: a read and a write past the end of the string, which
 * only shows under AddressSanitizer (the string itself comes out right).
 */
BOOST_AUTO_TEST_SUITE(Textutil_Substitute_Suite)

BOOST_AUTO_TEST_CASE(grow_match_at_start_middle_and_end)
{
    BOOST_TEST(substituted("ab", "xyzzy", "ab tail", "xyzzy tail") == "xyzzy tail");
    BOOST_TEST(substituted("ab", "xyzzy", "head ab tail", "head xyzzy tail") == "head xyzzy tail");
    BOOST_TEST(substituted("ab", "xyzzy", "head ab", "head xyzzy") == "head xyzzy");
    BOOST_TEST(substituted("ab", "xyzzy", "ab", "xyzzy") == "xyzzy");
}

BOOST_AUTO_TEST_CASE(grow_tail_lengths_around_the_match_length)
{
    // The tail after the match is 0, 1, olen - 1, olen and olen + 1 bytes long
    const std::string old = "abcd";
    const std::string neu = "0123456789";

    for (size_t tail = 0; tail <= old.size() + 1; tail++)
    {
        const std::string rest(tail, 't');
        BOOST_TEST_CONTEXT("tail " << tail)
        {
            BOOST_TEST(substituted(old.c_str(), neu.c_str(), "x" + old + rest, "x" + neu + rest) == "x" + neu + rest);
        }
    }
}

BOOST_AUTO_TEST_CASE(grow_every_match)
{
    BOOST_TEST(substituted(" id ", " i would ", " id id id ", " i would i would i would ") == " i would i would i would ");
}

BOOST_AUTO_TEST_CASE(grow_by_one_and_old_one_byte_long)
{
    BOOST_TEST(substituted("a", "bb", "xax", "xbbx") == "xbbx");
    BOOST_TEST(substituted("abc", "wxyz", "..abc", "..wxyz") == "..wxyz");
}

BOOST_AUTO_TEST_CASE(grow_leaves_the_bytes_after_the_buffer_alone)
{
    // Does not need AddressSanitizer: the bytes just after the exact-size buffer must keep their value
    const std::string old = "abcd";
    const std::string neu = "0123456789";
    const std::string input = "x" + old + "tt";
    const std::string expected = "x" + neu + "tt";
    constexpr size_t guard = 16;

    std::unique_ptr<char[]> mem(new char[expected.size() + 1 + guard]);
    // Not one repeated byte: a move that copies the guard over itself would go unseen
    for (size_t i = 0; i < expected.size() + 1 + guard; i++)
    {
        mem[i] = static_cast<char>('A' + i % 23);
    }
    memcpy(mem.get(), input.c_str(), input.size() + 1);

    str_substitute(old.c_str(), neu.c_str(), mem.get());

    BOOST_TEST(std::string(mem.get()) == expected);
    for (size_t i = expected.size() + 1; i < expected.size() + 1 + guard; i++)
    {
        BOOST_TEST_CONTEXT("byte " << i)
        {
            BOOST_TEST(mem[i] == static_cast<char>('A' + i % 23));
        }
    }
}

BOOST_AUTO_TEST_CASE(same_length_and_shrink)
{
    BOOST_TEST(substituted("ab", "xy", "ab ab", "xy xy") == "xy xy");
    BOOST_TEST(substituted("abcde", "x", "ab abcde abcde", "ab x x") == "ab x x");
    BOOST_TEST(substituted("abcde", "", "abcde", "") == "");
    BOOST_TEST(substituted("abcde", nullptr, "1abcde2", "12") == "12");
}

BOOST_AUTO_TEST_CASE(no_match_and_null_arguments)
{
    BOOST_TEST(substituted("zz", "x", "abc", "abc") == "abc");

    char buf[] = "abc";
    str_substitute(nullptr, "x", buf);
    BOOST_TEST(std::string(buf) == "abc");
    str_substitute("a", "x", nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
