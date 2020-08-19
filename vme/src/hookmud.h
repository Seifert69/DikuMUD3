#ifndef _MUD_HOOKMUD_H
#define _MUD_HOOKMUD_H

#include "hook.h"

#define MAX_MULTI 10 /* Maximum five multiconnects */

class cMultiHook : public cHook
{
public:
    cMultiHook(void);
    void Input(int nFlags);

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

extern class cMultiMaster Multi;



class cMotherHook : public cHook
{
public:
    void Input(int nFlags);
    void Close(void);
};

extern class cMotherHook MotherHook;


void multi_close_all(void);
void init_mother(int nPort);

#endif
