/*
 $Author: All $
 $RCSfile: protocol.cpp,v $
 $Date: 2003/12/01 00:19:50 $
 $Revision: 2.3 $
 */

#ifdef _WINDOWS
#include <winsock2.h>
#else
#include <unistd.h>
#endif

#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "essential.h"
#include "protocol.h"
#include "utility.h"
#include "db_file.h"

//
// This handles all protocol interpretation between the MUD and the Mplex.
//
//

/* Create the standard header and send a close request to file             */
/* file descriptor 'fd'. If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */
void protocol_send_close(cHook *Hook, ubit16 id)
{
    ubit16 len = 0;
    ubit8 buf[20];

    if (!Hook->IsHooked())
        return;

    memcpy(&(buf[0]), MULTI_TERMINATE, 2);
    memcpy(&(buf[2]), &id, sizeof(id));
    memcpy(&(buf[4]), &len, sizeof(len));

    Hook->Write(buf, 6);
}


/* Create the standard header and send a ping to the remote mplex          */
void protocol_send_ping(cHook *Hook)
{
    ubit8 buf[20];

    if (!Hook->IsHooked())
    {
        slog(LOG_ALL, 0, "protocol_send_ping() was UNHOOKED");
        return;
    }

    // slog(LOG_ALL, 0, "protocol_send_ping() transmitted");
    memcpy(&(buf[0]), MULTI_PING, 6);

    Hook->Write(buf, 6);
}


/* Create the standard header and send a connection confirm to             */
/* file descriptor 'fd'. If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */
void protocol_send_confirm(cHook *Hook, ubit16 id)
{
    ubit16 len = 0;
    ubit8 buf[20];

    if (!Hook->IsHooked())
    {
        //slog(LOG_ALL, 0, "protocol_send_confirm() was UNHOOKED");
        return;
    }

    //slog(LOG_ALL, 0, "protocol_send_confirm() ID %d", id);

    memcpy(&(buf[0]), MULTI_CONNECT_CON, 2);
    memcpy(&(buf[2]), &id, sizeof(id));
    memcpy(&(buf[4]), &len, sizeof(len));

    Hook->Write(buf, 6);
}

/* Create the standard header and send a connection request to             */
/* file descriptor 'fd'. If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */
void protocol_send_request(cHook *Hook)
{
    ubit16 id = 0, len = 0;
    ubit8 buf[10];

    if (!Hook->IsHooked())
    {
        //slog(LOG_ALL, 0, "protocol_send_request() was UNHOOKED");
        return;
    }

    //slog(LOG_ALL, 0, "protocol_send_request()");
    memcpy(&(buf[0]), MULTI_CONNECT_REQ, 2);
    memcpy(&(buf[2]), &id, sizeof(id));
    memcpy(&(buf[4]), &len, sizeof(len));

    Hook->Write(buf, 6);
}


/* Create the standard header and send a connection request to             */
/* file descriptor 'fd'. If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */
void protocol_send_mplex_info(cHook *Hook, ubit8 bWebsockets)
{
    ubit16 id = 0, len = 0;
    ubit8 buf[10];

    if (!Hook->IsHooked())
    {
        //slog(LOG_ALL, 0, "protocol_send_request() was UNHOOKED");
        return;
    }

    //slog(LOG_ALL, 0, "protocol_send_request()");
    len = 1;
    memcpy(&(buf[0]), MULTI_MPLEX_INFO, 2);
    memcpy(&(buf[2]), &id, sizeof(id));
    memcpy(&(buf[4]), &len, sizeof(len));
    memcpy(&(buf[6]), &bWebsockets, 1);

    Hook->Write(buf, 7);
}


/* Create the standard header and send (from mplex to server) the host     */
/* name information.     If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */
void protocol_send_host(cHook *Hook, ubit16 id, const char *host,
                        ubit16 nPort, ubit8 nLine)
{
    ubit16 len = 0;
    ubit8 buf[80];
    char ptext[1024] = "UNKNOWN HOST";
    ubit8 *b;

    if (!Hook->IsHooked())
        return;

    if (host)
    {
        memset(ptext, 0, 1024);
        strcpy(ptext, host);
        ptext[49] = 0;
    }

    len = strlen(ptext) + 4;

    b = buf;

    memcpy(b, MULTI_HOST, 2);
    b += 2;

    bwrite_ubit16(&b, id);
    bwrite_ubit16(&b, len);
    bwrite_ubit16(&b, nPort);
    bwrite_ubit8(&b, nLine);
    bwrite_string(&b, ptext);

    Hook->Write(buf, b - buf);
}

