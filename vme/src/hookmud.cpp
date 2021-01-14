#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/resource.h>

#include <netinet/tcp.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "structs.h"
#include "utils.h"
#include "system.h"
#include "db.h"
#include "utility.h"
#include "comm.h"
#include "textutil.h"
#include "ban.h"
#include "handler.h"
#include "files.h"
#include "protocol.h"
#include "main.h"
#include "account.h"
#include "vmelimits.h"
#include "db_file.h"
#include "str_parse.h"
#include "common.h"
#include "dilrun.h"
#include "hookmud.h"

class cMotherHook MotherHook;

/* ----------------------------------------------------------------- */
/*                                                                   */
/*                        Multi-Protocol-Procedures                  */
/*                                                                   */
/* ----------------------------------------------------------------- */

//
// The MultiHook contains up to 10 Hook classes. Each can be an 
// established Mplex connection. When data is received (Input)
// the mplex protocol data is parsed in Read()
// MotherHook sets up each multi and adds to CaptainHook
// which in turn calls cMultiMaster::Input when data is received.
//

cMultiMaster::cMultiMaster(void)
{
    nCount = 0;
}

cMultiHook::cMultiHook(void)
{
    succ_err = 0;
    bWebsockets = 0;
}

void cMultiHook::Input(int nFlags)
{
    if (nFlags & SELECT_EXCEPT)
    {
        slog(LOG_ALL, 0, "Freaky multi!");
        Close();
    }
    if (nFlags & SELECT_READ)
    {
        int n;

        for (;;)
        {
            n = Read();
            if ((n == 0) || (n == -1))
                break;
        }

        if (n == -1)
        {
            slog(LOG_OFF, 0, "ERROR READING FROM MPLEX.");
            Close();
        }
        else if (n == -2)
        {
            slog(LOG_OFF, 0, "MPLEX PROTOCOL ERROR.");
        }
    }
}


void cMultiHook::Ping(void)
{
    protocol_send_ping(this);
}

void cMultiHook::Unhook(void)
{
    if (this->IsHooked())
    {
        slog(LOG_OFF, 0, "Unhooking MultiHook");
        CaptainHook.Unhook(this);
        assert(this->fd != -1);
        cHook::Unhook();
    }
}

void cMultiHook::Close(void)
{
    slog(LOG_ALL, 0, "Closing connection to multi host.");

    if (this->IsHooked())
        Unhook();

    Multi.nCount--;
}


// Get the data and parse the mplex protocol
int cMultiHook::Read(void)
{
    class descriptor_data *d = NULL;
    int p;
    ubit16 id;
    ubit16 len;
    char *data;
    ubit8 text_type;

    p = protocol_parse_incoming(this, &id, &len, &data, &text_type);

    if (p <= 0)
        return p;

    if (id != 0)
    {
        for (d = descriptor_list; d; d = d->next)
            if (d->id == id)
            {
                assert(d->multi == this);
                break;
            }

        if (d == NULL)
        {
            /* This could perhaps occur if a connected player issues a command
               simultaneously with the server throwing the person out.
               I will simply test if d == NULL in all cases below and then
               ignore the message. */

            slog(LOG_ALL, 0, "No destination ID (%d).", id);

            if (succ_err++ > 5)
            {
                slog(LOG_ALL, 0, "Lost track of multi stream.");
                Close();
                if (data)
                    FREE(data);
                return -1;
            }
        }
        else
            succ_err = 0;
    }

    switch (p)
    {
    case MULTI_MPLEX_INFO_CHAR:
        bWebsockets = data[0]; // data[0] is 1 if websockets
        if (data)
            FREE(data);
        break;

    case MULTI_PING_CHAR:
        if (data)
            FREE(data);
        break;

    case MULTI_TERMINATE_CHAR:
        /* This is very nice, but it prevents descriptor_close to send
           a connection_close to the mplex'er */
        if (d)
            descriptor_close(d, FALSE);
        if (data)
            FREE(data);
        break;

    case MULTI_CONNECT_REQ_CHAR:
        d = descriptor_new(this);
        protocol_send_confirm(this, d->id);
        send_to_descriptor(g_cServerConfig.m_pLogo, d);
        send_to_descriptor("By what name do they call you? ", d);
        break;

    case MULTI_HOST_CHAR:
        if (d && data)
        {
            ubit8 *b = (ubit8 *)data;

            d->nPort = bread_ubit16(&b);
            d->nLine = bread_ubit8(&b);
            strncpy(d->host, (char *)b, sizeof(d->host));
            d->host[sizeof(d->host) - 1] = 0;
        }
        if (data)
            FREE(data);
        break;

    case MULTI_TEXT_CHAR:
        if (d)
        {
            // Important to HTML encode any string received from the Mplex
            // for security and other reasons.
            //
            char *mystr = html_encode_utf8(data);

            d->qInput.Append(new cQueueElem(mystr, FALSE));
            if (data)
                FREE(data);
        }
        /* Kept in queue */
        break;

    default:
        slog(LOG_ALL, 0, "Illegal unexpected unique multi character #1.");
        Close();
        if (data)
            FREE(data);
        return -1;
    }

    return p;
}

