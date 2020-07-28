/*
 $Author: All $
 $RCSfile: affect.cpp,v $
 $Date: 2004/05/16 04:34:34 $
 $Revision: 2.2 $
 */

#include <stdlib.h>
#include <stdio.h>

#include "structs.h"
#include "utils.h"
#include "affect.h"
#include "utility.h"
#include "main.h"

extern struct apply_function_type apf[];
extern struct tick_function_type tif[];

class unit_affected_type *affected_list = 0;	/* Global list pointer       */
class unit_affected_type *next_affected_dude;	/* dirty - very dirty indeed */
extern eventqueue events;

void register_destruct (int i, void *ptr);
void clear_destruct (int i);

void affect_beat (void *, void *);


/* Link an affected structure into the units affected structure */
void
link_affect (class unit_data * unit, class unit_affected_type *af)
{
    /*if (af->id > ID_TOP_IDX)
       error(HERE, "%s@%s (%s) linked affect ID %d > max value.",
       UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit),
       UNIT_NAME(unit), af->id); */

    af->gprevious = 0;

    if (affected_list)
    {
        af->gnext = affected_list;
        affected_list->gprevious = af;
    }
    else
    {
        af->gnext = 0;
    }

    affected_list = af;

    af->next = UNIT_AFFECTED (unit);
    UNIT_AFFECTED (unit) = af;
    af->owner = unit;
}



class unit_affected_type *link_alloc_affect (class unit_data * unit, class unit_affected_type *orgaf)
{
    class unit_affected_type *af;

    af = new unit_affected_type;
    assert (!af->is_destructed());

    *af = *orgaf;

    link_affect (unit, af);

    return af;
}



/* Allocate memory, link and call functions of new affect   */
/* If the apf function returns TRUE then the tif - function */
/* is *not* called - but the structure is still alloced and */
/* linked.                                                  */
void
create_affect (class unit_data * unit, class unit_affected_type *af)
{
    if (!unit->is_destructed())
    {
        af = link_alloc_affect (unit, af);
        af->event = NULL;
        /* If less than zero it is a transfer! */
        if (af->id >= 0)
        {
            if (af->applyf_i >= 0)
                if (!(*apf[af->applyf_i].func) (af, unit, TRUE))
                    return;

            if (af->firstf_i >= 0)
                (*tif[af->firstf_i].func) (af, unit);

            if (af->duration > 0)
                af->duration--;	/* When 1 it means stop next tick... */

            if (af->beat > 0)
            {
                if (af->event)
                    events.remove (affect_beat, (void *) af, 0);
                af->event = events.add (af->beat, affect_beat, (void *) af, 0);
            }
        }
        else
            af->event = NULL;
    }
}



/* Unlink  an affect structure from lists        */
/* It is freed by 'clear_destruct' automatically */
void unlink_affect(class unit_affected_type *af)
{
    class unit_affected_type *i;

    /* NB! Mucho importanto!                                    */
    /* Affects may never be removed by lower function than this */
    af->register_destruct();

    events.remove (affect_beat, (void *) af, 0);

    if (next_affected_dude == af)
        next_affected_dude = af->gnext;

    /* Unlink affect structure from global list of affects */

    if (affected_list == af)
        affected_list = af->gnext;

    if (af->gnext)
        af->gnext->gprevious = af->gprevious;

    if (af->gprevious)
        af->gprevious->gnext = af->gnext;


    /* Unlink affect structure from local list */

    i = UNIT_AFFECTED (af->owner);
    if (i)
    {
        if (i == af)
        {
            UNIT_AFFECTED (af->owner) = i->next;
        }
        else
        {
            for (; i->next != af; i = i->next);

            assert (i);
            i->next = af->next;
        }
    }
}



