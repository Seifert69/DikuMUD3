/*
 $Author: All $
 $RCSfile: unitfind.cpp,v $
 $Date: 2004/12/12 18:22:49 $
 $Revision: 2.5 $
 */

#include <ctype.h>

#include "utils.h"
#include "db.h"
#include "unitfind.h"
#include "handler.h"
#include "textutil.h"
#include "utility.h"
#include "common.h"

int follower_count(class unit_data *u)
{
    struct char_follow_type *f;
    int x = 0;

    if (IS_CHAR(u))
    {
        for (f = CHAR_FOLLOWERS(u); f; f = f->next)
            x++;
    }
    return x;
}

class unit_data *get_follower(class unit_data *u, int num)
{
    struct char_follow_type *f;
    int x = 0;

    if (IS_CHAR(u))
    {
        for (f = CHAR_FOLLOWERS(u); f; f = f->next)
        {
            if (x == num)
                return (f->follower);
            x++;
        }
    }
    return NULL;
}

/* Assumes UNIT_IN(room) == NULL */
static ubit1 same_surroundings_room(class unit_data *room, class unit_data *u2)
{
    if (!UNIT_IN(u2))
        return FALSE;

    if (UNIT_IN(u2) == room)
        return TRUE;

    if (UNIT_IS_TRANSPARENT(UNIT_IN(u2)) && UNIT_IN(UNIT_IN(u2)) == room)
        return TRUE;

    return FALSE;
}

ubit1 same_surroundings(class unit_data *u1, class unit_data *u2)
{
    if (!UNIT_IN(u1))
        return same_surroundings_room(u1, u2);
    else if (!UNIT_IN(u2))
        return same_surroundings_room(u2, u1);

    if (UNIT_IN(u1) == UNIT_IN(u2))
        return TRUE;

    if (UNIT_IS_TRANSPARENT(UNIT_IN(u1)) && UNIT_IN(UNIT_IN(u1)) == UNIT_IN(u2))
        return TRUE;

    if (UNIT_IS_TRANSPARENT(UNIT_IN(u2)) && UNIT_IN(UNIT_IN(u2)) == UNIT_IN(u1))
        return TRUE;

    if (UNIT_IS_TRANSPARENT(UNIT_IN(u1)) && UNIT_IS_TRANSPARENT(UNIT_IN(u2)) && UNIT_IN(UNIT_IN(u1)) == UNIT_IN(UNIT_IN(u2)))
        return TRUE;

    return FALSE;
}

/* Does not accept multiple find flags as argument! */
/* Call several times if that is required           */
/* Fri Jan 24 06:04:47 PST 1997  HHS:
   added FIND_UNIT_PAY and FIND_UNIT_NOPAY
       To locate paying/ nonpaying players OR
          locate rooms in pay/nonpay zones
   When using FIND_UNIT_PAY/FIND_UNIT_NOPAY, you will
   only get interresting results when seaching for rooms or
   pc's. Thus searching _EQUIP would not give any units.
   _SURRO is fixed, so it finds a vild unit in the generated list,
     seaching first forward then backward from a random position,
     looking for a valig unit.
   The general findunit will ignore these flags!
*/
/* Fri Jan 24 10:22:06 PST 1997  HHS:
   I fixed the selection of one unit, so it will not have to run through
   the list two times. And without using an array (like scan4unit)

   I 've analyzed the problem, each time you find a kandidate unit,
   you count (like before), but you choose a unit by a chance of
     p(i)= 1/(i+1)
   where i is the number of units found so far... thus only one run :-).
   The result is that the likelihood of selecing the each is 1/maxcount.
   (i've even tested it experimentally)
*/

/* returns if PC is pay/no pay !0/0 */
static inline int pcpay(class unit_data *u)
{
    return ((PC_ACCOUNT(u).credit > 0.0) ||
            (PC_ACCOUNT(u).discount == 100) ||
            (PC_ACCOUNT(u).flatrate > (ubit32)time(0)) ||
            (CHAR_DESCRIPTOR(u) ? g_cServerConfig.FromLAN(CHAR_DESCRIPTOR(u)->host) : 0));
}