void multi_clear(void)
{
    class descriptor_data *nextd, *d;

    for (d = descriptor_list; d; d = nextd)
    {
        nextd = d->next;
        if (!d->multi->IsHooked())
            descriptor_close(d);
    }
}

void multi_close_all(void)
{
    int i;

    slog(LOG_BRIEF, 0, "Closing all multi connections.");

    for (i = 0; i < MAX_MULTI; i++)
        Multi.Multi[i].Close();

    multi_clear();
}

void multi_ping_all(void)
{
    int i;

    for (i = 0; i < MAX_MULTI; i++)
        if (Multi.Multi[i].IsHooked())
            Multi.Multi[i].Ping();
}

/* ----------------------------------------------------------------- */
/*                                                                   */
/*                         MotherHook                                */
/*                                                                   */
/* ----------------------------------------------------------------- */

//
// init_mother is initially called to open file descriptor on 
// the mother port 4999. This fd is then added to CaptainHook with
// MotherHook. CaptainHook.Wait() will be listening for events on 
// this MotherHook and will trigger Input when data arrives.
//


// Listen for Mplex connections. If one is established then find
// an unhooked Multi and hook it.
void cMotherHook::Input(int nFlags)
{
    if (nFlags & SELECT_EXCEPT)
    {
        slog(LOG_ALL, 0, "Mother connection closed down.");
        Close();
    }

    if (nFlags & SELECT_READ)
    {
        struct sockaddr_in isa;
        int i, t;
        unsigned int len;

        len = sizeof(isa);

        if ((t = accept(this->tfd(), (struct sockaddr *)&isa, &len)) < 0)
        {
            slog(LOG_ALL, 0, "Mother accept error %d", errno);
            return;
        }

        if (!g_cServerConfig.ValidMplex(&isa))
        {
            slog(LOG_ALL, 0, "Mplex not from trusted host, terminating.");
            close(t);
            return;
        }

        i = fcntl(t, F_SETFL, FNDELAY);

        if (i == -1)
            error(HERE, "Noblock");

        int n;
        n = setsockopt(t, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
        if (n == -1)
        {
            error(HERE, "No Setsockopt()");
            exit(21);
        }

        for (i = 0; i < MAX_MULTI; i++)
            if (!Multi.Multi[i].IsHooked())
                break;

        if ((i >= MAX_MULTI) || Multi.Multi[i].IsHooked())
        {
            slog(LOG_ALL, 0, "No more multi connections allowed");
            close(t);
            return;
        }

        CaptainHook.Hook(t, &Multi.Multi[i]);

        Multi.nCount++;

        slog(LOG_ALL, 0, "A multi-host has connected to the game.");
        protocol_send_exchange(&Multi.Multi[i], 0, g_cServerConfig.m_mudname);
        protocol_send_color(&Multi.Multi[i], 0, g_cServerConfig.m_pColor);
    }
}


void cMotherHook::Unhook(void)
{
    if (this->IsHooked())
    {
        slog(LOG_OFF, 0, "Unhooking Mother Hook");
        CaptainHook.Unhook(this);
        assert(this->fd != -1);
        cHook::Unhook();
    }
}

// If the mother port 4999 is closed then close all mplex connections
void cMotherHook::Close(void)
{
    if (this->IsHooked())
        Unhook();

    multi_close_all();
}

void init_mother(int nPort)
{
    int n, fdMother;
    struct linger ld;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(nPort);

    fdMother = socket(AF_INET, SOCK_STREAM, 0);

    if (fdMother == -1)
    {
        slog(LOG_OFF, 0, "Can't open Mother Connection");
        exit(0);
    }

    n = 1;
    if (setsockopt(fdMother, SOL_SOCKET, SO_REUSEADDR, (char *)&n, sizeof(n)) < 0)
    {
        close(fdMother);
        slog(LOG_OFF, 0, "OpenMother setsockopt REUSEADDR");
        exit(0);
    }

    ld.l_onoff = 0;
    ld.l_linger = 1000;

    if (setsockopt(fdMother, SOL_SOCKET, SO_LINGER, (char *)&ld, sizeof(ld)) < 0)
    {
        close(fdMother);
        slog(LOG_OFF, 0, "OpenMother setsockopt LINGER");
        exit(0);
    }

    n = bind(fdMother, (struct sockaddr *)&server_addr,
             sizeof(struct sockaddr_in));

    if (n != 0)
    {
        close(fdMother);
        slog(LOG_OFF, 0, "Can't bind Mother Connection port %d (errno %d).",
             nPort, errno);
        exit(0);
    }

    n = listen(fdMother, 5);

    if (n != 0)
    {
        close(fdMother);
        slog(LOG_OFF, 0, "Can't listen on Mother Connection.");
        exit(0);
    }

    n = fcntl(fdMother, F_SETFL, FNDELAY);

    if (n == -1)
    {
        close(fdMother);
        slog(LOG_OFF, 0, "Non blocking set error.");
        exit(22);
    }

    int i=0;
    n = setsockopt(fdMother, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
    if (n == -1)
    {
        close(fdMother);
        slog(LOG_OFF, 0, "Setsockopt TCP_NODELAY failed.");
        exit(23);
    }

    CaptainHook.Hook(fdMother, &MotherHook);
}
