
/*
         $Author: All $
 $RCSfile: files.cpp,v $
 $Date: 2003/12/11 04:47:11 $
 $Revision: 2.8 $
 */

#ifdef _WINDOWS
    #include <direct.h>
    #include <io.h>
#endif

#include "error.h"
#include "files.h"
#include "textutil.h"
#include "utils.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cctype>
#include <cstdio>
#include <cstring>

/* Returns the size of a file in bytes */
long fsize(FILE *f)
{
    long oldpos = 0;
    long size = 0;

    oldpos = ftell(f);

    if (fseek(f, 0L, SEEK_END))
    { /* Seek to end of file */
        assert(FALSE);
    }

    size = ftell(f);

    if (fseek(f, oldpos, SEEK_SET))
    { /* Seek to end of file */
        assert(FALSE);
    }

    return size;
}

/* check if a file exists */
ubit1 file_exists(const char *name)
{
    FILE *fp = nullptr;

    if ((fp = fopen(name, "r")) == nullptr)
    {
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

ubit1 file_exists(const std::string &name)
{
    return file_exists(name.c_str());
}

/* create a file if it doesn't exist. if error, terminate */
void touch_file(const char *name)
{
    FILE *fp = nullptr;

    if (file_exists(name))
    {
        return;
    }

    if ((fp = fopen(name, "w")) == nullptr)
    {
        fprintf(stderr, "touch_file(): Couldn't create %s...\n", name);
        assert(FALSE);
    }
    fclose(fp);
}

void touch_file(const std::string &name)
{
    touch_file(name.c_str());
}

char *fread_line_commented(FILE *fl, char *buf, int max)
{
    char *s = nullptr;

    for (;;)
    {
        s = fgets(buf, max, fl);

        if (s == nullptr)
        {
            break;
        }

        if (*skip_spaces(buf) != '#')
        {
            break;
        }
    }

    return s;
}

/* read and allocate space for a '~'-terminated string from a given file */
char *fread_string_copy(FILE *fl, char *buf, int max)
{
    char *obuf = nullptr;
    char *point = nullptr;
    int flag = 0;
    int total = 0;

    memset(buf, '\0', max);
    total = 0;
    obuf = buf;

    do
    {
        if (!fgets(buf, max - total, fl))
        {
            error(HERE, "fread_string_copy");
        }

        total += strlen(buf);

        if (total >= max)
        {
            error(HERE, "fread_string_copy: string too large (db.c)");
        }

        for (point = buf + strlen(buf) - 2; point >= buf && isspace(*point); point--)
        {
            ;
        }

        if ((flag = (*point == '~')))
        {
            if (*(buf + strlen(buf) - 3) == '\n')
            {
                *(buf + strlen(buf) - 2) = '\r';
                *(buf + strlen(buf) - 1) = '\0';
            }
            else
            {
                *(buf + strlen(buf) - 2) = '\0';
            }
        }
        else
        {
            *(buf + strlen(buf) + 1) = '\0';
            *(buf + strlen(buf)) = '\r';
        }
        TAIL(buf);
    } while (!flag);

    return obuf;
}

/* read and allocate space for a '~'-terminated string from a given file */
char *fread_string(FILE *fl)
{
    char buf[MAX_STRING_LENGTH];

    fread_string_copy(fl, buf, MAX_STRING_LENGTH);

    if (strlen(buf) > 0)
    {
        return str_dup(buf);
    }
    else
    {
        return nullptr;
    }
}

/* Read contents of a file, but skip all remark lines and blank lines. */
int config_file_to_string(const char *name, char *buf, int max_len)
{
    FILE *fl = nullptr;
    char tmp[500];

    *buf = '\0';

    if (!(fl = fopen(name, "r")))
    {
        DEBUG("File-to-string error.\n");
        *buf = '\0';
        return (-1);
    }

    do
    {
        if (fgets(tmp, sizeof(tmp) - 1, fl))
        {
            if (tmp[0] == '#')
            {
                continue;
            }

            if (tmp[0] == 0)
            {
                continue;
            }

            if (strlen(buf) + strlen(tmp) + 2 > (ubit32)max_len)
            {
                DEBUG("fl->strng: string too big (db.c, file_to_string)\n");
                strcpy(buf + strlen(buf) - 20, "TRUNCATED!");
                fclose(fl);
                return (-1);
            }

            strcat(buf, tmp);
            *(buf + strlen(buf) + 1) = '\0';
            *(buf + strlen(buf)) = '\r';
        }
    } while (!feof(fl));

    fclose(fl);

    return (0);
}

int config_file_to_string(const std::string &name, char *buf, int max_len)
{
    return config_file_to_string(name.c_str(), buf, max_len);
}

/* read contents of a text file, and place in buf */
int file_to_string(const char *name, char *buf, int max_len)
{
    FILE *fl = nullptr;
    char tmp[500];

    *buf = '\0';

    if (!(fl = fopen(name, "r")))
    {
        slog(LOG_ALL, 0, "File-to-string error");
        *buf = '\0';
        return (-1);
    }

    do
    {
        if (fgets(tmp, sizeof(tmp) - 1, fl))
        {
            if (strlen(buf) + strlen(tmp) + 2 > (ubit32)max_len)
            {
                slog(LOG_ALL, 0, "fl->strng: string too big (db.c, file_to_string)");
                strcpy(buf + strlen(buf) - 20, "TRUNCATED!");
                return -1;
            }

            strcat(buf, tmp);
            *(buf + strlen(buf) /* + 1*/) = '\0';
            /*	  *(buf + strlen (buf)) = '\r';*/
        }
    } while (!feof(fl));

    fclose(fl);

    return 0;
}

int file_to_string(const std::string &name, char *buf, int max_len)
{
    return file_to_string(name.c_str(), buf, max_len);
}
/* Read a null terminated string from file into str */
void fstrcpy(CByteBuffer *pBuf, FILE *f)
{
    int c = 0;

    pBuf->Clear();

    while ((c = fgetc(f)) && (c != EOF))
    {
        pBuf->Append8(c);
    }

    pBuf->Append8(0);
}

/************************************************************************
 *  General purpose file Handling Procedures Below this line            *
 ********************************************************************** */

/* fopen_cache below this comment */

struct fcache_type
{
    int hits;
    int name_s;
    char *name;
    FILE *file;
};

static fcache_type fcache[FCACHE_MAX] = {
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr},
    {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}, {0, 0, nullptr, nullptr}};

char *enl_strcpy(char *dest, const char *source, int *dest_size)
{
    int len = strlen(source);

    if ((dest == nullptr) || (*dest_size < len + 1))
    {
        if (dest)
        {
            RECREATE(dest, char, len + 1);
        }
        else
        {
            CREATE(dest, char, 1 + len);
        }
        *dest_size = len + 1;
    }

    return strcpy(dest, source);
}

/* The mode is interpreted as follows:

   If mode contains a 'w' then the file is always opened as "w+"
   If mode contains an 'r' then the file is initially opened as "r+"
   If mode contains an 'a' then the file is initially opened as "a+"

Then succeeding calls to fopen cache with any mode (but 'w') will cause
a fseek to the end or start (for 'a' or 'r'). Thus performance is gained,
BUT 'read-only' files may be written to!

*/

FILE *fopen_cache(const char *name, const char *mode)
{
    int i = 0;
    int min_i = 0;
    int hit_i = 0;
    static int pure_hits = 0;
    static int purge = 0;

    min_i = number(0, FCACHE_MAX - 1); /* In case many are equal */
    hit_i = -1;

    for (i = 0; i < FCACHE_MAX; i++)
    {
        if (fcache[i].hits < fcache[min_i].hits)
        {
            min_i = i;
        }
        if (fcache[i].name && !strcmp(name, fcache[i].name))
        {
            hit_i = i;
        }
        fcache[i].hits--;
    }

    if (hit_i == -1)
    {
        if (fcache[min_i].file)
        {
            if (fclose(fcache[min_i].file) != 0)
            {
                error(HERE, "Error on fcache fclose() on file [%s].", fcache[min_i].name);
            }
            purge++;
        }
        fcache[min_i].name = enl_strcpy(fcache[min_i].name, name, &fcache[min_i].name_s);
        fcache[min_i].hits = 0;

        if (strchr(mode, 'w'))
        {
            fcache[min_i].file = fopen(name, "w+b");
        }
        else if (strchr(mode, 'a'))
        {
            fcache[min_i].file = fopen(name, "a+b");
        }
        else if (strchr(mode, 'r'))
        {
            fcache[min_i].file = fopen(name, "r+b");
        }
        else
        {
            error(HERE, "Bad file mode [%s] for file [%s]", mode, name);
        }

        return fcache[min_i].file;
    }
    else
    {
        if (!fcache[hit_i].file)
        {
            return nullptr;
        }
        if (strchr(mode, 'w'))
        {
            fcache[hit_i].file = freopen(name, "w+b", fcache[hit_i].file);
        }
        else if (strchr(mode, 'a'))
        {
            fseek(fcache[hit_i].file, 0L, SEEK_END);
        }
        else if (strchr(mode, 'r'))
        {
            fseek(fcache[hit_i].file, 0L, SEEK_SET);
        }
        else
        {
            error(HERE, "Bad file mode [%s] for file [%s]", mode, name);
        }

        pure_hits++;

        fcache[hit_i].hits = 0;

        /* Consider fflush(fcache[hit_i].file); */

        return fcache[hit_i].file;
    }
}

FILE *fopen_cache(const std::string &name, const char *mode)
{
    return fopen_cache(name.c_str(), mode);
}

void fclose_cache()
{
    int i = 0;

    /* closes all files in the cache! */

    for (i = 0; i < FCACHE_MAX; i++)
    {
        if (fcache[i].file)
        {
            if (fclose(fcache[i].file) != 0)
            {
                slog(LOG_ALL, 0, "fcache close failed on file %s.", fcache[i].name);
                return;
            }
            else
            {
                fcache[i].file = nullptr;
            }
        }
    }
}
int max_size_load = 1000000;
int max_size_save = 1000000;

int load_string(char *filename, char **file_str)
{
    int input = 0;
    unsigned int nread = 0;
    unsigned int rt = 0;
    char *temp = nullptr;
    struct stat statbuf;
    if (!file_exists(filename))
    {
        slog(LOG_ALL, 0, "load_string(): File does not exist: %s", filename);
        return (FILE_NOT_FOUND);
    }

    stat(filename, &statbuf);
    input = open(filename, O_RDONLY);
    if (!input)
    {
        slog(LOG_ALL, 0, "load_string(): File won't open: %s", filename);
        return (FILE_NOT_FOUND);
    }

    if (statbuf.st_size > max_size_load)
    {
        slog(LOG_ALL, 0, "load_string(): File too large: %s", filename);
        return (FILE_TO_LARGE);
    }

    *file_str = (char *)malloc(statbuf.st_size + 1);
    if (!(*file_str))
    {
        return (FILE_OUT_OF_MEMORY);
    }
    temp = *file_str;
    nread = statbuf.st_size;
    while (nread > 0)
    {
#ifndef _WINDOWS
        if (nread > (unsigned int)statbuf.st_blksize)
        {
            rt = read(input, temp, statbuf.st_blksize);
        }
        else
        {
            rt = read(input, temp, nread);
        }
#else
        rt = read(input, temp, nread);
#endif
        temp = temp + rt;
        nread = nread - rt;
    }
    *temp = 0;

    close(input);
    return (FILE_LOADED);
}

int save_string(char *filename, char **file_str, char *opp)
{
    int output = 0;
    unsigned int nwrite = 0;
    char *temp = nullptr;
    struct stat statbuf;
    if ((opp[0] != 'a') && (opp[0] != 'w'))
    {
        return (FILE_ILEGAL_OPP);
    }

    if ((file_str == nullptr) || (*file_str[0] == 0))
    {
        return (FILE_NOT_SAVED);
    }

    if (opp[0] == 'w')
    {
#ifdef _WINDOWS
        output = open(filename, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);
#else
        output = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
#endif
    }
    else
    {
#ifdef _WINDOWS
        output = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IREAD | S_IWRITE);
#else
        output = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
#endif
    }
    if (!output)
    {
        return (FILE_NOT_CREATED);
    }
    stat(filename, &statbuf);
    nwrite = strlen(*file_str);
    temp = *file_str;
    while (nwrite > 0)
    {
#ifndef _WINDOWS
        if (nwrite > (unsigned int)statbuf.st_blksize)
        {
            size_t mstmp = write(output, temp, statbuf.st_blksize);
            if (mstmp < 1)
            {
                slog(LOG_ALL, 0, "ERROR: Unexpected bytes written %d in save string", mstmp);
                assert(FALSE);
            }

            temp = temp + statbuf.st_blksize;
            nwrite = nwrite - statbuf.st_blksize;
        }
        else
        {
            size_t mstmp = write(output, temp, nwrite);
            if (mstmp < 1)
            {
                slog(LOG_ALL, 0, "ERROR: Unexpected bytes written %d in save string", mstmp);
                assert(FALSE);
            }
            nwrite = nwrite - nwrite;
        }
#else
        size_t mstmp = write(output, temp, nwrite);
        if (mstmp < 1)
        {
            slog(LOG_ALL, 0, "ERROR: Unexpected bytes written %d in save string", mstmp);
            assert(FALSE);
        }
        nwrite = nwrite - nwrite;
#endif
    }

    close(output);
    return (FILE_SAVED);
}

int store_name_test(char *name)
{
    int i = 0;
    int ln = 0;

    if (str_is_empty(name))
    {
        return (0);
    }

    ln = strlen(name);
    if (ln > 512)
    {
        return (0);
    }

    for (i = 0; (i < ln) && ((isalnum(name[i])) || (name[i] == '_') || (name[i] == '.')); i++)
    {
        ;
    }

    if (i < ln)
    {
        return (0);
    }
    return (1);
}
