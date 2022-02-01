#pragma once

#include "essential.h"
#include "structs.h"

#include <string>

std::string PlayerFileName(const char *);
int delete_inventory(const char *pName);
int delete_player(const char *);
int find_player_id(char *pName);
int player_exists(const char *pName);
int read_player_id();
void save_player(unit_data *pc);
unit_data *load_player(const char *pName);
void save_player_contents(unit_data *pc, int fast);
sbit32 new_player_id();
void assign_player_file_index(unit_data *pc);
void save_player_file(unit_data *pc);
void player_file_index();

extern sbit32 g_player_id;
