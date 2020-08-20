/*
 $Author: All $
 $RCSfile: main.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.10 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

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
#include "hookmud.h"
#include "dilrun.h"


extern ubit32 memory_total_alloc;
extern void special_event(void *p1, void *p2);
#define OPT_USEC 250000L /* time delay corresponding to 4 passes/sec */
long g_nTickUsec = OPT_USEC;  // Dont mess with this, it's the game heartbeat. Look at 'timewarp'

#define HEAPSPACE_INCREMENT 500
/* constants */
eventqueue events;
class descriptor_data *descriptor_list = NULL;
class descriptor_data *next_to_process = NULL;

/* For multi-connectors */
class cMultiMaster Multi;

/* The global server configuration */

int mud_bootzone = 1;   /* Used when booting & resolving DIL templts */
int mudboot = 1;        /* Used when booting & resolving DIL templts */
int no_players = 0;     /* Statistics                                */
int max_no_players = 0; /* Statistics                                */
int player_convert = 0;
int slow_death = 0;   /* Shut her down, Martha, she's sucking mud */
int mud_shutdown = 0; /* clean shutdown */
int mud_reboot = 0;   /* reboot the game after a shutdown */
int wizlock = 0;      /* no mortals on now */
int tics = 60;        /* number of tics since boot-time */

char world_boottime[64] = ""; /* boottime of world */

const char *compile_date = __DATE__;
const char *compile_time = __TIME__;

/* external functions */
void string_add(class descriptor_data *d, char *str);

void boot_db(void);

/* local functions */
void run_the_game(char *srvcfg);
void game_loop(void);
struct timeval timediff(struct timeval *a, struct timeval *b);

/* local events */
void game_event();
void ping_multiplexers_event(void *, void *);
void check_idle_event(void *, void *);
void perform_violence_event(void *, void *);
//void point_update_event(void *, void *);
void update_crimes_event(void *, void *);
void update_crimes();
void check_reboot_event(void *, void *);
void check_overpopulation_event(void *p1, void *p2);
int check_reboot();

/* ******************************************************************* *
 *             main game loop and related stuff                        *
 * ******************************************************************* */

#ifdef LINUX
#include <sys/time.h>
#include <sys/resource.h>

/* int setrlimit(int, struct rlimit *); */
int getrlimit(int, struct rlimit *);
#endif

//
// Need this to be sure the typedefs are right for 64 bit architecture (MS2020)
void type_validate_64(void)
{
    assert(sizeof(void *) == 8);
    assert(sizeof(char) == 1);
    assert(sizeof(ubit1) == 1);
    assert(sizeof(ubit8) == 1);
    assert(sizeof(ubit16) == 2);
    assert(sizeof(ubit32) == 4);
    assert(sizeof(ubit64) == 8);
    slog(LOG_ALL, 0, "64-bit architecture checked out OK");
}

