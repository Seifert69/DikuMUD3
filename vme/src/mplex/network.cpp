/*
 $Author: All $
 $RCSfile: network.cpp,v $
 $Date: 2003/12/29 11:25:10 $
 $Revision: 2.2 $
 */

#ifdef _WINDOWS
#include <winsock2.h>
#include <time.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/un.h>

#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#ifdef IRIX
#include <netinet/tcp.h>
#endif
#ifdef LINUX
#include <netinet/tcp.h>
#endif

#include "network.h"
#include "protocol.h"
#include "textutil.h"

#if defined(AMIGA)
#include <machine/endian.h>
#endif


int OpenMother(int nPort)
{
    int n, fdMother;
    struct linger ld;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(nPort);
#ifdef _WINDOWS
    {
        WORD wVersionRequested;
        WSADATA wsaData;

        wVersionRequested = MAKEWORD(1, 1);

        if (WSAStartup(wVersionRequested, &wsaData) != 0) {
            slog(LOG_ALL,0,"SYSERR: WinSock not available!");
            exit(1);
        }

    }
#endif
    fdMother = socket(AF_INET, SOCK_STREAM, 0);

    if (fdMother == -1)
    {
        slog(LOG_OFF, 0, "Can't open Mother Connection");
        exit(1);
    }
#ifdef LINUX
    n = fcntl(fdMother, F_SETFL, FNDELAY);
#else
    unsigned long val=1;
    n=ioctlsocket(fdMother, FIONBIO, &val);
#endif

    if (n == -1)
    {
#ifdef _WINDOWS
        closesocket(fdMother);
#else
        close(fdMother);
#endif
        slog(LOG_OFF, 0, "Non blocking set error.");
        exit(1);
    }

    n = 1;
    if (setsockopt(fdMother,SOL_SOCKET,SO_REUSEADDR,(char *) &n, sizeof(n)) < 0)
    {
#ifdef _WINDOWS
        closesocket(fdMother);
#else
        close(fdMother);
#endif
        slog(LOG_OFF, 0, "setsockopt REUSEADDR");
        exit(1);
    }

    ld.l_onoff = 0;
    ld.l_linger = 1000;

    if (setsockopt(fdMother,SOL_SOCKET,SO_LINGER, (char *)&ld, sizeof(ld)) < 0)
    {
#ifdef _WINDOWS
        closesocket(fdMother);
#else
        close(fdMother);
#endif
        slog(LOG_OFF, 0, "setsockopt LINGER");
        exit(1);
    }

    n = bind(fdMother, (struct sockaddr *) &server_addr,
             sizeof(struct sockaddr_in));

    if (n != 0)
    {
#ifdef _WINDOWS
        closesocket(fdMother);
#else
        close(fdMother);
#endif
        slog(LOG_OFF, 0, "Can't bind Mother Connection port %d (errno %d).",
             nPort, errno);
        exit(1);
    }

    n = listen(fdMother, 5);

    if (n != 0)
    {
#ifdef _WINDOWS
        closesocket(fdMother);
#else
        close(fdMother);
#endif
        slog(LOG_OFF, 0, "Can't listen on Mother Connection.");
        exit(1);
    }

    return fdMother;
}


int OpenNetwork(int nPort, char *pcAddress)
{
    struct sockaddr_in server_addr;
    int fdClient;
    int n;

    slog(LOG_ALL, 0, "Open connection to server on %s %d.",
         pcAddress, nPort);

    memset((char *) &server_addr, 0, sizeof(struct sockaddr_in));

    server_addr.sin_family         = AF_INET;
    server_addr.sin_addr.s_addr    = inet_addr(pcAddress);
    server_addr.sin_port           = htons(nPort);

    fdClient = socket(AF_INET, SOCK_STREAM, 0);

    if (fdClient == -1)
    {
        slog(LOG_OFF, 0, "No TCP/IP connection to server.");
        return -1;
    }

    n = connect(fdClient,
                (struct sockaddr *) &server_addr,
                sizeof(struct sockaddr_in));

    if (n == -1)
    {
#ifdef _WINDOWS
        closesocket(fdClient);
#else
        close(fdClient);
#endif
        slog(LOG_OFF, 0, "No connect allowed to server.");
        return -1;
    }
#ifdef _WINDOWS
    unsigned long val = 1;
    n=ioctlsocket(fdClient,FIONBIO, &val);
#else
    n = fcntl(fdClient, F_SETFL, FNDELAY);
#endif
    if (n == -1)
    {
#ifdef _WINDOWS
        closesocket(fdClient);
#else
        close(fdClient);
#endif
        slog(LOG_OFF, 0, "Non blocking set error on server connection.");
        return -1;
    }

#ifdef _WINDOWS
    char buf[1024];
    n = setsockopt(fdClient, IPPROTO_TCP, TCP_NODELAY,buf, sizeof(buf));
#else
    int i;
    n = setsockopt(fdClient, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
#endif
    if (n == -1)
    {
#ifdef _WINDOWS
        closesocket(fdClient);
#else
        close(fdClient);
#endif
        slog(LOG_OFF, 0, "Setsockopt TCP_NODELAY failed on server connection.");
        exit(1);
    }
    return fdClient;
}
