/*
 $Author: tperry $
 $RCSfile: spec_procs.cpp,v $
 $Date: 2003/01/02 23:39:20 $
 $Revision: 2.4 $
 */

#include "cmdload.h"
#include "structs.h"
#include "trie.h"
#include "utils.h"

#include <cstring>

/*String has format "<char name>{</><char-name>}<0> "   */
/* Example: "king welmar/tim/tom"                        */
/* Players are automatically non-excluded                */
/* Empty list means none                                 */
int charname_in_list(unit_data *ch, char *arg)
{
    char *c = nullptr;

    if (!arg || !*arg || IS_PC(ch))
    {
        return FALSE;
    }

    while ((c = strchr(arg, '/')))
    {
        *c = '\0';
        if (UNIT_NAMES(ch).IsName(arg))
        {
            *c = '/';
            return TRUE;
        }
        *c = '/';
        arg = ++c;
    }
    if (UNIT_NAMES(ch).IsName(arg))
    {
        return TRUE;
    }

    return FALSE;
}
