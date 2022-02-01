/*
 $Author: All $
 $RCSfile: mplex.cpp,v $
 $Date: 2003/12/31 01:21:59 $
 $Revision: 2.4 $
 */

#ifdef _WINDOWS
    #include "string.h"
    #include "telnet.h"
    #include "winbase.h"
    #include "winsock2.h"

    #include <time.h>
#endif

#include "MUDConnector.h"
#include "essential.h"
#include "hook.h"
#include "mplex.h"
#include "network.h"
#include "slog.h"
#include "textutil.h"

#include <netdb.h>

#include <cassert>
#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstdlib>

namespace mplex
{

char g_mudname[50] = "the MUD server (via DikuMUD Mplex)";
int g_bHadAlarm = FALSE;
arg_type g_mplex_arg;

void bye_signal(int signal)
{
    g_CaptainHook.Close();

    slog(LOG_OFF,
         0,
         "Received signal #%d (SIGQUIT, SIGHUP, SIGINT or SIGTERM)."
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
    fprintf(stderr,
            "Usage: %s [-a] [-h] [-c] [-e] [-r] [-t] [-x] [-l <filename>] [-p <num>] [-d <path>] [-s <port>] [-a <address>]\n",
            name);
    fprintf(stderr, "  -h  This help screen.\n");
    fprintf(stderr, "  -c  Deprecated. Always on. ANSI Colour when set, TTY when not.\n");
    fprintf(stderr, "  -e  Echo mode (echo chars locally).\n");
    fprintf(stderr, "  -r  Redraw prompt lcoally (usually only in -e mode).\n");
    fprintf(stderr, "  -t  Deprecated. Always on. Telnet mode when set (IAC negotiation).\n");
    fprintf(stderr, "  -p  Player port number, default is 4242.\n");
    fprintf(stderr, "  -a  Internet address of server (localhost default).\n");
    fprintf(stderr, "  -s  Internet port of server (4999 default).\n");
    fprintf(stderr, "  -l  Name of the logfile (default: ./mplex.log).\n");
    fprintf(stderr, "  -x  Output raw HTML on telnet (nice for debugging).\n");
    fprintf(stderr, "  -w  Use Websockets.\n");
    fprintf(stderr, "  -m  Use mud protocol (experimental).\n");
    exit(0);
}

int ParseArg(int argc, char *argv[], arg_type *arg)
{
    int i = 0;
    int n = 0;
    hostent *pHostInfo = nullptr;
    in_addr *pAddr = nullptr;
    char *c = nullptr;
    char *log_name = nullptr;

    arg->nMudPort = 4999;    /* Default port */
    arg->nMotherPort = 4242; /* Default port */
    arg->pAddress = str_dup(DEF_SERVER_ADDR);

    arg->g_bModeANSI = TRUE;   // Beginning of deprecating support for TTY
    arg->g_bModeTelnet = TRUE; // Beginning of deprecating support for TTY

    arg->g_bModeEcho = FALSE;
    arg->g_bModeRedraw = FALSE;
    arg->g_bModeRawHTML = FALSE;
    arg->bWebSockets = FALSE;
    arg->bMudProtocol = false;

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
                fprintf(stderr, "-c is deprecated, it's always on.\n");
                arg->g_bModeANSI = TRUE;
                break;

            case 'e':
                arg->g_bModeEcho = TRUE;
                break;

            case 'r':
                arg->g_bModeRedraw = TRUE;
                break;

            case 't':
                fprintf(stderr, "-t is deprecated, it's always on.\n");
                arg->g_bModeTelnet = TRUE;
                break;

            case 'w':
                arg->bWebSockets = TRUE;
                break;

            case 'm':
                arg->bMudProtocol = true;
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
                    Assert(pHostInfo != nullptr, "Could not lookup address.");
                    pAddr = (in_addr *)(pHostInfo->h_addr_list[0]);
                    c = inet_ntoa(*pAddr);
                    Assert(c != nullptr, "Error in address conversion");
                }

                arg->pAddress = str_dup(c);

                Assert(inet_addr(arg->pAddress) != (unsigned long)-1, "Illegal inet address");
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
    g_log_file_fd = fopen(log_name, "w");

    if (!g_log_file_fd)
    {
        fprintf(stderr, "Failed to open Log file:  %s", log_name);
        free(log_name);
        exit(0);
    }

    free(log_name);

    return TRUE;
}

} // namespace mplex
