/* 
 $Author: All $
 $RCSfile: dillex.l,v $
 $Date: 2004/01/01 22:33:30 $
 $Revision: 2.3 $
 */
%option nounput noyywrap

%{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wredundant-tags"

#include <ctype.h>
#include <string.h>
#include "essential.h"
#include "dilpar.h"
#include "dil.h"
#include "tmp_dilpar.h"
#include "diltok.h"
#include "textutil.h"
#include "db_file.h"

void dilmain(int tplate, char *zname, int lnum, char* dilp );
#define REFMAX 2048

#ifdef YYLMAX
#undef YYLMAX
#endif
#define YYLMAX 4096

/* #define yywrap() 1 */
// extern "C" int yylex(); // Trying to solve issue #70 for Debian

extern int dillex();
extern char dilcur_filename[];
extern int dilistemplate;
extern char *cur_zonename;
int dillinenum = 1;
CByteBuffer *dpBuf = &g_FileBuffer;

int dilpos;
void setline(char *str);
void dilwarning(const char *str);
void fatal(const char *str);
void syntax(const char *str);

char diltmpstr[YYLMAX * 2 + 1], *dila, *dilb;
%}

%%
\n      dillinenum++;
":="    return DILTO_ASS;
"=="    return DILTO_EQ;
"!="    return DILTO_NEQ;
"#="    return DILTO_PEQ;
"$="    return DILTO_SEQ;
"<="    return DILTO_LEQ;
">="    return DILTO_GEQ;
"<"     return DILTO_LE;
">"     return DILTO_GT;
"&"     return DILSE_AND;
"|"     return DILSE_OR;
"{"     |
"}"     |
"["     |
"]"     |
":"     |
";"     |
","     |
"("     |
")"     |
"/"     |
"*"     |
"+"     |
"%"     |
"`"     |
"."     |
"-"     return diltext[0];

\"[^\"]*\" {
    char deststr[YYLMAX];
    char buf[80];

    /* Process string */
    for (dila = diltext + 1, dilb = diltmpstr; *dila != '"'; dila++, dilb++)
    {
        if ((*dila < ' ') && !isspace(*dila))
        {
            sprintf(buf, "Illegal character ascii %d.", *dila);
            dilwarning(buf);
            *dilb = 'X';
            continue;
        }

        if ((*dilb = *dila) == '\n')
        {
            dillinenum++;
            *(++dilb) = '\r';
        }
    }
    *dilb = '\0';

    void dilfatal(const char *str, ...);
    if (strlen(diltmpstr) + 10 > sizeof(deststr))
    {
        dilfatal("not enough allocated size: %s", diltmpstr);
    }

    str_escape_format(diltmpstr, deststr, sizeof(deststr), FALSE);

    dillval.str = (char *)malloc(strlen(deststr) + 3);

    strcpy(dillval.str, deststr);

    return STRING;
}

([1-9][0-9]*)|0 {
    /* Process number */
    dillval.num = atoi(diltext);
    return PNUM;
}

0[xX][0-9a-fA-F]+ {
    /* Process hexnumber */
    sscanf(diltext + 2, "%x", &dillval.num);
    return PNUM;
}

[a-zA-Z_][a-zA-Z@0-9_]*[\t ]*[(] {
    /* not a variable */
    /* check external function */
    int i, refnum;
    void dilfatal(const char *str, ...);
    extern char **ref_names;
    //         extern char **var_names;
    extern char **ref_usednames;
    extern struct diltemplate tmpl;
    extern struct dilref refs[REFMAX];
    char nbuf[255], zbuf[255], buf[655], symbuf[255];
    extern char *cur_zonename;
    strncpy(symbuf, diltext, 254);
    symbuf[254] = 0;
    for (i = 0; i < (int)strlen(symbuf); i++) /*stop at the end of the string*/
    {
        if (symbuf[i] == ' ' || symbuf[i] == '\t' || symbuf[i] == '(')
        {
            symbuf[i] = 0;
            break;
        }
    }
    yyless(strlen(symbuf)); /* return the rest to the lexer */
    split_fi_ref(symbuf, zbuf, nbuf);
    if (!*zbuf)
    {
        strcpy(zbuf, cur_zonename);
    }
    sprintf(buf, "%s@%s", nbuf, zbuf);

    //  Can still be an internal function ie if ( or act (
    for (dilpos = 0; dilsym[dilpos].nam; dilpos++)
    {
        if (!strcmp(dilsym[dilpos].nam, symbuf))
        {
            return dilsym[dilpos].val;
        }
    }

    // Found a function lookin variable, now in buf, look it up see if it exists
    // Check if it is an existing used reference
    if ((refnum = (search_block(buf, (const char **)ref_usednames, TRUE))) != -1)
    {
        dillval.num = refnum;
        fprintf(stderr, "Returning function %s\n", symbuf);
        return FUNCTION;
    }
    else if ((i = (search_block(buf, (const char **)ref_names, TRUE))) != -1)
    {
        // It is a reference but has not been used before, lets set it up
        sprintf(buf, "%s@%s", refs[i].name, refs[i].zname);
        tmpl.xrefs[tmpl.xrefcount].name = str_dup(buf);
        tmpl.xrefs[tmpl.xrefcount].rtnt = refs[i].rtnt;
        tmpl.xrefs[tmpl.xrefcount].argc = refs[i].argc;
        tmpl.xrefs[tmpl.xrefcount].argt = refs[i].argt;

        refnum = tmpl.xrefcount++; /* this is the reference location */

        if (tmpl.xrefcount > REFMAX)
        {
            dilfatal("Too many references on %s", symbuf);
        }
        dillval.num = refnum;
        return FUNCTION;
    }
    else
    {
        strcpy(dillval.dilsym, diltext);
        return SYMBOL;
    }
}

[a-zA-Z_][a-zA-Z@0-9_]* {
    for (dilpos = 0; dilsym[dilpos].nam; dilpos++)
    {
        if (!strcmp(dilsym[dilpos].nam, diltext))
        {
            return dilsym[dilpos].val;
        }
    }

    strcpy(dillval.dilsym, diltext);
    return SYMBOL;
}

^\#.*\n {
    setline(diltext); /* cpp file/line settings */
}

[\t ]   /* ignore */
.       {
    return UNKNOWN;
}
%%

void dilmain(int tplate, char *zname, int lnum, char *dilp)
{
    // MS 2020 int result;
    extern int dillinenum;

    int dilparse(void);

    if (tplate == TRUE)
    {
        dilistemplate = TRUE;
    }
    else
    {
        dilistemplate = FALSE;
    }

    if (zname)
    {
        cur_zonename = zname;
    }
    dillinenum = lnum;

    dil_scan_string(dilp);

    dpBuf->Clear();
    // MS 2020 result = dilparse();
    dilparse();
    dil_delete_buffer(YY_CURRENT_BUFFER);
}

#pragma GCC diagnostic pop