/* returns if ROOM is pay/no pay !0/0 */
static inline int roompay(class unit_data *u)
{
    return (UNIT_FI_ZONE(u)->payonly);
}

/* These functions determine if the units are candidates in find */
static inline int findcheck(class unit_data *u, int pset, int tflags)
{
    if (IS_SET(UNIT_TYPE(u), tflags))
    {
        if (pset == FIND_UNIT_PAY)
        {
            if (IS_PC(u) && pcpay(u))
                return 1;

            if (IS_ROOM(u) && roompay(u))
                return 1;

            return 0;
        }
        else if (pset == FIND_UNIT_NOPAY)
        {
            if (IS_PC(u) && !pcpay(u))
                return 1;

            if (IS_ROOM(u) && !roompay(u))
                return 1;

            return 0;
        }
        else
            return 1;
    }

    return 0;
}


// 2020. This really needs some rewriting... Plus the number(1,count) looks
// like it will always return the first unit...
// 
class unit_data *random_unit(class unit_data *ref, int sflags, int tflags)
{
    register class unit_data *u, *selected = NULL;
    int count = 0;
    int pset = 0;

    pset = sflags & (FIND_UNIT_NOPAY | FIND_UNIT_PAY);
    sflags &= FIND_UNIT_LOCATION_MASK;

    /* Needs work, there must be a better way to do this... */

    if (sflags == FIND_UNIT_WORLD)
    {
        for (u = unit_list; u; u = u->gnext)
            if ((u != ref) && findcheck(u, pset, tflags))
            {
                count++;
                if (number(1, count) == 1)
                    selected = u;
            }

        return selected;
    }
    else if (sflags == FIND_UNIT_ZONE)
    {
        class zone_type *z;

        if (UNIT_FI_ZONE(ref))
            z = UNIT_FI_ZONE(ref);
        else
            z = unit_zone(ref);

        for (u = unit_list; u; u = u->gnext)
            if ((u != ref) &&
                ((IS_PC(u) && unit_zone(u) == z) ||
                 (UNIT_FI_ZONE(u) == z)) &&
                findcheck(u, pset, tflags))
            {
                count++;
                if (number(1, count) == 1)
                    selected = u;
            }

        return selected;
    }
    else if (sflags == FIND_UNIT_INVEN)
    {
        for (u = UNIT_CONTAINS(ref); u; u = u->next)
            if ((!IS_OBJ(u) || (OBJ_EQP_POS(u) == 0)) &&
                findcheck(u, pset, tflags))
            {
                count++;
                if (number(1, count) == count)
                    selected = u;
            }

        return selected;
    }
    else if (sflags == FIND_UNIT_EQUIP)
    {
        /* Should this return ONLY? objects? HHS */
        if (IS_SET(UNIT_ST_OBJ, tflags))
        {
            for (u = UNIT_CONTAINS(ref); u; u = u->next)
                if (IS_OBJ(u) && OBJ_EQP_POS(u))
                    if (number(1, count) == count)
                        selected = u;

            return selected;
        }
    }
    else if (sflags == FIND_UNIT_SURRO)
    {
        int i, p;
        scan4_unit(ref, tflags);
        if (UVITOP > 0)
        {
            p = number(0, UVITOP - 1);
            /* find nearest that fulfill findcheck() */
            for (i = p; i < UVITOP; i++)
                if (findcheck(UVI(i), pset, tflags))
                    return UVI(i);
            /* try lookin back then */
            for (i = p - 1; i >= 0; i--)
                if (findcheck(UVI(i), pset, tflags))
                    return UVI(i);
            /* then give up (UV not containing any usefull
               units (findcheck()) */
        }
    }

    return NULL;
}

/* As find_unit below, except visibility is relative to
   viewer with respect to CHAR_CAN_SEE */
