/*
 $Author: All $
 $RCSfile: destruct.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.2 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <type_traits>

#include "structs.h"
#include "utils.h"
#include "textutil.h"
#include "skills.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "affect.h"
#include "utility.h"
#include "destruct.h"
#include "event.h"

/*
   This method is possible since we know that the only problem occurs when:

      An event triggers affect_beat or special_event.

      During the call from either of these to some other routine - the
      unit may be destructed together with its original fptr/affect.

      Now the event routine request's a new event because it doesn't know
      that the affect/func was destructed.

   We can use this method to determine if a unit has been destructed.
   All destructed units, affects and funcs are registered and can
   be tested for - for example by 'is_destructed(, ch)'.

   All registered destructed 'things' are cleared each time the
   program reaches 'top level' (game loop).

   Thus these routines are to be used in the following way:

      in affect_beat and special_event we must test if the
      unit was destructed, if so a new event is not reordered.

      in special routines, skills and ticks we can test if
      the person has been killed by using (example wiz command 'at'):

	 unit_from_unit(ch);
	 unit_to_unit(ch, dest_room);
	 command_interpreter(ch, arg);
	 if (is_destructed(DR_UNIT, ch))
	    return;
	 unit_from_unit(ch);
	 unit_to_unit(ch, org_room);
*/


/* Registered by affect.c low level destruct affect */
/* (unlink affect). Used only by affect_beat.       */
/* Registered by destroy fptr */
/* Used only by special_event */
/* Registered by extract_unit */
/* Used lots of places        */

/* 3 Arrays of registered destructed things     */
void **destructed[3] = { 0, 0, 0 };

/* 3 Arrays which indicate size of above arrays */
int destructed_top[3] = { -1, -1, -1 };

/* 3 Arrays which point to first free entry     */
int destructed_idx[3] = { -1, -1, -1 };

void destruct_resize (int i)
{
    if (destructed_top[i] == -1)
    {
        CREATE (destructed[i], void *, 1000);
        destructed_top[i] = 1000;
        destructed_idx[i] = 0;
    }
    else
    {
        RECREATE (destructed[i], void *, 1000 + destructed_top[i]);
        destructed_top[i] += 1000;
    }
}



/* ======= Destruct class ========== */ 

basedestruct::basedestruct(void)
{
    m_bDestructed = 0;
}

basedestruct::~basedestruct(void)
{  
    //assert(m_bDestructed);
}

void basedestruct::register_destruct(void)
{
    assert(this);
    assert(!m_bDestructed);
    m_bDestructed = 1;

    //if (is_destructed())
    //    return;

    int i = destruct_classindex();

    if (destructed_idx[i] >= destructed_top[i])
        destruct_resize(i);

    destructed[i][destructed_idx[i]] = this;
    destructed_idx[i]++;
}

int basedestruct::is_destructed(void)
{
    return (m_bDestructed != 0);
}

void basedestruct::undo_destruct(void)
{
    m_bDestructed = 0;
}

int basedestruct::destruct_classindex(void)
{
    assert(FALSE);
    return -1;
}

int unit_data::destruct_classindex(void)
{
    return DR_UNIT;
}

int unit_fptr::destruct_classindex(void)
{
    return DR_FUNC;
}

int unit_affected_type::destruct_classindex(void)
{
    return DR_AFFECT;
}


/* ======================================= */

