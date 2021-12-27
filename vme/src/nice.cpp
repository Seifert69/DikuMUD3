/*
 $Author: tperry $
 $RCSfile: nice.cpp,v $
 $Date: 2003/01/30 04:08:58 $
 $Revision: 2.2 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>

#include "structs.h"
#include "system.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "comm.h"
#include "textutil.h"
#include "utility.h"

#define REBOOT_AT 10 /* 0-23, time of optional reboot if -e lib/reboot */

int check_reboot(void)
{
    long tc;
    struct tm *t_info;
    char buf[200];
    extern int mud_shutdown, mud_reboot;
    static int count = 12;

    tc = time(0);
    t_info = localtime(&tc);
    if (count < 12)
    {
        count -= 2;
        if (count > 0)
        {
            snprintf(buf, sizeof(buf),
                    "ATTENTION: %s will automaticly reboot "
                    "in %d minutes.<br/>",
                    g_cServerConfig.m_mudname,
                    count);
            send_to_all(buf);
            return (1);
        }
        else
        {
            slog(LOG_ALL, 0, "REBOOT:  ***Automatic Reboot***");
            send_to_all("Automatic reboot.  Come back in a little while.<br/>");
            mud_shutdown = mud_reboot = 1;
            return (0);
        }
    }

    if ((t_info->tm_hour + 1) == g_cServerConfig.m_hReboot && (t_info->tm_min > 49))
    {
        count -= 2;
        snprintf(buf, sizeof(buf),
                "ATTENTION: %s will automaticly reboot "
                "in 10 minutes.<br/>",
                g_cServerConfig.m_mudname);
        send_to_all(buf);
        return (1);
    }
    return (0);
}
