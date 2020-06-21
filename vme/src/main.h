/*
 $Author: All $
 $RCSfile: main.h,v $
 $Date: 2004/03/20 06:13:22 $
 $Revision: 2.1 $
 */

#ifndef _MUD_MAIN_H
#define _MUD_MAIN_H

extern class descriptor_data *descriptor_list;
extern class descriptor_data *next_to_process;

extern int mud_bootzone;
extern int no_connections;
extern int max_no_connections;
extern int no_players;
extern int max_no_players;
extern int player_convert;
extern int slow_death;
extern int mud_shutdown;
extern int mud_reboot;
extern int wizlock;
extern int tics;

extern int world_norooms;
extern int world_noobjects;
extern int world_nochars;
extern int world_nozones;
extern char world_boottime[64];


extern const char *compile_date;
extern const char *compile_time;

//Prototypes
void ShowUsage(const char *c);

#endif
