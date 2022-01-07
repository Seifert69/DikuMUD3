#pragma once
#include <ctime>
#include <pthread.h>
#include "essential.h"
#include "values.h"

class cSector;
class eventqueue;

#ifdef PROFILE
extern char g_etext;
#endif
extern cSector g_sector_dat;
extern char g_world_boottime[64];
extern class descriptor_data *g_descriptor_list;
extern class descriptor_data *g_next_to_process;
extern class file_index_type *g_letter_fi;
extern class file_index_type *g_slime_fi;
extern class log_buffer g_log_buf[];
extern class unit_data *g_combat_list;
extern class unit_data *g_destroy_room;
extern class unit_data *g_entry_room;
extern class unit_data *g_unit_list;
extern class zone_type *g_boot_zone;
extern const char *g_pc_race_adverbs[PC_RACE_MAX + 1];
extern const char *g_professions[PROFESSION_MAX + 1];
extern const time_t g_beginning_of_time;
extern eventqueue g_events;
extern int g_dilmenu;
extern int g_memory_npc_alloc;
extern int g_memory_obj_alloc;
extern int g_memory_pc_alloc;
extern int g_memory_room_alloc;
extern int g_memory_roomread_alloc;
extern int g_memory_zoneidx_alloc;
extern int g_memory_zonereset_alloc;
extern int g_mud_bootzone;
extern int g_mud_reboot;
extern int g_mud_shutdown;
extern int g_mudboot;
extern int g_player_convert;
extern int g_possible_saves;
extern int g_tics;
extern int g_wizlock;
extern int g_world_nochars;
extern int g_world_nonpc;
extern int g_world_noobjects;
extern int g_world_nopc;
extern int g_world_norooms;
extern int g_world_nozones;
extern long g_nTickUsec;
extern pthread_t g_dijkstra_thread;
extern sbit32 g_player_id;
extern struct apply_function_type g_apf[];
extern struct command_info *g_cmd_dirs[MAX_EXIT + 1];
extern struct command_info *g_cmd_follow;
extern struct command_info g_cmd_auto_extract;
extern struct command_info g_cmd_auto_msg;
extern struct command_info g_cmd_auto_tick;
extern struct damage_chart_type g_spell_chart[SPL_TREE_MAX];
extern struct diltemplate *g_nanny_dil_tmpl;
extern struct diltemplate *g_playerinit_tmpl;
extern struct requirement_type g_pc_race_base[]; // no storage allocated for this. Delete it?
extern struct shi_info_type g_shi_info[];
extern struct spell_info_type g_spell_info[SPL_TREE_MAX];
extern struct tick_function_type g_tif[];
extern struct trie_type *g_intr_trie;
extern struct unit_function_array_type g_unit_function_array[];
extern struct zone_info_type g_zone_info;
extern ubit32 g_memory_total_alloc;
extern unit_data *g_room_head;
extern unit_data *g_void_room;
