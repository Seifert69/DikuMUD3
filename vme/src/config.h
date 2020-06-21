/*
 $Author: All $
 $RCSfile: config.h,v $
 $Date: 2004/01/09 20:34:23 $
 $Revision: 2.4 $
 */

#ifndef _MUD_CONFIG_H
#define _MUD_CONFIG_H
#ifdef _WINDOWS
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#include "fight.h"
#include "common.h"
#include "color.h"


class CServerConfiguration
{
public:
    CServerConfiguration (void);

    void Boot (char *srvcfg);

    int FromLAN (char *pFromHost);
    int ValidMplex (struct sockaddr_in *isa);

    int m_nMotherPort;
    int m_nRentModifier;
    int m_bAccounting;
    int m_bAliasShout;
    int m_bBBS;
    int m_bLawful;
    int m_bNoSpecials;
    int m_bBOB;
    int m_nShout;
    int m_hReboot;

    class color_type color;
    struct in_addr m_sSubnetMask;
    struct in_addr m_sLocalhost;

    struct in_addr m_aMplexHosts[10];
    char *m_promptstr;
    char *m_libdir;		/* The lib directory, etc */
    char *m_plydir;
    char *m_etcdir;
    char *m_logdir;
    char *m_zondir;
    char *m_dilfiledir;
    char *m_mudname;		/* The mud name */
    char *m_pLogo;		/* Intro screen                    */
    char *m_pColor;
    char *m_pImmortName;		/*Name of the Immortal of the mud */
};

extern class CServerConfiguration g_cServerConfig;

#endif // _MUD_CONFIG_H
