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

namespace mplex
{

class cMotherHook : public cHook
{
public:
    void Unhook();
    void Input(int nFlags);
    int IsHooked();
};

class cMudHook : public cHook
{
public:
    void Unhook();
    int read_mud();
    void Input(int nFlags);
};

void Control();
void test_mud_up();
void mud_went_down();

// I think this is the open socket from the main listening telnet port
extern cMotherHook g_MotherHook;
// Connection to and from the MUD server
extern cMudHook g_MudHook;
extern int g_nConnectionsLeft;
extern color_type g_cDefcolor;

} // namespace mplex
