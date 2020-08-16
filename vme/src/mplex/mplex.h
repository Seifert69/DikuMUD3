/*
 $Author: tperry $
 $RCSfile: mplex.h,v $
 $Date: 2001/04/10 21:11:37 $
 $Revision: 2.0 $
 */

#ifndef _MUD_MPLEX_H
#define _MUD_MPLEX_H

#include "network.h"
#include <string.h>
#include "protocol.h"
#include "essential.h"
#include "queue.h"
#include "hook.h"
#include "color.h"
class cConHook : public cHook
{
public:
    cConHook(void);
    ~cConHook(void);

    void Close(int bNotifyMud);
    char AddInputChar(ubit8 c);
    void AddString(char *str);
    void ParseInput(void);
    void SendCon(const char *str);
    void WriteCon(const char *str);
    char *IndentText(const char *source, char *dest, int dest_size, int width);
    int  ColorDisp(const char *current, char *newptr);
    char *ParseOutput(const char *text);
    void PromptErase(void);
    void PromptRedraw(const char *prompt);
    void TransmitCommand(const char *text);
    void ShowChunk(void);
    void ProcessPaged(void);
    void PressReturn(const char *cmd);
    void PlayLoop(const char *cmd);
    void MudDown(const char *cmd);
    void MenuSelect(const char *cmd);
    void SequenceCompare(ubit8 *pBuf, int *pnLen);

    void Input(int nFlags);
    void getLine(ubit8 buf[], int *size);
    void testChar(ubit8 c);
    class color_type color;

    bool   m_bColorChange;
    bool   m_bColorCreate;
    bool   m_bColorInsert;
    bool   m_bColorDelete;
    bool   m_bColorDisp;
    bool   m_bColorRemove;

    bool   m_bGobble;
    ubit16 m_nId;
    int    m_nFirst;
    ubit8  m_nLine;
    int    m_nPromptMode;  /* 0 none, 1 press return */
    int    m_nPromptLen;   /* Prompt length          */
    int    m_nSequenceCompare;

    int    m_nState;
    int    m_nEscapeCode;  /* Very simplified state diagram assistance */
    char   m_aOutput[4096];
    char   m_aInputBuf[2*MAX_INPUT_LENGTH];
    char   m_aHost[50];
    void (*m_pFptr) (class cConHook *, const char *cmd);

    cConHook *m_pNext;

    struct terminal_setup_type m_sSetup;

    ubit8 m_nBgColor;       /* Stupid bitching ANSI   */

    cQueue m_qInput;         /* Input from user        */
    cQueue m_qPaged;         /* Paged text output      */
};

char * mplex_getcolor(class cConHook *hook, char *colorstr);
#endif