/* Create the standard header and send text to                              */
/* file descriptor 'fd'. If fd is -1 then don't send anything.              */
/* 'type' is used to tell difference between normal text, page string text, */
/* etc.                                                                     */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                 */
void protocol_send_text(cHook *Hook, const ubit16 id,
                        const char *text, const ubit8 type)
{
#define MAX_TEXT_LEN (32768)
    ubit16 len, txlen;
    ubit8 buf[6 + MAX_TEXT_LEN];
    char c;

    assert(id != 0);

    if (!Hook->IsHooked())
        return;

    len = strlen(text) + 1;

    txlen = MIN(MAX_TEXT_LEN, len);

    if (txlen < len)
    {
        /* Fragmented text, break it up at a newline for
           indentation purposes! (if possible) */
        slog(LOG_ALL, 0, "WARNING: protcol_send_text() needed to break up text %d long", len);

        for (; txlen != 0; txlen--)
        {
            c = text[txlen - 1];
            if ((c == '>') || ISNEWL(c))
                break;
        }

        if (txlen == 0)
            txlen = MIN(MAX_TEXT_LEN, len);
    }

    assert(txlen > 0);

    buf[0] = MULTI_UNIQUE_CHAR;
    buf[1] = type;
    memcpy(&(buf[2]), &id, sizeof(id));
    memcpy(&(buf[4]), &txlen, sizeof(txlen));
    memcpy(&(buf[6]), text, txlen);

    Hook->Write(buf, 6 + txlen);

    len -= txlen;

    if (len > 0)
        protocol_send_text(Hook, id, &text[txlen], type);
#undef MAX_TEXT_LEN
}

/* Create the standard header and send setup information from the server   */
/* to the mplex'er.                                                        */
/* file descriptor 'fd'. If fd is -1 then don't send anything.             */
/* Return -1 on socket fail, 0 on amount fail, 1 on success                */

void protocol_send_setup(cHook *Hook, ubit16 id,
                         struct terminal_setup_type *setup)
{
    ubit16 len;
    ubit8 buf[sizeof(struct terminal_setup_type) + 6 + 4];

    assert(id != 0);

    if (!Hook->IsHooked())
        return;

    len = sizeof(struct terminal_setup_type);

    memcpy(&(buf[0]), MULTI_SETUP, 2);
    memcpy(&(buf[2]), &id, 2);
    memcpy(&(buf[4]), &len, 2);
    memcpy(&(buf[6]), setup, sizeof(struct terminal_setup_type));

    Hook->Write(buf, 6 + len);
}


// Send the MUD name to the mplex
void protocol_send_exchange(cHook *Hook, ubit16 id, char *mudname)
{
    ubit16 len;
    ubit8 buf[MULTI_MAX_MUDNAME + 1 + 6 + 4];

    assert(strlen(mudname) < MULTI_MAX_MUDNAME);

    id = 0;
    if (!Hook->IsHooked())
        return;

    len = strlen(mudname) + 1;
    slog(LOG_ALL, 0, "Sending Mudname %s, Len %d", mudname, len);

    memcpy(&(buf[0]), MULTI_EXCHANGE, 2);
    memcpy(&(buf[2]), &id, 2);
    memcpy(&(buf[4]), &len, 2);
    memcpy(&(buf[6]), mudname, strlen(mudname) + 1);

    Hook->Write(buf, 6 + len);
}

// Send the default colors to the Mplex
void protocol_send_color(cHook *Hook, ubit16 id, char *colorstr)
{
    ubit16 len;
    ubit8 buf[(MAX_STRING_LENGTH * 2) + 11];

    assert(strlen(colorstr) < MAX_STRING_LENGTH * 2);

    id = 0;
    if (!Hook->IsHooked())
        return;

    len = strlen(colorstr) + 1;
    slog(LOG_ALL, 0, "Sending Default Colors to multihost.");

    memcpy(&(buf[0]), MULTI_COLOR, 2);
    memcpy(&(buf[2]), &id, 2);
    memcpy(&(buf[4]), &len, 2);
    memcpy(&(buf[6]), colorstr, strlen(colorstr) + 1);

    Hook->Write(buf, 6 + len);
}

