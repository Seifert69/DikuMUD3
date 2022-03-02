/*
 $Author: All $
 $RCSfile: main.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.10 $
 */
#include "main.h"

#include "comm.h"
#include "compile_defines.h"
#include "diku_exception.h"
#include "main_functions.h"
#include "pipe.h"
#include "slog.h"
#include "structs.h"
#include "system.h"
#include "textutil.h"

#include <pthread.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define OPT_USEC 250000L /* time delay corresponding to 4 passes/sec */

int main(int argc, char **argv)
{
    try
    {
        char srvcfg[512];
        char *tempcfg = nullptr;
        char *log_name = nullptr;
        int pos = 1;

        tempcfg = getenv("VME_SERVER_CFG");
        if (tempcfg)
        {
            strcpy(srvcfg, tempcfg);
        }
        else
        {
            strcpy(srvcfg, "../etc/server.cfg"); // MS2020 add a default...
        }

        log_name = str_dup("./vme.log");

        while ((pos < argc) && (*(argv[pos]) == '-'))
        {
            switch (*(argv[pos] + 1))
            {
                case '?':
                case 'h':
                    ShowUsage(argv[0]);
                    exit(0);

                case 'c':
                    slog(LOG_OFF, 0, "Converting player file mode.");
                    g_player_convert = 1;
                    break;

                case 'C':
                    slog(LOG_OFF, 0, "Cleaning up player file mode.");
                    g_player_convert = 2;
                    break;

                case 'L':
                    slog(LOG_OFF, 0, "Listing player file mode.");
                    g_player_convert = 3;
                    break;

                case 's':
                    if (*(argv[pos] + 2))
                    {
                        strcpy(srvcfg, argv[pos] + 2);
                    }
                    else if (++pos < argc)
                    {
                        strcpy(srvcfg, argv[pos]);
                    }
                    else
                    {
                        slog(LOG_OFF, 0, "Full path and file name for the config file expected.");
                        exit(4);
                    }
                    break;

                case 'd':
                    g_dumptables = true;
                    break;

                case 'l':
                    if (*(argv[pos] + 2))
                    {
                        FREE(log_name);
                        log_name = str_dup(argv[pos] + 2);
                    }
                    else if (++pos < argc)
                    {
                        FREE(log_name);
                        log_name = str_dup(argv[pos]);
                    }
                    else
                    {
                        slog(LOG_OFF, 0, "Full path and file name for the config file expected.");
                        exit(5);
                    }
                    break;

                default:
                    slog(LOG_OFF, 0, "Unknown option %s in argument string.", (argv[pos]));
                    break;
            }
            pos++;
        }

        if (pos < argc)
        {
            if (!isdigit(*argv[pos]))
            {
                ShowUsage(argv[0]);
                exit(6);
            }
        }

        g_log_file_fd = freopen(log_name, "w", stderr);
        if (!g_log_file_fd)
        {
            fprintf(stderr, "Error in opening the log:  '%s'", log_name);
            free(log_name);
            exit(7);
        }

        free(log_name);

        type_validate_64(); // MS2020

#ifdef LINUX
/*    {   MS2020 - appears not to be used at all?
        int n;
        struct rlimit rlim;

        n = getrlimit (RLIMIT_CPU, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT CPU: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

        n = getrlimit (RLIMIT_DATA, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT DATA: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

        n = getrlimit (RLIMIT_STACK, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT STACK: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

        n = getrlimit (RLIMIT_CORE, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT CORE: %d / %d.", rlim.rlim_cur, rlim.rlim_max);

        n = getrlimit (RLIMIT_RSS, &rlim);
//      slog(LOG_ALL, 0, "RLIMIT RSS: %d / %d.", rlim.rlim_cur, rlim.rlim_max);
    }*/
#endif

        /*
        #if defined(GENERIC_SYSV) || defined(HPUX)
          srand48 (time (0));
        #elif defined(LINUX)
        */
        /* Other used routines are declared obsolete by SVID3 */
        //  srand (time (0));
        //#else
        srandom(time(nullptr));
        //#endif
        run_the_game(srvcfg);
    }
    catch (const diku_exception &e)
    {
        std::cout << "DIKU EXCEPTION: " << e.what() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "EXCEPTION: " << e.what() << std::endl;
    }
    return (0);
}
