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
#include "compile_defines.h"
#include "echo_server.h"
#include "essential.h"
#include "hook.h"
#include "mplex.h"
#include "network.h"
#include "slog.h"
#include "translate.h"

#include <unistd.h>

#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <thread>

int main(int argc, char *argv[])
{
    static int i = 0;
    int fd = -1;

#ifdef tPROFILE
    extern etext();
    monstartup((int)2, etext);
#endif

    assert(i++ == 0); /* Make sure we dont call ourselves... cheap hack! :) */

    if (!ParseArg(argc, argv, &mplex::g_mplex_arg))
    {
        exit(0);
    }

#ifndef _WINDOWS
    signal(SIGQUIT, mplex::bye_signal);
    signal(SIGHUP, mplex::bye_signal);
    signal(SIGINT, mplex::bye_signal);
    signal(SIGTERM, mplex::bye_signal);
    signal(SIGALRM, mplex::alarm_signal);
#endif
    /* MS2020 Websockets test hack */
    mplex::translate_init();

    slog(LOG_ALL, 0, "MPlex compiled with [%s]", get_compiled_hash_defines().c_str());
    slog(LOG_OFF, 0, "Opening mother connection on port %d.", mplex::g_mplex_arg.nMotherPort);

    if (mplex::g_mplex_arg.bWebSockets)
    {
        /* MS2020 Websockets test hack */
        std::thread t1(mplex::runechoserver);
        t1.detach();
    }
    else
    {
        fd = mplex::OpenMother(mplex::g_mplex_arg.nMotherPort);
        Assert(fd != -1, "NO MOTHER CONNECTION.");

        if (mplex::g_MotherHook.tfd() != -1)
        {
            slog(LOG_ALL, 0, "Hook() in main called with a non -1 fd.");
        }

        g_CaptainHook.Hook(fd, &mplex::g_MotherHook);
    }

    /* Subtract stdout, stdin, stderr, fdmud, fdmother and 2 to be safe. */
#if defined(_WINDOWS)
    g_nActiveConnections = 256 - 3 - 2 - 2;
#else
    mplex::g_nConnectionsLeft = getdtablesize() - 3 - 2 - 2;
#endif
    mplex::Control();

    mplex::g_MudHook.Unhook();
    mplex::g_MotherHook.Unhook();

    return 0;
}
