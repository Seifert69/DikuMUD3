/*
 $Author: tperry $
 $RCSfile: mobact.cpp,v $
 $Date: 2002/07/01 01:30:58 $
 $Revision: 2.5 $
 */

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "structs.h"
#include "skills.h"
#include "db.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "utility.h"
#include "main.h"
#include "dilrun.h"
void special_event(void *p1, void *p2);

extern class zone_type *boot_zone;
extern struct unit_function_array_type unit_function_array[];
extern eventqueue events;
extern int mudboot;

void SetFptrTimer(class unit_data *u, class unit_fptr *fptr)
{
    ubit32 ticks;

    assert(!u->is_destructed());
    assert(!fptr->is_destructed());

    if ((ticks = fptr->heart_beat) > 0)
    {
        if (ticks < PULSE_SEC)
        {
            szonelog(boot_zone, "Error: %s@%s had heartbeat of %d.",
                     UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u), ticks);
            if ((fptr->index == SFUN_DILCOPY_INTERNAL) || (fptr->index == SFUN_DIL_INTERNAL))
            {
                class dilprg * p;
                p = (class dilprg *) fptr->data;

                if (p)
                {
                    szonelog(boot_zone, "DIL [%s] had heartbeat issue", p->fp->tmpl->prgname);
                }
            }
            ticks = fptr->heart_beat = PULSE_SEC * 3;
        }

        if (IS_SET(fptr->flags, SFB_RANTIME))
            ticks = number(ticks - ticks / 2, ticks + ticks / 2);
        if (fptr->event)
            events.remove(special_event, u, fptr);
        fptr->event = events.add(ticks, special_event, u, fptr);
        //      events.add(ticks, special_event, u, fptr);
        membug_verify(fptr);
        membug_verify(fptr->data);
    }
}

void ResetFptrTimer(class unit_data *u, class unit_fptr *fptr)
{
    membug_verify(u);
    membug_verify(fptr);
    membug_verify(fptr->data);

    events.remove(special_event, u, fptr);
    SetFptrTimer(u, fptr);

    membug_verify(fptr->data);
}

void special_event(void *p1, void *p2)
{
    class unit_data *u = (class unit_data *)p1;
    register class unit_fptr *fptr = (class unit_fptr *)p2;
    int priority;

    ubit32 ret = SFR_SHARE;
    class unit_fptr *ftmp;
    struct spec_arg sarg;

    void add_func_history(class unit_data * u, ubit16, ubit16);

    extern struct command_info cmd_auto_tick;

/*    if (fptr->index == SFUN_DIL_INTERNAL)
        if (fptr && fptr->data)
                if (strcmp(((class dilprg *) fptr->data)->fp->tmpl->prgname, "wander_zones")==0)
                {
                    if (strcmp(UNIT_FI_ZONENAME(u), "gnome") == 0)
                        slog(LOG_ALL, 0, "The wanderer!");
                } MS2020 debug */

    if (g_cServerConfig.m_bNoSpecials)
        return;

    if (!u)
        return;
    if (!fptr)
        return;
    if (u->is_destructed())
        return;
    if (fptr->is_destructed())
        return;
    if (fptr->event)
        fptr->event->func = NULL;
    fptr->event = NULL;
    priority = FALSE;
    for (ftmp = UNIT_FUNC(u); ftmp; ftmp = ftmp->next)
    {
        if (ftmp != fptr)
        {
            if (IS_SET(ftmp->flags, SFB_PRIORITY))
                priority = TRUE;
        }
    }
    if (!priority)
    /* If switched, disable all tick functions, so we can control
           the mother fucker!
           if (!IS_CHAR (u) || !CHAR_IS_SWITCHED (u)) */
    {
        if (unit_function_array[fptr->index].func)
        {
            if (IS_SET(fptr->flags, SFB_TICK))
            {
#ifdef DEBUG_HISTORY
                add_func_history(u, fptr->index, SFB_TICK);
#endif
                sarg.owner = u;
                sarg.activator = NULL;
                sarg.fptr = fptr;
                sarg.cmd = &cmd_auto_tick;
                sarg.arg = "";
                sarg.mflags = SFB_TICK;
                sarg.medium = NULL;
                sarg.target = NULL;
                sarg.pInt = NULL;

                ret = (*(unit_function_array[fptr->index].func))(&sarg);
            }
            assert((ret == SFR_SHARE) || (ret == SFR_BLOCK));
        }
        else
            slog(LOG_ALL, 0, "Null function call!");
    }
    else // Not executed because SFUN Before it raised priority 
    {
        if (fptr->index == SFUN_DIL_INTERNAL)
        {
            if (fptr->heart_beat == 1)
                fptr->heart_beat = 5 * PULSE_SEC;
        }
    }

    if (fptr->is_destructed())
        return;

    if (fptr->heart_beat < PULSE_SEC)
    {
        slog(LOG_ALL, 0,  "Error: %s@%s had heartbeat of %d.",
                    UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u), fptr->heart_beat);
    }

    if (fptr->index == SFUN_DIL_INTERNAL)
    {
        int diltick, i;
        diltick = FALSE;
        if (IS_SET(fptr->flags, SFB_TICK))
            diltick = TRUE;
        else if (fptr->data)
        {
            register class dilprg *prg = (class dilprg *)fptr->data;
            for (i = 0; i < prg->fp->intrcount; i++)
                if
                    IS_SET(prg->fp->intr[i].flags, SFB_TICK)
            diltick = TRUE;
        }
        if (!diltick)
            return;
    }

    if (!u->is_destructed() && !fptr->is_destructed())
        SetFptrTimer(u, fptr);
}

