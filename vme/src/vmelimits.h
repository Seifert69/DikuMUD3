/*
 $Author: All $
 $RCSfile: vmelimits.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */
#pragma once

#include "interpreter.h"
#include "structs.h"

int char_can_carry_unit(unit_data *ch, unit_data *unit);

int char_carry_n(unit_data *unit);
int char_carry_n_limit(unit_data *ch);
int char_can_carry_n(unit_data *ch, int n = 1);

int char_carry_w_limit(unit_data *ch);
int char_can_carry_w(unit_data *ch, int weight);

int mana_limit(unit_data *ch);
int hit_limit(unit_data *ch);
int move_limit(unit_data *ch);

int mana_gain(unit_data *ch);
int hit_gain(unit_data *ch);
int move_gain(unit_data *ch);

void set_title(unit_data *ch);

void advance_level(unit_data *ch);
void gain_exp(unit_data *ch, int gain);
void gain_exp_regardless(unit_data *ch, int gain);
void gain_condition(unit_data *ch, int condition, int value);
void do_level(unit_data *, char *, const command_info *);
