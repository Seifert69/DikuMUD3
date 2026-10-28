#include "textutil.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>

#include <boost/test/unit_test.hpp>

namespace
{
constexpr size_t GUARD = 16;

char guard_byte(size_t i)
{
    // Not one repeated byte: a move that copies the guard over itself would go unseen
    return static_cast<char>('A' + i % 23);
}

/// Runs str_substitute twice and returns the result.
/// First on a heap buffer exactly as big as the input or the expected result, whichever is longer,
/// so AddressSanitizer sees any read or write past it. Then on one with GUARD more bytes that hold a
/// pattern, which must be unchanged afterwards: this needs no sanitizer, but it only catches a move
/// that writes past the end (the growing branch); a read past the end is for AddressSanitizer.
std::string substituted(const char *old, const char *newstr, const std::string &input, const std::string &expected)
{
    const size_t size = std::max(input.size(), expected.size()) + 1;

    std::unique_ptr<char[]> exact(new char[size]);
    memcpy(exact.get(), input.c_str(), input.size() + 1);
    str_substitute(old, newstr, exact.get());

    std::unique_ptr<char[]> guarded(new char[size + GUARD]);
    for (size_t i = size; i < size + GUARD; i++)
    {
        guarded[i] = guard_byte(i);
    }
    memcpy(guarded.get(), input.c_str(), input.size() + 1);
    str_substitute(old, newstr, guarded.get());

    for (size_t i = size; i < size + GUARD; i++)
    {
        BOOST_TEST_CONTEXT("guard byte " << i - size << " after " << input)
        {
            BOOST_TEST(guarded[i] == guard_byte(i));
        }
    }
    BOOST_TEST(std::string(exact.get()) == std::string(guarded.get()));
    return exact.get();
}
} // namespace

/**********************************************************************************
 * Textutil_Substitute_Suite - the char * str_substitute. The growing branch used to
 * move olen - 1 bytes too many: a read and a write past the end of the string. Every case
 * runs on an exact-size buffer (AddressSanitizer) and on one with guard bytes after it
 * (any build), see substituted(). An empty old text must change nothing (it used to loop
 * for ever), and the search must restart after each replacement, as it always has.
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
    // Matches that don't touch, so each is found whatever the search does after a replacement
    BOOST_TEST(substituted(" id ", " i would ", " id . id . id ", " i would . i would . i would ") == " i would . i would . i would ");
}

BOOST_AUTO_TEST_CASE(grow_by_one_and_old_one_byte_long)
{
    BOOST_TEST(substituted("a", "bb", "xax", "xbbx") == "xbbx");
    BOOST_TEST(substituted("abc", "wxyz", "..abc", "..wxyz") == "..wxyz");
}

BOOST_AUTO_TEST_CASE(search_restarts_after_each_replacement)
{
    // Kept as it is: the search starts again from the beginning, so neighbouring matches share their
    // spaces and the oracle rewrites every word of " but but " and " id id id "
    BOOST_TEST(substituted(" id ", " i would ", " id id id ", " i would i would i would ") == " i would i would i would ");
    BOOST_TEST(substituted(" but ", " , ", " but but ", " , , ") == " , , ");
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

BOOST_AUTO_TEST_CASE(empty_old_changes_nothing)
{
    // Zero-filled like the oracle's own_name: str_str reads the byte after an empty text, so a
    // string literal "" can end up not looping while a zeroed array does
    char empty[16] = {};

    BOOST_TEST(substituted(empty, "x", "abc", "abc") == "abc");
    BOOST_TEST(substituted(empty, "", "", "") == "");
}

BOOST_AUTO_TEST_SUITE_END()
