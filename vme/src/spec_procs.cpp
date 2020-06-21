/*
 $Author: tperry $
 $RCSfile: spec_procs.cpp,v $
 $Date: 2003/01/02 23:39:20 $
 $Revision: 2.4 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "vmelimits.h"
#include "justice.h"
#include "affect.h"
#include "magic.h"
#include "utility.h"
#include "money.h"
#include "str_parse.h"
#include "files.h"
#include "trie.h"
#include "spec_assign.h"
#include "fight.h"
#include "cmdload.h"

/*String has format "<char name>{</><char-name>}<0> "   */
/* Example: "king welmar/tim/tom"                        */
/* Players are automatically non-excluded                */
/* Empty list means none                                 */
int charname_in_list(class unit_data *ch, char *arg)
{
    char *c;

    if (!arg || !*arg || IS_PC(ch))
        return FALSE;

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
        return TRUE;

    return FALSE;
}
