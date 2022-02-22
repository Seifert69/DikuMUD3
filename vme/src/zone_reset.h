#pragma once

#include "structs.h"
#include "zone_type.h"

void zone_reset(zone_type *zone);
void reset_all_zones();
void zone_event(void *, void *);

extern zone_type *g_boot_zone;