/* Return TRUE while stopping events */
void stop_special(class unit_data *u, class unit_fptr *fptr)
{
    events.remove(special_event, u, fptr);
}

void start_special(class unit_data *u, class unit_fptr *fptr)
{
    int diltick = 0, i;
    if (fptr->index == SFUN_DIL_INTERNAL)
    {
        if (IS_SET(fptr->flags, SFB_TICK))
            diltick = 1;
        else if (fptr->data)
        {
            register class dilprg *prg = (class dilprg *)fptr->data;
            for (i = 0; i < prg->fp->intrcount; i++)
                if (IS_SET(prg->fp->intr[i].flags, SFB_TICK))
                    diltick = 1;
        }
        if (!diltick)
            return;
    }

    if (IS_SET(fptr->flags, SFB_TICK) || fptr->index == SFUN_DIL_INTERNAL)
    {
        /* If people forget to set the ticking functions... */
        if (fptr->heart_beat <= 0)
        {
            fptr->heart_beat = unit_function_array[fptr->index].tick;

            /* Well, the builders are supposed to fix it! That's why it is
               sent to the log, so they can see it! */
            /* HUUUGE amount of log :(  /gnort */
            /* Now we default for the suckers... no need to warn any more
               szonelog(boot_zone,
               "%s@%s(%s): Heartbeat was 0 (idx %d), "
               "set to defualt: %d",
               UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u),
               UNIT_NAME(u), fptr->index,
               unit_function_array[fptr->index].tick); */
        }

        //      events.add(fptr->heart_beat, special_event, u, fptr);
        if (fptr->event)
            events.remove(special_event, u, fptr);
        
        if (!u->is_destructed() && !fptr->is_destructed())
            fptr->event = events.add(fptr->heart_beat, special_event, u, fptr);
    }
}

void start_all_special(class unit_data *u)
{
    class unit_fptr *fptr;

    for (fptr = UNIT_FUNC(u); fptr; fptr = fptr->next)
        start_special(u, fptr);
}

void stop_all_special(class unit_data *u)
{
    class unit_fptr *fptr;

    for (fptr = UNIT_FUNC(u); fptr; fptr = fptr->next)
        stop_special(u, fptr);
}
