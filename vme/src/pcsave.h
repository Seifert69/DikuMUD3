#pragma once

#include "essential.h"

#include <string>

std::string PlayerFileName(const char *);
int delete_inventory(const char *pName);
int delete_player(const char *);
int find_player_id(char *pName);
int player_exists(const char *pName);
int read_player_id(void);
void save_player(class unit_data *pc);
class unit_data *load_player(const char *pName);
void save_player_contents(class unit_data *pc, int fast);
sbit32 new_player_id(void);
void assign_player_file_index(unit_data *pc);
void save_player_file(class unit_data *pc);
void player_file_index(void);

extern sbit32 g_player_id;
