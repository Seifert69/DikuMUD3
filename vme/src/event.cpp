/*
 $Author: All $
 $RCSfile: event.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.11 $
 */

#ifdef _WINDOWS

#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "structs.h"
#include "utils.h"
#include "utility.h"
#include "handler.h"
#include "system.h"
#include "comm.h"
#include "db.h"
#include "interpreter.h"
#include "main.h"
#include "textutil.h"
#include "files.h"
#include "event.h"
#include "dilrun.h"
#include <sys/time.h>
void check_reboot_event(void *, void *);
void delayed_action(void *p1, void *p2);
void check_idle_event(void *p1, void *p2);
void perform_violence_event(void *, void *);
void weather_and_time_event(void *, void *);
void zone_event(void *, void *);

extern void special_event(void *p1, void *p2);
extern void affect_beat(void *p1, void *pd);
extern struct unit_function_array_type unit_function_array[];
extern int tics;

eventqueue::eventqueue(void)
{
    count = 0;
    heapsize = 0;
    heap = NULL;
    loop_process = 0;
    loop_time = 0;
    max_process = 0;
    max_process_time = 0;
    max_loop_time_process = 0;
    max_loop_time = 0;
    total_loops = 0;
    total_process = 0;
    total_time = 0;
}

eventqueue::~eventqueue(void)
{
    int i;
    for (i = 1; i <= count; i++)
    {
        delete heap[i];
    }
}

struct eventq_elem *eventqueue::add(int when, void (*func)(void *, void *),
                                     void *arg1, void *arg2)
{
    struct eventq_elem *end;
    int parent_index, current_index;

    if (when <= 0)
    {
        slog(LOG_ALL, 0, "Error: %d EVENT", when);
    }

    // One could add a sanity check to make sure that events for known 
    if (func == special_event)
    {
        struct unit_data *u = (struct unit_data *) arg1;
        struct unit_fptr *f = (struct unit_fptr *) arg2;
        membug_verify(u);
        membug_verify(f);
        membug_verify(f->data);

        if (u)
            assert(!u->is_destructed());

        if (f)
            assert(!f->is_destructed());

        if (f->index == 82)
        {
            struct dilprg *prg = (struct dilprg *) f->data;
            membug_verify(prg);

            assert(prg);
            assert(prg->owner == u);
        }
    }

    if ((count + 10) > heapsize)
    {
        if (!heapsize)
        {
            heapsize = 5000;
            CREATE(heap, struct eventq_elem *, heapsize);
        }
        else
        {
            heapsize = heapsize * 2;
            RECREATE(heap, struct eventq_elem *, heapsize);
            slog(LOG_DIL, 0, "HEAP expand to %d", heapsize);
        }
    }
    count++;
    end = new eventq_elem;
    end->when = tics + when;
    end->func = func;
    end->arg1 = arg1;
    end->arg2 = arg2;
    /* roll event into its proper place in da heap */
    parent_index = count / 2;
    current_index = count;

    while (parent_index > 0)
    {
        if (end->when < heap[parent_index]->when)
        {
            heap[current_index] = heap[parent_index];
            current_index = parent_index;
            parent_index = current_index / 2;
        }
        else
            break;
    }
    heap[current_index] = end;

    return end;
}

void eventqueue::remove(void (*func)(void *, void *), void *arg1, void *arg2)
{
    int i;
    if ((func == special_event) && arg2 && (((class unit_fptr *)arg2)->event))
    {
        ((class unit_fptr *)arg2)->event->func = NULL;
        ((class unit_fptr *)arg2)->event = NULL;

    }
    else if ((func == affect_beat) &&
             arg1 && (((class unit_affected_type *)arg1)->event))
    {
        ((class unit_affected_type *)arg1)->event->func = NULL;
        ((class unit_affected_type *)arg1)->event = NULL;
    }
    else if ((func == affect_beat) &&
             arg1 && (!((class unit_affected_type *)arg1)->event))
    {
        return;
    }
    else if ((func == special_event) &&
             arg2 && (!((class unit_fptr *)arg2)->event))
    {
        return;
    }
    else
    {
        for (i = 1; i <= count; i++)
            if (heap[i]->func == func && heap[i]->arg1 == arg1 &&
                heap[i]->arg2 == arg2)
                heap[i]->func = NULL;
    }
}

void eventqueue::remove_relaxed(void (*func)(void *, void *), void *arg1,
                                void *arg2)
{
    int i;

    for (i = 1; i <= count; i++)
        if ((heap[i]->func == func) && (!arg1 || (heap[i]->arg1 == arg1)) && (!arg2 || (heap[i]->arg2 == arg2)))
            heap[i]->func = 0;
}

