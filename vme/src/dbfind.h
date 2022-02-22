/*
 $Author: All $
 $RCSfile: dbfind.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */
#pragma once

#include "structs.h"
#include "zone_type.h"

descriptor_data *find_descriptor(const char *name, descriptor_data *except);
zone_type *find_zone(const char *zonename);
file_index_type *find_file_index(const char *zonename, const char *name);
diltemplate *find_dil_index(const char *zonename, const char *name);
unit_data *world_room(const char *zone, const char *name);
diltemplate *find_dil_template(const char *name);
file_index_type *str_to_file_index(const char *str);
file_index_type *pc_str_to_file_index(unit_data *ch, const char *str);
zone_type *find_zone(const char *zonename);
