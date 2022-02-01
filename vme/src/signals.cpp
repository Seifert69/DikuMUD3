/*
 $Author: All $
 $RCSfile: signals.cpp,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.3 $
 */

#include "comm.h"
#include "config.h"
#include "db.h"
#include "main_functions.h"
#include "slog.h"

#include <sys/time.h>

#include <csignal>
#include <cstdio>

void other_signal(int signal_no)
{
    slog(LOG_ALL, 0, "RECEIVED SIGNAL %d.", signal_no);
}

void checkpointing(int signal_no)
{
    static int last_tick = 0;

    if (g_player_convert)
    {
        return;
    }

    if (last_tick != 0 && g_tics == last_tick)
    {
        slog(LOG_ALL, 0, "CHECKPOINT shutdown: tics not updated");
        assert(FALSE);
    }

    assert(g_tics != 0);
    last_tick = g_tics;

#ifdef POSIX
    /* Default is usually a one_shot */
    signal(SIGVTALRM, checkpointing);
#endif
}

void shutdown_request(int signal_no)
{
    slog(LOG_ALL, 0, "Received USR2 - shutdown request");
    g_mud_shutdown = 1;
}

void message_request(int signal_no)
{
    FILE *msg_file_fd = nullptr;
    char line[512];

    slog(LOG_ALL, 0, "Received USR1 - message request");

    msg_file_fd = fopen(g_cServerConfig.getFileInEtcDir(MESSAGE_FILE).c_str(), "r");
    if (!msg_file_fd)
    {
        fprintf(stderr, "Error in opening the log:  '%s'", MESSAGE_FILE);
        return;
    }

    while (fgets(line, 512, msg_file_fd))
    {
        send_to_all(line);
        slog(LOG_ALL, 0, line);
    }

    fclose(msg_file_fd);
}

/* kick out players etc */
void hupsig(int signal)
{
    slog(LOG_ALL,
         0,
         "Received signal #%d (SIGHUP, SIGINT, or SIGTERM). "
         "Shutting down",
         signal);
    g_mud_shutdown = 1;
    //  exit (0);			/* something more elegant should perhaps be substituted */
}

void logsig(int signal)
{
    slog(LOG_ALL, 0, "Signal #%d received. Ignoring.", signal);
}

#ifdef DEBUG_HISTORY

void dump_debug_history(void);

void (*sigiot_func)(int) = NULL;
void (*sigsegv_func)(int) = NULL;
void (*sigbus_func)(int) = NULL;
void (*sigabrt_func)(int) = NULL;

void sig_debugdump(int tsignal)
{
    static int here = FALSE;

    slog(LOG_ALL, 0, "SIG_IOT #%d received!", tsignal);
    if (!here)
    {
        dump_debug_history();
        here = TRUE;
    }
    signal(SIGSEGV, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGIOT, SIG_DFL);

    //   signal(SIGBUS,  sigbus_func);
    //      signal(SIGIOT,  sigiot_func);
    //      signal(SIGSEGV, sigsegv_func);
    assert(FALSE);
}

#endif /* DEBUG_HISTORY */
void signal_reset()
{
    sigset_t sigt;

    sigfillset(&sigt);

    pthread_sigmask(SIG_BLOCK, &sigt, nullptr);
}

void signal_setup()
{
    itimerval itime;
    timeval interval;

    /* just to be on the safe side: */

    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, hupsig);
    signal(SIGINT, hupsig);
    signal(SIGALRM, logsig);
    signal(SIGTERM, hupsig);

    signal(SIGUSR2, shutdown_request);

    signal(SIGUSR1, message_request);

#ifdef DEBUG_HISTORY
    sigbus_func = signal(SIGBUS, sig_debugdump);
    sigiot_func = signal(SIGIOT, sig_debugdump);
    sigsegv_func = signal(SIGSEGV, sig_debugdump);
#endif

    /* set up the deadlock-protection */

    interval.tv_sec = 600; /* 600 = 10 minutes */
    interval.tv_usec = 0;
    itime.it_interval = interval;
    itime.it_value = interval;
    setitimer(ITIMER_VIRTUAL, &itime, nullptr);
    signal(SIGVTALRM, checkpointing);
}
