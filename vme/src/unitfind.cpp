/*
 $Author: All $
 $RCSfile: unitfind.cpp,v $
 $Date: 2004/12/12 18:22:49 $
 $Revision: 2.5 $
 */

#include "unitfind.h"

#include "common.h"
#include "db.h"
#include "descriptor_data.h"
#include "handler.h"
#include "slog.h"
#include "textutil.h"
#include "utils.h"

#include <cctype>

int follower_count(unit_data *u)
{
    char_follow_type *f = nullptr;
    int x = 0;

    if (IS_CHAR(u))
    {
        for (f = CHAR_FOLLOWERS(u); f; f = f->next)
        {
            x++;
        }
    }
    return x;
}

unit_data *get_follower(unit_data *u, int num)
{
    char_follow_type *f = nullptr;
    int x = 0;

    if (IS_CHAR(u))
    {
        for (f = CHAR_FOLLOWERS(u); f; f = f->next)
        {
            if (x == num)
            {
                return (f->follower);
            }
            x++;
        }
    }
    return nullptr;
}

/* Assumes UNIT_IN(room) == NULL */
static ubit1 same_surroundings_room(const unit_data *room, const unit_data *u2)
{
    if (!UNIT_IN(u2))
    {
        return FALSE;
    }

    if (UNIT_IN(u2) == room)
    {
        return TRUE;
    }

    if (UNIT_IS_TRANSPARENT(UNIT_IN(u2)) && UNIT_IN(UNIT_IN(u2)) == room)
    {
        return TRUE;
    }

    return FALSE;
}