//
// type are the UNIT_ST bits to match for, default is FIND_UNIT (all bits)
//
// I consolidated almost identical version of this function. The old "dil" variant
// didn't respect CHAR_CAN_SEE. Which led to odd game behaviour. If for any reason 
// this also needs to work on units that can't be seen, let's add a boolean  argument if the
// function only returns units that are visible to the viewer, or not.
//
class unit_data *find_unit_general(const class unit_data *viewer,
                                   const class unit_data *ch,
                                   char **arg,
                                   const class unit_data *list,
                                   const ubit32 bitvector, ubit8 type)
{
    class unit_data *best = NULL;
    int best_len = 0;
    ubit32 bitvectorm;

    int i, number, original_number;
    const char *ct = NULL;
    char name[MAX_INPUT_LENGTH * 2], *c;
    ubit1 is_fillword = TRUE;
    class unit_data *u, *uu;

    if (type == 0)
        return NULL;

    /* Eliminate the 'pay' bits */
    bitvectorm = bitvector & FIND_UNIT_LOCATION_MASK;

    for (c = *arg; isaspace(*c); c++)
        ;

    /* Eliminate spaces and all "ignore" words */
    while (is_fillword)
    {
        for (i = 0; (name[i] = c[i]) && name[i] != ' '; i++)
            ;
        name[i] = 0;

        if (search_block(name, fillwords, TRUE) < 0)
            is_fillword = FALSE;
        else
            c += i;

        for (; *c == ' '; c++)
            ;
    }

    if (!*c)
        return NULL;

    str_remspc(c);

    for (i = 0; isdigit(name[i] = c[i]); i++)
        ;
    name[i] = 0;

    if (c[i] == '.')
    {
        number = original_number = atoi(name);
        c += i + 1;
    }
    else
        number = original_number = 1;

    if (IS_CHAR(ch)) /* Only check bitvector if ch IS a char! */
    {
        /* Equipment can only be objects. */
        if (IS_SET(bitvectorm, FIND_UNIT_EQUIP))
            for (u = UNIT_CONTAINS(ch); u; u = u->next)
                if (IS_SET(type, UNIT_TYPE(u)) && IS_OBJ(u) && OBJ_EQP_POS(u) &&
                    ((viewer == ch) || CHAR_CAN_SEE(viewer, u)) &&
                    (ct = UNIT_NAMES(u).IsNameRaw(c)) && (ct - c >= best_len))
                {
                    if (ct - c > best_len)
                    {
                        number = original_number;
                        best_len = ct - c;
                    }

                    if (--number == 0)
                        best = u;
                }

        if (IS_SET(bitvectorm, FIND_UNIT_INVEN))
            for (u = UNIT_CONTAINS(ch); u; u = u->next)
                if (IS_SET(type, UNIT_TYPE(u)) && (ct = UNIT_NAMES(u).IsNameRaw(c)) &&
                    ((viewer == ch) || CHAR_CAN_SEE(viewer, u)) &&
                    !(IS_OBJ(u) && OBJ_EQP_POS(u)) && (ct - c >= best_len))
                {
                    if (ct - c > best_len)
                    {
                        number = original_number;
                        best_len = ct - c;
                    }

                    if (--number == 0)
                        best = u;
                }

        /* This is the ugly one, modified for transparance */
        if (IS_SET(bitvectorm, FIND_UNIT_SURRO))
        {
            const char *tmp_self[] = {"self", NULL};

            if ((ct = is_name_raw(c, tmp_self)))
            {
                *arg = (char *)ct;
                return (class unit_data *)ch;
            }

            if (UNIT_IN(ch) == NULL)
            {
                slog(LOG_ALL, 0, "%s@%s is not in a room while in find_unit_general<br/>", UNIT_FI_NAME(ch), UNIT_FI_ZONENAME(ch));
            }
            else
            {
                /* MS: Removed !IS_ROOM(UNIT_IN(ch)) because you must be able to
                open rooms from the inside... */
                if (IS_SET(type, UNIT_TYPE(UNIT_IN(ch))) && (ct = UNIT_NAMES(UNIT_IN(ch)).IsNameRaw(c)) &&
                    CHAR_CAN_SEE(viewer, UNIT_IN(ch)) && (ct - c >= best_len))
                {
                    if (ct - c > best_len)
                    {
                        number = original_number;
                        best_len = ct - c;
                    }

                    if (--number == 0)
                        best = UNIT_IN(ch);
                }

                /* Run through units in local environment */
                for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
                {
                    if (IS_SET(type, UNIT_TYPE(u)) && (IS_ROOM(u) || CHAR_CAN_SEE(viewer, u))) /* Cansee room in dark */
                    {
                        if ((ct = UNIT_NAMES(u).IsNameRaw(c)) &&
                            (ct - c >= best_len))
                        {
                            if (ct - c > best_len)
                            {
                                number = original_number;
                                best_len = ct - c;
                            }

                            if (--number == 0)
                                best = u;
                        }

                        /* check tranparancy */
                        if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
                            for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                                if (IS_SET(type, UNIT_TYPE(uu)) && IS_CHAR(uu) && (ct = UNIT_NAMES(uu).IsNameRaw(c)) && CHAR_CAN_SEE(viewer, uu) && (ct - c >= best_len))
                                {
                                    if (ct - c > best_len)
                                    {
                                        number = original_number;
                                        best_len = ct - c;
                                    }

                                    if (--number == 0)
                                        best = uu;
                                }
                    }

                } /* End for */

                /* Run through units in local environment if upwards transparent */
                if ((u = UNIT_IN(UNIT_IN(ch))) && UNIT_IS_TRANSPARENT(UNIT_IN(ch)))
                {
                    for (u = UNIT_CONTAINS(u); u; u = u->next)
                        if (u != UNIT_IN(ch) && CHAR_CAN_SEE(viewer, u))
                        {
                            if (IS_SET(type, UNIT_TYPE(u)) && (ct = UNIT_NAMES(u).IsNameRaw(c)) && (ct - c >= best_len))
                            {
                                if (ct - c > best_len)
                                {
                                    number = original_number;
                                    best_len = ct - c;
                                }

                                if (--number == 0)
                                    best = u;
                            }

                            /* check down into transparent unit */
                            if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
                                for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                                    if (IS_SET(type, UNIT_TYPE(uu)) && IS_CHAR(uu) &&
                                        (ct = UNIT_NAMES(uu).IsNameRaw(c)) &&
                                        CHAR_CAN_SEE(viewer, uu) && (ct - c >= best_len))
                                    {
                                        if (ct - c > best_len)
                                        {
                                            number = original_number;
                                            best_len = ct - c;
                                        }

                                        if (--number == 0)
                                            best = uu;
                                    }
                        }
                }
            }
        }

        if (IS_SET(bitvectorm, FIND_UNIT_ZONE))
            for (u = unit_list; u; u = u->gnext)
                if (IS_SET(type, UNIT_TYPE(u)) && (ct = UNIT_NAMES(u).IsNameRaw(c)) && CHAR_CAN_SEE(viewer, u) && unit_zone(u) == unit_zone(ch) && (ct - c >= best_len))
                {
                    if (ct - c > best_len)
                    {
                        number = original_number;
                        best_len = ct - c;
                    }

                    if (--number == 0)
                        best = u;
                }

        if (IS_SET(bitvectorm, FIND_UNIT_WORLD))
            for (u = unit_list; u; u = u->gnext)
                if (IS_SET(type, UNIT_TYPE(u)) && (ct = UNIT_NAMES(u).IsNameRaw(c)) &&
                    CHAR_CAN_SEE(viewer, u) && (ct - c >= best_len))
                {
                    if (ct - c > best_len)
                    {
                        number = original_number;
                        best_len = ct - c;
                    }

                    if (--number == 0)
                        best = u;
                }
    }

    for (; list; list = list->next)
    {
        if (IS_SET(type, UNIT_TYPE((class unit_data *) list)) && (ct = UNIT_NAMES((class unit_data *)list).IsNameRaw(c)) && (ct - c >= best_len) && CHAR_CAN_SEE(viewer, list))
        {
            if (ct - c > best_len)
            {
                number = original_number;
                best_len = ct - c;
            }

            if (--number == 0)
                best = (class unit_data *)list;
        }
    }

    *arg = (c + best_len);

    return best;
}

