/*
 $Author: All $
 $RCSfile: dbfind.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */

#ifndef _MUD_DBFIND_H
#define _MUD_DBFIND_H

class descriptor_data *find_descriptor (const char *name, class descriptor_data *except);
class zone_type *find_zone(const char *zonename);
class file_index_type *find_file_index (const char *zonename, const char *name);
struct diltemplate *find_dil_index (const char *zonename, const char *name);
class unit_data *world_room (const char *zone, const char *name);
struct diltemplate *find_dil_template (const char *name);
class file_index_type *str_to_file_index (const char *str);
class file_index_type *pc_str_to_file_index (class unit_data * ch, const char *str);

#endif
