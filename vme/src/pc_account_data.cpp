#include "pc_account_data.h"

void pc_account_data::readFrom(CByteBuffer &buf, ubit8 unit_version, int &error)
{
    error += buf.ReadFloat(&credit);
    credit_limit = buf.ReadU32(&error);
    total_credit = buf.ReadU32(&error);

    if (unit_version >= 44)
    {
        last4 = buf.ReadS16(&error);
    }
    else
    {
        if (unit_version >= 41)
        {
            error += buf.Skip32(); /* cc_time */
        }
        last4 = -1;
    }

    if (unit_version >= 45)
    {
        discount = buf.ReadU8(&error);

        if (unit_version >= 52)
        {
            flatrate = buf.ReadU32(&error);
        }
        else
        {
            flatrate = 0;
        }

        cracks = buf.ReadU8(&error);
    }
    else
    {
        flatrate = 0;
        discount = 0;
        cracks = 0;
    }
}

void pc_account_data::writeTo(CByteBuffer &buf)
{
    buf.AppendFloat(credit);
    buf.Append32(credit_limit);
    buf.Append32(total_credit);
    buf.Append16(last4);
    buf.Append8(discount);
    buf.Append32(flatrate);
    buf.Append8(cracks);
}

float pc_account_data::getAccountBalance() const
{
    return credit;
}

void pc_account_data::setAccountBalance(float value)
{
    credit = value;
}

void pc_account_data::reduceAccountBalanceBy(float value)
{
    credit -= value;
}

void pc_account_data::increaseAccountBalanceBy(float value)
{
    credit += value;
}

ubit32 pc_account_data::getCreditLimit() const
{
    return credit_limit;
}

void pc_account_data::setCreditLimit(ubit32 value)
{
    credit_limit = value;
}

ubit32 pc_account_data::getTotalCredit() const
{
    return total_credit;
}

void pc_account_data::increaseTotalCreditBy(ubit32 value)
{
    total_credit += value;
}

void pc_account_data::reduceTotalCreditBy(ubit32 value)
{
    total_credit -= value;
}

void pc_account_data::setTotalCredit(ubit32 value)
{
    total_credit = value;
}

sbit16 pc_account_data::getLastFourDigitsofCreditCard()
{
    return last4;
}

void pc_account_data::setLastFourDigitsofCreditCard(sbit16 value)
{
    last4 = value;
}

ubit8 pc_account_data::getCrackAttempts() const
{
    return cracks;
}

void pc_account_data::setCrackAttempts(ubit8 value)
{
    cracks = value;
}

ubit8 pc_account_data::getDiscountPercentage() const
{
    return discount;
}

void pc_account_data::setDiscountPercentage(ubit8 value)
{
    discount = value;
}

const ubit32 &pc_account_data::getFlatRateExpirationDate() const
{
    return flatrate;
}

void pc_account_data::incFlatRateExpirationDate(ubit32 value)
{
    flatrate += value;
}

void pc_account_data::setFlatRateExpirationDate(ubit32 value)
{
    flatrate = value;
}
