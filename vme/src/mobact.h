#pragma once

#include "structs.h"

void ResetFptrTimer(unit_data *u, unit_fptr *fptr);
void SetFptrTimer(unit_data *u, unit_fptr *fptr);
void special_event(void *p1, void *p2);
void start_all_special(unit_data *u);
void start_special(unit_data *u, unit_fptr *fptr);
void stop_all_special(unit_data *u);
void stop_special(unit_data *u, unit_fptr *fptr);