/*
  Find Unit - General procedure to locate a unit.
  Ch        - The person looking for something
  Arg       - Address of a pointer to a string. Upon return the
  pointer to the string will point to the location
  after the name of the found object.
  List      - A pointer to a unit. This is used when for example
  a get xxx from bag is issued. List must then be the
  first object in the bag, and the list of objects in
  the bag will be searched.
  Bitvector - A bitvector (structs.h) designating where to look
  for a unit relative to Ch.

  Returns   - The unit which was found.

  Example:
  ch = Rainbird, arg -> "the 2.small bottle and the laugh".

  find_unit(ch, &arg, 0, FIND_UNIT_SURRO | FIND_UNIT_INVEN);
  Will try to find the second small bottle in the room or
  in the inventory.
  arg -> "and the laugh" if found.

  Example:
  ch = Rainbird, arg -> "the 2.small bottle from the bag",
  bag = UNIT_CONTAINS(bag)

  find_unit(ch, &arg, bag, 0);
  Will try to find the second small bottle from one of the
  elements in the "list" (bag).


  */

class unit_data *find_unit(const class unit_data *ch, char **arg,
                           const class unit_data *list, const ubit32 bitvector)
{
    return find_unit_general(ch, ch, arg, list, bitvector);
}

class unit_data *find_unit_dil(const class unit_data *ch, char **arg,
                               const class unit_data *list, const ubit32 bitvector,
                               ubit8 type)
{
    return find_unit_general(ch, ch, arg, list, bitvector, type);
}

