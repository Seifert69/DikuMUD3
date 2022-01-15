/*
 $Author: All $
 $RCSfile: main.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.10 $
 */
#include "main_functions.h"

#include "comm.h"
#include "compile_defines.h"
#include "db.h"
#include "dilrun.h"
#include "files.h"
#include "handler.h"
#include "hookmud.h"
#include "interpreter.h"
#include "nanny.h"
#include "nice.h"
#include "path.h"
#include "pcsave.h"
#include "pipe.h"
#include "signals.h"
#include "structs.h"
#include "system.h"
#include "utils.h"

#include <pthread.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define OPT_USEC 250000L     /* time delay corresponding to 4 passes/sec */
long g_nTickUsec = OPT_USEC; // Dont mess with this, it's the game heartbeat. Look at 'timewarp'

#define HEAPSPACE_INCREMENT 500
/* constants */
eventqueue g_events;
class descriptor_data *g_descriptor_list = NULL;
class descriptor_data *g_next_to_process = NULL;

/* For multi-connectors */
class cMultiMaster g_Multi;

/* The global server configuration */

int g_mud_bootzone = 1;   /* Used when booting & resolving DIL templts */
int g_mudboot = 1;        /* Used when booting & resolving DIL templts */
int g_no_players = 0;     /* Statistics                                */
int g_max_no_players = 0; /* Statistics                                */
int g_player_convert = 0;
int g_slow_death = 0;   /* Shut her down, Martha, she's sucking mud */
int g_mud_shutdown = 0; /* clean shutdown */
int g_mud_reboot = 0;   /* reboot the game after a shutdown */
int g_wizlock = 0;      /* no mortals on now */
int g_tics = 60;        /* number of tics since boot-time */

char g_world_boottime[64] = ""; /* boottime of world */

/* ******************************************************************* *
 *             main game loop and related stuff                        *
 * ******************************************************************* */

const char *g_compile_date = __DATE__;
const char *g_compile_time = __TIME__;

//
// Need this to be sure the typedefs are right for 64 bit architecture (MS2020)
void type_validate_64(void)
{
    if (sizeof(void *) == 8)
    {
        slog(LOG_ALL, 0, "Running 64-bit mode");
    }
    else
    {
        slog(LOG_ALL, 0, "Running 32-bit mode");
    }

    // If any of these checks fail, update the type-defs so that they match your
    // particular architecture.
    assert(sizeof(char) == 1);
    assert(sizeof(ubit1) == 1);
    assert(sizeof(ubit8) == 1);
    assert(sizeof(ubit16) == 2);
    assert(sizeof(ubit32) == 4);
    assert(sizeof(ubit64) == 8);
    assert(sizeof(int) == 4); // Hell will probably freeze over if this isn't true
}

/* Init sockets, run game, and cleanup sockets */
void run_the_game(char *srvcfg)
{
#ifdef PROFILE
    extern char g_etext;
#endif
    /*
     * If you want BOOT TIME included in the PROFILE, use
     * monstartup HERE! Otherwise it defaults to the one below.
     *
     #ifdef PROFILE
     monstartup((int) 2, g_etext);
     #endif
     */

    g_descriptor_list = NULL;

    g_cServerConfig.Boot(srvcfg);
    slog(LOG_ALL, 0, "VME SERVER COMPILED AT %s %s", g_compile_date, g_compile_time);
    slog(LOG_ALL, 0, "VME Compiled with [%s]", get_compiled_hash_defines().c_str());
    slog(LOG_OFF, 0, "Read server configuration.");

    slog(LOG_OFF, 0, "Opening mother connection on port %d.", g_cServerConfig.getMotherPort());

    init_mother(g_cServerConfig.getMotherPort());

    slog(LOG_OFF, 0, "Signal trapping.");
    signal_setup();

    slog(LOG_OFF, 0, "Named pipe setup.");
    namedpipe_setup();

    boot_db();
    g_mudboot = 0;
    g_events.process();
    /*
    #ifdef PROFILE
       monstartup((int) 2, g_etext);
    #endif
    */
    slog(LOG_OFF, 0, "Priming eventqueue.");

    g_events.add(PULSE_SEC * SECS_PER_REAL_MIN * 2, check_idle_event, 0, 0);
    g_events.add(PULSE_SEC * 5, ping_multiplexers_event, 0, 0);
    g_events.add(PULSE_VIOLENCE, perform_violence_event, 0, 0);
    // g_events.add(PULSE_POINTS, point_update_event, 0, 0);
    // g_events.add(PULSE_SEC * SECS_PER_REAL_MIN * 5, update_crimes_event, 0, 0);
    g_events.add(PULSE_SEC * SECS_PER_REAL_MIN * 10, check_reboot_event, 0, 0);
    g_events.add(PULSE_SEC * SECS_PER_REAL_HOUR * 4, check_overpopulation_event, 0, 0);

    slog(LOG_OFF, 0, "Entering game loop.");

    game_loop();

#ifdef AMIGA1
    CloseTimer();
#endif

    g_CaptainHook.Close();

#ifdef PROFILE
    #if !defined(LINUX)
    monitor(0);
    #endif
#endif

    fclose_cache();
    db_shutdown();

    if (g_mud_reboot)
    {
        slog(LOG_OFF, 0, "Rebooting.");
        exit(42);
    }
    else
    {
        slog(LOG_OFF, 0, "Normal termination of game.");
        exit(0);
    }
}

