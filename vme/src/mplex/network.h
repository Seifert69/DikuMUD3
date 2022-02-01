/*
 $Author: tperry $
 $RCSfile: network.h,v $
 $Date: 2002/10/13 00:04:25 $
 $Revision: 2.1 $
 */
#pragma once

#ifdef _WINDOWS
    #include <winsock.h>
#endif

#include <sys/types.h>

#ifdef LINUX
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#define DEF_SERVER_ADDR "127.0.0.1"

namespace mplex
{

int OpenMother(int port);
int OpenNetwork(int nMudPort, char *pMudAddr);

} // namespace mplex
