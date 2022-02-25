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

#include "ClientConnector.h"
#include "MUDConnector.h"
#include "color.h"
#include "echo_server.h"
#include "essential.h"
#include "formatter.h"
#include "hook.h"
#include "mplex.h"
#include "network.h"
#include "protocol.h"
#include "queue.h"
#include "slog.h"
#include "textutil.h"
#include "translate.h"

#include <arpa/telnet.h>
#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <thread>

/*
   In Mplex main MotherHook is opened and put into CaptainHook.
   Mother listens on 4242 for new players connecting
   in the mplex's Control() loop in MUDConnector.cpp
   which calls CaptainHook.Wait();

    When a new player connects to the mother port,
    CaptainHook.Wait() in turn calls MotherHook.Input().
    MotherHook calls new cConHook() which is the player's
    client connection. The constructor adds itself to the
    g_connection_list list and sets itself to the function
    dumbMenuSelect();
    For a player connecting with Telnet the contructor
    adds the player to CaptainHook.Hook
    For a player connecting with WebSockets the constructor
    stops before that.

    If the connection to the MUD goes down, all cConHooks
    have their m_nId set to zero.

*/
namespace mplex
{

cConHook *g_connection_list = nullptr;

void dumbPlayLoop(cConHook *con, const char *cmd)
{
    con->PlayLoop(cmd);
}

void cConHook::PlayLoop(const char *cmd)
{
    if (m_nId == 0)
    {
        if (m_nState == 0)
        {
            auto buf = diku::format_to_str("Your ID to %s was reset due to connection lost.<br/>", g_mudname);
            WriteCon(buf);
            return;
        }

        if (m_nState++ < 50)
        {
            auto buf = diku::format_to_str("Waiting to receive an ID from %s.<br/>", g_mudname);
            WriteCon(buf);
            return;
        }

        WriteCon("Giving up... You should try to disconnect and try again.<br/>");
        return;
    }
    else // m_nId != 0
    {
        m_nState = 0;
        TransmitCommand(cmd);
    }
}

void dumbMenuSelect(cConHook *con, const char *cmd)
{
    con->MenuSelect(cmd);
}

// This is where players are when initially connecting only.
void cConHook::MenuSelect(const char *cmd)
{
    if (!g_MudHook.IsHooked())
    {
        auto buf = diku::format_to_str("%s is unreachable right now...<br/>", g_mudname);
        SendCon(buf);
        test_mud_up(); // Wonder if I have multi threading issues here :o)
        return;
    }

    if (m_nId == 0)
    {
        auto buf = diku::format_to_str("Requesting an ID from %s.<br/>", g_mudname);
        SendCon(buf);
        m_nState = 1; // This means we're waiting for a response
        m_pFptr = dumbPlayLoop;
        protocol_send_request(&g_MudHook);
    }
    else
    {
        auto buf = diku::format_to_str("You got ID from %s...<br/>", g_mudname);
        SendCon(buf);
        m_nState = 0;
        m_pFptr = dumbPlayLoop;
    }
}

void dumbMudDown(cConHook *con, const char *cmd)
{
    con->MudDown(cmd);
}

void cConHook::MudDown(const char *cmd)
{
    auto buf = diku::format_to_str("There is still no connection to %s.<br/>", g_mudname);
    SendCon(buf);

    test_mud_up(); // Wonder if I have multi threading issues here :o)
}

void Idle(cConHook *con, const char *cmd)
{
    /* This should not happen, but I am tryng to find a bug... */

    slog(LOG_ALL, 0, "AHA! Ilde was called! Tell Development team about this!");
}

void dumbPressReturn(cConHook *con, const char *cmd)
{
    con->PressReturn(cmd);
}

void cConHook::PressReturn(const char *cmd)
{
    int oldmode = m_nPromptMode;

    if (*skip_blanks(cmd))
    {
        SendCon("<br/><div class='return'>*** Read aborted ***</div><br/>");

        m_qPaged.Flush();
        m_pFptr = dumbPlayLoop;
        m_nPromptMode = 0;
        PlayLoop(cmd);
        return;
    }

    ShowChunk();

    if (m_nPromptMode == 1)
    {
    }
    else if (m_nPromptMode == 0)
    {
        m_qPaged.Flush();
        m_pFptr = dumbPlayLoop;
        m_nPromptMode = 0;

        if (oldmode != m_nPromptMode)
        {
            char buf[1000];
            strcpy(buf, ParseOutput("<br/><div class='return'>*** Read Done ***</div><br/>"));
            Write((ubit8 *)buf, strlen(buf));
            PlayLoop("");
        }
    }
}

// ===============================

// Destroy all clients that are no longer connected
void ClearUnhooked()
{
    cConHook *con = nullptr;
    cConHook *nextcon = nullptr;

    for (con = g_connection_list; con; con = nextcon)
    {
        nextcon = con->m_pNext;

        if (!con->IsHooked())
        {
            con->Close(TRUE);
            delete con;
        }
    }
}

/* ================== VIRTUAL OVERRIDE ===================== */

int cConHook::IsHooked()
{
    if (this->m_pWebsServer)
    {
        return TRUE;
    }
    else
    {
        return cHook::IsHooked();
    }
}

void cConHook::Unhook()
{
    if (IsHooked())
    {
        slog(LOG_OFF, 0, "Unhooking player connection");
        g_CaptainHook.Unhook(this);
    }

    if (this->m_pWebsServer == nullptr)
    {
        cHook::Unhook();
    }
}

void cConHook::Write(ubit8 *pData, ubit32 nLen, int bCopy)
{
    if (this->m_pWebsServer)
    {
        assert(pData[nLen] == 0);
        if (!ws_send_message(m_pWebsServer, m_pWebsHdl, (const char *)pData))
        {
            this->Close(TRUE);
        }
    }
    else
    {
        cHook::Write(pData, nLen, bCopy);
    }
}

void cConHook::Close(int bNotifyMud)
{
    if (m_nId == 0)
    {
        return;
    }

    m_mtx.lock();

    if (IsHooked())
    {
        Unhook();
    }

    if (bNotifyMud && m_nId != 0 && g_MudHook.IsHooked())
    {
        slog(LOG_ALL, 0, "Closing con id %d.", m_nId);
        protocol_send_close(&g_MudHook, m_nId);
    }

    m_aOutput[0] = 0;
    m_aInputBuf[0] = 0;
    m_nState = 0;
    m_pFptr = Idle;
    m_nId = 0;

    m_pWebsServer = nullptr;

    remove_gmap(this);

    // Still have the issue that we ought to close the
    // websocket connection here and remove from g_cMapHandler - but what if we were call by
    // websockets.

    m_mtx.unlock();
}

/* ================================================================== */

// Send command to the MUD server connection
void cConHook::TransmitCommand(const char *text)
{
    char sendText[MAX_INPUT_LENGTH];
    char *d = nullptr;

    strncpy(sendText, text, MAX_INPUT_LENGTH);
    sendText[MAX_INPUT_LENGTH - 1] = 0; // MS2020 moved up to avoid strchr bug.

    if ((d = strchr(sendText, '\r')))
    {
        *d = 0;
    }

    if ((d = strchr(sendText, '\n')))
    {
        *d = 0;
    }

    strip_trailing_spaces(sendText);
    // keep input short.
    // maybe warn user if to long but we are not now.

    m_nPromptLen = 0;

    protocol_send_text(&g_MudHook, m_nId, sendText, MULTI_TEXT_CHAR);
}

/* ======================= TEXT PARSE & ECHO INPUT ====================== */

/* Maybe this would be overall easier? */
char cConHook::AddInputChar(ubit8 c)
{
    char *cp = m_aInputBuf;

    if (c == '\x1B')
    {
        m_nEscapeCode = 1;
        return 0;
    }

    if (m_nEscapeCode > 0)
    {
        if (m_nEscapeCode == 1)
        {
            if (c == '[')
            {
                m_nEscapeCode++;
                return 0;
            }
        }
        else if (m_nEscapeCode == 2)
        {
            if (c == 'D')
            {
                m_nEscapeCode = 0;
                c = '\b';
            }
        }
        else
        {
            m_nEscapeCode = 0;
        }
    }

    TAIL(cp);
    if ((c == '\b') || (c == '\177'))
    {
        if (cp != m_aInputBuf)
        {
            *(cp - 1) = 0;
            return '\b';
        }

        return 0;
    }

    m_nEscapeCode = 0;

    *cp++ = c;
    *cp = 0;

    if ((c < ' ') || (c == 255) || (m_sSetup.telnet && (c > 127)))
    {
        *(cp - 1) = 0;
    }

    return *(cp - 1);
}

void cConHook::AddString(char *str)
{
    char *s = nullptr;
    char echobuf[4096]; // ~= 12 * sizeof(str) = 12 * 256 = 3000

    char *eb = nullptr;
    char c = 0;

    eb = echobuf;

    for (s = str; *s; s++)
    {
        if (ISNEWL(*s) || (strlen(m_aInputBuf) >= MAX_INPUT_LENGTH - 4))
        {
            while (ISNEWL(*(s + 1)))
            {
                s++;
            }
            *eb++ = '\n';
            *eb++ = '\r';
            m_qInput.Append(new cQueueElem(m_aInputBuf));
            m_aInputBuf[0] = 0;
        }
        else
        {
            int x_pos = (m_nPromptLen + strlen(m_aInputBuf)) % m_sSetup.width;
            c = AddInputChar(*s);
            if (c)
            {
                if (c == '\b')
                {
                    if (m_sSetup.emulation == TERM_ANSI)
                    {
                        if (x_pos == 0)
                        {
                            sprintf(eb, "[A[%dC[K", m_sSetup.width - 1);
                            TAIL(eb);
                        }
                        else
                        {
                            strcpy(eb, "[D[K");
                            TAIL(eb);
                        }
                    }
                    else
                    {
                        *eb++ = '\b';
                        *eb++ = ' ';
                        *eb++ = '\b';
                    }
                }
                else
                {
                    *eb++ = c;
                }
            }
        }
    }

    *eb = 0;
    assert(eb - echobuf < (int)sizeof(echobuf) - 1);

    if (m_sSetup.echo)
    {
        Write((ubit8 *)echobuf, eb - echobuf);
    }
}

/* On -1 'con' was destroyed */
void cConHook::ParseInput()
{
    if (m_qInput.Size() > 50)
    {
        SendCon("STOP FLOODING ME!!!<br/>");

        if (m_qInput.Size() > 100)
        {
            Close(TRUE);
            return;
        }
    }
}

/* ======================= STUFF ====================== */

void cConHook::ProcessPaged()
{
    if (m_pFptr != dumbPressReturn)
    {
        m_pFptr = dumbPressReturn;
        PressReturn("");
    }
}

void cConHook::Input(int nFlags)
{
    if (nFlags & SELECT_EXCEPT)
    {
        Close(TRUE);
    }
    else if (nFlags & SELECT_READ)
    {
        char *c = nullptr;
        ubit8 buf[1024];

#if defined(_WINDOWS)
        b = (char *)buf;
        int n = recv(this->tfd(), b, sizeof(buf), 0);
        strcpy((char *)buf, b);
#endif
#if defined(LINUX)
        int n = ::read(this->tfd(), buf, sizeof(buf) - 1);
#endif

        if (n == -1)
        {
#if defined(_WINDOWS)
            if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR)
            {
                return;
            }
#else
            if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
            {
                return;
            }
#endif

            Close(TRUE);
            return;
        }
        else if (n == 0)
        {
            Close(TRUE);
            return;
        }

        if (m_nSequenceCompare != -1)
        {
            SequenceCompare(buf, &n);
            if (n <= 0)
            {
                return;
            }
        }

        if (m_nFirst >= 0)
        {
            getLine(buf, &n);
            if (n <= 0)
            {
                return;
            }
        }

        buf[n] = 0;
        AddString((char *)buf);

        ParseInput();

        cQueueElem *qe = nullptr;

        while ((qe = m_qInput.GetHead()))
        {
            c = (char *)qe->Data();
            assert(strlen(c) < MAX_INPUT_LENGTH);
            m_pFptr(this, c);
            delete qe;
        }
    }
}

