/*
 $Author: All $
 $RCSfile: dbfind.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.4 $
 */
#include "dbfind.h"

#include "db.h"
#include "descriptor_data.h"
#include "dil.h"
#include "file_index_type.h"
#include "handler.h"
#include "main_functions.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"

#include <cstring>

descriptor_data *find_descriptor(const char *name, descriptor_data *except)
{
    descriptor_data *d = nullptr;

    /* Check if already playing */
    for (d = g_descriptor_list; d; d = d->getNext())
    {
        if (d != except && str_ccmp(PC_FILENAME(CHAR_ORIGINAL(d->cgetCharacter())), name) == 0)
        {
            return d;
        }
    }

    return nullptr;
}

/* Find a named zone */
zone_type *find_zone(const char *zonename)
{
    // bin_search_type *ba;

    if ((zonename == nullptr) || !*zonename)
    {
        return nullptr;
    }

    auto it = g_zone_info.mmp.find(zonename);
    if (it != g_zone_info.mmp.end())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }

    /*    ba = binary_search(g_zone_info.ba, zonename, g_zone_info.no_of_zones);

        return ba ? (class zone_type *)ba->block : NULL;*/
}

/* Zonename & name must point to non-empty strings. Must be lower case */
file_index_type *find_file_index(const char *zonename, const char *name)
{
    zone_type *zone = nullptr;

    if (!*name)
    {
        return nullptr;
    }

    char bufzone[MAX_STRING_LENGTH];
    char bufname[MAX_STRING_LENGTH];

    strcpy(bufzone, zonename);
    str_lower(bufzone);

    if ((zone = find_zone(bufzone)) == nullptr)
    {
        return nullptr;
    }

    strcpy(bufname, name);
    str_lower(bufname);

    auto it = zone->cgetFileIndexMap().find(bufname);

    if (it != zone->cgetFileIndexMap().end())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }

    /*if ((ba = binary_search(zone->ba, name, zone->no_of_fi)) == NULL)
        return NULL;

    return (class file_index_type *)ba->block;*/
}

/* Zonename & name must point to non-empty strings */
diltemplate *find_dil_index(const char *zonename, const char *name)
{
    zone_type *zone = nullptr;
    // struct bin_search_type *ba;

    if (str_is_empty(name))
    {
        return nullptr;
    }

    if ((zone = find_zone(zonename)) == nullptr)
    {
        return nullptr;
    }

    auto it = zone->cgetDILTemplate().find(name);

    if (it != zone->cgetDILTemplate().end())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }

    /*    if ((ba = binary_search(zone->tmplba, name, zone->no_tmpl)) == NULL)
            return NULL;

        return (struct diltemplate *)ba->block;*/
}

/*
 * This function searches for the
 * defined DIL template for external
 * call by a DIL program.
 * Uses find_dil_index...
 */
diltemplate *find_dil_template(const char *name)
{
    char zbuf[256];
    char pbuf[256];

    if (str_is_empty(name))
    {
        return nullptr;
    }

    split_fi_ref(name, zbuf, pbuf);

    return find_dil_index(zbuf, pbuf);
}

/*  Find a room in the world based on zone name and name e.g.
 *  "midgaard", "prison" and return a pointer to the room
 */
unit_data *world_room(const char *zone, const char *name)
{
    file_index_type *fi = nullptr;
    fi = find_file_index(zone, name);

    if (fi && (fi->getType() == UNIT_ST_ROOM) && (!fi->Empty()))
    {
        return (fi->Front());
    }

    return nullptr;
}

/*  Find file index.
 *  String MUST be in format 'name@zone\0' or 'zone/name'.
 */
file_index_type *str_to_file_index(const char *str)
{
    char name[FI_MAX_UNITNAME + 1];
    char zone[FI_MAX_ZONENAME + 1];

    split_fi_ref(str, zone, name);

    return find_file_index(zone, name);
}

/*  As str_to_file_index, except that if no zone is given, the
 *  zone of the 'ch' is assumed
 */
file_index_type *pc_str_to_file_index(unit_data *ch, const char *str)
{
    char name[MAX_INPUT_LENGTH + 1];
    char zone[MAX_INPUT_LENGTH + 1];

    split_fi_ref(str, zone, name);

    if (*name && !*zone)
    {
        strcpy(zone, unit_zone(ch)->getName());
    }

    return find_file_index(zone, name);
}
