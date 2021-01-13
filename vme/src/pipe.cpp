#include <stdio.h> 
#include <string> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <errno.h> 
#include "essential.h"
#include <cstring>
#include "dilrun.h"
#include "dbfind.h"
#include "vme.h"
#include "unitfind.h"
#include "dil.h"
#include "textutil.h"

int g_pipeMUD_RO = -1;
int g_pipeMUD_WO = -1;

int namedpipe_open_ro(void)
{
    // FIFO file path 
    const char *myfifo = "./pipeMUD";
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>, <permission>) 
    mkfifo(myfifo, 0666); 
  
    // Open FIFO for write only 
    g_pipeMUD_RO = open(myfifo, O_RDWR | O_NONBLOCK);

    if (g_pipeMUD_RO <= 0)
    {
        slog(LOG_OFF, 0, "Unable to open pipe RO %s, error code %d.", myfifo, errno);
        g_pipeMUD_RO = -1;
        return -1;
    }

    return g_pipeMUD_RO;
}


int namedpipe_open_wo(void)
{
    // FIFO file path 
    const char *myfifo = "./pipeDispatcher";
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>, <permission>) 
    mkfifo(myfifo, 0666); 
  
    // Open FIFO for write only 
    g_pipeMUD_WO = open(myfifo, O_WRONLY | O_NONBLOCK);

    if (g_pipeMUD_WO <= 0)
    {
        slog(LOG_OFF, 0, "Unable to open pipe WO %s, error code %d.", myfifo, errno);
        g_pipeMUD_WO = -1;
        return -1;
    }

    return g_pipeMUD_WO;
}


void pipeMUD_write(const char *c)
{
    if (g_pipeMUD_WO == -1)
    {
        g_pipeMUD_WO = namedpipe_open_wo();

        if (g_pipeMUD_WO == -1)
            return;
    }

    int n;
    string str;

    str = c;
    str.append("\n");

    n = write(g_pipeMUD_WO, str.c_str(), str.length());

    if (n == -1)
    {
        slog(LOG_OFF, 0, "Unable to write to pipeMUD WO, error code %d.", errno);
    }
    else if (n < (int) str.length())
    {
        slog(LOG_OFF, 0, "Unable to write full string to pipeMUD WO. code not developed for this yet", errno);
    }
    else
        slog(LOG_OFF, 0, "Wrote message [%s] to dispatcher.", str.c_str());
}


void pipeMUD_dispatch(std::string str)
{
    struct diltemplate *tmpl;
    class dilprg *prg;
    static unit_data *u = NULL;

    if (!u)
    {
        u = find_symbolic("basis", "discord");
        if (u == NULL)
        {
            slog(LOG_OFF, 0, "Can't find discord@basis.");
            return;
        }
    }

    tmpl = find_dil_template("dispatcher@comm");
    prg = dil_copy_template(tmpl, u, NULL);

    if (prg)
    {
        str_correct_utf8(str);
        prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file
        prg->fp->vars[0].val.string = str_dup(str.c_str());
        dil_activate(prg);
    }
}

// returns:
//  -1 on fail
//   0 on no data
// >=1 how many bytes of data
// str contains data.
//
int pipeMUD_read(std::string &str)
{
    int n;
    char buf[MAX_STRING_LENGTH];
    //fd_set read_fds; 
    //timeval null_time = {0, 0};    

    if (g_pipeMUD_RO == -1)
    {
        g_pipeMUD_RO = namedpipe_open_ro();

        if (g_pipeMUD_RO == -1)
            return -1;
    }

    str = "";

    do
    {
        buf[sizeof(buf)-1] = 0;
        n = read(g_pipeMUD_RO, buf, sizeof(buf)-1);
        
        if (n == -1)
        {
            if (errno == EAGAIN)
                break;
            slog(LOG_OFF, 0, "Error reading from pipeMUD RO %d, error code %d.", g_pipeMUD_RO, errno);
            break;
        }
        else // n > 0
        {
            buf[n] = 0;
            str.append(buf);
            if (n < (int) strlen(buf)-1)
                break;

            if (str.length() > 500000)
            {
                slog(LOG_OFF, 0, "pipeMUD RO, help I'm getting spammed. Discarding");
                str = ""; // Just consume the spam and toss it out
            }
        }
    }
    while (n > 0);

    if (str.length() > 0)
    {
        const char *p, *o;

        o = str.c_str();
        do {
            p = strchr(o, '\n');
            if (p == NULL)
            {
                if (strlen(o) < 1)
                    break;

                p = o + strlen(o);
            }
            std::string line(o, p-o);
            o = p;
            if (*o == '\n')
                o++;
            slog(LOG_OFF, 0, "Received line [%s]. Send to DIL", line.c_str());
            pipeMUD_dispatch(line);
        } while (p);
    }

    return str.length();
}


void namedpipe_setup(void)
{
    //namedpipe_open_wo();
    namedpipe_open_ro();
}

