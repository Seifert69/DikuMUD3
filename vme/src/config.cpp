/*
 $Author: All $
 $RCSfile: config.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.5 $
 */

#ifdef _WINDOWS
#include <winsock2.h>
#include <io.h>
#else
#include <unistd.h>

#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "essential.h"
#include "utility.h"
#include "config.h"
#include "files.h"
#include "textutil.h"
#include "db.h"
#include "str_parse.h"
#include "common.h"
#include "color.h"
#include "values.h"
#include "structs.h"
class CServerConfiguration g_cServerConfig;

CServerConfiguration::CServerConfiguration(void)
{
    m_bAccounting = 0;
    m_bBBS = FALSE;
    m_bLawful = FALSE;
    m_bAliasShout = TRUE;
    m_bNoSpecials = FALSE;
    m_bBOB = FALSE;
    m_nShout = 1;
    m_nRentModifier = 10;
    m_nMotherPort = 4999;

    memset(&m_sSubnetMask, 0, sizeof(m_sSubnetMask));
    memset(&m_sLocalhost, 0, sizeof(m_sLocalhost));
    m_logdir = NULL;
    m_libdir = NULL;
    m_etcdir = NULL;
    m_zondir = NULL;
    m_plydir = NULL;
    m_mudname = NULL;
    m_pLogo = NULL;
    m_pImmortName = NULL;
    m_hReboot = 0;
}

int CServerConfiguration::FromLAN(char *pFromHost)
{
    struct in_addr sTmp;

#ifdef _WINDOWS
    if (inet_addr(pFromHost) == INADDR_NONE)
    {
        slog(LOG_ALL, 0, "Localhost invalid.");
        return FALSE;
    }
#else
    if (inet_aton(pFromHost, &sTmp) == 0)
    {
        slog(LOG_ALL, 0, "Localhost [%s] invalid.", pFromHost);
        return FALSE;
    }
#endif

    return ((m_sSubnetMask.s_addr & m_sLocalhost.s_addr) ==
            (m_sSubnetMask.s_addr & sTmp.s_addr));
}

int CServerConfiguration::ValidMplex(struct sockaddr_in *isa)
{

    int i;

    for (i = 0; i < 10; i++)
    {
        if (isa->sin_addr.s_addr == m_aMplexHosts[i].s_addr)
            return TRUE;
    }

    return FALSE;
}

