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
#include <functional>
#include <mutex>
#include <websocketpp/common/connection_hdl.hpp>

namespace mplex
{

/**
 * How a hook writes to its websocket, without naming the server type.
 *
 * websocketpp fixes the transport in the server's template parameter, so a
 * plain server and a TLS one are unrelated types and no pointer can hold
 * either. Only echo_server.cpp knows which is running; a hook only ever needs
 * to send a string, so that is all it is given. Returns 1 on success and 0 on
 * failure, as ws_send_message did.
 *
 * Empty on a telnet hook, which is also how the code tells the two apart.
 */
using WebsocketSender = std::function<int(websocketpp::connection_hdl, const char *)>;

class cConHook : public cHook
{
public:
    cConHook();
    ~cConHook();

    void Unhook();
    int IsHooked(); ///< At this level we also need to check for websockets
    void Write(ubit8 *pData, ubit32 nLen, int bCopy = TRUE);

    void Close(int bNotifyMud);
    void ResetSessionState();
    char AddInputChar(ubit8 c);
    void AddString(char *str);
    void ParseInput();
    void SendCon(const char *str);
    void SendCon(const std::string &str);
    void WriteCon(const char *str);
    void WriteCon(const std::string &str);
    char *IndentText(const char *source, char *dest, int dest_size, int width);
    int ColorDisp(const char *current, char *newptr);
    char *ParseOutput(const char *text);
    void PromptErase();
    void PromptRedraw(const char *prompt);
    void TransmitCommand(const char *text);
    void ShowChunk();
    void ProcessPaged();
    void PressReturn(const char *cmd);
    void PlayLoop(const char *cmd);
    void MudDown(const char *cmd);
    void MenuSelect(const char *cmd);
    void SequenceCompare(ubit8 *pBuf, int *pnLen);
    void SetWebsocket(WebsocketSender sender, websocketpp::connection_hdl hdl);
    void StripHTML(char *dest, const char *src);

    void Input(int nFlags);
    void getLine(ubit8 buf[], int *size);
    void testChar(ubit8 c);

    void TelnetParse(ubit8 *pBuf, int *pnLen);
    void SendTelnetInitialNegotiation();

    color_type color;

    bool m_bColorChange;
    bool m_bColorCreate;
    bool m_bColorInsert;
    bool m_bColorDelete;
    bool m_bColorDisp;
    bool m_bColorRemove;

    bool m_bGobble;
    ubit16 m_nId;
    int m_nFirst;
    ubit8 m_nLine;
    int m_nPromptMode; ///< 0 none, 1 press return
    int m_nPromptLen;  ///< Prompt length
    int m_nSequenceCompare;

    int m_nState;
    int m_nEscapeCode; ///< Very simplified state diagram assistance
    char m_aOutput[4096];
    char m_aInputBuf[2 * MAX_INPUT_LENGTH];
    char m_aHost[50];
    void (*m_pFptr)(cConHook *, const char *cmd);

    WebsocketSender m_fWebsSend;
    websocketpp::connection_hdl m_pWebsHdl;

    cConHook *m_pNext;

    terminal_setup_type m_sSetup;

    ubit8 m_nBgColor; ///< Stupid bitching ANSI

    cQueue m_qInput; ///< Input from user
    cQueue m_qPaged; ///< Paged text output

    // Telnet negotiation state (see TelnetParse). Lives with the TCP
    // connection - deliberately NOT reset by ResetSessionState() - and is
    // initialized here (NSDMI) because the constructor runs
    // ResetSessionState() before its own body, which reads m_nNawsWidth.
    enum eTelnetState : ubit8
    {
        TS_DATA = 0, // regular data flow
        TS_IAC,      // seen IAC
        TS_CMD,      // seen IAC WILL/WONT/DO/DONT, expecting option
        TS_SB,       // inside subnegotiation
        TS_SB_IAC    // seen IAC inside subnegotiation
    };
    ubit8 m_nTelnetState{TS_DATA};
    ubit8 m_nTelnetCmd{0};   ///< Pending WILL/WONT/DO/DONT verb
    ubit8 m_aSubneg[1024];   ///< [0] = option, rest = payload (sized for GMCP Core.Supports.Set)
    int m_nSubnegLen{0};     ///< -1 = overflowed, discard until SE
    bool m_bNawsOk{false};   ///< Client agreed to NAWS
    bool m_bTtypeOk{false};  ///< Client agreed to TTYPE
    bool m_bEorOk{false};    ///< Client wants IAC EOR instead of IAC GA
    bool m_bGmcpOk{false};   ///< Client accepted GMCP (out-of-band JSON packages)
    ubit8 m_nTtypeCount{0};  ///< TTYPE SEND rounds issued (MTTS walk, max 3)
    char m_aClientName[64]{}; ///< First TTYPE IS response, e.g. "Mudlet"
    int m_nMTTS{-1};         ///< MTTS capability bitfield, -1 = not seen
    ubit8 m_nNawsWidth{0};   ///< Last clamped NAWS width, 0 = never received
    ubit8 m_nNawsHeight{0};  ///< Last clamped NAWS height
    char m_aLastBars[64]{};  ///< Last bars value sent as Char.Vitals (dedupe)

private:
    void TelnetNegotiate(ubit8 cmd, ubit8 opt, ubit8 *pOut, int *pnOutLen);
    void TelnetSubneg(ubit8 *pOut, int *pnOutLen);
    void GmcpSend(const char *package, const char *json);
    void GmcpVitals(const char *bars);
    void GmcpRoomInfo(const char *aTag, const char *roomid, const char *text);

    std::mutex m_mtx; ///< Mutex for websockets threading
};

std::string mplex_getcolor(cConHook *hook, const char *colorstr);

void dumbPlayLoop(cConHook *con, const char *cmd);
void dumbPressReturn(cConHook *con, const char *cmd);
void dumbMenuSelect(cConHook *con, const char *cmd);
void dumbMudDown(cConHook *con, const char *cmd);
void Idle(cConHook *con, const char *cmd);

void ClearUnhooked();

extern cConHook *g_connection_list;

} // namespace mplex
