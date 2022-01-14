/*
 $Author: tperry $
 $RCSfile: nice.cpp,v $
 $Date: 2003/01/30 04:08:58 $
 $Revision: 2.2 $
 */

#include "comm.h"
#include "db.h"
#include "main_functions.h"

#include <cstdio>

#define REBOOT_AT 10 /* 0-23, time of optional reboot if -e lib/reboot */

int check_reboot(void)
{
    long tc;
    struct tm *t_info;
    char buf[200];
    static int count = 12;

    tc = time(0);
    t_info = localtime(&tc);
    if (count < 12)
    {
        count -= 2;
        if (count > 0)
        {
            snprintf(buf,
                     sizeof(buf),
                     "ATTENTION: %s will automaticly reboot "
                     "in %d minutes.<br/>",
                     g_cServerConfig.getMudName().c_str(),
                     count);
            send_to_all(buf);
            return (1);
        }
        else
        {
            slog(LOG_ALL, 0, "REBOOT:  ***Automatic Reboot***");
            send_to_all("Automatic reboot.  Come back in a little while.<br/>");
            g_mud_shutdown = g_mud_reboot = 1;
            return (0);
        }
    }

    if ((t_info->tm_hour + 1) == g_cServerConfig.getReboot() && (t_info->tm_min > 49))
    {
        count -= 2;
        snprintf(buf,
                 sizeof(buf),
                 "ATTENTION: %s will automaticly reboot "
                 "in 10 minutes.<br/>",
                 g_cServerConfig.getMudName().c_str());
        send_to_all(buf);
        return (1);
    }
    return (0);
}