void CServerConfiguration::Boot(char *srvcfg)
{
    char Buf[2 * MAX_STRING_LENGTH];
    char *c, *d;
    int i;
#ifdef _WINDOWS
    struct _stat statbuf;
#else
    struct stat statbuf;

#endif

    slog(LOG_OFF, 0, "Booting server.");

    if (!file_exists(srvcfg))
    {
        slog(LOG_ALL, 0, "Could not find server configuration file. %s",
             srvcfg);
        exit(0);
    }

    config_file_to_string(srvcfg, Buf, sizeof(Buf));

    c = Buf;

    d = parse_match_name((const char **)&c, "mudname");
    if (d == NULL)
        d = str_dup("No Mud server name in server.cfg");
    slog(LOG_ALL, 0, "The Mud Name is %s.", d);
    if (strlen(d) > 49)
        d[50] = '\0';

    m_mudname = d;

    d = parse_match_name((const char **)&c, "immortal_name");
    if (d == NULL)
        d = str_dup("immortal");
    if (strlen(d) > PC_MAX_NAME)
        d[PC_MAX_NAME] = '\0';

    m_pImmortName = d;

    d = parse_match_name((const char **)&c, "libdir");
    if (d == NULL)
        d = str_dup("../lib/");

#ifdef _WINDOWS
    _stat(d, &statbuf);
    if (!IS_SET(statbuf.st_mode, _S_IFDIR))
#else
    stat(d, &statbuf);
    if (!S_ISDIR(statbuf.st_mode))
#endif
    {
        slog(LOG_ALL, 0, "The lib directory %s does not exist.", d);
        exit(0);
    }
    slog(LOG_ALL, 0, "The lib directory is %s.", d);
    m_libdir = d;

    d = parse_match_name((const char **)&c, "etcdir");
    if (d == NULL)
        d = str_dup("../etc/");

#ifdef _WINDOWS
    _stat(d, &statbuf);
    if (!IS_SET(statbuf.st_mode, _S_IFDIR))
#else
    stat(d, &statbuf);
    if (!S_ISDIR(statbuf.st_mode))
#endif
    {
        slog(LOG_ALL, 0, "The etc directory %s does not exist.", d);
        exit(0);
    }
    slog(LOG_ALL, 0, "The etc directory is %s.", d);
    m_etcdir = d;

    d = parse_match_name((const char **)&c, "logdir");
    if (d == NULL)
        d = str_dup("../log/");

#ifdef _WINDOWS
    _stat(d, &statbuf);
    if (!IS_SET(statbuf.st_mode, _S_IFDIR))
#else
    stat(d, &statbuf);
    if (!S_ISDIR(statbuf.st_mode))
#endif
    {
        slog(LOG_ALL, 0, "The log directory %s does not exist.", d);
        exit(0);
    }
    slog(LOG_ALL, 0, "The log directory is %s.", d);
    m_logdir = d;

    d = parse_match_name((const char **)&c, "zondir");
    if (d == NULL)
        d = str_dup("../zone/");

#ifdef _WINDOWS
    _stat(d, &statbuf);
    if (!IS_SET(statbuf.st_mode, _S_IFDIR))
#else
    stat(d, &statbuf);
    if (!S_ISDIR(statbuf.st_mode))
#endif
    {
        slog(LOG_ALL, 0, "The zone directory %s does not exist.", d);
        exit(0);
    }
    slog(LOG_ALL, 0, "The zone directory is %s.", d);
    m_zondir = d;

    d = parse_match_name((const char **)&c, "plydir");
    if (d == NULL)
        d = str_dup("../lib/ply/");

#ifdef _WINDOWS
    _stat(d, &statbuf);
    if (!IS_SET(statbuf.st_mode, _S_IFDIR))
#else
    stat(d, &statbuf);
    if (!S_ISDIR(statbuf.st_mode))
#endif
    {
        slog(LOG_ALL, 0, "The player directory %s does not exist.", d);
        exit(0);
    }
    slog(LOG_ALL, 0, "The player directory is %s.", d);
    m_plydir = d;

    d = parse_match_name((const char **)&c, "dil_file_dir");
    if (d == NULL)
        d = str_dup("../lib/file/");

#ifdef _WINDOWS
    _stat(d, &statbuf);
    if (!IS_SET(statbuf.st_mode, _S_IFDIR))
#else
    stat(d, &statbuf);
    if (!S_ISDIR(statbuf.st_mode))
#endif
    {
        slog(LOG_ALL, 0, "The dil file directory %s does not exist.", d);
        exit(0);
    }
    slog(LOG_ALL, 0, "The dil file directory is %s.", d);
    m_dilfiledir = d;
    if (parse_match_num((const char **)&c, "Port", &i))
    {
        m_nMotherPort = i;
    }

    if (!is_in(m_nMotherPort, 2000, 8000))
    {
        slog(LOG_ALL, 0, "Mother port not in [2000..8000].");
        exit(0);
    }

    if (parse_match_num((const char **)&c, "auto_reboot_hour", &i))
    {
        m_hReboot = i;
    }

    if (!((m_hReboot >= 0) && (m_hReboot <= 24)))
    {
        m_hReboot = 0;
        slog(LOG_ALL, 0, "Auto Reboot Set to an invaild hour, disabling.");
    }

    if (parse_match_num((const char **)&c, "Rent", &i))
    {
        m_nRentModifier = i;
    }

    if (!is_in(m_nRentModifier, 0, 100))
    {
        slog(LOG_ALL, 0, "Rent modifier not in [0..100].");
        exit(0);
    }

    if (parse_match_num((const char **)&c, "BOB", &i))
    {
        m_bBOB = i;
    }

    if (!is_in(m_bBOB, 0, 1))
    {
        slog(LOG_ALL, 0, "BOB not 0 or 1");
        exit(0);
    }

    if (parse_match_num((const char **)&c, "Alias Shout", &i))
    {
        m_bAliasShout = i;
    }

    if (!is_in(m_bAliasShout, 0, 1))
    {
        slog(LOG_ALL, 0, "Alias Shout not 0 or 1");
        exit(0);
    }

    if (parse_match_num((const char **)&c, "No Specials", &i))
    {
        m_bNoSpecials = i;
    }

    if (!is_in(m_bNoSpecials, 0, 1))
    {
        slog(LOG_ALL, 0, "Specials not 0 or 1");
        exit(0);
    }

    if (parse_match_num((const char **)&c, "Lawful", &i))
    {
        m_bLawful = i;
    }

    if (!is_in(m_bLawful, 0, 1))
    {
        slog(LOG_ALL, 0, "Lawful not 0 or 1");
        exit(0);
    }

    if (parse_match_num((const char **)&c, "BBS", &i))
    {
        m_bBBS = i;
    }

    if (!is_in(m_bBBS, 0, 1))
    {
        slog(LOG_ALL, 0, "BBS not 0 or 1");
        exit(0);
    }

    if (parse_match_num((const char **)&c, "Accounting", &i))
    {
        m_bAccounting = i;
    }

    if (!is_in(m_bAccounting, 0, 1))
    {
        slog(LOG_ALL, 0, "Accounting not 0 or 1");
        exit(0);
    }

    if (parse_match_num((const char **)&c, "Shout", &i))
    {
        m_nShout = i;
    }

    if (!is_in(m_nShout, 0, 255))
    {
        slog(LOG_ALL, 0, "Shout level not [0..255]");
        exit(0);
    }

    d = parse_match_name((const char **)&c, "subnetmask");

    if (d == NULL)
        d = str_dup("255.255.255.255");

#ifdef _WINDOWS
    if (m_sSubnetMask.S_un.S_addr = inet_addr(d) == INADDR_NONE)
    {
        slog(LOG_ALL, 0, "SubnetMask invalid.");
        exit(0);
    }

#else
    if (inet_aton(d, &m_sSubnetMask) == 0)
    {
        slog(LOG_ALL, 0, "SubnetMask invalid.");
        exit(0);
    }
#endif

    d = parse_match_name((const char **)&c, "localhost");

    if (d == NULL)
        d = str_dup("127.0.0.1");

#ifdef _WINDOWS

    if (m_sLocalhost.S_un.S_addr = inet_addr(d) == INADDR_NONE)
        if (inet_addr(d) == INADDR_NONE)
        {
            slog(LOG_ALL, 0, "Localhost invalid.");
            exit(0);
        }

#else

    if (inet_aton(d, &m_sLocalhost) == 0)
    {
        slog(LOG_ALL, 0, "Localhost [%s] invalid.", d);
        exit(0);
    }

#endif
    char **ppNames;

    ppNames = parse_match_namelist((const char **)&c, "mplex hosts");

    if (ppNames == NULL)
    {
        slog(LOG_ALL, 0, "Mplex hosts must be specified.");
        exit(0);
    }

    int l = 0;

    while (ppNames[l])
        l++;

    if (l < 1)
    {
        slog(LOG_ALL, 0, "Mplex hosts must have at least one entry.");
        exit(0);
    }

    if (l > 10)
    {
        slog(LOG_ALL, 0, "10 is maximum number of mplex hosts.");
        exit(0);
    }

    for (i = 0; i < 10; i++)
    {
        if (i < l)
        {
#ifdef _WINDOWS
            if (m_aMplexHosts[i].S_un.S_addr =
                    inet_addr(ppNames[i]) == INADDR_NONE)
            {
                slog(LOG_ALL, 0, "SubnetMask invalid.");
                exit(0);
            }

#else
            if (inet_aton(ppNames[i], &m_aMplexHosts[i]) == 0)
            {
                slog(LOG_ALL, 0, "Mplex host invalid IP.");
                exit(0);
            }
#endif
        }
        else
            m_aMplexHosts[i] = m_aMplexHosts[i - 1];
    }

    d = parse_match_name((const char **)&c, "promptstr");
    m_promptstr = d;

    slog(LOG_OFF, 0, "Reading info and configuration files.");

    char *read_info_file(char *name, char *oldstr);

    slog(LOG_OFF, 0, "Reading in etc / colors.");
    touch_file(str_cc(m_etcdir, COLOR_FILE));
    m_pColor = read_info_file(str_cc(m_etcdir, COLOR_FILE), m_pColor);
    color.create(m_pColor);

    slog(LOG_OFF, 0, "Reading in etc / logo.");
    touch_file(str_cc(m_etcdir, LOGO_FILE));
    m_pLogo = read_info_file(str_cc(m_etcdir, LOGO_FILE), m_pLogo);
}
