/*
 $Author: All $
 $RCSfile: vmelimits.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */

#ifndef _MUD_LIMITS_H
#define _MUD_LIMITS_H

#include "structs.h"

int char_can_carry_unit (class unit_data * ch, class unit_data * unit);


int char_carry_n (class unit_data * unit);
int char_carry_n_limit (class unit_data * ch);
int char_can_carry_n (class unit_data * ch, int n = 1);

int char_carry_w_limit (class unit_data * ch);
int char_can_carry_w (class unit_data * ch, int weight);


int mana_limit (class unit_data * ch);
int hit_limit_number (int);
int hit_limit (class unit_data * ch);
int move_limit (class unit_data * ch);

int mana_gain (class unit_data * ch);
int hit_gain (class unit_data * ch);
int move_gain (class unit_data * ch);

void set_title (class unit_data * ch);

void advance_level (class unit_data * ch);
void gain_exp (class unit_data * ch, int gain);
void gain_exp_regardless (class unit_data * ch, int gain);
void gain_condition (class unit_data * ch, int condition, int value);



#endif /* _MUD_LIMITS_H */
