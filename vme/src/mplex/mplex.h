/*
 $Author: tperry $
 $RCSfile: mplex.h,v $
 $Date: 2001/04/10 21:11:37 $
 $Revision: 2.0 $
 */
#pragma once

#include "color.h"
#include "essential.h"
#include "hook.h"
#include "network.h"
#include "protocol.h"
#include "queue.h"

#include <cstring>
#include <mutex>

namespace mplex
{

struct arg_type
{
    int nMotherPort;
    int nMudPort;
    char *pAddress;
    int g_bModeANSI;
    int g_bModeEcho;
    int g_bModeRedraw;
    int g_bModeTelnet;
    int g_bModeRawHTML;
    int bWebSockets;
    int bMudProtocol;
    int bForceAscii;
    bool g_bUseTLS;  ///< -t: with -w, wss:// instead of ws://
    bool bTelnetTLS; ///< -z: TLS on the telnet port (telnets)
};

#define Assert(a, b)                                                                                                                       \
    do                                                                                                                                     \
    {                                                                                                                                      \
        if (!(a))                                                                                                                          \
        {                                                                                                                                  \
            fprintf(stderr, "%s\n", b);                                                                                                    \
            assert(a);                                                                                                                     \
        }                                                                                                                                  \
    } while (0);

extern int g_bHadAlarm;
extern arg_type g_mplex_arg;
extern char g_mudname[50];

// The hooks and everything they reach (their queues, g_connection_list, g_MudHook) are used
// by two threads in websocket mode: the main thread's select() loop and the websocket thread.
// This lock makes them take turns. The main thread holds it all the time, except while it
// waits in select() or sleeps; the websocket thread takes it for each event it handles.
extern std::mutex g_hookLock;

// sleep() for the main thread, which lets the websocket thread work meanwhile
void sleep_unlocked(unsigned int seconds);

int ParseArg(int argc, char *argv[], arg_type *arg);
void bye_signal(int signal);
void alarm_signal(int sig);

} // namespace mplex