ubit1 same_surroundings(const unit_data *u1, const unit_data *u2)
{
    if (!UNIT_IN(u1))
    {
        return same_surroundings_room(u1, u2);
    }
    else if (!UNIT_IN(u2))
    {
        return same_surroundings_room(u2, u1);
    }

    if (UNIT_IN(u1) == UNIT_IN(u2))
    {
        return TRUE;
    }

    if (UNIT_IS_TRANSPARENT(UNIT_IN(u1)) && UNIT_IN(UNIT_IN(u1)) == UNIT_IN(u2))
    {
        return TRUE;
    }

    if (UNIT_IS_TRANSPARENT(UNIT_IN(u2)) && UNIT_IN(UNIT_IN(u2)) == UNIT_IN(u1))
    {
        return TRUE;
    }

    if (UNIT_IS_TRANSPARENT(UNIT_IN(u1)) && UNIT_IS_TRANSPARENT(UNIT_IN(u2)) && UNIT_IN(UNIT_IN(u1)) == UNIT_IN(UNIT_IN(u2)))
    {
        return TRUE;
    }

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
static inline int pcpay(unit_data *u)
{
    return ((PC_ACCOUNT(u).credit > 0.0) || (PC_ACCOUNT(u).discount == 100) || (PC_ACCOUNT(u).flatrate > (ubit32)time(nullptr)) ||
            (CHAR_DESCRIPTOR(u) ? g_cServerConfig.FromLAN(CHAR_DESCRIPTOR(u)->getHostname()) : 0));
}

/* returns if ROOM is pay/no pay !0/0 */
static inline int roompay(unit_data *u)
{
    return (UNIT_FI_ZONE(u)->getPayOnly());
}

/* These functions determine if the units are candidates in find */
static inline int findcheck(unit_data *u, int pset, int tflags)
{
    if (IS_SET(UNIT_TYPE(u), tflags))
    {
        if (pset == FIND_UNIT_PAY)
        {
            if (IS_PC(u) && pcpay(u))
            {
                return 1;
            }

            if (IS_ROOM(u) && roompay(u))
            {
                return 1;
            }

            return 0;
        }
        else if (pset == FIND_UNIT_NOPAY)
        {
            if (IS_PC(u) && !pcpay(u))
            {
                return 1;
            }

            if (IS_ROOM(u) && !roompay(u))
            {
                return 1;
            }

            return 0;
        }
        else
        {
            return 1;
        }
    }

    return 0;
}

// 2020. This really needs some rewriting... Plus the number(1,count) looks
// like it will always return the first unit...
//
unit_data *random_unit(unit_data *ref, int sflags, int tflags)
{
    unit_data *u = nullptr;
    unit_data *selected = nullptr;
    int count = 0;
    int pset = 0;

    pset = sflags & (FIND_UNIT_NOPAY | FIND_UNIT_PAY);
    sflags &= FIND_UNIT_LOCATION_MASK;

    /* Needs work, there must be a better way to do this... */

    if (sflags == FIND_UNIT_WORLD)
    {
        for (u = g_unit_list; u; u = u->gnext)
        {
            if ((u != ref) && findcheck(u, pset, tflags))
            {
                count++;
                if (number(1, count) == 1)
                {
                    selected = u;
                }
            }
        }

        return selected;
    }
    else if (sflags == FIND_UNIT_ZONE)
    {
        zone_type *z = nullptr;

        if (UNIT_FI_ZONE(ref))
        {
            z = UNIT_FI_ZONE(ref);
        }
        else
        {
            z = unit_zone(ref);
        }

        for (u = g_unit_list; u; u = u->gnext)
        {
            if ((u != ref) && ((IS_PC(u) && unit_zone(u) == z) || (UNIT_FI_ZONE(u) == z)) && findcheck(u, pset, tflags))
            {
                count++;
                if (number(1, count) == 1)
                {
                    selected = u;
                }
            }
        }

        return selected;
    }
    else if (sflags == FIND_UNIT_INVEN)
    {
        for (u = UNIT_CONTAINS(ref); u; u = u->next)
        {
            if ((!IS_OBJ(u) || (OBJ_EQP_POS(u) == 0)) && findcheck(u, pset, tflags))
            {
                count++;
                if (number(1, count) == count)
                {
                    selected = u;
                }
            }
        }

        return selected;
    }
    else if (sflags == FIND_UNIT_EQUIP)
    {
        /* Should this return ONLY? objects? HHS */
        if (IS_SET(UNIT_ST_OBJ, tflags))
        {
            for (u = UNIT_CONTAINS(ref); u; u = u->next)
            {
                if (IS_OBJ(u) && OBJ_EQP_POS(u))
                {
                    if (number(1, count) == count)
                    {
                        selected = u;
                    }
                }
            }

            return selected;
        }
    }
    else if (sflags == FIND_UNIT_SURRO)
    {
        int i = 0;
        int p = 0;
        scan4_unit(ref, tflags);
        if (UVITOP > 0)
        {
            p = number(0, UVITOP - 1);
            /* find nearest that fulfill findcheck() */
            for (i = p; i < UVITOP; i++)
            {
                if (findcheck(UVI(i), pset, tflags))
                {
                    return UVI(i);
                }
            }
            /* try lookin back then */
            for (i = p - 1; i >= 0; i--)
            {
                if (findcheck(UVI(i), pset, tflags))
                {
                    return UVI(i);
                }
            }
            /* then give up (UV not containing any usefull
               units (findcheck()) */
        }
    }

    return nullptr;
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
unit_data *
find_unit_general(const unit_data *viewer, const unit_data *ch, char **arg, const unit_data *list, const ubit32 bitvector, ubit8 type)
{
    unit_data *best = nullptr;
    int best_len = 0;
    ubit32 bitvectorm = 0;

    int i = 0;
    int number = 0;
    int original_number = 0;
    const char *ct = nullptr;
    char name[MAX_INPUT_LENGTH * 2];
    char *c = nullptr;
    ubit1 is_fillword = TRUE;
    unit_data *u = nullptr;
    unit_data *uu = nullptr;

    if (type == 0)
    {
        return nullptr;
    }

    /* Eliminate the 'pay' bits */
    bitvectorm = bitvector & FIND_UNIT_LOCATION_MASK;

    for (c = *arg; isaspace(*c); c++)
    {
        ;
    }

    /* Eliminate spaces and all "ignore" words */
    while (is_fillword)
    {
        for (i = 0; (name[i] = c[i]) && name[i] != ' '; i++)
        {
            ;
        }
        name[i] = 0;

        if (search_block(name, g_fillwords, TRUE) < 0)
        {
            is_fillword = FALSE;
        }
        else
        {
            c += i;
        }

        for (; *c == ' '; c++)
        {
            ;
        }
    }

    if (!*c)
    {
        return nullptr;
    }

    str_remspc(c);

    for (i = 0; isdigit(name[i] = c[i]); i++)
    {
        ;
    }
    name[i] = 0;

    if (c[i] == '.')
    {
        number = original_number = atoi(name);
        c += i + 1;
    }
    else
    {
        number = original_number = 1;
    }

    if (IS_CHAR(ch)) /* Only check bitvector if ch IS a char! */
    {
        /* Equipment can only be objects. */
        if (IS_SET(bitvectorm, FIND_UNIT_EQUIP))
        {
            for (u = UNIT_CONTAINS(ch); u; u = u->next)
            {
                if (IS_SET(type, UNIT_TYPE(u)) && IS_OBJ(u) && OBJ_EQP_POS(u) && ((viewer == ch) || CHAR_CAN_SEE(viewer, u)) &&
                    (ct = UNIT_NAMES(u).IsNameRaw(c)) && (ct - c >= best_len))
                {
                    if (ct - c > best_len)
                    {
                        number = original_number;
                        best_len = ct - c;
                    }

                    if (--number == 0)
                    {
                        best = u;
                    }
                }
            }
        }

        if (IS_SET(bitvectorm, FIND_UNIT_INVEN))
        {
            for (u = UNIT_CONTAINS(ch); u; u = u->next)
            {
                if (IS_SET(type, UNIT_TYPE(u)) && (ct = UNIT_NAMES(u).IsNameRaw(c)) && ((viewer == ch) || CHAR_CAN_SEE(viewer, u)) &&
                    !(IS_OBJ(u) && OBJ_EQP_POS(u)) && (ct - c >= best_len))
                {
                    if (ct - c > best_len)
                    {
                        number = original_number;
                        best_len = ct - c;
                    }

                    if (--number == 0)
                    {
                        best = u;
                    }
                }
            }
        }

        /* This is the ugly one, modified for transparance */
        if (IS_SET(bitvectorm, FIND_UNIT_SURRO))
        {
            const char *tmp_self[] = {"self", nullptr};

            if ((ct = is_name_raw(c, tmp_self)))
            {
                *arg = (char *)ct;
                return (unit_data *)ch;
            }

            if (UNIT_IN(ch) == nullptr)
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
                    {
                        best = UNIT_IN(ch);
                    }
                }

                /* Run through units in local environment */
                for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
                {
                    if (IS_SET(type, UNIT_TYPE(u)) && (IS_ROOM(u) || CHAR_CAN_SEE(viewer, u))) /* Cansee room in dark */
                    {
                        if ((ct = UNIT_NAMES(u).IsNameRaw(c)) && (ct - c >= best_len))
                        {
                            if (ct - c > best_len)
                            {
                                number = original_number;
                                best_len = ct - c;
                            }

                            if (--number == 0)
                            {
                                best = u;
                            }
                        }

                        /* check tranparancy */
                        if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
                        {
                            for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                            {
                                if (IS_SET(type, UNIT_TYPE(uu)) && IS_CHAR(uu) && (ct = UNIT_NAMES(uu).IsNameRaw(c)) &&
                                    CHAR_CAN_SEE(viewer, uu) && (ct - c >= best_len))
                                {
                                    if (ct - c > best_len)
                                    {
                                        number = original_number;
                                        best_len = ct - c;
                                    }

                                    if (--number == 0)
                                    {
                                        best = uu;
                                    }
                                }
                            }
                        }
                    }

                } /* End for */

                /* Run through units in local environment if upwards transparent */
                if ((u = UNIT_IN(UNIT_IN(ch))) && UNIT_IS_TRANSPARENT(UNIT_IN(ch)))
                {
                    for (u = UNIT_CONTAINS(u); u; u = u->next)
                    {
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
                                {
                                    best = u;
                                }
                            }

                            /* check down into transparent unit */
                            if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
                            {
                                for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                                {
                                    if (IS_SET(type, UNIT_TYPE(uu)) && IS_CHAR(uu) && (ct = UNIT_NAMES(uu).IsNameRaw(c)) &&
                                        CHAR_CAN_SEE(viewer, uu) && (ct - c >= best_len))
                                    {
                                        if (ct - c > best_len)
                                        {
                                            number = original_number;
                                            best_len = ct - c;
                                        }

                                        if (--number == 0)
                                        {
                                            best = uu;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (IS_SET(bitvectorm, FIND_UNIT_ZONE))
        {
            for (u = g_unit_list; u; u = u->gnext)
            {
                if (IS_SET(type, UNIT_TYPE(u)) && (ct = UNIT_NAMES(u).IsNameRaw(c)) && CHAR_CAN_SEE(viewer, u) &&
                    unit_zone(u) == unit_zone(ch) && (ct - c >= best_len))
                {
                    if (ct - c > best_len)
                    {
                        number = original_number;
                        best_len = ct - c;
                    }

                    if (--number == 0)
                    {
                        best = u;
                    }
                }
            }
        }

        if (IS_SET(bitvectorm, FIND_UNIT_WORLD))
        {
            for (u = g_unit_list; u; u = u->gnext)
            {
                if (IS_SET(type, UNIT_TYPE(u)) && (ct = UNIT_NAMES(u).IsNameRaw(c)) && CHAR_CAN_SEE(viewer, u) && (ct - c >= best_len))
                {
                    if (ct - c > best_len)
                    {
                        number = original_number;
                        best_len = ct - c;
                    }

                    if (--number == 0)
                    {
                        best = u;
                    }
                }
            }
        }
    }

    for (; list; list = list->next)
    {
        if (IS_SET(type, UNIT_TYPE((unit_data *)list)) && (ct = UNIT_NAMES((unit_data *)list).IsNameRaw(c)) && (ct - c >= best_len) &&
            CHAR_CAN_SEE(viewer, list))
        {
            if (ct - c > best_len)
            {
                number = original_number;
                best_len = ct - c;
            }

            if (--number == 0)
            {
                best = (unit_data *)list;
            }
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

unit_data *find_unit(const unit_data *ch, char **arg, const unit_data *list, const ubit32 bitvector)
{
    return find_unit_general(ch, ch, arg, list, bitvector);
}

unit_data *find_unit_dil(const unit_data *ch, char **arg, const unit_data *list, const ubit32 bitvector, ubit8 type)
{
    return find_unit_general(ch, ch, arg, list, bitvector, type);
}

// Will find the unit_data for the file_index_type in question
// which is matching the idx (ignore for PCs)
//
unit_data *find_symbolic_idx(const char *zone, const char *name, int idx)
{
    file_index_type *fi = find_file_index(zone, name);

    union
    {
        int i;
        void *p;
    } val;

    if (fi && !fi->Empty())
    {
        if (fi->getType() == UNIT_ST_PC)
        {
            return fi->Front();
        }

        for (std::forward_list<unit_data *>::iterator it = fi->Begin(); it != fi->End(); it++)
        {
            val.p = *it;
            if (val.i == idx)
            {
                return *it;
            }
        }
    }

    return nullptr;
}

unit_data *find_symbolic(const char *zone, const char *name)
{
    file_index_type *fi = find_file_index(zone, name);

    if (fi)
    {
        return fi->find_symbolic_instance();
    }
    else
    {
        return nullptr;
    }
}

unit_vector_data g_unit_vector;

/* Init the g_unit_vector for FIRST use */
static void init_unit_vector()
{
    g_unit_vector.size = 10;

    CREATE(g_unit_vector.units, unit_data *, g_unit_vector.size);
}

/* If things get too cramped, double size of g_unit_vector */
static void double_unit_vector()
{
    g_unit_vector.size *= 2;

    RECREATE(g_unit_vector.units, unit_data *, g_unit_vector.size);
}

/* Scan the chars surroundings and all transparent surroundings for all  */
/* units of types which match 'flags' in the 'room' specified.           */
/* Difference to scan4_unit is that a room is searched for contents,     */
/* but not outside room.                                                 */
void scan4_unit_room(unit_data *room, ubit8 type)
{
    unit_data *u = nullptr;
    unit_data *uu = nullptr;

    g_unit_vector.top = 0;

    if (g_unit_vector.size == 0)
    {
        init_unit_vector();
    }

    for (u = UNIT_CONTAINS(room); u; u = u->next)
    {
        if (IS_SET(UNIT_TYPE(u), type))
        {
            g_unit_vector.units[g_unit_vector.top++] = u;
            if (g_unit_vector.size == g_unit_vector.top)
            {
                double_unit_vector();
            }
        }

        /* down into transparent unit */
        if (UNIT_IS_TRANSPARENT(u))
        {
            for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
            {
                if (IS_SET(UNIT_TYPE(uu), type))
                {
                    g_unit_vector.units[g_unit_vector.top++] = uu;
                    if (g_unit_vector.size == g_unit_vector.top)
                    {
                        double_unit_vector();
                    }
                }
            }
        }
    }
}

/* MS: Fixed bug in scan4_XXX, only assigned 'u' in last               */
/*     check. Now assigns 'uu' instead                                 */

/* Scan the chars surroundings and all transparent surroundsings for all */
/* units of types which match 'flags'. Updates the 'g_unit_vector' for     */
/* use in local routines.                                                */
void scan4_unit(unit_data *ch, ubit8 type)
{
    unit_data *u = nullptr;
    unit_data *uu = nullptr;

    if (!UNIT_IN(ch))
    {
        scan4_unit_room(ch, type);
        return;
    }

    g_unit_vector.top = 0;

    if (g_unit_vector.size == 0)
    {
        init_unit_vector();
    }

    for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
    {
        if (u != ch && IS_SET(UNIT_TYPE(u), type))
        {
            g_unit_vector.units[g_unit_vector.top++] = u;
            if (g_unit_vector.size == g_unit_vector.top)
            {
                double_unit_vector();
            }
        }

        /* down into transparent unit */
        if (UNIT_IS_TRANSPARENT(u))
        {
            for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
            {
                if (IS_SET(UNIT_TYPE(uu), type))
                {
                    g_unit_vector.units[g_unit_vector.top++] = uu;
                    if (g_unit_vector.size == g_unit_vector.top)
                    {
                        double_unit_vector();
                    }
                }
            }
        }
    }

    /* up through transparent unit */
    if (UNIT_IS_TRANSPARENT(UNIT_IN(ch)) && UNIT_IN(UNIT_IN(ch)))
    {
        for (u = UNIT_CONTAINS(UNIT_IN(UNIT_IN(ch))); u; u = u->next)
        {
            if (IS_SET(UNIT_TYPE(u), type))
            {
                g_unit_vector.units[g_unit_vector.top++] = u;
                if (g_unit_vector.size == g_unit_vector.top)
                {
                    double_unit_vector();
                }
            }

            /* down into transparent unit */
            if (UNIT_IS_TRANSPARENT(u) && u != UNIT_IN(ch))
            {
                for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                {
                    if (IS_SET(UNIT_TYPE(uu), type))
                    {
                        g_unit_vector.units[g_unit_vector.top++] = uu; /* MS FIX */
                        if (g_unit_vector.size == g_unit_vector.top)
                        {
                            double_unit_vector();
                        }
                    }
                }
            }
        }
    }
}

static unit_data *scan4_ref_room(unit_data *room, unit_data *fu)
{
    unit_data *u = nullptr;

    if (room == fu)
    {
        return fu;
    }

    for (u = UNIT_CONTAINS(room); u; u = u->next)
    {
        if (u == fu)
        {
            return fu;
        }
    }

    for (u = UNIT_CONTAINS(room); u; u = u->next)
    {
        if (((IS_ROOM(u) || IS_OBJ(u)) && (!UNIT_IS_TRANSPARENT(u))))
        {
            continue;
        }
        if (scan4_ref_room(u, fu))
        {
            return fu;
        }
    }

    return nullptr;
}

/* Scan the chars surroundings and all transparent surroundings for any   */
/* unit pointer occurance matching *fu. Return NULL or fu. Used only when */
/* you do not know wheter or not *fu has been extracted from the game. If */
/* you know that *fu exists, then a much simpler test is possible using   */
/* the 'same_surroundings()' function.                                     */
/* No checks for invisibility and the like                                */
unit_data *scan4_ref(unit_data *ch, unit_data *fu)
{
    if (!UNIT_IN(ch))
    {
        return (scan4_ref_room(ch, fu));
    }
    else if (((IS_ROOM(UNIT_IN(ch)) || IS_OBJ(UNIT_IN(ch))) && (!UNIT_IS_TRANSPARENT(UNIT_IN(ch)))))
    {
        return (scan4_ref_room(UNIT_IN(ch), fu));
    }
    else
    {
        return (scan4_ref(UNIT_IN(ch), fu));
    }
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
