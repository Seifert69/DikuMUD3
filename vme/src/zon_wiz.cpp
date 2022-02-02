/*
 $Author: tperry $
 $RCSfile: zon_wiz.cpp,v $
 $Date: 2003/01/02 23:39:20 $
 $Revision: 2.2 $
 */

#include "comm.h"
#include "db.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

int ball(spec_arg *sarg)
{
#define TOP_MAX 10

    char *arg = (char *)sarg->arg;
    char buf[128];
    unit_data *u = nullptr;
    unit_data *top[TOP_MAX];
    int v1 = 0;
    int v2 = 0;
    int i = 0;

    if (is_command(sarg->cmd, "use"))
    {
        u = find_unit(sarg->activator, &arg, nullptr, FIND_UNIT_IN_ME);

        if (u != sarg->owner)
        {
            return SFR_SHARE;
        }

        arg = one_argument(arg, buf);
        v1 = atoi(buf);
        arg = one_argument(arg, buf);
        v2 = atoi(buf);
        if ((v2 < 0) || (v2 > 4))
        {
            v2 = 0;
        }

        for (i = 0; i < TOP_MAX; top[i++] = nullptr)
        {
            ;
        }

        auto msg = diku::format_to_str("Searching for Objects of type %d with max value[%d]<br/>", v1, v2);

        send_to_char(msg, sarg->activator);

        for (u = g_unit_list; u; u = u->gnext)
        {
            if (IS_OBJ(u) && (OBJ_TYPE(u) == v1))
            {
                for (i = 0; i < TOP_MAX; i++)
                {
                    if ((top[i] == nullptr) || OBJ_VALUE(u, v2) > OBJ_VALUE(top[i], v2))
                    {
                        top[i] = u;
                        break;
                    }
                }
            }
        }

        for (i = 0; i < TOP_MAX; i++)
        {
            if (top[i])
            {
                auto msg2 = diku::format_to_str("%4ld %-15s@%-15s  IN  %s [%s@%s]<br/>",
                                                (signed long)OBJ_VALUE(top[i], v2),
                                                UNIT_FI_NAME(top[i]),
                                                UNIT_FI_ZONENAME(top[i]),
                                                UNIT_NAME(UNIT_IN(top[i])),
                                                UNIT_FI_NAME(UNIT_IN(top[i])),
                                                UNIT_FI_ZONENAME(UNIT_IN(top[i])));
                send_to_char(msg2, sarg->activator);
            }
        }
        return SFR_BLOCK;
    }
    return SFR_SHARE;
#undef TOP_MAX
}
