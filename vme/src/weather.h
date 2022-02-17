/*
 $Author: All $
 $RCSfile: weather.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */
#pragma once

#include "structs.h"
#include "time_info_data.h"

#include <ctime>

time_info_data real_time_passed(time_t t2, time_t t1);
time_info_data mud_date();
time_info_data mud_time_passed(time_t t2, time_t t1);
time_info_data age(unit_data *ch);
void weather_change(time_info_data time_data);

void update_time_and_weather();
void boot_time_and_weather();
void weather_and_time_event(void *, void *);

extern const time_t g_beginning_of_time;
extern int g_sunlight;
