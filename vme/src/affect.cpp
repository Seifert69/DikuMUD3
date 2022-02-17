/*
 $Author: All $
 $RCSfile: affect.cpp,v $
 $Date: 2004/05/16 04:34:34 $
 $Revision: 2.2 $
 */
#include "affect.h"

#include "apf_affect.h"
#include "main_functions.h"
#include "slog.h"
#include "structs.h"
#include "tif_affect.h"
#include "utility.h"
#include "utils.h"

unit_affected_type *affected_list = nullptr; /* Global list pointer       */
unit_affected_type *next_affected_dude;      /* dirty - very dirty indeed */

/* Link an affected structure into the units affected structure */
void link_affect(unit_data *unit, unit_affected_type *af)
{
    /*if (af->id > ID_TOP_IDX)
       error(HERE, "%s@%s (%s) linked affect ID %d > max value.",
       UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit),
       UNIT_NAME(unit), af->id); */

    af->gprevious = nullptr;

    if (affected_list)
    {
        af->gnext = affected_list;
        affected_list->gprevious = af;
    }
    else
    {
        af->gnext = nullptr;
    }

    affected_list = af;

    af->next = UNIT_AFFECTED(unit);
    UNIT_AFFECTED(unit) = af;
    af->owner = unit;
}

unit_affected_type *link_alloc_affect(unit_data *unit, unit_affected_type *orgaf)
{
    unit_affected_type *af = nullptr;

    af = new unit_affected_type;
    assert(!af->is_destructed());

    *af = *orgaf;

    link_affect(unit, af);

    return af;
}

/* Allocate memory, link and call functions of new affect   */
/* If the apf function returns TRUE then the tif - function */
/* is *not* called - but the structure is still alloced and */
/* linked.                                                  */
void create_affect(unit_data *unit, unit_affected_type *af)
{
    if (!unit->is_destructed())
    {
        af = link_alloc_affect(unit, af);
        af->event = nullptr;
        /* If less than zero it is a transfer! */
        if (af->getID() >= 0)
        {
            if (af->applyf_i >= 0)
            {
                if (!(*g_apf[af->applyf_i].func)(af, unit, TRUE))
                {
                    return;
                }
            }

            if (af->getFirstFI() >= 0)
            {
                (*g_tif[af->getFirstFI()].func)(af, unit);
            }

            // If there is no beat, decrease duration on creation
            if ((af->getBeat() <= 0) && (af->getDuration() > 0))
            {
                af->decrementDuration(); /* When 1 it means stop next tick... */
            }

            if (af->getBeat() > 0)
            {
                if (af->event)
                {
                    g_events.remove(affect_beat, (void *)af, nullptr);
                }
                af->event = g_events.add(af->getBeat(), affect_beat, (void *)af, nullptr);
            }
        }
        else
        {
            af->event = nullptr;
        }
    }
}

/* Unlink  an affect structure from lists        */
/* It is freed by 'clear_destruct' automatically */
void unlink_affect(unit_affected_type *af)
{
    unit_affected_type *i = nullptr;

    /* NB! Mucho importanto!                                    */
    /* Affects may never be removed by lower function than this */
    af->register_destruct();

    g_events.remove(affect_beat, (void *)af, nullptr);

    if (next_affected_dude == af)
    {
        next_affected_dude = af->gnext;
    }

    /* Unlink affect structure from global list of affects */

    if (affected_list == af)
    {
        affected_list = af->gnext;
    }

    if (af->gnext)
    {
        af->gnext->gprevious = af->gprevious;
    }

    if (af->gprevious)
    {
        af->gprevious->gnext = af->gnext;
    }

    /* Unlink affect structure from local list */

    i = UNIT_AFFECTED(af->owner);
    if (i)
    {
        if (i == af)
        {
            UNIT_AFFECTED(af->owner) = i->next;
        }
        else
        {
            for (; i->next != af; i = i->next)
            {
                ;
            }

            assert(i);
            i->next = af->next;
        }
    }
}

