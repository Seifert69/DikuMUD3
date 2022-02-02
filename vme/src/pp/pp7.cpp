#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wredundant-tags"

/************************************************************************/
/*									*/
/*	File:	pp7.c							*/
/*									*/
/*	Error message routines.						*/
/*									*/
/*	Written by:							*/
/*			Gary Oliver					*/
/*			3420 NW Elmwood Dr.				*/
/*			PO Box 826					*/
/*			Corvallis, Oregon 97339				*/
/*			(503)758-5549					*/
/*	Maintained by:							*/
/*			Kirk Bailey					*/
/*			Logical Systems					*/
/*			P.O. Box 1702					*/
/*			Corvallis, OR 97339				*/
/*			(503)753-9051					*/
/*									*/
/*	This program is hereby placed in the public domain.  In		*/
/*	contrast to other claims of "public domain", this means no	*/
/*	copyright is claimed and you may do anything you like with PP,	*/
/*	including selling it!  As a gesture of courtesy, please retain	*/
/*	the authorship information in the source code and		*/
/*	documentation.							*/
/*									*/
/*	Functions contained within this module:				*/
/*									*/
/*		end_of_file		Fatal for Unexpected EOF.	*/
/*		fatal			Print FATAL diagnostic.		*/
/*		illegal_symbol		Print illegal symbol diagnostic.*/
/*		non_fatal		Print normal error message.	*/
/*		out_of_memory		Fatal for out of memory.	*/
/*		prmsg			Print a error/warning message.	*/
/*		warning			Print a warning message.	*/
/*									*/
/************************************************************************/

#include "pp.h"

/************************************************************************/
/*									*/
/*	end_of_file							*/
/*									*/
/*	Print fatal "Unexpected EOF" message.				*/
/*									*/
/************************************************************************/

void end_of_file()
{
    fatal("Unexpected EOF", "");
}

/************************************************************************/
/*									*/
/*	fatal								*/
/*									*/
/*	Print error message to standard error and abort.		*/
/*									*/
/************************************************************************/

void fatal(const char *s1, const char *s2)
{
    fprintf(STDERR, "FATAL: %s%s\n", s1, s2); /* Print message */
    exit(TRUE);
}

/************************************************************************/
/*									*/
/*	illegal_symbol							*/
/*									*/
/*	Print error message with Token as illegal symbol name.		*/
/*									*/
/************************************************************************/

void illegal_symbol()
{
    //	non_fatal("Illegal symbol name: ",Token);
    warning("Illegal symbol name: ", Token);
}

/************************************************************************/
/*									*/
/*	non_fatal							*/
/*									*/
/*	Print error message to standard error and count it.		*/
/*									*/
/************************************************************************/

void non_fatal(const char *s1, const char *s2)
{
    prmsg("", s1, s2);
    Errors++; /* Count the error */
}

/************************************************************************/
/*									*/
/*	out_of_memory							*/
/*									*/
/*	Print fatal "Out of memory" message.				*/
/*									*/
/************************************************************************/

void out_of_memory()
{
    fatal("Out of memory", "");
}

/************************************************************************/
/*									*/
/*	prmsg								*/
/*									*/
/*	Print a non_fatal/warning message to standard error.		*/
/*									*/
/*	Note:	If the file stack is < 0, then all files have been	*/
/*		popped off the stack.  Use 0 (the original file name.)	*/
/*									*/
/************************************************************************/

void prmsg(const char *s1, const char *s2, const char *s3)
{
    fprintf(STDERR, "<%s> @ %u: %s%s%s\n", Filestack[Filelevel >= 0 ? Filelevel : 0]->f_name, LLine, s1, s2, s3);
}

/************************************************************************/
/*									*/
/*	warning								*/
/*									*/
/*	Print standard warning message to standard error.		*/
/*									*/
/************************************************************************/

void warning(const char *s1, const char *s2)
{
    prmsg("WARNING: ", s1, s2);
}

#pragma GCC diagnostic pop