/* Call apply (unset), Unlink, and last function, then free structure */
/* If the apf funtion returns TRUE then the affect will neither be    */
/* Unliked nor freed nor will the tif funtion be called               */
void
destroy_affect (class unit_affected_type *af)
{
    /* It is assumed that none of these function calls can */
    /* destroy the affect.                                 */
    if (af->id >= 0)
    {
        if (af->applyf_i >= 0)
            if (!(*apf[af->applyf_i].func) (af, af->owner, FALSE))
            {
                af->duration = 0;
                af->beat = WAIT_SEC * 5;
                if (af->event)
                    events.remove (affect_beat, (void *) af, 0);
                af->event =
                    events.add (number (120, 240), affect_beat, (void *) af, 0);
                return;
            }

        if (af->lastf_i >= 0 && !af->owner->is_destructed())
            (*tif[af->lastf_i].func) (af, af->owner);
    }

    unlink_affect (af);
}


/* Attempts to clear a unit entirely of affects */
void
affect_clear_unit (class unit_data * unit)
{
    int i;
    class unit_affected_type *taf1, *taf2;

    /* Some affects may not be destroyed at first attempt if it would */
    /* cause an overflow, therefore do several attemps to destroy     */
    for (i = 0; UNIT_AFFECTED (unit) && (i < 5); i++)
    {
        for (taf1 = UNIT_AFFECTED (unit); taf1; taf1 = taf2)
        {
            taf2 = taf1->next;
            destroy_affect (taf1);
        }
    }

    if (UNIT_AFFECTED (unit))
        slog (LOG_ALL, 0, "ERROR: Could not clear unit of affects!");
}


class unit_affected_type *
affected_by_spell (const class unit_data * unit, sbit16 id)
{
    class unit_affected_type *af;

    for (af = UNIT_AFFECTED (unit); af; af = af->next)
        if (af->id == id)
            return af;

    return 0;
}



/* Called by event handler when its ticking time */
void
affect_beat (void *p1, void *p2)
{
    register class unit_affected_type *af = (class unit_affected_type *) p1;
    int destroyed;


    assert (af->id >= 0);		/* Negative ids (transfer) dont have beats */


    /* Used to be assert(af->beat > 0);  */
    /* But crashes game, I've set 0 to 8 */
    if (af->beat <= 0)
        af->beat = 2 * WAIT_SEC;

    destroyed = FALSE;

    if (!IS_PC (af->owner) || CHAR_DESCRIPTOR (af->owner))
    {
        if (af->duration == 0)
        {
            /* 'destroy_affect' will try to destroy the event, but there */
            /* is none. It doesn't matter though                         */
            destroy_affect (af);
            return;
        }
        else
        {
            if (af->tickf_i >= 0)
                (*tif[af->tickf_i].func) (af, af->owner);

            destroyed = af->is_destructed();

            if (!destroyed && (af->duration > 0))
                af->duration--;
        }
    }
    if (!destroyed)
    {
        if (af->event)
            events.remove (affect_beat, (void *) af, 0);
        af->event = events.add (af->beat, affect_beat, (void *) af, 0);
    }
}



/* ONLY USED WHEN LOADING UNITS                          */
/* If 'apply' is TRUE then apply function will be called */
void
apply_affect (class unit_data * unit)
{
    class unit_affected_type *af;

    /* If less than zero it is a transfer, and nothing will be set */
    for (af = UNIT_AFFECTED (unit); af; af = af->next)
        if ((af->id >= 0) && (af->applyf_i >= 0))
        {
            if (!(*apf[af->applyf_i].func) (af, unit, TRUE))
                continue;
        }
}


void
start_affect (class unit_data * unit)
{
    class unit_affected_type *af;

    /* If less than zero it is a transfer, and nothing will be set */
    for (af = UNIT_AFFECTED (unit); af; af = af->next)
        if ((af->id >= 0) && (af->beat > 0))
        {
            if (af->event)
                events.remove (affect_beat, (void *) af, 0);
            af->event = events.add (af->beat, affect_beat, (void *) af, 0);
        }
        else
            af->event = NULL;
}


void
stop_affect (class unit_data * unit)
{
    class unit_affected_type *af;

    for (af = UNIT_AFFECTED (unit); af; af = af->next)
        if (af->event != NULL)
            events.remove (affect_beat, (void *) af, 0);
}
