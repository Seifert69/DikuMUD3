/*
 $Author: All $
 $RCSfile: mplex.cpp,v $
 $Date: 2003/12/31 01:21:59 $
 $Revision: 2.4 $
 */

/** @mainpage Multiplexer
 *
 * @section intro_sec Introduction
 *
 * This is the introduction.
 *
 * @authors All
 * @date 2003/12/31 01:21:59
 * @remark This text is located in mplex/main.cpp
 */
#ifdef _WINDOWS
    #include "string.h"
    #include "telnet.h"
    #include "winbase.h"
    #include "winsock2.h"

    #include <time.h>
#endif

#include "ClientConnector.h"
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

#include <algorithm>
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

    slog(LOG_ALL, 0, "mplex compiled with [%s]", get_compiled_hash_defines().c_str());

    if (mplex::g_mplex_arg.bWebSockets)
    {
        /* MS2020 Websockets test hack */
        slog(LOG_OFF, 0, "Creating WebSocket thread, TLS = %s", mplex::g_mplex_arg.g_bUseTLS ? "true" : "false");
        std::thread t1(mplex::runechoserver);
        t1.detach();
        slog(LOG_OFF, 0, "WebSocket thread created and detached");
        
        // In WebSocket mode, we don't connect to MUD server - WebSocket server handles clients directly
        slog(LOG_OFF, 0, "WebSocket mode: skipping MUD server connection");
    }
    else
    {
        if (mplex::g_mplex_arg.bTelnetTLS)
        {
            // Not Assert(): with NDEBUG that carries on, and the port would
            // serve clear text while claiming to be TLS.
            if (!mplex::tls_init_telnet("server.pem"))
            {
                fprintf(stderr, "TLS: cannot load ./server.pem, see the log.\n");
                exit(1);
            }
            slog(LOG_OFF, 0, "Telnet port %d is TLS (telnets), certificate from ./server.pem", mplex::g_mplex_arg.nMotherPort);
        }
        else if (mplex::g_mplex_arg.g_bUseTLS)
        {
            slog(LOG_OFF, 0, "-t without -w does nothing; port %d is plain telnet. -z makes it TLS (telnets).", mplex::g_mplex_arg.nMotherPort);
        }

        slog(LOG_OFF, 0, "Opening telnet mother connection on port %d.", mplex::g_mplex_arg.nMotherPort);
        fd = mplex::OpenMother(mplex::g_mplex_arg.nMotherPort);
        Assert(fd != -1, "NO MOTHER CONNECTION.");

        if (mplex::g_MotherHook.tfd() != -1)
        {
            slog(LOG_ALL, 0, "Hook() in main called with a non -1 fd.");
        }

        if (!g_CaptainHook.Hook(fd, &mplex::g_MotherHook))
        {
            fprintf(stderr, "Can't watch the mother connection, fd %d.\n", fd);
            exit(1);
        }
    }

    /* Subtract stdout, stdin, stderr, fdmud, fdmother, the spare fd and 1 to be safe. */
#if defined(_WINDOWS)
    g_nActiveConnections = 256 - 3 - 2 - 2;
#else
    // Not past FD_SETSIZE however high the fd limit: select() can't watch more
    mplex::g_nConnectionsLeft = std::min(getdtablesize(), FD_SETSIZE) - 3 - 2 - 2;
#endif
    mplex::Control();

    mplex::g_MudHook.Unhook();
    mplex::g_MotherHook.Unhook();

    return 0;
}
