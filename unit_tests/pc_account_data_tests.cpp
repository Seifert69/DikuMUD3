#include "bytestring.h"
#include "pc_account_data.h"

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * pc_account_data_Suite
 *
 * readFrom() reads a player's account from a unit file: from version 52 with the
 * flat rate expiry date. read_unit_string() refuses versions below 48.
 */
BOOST_AUTO_TEST_SUITE(pc_account_data_Suite)

static void append_account(CByteBuffer &buf, bool flatrate)
{
    buf.AppendFloat(12.5f); // credit
    buf.Append32(300);      // credit limit
    buf.Append32(4000);     // total credit
    buf.Append16(1234);     // last four digits
    buf.Append8(15);        // discount
    if (flatrate)
    {
        buf.Append32(987654); // flat rate expiry
    }
    buf.Append8(2); // crack attempts
}

static void check(const pc_account_data &account, ubit32 flatrate)
{
    pc_account_data a = account;
    BOOST_TEST(a.getAccountBalance() == 12.5f);
    BOOST_TEST(a.getCreditLimit() == 300u);
    BOOST_TEST(a.getTotalCredit() == 4000u);
    BOOST_TEST(a.getLastFourDigitsofCreditCard() == 1234);
    BOOST_TEST(a.getDiscountPercentage() == 15);
    BOOST_TEST(a.getFlatRateExpirationDate() == flatrate);
    BOOST_TEST(a.getCrackAttempts() == 2);
}

BOOST_AUTO_TEST_CASE(versions_48_to_51_have_no_flat_rate)
{
    for (const ubit8 version : {ubit8(48), ubit8(51)})
    {
        CByteBuffer buf;
        append_account(buf, false);
        pc_account_data account;
        int error = 0;
        account.readFrom(buf, version, error);
        BOOST_TEST(error == 0);
        BOOST_TEST(buf.GetReadPosition() == buf.GetLength());
        check(account, 0);
    }
}

BOOST_AUTO_TEST_CASE(version_52_on_has_the_flat_rate)
{
    for (const ubit8 version : {ubit8(52), ubit8(77)})
    {
        CByteBuffer buf;
        append_account(buf, true);
        pc_account_data account;
        int error = 0;
        account.readFrom(buf, version, error);
        BOOST_TEST(error == 0);
        BOOST_TEST(buf.GetReadPosition() == buf.GetLength());
        check(account, 987654);
    }
}

// What writeTo() writes, readFrom() reads back
BOOST_AUTO_TEST_CASE(write_then_read)
{
    CByteBuffer in;
    append_account(in, true);
    pc_account_data account;
    int error = 0;
    account.readFrom(in, 77, error);

    CByteBuffer out;
    account.writeTo(out);
    pc_account_data copy;
    copy.readFrom(out, 77, error);
    BOOST_TEST(error == 0);
    check(copy, 987654);
}

BOOST_AUTO_TEST_SUITE_END()
