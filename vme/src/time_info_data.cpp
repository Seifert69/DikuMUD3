#include "time_info_data.h"

time_info_data::time_info_data(long h, long d, long m, long y)
    : hours(h)
    , day(d)
    , month(m)
    , year(y)
{
}

bool operator==(const time_info_data &left, const time_info_data &right) noexcept
{
    return left.hours == right.hours && left.day == right.day && left.month == right.month && left.year == right.year;
}

std::ostream &operator<<(std::ostream &os, const time_info_data &value) noexcept
{
    os << "h:" << value.hours << " d:" << value.day << " m:" << value.month << " y:" << value.year;
    return os;
}