int main(int argc, char **argv)
{
    void cleanup_playerfile(int argc, char *argv[]);
    char srvcfg[512], *tempcfg, *log_name;
    int pos = 1;

    tempcfg = getenv("VME_SERVER_CFG");
    if (tempcfg)
        strcpy(srvcfg, tempcfg);
    else
        strcpy(srvcfg, "../etc/server.cfg"); //MS2020 add a default...

    log_name = str_dup("./vme.log");

    while ((pos < argc) && (*(argv[pos]) == '-'))
    {
        switch (*(argv[pos] + 1))
        {
        case '?':
        case 'h':
            ShowUsage(argv[0]);
            exit(0);

        case 'c':
            slog(LOG_OFF, 0, "Converting player file mode.");
            player_convert = 1;
            break;

        case 'C':
            slog(LOG_OFF, 0, "Cleaning up player file mode.");
            player_convert = 2;
            break;

        case 'L':
            slog(LOG_OFF, 0, "Listing player file mode.");
            player_convert = 3;
            break;

        case 's':
            if (*(argv[pos] + 2))
                strcpy(srvcfg, argv[pos] + 2);
            else if (++pos < argc)
                strcpy(srvcfg, argv[pos]);
            else
            {
                slog(LOG_OFF, 0,
                     "Full path and file name for the config file expected.");
                exit(4);
            }
            break;

        case 'l':
            if (*(argv[pos] + 2))
            {
                FREE(log_name);
                log_name = str_dup(argv[pos] + 2);
            }
            else if (++pos < argc)
            {
                FREE(log_name);
                log_name = str_dup(argv[pos]);
            }
            else
            {
                slog(LOG_OFF, 0,
                     "Full path and file name for the config file expected.");
                exit(5);
            }
            break;

        default:
            slog(LOG_OFF, 0, "Unknown option %s in argument string.",
                 (argv[pos]));
            break;
        }
        pos++;
    }

    if (pos < argc)
        if (!isdigit(*argv[pos]))
        {
            ShowUsage(argv[0]);
            exit(6);
        }

    log_file_fd = freopen(log_name, "w", stderr);
    if (!log_file_fd)
    {
        fprintf(stderr, "Error in opening the log:  '%s'", log_name);
        free(log_name);
        exit(7);
    }

    free(log_name);

    type_validate_64(); // MS2020

#ifdef LINUX
/*    {   MS2020 - appears not to be used at all?
        int n;
        struct rlimit rlim;

        n = getrlimit (RLIMIT_CPU, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT CPU: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

        n = getrlimit (RLIMIT_DATA, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT DATA: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

        n = getrlimit (RLIMIT_STACK, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT STACK: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

        n = getrlimit (RLIMIT_CORE, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT CORE: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

        n = getrlimit (RLIMIT_RSS, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT RSS: %d / %d.", rlim.rlim_cur, rlim.rlim_max);
    }*/
#endif

    /*
    #if defined(GENERIC_SYSV) || defined(HPUX)
      srand48 (time (0));
    #elif defined(LINUX)
    */
    /* Other used routines are declared obsolete by SVID3 */
    //  srand (time (0));
    //#else
    srandom(time(0));
    //#endif
    run_the_game(srvcfg);
    return (0);
}

/* Init sockets, run game, and cleanup sockets */
void run_the_game(char *srvcfg)
{
#ifdef PROFILE
    extern char etext;
#endif

    void signal_setup(void);

    /*
     * If you want BOOT TIME included in the PROFILE, use
     * monstartup HERE! Otherwise it defaults to the one below.
     *
     #ifdef PROFILE
     monstartup((int) 2, etext);
     #endif
     */

    descriptor_list = NULL;

    g_cServerConfig.Boot(srvcfg);
    slog(LOG_ALL, 0, "VME SERVER COMPILED AT %s %s",
         compile_date, compile_time);
    slog(LOG_OFF, 0, "Read server configuration.");

    slog(LOG_OFF, 0, "Opening mother connection on port %d.",
         g_cServerConfig.m_nMotherPort);

    init_mother(g_cServerConfig.m_nMotherPort);

    slog(LOG_OFF, 0, "Signal trapping.");
    signal_setup();

    boot_db();
    mudboot = 0;
    events.process();
    /*
    #ifdef PROFILE
       monstartup((int) 2, etext);
    #endif
    */
    slog(LOG_OFF, 0, "Priming eventqueue.");

    events.add(PULSE_SEC * SECS_PER_REAL_MIN * 2, check_idle_event, 0, 0);
    events.add(PULSE_SEC * 5, ping_multiplexers_event, 0, 0);
    events.add(PULSE_VIOLENCE, perform_violence_event, 0, 0);
    //events.add(PULSE_POINTS, point_update_event, 0, 0);
    events.add(PULSE_SEC * SECS_PER_REAL_MIN * 5, update_crimes_event, 0, 0);
    events.add(PULSE_SEC * SECS_PER_REAL_MIN * 10, check_reboot_event, 0, 0);
    events.add(PULSE_SEC * SECS_PER_REAL_HOUR * 4, check_overpopulation_event, 0, 0);

    slog(LOG_OFF, 0, "Entering game loop.");

    game_loop();

#ifdef AMIGA1
    CloseTimer();
#endif

    CaptainHook.Close();

#ifdef PROFILE
#if !defined(LINUX)
    monitor(0);
#endif
#endif

    fclose_cache();
    void db_shutdown(void);
    db_shutdown();

    if (mud_reboot)
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
    void clear_destructed(void);

    tics = 61;
    gettimeofday(&old, (struct timezone *)0);

    while (!mud_shutdown)
    {
        /* work */
        game_event();

        events.process();

        clear_destructed();

        tics++;

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
    for (d = descriptor_list; d; d = d->next)
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

    extern pthread_t dijkstra_thread;
    //pthread_cancel(dijkstra_thread);
    pthread_join(dijkstra_thread, NULL);
    sleep(1);
}

/* Accept new connects, relay commands, and call 'heartbeat-functs' */
void game_event(void)
{
    int i;
    char *pcomm = NULL;
    class descriptor_data *point;
    static struct timeval null_time = {0, 0};
    void multi_close(struct multi_element * pe);
    void multi_clear(void);

    i = CaptainHook.Wait(&null_time);

    if (i < 0)
    {
        slog(LOG_ALL, 0, "Captain Hook error %d", i);
        return;
    }

    /* process_commands; */
    for (point = descriptor_list; point; point = next_to_process)
    {
        next_to_process = point->next;

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
                sprintf(buffer, "%s%s<br/>", SNOOP_PROMPT, pcomm);
                send_to_descriptor(buffer,
                                   CHAR_DESCRIPTOR(point->snoop.snoop_by));
            }

            point->fptr(point, pcomm);
            FREE(pcomm);
        }
    }

    /* give the people some prompts */
    for (point = descriptor_list; point; point = point->next)
    {
        if (point->prompt_mode == PROMPT_EXPECT)
        {
            send_prompt(point->character);
            point->prompt_mode = PROMPT_IGNORE;
        }
    }

    multi_clear(); /* Close all descriptors with no associated multi */
}

