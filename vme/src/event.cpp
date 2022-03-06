/*
 $Author: All $
 $RCSfile: event.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.11 $
 */
#include "event.h"

#include "affect.h"
#include "comm.h"
#include "eliza.h"
#include "interpreter.h"
#include "main_functions.h"
#include "mobact.h"
#include "slog.h"
#include "spec_assign.h"
#include "structs.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "utils.h"
#include "weather.h"
#include "zone_reset.h"
#include "zone_type.h"

#include <sys/time.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

eventqueue::eventqueue()
{
    count = 0;
    heapsize = 0;
    heap = nullptr;
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

eventqueue::~eventqueue()
{
    int i = 0;
    for (i = 1; i <= count; i++)
    {
        delete heap[i];
    }
}

eventq_elem *eventqueue::add(int when, void (*func)(void *, void *), void *arg1, void *arg2)
{
    eventq_elem *end = nullptr;
    int parent_index = 0;
    int current_index = 0;

    if (when <= 0)
    {
        slog(LOG_ALL, 0, "Error: %d EVENT", when);
    }

    // One could add a sanity check to make sure that events for known
    if (func == special_event)
    {
        unit_data *u = (unit_data *)arg1;
        unit_fptr *f = (unit_fptr *)arg2;
        membug_verify(u);
        membug_verify(f);
        membug_verify(f->data);

        if (u)
        {
            assert(!u->is_destructed());
        }

        if (f)
        {
            assert(!f->is_destructed());
        }

        if (f->getFunctionPointerIndex() == 82)
        {
            dilprg *prg = (dilprg *)f->getData();
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
            CREATE(heap, eventq_elem *, heapsize);
        }
        else
        {
            heapsize = heapsize * 2;
            RECREATE(heap, eventq_elem *, heapsize);
            slog(LOG_DIL, 0, "HEAP expand to %d", heapsize);
        }
    }
    count++;
    end = new eventq_elem;
    end->when = g_tics + when;
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
        {
            break;
        }
    }
    heap[current_index] = end;

    return end;
}

void eventqueue::remove(void (*func)(void *, void *), void *arg1, void *arg2)
{
    int i = 0;
    if ((func == special_event) && arg2 && (((unit_fptr *)arg2)->getEventQueue()))
    {
        ((unit_fptr *)arg2)->getEventQueue()->func = nullptr;
        ((unit_fptr *)arg2)->setEventQueue(nullptr);
    }
    else if ((func == affect_beat) && arg1 && (((unit_affected_type *)arg1)->cgetEventQueueElement()))
    {
        ((unit_affected_type *)arg1)->getEventQueueElement()->func = nullptr;
        ((unit_affected_type *)arg1)->setEventQueueElement(nullptr);
    }
    else if ((func == affect_beat) && arg1 && (!((unit_affected_type *)arg1)->cgetEventQueueElement()))
    {
        return;
    }
    else if ((func == special_event) && arg2 && (!((unit_fptr *)arg2)->getEventQueue()))
    {
        return;
    }
    else
    {
        for (i = 1; i <= count; i++)
        {
            if (heap[i]->func == func && heap[i]->arg1 == arg1 && heap[i]->arg2 == arg2)
            {
                heap[i]->func = nullptr;
            }
        }
    }
}

void eventqueue::remove_relaxed(void (*func)(void *, void *), void *arg1, void *arg2)
{
    int i = 0;

    for (i = 1; i <= count; i++)
    {
        if ((heap[i]->func == func) && (!arg1 || (heap[i]->arg1 == arg1)) && (!arg2 || (heap[i]->arg2 == arg2)))
        {
            heap[i]->func = nullptr;
        }
    }
}

