#pragma once

void dmc_error(int fatal, const char *fmt, ...);
void init_unit(class unit_data *u);
void process_unit(class unit_data *u);
