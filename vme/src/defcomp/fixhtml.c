/*
  MS2020 This is not a good home for fixhtml.c :)

  Had to write this to convert tens of thousands lines of
  code with old symbols, &n, &x, &c+w to HTML
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define BUFFER_SIZE 4096

int g_nComment = 0;
int g_nQuote = 0;
int g_nDiv = 0;
int g_bPre = 0;

char *spc(int n)
{
    static char buf[256];

    buf[n] = '\0';

    if (n > 256)
        n = 256;

    for (; n;)
        buf[--n] = ' ';

    return buf;
}

void process_line(char *c, int ln, int nLine)
{
    char *p, *q, *r;
    char buf[BUFFER_SIZE];
    char *b;
    int nLineComment = 0;
    static int nQuoteLine;

    b = buf;

    for (int i = 0; i < ln; i++)
    {
        *b++ = c[i];

        if (nLineComment)
            continue;

        if (!g_nComment && (c[i] == '"'))
        {
            if (g_nQuote == 0)
                nQuoteLine = nLine;

            g_nQuote = 1 - g_nQuote;
            if (g_nQuote == 0)
            {
                if (nLine - nQuoteLine > 100)
                {
                    fprintf(stderr, "Line %d: Warning. Over 100 lines to the next quote\n", nQuoteLine);
                    //exit(1);
                }
                while (g_nDiv > 0)
                {
                    g_nDiv--;
                    *(b - 1) = '<';
                    *b++ = '/';
                    *b++ = 'd';
                    *b++ = 'i';
                    *b++ = 'v';
                    *b++ = '>';
                    *b++ = '"';
                }
                if (g_bPre)
                {
                    *(b - 1) = '<';
                    *b++ = '/';
                    *b++ = 'p';
                    *b++ = 'r';
                    *b++ = 'e';
                    *b++ = '>';
                    *b++ = '"';
                    g_bPre = 0;
                }
            }
            continue;
        }

        if (g_nQuote)
        {
            if ((c[i] == '&') && (c[i + 1] == '['))
            {
                char tmp[50];
                int j;

                for (j = 0; c[i+2+j] && j < 30; j++)
                {
                    tmp[j] = c[i+2+j];
                    if (c[i+2+j] == ']')
                        break;                    
                }
                if (j < 1)
                {
                    fprintf(stderr, "Line %d: &[] too short %s", nLine, buf);
                    exit(1);
                }

                tmp[j] = 0;
                if (j > 20)
                {
                    fprintf(stderr, "Line %d: &[%s] too long", nLine, tmp);
                    exit(1);                    
                }

                if (g_nDiv > 0)
                {
                    sprintf((b-1), "</div><div class='%s'>", tmp);
                }
                else
                {
                    sprintf((b-1), "<div class='%s'>", tmp);
                    g_nDiv++;
                }
                
                while (*b++) ; //TAIL
                b--;
                i += 1; // skip [
                i += j; // skip name
                i += 1; // skip ]
            }
            else if ((c[i] == '&') && (c[i + 1] == 's'))
            {
                char tmp[4];

                if (isdigit(c[i + 2]) && isdigit(c[i + 3]))
                {
                    tmp[0] = c[i+2];
                    tmp[1] = c[i+3];
                    tmp[2] = 0;
                    i += 3;
                }
                else if (isdigit(c[i + 2]))
                {
                    tmp[0] = c[i+2];
                    tmp[1] = 0;
                    i += 2;
                }
                else
                {
                    tmp[0] = '1';
                    tmp[1] = 0;
                    i += 1;                    
                }
                sprintf((b-1), "%s", spc(atoi(tmp)));
                while (*b++) ; //TAIL
                b--;
            }
            else if ((c[i] == '&') && (c[i + 1] == 'b'))
            {
                char tmp[4];

                tmp[0] = 'b';
                tmp[1] = c[i + 2];
                tmp[2] = 0;                    
                i += 2;
                
                if (g_nDiv > 0)
                {
                    sprintf((b-1), "</div><div class='%s'>", tmp);
                }
                else
                {
                    sprintf((b-1), "<div class='%s'>", tmp);
                    g_nDiv++;
                }
                while (*b++) ; //TAIL
                b--;
            }
            else if ((c[i] == '&') && (c[i + 1] == 'c'))
            {
                char tmp[4];

                if (c[i + 2] == '+')
                {
                    tmp[0] = 'c';
                    tmp[1] = 'p';
                    tmp[2] = c[i + 3];
                    tmp[3] = 0;
                    i += 3;
                }
                else
                {
                    tmp[0] = 'c';
                    tmp[1] = c[i + 2];
                    tmp[2] = 0;                    
                    i += 2;
                }
                
                if (g_nDiv > 0)
                {
                    sprintf((b-1), "</div><div class='%s'>", tmp);
                }
                else
                {
                    sprintf((b-1), "<div class='%s'>", tmp);
                    g_nDiv++;
                }
                while (*b++) ; //TAIL
                b--;
            }
            else if ((c[i] == '&') && (c[i + 1] == 'n'))
            {
                *(b - 1) = '<';
                *b++ = 'b';
                *b++ = 'r';
                *b++ = '/';
                *b++ = '>';
                i++; // Skip n
            }
            else if ((c[i] == '&') && (c[i + 1] == 'x'))
            {
                *(b - 1) = '\n';
            }
            else if ((c[i] == '&') && ((c[i + 1] == 'h') || (c[i + 1] == 'p') || (c[i + 1] == 'f')))  
            {
                b--; // Let's overwrite the &
                i++; // Skip h/p/f
            }
            else if ((c[i] == '&') && ((c[i + 1] == '&') || (c[i + 1] == ' ')))
            {
                // Presume && was forgotten if & followed by space (common error)
                // Works the same in pre and not pre
                *(b - 1) = '&';
                *b++ = 'a';
                *b++ = 'm';
                *b++ = 'p';
                *b++ = ';';
                i++; // SKip escaped &
            }
            else if ((c[i] == '&') && (c[i + 1] == 'l'))
            {
                if (g_bPre)
                {
                    b--; // Skip &
                    i++; // skip l
                    continue;
                }

                if (g_nDiv > 0)
                {
                    *(b - 1) = '<';
                    *b++ = '/';
                    *b++ = 'd';
                    *b++ = 'i';
                    *b++ = 'v';
                    *b++ = '>';
                    *b++ = 0; // Because it does (b-1) below
                    g_nDiv--;
                }
                *(b - 1) = '<';
                *b++ = 'p';
                *b++ = 'r';
                *b++ = 'e';
                *b++ = '>';
                i++; // Skip l
                g_bPre = 1;
            }
            else if ((c[i] == '&') && (c[i + 1] == 'g'))
            {

            }
            else if ((c[i] == '&') && (c[i + 1] == 'a'))
            {

            }
            else if (c[i] == '&')
            {
                fprintf(stderr, "Line %d ERROR: Unknown code &[%c]\n", nLine, c[i+1]);
            }
            else if ((c[i] == '\n') && (c[i] == '\r') && g_bPre)
            {
                *(b - 1) = '<';
                *b++ = 'b';
                *b++ = 'r';
                *b++ = '/';
                *b++ = '>';
                *b++ = '\n';
                *b++ = '\r';
                i++; // Skip \r
            }
            else if ((c[i] == '\n') && g_bPre)
            {
                *(b - 1) = '<';
                *b++ = 'b';
                *b++ = 'r';
                *b++ = '/';
                *b++ = '>';
                *b++ = '\n';
            }
            else if (c[i] == '&')
            {
                *(b - 1) = '&';
                *b++ = 'a';
                *b++ = 'm';
                *b++ = 'p';
                *b++ = ';';
            }
            else if (c[i] == '<')
            {
                *(b - 1) = '&';
                *b++ = 'l';
                *b++ = 't';
                *b++ = ';';
            }
            else if (c[i] == '>')
            {
                *(b - 1) = '&';
                *b++ = 'g';
                *b++ = 't';
                *b++ = ';';
            }
            continue;
        }

        if ((c[i] == '/') && (c[i + 1] == '/'))
        {
            nLineComment = 1;
            continue;
        }

        if ((c[i] == '/') && c[i + 1] == '*')
        {
            if (g_nComment)
            {
                fprintf(stderr, "Line %d: /* in /* : %s\n", nLine, c);
                exit(1);
            }
            g_nComment = 1;
        }

        if ((c[i] == '*') && (c[i + 1] == '/'))
        {
            if (!g_nComment)
            {
                fprintf(stderr, "Line %d: */ without /* : %s\n", nLine, c);
                exit(1);
            }
            g_nComment = 0;
        }
    }
    *b = 0;

    fprintf(stdout, "%s", buf);
}


int main(int argc, char **argv)
{
    int file;
    char buffer[BUFFER_SIZE];
    char *c;
    char *p, *q;
    size_t n;
    ssize_t r;
    int bComment = 0;
    int nLine;
    int bQuote = 0;

    c = buffer;

    if (argc >= 2)
    {
        if ((strcmp(argv[1], "-q") != 0) || (argc >= 3))
        {
            fprintf(stderr, "Usage: %s [-q]\n", argv[0]);
            fprintf(stderr, "  Stream from stdin and output is on stdout\n");
            fprintf(stderr, "  -q add option to pretend wrapped in quotes (for converting help files)\n");
            fprintf(stderr, "Unknown option: %s\n", argv[1]);
            exit(1);
        }

        g_nQuote = 1;
        bQuote = 1;
    }

    for (;;)
    {
        n = sizeof(buffer) - 10;
        c = buffer;
        r = getline(&c, &n, stdin);
        nLine++;

        if (r == -1)
            break;

        assert(r > 0);
        //fprintf(stderr, "r=%d, strlen=%d, [r-2]=%d, [r-1]=%d, [r]=%d\n", (int) r, (int) strlen(c), c[r-2], c[r-1], c[r]);
        assert(n < sizeof(buffer) - 9);
        assert(c[r] == 0);

        process_line(c, r, nLine);
    }

    /*
        If you can't find an unclosed quote, then go up to process line,
        and give a warning at a quote spanning e.g. 10 lines of text. 
        That will get you close to the missing quote (search up)  
    */

    if (bQuote == 0)
        if (g_nQuote)
            fprintf(stderr, "ERROR: Unclosed quote the end of of the file\n");

    if (bQuote)
        if (g_nQuote == 0)
            fprintf(stderr, "ERROR: Unclosed quote the end of of the file\n");

    if (g_nQuote)
        process_line("\"", 1, 999999);

    return (0);
}