void eventqueue::process()
{
    eventq_elem *tmp_event = nullptr;
    eventq_elem *newtop = nullptr;
    int j = 0;
    int k = 0;
    char dilname[256];
    char dilzname[256];
    char diloname[256];
    char dilozname[256];
    timeval now;
    timeval old;
    timeval pnow;
    ubit32 us = 0;
    void (*tfunc)(void *, void *) = nullptr;
    loop_process = 0;
    gettimeofday(&old, nullptr);

    while ((count >= 1) && (heap[1]->when <= g_tics))
    {
        gettimeofday(&now, nullptr);
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
        heap[count] = nullptr;
        count--;
        newtop = heap[1];
        j = 1;
        k = 2 * j;
        while (k <= count)
        {
            if (k < count)
            {
                if (heap[k]->when > heap[k + 1]->when)
                {
                    k++;
                }
            }
            if (newtop->when > heap[k]->when)
            {
                heap[j] = heap[k];
                j = k;
                k = 2 * j;
            }
            else
            {
                break;
            }
        }
        heap[j] = newtop;
        if (tmp_event->func)
        {
            tfunc = tmp_event->func;
            int bDestructed = 0;

            if (tfunc == special_event && ((unit_fptr *)tmp_event->arg2)->getData() &&
                (((unit_fptr *)tmp_event->arg2)->getFunctionPointerIndex() == SFUN_DIL_INTERNAL))
            {
                strcpy(dilname, "NO NAME");
                strcpy(dilzname, "NO ZONE");
                strcpy(diloname, "NO NAME");
                strcpy(dilozname, "NO ZONE");

                unit_data *u = (unit_data *)tmp_event->arg1;
                unit_fptr *fptr = (unit_fptr *)tmp_event->arg2;

                membug_verify(u);
                membug_verify(fptr);

                bDestructed = (u->is_destructed() || fptr->is_destructed() || (tmp_event->func == nullptr));

                if (!bDestructed)
                {
                    dilprg *prg = (dilprg *)fptr->getData();

                    assert(prg);
                    membug_verify(prg);

                    assert(prg->owner);
                    assert(prg->fp);

                    // One could check here if the DIL (if it is) is supposed to run
                    // if (prg->waitcmd > WAITCMD_FINISH) bDestructed = 1;
                    // But rundil checks too.

                    if (prg->fp->tmpl->prgname)
                    {
                        strcpy(dilname, prg->fp->tmpl->prgname);
                    }
                    if (prg->fp->tmpl->zone)
                    {
                        strcpy(dilzname, prg->fp->tmpl->zone->getName());
                    }
                    strcpy(diloname, tmp_event->arg1 ? UNIT_FI_NAME((unit_data *)(tmp_event->arg1)) : "NO NAME");
                    strcpy(dilozname, tmp_event->arg1 ? UNIT_FI_ZONENAME((unit_data *)(tmp_event->arg1)) : "NO ZONE");
                }
            }

            if (!bDestructed)
            {
                assert(tmp_event->func);
                (tmp_event->func)(tmp_event->arg1, tmp_event->arg2);
                loop_process++;
                total_process++;
                if (max_process < loop_process)
                {
                    max_process_time = loop_time;
                    max_process = loop_process;
                }

                gettimeofday(&pnow, nullptr);
                us = (pnow.tv_sec - now.tv_sec) * 1000000L + (pnow.tv_usec - now.tv_usec);
                loop_time = us / 1000000.0;

                if (loop_time > .1)
                {
                    if (tfunc == special_event)
                    {
                        if (((unit_fptr *)tmp_event->arg2)->getFunctionPointerIndex() == SFUN_DIL_INTERNAL)
                        {
                            slog(LOG_DIL,
                                 0,
                                 "Process took %1.4f seconds to complete: %s@%s on %s@%s - %s (%d)'",
                                 loop_time,
                                 dilname,
                                 dilzname,
                                 diloname,
                                 dilozname,
                                 g_unit_function_array[((unit_fptr *)tmp_event->arg2)->getFunctionPointerIndex()].name,
                                 ((unit_fptr *)tmp_event->arg2)->getFunctionPointerIndex());
                        }
                        else
                        {
                            slog(LOG_DIL,
                                 0,
                                 "Internal process took %1.4f seconds to complete: %s (%d)'",
                                 loop_time,
                                 g_unit_function_array[((unit_fptr *)tmp_event->arg2)->getFunctionPointerIndex()].name,
                                 ((unit_fptr *)tmp_event->arg2)->getFunctionPointerIndex());
                        }
                    }
                    else
                    {
                        std::string pname;
                        if (tfunc == check_reboot_event)
                        {
                            pname = "Reboot Event";
                        }
                        else if (tfunc == affect_beat)
                        {
                            pname = "Affect Beat";
                        }
                        else if (tfunc == delayed_action)
                        {
                            pname = "Affect Beat";
                        }
                        else if (tfunc == check_idle_event)
                        {
                            pname = "Check Idle Event";
                        }
                        else if (tfunc == perform_violence_event)
                        {
                            pname = "Violence Event";
                        }
                        else if (tfunc == weather_and_time_event)
                        {
                            pname = "Weather And Time Event";
                        }
                        else if (tfunc == zone_event)
                        {
                            pname = "Zone Reset Event";
                        }
                        else
                        {
                            pname = "UNKNOWN Event";
                        }
                        slog(LOG_DIL, 0, "Internal Process (%s) Took %1.4f seconds to Complete", pname, loop_time);
                    }
                }
            } // !bDestructed
        }
        delete tmp_event;
        tmp_event = nullptr;
    }

    gettimeofday(&now, nullptr);
    us = (now.tv_sec - old.tv_sec) * 1000000L + (now.tv_usec - old.tv_usec);
    loop_time = us / 1000000.0;

    total_loops++;
    total_time += loop_time;
}
