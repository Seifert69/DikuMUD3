#pragma once

#include "hook.h"

#define MAX_MULTI 10 /* Maximum five multiconnects */

class cMultiHook : public cHook
{
public:
    cMultiHook(void);
    void Input(int nFlags);
    void Unhook(void);

    void Close(void);
    int Read(void);
    void Ping(void);

    int succ_err; /* Number of successive errors */
    ubit8 bWebsockets;
};

class cMultiMaster
{
public:
    cMultiMaster(void);

    int nCount;
    cMultiHook Multi[MAX_MULTI];
};

extern class cMultiMaster g_Multi;

class cMotherHook : public cHook
{
public:
    void Input(int nFlags);
    void Close(void);
    void Unhook(void);
};

void multi_close_all(void);
void init_mother(int nPort);
void multi_clear(void);
void multi_ping_all(void);

extern class cMotherHook g_MotherHook;
