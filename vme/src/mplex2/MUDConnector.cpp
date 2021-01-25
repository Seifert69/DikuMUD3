/*
 $Author: All $
 $RCSfile: MUDConnector.cpp,v $
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
#include "ClientConnector.h"
#include "MUDConnector.h"

class color_type g_cDefcolor;
int g_nConnectionsLeft = -1;

// Listening for new telnet connections on def. 4242
class cMotherHook MotherHook;

// Sending and receiving data to the MUD server
class cMudHook MudHook;

//
// The MotherHook: listens for new connections on the client port (4242)
// The MUDHook: connects to the MUD server
// The Control() loop keeps listening for new connections as well as
// read / writing data to and from the MUDHook.
//

void mud_went_down(void); // Circular reference

void alarm_check(int i)
{
    if (g_bHadAlarm)
    {
        slog(LOG_OFF, 0, "Alarm checkpoint %d.", i);
        g_bHadAlarm = FALSE;
    }
}

// Open connection to the MUD erver and alert all connected clients if OK.
void test_mud_up(void)
{
    int fd;
    class cConHook *nextcon, *con;
    char buf[200];
    static int nRetries = 0;

    fd = OpenNetwork(mplex_arg.nMudPort, mplex_arg.pAddress);

    // If this acts up then it might be because of multithreading issues
    // from the clientconnector.

    if (fd == -1)
    {
#if defined(_WINDOWS)
        Sleep(1000);
#else
        sleep(1);
#endif
        slog(LOG_OFF, 0, "Unable to connect to MUD server %s:%d", mplex_arg.pAddress, mplex_arg.nMudPort);
        nRetries++;
        if (nRetries > 600)
        {
            slog(LOG_OFF, 0, "Giving up after %d attempts", nRetries);
            exit(1);
        }
        return;
    }

    nRetries = 0;
    slog(LOG_OFF, 0, "Stream to server was re-opened!");

    if (MudHook.tfd() != -1)
        slog(LOG_ALL, 0, "test mud up called with a non -1 fd.");

    CaptainHook.Hook(fd, &MudHook);
    //assert(MudHook.IsHooked());

    protocol_send_mplex_info(&MudHook, mplex_arg.bWebSockets);

    for (con = g_connection_list; con; con = nextcon)
    {
        nextcon = con->m_pNext;
        sprintf(buf, "%s has begun rebooting... please wait.<br/>", mudname);
        con->SendCon(buf);

        con->m_pFptr = dumbMenuSelect;
        con->m_nState = 0;
        con->m_qInput.Flush();
        con->m_pFptr(con, "");
    }
}

// Alert all connected clients that the MUD server is unavailable
void mud_went_down(void)
{
    class cConHook *con;
    char buf[200];

    if (MudHook.IsHooked())
        MudHook.Unhook();

    for (con = g_connection_list; con; con = con->m_pNext)
    {
        sprintf(buf, "The connection to %s was broken. "
                     "Please be patient...<br/>",
                mudname);
        con->SendCon(buf);
        con->m_nId = 0;
        con->m_pFptr = dumbMudDown;
        con->m_nState = 0;
    }
}

// Main MPLEX loop called by main() in mplex.cpp
void Control(void)
{
    static int tries = 0;
    int n;

    for (;;)
    {
        if (!MotherHook.IsHooked())
            return;

        if (!MudHook.IsHooked())
        {
            if (tries++ > 600)
            {
                slog(LOG_ALL, 0, "Giving up connecting to mud, retrying...");
                return;
            }
#if defined(_WINDOWS)
            Sleep(1000);
#else
            sleep(1);
#endif
            test_mud_up(); /* Need to do this first... */

            if (mplex_arg.bWebSockets)
            {
                slog(LOG_ALL, 0, "Waiting for a MUDHook to get reconnected...");
                continue; // See bug description below. Dont wait until MUDHook is reestablished. 
            }
        }
        else
            tries = 0;

        ClearUnhooked(); /* Clear all closed down connections */

        //
        // Took me forever to find this bug. When run in websockets mode
        // if the MUDHook is closed then only the motherport is open (4242).
        // Since we're on Websockets and no traffic comes in now on 4242,
        // CaptainHook will be stuck forever in Wait(). In telnet mode that's
        // not a problem because somebody will either trigger an existing connection
        // or telnet to get a new connection. Then Wait() will finish and a new
        // round begins. 
        n = CaptainHook.Wait(NULL);

        if (n == -1)
        {
            slog(LOG_ALL, 0, "Select error (errno %d)", errno);
            return;
        }
    }
}

// ====================== MotherHook (Listening to TCP port)

void cMotherHook::Unhook(void)
{
    CaptainHook.Unhook(this);
}


int cMotherHook::IsHooked(void)
{
    if (mplex_arg.bWebSockets)
        return TRUE;
    return cHook::IsHooked();
}

void cMotherHook::Input(int nFlags)
{
    if (nFlags & SELECT_EXCEPT)
    {
        slog(LOG_OFF, 0, "MOTHER CONNECTION CLOSED DOWN!");
        this->Unhook();
    }
    else if (nFlags & SELECT_READ)
    {
        class cConHook *con = new cConHook();

        if (g_nConnectionsLeft <= 1)
        {
            slog(LOG_OFF, 0, "Can't accept more connections.");
            con->SendCon("This server has no more available connections, try "
                         "another port.<br/>");
        }
        else
        {
            con->m_pFptr(con, "");
        }
    }
}

// ====================== MUD Hook (communicating with the MUD server)

