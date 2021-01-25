/*
 $Author: tperry $
 $RCSfile: mplex.h,v $
 $Date: 2001/04/10 21:11:37 $
 $Revision: 2.0 $
 */

#ifndef _MUD_MUDCONNECTOR_H
#define _MUD_MUDCONNECTOR_H

#include "network.h"
#include <string.h>
#include "protocol.h"
#include "essential.h"
#include "queue.h"
#include "hook.h"
#include "color.h"

class cMotherHook : public cHook
{
public:
   void Unhook(void);
   void Input(int nFlags);
   int IsHooked(void);
};

class cMudHook : public cHook
{
public:
   void Unhook(void);
   int read_mud(void);
   void Input(int nFlags);
};

// I think this is the open socket from the main listening telnet port
extern cMotherHook MotherHook;

// Connection to and from the MUD server
extern cMudHook MudHook;
void Control(void);
extern int g_nConnectionsLeft;
extern class color_type g_cDefcolor;

void test_mud_up(void);

#endif
