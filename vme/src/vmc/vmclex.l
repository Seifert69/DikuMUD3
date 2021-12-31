/* 
 $Author: All $
 $RCSfile: vmclex.l,v $
 $Date: 2004/01/01 22:33:30 $
 $Revision: 2.5 $
 */
 
%option nounput 
%{
#include <stdio.h>
#include <ctype.h>
#include "vmc.h"
#include "tmp_vmcpar.h"
#include "vmctok.h"
#include "textutil.h"
#include "db_file.h"

extern char* sOutput;
extern int sOutput_len;
extern int sOutput_mlen;
void dilmain(int tplate, char *zname, int lnum, char* dilp );
extern int yylex(); // Testing fix for issue #70


#ifdef YYLMAX
#undef YYLMAX
#endif
#define YYLMAX 65536  /* For looong strings... */
/*
#ifdef yywrap
#undef yywrap
#endif

  */
#define YY_SKIP_YYWRAP  
#define yywrap(void) 1
extern char cur_filename[], top_filename[];
extern struct zone_info zone;
extern int istemplate;

int first_dil = TRUE;
int linenum = 1;
int startlinenum = 0;
char buf[256];
char *dbuf,*bptr;
int dlen=0,curlen=0;
CByteBuffer *pBuf = &g_FileBuffer;

int pos, i;
void setline(char *str);
void warning(const char *str);
void fatal(const char *str);
void dmc_exit(int n);

char tmpstr[YYLMAX + 1], *a, *b;
%}

%%
\n			{
			  linenum++;
			}
"{"			|
"}"			|
"["			|
"]"			|
";"			|
","			|
"@"			|
"("         |
")"         |
"/"         |
"*"         |
"+"         |
"`"         |
"-"			{
			  return yytext[0];
			}
\"[^\"]*\"	{
				char deststr[YYLMAX];
				
				/* Process string */
				for (a = yytext + 1, b = tmpstr; *a != '"';a++, b++)
				{
					if (a - yytext >= (int)sizeof(tmpstr))
			      	{
				 		fprintf(stderr, "Fixed string too long, aborting.\n");
				 		dmc_exit(1);
			      	}

			      	/* if ((*a < ' ') && !isspace(*a))
			      	{
				 		sprintf(buf, "Illegal character ascii %d.", *a);
				 		*b = 'X';
				 		warning(buf);
				 		continue;
			      	} Allow UTF-8 */ 

			      	if ((*b = *a) == '\n')
						linenum++;
			   	}
			   	*b = '\0';

			   	str_escape_format(tmpstr, deststr, sizeof(deststr));

			   /* + 6 because of concatenated "\n\r" later on
			      and perhaps 3 leading spaces. */

			   yylval.str = (char *) mmalloc(strlen(deststr) + 6);

			   strcpy(yylval.str, deststr);

			   return STRING;
			}
([1-9][0-9]*)|0		{
			  /* Process number */
			  yylval.num = atoi(yytext);
			  return PNUM;
			}
0[xX][0-9a-fA-F]+	{
			  /* Process hexnumber */
			  sscanf(yytext + 2, "%x", &yylval.num);
			  return PNUM;
			}
dilbegin[^\^]*\^        {
		          CREATE(dbuf, char, 2048);
                          curlen=2048;
                          bptr=dbuf;
                          dlen=0;
			  startlinenum = linenum;
			  /* Process string */
			  for (a = yytext; *a != '^'; a++)
			  {
                            dlen++;
                            if (curlen < (dlen+10))
			    {
				RECREATE(bptr, char, curlen*2);
 				curlen=curlen*2;
                                dbuf=bptr+dlen-1;
			    }
                            *dbuf=*a;
                            dbuf++;
			    *dbuf='\0';
			    if (*a == '\n')
			      linenum++;
			  }
			  /* During %dil section, tell compiler only to
                             generate templates */

			  first_dil = FALSE;
			  pBuf->Clear();
			  dilmain(istemplate, zone.z_zone.name,startlinenum,bptr) ;
			  FREE(bptr);
			  if (pBuf->GetLength()==0)
			  {

			    fatal("DIL not compiled properly.");
                            dmc_exit(1);
			  }

//			  yylval.str = (char *) mmalloc(pBuf->GetLength() + 1);
//			  strcpy(yylval.str, (char *) pBuf->GetData());
			  return DILPRG;
			}
null  { return NULLRET; }

[a-zA-Z_][a-zA-Z0-9_]*	{
			  for (pos = 0; sym[pos].nam; pos++)
			    if (!strcmp(sym[pos].nam, yytext))
			      return sym[pos].val;
			  strcpy(yylval.sym, yytext);
			  return SYMBOL;
			}
\%dil	  		{
			  return DIL;
	       		}
\%end                   {
                          return ENDMARK;
                        }
\%zone			{
			  return ZONE;
			}
\%reset			{
			  return RESET;
			}
\%mobiles		{
			  return MOBILES;
			}
\%rooms			{
			  return ROOMS;
			}
\%objects		{
			  return OBJECTS;
			}
^\#.*\n			{
			  setline(yytext); /* cpp file/line settings */
			}
[\t ]			/* ignore */
.			{
			  return UNKNOWN;
			}
%%
void setline(char *str)
{
  char *p;

  while (!isdigit(*str) && *str)
    str++;

  if (!isdigit(*str))
    return;

  linenum = atoi(str);

  while (*str != '"' && *str)
    str++;

  if (!*str)
    return;

  str++;
  p = cur_filename;

  while (*str != '"' && *str)
    (*(p++) = *(str++));

  *p = '\0';

  if(!*cur_filename)
    strcpy(cur_filename, top_filename);
}

void init_lex(char *str)
{
   static int count = 0;

  if (count++ > 0)
    yy_delete_buffer( YY_CURRENT_BUFFER );

   yy_scan_string(str);
}
