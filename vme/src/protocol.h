/*
 $Author: All $
 $RCSfile: protocol.h,v $
 $Date: 2003/10/09 01:12:32 $
 $Revision: 2.2 $
 */

#ifndef _MUD_PROTOCOL_H
#define _MUD_PROTOCOL_H

#include "essential.h"
#include "hook.h"

#define TERM_DUMB     0
#define TERM_TTY      1
#define TERM_ANSI     2
#define TERM_VT100    3
#define TERM_INTERNAL 4

struct terminal_setup_type
{
    ubit8 redraw;			// Redraw the users prompt?
    ubit8 echo;			    // Echo chars to user?
    ubit8 width, height;	// The colour of his nose
    ubit8 emulation;		// TERM_XXX
    ubit8 telnet;			// Is telnet used?
    ubit8 colour_convert;	// Colour conversion (mapping) used
    ubit8 websockets;       // Using websockets (HTML)?
};

void protocol_send_ping(cHook *Hook);
void protocol_send_close (cHook * Hook, ubit16 id);
void protocol_send_confirm (cHook * Hook, ubit16 id);
void protocol_send_request (cHook * Hook);
void protocol_send_text (cHook * Hook, const ubit16 id,
                         const char *text, const ubit8 type);
void protocol_send_setup (cHook * Hook, ubit16 id,
                          struct terminal_setup_type *setup);
void protocol_send_exchange (cHook * Hook, ubit16 id, char *mudname);
void protocol_send_color (cHook * Hook, ubit16 id, char *color);
void protocol_send_host (cHook * Hook, ubit16 id, const char *host,
                         ubit16 nPort, ubit8 nLine);

int protocol_parse_incoming (cHook * Hook, ubit16 * pid, ubit16 * plen,
                             char **str, ubit8 * text_type);
void protocol_send_mplex_info(cHook *Hook, ubit8 bWebsockets);


// These definitions are used by the mplex protocol
#define MULTI_UNIQUE_CHAR '\x01'
#define MULTI_UNIQUE_STR  "\x01"

#define MULTI_CONNECT_REQ_CHAR 'A'
#define MULTI_CONNECT_CON_CHAR 'B'
#define MULTI_TERMINATE_CHAR   'C'
#define MULTI_SETUP_CHAR       'D'
#define MULTI_HOST_CHAR        'E'
#define MULTI_TEXT_CHAR        'F'
#define MULTI_PAGE_CHAR        'G'
#define MULTI_PROMPT_CHAR      'H'
#define MULTI_EXCHANGE_CHAR    'I'
#define MULTI_COLOR_CHAR       'J'
#define MULTI_PING_CHAR        'K'
#define MULTI_MPLEX_INFO_CHAR  'L'

#define MULTI_CONNECT_REQ_STR "A"
#define MULTI_CONNECT_CON_STR "B"
#define MULTI_TERMINATE_STR   "C"
#define MULTI_SETUP_STR       "D"
#define MULTI_HOST_STR        "E"
#define MULTI_TEXT_STR        "F"
#define MULTI_PAGE_STR        "G"
#define MULTI_PROMPT_STR      "H"
#define MULTI_EXCHANGE_STR    "I"
#define MULTI_COLOR_STR       "J"
#define MULTI_PING_STR        "K"
#define MULTI_MPLEX_INFO_STR  "L"

#define MULTI_EXCHANGE     MULTI_UNIQUE_STR MULTI_EXCHANGE_STR "\0\0\0\0"
#define MULTI_COLOR        MULTI_UNIQUE_STR MULTI_COLOR_STR "\0\0\0\0"
#define MULTI_CONNECT_REQ  MULTI_UNIQUE_STR MULTI_CONNECT_REQ_STR "\0\0\0\0"
#define MULTI_CONNECT_CON  MULTI_UNIQUE_STR MULTI_CONNECT_CON_STR
#define MULTI_TERMINATE    MULTI_UNIQUE_STR MULTI_TERMINATE_STR "\0\0\0\0"
#define MULTI_SETUP        MULTI_UNIQUE_STR MULTI_SETUP_STR
#define MULTI_HOST         MULTI_UNIQUE_STR MULTI_HOST_STR "\0\0\0\0"
#define MULTI_PING         MULTI_UNIQUE_STR MULTI_PING_STR "\0\0\0\0"
#define MULTI_MPLEX_INFO   MULTI_UNIQUE_STR MULTI_MPLEX_INFO_STR "\0\0\0\0"


#define MULTI_TEXT         MULTI_UNIQUE_STR MULTI_TXT_STR
#define MULTI_PAGE         MULTI_UNIQUE_STR MULTI_TXT_STR
#define MULTI_PROMPT       MULTI_UNIQUE_STR MULTI_TXT_STR

#define MULTI_MAX_MUDNAME  256

#endif