/* Call apply (unset), Unlink, and last function, then free structure */
/* If the apf funtion returns TRUE then the affect will neither be    */
/* Unliked nor freed nor will the tif funtion be called               */
void destroy_affect(unit_affected_type *af)
{
    /* It is assumed that none of these function calls can */
    /* destroy the affect.                                 */
    if (af->getID() >= 0)
    {
        if (af->applyf_i >= 0)
        {
            if (!(*g_apf[af->applyf_i].func)(af, af->owner, FALSE))
            {
                af->setDuration(0);
                af->setBeat(WAIT_SEC * 5);
                if (af->event)
                {
                    g_events.remove(affect_beat, (void *)af, nullptr);
                }
                af->event = g_events.add(number(120, 240), affect_beat, (void *)af, nullptr);
                return;
            }
        }

        if (af->getLastFI() >= 0 && !af->owner->is_destructed())
        {
            (*g_tif[af->getLastFI()].func)(af, af->owner);
        }
    }

    unlink_affect(af);
}

/* Attempts to clear a unit entirely of affects */
void affect_clear_unit(unit_data *unit)
{
    int i = 0;
    unit_affected_type *taf1 = nullptr;
    unit_affected_type *taf2 = nullptr;

    /* Some affects may not be destroyed at first attempt if it would */
    /* cause an overflow, therefore do several attemps to destroy     */
    for (i = 0; UNIT_AFFECTED(unit) && (i < 5); i++)
    {
        for (taf1 = UNIT_AFFECTED(unit); taf1; taf1 = taf2)
        {
            taf2 = taf1->next;
            destroy_affect(taf1);
        }
    }

    if (UNIT_AFFECTED(unit))
    {
        slog(LOG_ALL, 0, "ERROR: Could not clear unit of affects!");
    }
}

unit_affected_type *affected_by_spell(const unit_data *unit, sbit16 id)
{
    unit_affected_type *af = nullptr;

    for (af = UNIT_AFFECTED(unit); af; af = af->next)
    {
        if (af->getID() == id)
        {
            return af;
        }
    }

    return nullptr;
}

/* Called by event handler when its ticking time */
void affect_beat(void *p1, void *p2)
{
    unit_affected_type *af = (unit_affected_type *)p1;
    int destroyed = 0;

    assert(af->getID() >= 0); /* Negative ids (transfer) dont have beats */

    /* Used to be assert(af->beat > 0);  */
    /* But crashes game, I've set 0 to 8 */
    if (af->getBeat() <= 0)
    {
        af->setBeat(2 * WAIT_SEC);
    }

    destroyed = FALSE;

    if (!IS_PC(af->owner) || CHAR_DESCRIPTOR(af->owner))
    {
        if (af->getDuration() == 0)
        {
            /* 'destroy_affect' will try to destroy the event, but there */
            /* is none. It doesn't matter though                         */
            destroy_affect(af);
            return;
        }
        else
        {
            if (af->getTickFI() >= 0)
            {
                (*g_tif[af->getTickFI()].func)(af, af->owner);
            }

            destroyed = af->is_destructed();

            if (!destroyed && (af->getDuration() > 0))
            {
                af->decrementDuration();
            }
        }
    }
    if (!destroyed)
    {
        if (af->event)
        {
            g_events.remove(affect_beat, (void *)af, nullptr);
        }
        af->event = g_events.add(af->getBeat(), affect_beat, (void *)af, nullptr);
    }
}

/* ONLY USED WHEN LOADING UNITS                          */
/* If 'apply' is TRUE then apply function will be called */
void apply_affect(unit_data *unit)
{
    unit_affected_type *af = nullptr;

    /* If less than zero it is a transfer, and nothing will be set */
    for (af = UNIT_AFFECTED(unit); af; af = af->next)
    {
        if ((af->getID() >= 0) && (af->applyf_i >= 0))
        {
            if (!(*g_apf[af->applyf_i].func)(af, unit, TRUE))
            {
                continue;
            }
        }
    }
}

void start_affect(unit_data *unit)
{
    unit_affected_type *af = nullptr;

    /* If less than zero it is a transfer, and nothing will be set */
    for (af = UNIT_AFFECTED(unit); af; af = af->next)
    {
        if ((af->getID() >= 0) && (af->getBeat() > 0))
        {
            if (af->event)
            {
                g_events.remove(affect_beat, (void *)af, nullptr);
            }
            af->event = g_events.add(af->getBeat(), affect_beat, (void *)af, nullptr);
        }
        else
        {
            af->event = nullptr;
        }
    }
}

void stop_affect(unit_data *unit)
{
    unit_affected_type *af = nullptr;

    for (af = UNIT_AFFECTED(unit); af; af = af->next)
    {
        if (af->event != nullptr)
        {
            g_events.remove(affect_beat, (void *)af, nullptr);
        }
    }
}