class unit_data *file_index_type::find_symbolic_instance_ref(class unit_data *ref, ubit16 bitvector)
{
    register class unit_data *u, *uu;

    assert(this);

    if (ref == NULL)
        return NULL;

    if (IS_SET(bitvector, FIND_UNIT_EQUIP))
    {
        for (u = UNIT_CONTAINS(ref); u; u = u->next)
            if ((UNIT_FILE_INDEX(u) == this) && UNIT_IS_EQUIPPED(u))
                return u;
    }

    if (IS_SET(bitvector, FIND_UNIT_INVEN))
    {
        for (u = UNIT_CONTAINS(ref); u; u = u->next)
            if ((UNIT_FILE_INDEX(u) == this) && !UNIT_IS_EQUIPPED(u))
                return u;
    }

    if (IS_SET(bitvector, FIND_UNIT_SURRO) && UNIT_IN(ref))
    {
        if (this == UNIT_FILE_INDEX(UNIT_IN(ref)))
            return UNIT_IN(ref);

        /* Run through units in local environment */
        for (u = UNIT_CONTAINS(UNIT_IN(ref)); u; u = u->next)
        {
            if (UNIT_FILE_INDEX(u) == this)
                return u;

            /* check tranparancy */
            if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
                for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                    if (UNIT_FILE_INDEX(uu) == this)
                        return uu;
        }

        /* Run through units in local environment if upwards transparent */
        if ((u = UNIT_IN(UNIT_IN(ref))) && UNIT_IS_TRANSPARENT(UNIT_IN(ref)))
        {
            for (u = UNIT_CONTAINS(u); u; u = u->next)
                if (u != UNIT_IN(ref))
                {
                    if (this == UNIT_FILE_INDEX(u))
                        return u;

                    /* check down into transparent unit */
                    if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
                        for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                            if (this == UNIT_FILE_INDEX(uu))
                                return uu;
                }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_ZONE))
    {
        /* for (u = unit_list; u; u = u->gnext)
            if ((unit_zone(u) == this->zone) && (UNIT_FILE_INDEX(u) == this))
                return u;*/

        if (!this->fi_unit_list.empty())
        {
            for (std::forward_list<class unit_data *>::iterator it = this->fi_unit_list.begin() ; it != this->fi_unit_list.end(); it++)
                if (UNIT_FILE_INDEX(*it) == this)
                    return u;
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_WORLD))
    {
        /* for (u = unit_list; u; u = u->gnext)
            if (UNIT_FILE_INDEX(u) == this)
                return u;*/

        if (!this->fi_unit_list.empty())
            return this->fi_unit_list.front();
    }

