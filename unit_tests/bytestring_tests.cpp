#include "bytestring.h"

#include <cstdlib>
#include <cstring>
#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * CByteBuffer_Suite
 *
 * Strings in unit and player files end with a NUL. In a truncated or corrupt
 * file the data can end first: the string readers must report that instead of
 * reading on past the end of the data.
 */
BOOST_AUTO_TEST_SUITE(CByteBuffer_Suite)

static void append_bytes(CByteBuffer &buf, const char *s)
{
    for (; *s; s++)
    {
        buf.Append8(static_cast<ubit8>(*s));
    }
}

BOOST_AUTO_TEST_CASE(strings_read_as_before)
{
    CByteBuffer buf;
    buf.AppendString("abc");
    buf.AppendString("de");
    buf.AppendString("");

    char out[16];
    BOOST_TEST(buf.ReadStringCopy(out, sizeof(out)) == 0);
    BOOST_TEST(std::string(out) == "abc");
    BOOST_TEST(buf.GetReadPosition() == 4u);

    char *p = nullptr;
    BOOST_TEST(buf.ReadStringAlloc(&p) == 0);
    BOOST_TEST(std::string(p) == "de");
    free(p);

    p = reinterpret_cast<char *>(1);
    BOOST_TEST(buf.ReadStringAlloc(&p) == 0); // the empty string gives nullptr, as before
    BOOST_TEST(p == nullptr);
    BOOST_TEST(buf.GetReadPosition() == buf.GetLength());

    buf.Rewind();
    char *c = nullptr;
    BOOST_TEST(buf.SkipString(&c) == 0);
    BOOST_TEST(std::string(c) == "abc");
    BOOST_TEST(buf.GetReadPosition() == 4u);
}

BOOST_AUTO_TEST_CASE(string_without_nul_is_corrupt)
{
    CByteBuffer buf;
    append_bytes(buf, "abcdef"); // no NUL before the end of the data

    char out[16] = "x";
    BOOST_TEST(buf.ReadStringCopy(out, sizeof(out)) == 1);
    BOOST_TEST(out[0] == 0);
    BOOST_TEST(buf.GetReadPosition() == 0u);

    char *p = reinterpret_cast<char *>(1);
    BOOST_TEST(buf.ReadStringAlloc(&p) == 1);
    BOOST_TEST(p == nullptr);
    BOOST_TEST(buf.GetReadPosition() == 0u);

    char *c = nullptr;
    BOOST_TEST(buf.SkipString(&c) == 1);
    BOOST_TEST(c != nullptr);
    BOOST_TEST(c[0] == 0); // callers that ignore the error get an empty name
    BOOST_TEST(buf.GetReadPosition() == 0u);
}

BOOST_AUTO_TEST_CASE(reading_at_the_end_is_corrupt)
{
    CByteBuffer buf;
    buf.AppendString("abc");
    char out[16];
    BOOST_TEST(buf.ReadStringCopy(out, sizeof(out)) == 0);

    BOOST_TEST(buf.ReadStringCopy(out, sizeof(out)) == 1);
    char *p = nullptr;
    BOOST_TEST(buf.ReadStringAlloc(&p) == 1);
    BOOST_TEST(buf.SkipString(nullptr) == 1);
}

BOOST_AUTO_TEST_CASE(string_too_long_for_the_copy)
{
    CByteBuffer buf;
    buf.AppendString("abc");
    char out[3] = "xy";
    BOOST_TEST(buf.ReadStringCopy(out, sizeof(out)) == 1);
    BOOST_TEST(out[0] == 0);
}

// ReadBlock() reads a length and then that many bytes. A corrupt length (up to
// 2^31) used to be allocated before the read failed, and left behind with the
// block, so the caller had a huge size next to uninitialised memory.
BOOST_AUTO_TEST_CASE(ReadBlock_reads_a_valid_block)
{
    CByteBuffer buf;
    buf.Append32(3);
    buf.Append8('x');
    buf.Append8('y');
    buf.Append8('z');

    ubit8 *data = nullptr;
    ubit32 len = 99;
    BOOST_TEST(buf.ReadBlock(&data, &len) == 0);
    BOOST_TEST(len == 3u);
    BOOST_TEST(data != nullptr);
    BOOST_TEST(std::string(reinterpret_cast<char *>(data), 3) == "xyz");
    free(data);
}

BOOST_AUTO_TEST_CASE(ReadBlock_length_beyond_the_data_is_corrupt)
{
    CByteBuffer buf;
    buf.Append32(2000000000);
    buf.Append8('x');

    ubit8 *data = reinterpret_cast<ubit8 *>(1);
    ubit32 len = 99;
    BOOST_TEST(buf.ReadBlock(&data, &len) == 1);
    BOOST_TEST(data == nullptr);
    BOOST_TEST(len == 0u);
}

BOOST_AUTO_TEST_CASE(ReadBlock_missing_length_is_corrupt)
{
    CByteBuffer buf;
    ubit8 *data = reinterpret_cast<ubit8 *>(1);
    ubit32 len = 99;
    BOOST_TEST(buf.ReadBlock(&data, &len) == 1);
    BOOST_TEST(data == nullptr);
    BOOST_TEST(len == 0u);
}

BOOST_AUTO_TEST_SUITE_END()