/* Data is assumed ready on 'fd' and it is interpreted. Any of the three   */
/* pointers can be set to NIL if desired.                                  */
/*                                                                         */
/* fd        : Socket to read from.                                        */
/* pid       : Will be set to the destination id.                          */
/* plen      : The length of what ever data is received.                   */
/* str       : Set to point to the malloced data (if any)                  */
/*                                                                         */
/* Returns                                                                 */
/*       0 if there is nothing more to do (i.e. queue empty)               */
/*      -1 on socket fail                                                  */
/*      -2 on protocol fail                                                */
/* or type on success                                                      */
/*                                                                         */
int protocol_parse_incoming(cHook *Hook, ubit16 *pid,
                            ubit16 *plen, char **str, ubit8 *text_type)
{
    int n;
    ubit16 id;
    ubit16 len;
    char buf[10];
    char *data;

    if (str)
        *str = NULL;

    if (!Hook->IsHooked())
        return 0;

    n = Hook->ReadToQueue();

    if (n == -1)
    {
        slog(LOG_ALL, 0, "Protocol: parse_incoming error.");
        return -1;
    }

    if (Hook->qRX.Bytes() < 6)
        return 0;

    Hook->qRX.Copy((ubit8 *)buf, 6);

    if (buf[0] != MULTI_UNIQUE_CHAR)
    {
        slog(LOG_ALL, 0, "Illegal unexpected unique multi character #2.");
        Hook->Unhook();
        return -2;
    }

    memcpy(&id, &(buf[2]), sizeof(ubit16));
    memcpy(&len, &(buf[4]), sizeof(sbit16));

    if (Hook->qRX.Bytes() - 6 < len)
    {
        // slog(LOG_ALL, 0, "Short of data...");
        return 0; /* We havn't got all the data yet! */
    }

    Hook->qRX.Cut(6);

    if (pid)
        *pid = id;

    if (plen)
        *plen = len;

    switch (buf[1])
    {
    case MULTI_MPLEX_INFO_CHAR:
        break; // Get the data (down below)

    case MULTI_PING_CHAR:
        if (id != 0)
        {
            slog(LOG_ALL, 0, "Received none-zero ID on a ping");
            return -2;
        }
        //slog(LOG_ALL, 0, "Ping received");
        return buf[1];

    case MULTI_TERMINATE_CHAR:
        if (id == 0)
        {
            slog(LOG_ALL, 0, "Received ID zero on a terminate request!");
            return -2;
        }
        return buf[1];

    case MULTI_CONNECT_CON_CHAR:
        //slog(LOG_ALL, 0, "MULTI_CONNECT_CON_CHAR protocol_parse_incoming() ID=%d", id);
        if (id == 0)
        {
            slog(LOG_ALL, 0, "ID 0 on connection confirm.");
            return -2;
        }
        return buf[1];

    case MULTI_CONNECT_REQ_CHAR:
        //slog(LOG_ALL, 0, "MULTI_CONNECT_REQ_CHAR protocol_parse_incoming()", id);
        if (id != 0)
        {
            slog(LOG_ALL, 0, "Received non-zero ID on a connection request!");
            return -2;
        }
        return buf[1];

    case MULTI_HOST_CHAR:
        if (id == 0)
        {
            slog(LOG_ALL, 0, "Received zero ID on a host name transfer!");
            return -2;
        }
        if (len <= 0)
        {
            slog(LOG_ALL, 0, "Received 0 length host information.");
            return -2;
        }
        break; /* Get text */

    case MULTI_SETUP_CHAR:
        if (id == 0)
        {
            slog(LOG_ALL, 0, "Received setup from ID zero!");
            return -2;
        }
        break; /* Get data */

    case MULTI_EXCHANGE_CHAR:
        if (id != 0)
        {
            slog(LOG_ALL, 0, "ID must be 0 on data exchange!");
            return -2;
        }
        break; /* Get data */

    case MULTI_COLOR_CHAR:
        if (id != 0)
        {
            slog(LOG_ALL, 0, "ID must be 0 on data exchange!");
            return -2;
        }
        break; /* Get data */

    case MULTI_TEXT_CHAR:
    case MULTI_PAGE_CHAR:
    case MULTI_PROMPT_CHAR:
        if (id == 0)
            slog(LOG_ALL, 0, "Received text from ID zero!");
        break; /* Get text */

    default:
        slog(LOG_ALL, 0, "Illegal unexpected unique multi character.#3");
        return -2;
    }

    /* Get extra data into "text" buffer */
    assert(len > 0);

    CREATE(data, char, len + 1);

    Hook->qRX.CutCopy((ubit8 *)data, len);

    data[len] = 0;

    *str = data;

    return buf[1];
}
