#pragma once

#include "hook.h"

#define MAX_MULTI 10 /* Maximum five multiconnects */

class cMultiHook : public cHook
{
public:
    cMultiHook();
    void Input(int nFlags);
    void Unhook();

    void Close();
    int Read();
    void Ping();

    int succ_err; /* Number of successive errors */
    ubit8 bWebsockets;
};

class cMultiMaster
{
public:
    cMultiMaster();

    int nCount;
    cMultiHook Multi[MAX_MULTI];
};

extern cMultiMaster g_Multi;

class cMotherHook : public cHook
{
public:
    void Input(int nFlags);
    void Close();
    void Unhook();
};

void multi_close_all();
void init_mother(int nPort);
void multi_clear();
void multi_ping_all();

extern cMotherHook g_MotherHook;