/* ================= sending data to client output handling ====================== */

/* -1 on connection closed, 0 on success */
void cConHook::WriteCon(const char *text)
{
    if (text == nullptr)
    {
        return;
    }

    Write((ubit8 *)text, strlen(text));
}

void cConHook::WriteCon(const std::string &text)
{
    WriteCon(text.c_str());
}

void cConHook::SendCon(const char *text)
{
    WriteCon(ParseOutput(text));
}

void cConHook::SendCon(const std::string &text)
{
    SendCon(text.c_str());
}

/* ======================= TEXT FORMATTING OUTPUT ====================== */

std::string mplex_getcolor(cConHook *hook, const char *colorstr)
{
    auto gcolor = hook->color.get(colorstr);

    if (gcolor.empty())
    {
        gcolor = g_cDefcolor.get(colorstr);
    }

    return gcolor;
}

char *cConHook::IndentText(const char *source, char *dest, int dest_size, int width)
{
    const char *last = source;
    const char *current = nullptr;
    char tmpbuf[MAX_STRING_LENGTH * 2];
    int i = 0;
    char *newptr = nullptr;
    int column = 0;
    int cutpoint = MIN(30, width / 2);

    if (!(current = source))
    {
        return nullptr;
    }

    newptr = dest;

    while (*current)
    {
        if ((newptr + 100) > dest + dest_size)
        {
            *newptr = 0;
            break;
        }

        if (*current == CONTROL_CHAR)
        {
            *newptr++ = *current++; // Copy control char

            for (int j = 0; j < 20; j++) // Skip the control sequence which ends with 'm'
            {
                *newptr++ = *current; // copy char

                if (*current == 0)
                {
                    break;
                }

                if (*current == 'm')
                {
                    current++;
                    break;
                }
                current++;
            }
            continue;
        }

        if (m_bGobble)
        {
            current++;
            continue;
        }

        if (m_bColorDisp == true)
        {
            *tmpbuf = '\0';
            i = 0;

            while (m_bColorDisp == true && *current)
            {
                if (*current == CONTROL_CHAR)
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (*current == CONTROL_COLOR_END_CHAR)
                    {
                        auto mplex_color = mplex_getcolor(this, tmpbuf);
                        if (mplex_color.empty() == false)
                        {
                            auto cretbuf = mplex_color.begin();
                            while (cretbuf != mplex_color.end())
                            {
                                if (*cretbuf == CONTROL_CHAR)
                                {
                                    cretbuf++;
                                    protocol_translate(this, *cretbuf, &newptr);
                                    cretbuf++;

                                    continue;
                                }
                                else
                                {
                                    cretbuf++;
                                }
                            }
                        }
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (m_bColorCreate == true)
        {
            *tmpbuf = '\0';
            i = 0;
            while (m_bColorCreate == true && *current)
            {
                if ((*current == CONTROL_CHAR) && (*(current + 1) == CONTROL_COLOR_END_CHAR))
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (!*current || *current == CONTROL_COLOR_END_CHAR)
                    {
                        color.create(tmpbuf);
                        m_bColorCreate = false;
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (m_bColorChange == true)
        {
            *tmpbuf = '\0';
            i = 0;
            while (m_bColorChange == true && *current)
            {
                if ((*current == CONTROL_CHAR) && (*(current + 1) == CONTROL_COLOR_END_CHAR))
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (!*current || *current == CONTROL_COLOR_END_CHAR)
                    {
                        color.change(tmpbuf);
                        m_bColorChange = false;
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (m_bColorInsert == true)
        {
            *tmpbuf = '\0';
            i = 0;
            while (m_bColorInsert == true && *current)
            {
                if ((*current == CONTROL_CHAR) && (*(current + 1) == CONTROL_COLOR_END_CHAR))
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (!*current || *current == CONTROL_COLOR_END_CHAR)
                    {
                        color.insert(tmpbuf);
                        m_bColorInsert = false;
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (m_bColorRemove == true)
        {
            *tmpbuf = '\0';
            i = 0;
            while (m_bColorRemove == true && *current)
            {
                if ((*current == CONTROL_CHAR) && (*(current + 1) == CONTROL_COLOR_END_CHAR))
                {
                    current++;
                    protocol_translate(this, *current, &newptr);
                    if (!*current || *current == CONTROL_COLOR_END_CHAR)
                    {
                        color.remove(tmpbuf);
                        m_bColorRemove = false;
                    }
                    current++;
                }
                else
                {
                    tmpbuf[i] = tolower(*current);
                    tmpbuf[i + 1] = 0;
                    i++;
                    current++;
                }
            }
            continue;
        }

        if (isaspace(*current))
        { /* Remember last space */
            last = current;
        }

        if (*current == '\n' || *current == '\r')
        { /* Newlines signify new col. */
            column = 0;
        }

        if ((++column <= width))
        { /* MS: Added '<=' Have some space.. */
            *(newptr++) = *(current++);
        }
        else
        /* Out of space, so... */
        {
            column = 0;
            if (last == nullptr || cutpoint < current - last)
            { /* backtrack or cut */
                last = current;
            }
            newptr -= (current - last);
            current = ++last;
            *(newptr++) = '\n';
            *(newptr++) = '\r';
            current = skip_spaces(current); /* Skip any double spaces, etc. */
            last = nullptr;
        }
    }

    *newptr = '\0';

    assert(dest + dest_size > newptr);

    return dest;
}

/*  Misleading name. Function changes HTML to TELNET
 *  <br/> becomes \n\r except if it is <br/>\n\r then it is ignored. Thus
 *  <br/><br/>\n\r becomes \n\r\n\r.
 *  That's by design to support both telnet and HTML reasonably.
 */
void cConHook::StripHTML(char *dest, const char *src)
{
    if (g_mplex_arg.g_bModeRawHTML)
    {
        strcpy(dest, src);
        return;
    }

    const char *p = nullptr;
    const char *t = nullptr;

    p = src;

    while (*p)
    {
        if (*p == '<')
        {
            char aTag[256];

            p = getHTMLTag(p, aTag, sizeof(aTag));

            if (aTag[0] == 0)
            {
                continue;
            }

            // We got a HTML tag
            if (strcmp(aTag, "br") == 0 || strcmp(aTag, "br/") == 0)
            {
                if (*p == '\n' || *(p + 1) == '\n')
                { // If the next is \n then dont add <br>
                    continue;
                }

                // the <br/> tag was not followed by \n\r so add \n\r
                *dest++ = '\n';
                *dest++ = '\r';
                continue;
            }
            if (strcmp(aTag, "/tr") == 0)
            {
                *dest++ = '\n';
                *dest++ = '\r';
                continue;
            }
            if (strcmp(aTag, "go-ahead/") == 0)
            {
                *dest++ = IAC;
                *dest++ = GA;
                continue;
            }
            if (strcmp(aTag, "script") == 0)
            {
                if (strncasecmp(p, "PasswordOn()", 12) == 0)
                {
                    if (g_mplex_arg.bMudProtocol)
                    {
                        // Kyle, might be something here for MUD protocol
                    }

                    Control_Echo_Off(this, &dest, 0);
                    p += 12;
                }
                else if (strncasecmp(p, "PasswordOff(", 12) == 0)
                {
                    if (g_mplex_arg.bMudProtocol)
                    {
                        // Kyle, might be something here for MUD protocol
                    }
                    Control_Echo_On(this, &dest, 0);
                    p += 12;
                }
                t = strstr(p, "</script>");
                if (t)
                {
                    p = t + 9;
                }
                continue;
            }
            else if (strncmp(aTag, "/div", 4) == 0)
            {
                // Hm, this'll send a lot of code... but telnet's a hack now :)
                // How on earth will I get the "default color" if someone prefers e.g. yellow... ?
                Control_ANSI_Fg(this, &dest, 'w', FALSE);
                Control_ANSI_Bg(this, &dest, 'n');
            }
            else if ((strncmp(aTag, "div ", 4) == 0) || (strncmp(aTag, "h1 ", 3) == 0))
            {
                char buf[256];
                int l = 0;

                if (g_mplex_arg.bMudProtocol)
                {
                    // Kyle: if you remove these comments you'll see the full tag contents
                    //       which is nice for debugging. Or use gdb ;)
                    // strcpy(dest, "||"); TAIL(dest);
                    // strcpy(dest, aTag); TAIL(dest);
                    // strcpy(dest, "||"); TAIL(dest);

                    // If the tag has 'bars' it's a health update
                    l = getHTMLValue("bars", aTag, buf, sizeof(buf) - 1);

                    if (l != 0)
                    {
                        strcpy(dest, "||");
                        TAIL(dest);
                        strcpy(dest, buf);
                        TAIL(dest);
                        strcpy(dest, "||");
                        TAIL(dest);

                        // KYLE: buf will have the hp,mp,ep
                        // remove the three debug lines above, parse the string, and
                        // output mud protocol codes
                        //
                        continue;
                    }

                    l = getHTMLValue("exits", aTag, buf, sizeof(buf) - 1);

                    if (l != 0)
                    {
                        strcpy(dest, "||");
                        TAIL(dest);
                        strcpy(dest, buf);
                        TAIL(dest);
                        strcpy(dest, "||");
                        TAIL(dest);

                        // KYLE: buf will have the visible exits
                        // remove the three debug lines above, parse the string, and
                        // output mud protocol codes
                        //
                        continue;
                    }
                }

                l = getHTMLValue("class", aTag, buf, sizeof(buf) - 1);

                if (l == 0)
                {
                    continue;
                }

                // We got a single or double color code on our hands
                // e.g. cpg, cg bn, cpy bb, etc.
                if ((l >= 2) && (l <= 6) && ((buf[0] == 'b') || (buf[0] == 'c')))
                {
                    char *p = nullptr;
                    char tmp[256];

                    p = str_next_word(buf, tmp);

                    while (tmp[0])
                    {
                        if (tmp[0] == 'c')
                        {
                            if (tmp[1] == 'p')
                            {
                                Control_ANSI_Fg(this, &dest, tmp[2], TRUE);
                            }
                            else
                            {
                                Control_ANSI_Fg(this, &dest, tmp[1], FALSE);
                            }
                        }
                        else if (tmp[0] == 'b')
                        {
                            Control_ANSI_Bg(this, &dest, tmp[1]);
                        }
                        else
                        {
                            slog(LOG_ALL, 0, "Illegal color code %s", buf);
                            break;
                        }

                        p = str_next_word(p, tmp);
                    }
                }
            }
            continue;
        }
        else if (*p == '&')
        {
            if (strncasecmp(p, "&gt;", 4) == 0)
            {
                *dest++ = '>';
                p += 4;
            }
            else if (strncasecmp(p, "&lt;", 4) == 0)
            {
                *dest++ = '<';
                p += 4;
            }
            else if (strncasecmp(p, "&amp;", 5) == 0)
            {
                *dest++ = '&';
                p += 5;
            }
            else if (strncasecmp(p, "&apos;", 5) == 0)
            {
                *dest++ = '\'';
                p += 5;
            }
            else if (strncasecmp(p, "&nbsp;", 6) == 0)
            {
                *dest++ = ' ';
                p += 6;
            }
            else if (strncasecmp(p, "&quot;", 6) == 0)
            {
                *dest++ = '\"';
                p += 6;
            }
            else
            {
                *dest++ = *p++;
            }

            continue;
        }

        if (*p == '\n' && *(p + 1) != '\r')
        {
            *dest++ = '\n';
            *dest++ = '\r';
            p++;
        }
        else
        {
            *dest++ = *p++;
        }
    }
    *dest = 0;
}

// Parse the string 'text' and prepare it for output on 'con'
// This should only be called for telnet
char *cConHook::ParseOutput(const char *text)
{
    static char Outbuf[65536];  // I know this sucks. Got to rewrite thath StripHTML and 
    static char Outbuf2[65536]; // rewrite indentText too to either stop at the end or realloc...

    assert(strlen(text) < sizeof(Outbuf));

    if (this->m_pWebsServer)
    {
        size_t n = strlen(text);
        if (n < sizeof(Outbuf))
        {
            strcpy(Outbuf, text);
        }
        else
        {
            memcpy(Outbuf, text, sizeof(Outbuf));
            Outbuf[sizeof(Outbuf) - 100] = 0;
        }

        return Outbuf;
    }
    else
    {
        StripHTML(Outbuf2, text);

        return IndentText(Outbuf2, Outbuf, sizeof(Outbuf), m_sSetup.width);
    }
}

/* Attempts to erase the characters input after the prompt
void cConHook::PromptErase (void)
{
    int n;
    char buf[MAX_STRING_LENGTH];
    char *b = buf;

    if (m_nPromptLen == 0)
        return;

    *b = 0;

    if ((m_sSetup.emulation == TERM_TTY) ||
            (m_sSetup.emulation == TERM_VT100))
    {
        for (n = 0; m_aInputBuf[n]; n++)
            *b++ = '\b';
        for (n = 0; m_aInputBuf[n]; n++)
            *b++ = ' ';
        *b = 0;
    }
    else if (m_sSetup.emulation == TERM_ANSI)
    {
        int len = strlen (m_aInputBuf);
        int lines = 0;
        int i;

        if (len > 0)
        {
            lines = (m_nPromptLen + len - 1) / m_sSetup.width;

            for (i = 0; i < lines; i++)
            {
                sprintf (b, "[%dD[K[A",
                         m_sSetup.width);
                TAIL (b);
            }

            if (m_nPromptLen > 1)
                sprintf (b, "[%dD[%dC[K",
                         m_sSetup.width,
                         m_nPromptLen - 1);
            else
                sprintf (b, "[%dD[K",
                         m_sSetup.width);

            TAIL (b);
        }
    }

    m_nPromptLen = 0;

    if (*buf)
        Write ((ubit8 *) buf, strlen (buf));
}
*/

/* Assumes that a newline has already been output  */
void cConHook::PromptRedraw(const char *prompt)
{
    if (*prompt)
    {
        Write((ubit8 *)prompt, strlen(prompt));
    }

    if (*m_aInputBuf)
    {
        Write((ubit8 *)m_aInputBuf, strlen(m_aInputBuf));
    }

    m_nPromptLen = strlen(prompt);
}

/* ======================= 'Con' handling ====================== */

void cConHook::SequenceCompare(ubit8 *pBuf, int *pnLen)
{
    // static const char *match = "a";
    static const char *match = "[W32-V/C@SiGn]";
    /*  static int state = 0;*/

    assert(m_nSequenceCompare != -1);

    for (int i = 0; i < *pnLen; i++)
    {
        if (pBuf[i] == match[m_nSequenceCompare])
        {
            if (match[m_nSequenceCompare + 1] == 0)
            {
                m_nSequenceCompare = -1;

                m_sSetup.echo = FALSE;
                m_sSetup.redraw = FALSE;
                m_sSetup.emulation = TERM_INTERNAL;
                m_sSetup.telnet = FALSE;

                memmove(pBuf, pBuf + strlen(match), *pnLen - strlen(match));
                *pnLen -= strlen(match);

                return;
            }
        }
        else
        {
            m_nSequenceCompare = -1;
            return;
        }

        m_nSequenceCompare++;
    }
}

void cConHook::testChar(ubit8 c)
{
    switch (m_nFirst)
    {
        case 0:
            if (c == IAC)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
            }
            break;

        case 1:
            if (c == WILL)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
            }
            break;

        case 2:
            if (c == TELOPT_ECHO)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
            }
            break;

        case 3:
            m_nLine = c;
            m_nFirst++;
            break;

        case 4:
            if (c == IAC)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
                m_nLine = 255;
            }
            break;

        case 5:
            if (c == WONT)
            {
                m_nFirst++;
            }
            else
            {
                m_nFirst = -1;
                m_nLine = 255;
            }
            break;

        case 6:
            if (c == TELOPT_ECHO)
            {
                m_nFirst = -2;
            }
            else
            {
                m_nFirst = -1;
                m_nLine = 255;
            }
            break;

        default:
            m_nFirst = -1;
            m_nLine = 255;
            break;
    }
}

void cConHook::getLine(ubit8 buf[], int *size)
{
    int i = 0;

    for (i = 0; i < *size; i++)
    {
        testChar(buf[i]);

        // slog(LOG_ALL, 0, "Testchar %d, first %d.", buf[i], m_nFirst);

        if (m_nFirst == -1)
        {
            return;
        }
        if (m_nFirst == -2)
        {
            break;
        }
    }

    if (i < *size)
    {
        memmove(buf, buf + i + 1, *size - (i + 1));
    }
    *size -= i;
}

/* Returns new point (NULL if done) */
void cConHook::ShowChunk()
{
    char buffer[160 * 60 + 600];
    char *scan = nullptr;
    char *point = nullptr;
    const int max_lines = m_sSetup.height;
    int lines = 0;

    scan = buffer;

    if (m_qPaged.IsEmpty())
    {
        return;
    }

    cQueueElem *qe = m_qPaged.GetHead();

    point = (char *)qe->Data();

    for (;;)
    {
        if (!*point)
        {
            if (m_qPaged.IsEmpty())
            {
                break;
            }

            delete qe;
            qe = m_qPaged.GetHead();
            point = (char *)qe->Data();
            continue;
        }

        *scan = *point++;

        if (*scan == '\n')
        {
            lines++;
        }

        if (max_lines <= lines)
        {
            break;
        }

        scan++;

        if (scan >= buffer + sizeof(buffer) - 200)
        {
            sprintf(scan, "TRUNCATED!");
            break;
        }
    }

    *scan = '\0';

    /* Insert the rest of the un-paged stringback into buffer */
    if (!str_is_empty(point))
    {
        m_qPaged.Prepend(new cQueueElem(point));
    }

    if (qe)
    {
        delete qe;
    }

    m_nPromptMode = !m_qPaged.IsEmpty();

    if (m_nPromptMode == 1)
    {
        strcat(buffer, ParseOutput("<br/><div class='paged'> *** Return for more *** </div><br/>"));
    }

    assert(strlen(buffer) < sizeof(buffer));

    Write((ubit8 *)buffer, strlen(buffer));
}

typedef websocketpp::server<websocketpp::config::asio> wsserver;

// Nice if it could be in the constructor
void cConHook::SetWebsocket(wsserver *server, websocketpp::connection_hdl hdl)
{
    m_pWebsServer = server;
    m_pWebsHdl = hdl;
}

// cConHook Constructor
cConHook::cConHook()
{
    m_bGobble = false;
    m_bColorCreate = false;
    m_bColorChange = false;
    m_bColorDisp = false;
    m_bColorRemove = false;
    m_bColorDelete = false;
    m_bColorInsert = false;
    m_bGobble = false;
    m_nEscapeCode = 0;
    m_aOutput[0] = 0;
    m_aInputBuf[0] = 0;
    m_nState = 0;

    m_nSequenceCompare = 0;
    m_nId = 0;
    m_nFirst = 0;
    m_nLine = 255;

    m_nPromptMode = 0;
    m_nPromptLen = 0;

    // m_pWebsHdl = 0;
    m_pWebsServer = nullptr;

    m_sSetup.echo = g_mplex_arg.g_bModeEcho;
    m_sSetup.redraw = g_mplex_arg.g_bModeRedraw;
    m_sSetup.telnet = g_mplex_arg.g_bModeTelnet;
    m_sSetup.websockets = g_mplex_arg.bWebSockets;

    if (g_mplex_arg.g_bModeANSI)
    {
        m_sSetup.emulation = TERM_ANSI;
    }
    else
    {
        m_sSetup.emulation = TERM_TTY;
    }

    m_sSetup.height = 15;
    m_sSetup.width = 80;
    m_sSetup.colour_convert = 0;
    m_nBgColor = CONTROL_BG_BLACK_CHAR;

    m_pNext = g_connection_list;
    g_connection_list = this;

    m_nState = 0;
    m_pFptr = dumbMenuSelect;
    m_nPromptMode = 0;
    strcpy(m_aHost, "");

    if (g_mplex_arg.bWebSockets)
    {
        return; // Hack for making websockets work
    }

    int fd = 0;
#ifdef _WINDOWS
    int j;
    int size;
#else
    unsigned int j = 0;
    unsigned int size = 0;
#endif
    sockaddr_in conaddr;
    sockaddr_in sock;
    char hostname[49];
    int x = 0;

    assert(g_MotherHook.IsHooked());

    j = sizeof(conaddr);
    // slog(LOG_OFF, 0, "accept() before.");
    fd = accept(g_MotherHook.tfd(), (sockaddr *)&conaddr, &j);
    // slog(LOG_OFF, 0, "accept() after.");
    Assert(fd >= 0, "ACCEPT");

#ifdef LINUX
    x = fcntl(fd, F_SETFL, FNDELAY);
    Assert(x != -1, "Non blocking set error.");
#endif

#ifdef _WINDOWS
    unsigned long val = 1;
    x = ioctlsocket(fd, FIONBIO, &val);
#endif
    /* We *want* Nagle to work on the individual socket connections
       to the outside world, but not between the servers & mplex'ers
       int i;
       j = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
       if (j == -1)
       error(HERE, "No Setsockopt()"); */

    size = sizeof(sock);
    if (getpeername(fd, (sockaddr *)&sock, &size) == -1)
    {
        slog(LOG_OFF, 0, "getpeername: socket %d error no %d.", fd, errno);
        // MS2020 strncpy (hostname,"NO GETPEERNAME",strlen("NO GETPEERNAME")+1);
        strcpy(hostname, "NO GETPEERNAME");
    }
    else
    {
        /*
           This occasionally causes freezing (for all players) of a duration
           from ~1 - 60 seconds. Too bad... if I get threads I will put it back
           in, I hate having only the numeric address. Quinn suggested an
           alarm of a few seconds, I'll try that out.
         */

        /*alarm(1); */

        /*
           if ((from = gethostbyaddr((char*) &sock.sin_addr,
           sizeof(sock.sin_addr),
           AF_INET)) == NULL)
           {
           slog(LOG_ALL, 0, "No symbolic INET address, h_errno %d.", h_errno);
           addr = (unsigned char *) &sock.sin_addr.s_addr;
        strncpy (hostname,inet_ntoa(sock.sin_addr),49);
           }
           else
           {
           m_aHostname = (char *) from->h_name;
           }
         */
        /* alarm(0); unschedule alarm */

        /* The fix */
        //      addr = (unsigned char *) &sock.sin_addr.s_addr;
        strncpy(hostname, inet_ntoa(sock.sin_addr), sizeof(hostname) - 1);
    }

    strncpy(m_aHost, hostname, sizeof(m_aHost) - 1);
    *(m_aHost + sizeof(m_aHost) - 1) = '\0';

    if (this->tfd() != -1)
    {
        slog(LOG_ALL, 0, "cConHook() called with a non -1 fd.");
    }

    g_CaptainHook.Hook(fd, this);

    g_nConnectionsLeft--;
}

// cConHook Destructor
cConHook::~cConHook()
{
    if (this == g_connection_list)
    {
        g_connection_list = this->m_pNext;
    }
    else
    {
        cConHook *tmp = nullptr;

        for (tmp = g_connection_list; tmp; tmp = tmp->m_pNext)
        {
            if (tmp->m_pNext == this)
            {
                break;
            }
        }
        tmp->m_pNext = this->m_pNext;
    }

    g_nConnectionsLeft++;
}

} // namespace mplex
