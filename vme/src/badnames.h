/*
 $Author: All $
 $RCSfile: badnames.h,v $
 $Date: 2004/01/09 20:34:23 $
 $Revision: 2.2 $
 */

#ifndef _MUD_BADNAMES_H_
#define	_MUD_BADNAMES_H_
#ifdef _WINDOWS
#include <string.h>
#else
#include <strings.h>
#endif
class badnames_list
{
private:
    char *name;
    badnames_list *next;

public:

    badnames_list (void);
    badnames_list (char *n);
    ~badnames_list (void);

    char *insert (char *n);
    char *in (char *n);
    char *equal (char *n);
    void create (char *input_str);
};
#endif
