/*
 $Author: All $
 $RCSfile: weather.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */
#pragma once

#include "time_info_data.h"

#include <ctime>
#include <optional>
#include <string>

class unit_data;

time_info_data real_time_passed(time_t t2, time_t t1);
time_info_data mud_date();
time_info_data mud_time_passed(time_t t2, time_t t1);
time_info_data age(unit_data *ch);

void update_time_and_weather();
void boot_time_and_weather();
void weather_and_time_event(void *, void *);

extern const time_t g_beginning_of_time;
extern int g_sunlight;

class Weather
{
public:
    Weather() = default;

    /**
     * Setup and initial the weather.
     */
    void boot(const time_info_data &time_info);

    /**
     * Update the weather
     * @param time_data Mud time
     * @return May return a string message describing the weather change
     */
    std::optional<std::string> updateWeather(const time_info_data &time_data);

    int getPressure() const;
    int getChange() const;
    int getSky() const;
    int getBase() const;

    void setBase(int value);
    void incrementChangeBy(int value);
    void decrementChangeBy(int value);
    void modifyPressureBy(int value);

private:
    void setChange(int value);
    void setPressure(int value);
    ;

    int m_pressure{0}; ///< How is the pressure ( Mb )
    int m_change{0};   ///< How fast and what way does it change.
    int m_sky{0};      ///< How is the sky.
    int m_base{0};     ///< What is the basis pressure like?
    void updateSky();
};
