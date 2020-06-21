/*
 $Author: All $
 $RCSfile: mplex.cpp,v $
 $Date: 2003/12/31 01:21:59 $
 $Revision: 2.4 $
 */

#ifdef _WINDOWS
#include "telnet.h"
#include "winsock2.h"
#include <time.h>
#include "string.h"
#include "winbase.h"
#endif

#ifdef LINUX
#include <unistd.h>
#include <arpa/telnet.h>
#include <sys/time.h>
#include <netdb.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <assert.h>
#include <signal.h>
#include <thread>

#define MPLEX_COMPILE 1
#include "structs.h"

#include "network.h"
#include "protocol.h"
#include "essential.h"
#include "textutil.h"
#include "ttydef.h"
#include "db.h"
#include "utility.h"
#include "translate.h"
#include "hook.h"
#include "common.h"
#include "queue.h"
#include "mplex.h"
#include "MUDConnector.h"
#include "ClientConnector.h"

char mudname[50] = "the MUD server (via DikuMUD Mplex)";
int g_bHadAlarm = FALSE;
struct arg_type mplex_arg;

void bye_signal(int signal)
{
    CaptainHook.Close();

    slog(LOG_OFF, 0, "Received signal #%d (SIGQUIT, SIGHUP, SIGINT or SIGTERM)."
                     "  Shutting down",
         signal);

    exit(1);
}

#ifndef _WINDOWS
void alarm_signal(int sig)
{
    signal(SIGALRM, alarm_signal);
    slog(LOG_OFF, 0, "Received alarm signal.");
    g_bHadAlarm = TRUE;
}
#endif

void ShowUsage(const char *name)
{
    fprintf(stderr, "Usage: %s [-a] [-h] [-c] [-e] [-r] [-t] [-x] [-l <filename>] [-p <num>] [-d <path>] [-s <port>] [-a <address>]\n", name);
    fprintf(stderr, "  -h  This help screen.\n");
    fprintf(stderr, "  -c  ANSI Colour when set, TTY when not.\n");
    fprintf(stderr, "  -e  Echo mode (echo chars locally).\n");
    fprintf(stderr, "  -r  Redraw prompt lcoally (usually only in -e mode).\n");
    fprintf(stderr, "  -t  Telnet mode when set (IAC negotiation).\n");
    fprintf(stderr, "  -p  Player port number, default is 4242.\n");
    fprintf(stderr, "  -a  Internet address of server (localhost default).\n");
    fprintf(stderr, "  -s  Internet port of server (4999 default).\n");
    fprintf(stderr, "  -l  Name of the logfile (default: ./mplex.log).\n");
    fprintf(stderr, "  -x  Output raw HTML on telnet (nice for debugging).\n");
    fprintf(stderr, "  -w  Use Websockets.\n");
    exit(0);
}

int ParseArg(int argc, char *argv[], struct arg_type *arg)
{
    int i, n;
    struct hostent *pHostInfo;
    struct in_addr *pAddr;
    char *c;
    char *log_name;

    arg->nMudPort = 4999;    /* Default port */
    arg->nMotherPort = 4242; /* Default port */
    arg->pAddress = str_dup(DEF_SERVER_ADDR);
    arg->g_bModeANSI = FALSE;
    arg->g_bModeEcho = FALSE;
    arg->g_bModeRedraw = FALSE;
    arg->g_bModeTelnet = FALSE;
    arg->g_bModeRawHTML = FALSE;
    arg->bWebSockets = FALSE;
    log_name = str_dup("./mplex.log");

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            fprintf(stderr, "Illegal argument '%s'.\n", argv[i]);
            ShowUsage(argv[0]);
        }

        switch (argv[i][1])
        {
        case 'h':
        case '?':
            ShowUsage(argv[0]);
            break;

        case 'x':
            arg->g_bModeRawHTML = TRUE;
            break;

        case 'c':
            arg->g_bModeANSI = TRUE;
            break;

        case 'e':
            arg->g_bModeEcho = TRUE;
            break;

        case 'r':
            arg->g_bModeRedraw = TRUE;
            break;

        case 't':
            arg->g_bModeTelnet = TRUE;
            break;

        case 'w':
            arg->bWebSockets = TRUE;
            break;

        case 'l':
            i++;
            Assert(i < argc, "No argument to Mplex log.");
            free(log_name);
            log_name = str_dup(argv[i]);
            break;
        case 'a':
            i++;
            Assert(i < argc, "No argument to internet address.");

            c = argv[i];

            if (!isdigit(c[0]))
            {
                pHostInfo = gethostbyname(c);
                Assert(pHostInfo != NULL, "Could not lookup address.");
                pAddr = (struct in_addr *)(pHostInfo->h_addr_list[0]);
                c = inet_ntoa(*pAddr);
                Assert(c != NULL, "Error in address conversion");
            }

            arg->pAddress = str_dup(c);

            Assert(inet_addr(arg->pAddress) != (unsigned long)-1,
                   "Illegal inet address");
            break;

        case 'p':
            i++;
            n = atoi(argv[i]);
            Assert(n > 1000, "Port number in reserved area.");
            arg->nMotherPort = n;
            break;

        case 's':
            i++;
            n = atoi(argv[i]);
            Assert(n > 1000, "Port number in reserved area.");
            arg->nMudPort = n;
            break;

        default:
            fprintf(stderr, "Illegal option.\n");
            ShowUsage(argv[0]);
            exit(0);
        }
    }
    log_file_fd = fopen(log_name, "w");

    if (!log_file_fd)
    {
        fprintf(stderr, "Failed to open Log file:  %s", log_name);
        free(log_name);
        exit(0);
    }

    free(log_name);

    return TRUE;
}

int main(int argc, char *argv[])
{
    static int i = 0;
    int fd = -1;

#ifdef tPROFILE
    extern etext();
    monstartup((int)2, etext);
#endif

    assert(i++ == 0); /* Make sure we dont call ourselves... cheap hack! :) */

    if (!ParseArg(argc, argv, &mplex_arg))
        exit(0);

#ifndef _WINDOWS
    signal(SIGQUIT, bye_signal);
    signal(SIGHUP, bye_signal);
    signal(SIGINT, bye_signal);
    signal(SIGTERM, bye_signal);
    signal(SIGALRM, alarm_signal);
#endif
    /* MS2020 Websockets test hack */
    translate_init();

    slog(LOG_OFF, 0, "Opening mother connection on port %d.", mplex_arg.nMotherPort);

    if (mplex_arg.bWebSockets)
    {
        /* MS2020 Websockets test hack */
        void runechoserver(void);
        thread t1(runechoserver);
        t1.detach();
    }
    else
    {
        fd = OpenMother(mplex_arg.nMotherPort);
        Assert(fd != -1, "NO MOTHER CONNECTION.");

        CaptainHook.Hook(fd, &MotherHook);
    }

    /* Subtract stdout, stdin, stderr, fdmud, fdmother and 2 to be safe. */
#if defined(_WINDOWS)
    g_nActiveConnections = 256 - 3 - 2 - 2;
#else
    g_nConnectionsLeft = getdtablesize() - 3 - 2 - 2;
#endif
    Control();

    MudHook.Unhook();
    MotherHook.Unhook();

    return 0;
}
