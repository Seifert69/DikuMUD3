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
#include <unistd.h>

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
std::mutex g_hookLock;

void sleep_unlocked(unsigned int seconds)
{
    g_hookLock.unlock();
    sleep(seconds);
    g_hookLock.lock();
}

// The log file's descriptor, for bye_signal()
static int g_log_fd = -1;

void bye_signal(int /* signal */)
{
    // Only async-signal-safe calls here, write(2) and _exit(2): the signal may have interrupted
    // the main loop inside malloc, OpenSSL or slog's own stdio, and slog (a std::string, ctime,
    // fprintf) or closing the hooks (SSL_shutdown, SSL_free) would wait on their locks for ever.
    // The kernel closes every socket when we exit; the clients see that as a close. The
    // websocket server catches these signals itself and says goodbye first (echo_server.cpp).
    static const char msg[] = "Received SIGQUIT, SIGHUP, SIGINT or SIGTERM: stopping on request.\n";
    if (g_log_fd >= 0 && write(g_log_fd, msg, sizeof(msg) - 1) < 0)
    {
        // nothing to do about it
    }
    _exit(MPLEX_EXIT_STOPPED);
}

#ifndef _WINDOWS
void alarm_signal(int sig)
{
    signal(SIGALRM, alarm_signal);
    slog(LOG_OFF, 0, "Received alarm signal.");
    g_bHadAlarm = TRUE;
}
#endif

// Prints the usage and exits with code: 0 when asked for it (-h), 1 after a wrong argument
void ShowUsage(const char *name, int code)
{
    fprintf(stderr,
            "Usage: %s [-a] [-h] [-c] [-e] [-r] [-t] [-w] [-x] [-z] [-l <filename>] [-p <num>] [-d <path>] [-s <port>] [-a <address>]\n",
            name);
    fprintf(stderr, "  -h  This help screen.\n");
    fprintf(stderr, "  -c  Deprecated. Always on. ANSI Colour when set, TTY when not.\n");
    fprintf(stderr, "  -e  Echo mode (echo chars locally).\n");
    fprintf(stderr, "  -r  Redraw prompt lcoally (usually only in -e mode).\n");
    fprintf(stderr, "  -p  Player port number, default is 4242.\n");
    fprintf(stderr, "  -a  Internet address of server (localhost default).\n");
    fprintf(stderr, "  -s  Internet port of server (4999 default).\n");
    fprintf(stderr, "  -l  Name of the logfile (default: ./mplex.log).\n");
    fprintf(stderr, "  -x  Output raw HTML on telnet (nice for debugging).\n");
    fprintf(stderr, "  -y  Strip away the HTML on websockets.\n");
    fprintf(stderr, "  -w  Use Websockets.\n");
    fprintf(stderr, "  -t  With -w: TLS, so wss:// for browsers. Reads ./server.pem and ./dh.pem.\n");
    fprintf(stderr, "  -z  TLS on the telnet port (telnets://), for TinTin++ '#ssl', Mudlet's\n");
    fprintf(stderr, "      'Secure' box and 'telnet -z ssl'. Reads ./server.pem. Not with -w.\n");
    fprintf(stderr, "      Run a second mplex without -z for plain telnet.\n");
    fprintf(stderr, "  -m  Use mud protocol (experimental).\n");
    
    exit(code);
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
    arg->g_bUseTLS = false;
    arg->bTelnetTLS = false;

    arg->g_bModeANSI = TRUE;   // Beginning of deprecating support for TTY
    arg->g_bModeTelnet = TRUE; // Beginning of deprecating support for TTY

    arg->g_bModeEcho = FALSE;
    arg->g_bModeRedraw = FALSE;
    arg->g_bModeRawHTML = FALSE;
    arg->bWebSockets = FALSE;
    arg->bMudProtocol = false;
    arg->bForceAscii = false;

    log_name = str_dup("./mplex.log");

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            fprintf(stderr, "Illegal argument '%s'.\n", argv[i]);
            ShowUsage(argv[0], 1);
        }

        switch (argv[i][1])
        {
            case 'h':
            case '?':
                ShowUsage(argv[0], 0);
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

            case 'w':
                arg->bWebSockets = TRUE;
                break;

            case 't':
                arg->g_bUseTLS = true;
                break;

            case 'z':
                arg->bTelnetTLS = true;
                break;

            case 'm':
                arg->bMudProtocol = true;
                break;

            case 'y':
                arg->bForceAscii = true;
                break;

            case 'l':
                i++;
                Assert(i < argc, "No argument to Mplex log.");
                FREE(log_name);
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
                ShowUsage(argv[0], 1);
        }
    }
    if (arg->bTelnetTLS && arg->bWebSockets)
    {
        fprintf(stderr, "-z is for the telnet port; with -w use -t for wss://.\n");
        exit(1);
    }

    g_log_file_fd = fopen(log_name, "w");

    if (!g_log_file_fd)
    {
        fprintf(stderr, "Failed to open Log file:  %s", log_name);
        FREE(log_name);
        exit(1);
    }
    g_log_fd = fileno(g_log_file_fd); // for bye_signal(), which can't use the FILE

    FREE(log_name);

    return TRUE;
}

} // namespace mplex
