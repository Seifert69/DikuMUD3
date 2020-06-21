/*
 $Author: tperry $
 $RCSfile: network.h,v $
 $Date: 2002/10/13 00:04:25 $
 $Revision: 2.1 $
 */

#ifndef _MUD_NETWORK_H
#define _MUD_NETWORK_H

#ifdef _WINDOWS
#include <winsock.h>
#endif

#include <sys/types.h>

#ifdef LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define DEF_SERVER_ADDR  "127.0.0.1"
#endif

int OpenMother(int port);
int OpenNetwork(int nMudPort, char *pMudAddr);

#endif