void eventqueue::process(void)
{
    struct eventq_elem *tmp_event, *newtop;
    int j, k;
    char pname[64];
    char dilname[256];
    char dilzname[256];
    char diloname[256];
    char dilozname[256];
    struct timeval now, old, pnow;
    ubit32 us;
    void (*tfunc)(void *, void *);
    loop_process = 0;
    gettimeofday(&old, (struct timezone *)0);

    while ((count >= 1) && (heap[1]->when <= tics))
    {
        gettimeofday(&now, (struct timezone *)0);
        us = (now.tv_sec - old.tv_sec) * 1000000L + (now.tv_usec - old.tv_usec);
        loop_time = us / 1000000.0;

        if (max_loop_time < loop_time)
        {
            max_loop_time = loop_time;
            max_loop_time_process = loop_process;
        }
        if (loop_time > .20)
        {
            total_loops++;
            total_time += loop_time;
            return;
        }
        /* dequeue & process event */
        tmp_event = heap[1];
        heap[1] = heap[count];
        heap[count] = NULL;
        count--;
        newtop = heap[1];
        j = 1;
        k = 2 * j;
        while (k <= count)
        {
            if (k < count)
                if (heap[k]->when > heap[k + 1]->when)
                    k++;
            if (newtop->when > heap[k]->when)
            {
                heap[j] = heap[k];
                j = k;
                k = 2 * j;
            }
            else
                break;
        }
        heap[j] = newtop;
        if (tmp_event->func)
        {
            tfunc = tmp_event->func;
            int bDestructed = 0;

            if (tfunc == special_event && ((unit_fptr *)tmp_event->arg2)->data &&
                (((class unit_fptr *)tmp_event->arg2)->index == SFUN_DIL_INTERNAL))
            {
                sprintf(dilname, "NO NAME");
                sprintf(dilzname, "NO ZONE");
                sprintf(diloname, "NO NAME");
                sprintf(dilozname, "NO ZONE");

                class unit_data *u = (unit_data *)tmp_event->arg1;
                class unit_fptr *fptr = (unit_fptr *)tmp_event->arg2;

                membug_verify(u);
                membug_verify(fptr);

                bDestructed = (u->is_destructed() || fptr->is_destructed());

                if (!bDestructed)
                {
                    class dilprg *prg = (class dilprg *) fptr->data;

                    assert(prg);
                    membug_verify(prg);

                    assert(prg->owner);
                    assert(prg->fp);

                    // One could check here if the DIL (if it is) is supposed to run
                    // if (prg->waitcmd > WAITCMD_FINISH) bDestructed = 1;
                    // But rundil checks too.

                    if (prg->fp->tmpl->prgname)
                        sprintf(dilname, "%s",
                                prg->fp->tmpl->prgname);
                    if (prg->fp->tmpl->zone)
                        sprintf(dilzname, "%s",
                                prg->fp->tmpl->zone->name);
                    sprintf(diloname, "%s", tmp_event->arg1 ? UNIT_FI_NAME((class unit_data *)(tmp_event->arg1)) : "NO NAME");
                    sprintf(dilozname, "%s", tmp_event->arg1 ? UNIT_FI_ZONENAME((class unit_data *)(tmp_event->arg1)) : "NO ZONE");
                }
            }

            if (!bDestructed)
            {
                (tmp_event->func)(tmp_event->arg1, tmp_event->arg2);
                loop_process++;
                total_process++;
                if (max_process < loop_process)
                {
                    max_process_time = loop_time;
                    max_process = loop_process;
                }

                gettimeofday(&pnow, (struct timezone *)0);
                us = (pnow.tv_sec - now.tv_sec) * 1000000L + (pnow.tv_usec - now.tv_usec);
                loop_time = us / 1000000.0;

                if (loop_time > .1)
                {
                    if (tfunc == special_event)
                    {
                        if (((class unit_fptr *)tmp_event->arg2)->index == SFUN_DIL_INTERNAL)
                            slog(LOG_DIL, 0, "Process took %1.4f seconds to complete: %s@%s on %s@%s - %s (%d)'", loop_time, dilname, dilzname, diloname, dilozname, unit_function_array[((class unit_fptr *)tmp_event->arg2)->index].name, ((class unit_fptr *)tmp_event->arg2)->index);

                        else
                            slog(LOG_DIL, 0, "Internal process took %1.4f seconds to complete: %s (%d)'",
                                loop_time, unit_function_array[((class unit_fptr *)tmp_event->arg2)->index].name, ((class unit_fptr *)tmp_event->arg2)->index);
                    }
                    else
                    {
                        if (tfunc == check_reboot_event)
                            sprintf(pname, "Reboot Event");
                        else if (tfunc == affect_beat)
                            sprintf(pname, "Affect Beat");
                        else if (tfunc == delayed_action)
                            sprintf(pname, "Affect Beat");
                        else if (tfunc == check_idle_event)
                            sprintf(pname, "Check Idle Event");
                        else if (tfunc == perform_violence_event)
                            sprintf(pname, "Violence Event");
                        else if (tfunc == weather_and_time_event)
                            sprintf(pname, "Weather And Time Event");
                        else if (tfunc == zone_event)
                            sprintf(pname, "Zone Reset Event");
                        else
                            sprintf(pname, "UNKNOWN Event");
                        slog(LOG_DIL, 0, "Internal Process (%s) Took %1.4f seconds to Complete", pname, loop_time);
                    }
                }
            }  // !bDestructed
        }
        delete tmp_event;
        tmp_event = NULL;
    }

    gettimeofday(&now, (struct timezone *)0);
    us = (now.tv_sec - old.tv_sec) * 1000000L + (now.tv_usec - old.tv_usec);
    loop_time = us / 1000000.0;

    total_loops++;
    total_time += loop_time;
}