void game_loop()
{
    struct timeval now, old;
    long delay;
    class descriptor_data *d = NULL;
    std::string str;

    g_tics = 61;
    gettimeofday(&old, (struct timezone *)0);

    while (!g_mud_shutdown)
    {
        /* work */
        game_event();

        pipeMUD_read(str);

        g_events.process();

        clear_destructed();

        g_tics++;

        /* Timer stuff. MUD is always at least OPT_USEC useconds in making one cycle. */

        gettimeofday(&now, (struct timezone *)0);

        delay = g_nTickUsec - (1000000L * (now.tv_sec - old.tv_sec) + (now.tv_usec - old.tv_usec));

        old = now;

        if (delay > 0)
        {
            usleep(delay);
            old.tv_usec += delay; /* This time has passed in usleep. Overrun is not important. */
        }
    }

    slog(LOG_ALL, 0, "Saving all players before exiting");
    for (d = g_descriptor_list; d; d = d->next)
    {
        if (d->original != NULL)
        {
            if (IS_PC(d->original) && UNIT_IN(d->original))
            {
                slog(LOG_ALL, 0, "Saving %s.", UNIT_NAME(d->original));
                save_player(d->original);
                save_player_contents(d->original, TRUE);
            }
        }
        else
        {
            if (IS_PC(d->character) && UNIT_IN(d->character))
            {
                slog(LOG_ALL, 0, "Saving %s.", UNIT_NAME(d->character));
                save_player(d->character);
                save_player_contents(d->character, TRUE);
            }
        }
    }
    multi_close_all();

    // pthread_cancel(g_dijkstra_thread);
    pthread_join(g_dijkstra_thread, NULL);
    sleep(1);
}

/* Accept new connects, relay commands, and call 'heartbeat-functs' */
void game_event(void)
{
    int i;
    char *pcomm = NULL;
    class descriptor_data *point;
    static struct timeval null_time = {0, 0};

    i = g_CaptainHook.Wait(&null_time);

    if (i < 0)
    {
        slog(LOG_ALL, 0, "Captain Hook error %d", i);
        return;
    }

    /* process_commands; */
    for (point = g_descriptor_list; point; point = g_next_to_process)
    {
        g_next_to_process = point->next;

        if (--(point->wait) <= 0 && !point->qInput.IsEmpty())
        {
            class cQueueElem *qe = point->qInput.GetHead();
            pcomm = (char *)qe->Data();
            qe->SetNull();
            delete qe;

            point->wait = 1;
            point->timer = 0;
            point->prompt_mode = PROMPT_EXPECT;

            if (point->snoop.snoop_by)
            {
                char buffer[MAX_INPUT_LENGTH + 10];
                snprintf(buffer, sizeof(buffer), "%s%s<br/>", SNOOP_PROMPT, pcomm);
                send_to_descriptor(buffer, CHAR_DESCRIPTOR(point->snoop.snoop_by));
            }

            point->fptr(point, pcomm);
            FREE(pcomm);
        }
    }

    /* give the people some prompts */
    for (point = g_descriptor_list; point; point = point->next)
    {
        if (point->prompt_mode == PROMPT_EXPECT)
        {
            send_prompt(point->character);
            send_to_descriptor("<go-ahead/>", point);
            point->prompt_mode = PROMPT_IGNORE;
        }
    }

    multi_clear(); /* Close all descriptors with no associated multi */
}

