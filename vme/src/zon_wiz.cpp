/*
 $Author: tperry $
 $RCSfile: zon_wiz.cpp,v $
 $Date: 2003/01/02 23:39:20 $
 $Revision: 2.2 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

extern class unit_data *unit_list;

int ball(struct spec_arg *sarg)
{
#define TOP_MAX 10

    char *arg = (char *)sarg->arg;
    char buf[128];
    class unit_data *u;
    class unit_data *top[TOP_MAX];
    int v1, v2;
    int i;

    if (is_command(sarg->cmd, "use"))
    {
        u = find_unit(sarg->activator, &arg, 0, FIND_UNIT_IN_ME);

        if (u != sarg->owner)
            return SFR_SHARE;

        arg = one_argument(arg, buf);
        v1 = atoi(buf);
        arg = one_argument(arg, buf);
        v2 = atoi(buf);
        if ((v2 < 0) || (v2 > 4))
            v2 = 0;

        for (i = 0; i < TOP_MAX; top[i++] = NULL)
            ;

        snprintf(buf, sizeof(buf), "Searching for Objects of type %d with max value[%d]<br/>", v1, v2);

        send_to_char(buf, sarg->activator);

        for (u = unit_list; u; u = u->gnext)
            if (IS_OBJ(u) && (OBJ_TYPE(u) == v1))
            {
                for (i = 0; i < TOP_MAX; i++)
                    if ((top[i] == NULL) || OBJ_VALUE(u, v2) > OBJ_VALUE(top[i], v2))
                    {
                        top[i] = u;
                        break;
                    }
            }

        for (i = 0; i < TOP_MAX; i++)
            if (top[i])
            {
                snprintf(buf, sizeof(buf),
                        "%4ld %-15s@%-15s  IN  %s [%s@%s]<br/>",
                        (signed long)OBJ_VALUE(top[i], v2),
                        UNIT_FI_NAME(top[i]),
                        UNIT_FI_ZONENAME(top[i]),
                        UNIT_NAME(UNIT_IN(top[i])),
                        UNIT_FI_NAME(UNIT_IN(top[i])),
                        UNIT_FI_ZONENAME(UNIT_IN(top[i])));
                send_to_char(buf, sarg->activator);
            }
        return SFR_BLOCK;
    }
    return SFR_SHARE;
#undef TOP_MAX
}
