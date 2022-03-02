#pragma once

#include "event.h"
#include "structs.h"

#include <string>

// Prototypes
void ShowUsage(const char *c);
void type_validate_64();
void run_the_game(char *srvcfg);
void check_idle_event(void *, void *);
void check_overpopulation_event(void *p1, void *p2);
void check_reboot_event(void *, void *);
void game_event();
void game_loop();
void perform_violence_event(void *, void *);
void ping_multiplexers_event(void *, void *);
void timewarp_end(void *p1, void *p2);

#ifdef PROFILE
extern char g_etext;
#endif
extern std::string g_world_boottime;
extern descriptor_data *g_descriptor_list;
extern descriptor_data *g_next_to_process;
extern eventqueue g_events;
extern int g_mud_bootzone;
extern int g_no_players;
extern int g_max_no_players;
extern int g_player_convert;
extern int g_slow_death;
extern int g_mud_shutdown;
extern int g_mud_reboot;
extern int g_wizlock;
extern int g_tics;
extern int g_mudboot;
extern long g_nTickUsec;
extern bool g_dumptables;

extern const char *g_compile_date;
extern const char *g_compile_time;