    return NULL;
}

class unit_data * file_index_type::find_symbolic_instance(void)
{
    // class unit_data *u;

    assert(this);

    /*
    for (class unit_data *u = unit_list; u; u = u->gnext)
    {
        if (UNIT_FILE_INDEX(u) == this)
        {
            if (this->fi_unit_list.front() != u)
            {
                slog(LOG_ALL, 0, "Break me here %s@%s is not in a room while in find_unit_general<br/>", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
            }

            return u;
        }
    }*/

    if (!this->fi_unit_list.empty())
        return this->fi_unit_list.front();

    return NULL;
}


// Will find the unit_data for the file_index_type in question
// which is matching the idx (ignore for PCs)
//
class unit_data *find_symbolic_idx(const char *zone, const char *name, int idx)
{
    class file_index_type *fi = find_file_index(zone, name);

    union {
        int i;
        void *p;
    } val;
    
    if (fi && !fi->fi_unit_list.empty())
    {
        if (fi->type == UNIT_ST_PC)
            return fi->fi_unit_list.front();

        for (std::forward_list<class unit_data *>::iterator it = fi->fi_unit_list.begin() ; it != fi->fi_unit_list.end(); it++)
        {
            val.p = *it;
            if (val.i == idx)
                return *it;
 
        }
    }

    return NULL;
}

class unit_data *find_symbolic(const char *zone, const char *name)
{
    class file_index_type *fi = find_file_index(zone, name);

    if (fi)
        return fi->find_symbolic_instance();
    else
        return NULL;
}

struct unit_vector_data unit_vector;

/* Init the unit_vector for FIRST use */
static void init_unit_vector(void)
{
    unit_vector.size = 10;

    CREATE(unit_vector.units, class unit_data *, unit_vector.size);
}

/* If things get too cramped, double size of unit_vector */
static void double_unit_vector(void)
{
    unit_vector.size *= 2;

    RECREATE(unit_vector.units, class unit_data *, unit_vector.size);
}

/* Scan the chars surroundings and all transparent surroundings for all  */
/* units of types which match 'flags' in the 'room' specified.           */
/* Difference to scan4_unit is that a room is searched for contents,     */
/* but not outside room.                                                 */
void scan4_unit_room(class unit_data *room, ubit8 type)
{
    class unit_data *u, *uu;

    unit_vector.top = 0;

    if (unit_vector.size == 0)
        init_unit_vector();

    for (u = UNIT_CONTAINS(room); u; u = u->next)
    {
        if (IS_SET(UNIT_TYPE(u), type))
        {
            unit_vector.units[unit_vector.top++] = u;
            if (unit_vector.size == unit_vector.top)
                double_unit_vector();
        }

        /* down into transparent unit */
        if (UNIT_IS_TRANSPARENT(u))
            for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                if (IS_SET(UNIT_TYPE(uu), type))
                {
                    unit_vector.units[unit_vector.top++] = uu;
                    if (unit_vector.size == unit_vector.top)
                        double_unit_vector();
                }
    }
}

/* MS: Fixed bug in scan4_XXX, only assigned 'u' in last               */
/*     check. Now assigns 'uu' instead                                 */

