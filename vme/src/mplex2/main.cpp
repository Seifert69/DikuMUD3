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
#include "compile_defines.h"

int main(int argc, char *argv[])
{
    static int i = 0;
    int fd = -1;

#ifdef tPROFILE
    extern etext();
    monstartup((int)2, etext);
#endif

    assert(i++ == 0); /* Make sure we dont call ourselves... cheap hack! :) */

    if (!ParseArg(argc, argv, &g_mplex_arg))
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

    slog(LOG_ALL, 0, "MPlex compiled with [%s]", get_compiled_hash_defines().c_str());
    slog(LOG_OFF, 0, "Opening mother connection on port %d.", g_mplex_arg.nMotherPort);

    if (g_mplex_arg.bWebSockets)
    {
        /* MS2020 Websockets test hack */
        void runechoserver(void);
        std::thread t1(runechoserver);
        t1.detach();
    }
    else
    {
        fd = OpenMother(g_mplex_arg.nMotherPort);
        Assert(fd != -1, "NO MOTHER CONNECTION.");

        if (g_MotherHook.tfd() != -1)
            slog(LOG_ALL, 0, "Hook() in main called with a non -1 fd.");

        g_CaptainHook.Hook(fd, &g_MotherHook);
    }

    /* Subtract stdout, stdin, stderr, fdmud, fdmother and 2 to be safe. */
#if defined(_WINDOWS)
    g_nActiveConnections = 256 - 3 - 2 - 2;
#else
    g_nConnectionsLeft = getdtablesize() - 3 - 2 - 2;
#endif
    Control();

    g_MudHook.Unhook();
    g_MotherHook.Unhook();

    return 0;
}
