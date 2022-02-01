/*
 $Author: All $
 $RCSfile: weather.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */
#pragma once

#include <ctime>

struct time_info_data real_time_passed(time_t t2, time_t t1);
struct time_info_data mud_date();
struct time_info_data mud_time_passed(time_t t2, time_t t1);
struct time_info_data age(class unit_data *ch);
void weather_change(struct time_info_data time_data);

void update_time_and_weather();
void boot_time_and_weather();
void weather_and_time_event(void *, void *);

extern const time_t g_beginning_of_time;
extern int g_sunlight;