/* Scan the chars surroundings and all transparent surroundsings for all */
/* units of types which match 'flags'. Updates the 'unit_vector' for     */
/* use in local routines.                                                */
void scan4_unit(class unit_data *ch, ubit8 type)
{
    class unit_data *u, *uu;

    if (!UNIT_IN(ch))
    {
        scan4_unit_room(ch, type);
        return;
    }

    unit_vector.top = 0;

    if (unit_vector.size == 0)
        init_unit_vector();

    for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
    {
        if (u != ch && IS_SET(UNIT_TYPE(u), type))
        {
            unit_vector.units[unit_vector.top++] = u;
            if (unit_vector.size == unit_vector.top)
                double_unit_vector();
        }

        /* down into transparent unit */
        if (UNIT_IS_TRANSPARENT(u))
            for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                if (IS_SET(UNIT_TYPE(uu), type))
                {
                    unit_vector.units[unit_vector.top++] = uu;
                    if (unit_vector.size == unit_vector.top)
                        double_unit_vector();
                }
    }

    /* up through transparent unit */
    if (UNIT_IS_TRANSPARENT(UNIT_IN(ch)) && UNIT_IN(UNIT_IN(ch)))
        for (u = UNIT_CONTAINS(UNIT_IN(UNIT_IN(ch))); u; u = u->next)
        {
            if (IS_SET(UNIT_TYPE(u), type))
            {
                unit_vector.units[unit_vector.top++] = u;
                if (unit_vector.size == unit_vector.top)
                    double_unit_vector();
            }

            /* down into transparent unit */
            if (UNIT_IS_TRANSPARENT(u) && u != UNIT_IN(ch))
                for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                    if (IS_SET(UNIT_TYPE(uu), type))
                    {
                        unit_vector.units[unit_vector.top++] = uu; /* MS FIX */
                        if (unit_vector.size == unit_vector.top)
                            double_unit_vector();
                    }
        }
}

static class unit_data *scan4_ref_room(class unit_data *room, class unit_data *fu)
{
    class unit_data *u;

    if (room == fu)
        return fu;
        
    for (u = UNIT_CONTAINS(room); u; u = u->next)
        if (u == fu)
            return fu;

    for (u = UNIT_CONTAINS(room); u; u = u->next)
    {
        if (((IS_ROOM(u) || IS_OBJ(u)) && (!UNIT_IS_TRANSPARENT(u))))
            continue;
        if (scan4_ref_room(u, fu))
            return fu;
    }

    return NULL;
}

/* Scan the chars surroundings and all transparent surroundings for any   */
/* unit pointer occurance matching *fu. Return NULL or fu. Used only when */
/* you do not know wheter or not *fu has been extracted from the game. If */
/* you know that *fu exists, then a much simpler test is possible using   */
/* the 'same_surroundings()' function.                                     */
/* No checks for invisibility and the like                                */
class unit_data *scan4_ref(class unit_data *ch, class unit_data *fu)
{

    if (!UNIT_IN(ch))
        return (scan4_ref_room(ch, fu));
    else if (((IS_ROOM(UNIT_IN(ch)) || IS_OBJ(UNIT_IN(ch))) && (!UNIT_IS_TRANSPARENT(UNIT_IN(ch)))))
        return (scan4_ref_room(UNIT_IN(ch), fu));
    else
        return (scan4_ref(UNIT_IN(ch), fu));
}

/* Possible alternate of the above function. The above doesn't seem to properly 
   interpret the transparent and closed. But it proabably doesn't matter because
   it's just a DIL secure question I believe :o)
class unit_data *alternate_scan4_ref(class unit_data *ch, class unit_data *fu)
{
    if (scan4_ref_room(ch, fu))
        return fu;

    if (UNIT_IN(ch))
    {
        if (scan4_ref_room(UNIT_IN(ch), fu))
            return fu;

        if (UNIT_IN(UNIT_IN(ch)) && UNIT_IS_TRANSPARENT(UNIT_IN(ch)) && !IS_SET(UNIT_OPEN_FLAGS(UNIT_IN(ch)), EX_CLOSED))
            if (scan4_ref_room(UNIT_IN(UNIT_IN(ch)), fu))
                return fu;
    }

    return NULL;
}
*/