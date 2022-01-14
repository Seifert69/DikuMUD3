#pragma once

void ResetFptrTimer(class unit_data *u, class unit_fptr *fptr);
void SetFptrTimer(class unit_data *u, class unit_fptr *fptr);
void special_event(void *p1, void *p2);
void start_all_special(class unit_data *u);
void start_special(class unit_data *u, class unit_fptr *fptr);
void stop_all_special(class unit_data *u);
void stop_special(class unit_data *u, class unit_fptr *fptr);