// Will keep reading from MUD forever, until error
void cMudHook::Input(int nFlags)
{
    //slog(LOG_OFF, 0, "cMudHook: Input()");

    if (nFlags & SELECT_EXCEPT)
    {
        slog(LOG_OFF, 0, "MUD CONNECTION CLOSED DOWN!");
        mud_went_down();
    }
    else if (nFlags & SELECT_READ)
    {
        int n;

        for (;;)
        {
            n = read_mud();
            if ((n == 0) || (n == -1))
                break;
        }

        if (n == -1)
        {
            slog(LOG_OFF, 0, "ERROR READING FROM MUD.");
            mud_went_down();
        }
        else if (n == -2)
        {
            slog(LOG_OFF, 0, "PROTOCOL ERROR.");
        }
        /*else
            slog(LOG_OFF, 0, "cMudHook: n = %d", n);*/
    }
}

/*
#define Assert(a,b) \
   do { if (!(a)) {fprintf(stderr, "%s\n", b); assert(a);} } while (0);

class cConHook *connection_list = NULL;

char Outbuf[32768];
*/
void cMudHook::Unhook(void)
{
    if (this->IsHooked())
    {
        slog(LOG_OFF, 0, "Unhooking MUD Hook");
        CaptainHook.Unhook(this);
        assert(this->fd != -1);
        cHook::Unhook();
    }
}

int cMudHook::read_mud(void)
{
    class cConHook *con;
    ubit16 id, len;
    int p, n;
    char *data;
    ubit8 text_type;

    p = protocol_parse_incoming(this, &id, &len, &data, &text_type);
    //slog(LOG_OFF, 0, "read_mud(): p=%d. len=%d", p, len);

    if (p <= 0)
        return p;

    switch (p)
    {
    case MULTI_PING_CHAR:
        break;

    case MULTI_EXCHANGE_CHAR:
        memcpy(mudname, data, len);
        break;

    case MULTI_COLOR_CHAR:
        g_cDefcolor.create(data);
        break;

    case MULTI_SETUP_CHAR:
        if (len != sizeof(struct terminal_setup_type))
        {
            slog(LOG_OFF, 0, "Illegal size m_sSetup received.");
            break;
        }

        for (con = g_connection_list; con; con = con->m_pNext)
        {
            if (con->m_nId == id)
            {
                n = (con->m_sSetup.emulation == TERM_INTERNAL);

                memcpy(&(con->m_sSetup), data, len);

                if (n)
                    con->m_sSetup.emulation = TERM_INTERNAL;

                assert(is_in(con->m_sSetup.emulation, TERM_DUMB, TERM_INTERNAL));

                assert(con->m_sSetup.width >= 40 &&
                       con->m_sSetup.width <= 160);

                assert(con->m_sSetup.height >= 15 &&
                       con->m_sSetup.height <= 60);

                // MS2020 xxx con->SendCon (CONTROL_RESET CONTROL_FG_WHITE CONTROL_BG_BLACK);
                n = 0;
                break;
            }
        }
        if (con == NULL)
            slog(LOG_OFF, 0, "ERROR: Unknown ID m_sSetup received.");
        break;

    case MULTI_TERMINATE_CHAR:
        if (data)
            free(data);
        for (con = g_connection_list; con; con = con->m_pNext)
        {
            if (con->m_nId == id)
            {
                con->SendCon("Your connection was requested terminated.<br/>");
                con->Close(FALSE);
                return 1;
            }
        }
        if (con == NULL)
            slog(LOG_OFF, 0, "ERROR: Unknown ID requested terminated.");
        return 1;

    case MULTI_CONNECT_CON_CHAR:
        //slog(LOG_OFF,0,"MULTI_CON_CHAR: Received id=%d", id);
        if (data)
            free(data);
        for (con = g_connection_list; con; con = con->m_pNext)
        {
            // Only pick those waiting in the PlayLoop, not those new connections
            if (con->m_nId == 0)
            {
                //slog(LOG_OFF,0,"MULTI_CON_CHAR: Found cCOnHook with zero ID");
                if (con->m_pFptr == dumbPlayLoop)
                {
                    slog(LOG_OFF,0,"MULTI_CON_CHAR: Connected zero id to %d", id);
                    assert(id != 0);
                    con->m_nId = id;
                    protocol_send_host(&MudHook, id, con->m_aHost,
                                    mplex_arg.nMotherPort, con->m_nLine);
                    break;
                }
            }
        }
        if (con == NULL)
        {
            slog(LOG_OFF,0,"Unknown destination for confirm! Requesting term.");
            protocol_send_close(&MudHook, id);
        }
        return 1;

    case MULTI_TEXT_CHAR:
    case MULTI_PAGE_CHAR:
    case MULTI_PROMPT_CHAR:
        if (len == 0)
            break;

        for (con = g_connection_list; con; con = con->m_pNext)
        {
            if (id == con->m_nId)
            {
                char *parsed;

                parsed = con->ParseOutput(data);

                switch (p)
                {
                case MULTI_PAGE_CHAR:
                    // Dont page to the web-client - it pops up an overlay instead.
                    if (con->m_pWebsServer)
                        con->Write((ubit8 *)parsed, strlen(parsed));
                    else
                    {
                        con->m_qPaged.Append(new cQueueElem(parsed));
                        con->ProcessPaged();
                    }
                    break;

                case MULTI_PROMPT_CHAR:
                    con->PromptRedraw(parsed);
                    break;

                default:
                    con->Write((ubit8 *)parsed, strlen(parsed));
                    break;
                }
                break;
            }
        }

        if (con == NULL)
        {
            slog(LOG_OFF, 0, "Unknown destination text received.");
        }
        break;

    default:
        abort();
    }

    if (data)
        free(data);

    return 1;
}