void ping_multiplexers_event(void *p1, void *p2)
{
    void multi_ping_all(void);

    multi_ping_all();

    events.add(PULSE_SEC*5, ping_multiplexers_event, 0, 0);
}

void check_idle_event(void *p1, void *p2)
{
    void check_idle(void);

    check_idle();
    events.add(PULSE_ZONE, check_idle_event, 0, 0);
}

void perform_violence_event(void *p1, void *p2)
{
    CombatList.PerformViolence();
    events.add(PULSE_VIOLENCE, perform_violence_event, 0, 0);
}

void update_crimes_event(void *p1, void *p2)
{
    update_crimes();
    events.add(1200, update_crimes_event, 0, 0);
}

/*void point_update_event(void *p1, void *p2)
{
    void point_update(void);

    slog(LOG_ALL, 0, "Point update event()");
    point_update();
    events.add(PULSE_POINTS, point_update_event, 0, 0);
}*/

void timewarp_end(void *p1, void *p2)
{
    g_nTickUsec = OPT_USEC; // End the timewarp
    slog(LOG_ALL, 0, "Ending time warp, back to normal game speed.");
}


void check_overpopulation_event(void *p1, void *p2)
{
    events.add(PULSE_SEC * SECS_PER_REAL_HOUR * 4, check_overpopulation_event, 0, 0);

    int nHours;
    nHours = (tics / PULSE_SEC) / 3600;
    slog(LOG_ALL, 0, "Game up for %d tick hours, checking for overpopulation", nHours);

    class unit_data *u, *t;
    int i;
    int nUnits = 0;

    for (u = unit_list; u; u = u->gnext)
    {
        membug_verify_class(u);
        nUnits++;
        // Make sure it's not a player
        t = u;
        while (t)
        {
            if (UNIT_TYPE(t) == UNIT_ST_PC)
                break;
            t = UNIT_IN(t);
        }
        if (t)
            continue; // If it's inside a player, skip

        i = 0;
        for (t = UNIT_CONTAINS(u); t; t = t->next) // count top layer
            i++;

        if (i >= 50)
        {
            slog(LOG_ALL, 0, "Too many items in %s@%s(%s) : %d units", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u), UNIT_NAME(u), i);

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
        events.add(PULSE_SEC * SECS_PER_REAL_MIN * 2, check_reboot_event, 0,
                   0);
    }
    else
    {
        events.add(PULSE_SEC * SECS_PER_REAL_MIN * 10, check_reboot_event, 0,
                   0);
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
        rslt.tv_sec = tmp.tv_sec - b->tv_sec;

    return rslt;
}

void ShowUsage(const char *name)
{
    fprintf(stderr, "Usage: %s [-h] [-d pathname] -l [filename] "
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

