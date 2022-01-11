#pragma once

void zone_reset(class zone_type *zone);
void reset_all_zones(void);
void zone_event(void *, void *);

extern class zone_type *g_boot_zone;
