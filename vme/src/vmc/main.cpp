/*
 $Author: All $
 $RCSfile: vmc.cpp,v $
 $Date: 2004/09/21 08:45:46 $
 $Revision: 2.10 $
 */

#include "common.h"
#include "compile_defines.h"
#include "money.h"
#include "pp.h"
#include "slog.h"
#include "vmc.h"

#include <unistd.h>

#include <cstdio>
#include <cstdlib>

#define MEMBLOCK (200000)
#define BUFS 30

int main(int argc, char **argv)
{
    int pos = 0;
    char moneyfile[512];
    char *money = nullptr;
    bool b_first = true;

    mem_init();

    money = getenv("VME_MONEY");
    if (money)
    {
        strcpy(moneyfile, money);
    }
    else
    {
        strcpy(moneyfile, "../etc/");
    }

    if (argc == 1)
    {
        ShowUsage(argv[0]);
        exit(1);
    }

    /*
    * not sure why this is here anymore.
    inc_dirs[inc_count++] = CPPI;
    */

    for (pos = 1; pos < argc; pos++)
    {
        if (*argv[pos] == '-')
        {
            switch (*(argv[pos] + 1))
            {
                case 'I':
                    Ipath[Ipcnt++] = argv[pos] + 2;
                    ;
                    break;
                case 'm':
                    g_make = 1;
                    break;
                case 's':
                    g_nooutput = 1;
                    break;
                case 'v':
                    g_verbose = 1;
                    break;
                case 'p':
                    pponly = 1;
                    break;
                case 'l':
                    g_fatal_warnings = 1;
                    break;
                case 'd':
                    if (*(argv[pos] + 2))
                    {
                        strcpy(moneyfile, argv[pos] + 2);
                    }
                    else if (++pos < argc)
                    {
                        strcpy(moneyfile, argv[pos]);
                    }
                    else
                    {
                        slog(LOG_OFF, 0, "Full path of the money file expected.");
                        exit(1);
                    }
                    break;
                case 'q':
                    g_quiet_compile = true;
                    break;

                case '?':
                    ShowUsage(argv[0]);
                    exit(0);
                case 'h':
                    ShowUsage(argv[0]);
                    exit(0);

                default:
                    fprintf(stderr, "Unknown option '%c'.\n", *(argv[pos] + 1));
                    ShowUsage(argv[0]);
                    exit(0);
            }
        }
        else
        {
            if (!g_quiet_compile && b_first)
            {
                fprintf(stderr, "VMC %s Copyright (C) 2001 by Valhalla [%s]\n", VERSION, __DATE__);
                fprintf(stderr, "VMC Compiled with [%s]\n", get_compiled_hash_defines().c_str());
                b_first = false;
            }
#ifndef WINDOWS
            alarm(15 * 60); /* If not done in 5 minutes, abort */
#endif
            boot_money(moneyfile); /* I guess it was inevitable... /gnort */
            fix(argv[pos]);
        }
    }

    if (!g_quiet_compile)
    {
        fprintf(stderr, "VMC Done.\n");
    }

    return 0;
}
