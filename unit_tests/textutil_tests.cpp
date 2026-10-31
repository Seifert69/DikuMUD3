#include "textutil.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

namespace
{
constexpr size_t GUARD = 16;
constexpr size_t ROOMY = 4 * 1024 * 1024;

char guard_byte(size_t i)
{
    // Not one repeated byte: a move that copies the guard over itself would go unseen
    return static_cast<char>('A' + i % 23);
}

/// Runs str_substitute up to three times and returns the result, stopping at the first run that is wrong.
/// First on a roomy buffer: a result other than expected is reported and returned here, because the next
/// two runs have no room for it. The bytes after the input hold '#', so a result that lost its NUL is seen.
/// A loop that rescans from the start grows the string slowly and runs into the suite's timeout first; one
/// that does not (an empty old text matching at the NUL) fills the buffer in milliseconds, and Boost reports
/// the memory access violation against the case. One buffer is reused, since a timeout leaves the call by a
/// signal and would leak a local one.
/// Then on a heap buffer with GUARD more bytes that hold a pattern, which must be unchanged afterwards: this
/// needs no sanitizer, but it only catches a move that writes past the end (the growing branch).
/// Last on a heap buffer exactly as big as the input or the expected result, whichever is longer, so
/// AddressSanitizer sees any read or write past it.
std::string substituted(const char *old, const char *newstr, const std::string &input, const std::string &expected)
{
    static std::vector<char> roomy(ROOMY);
    memset(roomy.data(), '#', input.size() + expected.size() + GUARD);
    memcpy(roomy.data(), input.c_str(), input.size() + 1);
    str_substitute(old, newstr, roomy.data());
    if (std::string(roomy.data()) != expected)
    {
        BOOST_TEST_CONTEXT("old '" << old << "' new '" << (newstr ? newstr : "(null)") << "' in '" << input << "'")
        {
            BOOST_TEST(std::string(roomy.data()) == expected);
        }
        return roomy.data();
    }

    const size_t size = std::max(input.size(), expected.size()) + 1;

    std::unique_ptr<char[]> guarded(new char[size + GUARD]);
    for (size_t i = size; i < size + GUARD; i++)
    {
        guarded[i] = guard_byte(i);
    }
    memcpy(guarded.get(), input.c_str(), input.size() + 1);
    str_substitute(old, newstr, guarded.get());

    bool guard_intact = true;
    for (size_t i = size; i < size + GUARD; i++)
    {
        BOOST_TEST_CONTEXT("guard byte " << i - size << " after " << input)
        {
            BOOST_TEST(guarded[i] == guard_byte(i));
        }
        guard_intact = guard_intact && guarded[i] == guard_byte(i);
    }
    if (!guard_intact)
    {
        return guarded.get();
    }

    std::unique_ptr<char[]> exact(new char[size]);
    memcpy(exact.get(), input.c_str(), input.size() + 1);
    str_substitute(old, newstr, exact.get());

    BOOST_TEST(std::string(exact.get()) == std::string(guarded.get()));
    return exact.get();
}
} // namespace

/**********************************************************************************
 * Textutil_Substitute_Suite - the char * str_substitute. The growing branch used to
 * move olen - 1 bytes too many: a read and a write past the end of the string. Most cases
 * go through substituted(): a roomy buffer, one with guard bytes after it (any build) and an
 * exact-size one (AddressSanitizer), each only if the one before was right. An empty old text
 * must change nothing (it used to loop for ever), and the search resumes after each replacement,
 * so it ends when the new text contains the old one. The suite has one timeout of 10 s in all: a
 * loop that never ends fails the case that hits it, and the cases after it are skipped.
 */
BOOST_AUTO_TEST_SUITE(Textutil_Substitute_Suite, *boost::unit_test::timeout(10))

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

BOOST_AUTO_TEST_CASE(new_text_containing_old_text_ends)
{
    // Each match is replaced once. The old function searched again from the start and never ended,
    // growing the string by a byte a time: it runs into the timeout (see substituted())
    BOOST_TEST(substituted("a", "aa", "aba", "aabaa") == "aabaa");
    BOOST_TEST(substituted("ab", "xabx", "ab ab", "xabx xabx") == "xabx xabx");
}

BOOST_AUTO_TEST_CASE(search_resumes_after_the_replacement)
{
    // A match is not found again in the text left of a replacement
    BOOST_TEST(substituted("aa", "a", "aaaa", "aa") == "aa");
    // A replacement that joins the text on both sides of it does not make a new match
    BOOST_TEST(substituted("&cw", "", "x&c&cwwy", "x&cwy") == "x&cwy");
    // Neighbouring matches share nothing: " id " takes both of its spaces
    BOOST_TEST(substituted(" id ", " i would ", " id id id ", " i would id i would ") == " i would id i would ");
    BOOST_TEST(substituted(" but ", " , ", " but but ", " , but ") == " , but ");
}

BOOST_AUTO_TEST_CASE(agrees_with_the_std_string_overload)
{
    const char *olds[] = {"a", "ab", " id ", "aa", "aba"};
    const char *news[] = {"", "a", "aa", "xabx", "zz"};
    const char *inputs[] = {"", "a", "aba", "aaa", "aaaa", "aaaaa", "ababa", " id id id ", "ab ab abab", "xyz"};

    for (const char *old : olds)
    {
        for (const char *neu : news)
        {
            for (const char *in : inputs)
            {
                std::string expected = in;
                str_substitute(std::string(old), std::string(neu), expected);
                BOOST_TEST_CONTEXT("old '" << old << "' new '" << neu << "' in '" << in << "'")
                {
                    BOOST_TEST(substituted(old, neu, in, expected) == expected);
                }
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(same_length_and_shrink)
{
    BOOST_TEST(substituted("ab", "xy", "ab ab", "xy xy") == "xy xy");
    BOOST_TEST(substituted("abcde", "x", "ab abcde abcde", "ab x x") == "ab x x");
    BOOST_TEST(substituted("abcde", "", "abcde", "") == "");
    BOOST_TEST(substituted("abcde", nullptr, "1abcde2", "12") == "12");
    BOOST_TEST(substituted("aa", nullptr, "aaaaa", "a") == "a");
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