/* May only be called by clear_destuct! */
void destruct_unit(class unit_data * unit)
{
#ifdef DMSERVER
    class descriptor_data *d;
    int in_menu = FALSE;
    if (!unit)
        return;

    extern class descriptor_data *descriptor_list;
    extern class unit_data *unit_list;

    void stop_all_special (class unit_data * u);
    void unswitchbody (class unit_data * npc);
    void unsnoop (class unit_data * ch, int mode);
    void die_follower (class unit_data * ch);
    void stop_fighting (class unit_data * ch);
    void unlink_affect (class unit_affected_type *af);
    void nanny_menu (class descriptor_data *d, char *arg);
    void nanny_close (class descriptor_data *d, char *arg);

    void do_return (class unit_data * ch, char *arg, struct command_info *cmd);

    if (!IS_PC(unit))
    {
        stop_all_special(unit); // MS2020 reactivated, this is imperative to avoid crash from obsoleted event q events
        stop_affect(unit);      // Reactivated
    }

    /* Remove all snooping, snoopers and return from any body */
    if (IS_CHAR(unit))
    {
        if (CHAR_DESCRIPTOR(unit))
        {
            assert (IS_PC(unit));

            in_menu = TRUE;

            extern int dilmenu;
            if (UNIT_IN (unit) && !dilmenu)
            {
                set_descriptor_fptr (CHAR_DESCRIPTOR (unit), nanny_menu, TRUE);
                unit->undo_destruct();
            }
            else
            {
                set_descriptor_fptr (CHAR_DESCRIPTOR (unit), nanny_close, TRUE);
                in_menu = FALSE;
            }
        }

        assert (!CHAR_IS_SWITCHED (unit));
        assert (!CHAR_IS_SNOOPING (unit));
        assert (!CHAR_IS_SNOOPED (unit));

        /* If the PC which is switched is extracted, then unswitch */
        if (IS_PC (unit) && !CHAR_DESCRIPTOR (unit))
            for (d = descriptor_list; d; d = d->next)
                assert (d->original != unit);

        assert (!CHAR_FOLLOWERS (unit));
        assert (!CHAR_MASTER (unit));
        assert (!CHAR_COMBAT (unit));
    }
    else if (!IS_OBJ (unit))
    {
        slog (LOG_OFF, 0, "Extract on something not a char or an obj.");
        assert (FALSE);
    }

    while (UNIT_CONTAINS (unit))
    {
        if (IS_OBJ (UNIT_CONTAINS (unit)) && OBJ_EQP_POS (UNIT_CONTAINS (unit)))
            unequip_object (UNIT_CONTAINS (unit));
        destruct_unit (UNIT_CONTAINS (unit));
    }

    if (!in_menu)
    {
        /* Call functions of the unit which have any data                     */
        /* that they might want to work on.                                   */
        while (UNIT_FUNC (unit))
            destroy_fptr (unit, UNIT_FUNC (unit));	/* Unlinks, no free */

        while (UNIT_AFFECTED (unit))
            unlink_affect (UNIT_AFFECTED (unit));
    }

    if (UNIT_IN (unit))
        unit_from_unit (unit);

    if (UNIT_FILE_INDEX (unit))
        UNIT_FILE_INDEX (unit)->no_in_mem--;

    if ((unit_list == unit) || unit->gnext || unit->gprevious)
        remove_from_unit_list (unit);

    if (!in_menu)
    {
        DELETE(unit_data, unit);
        unit = NULL;
    }
#endif
}


/* May only be called from comm.c event loop */
void clear_destructed (void)
{
    class unit_fptr *f;
    int i;

    extern void special_event(void *p1, void *p2);
    extern eventqueue events;

    for (i = 0; i < destructed_idx[DR_AFFECT]; i++)
        delete (class unit_affected_type *) destructed[DR_AFFECT][i];

    destructed_idx[DR_AFFECT] = 0;

    for (i = 0; i < destructed_idx[DR_FUNC]; i++)
    {
        f = (class unit_fptr *) destructed[DR_FUNC][i];

        if (f->index == 82)
            assert(f->data == NULL);

        assert(f->event == NULL);

        if (f->data)
            FREE(f->data);

        DELETE(unit_fptr, f);
        destructed[DR_FUNC][i] = NULL;
    }
    destructed_idx[DR_FUNC] = 0;

    for (i = 0; i < destructed_idx[DR_UNIT]; i++)
    {
        if ((class unit_data *) destructed[DR_UNIT][i])
        {
            destruct_unit((class unit_data *) destructed[DR_UNIT][i]);            
            destructed[DR_UNIT][i] = NULL;
        }
    }
    destructed_idx[DR_UNIT] = 0;
}


/*
int is_destructed (int i, void *ptr)
{
    if (ptr == NULL)
        return TRUE;

    switch (i)
    {
    case DR_AFFECT:
        return ((class unit_affected_type *) ptr)->destructed;
        break;
    case DR_UNIT:
        return ((class unit_data *) ptr)->destructed;
        break;
    case DR_FUNC:
        return ((class unit_fptr *) ptr)->destructed;
        break;
    }

    return TRUE;
    / *   for (n=0; n < destructed_idx[i]; n++)
          if (destructed[i][n] == ptr)
    	 return TRUE;

       return FALSE;* /
}
*/