void ping_multiplexers_event(void *p1, void *p2)
{
    multi_ping_all();

    g_events.add(PULSE_SEC * 5, ping_multiplexers_event, 0, 0);
}

void check_idle_event(void *p1, void *p2)
{
    check_idle();
    g_events.add(PULSE_ZONE, check_idle_event, 0, 0);
}

void perform_violence_event(void *p1, void *p2)
{
    g_CombatList.PerformViolence();
    g_events.add(PULSE_VIOLENCE, perform_violence_event, 0, 0);
}

// void update_crimes_event(void *p1, void *p2)
//{
//    update_crimes();
//    g_events.add(1200, update_crimes_event, 0, 0);
//}

/*void point_update_event(void *p1, void *p2)
{
    void point_update(void);

    slog(LOG_ALL, 0, "Point update event()");
    point_update();
    g_events.add(PULSE_POINTS, point_update_event, 0, 0);
}*/

void timewarp_end(void *p1, void *p2)
{
    g_nTickUsec = OPT_USEC; // End the timewarp
    slog(LOG_ALL, 0, "Ending time warp, back to normal game speed.");
}

void check_overpopulation_event(void *p1, void *p2)
{
    g_events.add(PULSE_SEC * SECS_PER_REAL_HOUR * 4, check_overpopulation_event, 0, 0);

    int nHours;
    nHours = (g_tics / PULSE_SEC) / 3600;
    slog(LOG_ALL, 0, "Game up for %d tick hours, checking for overpopulation", nHours);

    class unit_data *u, *t;
    int i;
    int nUnits = 0;

    for (u = g_unit_list; u; u = u->gnext)
    {
        membug_verify_class(u);
        nUnits++;
        // Make sure it's not a player
        t = u;
        while (t)
        {
            if (UNIT_TYPE(t) == UNIT_ST_PC)
            {
                break;
            }
            t = UNIT_IN(t);
        }
        if (t)
        {
            continue; // If it's inside a player, skip
        }

        i = 0;
        for (t = UNIT_CONTAINS(u); t; t = t->next)
        { // count top layer
            i++;
        }

        if (i >= 50)
        {
            slog(LOG_ALL, 0, "Too many items in %s@%s(%s) : %d units", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u), unit_trace_up(u).c_str(), i);

            struct diltemplate *worms;
            worms = find_dil_template("worms@basis");
            if (worms)
            {
                class dilprg *prg = dil_copy_template(worms, u, NULL);
                if (prg)
                {
                    prg->waitcmd = WAITCMD_MAXINST - 1;
                    dil_activate(prg);
                }
            }
        }
    }
    slog(LOG_ALL, 0, "Server has %d units.", nUnits);
}

void check_reboot_event(void *p1, void *p2)
{
    if (check_reboot() == 1)
    {
        g_events.add(PULSE_SEC * SECS_PER_REAL_MIN * 2, check_reboot_event, 0, 0);
    }
    else
    {
        g_events.add(PULSE_SEC * SECS_PER_REAL_MIN * 10, check_reboot_event, 0, 0);
    }
}

/* utility procedure */
struct timeval timediff(struct timeval *a, struct timeval *b)
{
    struct timeval rslt, tmp;

    tmp = *a;

    if ((rslt.tv_usec = tmp.tv_usec - b->tv_usec) < 0)
    {
        rslt.tv_usec += 1000000;
        --(tmp.tv_sec);
    }
    if (tmp.tv_sec - b->tv_sec < 0)
    {
        rslt.tv_usec = 0;
        rslt.tv_sec = 0;
    }
    else
    {
        rslt.tv_sec = tmp.tv_sec - b->tv_sec;
    }

    return rslt;
}

void ShowUsage(const char *name)
{
    fprintf(stderr,
            "Usage: %s [-h] [-d pathname] -l [filename] "
            "[-z pathname] [-c -C -L\n\n",
            name);
    fprintf(stderr, "  -h: This help screen.\n");
    fprintf(stderr, "  -c: Convert player files\n");
    fprintf(stderr, "  -C: Cleanup  player files (deletes bad or old chars use with care)\n");
    fprintf(stderr, "  -l: Log file\n");
    fprintf(stderr, "  -L: List player files\n");
    fprintf(stderr, "  -s: Location of the server.cfg file\n");
    fprintf(stderr, "  -p: Persistant containers list\n");
    fprintf(stderr, "Copyright (C) 1994 - 1996 by Valhalla.\n");
}
