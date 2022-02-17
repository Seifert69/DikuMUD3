#pragma once

#include <ostream>

/*
 * This structure is purely intended to be an easy way to transfer
 * and return information about time (real or mudwise).
 */
class time_info_data
{
public:
    time_info_data() = default;
    time_info_data(long h, long d, long m, long y);
    time_info_data(const time_info_data &) = default;
    time_info_data(time_info_data &&) = default;
    time_info_data &operator=(const time_info_data &) = default;
    time_info_data &operator=(time_info_data &&) = default;
    ~time_info_data() = default;

    friend bool operator==(const time_info_data &left, const time_info_data &right) noexcept;
    friend std::ostream &operator<<(std::ostream &os, const time_info_data &value) noexcept;

    long getHours() const { return hours; }
    long getDay() const { return day; }
    long getMonth() const { return month; }
    long getYear() const { return year; }

    void setHours(long value) { hours = value; }
    void setDay(long value) { day = value; }
    void setMonth(long value) { month = value; }
    void setYear(long value) { year = value; }

private:
    long hours{0};
    long day{0};
    long month{0};
    long year{0};
};
