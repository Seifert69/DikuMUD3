/*
 $Author: All $
 $RCSfile: dbfind.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.4 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "db.h"
#include "dil.h"
#include "textutil.h"
#include "utility.h"
#include "dbfind.h"
#include "handler.h"

extern class descriptor_data *descriptor_list;

class descriptor_data *find_descriptor(const char *name, class descriptor_data *except)
{
    class descriptor_data *d;

    /* Check if already playing */
    for (d = descriptor_list; d; d = d->next)
        if (d != except &&
            str_ccmp(PC_FILENAME(CHAR_ORIGINAL(d->character)), name) == 0)
            return d;

    return NULL;
}

/*  Top is the size of the array (minimum 1).
 *  Returns pointer to element of array or null.
 *  Perhaps an index vs. -1 would be better?
 */
struct bin_search_type *binary_search(struct bin_search_type *ba, const char *str, register int top)
{
    register int mid = 0, bot, cmp;

    cmp = 1; /* Assume no compare                        */
    bot = 0; /* Point to lowest element in array         */
    top--;   /* Point to top element in array [0..top-1] */

    while (bot <= top)
    {
        mid = (bot + top) / 2;
        if ((cmp = strcmp(str, ba[mid].compare)) < 0)
            top = mid - 1;
        else if (cmp > 0)
            bot = mid + 1;
        else /* cmp == 0 */
            break;
    }

    return (cmp ? 0 : &ba[mid]);
}

/* Find a named zone */
class zone_type *find_zone(const char *zonename)
{
    struct bin_search_type *ba;

    if ((zonename == NULL) || !*zonename)
        return NULL;

    ba = binary_search(zone_info.ba, zonename, zone_info.no_of_zones);

    return ba ? (class zone_type *)ba->block : NULL;
}

/* Zonename & name must point to non-empty strings */
class file_index_type *find_file_index(const char *zonename, const char *name)
{
    class zone_type *zone;
    struct bin_search_type *ba;

    if (!*name)
        return NULL;

    if ((zone = find_zone(zonename)) == NULL)
        return NULL;

    if ((ba = binary_search(zone->ba, name, zone->no_of_fi)) == NULL)
        return NULL;

    return (class file_index_type *)ba->block;
}

/* Zonename & name must point to non-empty strings */
struct diltemplate *find_dil_index(char *zonename, char *name)
{
    class zone_type *zone;
    struct bin_search_type *ba;

    if (str_is_empty(name))
        return NULL;

    if ((zone = find_zone(zonename)) == NULL)
        return NULL;

    if ((ba = binary_search(zone->tmplba, name, zone->no_tmpl)) == NULL)
        return NULL;

    return (struct diltemplate *)ba->block;
}

/*
 * This function searches for the
 * defined DIL template for external
 * call by a DIL program.
 * Uses find_dil_index...
 */
struct diltemplate *find_dil_template(const char *name)
{
    char zbuf[256], pbuf[256];

    if (str_is_empty(name))
        return NULL;

    split_fi_ref(name, zbuf, pbuf);

    return find_dil_index(zbuf, pbuf);
}

/*  Find a room in the world based on zone name and name e.g.
 *  "midgaard", "prison" and return a pointer to the room
 */
class unit_data *world_room(const char *zone, const char *name)
{
    class file_index_type *fi;
    fi = find_file_index(zone, name);
    if (fi && fi->type == UNIT_ST_ROOM)
        return (fi->unit);

    return NULL;
}

/*  Find file index.
 *  String MUST be in format 'name@zone\0' or 'zone/name'.
 */
class file_index_type *str_to_file_index(const char *str)
{
    char name[FI_MAX_UNITNAME + 1], zone[FI_MAX_ZONENAME + 1];

    split_fi_ref(str, zone, name);

    return find_file_index(zone, name);
}

/*  As str_to_file_index, except that if no zone is given, the
 *  zone of the 'ch' is assumed
 */
class file_index_type *pc_str_to_file_index(class unit_data *ch, const char *str)
{
    char name[MAX_INPUT_LENGTH + 1], zone[MAX_INPUT_LENGTH + 1];

    split_fi_ref(str, zone, name);

    if (*name && !*zone)
        strcpy(zone, unit_zone(ch)->name);

    return find_file_index(zone, name);
}
