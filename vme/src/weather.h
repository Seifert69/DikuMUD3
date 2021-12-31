/*
 $Author: All $
 $RCSfile: weather.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */

#ifndef _MUD_WEATHER
#define _MUD_WEATHER

extern int sunlight;

extern const time_t beginning_of_time;

struct time_info_data real_time_passed(time_t t2, time_t t1);
struct time_info_data mud_date(void);
struct time_info_data mud_time_passed(time_t t2, time_t t1);
struct time_info_data age(class unit_data *ch);
void weather_change(struct time_info_data time_data);

void update_time_and_weather(void);
void boot_time_and_weather(void);

#endif
