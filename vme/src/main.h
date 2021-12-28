/*
 $Author: All $
 $RCSfile: main.h,v $
 $Date: 2004/03/20 06:13:22 $
 $Revision: 2.1 $
 */

#ifndef _MUD_MAIN_H
#define _MUD_MAIN_H

extern class descriptor_data *g_descriptor_list;
extern class descriptor_data *g_next_to_process;

extern int g_mud_bootzone;
extern int g_no_connections;
extern int g_max_no_connections;
extern int g_no_players;
extern int g_max_no_players;
extern int g_player_convert;
extern int g_slow_death;
extern int g_mud_shutdown;
extern int g_mud_reboot;
extern int g_wizlock;
extern int g_tics;

extern int g_world_norooms;
extern int g_world_noobjects;
extern int g_world_nochars;
extern int g_world_nozones;
extern char g_world_boottime[64];

extern const char *g_compile_date;
extern const char *g_compile_time;

// Prototypes
void ShowUsage(const char *c);

#endif
