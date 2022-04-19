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
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace mplex
{

// typedef websocketpp::server<websocketpp::config::asio> wsserver;
typedef websocketpp::server<websocketpp::config::asio> wsserver;

class cConHook : public cHook
{
public:
    cConHook();
    ~cConHook();

    void Unhook();
    int IsHooked(); ///< At this level we also need to check for websockets
    void Write(ubit8 *pData, ubit32 nLen, int bCopy = TRUE);

    void Close(int bNotifyMud);
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
    void SetWebsocket(wsserver *server, websocketpp::connection_hdl hdl);
    void StripHTML(char *dest, const char *src);

    void Input(int nFlags);
    void getLine(ubit8 buf[], int *size);
    void testChar(ubit8 c);
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

    wsserver *m_pWebsServer;
    websocketpp::connection_hdl m_pWebsHdl;

    cConHook *m_pNext;

    terminal_setup_type m_sSetup;

    ubit8 m_nBgColor; ///< Stupid bitching ANSI

    cQueue m_qInput; ///< Input from user
    cQueue m_qPaged; ///< Paged text output
private:
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
