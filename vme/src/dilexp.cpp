
/*
$Author: All $
$RCSfile: dilexp.cpp,v $
$Date: 2005/06/28 20:17:48 $
$Revision: 2.18 $
*/

#ifdef _WINDOWS
#include <direct.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h> /* For type_check */
#include <sys/stat.h>
#include <errno.h>

/* Per https://sourceforge.net/p/predef/wiki/OperatingSystems/, this identifies
 *  Mac OS X. This is needed since OS X doesn't have crypt.h and instead uses
 *  unistd.h for these mappings. */
#if defined __APPLE__ && __MACH__
#include <unistd.h>
#elif defined LINUX
#include <crypt.h>
#endif

#include "structs.h"
#include "handler.h"
#include "textutil.h"
#include "interpreter.h"
#include "comm.h"
#include "dil.h"
#include "affect.h"
#include "utility.h"
#include "utils.h"
#include "db.h"
#include "vmelimits.h"
#include "common.h"
#include "spells.h"
#include "db_file.h"
#include "dilexp.h"
#include "dilrun.h"
#include "money.h"
#include "magic.h"
#include "fight.h"
#include "skills.h"
#include "color.h"
#include "files.h"
#include "trie.h"
#include "intlist.h"
#include "combat.h"
#include "movement.h"
#include "justice.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>

#include <iomanip>
namespace fs = boost::filesystem;

#include <boost/regex.hpp>
#include <string>

using namespace boost;

struct time_info_data mud_date();
extern struct trie_type *intr_trie;
/* ************************************************************************ */
/* DIL-expressions							    */
/* ************************************************************************ */

void dilfe_illegal(register class dilprg *p)
{
    szonelog(UNIT_FI_ZONE(p->sarg->owner),
             "DIL %s@%s, Illegal Expression/Instruction Node.\n",
             UNIT_FI_NAME(p->sarg->owner), UNIT_FI_ZONENAME(p->sarg->owner));
    p->waitcmd = WAITCMD_QUIT;
}

/* replace all old with new in  string 3
   replace(old, new, string 3)*/
void dilfe_replace(register class dilprg *p)
{
    dilval *v = new dilval;
    //char *buf;
    //int olen, nlen, buflen, i;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
            break;
        case DILV_SP:

            switch (dil_getval(v3))
            {
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            case DILV_SP:
                if (v->type == DILV_SP)
                {
                    string sch((char *) v1->val.ptr);
                    string rpl((char *) v2->val.ptr);
                    string mystr((char *) v3->val.ptr);

                    str_substitute(sch, rpl, mystr);

                    v->atyp = DILA_EXP;
                    v->val.ptr = str_dup(mystr.c_str());
                    /*
                    MS2020 This could crash if replacing multiple instances of string (buffer OOB)
                    olen = strlen((char *)v1->val.ptr);
                    nlen = strlen((char *)v2->val.ptr);
                    buflen = strlen((char *)v3->val.ptr);

                    i = (nlen - olen);
                    if (i < 0)
                        buflen = buflen + olen + 10;
                    else
                        buflen =
                            buflen + (((nlen - olen) / (olen)) * olen) + olen + 20;
                    CREATE(buf, char, buflen);
                    strcpy(buf, (char *)v3->val.ptr);
                    str_substitute((char *)v1->val.ptr, (char *)v2->val.ptr, buf);

                    FREE(buf);*/
                }
                else
                    v->type = DILV_FAIL;

                break;
            }

            break;
        }

        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_tolower(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char *s1;

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    case DILV_SP:
        if (str_is_empty((char *)v1->val.ptr))
            v->val.ptr = NULL;
        else
        {
            v->atyp = DILA_EXP;
            s1 = str_dup((char *)v1->val.ptr);
            str_lower(s1);
            v->val.ptr = s1;
        }
        break;
    }

    p->stack.push(v);
    delete v1;
}

void dilfe_toupper(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char *s1;

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    case DILV_SP:

        if (str_is_empty((char *)v1->val.ptr))
            v->val.ptr = NULL;
        else
        {
            v->atyp = DILA_EXP;
            s1 = str_dup((char *)v1->val.ptr);
            str_upper(s1);
            v->val.ptr = s1;
        }

        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_left(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int strl, x;

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    case DILV_SP:

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
            break;
        case DILV_INT:

            if (str_is_empty((char *)v1->val.ptr))
            {
                v->val.ptr = NULL;
                break;
            }
            if ((int)strlen((char *)v1->val.ptr) < v2->val.num)
                strl = strlen((char *)v1->val.ptr);
            else
                strl = v2->val.num;

            if (str_is_empty((char *)v1->val.ptr) || strl == 0)
                v->val.ptr = NULL;
            else
            {
                v->atyp = DILA_EXP;
                v->val.ptr = (char *)malloc(strl + 1);
                for (x = 0; x < strl; x++)
                    ((char *)v->val.ptr)[x] = ((char *)v1->val.ptr)[x];
                ((char *)v->val.ptr)[strl] = 0;
            }

            break;
        }
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_right(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int strl, x, y;

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
            break;
        case DILV_INT:

            if (str_is_empty((char *)v1->val.ptr))
            {
                v->val.ptr = NULL;
                break;
            }

            if ((int)strlen((char *)v1->val.ptr) < v2->val.num)
                strl = strlen((char *)v1->val.ptr);
            else
                strl = v2->val.num;

            if (str_is_empty((char *)v1->val.ptr) || strl == 0)
                v->val.ptr = NULL;
            else
            {
                v->atyp = DILA_EXP;
                v->val.ptr = malloc(strl + 1);
                for (x = 0, y = strlen((char *)v1->val.ptr) - strl; x < strl;
                     y++, x++)
                    ((char *)v->val.ptr)[x] = ((char *)v1->val.ptr)[y];
                ((char *)v->val.ptr)[strl] = 0;
            }

            break;
        }
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_mid(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int strl, l, r, x;

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
            break;
        case DILV_INT:

            switch (dil_getval(v3))
            {
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            case DILV_INT:
                if ((v2->val.num >= 0) && ((int)strlen((char *)v1->val.ptr) < v2->val.num))
                {
                    v1->val.ptr = NULL;
                    break;
                }
                else
                    l = v2->val.num;

                if ((v2->val.num >= 0) && ((int)strlen((char *)v1->val.ptr) < v3->val.num))
                    r = strlen((char *)v1->val.ptr);
                else
                    r = v3->val.num;

                if (r < l)
                {
                    szonelog(UNIT_FI_ZONE(p->sarg->owner),
                             "DIL %s@%s, Illegal: mid(1,2,3) 3 > 2 \n",
                             UNIT_FI_NAME(p->sarg->owner),
                             UNIT_FI_ZONENAME(p->sarg->owner));
                    v->type = DILV_FAIL;
                    break;
                }

                strl = (r - l) + 1;

                if (str_is_empty((char *)v1->val.ptr) || strl == 0)
                    v->val.ptr = NULL;
                else
                {
                    v->atyp = DILA_EXP;
                    v->val.ptr = malloc(strl + 1);
                    for (x = 0; l <= r; l++, x++)
                        ((char *)v->val.ptr)[x] = ((char *)v1->val.ptr)[l - 1];
                    ((char *)v->val.ptr)[strl] = 0;
                }

                break;
            }
            break;
        }
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

/* islight */
void dilfe_islt(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    case DILV_UP:
        if (!v1->val.ptr)
            v->type = DILV_FAIL;
        else
        {
            if (UNIT_IS_LIGHT((class unit_data *)v1->val.ptr))
                v->val.num = 1;
            else
                v->val.num = 0;
        }
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
}

void dilfe_ghead(class dilprg *p)
{
    dilval *v = new dilval;
    v->atyp = DILA_NORM;
    v->type = DILV_UP;
    v->val.ptr = unit_list;
    p->stack.push(v);
}

void dilfe_phead(class dilprg *p)
{
    dilval *v = new dilval;
    if (IS_PC(unit_list))
    {
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        v->val.ptr = unit_list;
    }
    else
        v->type = DILV_NULL;
    p->stack.push(v);
}

void dilfe_ohead(class dilprg *p)
{
    dilval *v = new dilval;
    if (IS_OBJ(obj_head))
    {
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        v->val.ptr = obj_head;
    }
    else
        v->type = DILV_NULL;
    p->stack.push(v);
}

void dilfe_nhead(class dilprg *p)
{
    dilval *v = new dilval;
    if (IS_NPC(npc_head))
    {
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        v->val.ptr = npc_head;
    }
    else
        v->type = DILV_NULL;
    p->stack.push(v);
}

void dilfe_rhead(class dilprg *p)
{
    dilval *v = new dilval;
    if (IS_ROOM(room_head))
    {
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        v->val.ptr = room_head;
    }
    else
        v->type = DILV_NULL;
    p->stack.push(v);
}

void dilfe_zhead(class dilprg *p)
{
    dilval *v = new dilval;
    v->atyp = DILA_NORM;
    v->type = DILV_ZP;
    v->val.ptr = zone_info.zone_list;
    p->stack.push(v);
}

void dilfe_chead(class dilprg *p)
{
    dilval *v = new dilval;
    v->atyp = DILA_NORM;
    v->type = DILV_CP;
    v->val.ptr = cmdlist;
    p->stack.push(v);
}

void dilfe_clr(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        v->atyp = DILA_EXP;
        v->val.ptr = str_dup(divcolor((char *)v1->val.ptr));
        break;
    case DILV_FAIL:
    case DILV_UP:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_sendpre(class dilprg *p)
{
    dilval *v = new dilval;
    struct command_info *cmd;
    dilval *v7 = p->stack.pop();
    dilval *v6 = p->stack.pop();
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_UP:
        case DILV_NULL:
            switch (dil_getval(v3))
            {
            case DILV_UP:
            case DILV_NULL:
                switch (dil_getval(v4))
                {
                case DILV_UP:
                case DILV_NULL:
                    switch (dil_getval(v5))
                    {
                    case DILV_INT:
                        switch (dil_getval(v6))
                        {
                        case DILV_SP:
                            switch (dil_getval(v7))
                            {
                            case DILV_UP:
                            case DILV_NULL:
                                cmd = ((struct command_info *)
                                         search_trie((char *)v1->val.ptr,
                                                     intr_trie));
                                if (!cmd)
                                {
                                    szonelog(UNIT_FI_ZONE(p->sarg->owner),
                                             "DIL %s@%s, : Can not send_preprocess. No such command %s\n",
                                             UNIT_FI_NAME(p->sarg->owner),
                                             UNIT_FI_ZONENAME(p->sarg->owner),
                                             (char *)v1->val.ptr);
                                    v->type = DILV_FAIL;
                                }
                                else
                                {
                                    v->val.num =
                                        send_ack((class unit_data *)v2->val.ptr,
                                                (class unit_data *)v3->val.ptr,
                                                (class unit_data *)v4->val.ptr,
                                                &(v5->val.num),
                                                cmd,
                                                (const char *)v6->val.ptr,
                                                (class unit_data *)v7->val.ptr);
                                }
                                break;
                            default:
                                v->type = DILV_FAIL;
                                break;
                            }

                            break;
                        default:
                            v->type = DILV_FAIL;
                            break;
                        }
                        break;
                    default:
                        v->type = DILV_FAIL;
                        break;
                    }

                    break;
                default:
                    v->type = DILV_FAIL;
                    break;
                }

                break;
            default:
                v->type = DILV_FAIL;
                break;
            }
            break;
        default:
            v->type = DILV_FAIL;
            break;
        }

        break;
    default:
        v->type = DILV_FAIL;
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
    delete v7;
}

void dilfe_clradd(class dilprg *p)
{
    dilval *v = new dilval;

    char full_name[21];
    char *color;
    unsigned int x;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
    case DILV_UP:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
        case DILV_SP:
            switch (dil_getval(v3))
            {
            default:
                v->type = DILV_ERR;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            case DILV_SP:

                if (!IS_PC((unit_data *)v1->val.ptr))
                {
                    szonelog(UNIT_FI_ZONE(p->sarg->owner),
                             "DIL %s@%s, Illegal: Tried to add a color to a non pc.\n",
                             UNIT_FI_NAME(p->sarg->owner),
                             UNIT_FI_ZONENAME(p->sarg->owner));
                    v->type = DILV_FAIL;
                }

                if (strlen((char *)v2->val.ptr) > 20)
                {
                    szonelog(UNIT_FI_ZONE(p->sarg->owner),
                             "DIL %s@%s, Illegal: Color key is greater than 20 characters.\n",
                             UNIT_FI_NAME(p->sarg->owner),
                             UNIT_FI_ZONENAME(p->sarg->owner));
                    v->type = DILV_FAIL;
                }

                x = 0;
                while (x < strlen((char *)v2->val.ptr))
                {
                    if ((!isalnum(*((char *)v2->val.ptr + x))) &&
                        (*((char *)v2->val.ptr + x) != '_'))
                    {
                        szonelog(UNIT_FI_ZONE(p->sarg->owner),
                                 "DIL %s@%s, Illegal: Color key contains invalid characters.\n",
                                 UNIT_FI_NAME(p->sarg->owner),
                                 UNIT_FI_ZONENAME(p->sarg->owner));
                        v->type = DILV_FAIL;
                        break;
                    }
                    x++;
                }

                if (v->type != DILV_FAIL)
                {
                    if (UPC((unit_data *)v1->val.ptr)->color.get((char *)v2->val.ptr, full_name))
                    {
                        v->type = DILV_INT;
                        v->val.num = FALSE;
                    }
                    else
                    {
                        color = str_escape_format((char *)v3->val.ptr, FALSE);
                        UPC((unit_data *)v1->val.ptr)->color.insert((char *)v2->val.ptr, color);
                        FREE(color);
                        v->type = DILV_INT;
                        v->val.num = TRUE;
                    }
                }
                break;
            }

            break;
        }

        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_clrchg(class dilprg *p)
{
    dilval *v = new dilval;

    char full_name[21];
    char *color;
    unsigned int x;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
    case DILV_UP:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
        case DILV_SP:
            switch (dil_getval(v3))
            {
            default:
                v->type = DILV_ERR;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            case DILV_SP:

                if (!IS_PC((unit_data *)v1->val.ptr))
                {
                    szonelog(UNIT_FI_ZONE(p->sarg->owner),
                             "DIL %s@%s, Illegal: Tried to change a color to a non pc.\n",
                             UNIT_FI_NAME(p->sarg->owner),
                             UNIT_FI_ZONENAME(p->sarg->owner));
                    v->type = DILV_FAIL;
                }

                if (strlen((char *)v2->val.ptr) > 20)
                {
                    szonelog(UNIT_FI_ZONE(p->sarg->owner),
                             "DIL %s@%s, Illegal: Color key is greater than 20 characters.\n",
                             UNIT_FI_NAME(p->sarg->owner),
                             UNIT_FI_ZONENAME(p->sarg->owner));
                    v->type = DILV_FAIL;
                }

                x = 0;
                while (x < strlen((char *)v2->val.ptr))
                {
                    if ((!isalnum(*((char *)v2->val.ptr + x))) &&
                        (*((char *)v2->val.ptr + x) != '_'))
                    {
                        szonelog(UNIT_FI_ZONE(p->sarg->owner),
                                 "DIL %s@%s, Illegal: Color key contains invalid characters.\n",
                                 UNIT_FI_NAME(p->sarg->owner),
                                 UNIT_FI_ZONENAME(p->sarg->owner));
                        v->type = DILV_FAIL;
                        break;
                    }
                    x++;
                }

                if (v->type != DILV_FAIL)
                {
                    if (UPC((unit_data *)v1->val.ptr)->color.get((char *)v2->val.ptr, full_name))
                    {
                        color = str_escape_format((char *)v3->val.ptr, FALSE);
                        UPC((unit_data *)v1->val.ptr)->color.change((char *)v2->val.ptr, color);
                        FREE(color);
                        v->type = DILV_INT;
                        v->val.num = TRUE;
                    }
                    else
                    {
                        v->type = DILV_INT;
                        v->val.num = FALSE;
                    }
                }
                break;
            }

            break;
        }

        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_clrdel(class dilprg *p)
{
    dilval *v = new dilval;

    char full_name[21];
    unsigned int x;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    default:
        v->type = DILV_ERR;
        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    case DILV_UP:
        switch (dil_getval(v2))
        {
        default:
            v->type = DILV_ERR;
            break;
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        case DILV_SP:
            if (!IS_PC((unit_data *)v1->val.ptr))
            {
                szonelog(UNIT_FI_ZONE(p->sarg->owner),
                         "DIL %s@%s, Illegal: Tried to delete a color to a non pc.\n",
                         UNIT_FI_NAME(p->sarg->owner),
                         UNIT_FI_ZONENAME(p->sarg->owner));
                v->type = DILV_FAIL;
            }

            if (strlen((char *)v2->val.ptr) > 20)
            {
                szonelog(UNIT_FI_ZONE(p->sarg->owner),
                         "DIL %s@%s, Illegal: Color key is greater than 20 characters.\n",
                         UNIT_FI_NAME(p->sarg->owner),
                         UNIT_FI_ZONENAME(p->sarg->owner));
                v->type = DILV_FAIL;
            }

            x = 0;
            while (x < strlen((char *)v2->val.ptr))
            {
                if ((!isalnum(*((char *)v2->val.ptr + x))) &&
                    (*((char *)v2->val.ptr + x) != '_'))
                {
                    szonelog(UNIT_FI_ZONE(p->sarg->owner),
                             "DIL %s@%s, Illegal: Color key contains invalid characters.\n",
                             UNIT_FI_NAME(p->sarg->owner),
                             UNIT_FI_ZONENAME(p->sarg->owner));
                    v->type = DILV_FAIL;
                    break;
                }
                x++;
            }

            if (v->type != DILV_FAIL)
            {
                if (UPC((unit_data *)v1->val.ptr)->color.get((char *)v2->val.ptr, full_name))
                {
                    UPC((unit_data *)v1->val.ptr)->color.remove(full_name);
                    v->type = DILV_INT;
                    v->val.num = TRUE;
                }
                else
                {
                    v->type = DILV_INT;
                    v->val.num = FALSE;
                }
            }

            break;
        }

        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_ckpwd(class dilprg *p)
{
    dilval *v = new dilval;

    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_UP:
        switch (dil_getval(v2))
        {
        case DILV_SP:
            if (!IS_PC((unit_data *)v1->val.ptr))
            {
                szonelog(UNIT_FI_ZONE(p->sarg->owner),
                         "DIL %s@%s, Illegal: Unit must be a pc to check password.\n",
                         UNIT_FI_NAME(p->sarg->owner),
                         UNIT_FI_ZONENAME(p->sarg->owner));
                v->type = DILV_FAIL;
                break;
            }

            v->type = DILV_INT;
            if (strncmp(crypt((char *)v2->val.ptr,
                              PC_FILENAME((class unit_data *)v1->val.ptr)),
                        PC_PWD((class unit_data *)v1->val.ptr), 10))
                v->val.num = FALSE;
            else
                v->val.num = TRUE;

            break;
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
        }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_atsp(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v6 = p->stack.pop();
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_UP:
            if (!v2->val.ptr || !IS_CHAR((class unit_data *)v2->val.ptr))
                v->type = DILV_FAIL;
            else
                switch (dil_getval(v3))
                {
                case DILV_UP:
                    if (!v3->val.ptr)
                        v->type = DILV_FAIL;
                    else
                        switch (dil_getval(v4))
                        {
                        case DILV_UP:
                            if (!v4->val.ptr)
                                v->type = DILV_FAIL;
                            else
                                switch (dil_getval(v5))
                                {
                                case DILV_INT:

                                    switch (dil_getval(v6))
                                    {
                                    case DILV_SP:
                                        if (is_in(v1->val.num, SPL_GROUP_MAX,
                                                  SPL_TREE_MAX - 1) &&
                                            (spell_info[v1->val.num].spell_pointer || spell_info[v1->val.num].tmpl))
                                        {
                                            struct spell_args sa;

                                            set_spellargs(&sa,
                                                          (class unit_data *)v2->val.ptr,
                                                          (class unit_data *)v3->val.ptr,
                                                          (class unit_data *)v4->val.ptr, NULL, 0);
                                            if (str_is_empty((char *)v6->val.ptr) || ((char *)v6->val.ptr == NULL))
                                                sa.pEffect = NULL;
                                            else
                                                sa.pEffect = str_dup((char *)v6->val.ptr); 

                                            /* cast the spell */
                                            v->val.num =
                                                spell_offensive(&sa, v1->val.num,
                                                                v5->val.num);

                                            if (sa.pEffect)
                                                FREE(sa.pEffect);
                                            dil_test_secure(p);
                                        }
                                        break;
                                    case DILV_FAIL:
                                        v->type = DILV_FAIL;
                                        break;
                                    case DILV_NULL:
                                    default:
                                        v->type = DILV_ERR;
                                        break;
                                    }

                                    break;
                                case DILV_FAIL:
                                case DILV_NULL:
                                    v->type = DILV_FAIL;
                                    break;
                                default:
                                    v->type = DILV_ERR;
                                    break;
                                }
                            break;
                        case DILV_FAIL:
                        case DILV_NULL:
                            v->type = DILV_FAIL;
                            break;
                        default:
                            v->type = DILV_ERR;
                            break;
                        }
                    break;
                case DILV_FAIL:
                case DILV_NULL:
                    v->type = DILV_FAIL;
                    break;
                default:
                    v->type = DILV_ERR;
                    break;
                }
            break;
        default:
            v->type = DILV_ERR;
            break;
        }
        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
}

void dilfe_cast2(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_UP:
            if (!v2->val.ptr || !IS_CHAR((class unit_data *)v2->val.ptr))
                v->type = DILV_FAIL;
            else
                switch (dil_getval(v3))
                {
                case DILV_UP:
                    if (!v3->val.ptr)
                        v->type = DILV_FAIL;
                    else
                        switch (dil_getval(v4))
                        {
                        case DILV_UP:
                            if (!v4->val.ptr)
                                v->type = DILV_FAIL;
                            else
                                switch (dil_getval(v5))
                                {
                                case DILV_SP:
                                    if (is_in(v1->val.num, SPL_GROUP_MAX, SPL_TREE_MAX - 1) && (spell_info[v1->val.num].spell_pointer || spell_info[v1->val.num].tmpl))
                                    {
                                        /* cast the spell */
                                        v->val.num =
                                            spell_perform(v1->val.num, MEDIA_SPELL,
                                                          (class unit_data *)v2->val.ptr,
                                                          (class unit_data *)v3->val.ptr,
                                                          (class unit_data *)v4->val.ptr, "",
                                                          (char *)v5->val.ptr);
                                        dil_test_secure(p);
                                    }
                                    break;
                                case DILV_FAIL:
                                case DILV_NULL:
                                    v->type = DILV_FAIL;
                                    break;
                                default:
                                    v->type = DILV_ERR;
                                    break;
                                }
                            break;
                        case DILV_FAIL:
                        case DILV_NULL:
                            v->type = DILV_FAIL;
                            break;
                        default:
                            v->type = DILV_ERR;
                            break;
                        }

                    break;
                case DILV_FAIL:
                case DILV_NULL:
                    v->type = DILV_FAIL;
                    break;
                default:
                    v->type = DILV_ERR;
                    break;
                }
            break;
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
            break;
        }
        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
}

void dilfe_resta(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    class unit_data *restore_all_unit(char *filename, unit_data *udest);
    char filename[512];

    v->type = DILV_UP;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_UP:
        case DILV_NULL:
            if (p->frame[0].tmpl->zone->access > 10)
            {
                szonelog(p->frame->tmpl->zone,
                         "DIL '%s' attempt to violate system access security (restorall).",
                         p->frame->tmpl->prgname);
                p->waitcmd = WAITCMD_QUIT;
                break;
            }

            if (!store_name_test((char *)v1->val.ptr))
            {
                szonelog(p->frame->tmpl->zone,
                         "DIL '%s' attempted to restore an illegal file name.",
                         p->frame->tmpl->prgname);
                v->val.ptr = NULL;
                break;
            }

            if (str_is_empty(p->frame[0].tmpl->zone->dilfilepath))
                strcpy(filename, g_cServerConfig.m_dilfiledir);
            else
                strcpy(filename, p->frame[0].tmpl->zone->dilfilepath);
            strcat(filename, "/units/");
            strcat(filename, (char *)v1->val.ptr);
            if (v2->val.ptr)
                v->val.ptr = restore_all_unit(filename,
                                              (class unit_data *)v2->val.ptr);
            else
                v->val.ptr = restore_all_unit(filename, p->owner);
            if (v->val.ptr == NULL)
                v->type = DILV_NULL;
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
        default:
            v->type = DILV_ERR;
            break;
        }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
    default:
        v->type = DILV_ERR;
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_opro(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_INT:
            if ((v1->val.num > 0) && (v2->val.num > 0) &&
                (v2->val.num < v1->val.num / 2 - 1))
                v->val.num = open_ended_roll(v1->val.num, v2->val.num);
            else
                v->val.num = 0;
            break;
        default:
            v->type = DILV_ERR;
            break;
        }

        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_eqpm(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_UP;

    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_INT:
                v->val.ptr =
                    equipment((class unit_data *)v1->val.ptr, v2->val.num);
                if (v->val.ptr == NULL)
                    v->type = DILV_NULL;
                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

/* int meleeAttack(unit, unit, int, int) */
void dilfe_mel(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_UP:
                if (!v2->val.ptr || !IS_CHAR((class unit_data *)v2->val.ptr))
                    v->type = DILV_FAIL;
                else
                    switch (dil_getval(v3))
                    {
                    case DILV_INT:
                        switch (dil_getval(v4))
                        {
                        case DILV_INT:
                            v->val.num = one_hit((class unit_data *)v1->val.ptr,
                                                 (class unit_data *)v2->val.ptr,
                                                 v3->val.num, v4->val.num, TRUE,
                                                 TRUE);
                            dil_test_secure(p);

                            break;
                        default:
                            v->type = DILV_ERR;
                            break;
                        }

                        break;
                    default:
                        v->type = DILV_ERR;
                        break;
                    }

                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
}

/* int meleedamage(unit, unit, int, int) */
void dilfe_meldam(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_UP:
                if (!v2->val.ptr || !IS_CHAR((class unit_data *)v2->val.ptr))
                    v->type = DILV_FAIL;
                else
                    switch (dil_getval(v3))
                    {
                    case DILV_INT:
                        switch (dil_getval(v4))
                        {
                        case DILV_INT:
                            v->val.num = one_hit((class unit_data *)v1->val.ptr,
                                                 (class unit_data *)v2->val.ptr,
                                                 v3->val.num, v4->val.num, TRUE,
                                                 FALSE);
                            dil_test_secure(p);
                            break;
                        case DILV_FAIL:
                        case DILV_NULL:
                            v->type = DILV_FAIL;
                            break;
                        default:
                            v->type = DILV_ERR;
                            break;
                        }

                        break;
                    case DILV_FAIL:
                    case DILV_NULL:
                        v->type = DILV_FAIL;
                        break;
                    default:
                        v->type = DILV_ERR;
                        break;
                    }

                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
}

void dilfe_flog(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *sstr;
    char filename[255];

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_SP:
            switch (dil_getval(v3))
            {
            case DILV_SP:
                if (p->frame[0].tmpl->zone->access > 1)
                {
                    szonelog(p->frame->tmpl->zone,
                             "DIL '%s' attempt to access logs w/o access.",
                             p->frame->tmpl->prgname);
                    p->waitcmd = WAITCMD_QUIT;
                }
                else
                {
                    strcpy(filename, g_cServerConfig.m_logdir);
                    strcat(filename, (char *)v1->val.ptr);
                    sstr = str_dup((char *)v2->val.ptr);
                    v->val.num = save_string(filename, &sstr, (char *)v3->val.ptr);
                    FREE(sstr);
                }
                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

            break;
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
            break;
        }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_ldstr(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *sstr;
    char filename[512];

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_SP:
            if (p->frame[0].tmpl->zone->access > 10)
            {
                szonelog(p->frame->tmpl->zone,
                         "DIL '%s' attempt to violate system access security (loadstr).",
                         p->frame->tmpl->prgname);
                p->waitcmd = WAITCMD_QUIT;
            }
            else if (!store_name_test((char *)v1->val.ptr))
            {
                szonelog(p->frame->tmpl->zone,
                         "DIL '%s' attempted to loadstr n illegal file name.",
                         p->frame->tmpl->prgname);
                v->val.num = FALSE;
                ;
            }
            else
            {
                sstr = NULL;
                if (p->frame[0].tmpl->zone->dilfilepath)
                    strcpy(filename, p->frame[0].tmpl->zone->dilfilepath);
                else
                    strcpy(filename, g_cServerConfig.m_dilfiledir);
                strcat(filename, "/strings/");
                strcat(filename, (char *)v1->val.ptr);
                v->val.num = load_string(filename, &sstr);
                if (!str_is_empty(sstr))
                {
                    if (*((char **)v2->ref))
                        FREE(*((char **)v2->ref));
                    *((char **)v2->ref) = str_dup(sstr);
                    FREE(sstr);
                }
                else
                    v2->val.ptr = NULL;
            }

            break;
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
            break;
        }
        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_delstr(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    int ret;
    char filename[512];

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_SP:

        if (p->frame[0].tmpl->zone->access > 10)
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempt to violate system access security (delstr).",
                     p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else if (!store_name_test((char *)v1->val.ptr))
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempted to delstr an illegal file name.",
                     p->frame->tmpl->prgname);
            v->val.num = FALSE;
        }
        else
        {
            if (str_is_empty(p->frame[0].tmpl->zone->dilfilepath))
                strcpy(filename, g_cServerConfig.m_dilfiledir);
            else
                strcpy(filename, p->frame[0].tmpl->zone->dilfilepath);
            strcat(filename, "/strings/");
            strcat(filename, (char *)v1->val.ptr);
            ret = remove(filename);
            if (ret)
                v->val.num = TRUE;
            else
                v->val.num = FALSE;
        }
        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
}

void dilfe_delunit(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    int ret;
    char filename[512];

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        if (p->frame[0].tmpl->zone->access > 10)
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempt to violate system access security (delunit).",
                     p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else if (!store_name_test((char *)v1->val.ptr))
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempted to delunit an illegal file name.",
                     p->frame->tmpl->prgname);
            v->val.num = FALSE;
        }
        else
        {
            if (str_is_empty(p->frame[0].tmpl->zone->dilfilepath))
                strcpy(filename, g_cServerConfig.m_dilfiledir);
            else
                strcpy(filename, p->frame[0].tmpl->zone->dilfilepath);
            strcat(filename, "/units/");
            strcat(filename, (char *)v1->val.ptr);
            ret = remove(filename);
            if (ret)
                v->val.num = TRUE;
            else
                v->val.num = FALSE;
        }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
}

/* savestr - write a string to a file */
void dilfe_svstr(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *sstr;
    char filename[511];

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_SP:
            switch (dil_getval(v3))
            {
            case DILV_SP:
                if (p->frame[0].tmpl->zone->access > 10)
                {
                    szonelog(p->frame->tmpl->zone,
                             "DIL '%s' attempt to violate system access security (savestr).",
                             p->frame->tmpl->prgname);
                    p->waitcmd = WAITCMD_QUIT;
                }
                else if (str_is_empty((char *)v1->val.ptr) ||
                         str_is_empty((char *)v2->val.ptr) ||
                         str_is_empty((char *)v3->val.ptr))
                    v->type = DILV_FAIL;
                else
                {
                    if (str_is_empty(p->frame[0].tmpl->zone->dilfilepath))
                    {
                        if (!file_exists(g_cServerConfig.m_dilfiledir))
                        {
#ifdef _WINDOWS
                            _mkdir(g_cServerConfig.m_dilfiledir);
#else
                            mkdir(g_cServerConfig.m_dilfiledir,
                                  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
                                      S_IWGRP | S_IXGRP);
#endif
                        }
                        strcpy(filename, g_cServerConfig.m_dilfiledir);
                    }
                    else
                    {
                        if (!file_exists(p->frame[0].tmpl->zone->dilfilepath))
                        {
#ifdef _WINDOWS
                            _mkdir(p->frame[0].tmpl->zone->dilfilepath);
#else
                            mkdir(p->frame[0].tmpl->zone->dilfilepath,
                                  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
                                      S_IWGRP | S_IXGRP);
#endif
                        }
                        strcpy(filename, p->frame[0].tmpl->zone->dilfilepath);
                    }
                    strcat(filename, "/strings/");
                    if (!file_exists(filename))
#ifdef _WINDOWS
                        _mkdir(filename);
#else
                        mkdir(filename,
                              S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP |
                                  S_IXGRP);
#endif
                    strcat(filename, (char *)v1->val.ptr);
                    sstr = str_dup((char *)v2->val.ptr);
                    v->val.num =
                        save_string(filename, &sstr, (char *)v3->val.ptr);
                    FREE(sstr);
                }

                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }
            break;
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
            break;
        }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_filesz(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char filename[511];
    FILE *fp;

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        if (str_is_empty(p->frame[0].tmpl->zone->dilfilepath))
        {
            if (!file_exists(g_cServerConfig.m_dilfiledir))
            {
#ifdef _WINDOWS
                _mkdir(g_cServerConfig.m_dilfiledir);
#else
                mkdir(g_cServerConfig.m_dilfiledir,
                      S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP |
                          S_IXGRP);
#endif
            }
            strcpy(filename, g_cServerConfig.m_dilfiledir);
        }
        else
        {

            if (!file_exists(p->frame[0].tmpl->zone->dilfilepath))
            {
#ifdef _WINDOWS
                _mkdir(p->frame[0].tmpl->zone->dilfilepath);
#else
                mkdir(p->frame[0].tmpl->zone->dilfilepath,
                      S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP |
                          S_IXGRP);
#endif
            }

            strcpy(filename, p->frame[0].tmpl->zone->dilfilepath);
        }
        strcat(filename, "/strings/");
        if (!file_exists(filename))
#ifdef _WINDOWS
            _mkdir(filename);
#else
            mkdir(filename,
                  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
#endif
        strcat(filename, (char *)v1->val.ptr);
        if ((fp = fopen(filename, "r")) == NULL)
        {
            v->val.num = -1;
        }
        else
        {
            v->val.num = fsize(fp);
            fclose(fp);
        }
        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
}

/* visible, some vs other */
void dilfe_visi(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_UP:
                if (!v2->val.ptr)
                    v->type = DILV_FAIL;
                else
                    v->val.num = CHAR_CAN_SEE((class unit_data *)v1->val.ptr,
                                              (class unit_data *)v2->val.ptr);
                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

/* is unit opponent of other */
void dilfe_oppo(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_UP:
                if (!v2->val.ptr || !IS_CHAR((class unit_data *)v2->val.ptr))
                    v->type = DILV_FAIL;
                else
                    v->val.num = (CHAR_COMBAT((class unit_data *)v1->val.ptr) ? CHAR_COMBAT((class unit_data *)v1->val.ptr)->FindOpponent((class unit_data *)v2->val.ptr) !=
                                                                                    NULL
                                                                              : FALSE);
                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

/* get unit opponent of other */
void dilfe_gopp(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_INT:

                if (v2->val.num >= 0)
                {
                    if (CHAR_COMBAT((class unit_data *)v1->val.ptr))
                    {
                        v->val.ptr =
                            CHAR_COMBAT((class unit_data *)v1->val.ptr)->Opponent((int)v2->val.num);
                        if (v->val.ptr)
                        {
                            v->atyp = DILA_NORM;
                            v->type = DILV_UP;
                        }
                        else
                        {
                            v->val.ptr = NULL;
                            v->atyp = DILA_NORM;
                            v->type = DILV_NULL;
                        }
                    }
                }
                else
                {
                    v->val.ptr = NULL;
                    v->atyp = DILA_NORM;
                    v->type = DILV_NULL;
                }
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_FAIL:
    case DILV_NULL:
    default:
        v->type = DILV_ERR;
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

/* get unit follower of other */
void dilfe_gfol(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_INT:
                if (v2->val.num < 0)
                {
                    v->val.ptr = NULL;
                    v->atyp = DILA_NORM;
                    v->type = DILV_NULL;
                }
                else
                {

                    v->val.ptr =
                        get_follower((class unit_data *)v1->val.ptr, v2->val.num);
                    if (v->val.ptr)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_UP;
                    }
                    else
                    {
                        v->val.ptr = NULL;
                        v->atyp = DILA_NORM;
                        v->type = DILV_NULL;
                    }
                }
                break;
            default:
                v->type = DILV_ERR;
                break;
            }
        break;
    case DILV_FAIL:
    case DILV_NULL:
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

/* spellindex */
void dilfe_splx(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
    case DILV_SP:
        if (!v1->val.ptr)
            v->type = DILV_FAIL;
        else
        {
            const char *c = (const char *) v1->val.ptr;
            if (str_is_empty(c))
                v->val.num = -1;
            else
                v->val.num =
                    search_block_abbrevs(c, g_SplColl.text, &c);
        }
        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
}

/* spellinfo */
void dilfe_spli(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v8 = p->stack.pop();
    dilval *v7 = p->stack.pop();
    dilval *v6 = p->stack.pop();
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_INT:
        if (!is_in(v1->val.num, SPL_ALL, SPL_TREE_MAX - 1))
            v->type = DILV_FAIL;
        else
            switch (v2->type)
            {
            case DILV_SINT4R:
            case DILV_UINT4R:
                switch (v3->type)
                {
                case DILV_SINT4R:
                case DILV_UINT4R:
                    switch (v4->type)
                    {
                    case DILV_SINT4R:
                    case DILV_UINT4R:
                        switch (v5->type)
                        {
                        case DILV_SINT4R:
                        case DILV_UINT4R:
                            switch (v6->type)
                            {
                            case DILV_SINT4R:
                            case DILV_UINT4R:
                                switch (v7->type)
                                {
                                case DILV_SINT4R:
                                case DILV_UINT4R:
                                    switch (v8->type)
                                    {
                                    case DILV_SINT4R:
                                    case DILV_UINT4R:
                                        /* pName := spellinfo(SPL_LIGHTNING_3, nRealm, nSphere, nMana, bOffensive, nResistType, bvMedia, bvTargets); */

                                        /* We're home free... */

                                        *((ubit32 *)v2->ref) =
                                            spell_info[v1->val.num].realm;
                                        *((ubit32 *)v3->ref) =
                                            g_SplColl.tree[v1->val.num].parent;
                                        *((ubit32 *)v4->ref) =
                                            spell_info[v1->val.num].usesmana;
                                        *((ubit32 *)v5->ref) =
                                            spell_info[v1->val.num].offensive;
                                        *((ubit32 *)v6->ref) =
                                            spell_info[v1->val.num].cast_type;
                                        *((ubit32 *)v7->ref) =
                                            spell_info[v1->val.num].media;
                                        *((ubit32 *)v8->ref) =
                                            spell_info[v1->val.num].targets;

                                        v->type = DILV_SP;
                                        v->atyp = DILA_EXP;
                                        v->val.ptr =
                                            str_dup(g_SplColl.text[v1->val.num] == NULL ? "": g_SplColl.text[v1->val.num]);
                                        break;

                                    default:
                                        v->type = DILV_ERR;
                                        break;
                                    }

                                    break;

                                default:
                                    v->type = DILV_ERR;
                                    break;
                                }

                                break;

                            default:
                                v->type = DILV_ERR;
                                break;
                            }

                            break;

                        default:
                            v->type = DILV_ERR;
                            break;
                        }

                        break;

                    default:
                        v->type = DILV_ERR;
                        break;
                    }

                    break;

                default:
                    v->type = DILV_ERR;
                    break;
                }

                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
    delete v7;
    delete v8;
}

/* contents of purse */
void dilfe_purs(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr)
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_SP:
                if ((!v2->val.ptr) || (!*((char *)v2->val.ptr)))
                    v->type = DILV_FAIL;
                else
                {
                    int i;

                    v->val.num = 0;

                    for (i = 0; i <= MAX_MONEY; i++)
                        if (strcmp((char *)v2->val.ptr, money_types[i].abbrev) ==
                            0)
                            break;

                    if (i <= MAX_MONEY)
                    {
                        /* Note down money-objects in from, and their values */
                        for (class unit_data *tmp =
                                 UNIT_CONTAINS((class unit_data *)v1->val.ptr);
                             tmp; tmp = tmp->next)
                            if (IS_MONEY(tmp) && MONEY_TYPE(tmp) == i)
                            {
                                v->val.num = MONEY_AMOUNT(tmp);
                                break;
                            }
                    }
                }
                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

/* money_string */
void dilfe_mons(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_INT:
            v->atyp = DILA_EXP;
            v->val.ptr =
                str_dup(money_string(v1->val.num, DEF_CURRENCY, v2->val.num));
            break;
        default:
            v->type = DILV_ERR;
            break;
        }

        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

/* pathto */
void dilfe_path(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr)
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_UP:
                if (!v2->val.ptr)
                    v->type = DILV_FAIL;
                else
                {
                    class unit_data *u1, *u2;
                    u1 = unit_room((class unit_data *)v1->val.ptr);
                    u2 = unit_room((class unit_data *)v2->val.ptr);

                    v->val.num = move_to(u1, u2);
                }
                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

/* can_carry */
void dilfe_cary(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_UP:
                if (!v2->val.ptr)
                    v->type = DILV_FAIL;
                else
                    switch (dil_getval(v3))
                    {
                    case DILV_INT:
                        if (v3->val.num < 1)
                            v->type = DILV_FAIL;
                        else
                        {
                            if (!char_can_carry_n((class unit_data *)v1->val.ptr, v3->val.num))
                                v->val.num = 1;
                            else if (!char_can_carry_w((class unit_data *)v1->val.ptr,
                                                       v3->val.num *
                                                           UNIT_WEIGHT((class unit_data *)v2->val.ptr)))
                                v->val.num = 2;
                            else
                                v->val.num = 0;
                        }
                        break;
                    default:
                        v->type = DILV_ERR;
                        break;
                    }
                break;
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

/* transfermoney */
void dilfe_trmo(class dilprg *p)
{
    dilval *v = new dilval;
    char buf[1024];
    int i;
    buf[0] = 0;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_NULL:
    case DILV_UP:
        if (v1->val.ptr && !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_FAIL:
                v->type = DILV_FAIL;
                break;
            case DILV_NULL:
            case DILV_UP:
                if (v2->val.ptr && !IS_CHAR((class unit_data *)v2->val.ptr))
                    v->type = DILV_FAIL;
                else
                    switch (dil_getval(v3))
                    {
                    case DILV_FAIL:
                    case DILV_NULL:
                        v->type = DILV_FAIL;
                        break;
                    case DILV_INT:
                        break;
                    default:
                        v->type = DILV_ERR;
                        break;
                    }

                break;
            default:
                v->type = DILV_ERR;
                break;
            }
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    v->val.num = 0;

    if (v->type == DILV_INT && (v1->val.ptr || v2->val.ptr))
    {
        if (v1->val.ptr == NULL)
        {
            for (i = 0; i <= MAX_CURRENCY; ++i)
            {
                strcat(buf, " ");
                strcat(buf, money_string(v3->val.num, i, TRUE));
            }

            slog(LOG_ALL, 0, "%s was given %s by DIL %s@%s.",
                 UNIT_NAME((class unit_data *)v2->val.ptr),
                 buf,
                 UNIT_FI_NAME(p->sarg->owner),
                 UNIT_FI_ZONENAME(p->sarg->owner));
            money_transfer(NULL,
                           (class unit_data *)v2->val.ptr,
                           v3->val.num,
                           local_currency((class unit_data *)v2->val.ptr));
            v->val.num = 1;
        }
        else if (v2->val.ptr == NULL)
        {
            if (char_can_afford((class unit_data *)v1->val.ptr, v3->val.num,
                                local_currency((class unit_data *)v1->val.ptr)))
            {
                money_transfer((class unit_data *)v1->val.ptr, NULL,
                               v3->val.num,
                               local_currency((class unit_data *)v1->val.ptr));
                v->val.num = 1;
            }
        }
        else
        {
            if (char_can_afford((class unit_data *)v1->val.ptr, v3->val.num,
                                local_currency((class unit_data *)v2->val.ptr)))
            {
                money_transfer((class unit_data *)v1->val.ptr,
                               (class unit_data *)v2->val.ptr,
                               v3->val.num,
                               local_currency((class unit_data *)v2->val.ptr));
                v->val.num = 1;
            }
        }
    }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_fits(class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *c;

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_UP:
                if (!v2->val.ptr || !IS_OBJ((class unit_data *)v2->val.ptr))
                    v->type = DILV_FAIL;
                else
                    switch (dil_getval(v3))
                    {
                    case DILV_INT:
                        char *obj_wear_size(class unit_data * ch,
                                            class unit_data * obj, int keyword);

                        c = obj_wear_size((class unit_data *)v1->val.ptr,
                                          (class unit_data *)v2->val.ptr,
                                          v3->val.num);

                        v->atyp = DILA_EXP;
                        v->val.ptr = str_dup(c == NULL ? "" : c);
                        break;
                    default:
                        v->type = DILV_ERR;
                        break;
                    }
                break;
            case DILV_NULL:
            case DILV_FAIL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    if (v->type == DILV_SP)
        p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_intr(class dilprg *p)
{
    dilval *v = new dilval;
    /* add interrupt to current frame */
    ubit16 intnum;
    ubit8 *beg;
    ubit8 *end;
    ubit32 endptr;
    ubit32 begptr;

    dilval *v1 = p->stack.pop();

    begptr = bread_ubit32(&(p->fp->pc));
    endptr = bread_ubit32(&(p->fp->pc));
    beg = &(p->fp->tmpl->core[begptr]);
    end = &(p->fp->tmpl->core[endptr]);

    intnum = dil_intr_insert(p, beg, end, v1->val.num);

    // skip past the expression and label
    p->fp->pc = &(p->fp->tmpl->core[endptr + 4]);

    v->val.num = intnum;
    v->type = DILV_INT;
    v->atyp = DILA_NONE;

    p->stack.push(v);
    delete v1;
}

void dilfe_not(register class dilprg *p)
{
    dilval *v = new dilval;
    ;
    dilval *v1 = p->stack.pop();
    /* Negation of integers (and booleans, etc.) */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = !dil_getbool(v1, p);
    p->stack.push(v);
    delete v1;
}

void dilfe_umin(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    /* Unary minus */

    switch (dil_getval(v1))
    {
    case DILV_INT:
        v->atyp = DILA_NONE;
        v->type = DILV_INT;
        v->val.num = -v1->val.num;
        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    delete v1;
    p->stack.push(v);
}

void dilfe_skitxt(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    /* skill_name uses ski_text values to return skill names for a skill */

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        v->type = DILV_SP;
        v->atyp = DILA_EXP;
        if ((v1->val.num < SKI_TREE_MAX) && (v1->val.num >= 0) && g_SkiColl.text[v1->val.num])
            v->val.ptr = str_dup(g_SkiColl.text[v1->val.num]);
        else
            v->val.ptr = str_dup("");
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_wpntxt(register class dilprg *p)
{
    dilval *v = new dilval;
    /* weapon_name uses wpn_text values to return skill names for a skill */
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        v->type = DILV_SP;
        v->atyp = DILA_EXP;
        if ((v1->val.num < WPN_TREE_MAX) && (v1->val.num >= 0))
        {
            if (g_WpnColl.text[v1->val.num])
                v->val.ptr = str_dup(g_WpnColl.text[v1->val.num]);
            else
                v->val.ptr = str_dup("");
        }
        else
        {
            v->val.ptr = NULL; // OOB
            v->type = DILV_NULL;
        }
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_itoa(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Conversion of integers to strings */
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        v->type = DILV_SP;
        v->atyp = DILA_EXP;
        v->val.ptr = str_dup(itoa(v1->val.num));
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_atoi(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Conversion of strings to integers */
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;
    case DILV_SP:
        if (v1->val.ptr)
        {
            v->atyp = DILA_NONE;
            v->type = DILV_INT;
            v->val.num = atoi((char *)v1->val.ptr);
        }
        else
            v->type = DILV_FAIL; /* failed */
        break;
    default:
        v->type = DILV_ERR; /* Wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_isplayer(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Check to see if a player exists by name xxx */
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;
    case DILV_SP:
        if (v1->val.ptr)
        {
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            int find_player_id(char *pName);
            if (find_player_id((char *)v1->val.ptr) != -1)
                v->val.num = TRUE;
            else
                v->val.num = FALSE;
        }
        else
            v->type = DILV_FAIL; /* failed */
        break;
    default:
        v->type = DILV_ERR; /* Wrong type */
        break;
    }

    p->stack.push(v);
    delete v1;
}

void dilfe_len(register class dilprg *p)
{
    dilval *v = new dilval;
    /* length of strings, stringlists, or intlist */
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;
    case DILV_SP:
        v->atyp = DILA_NONE;
        v->type = DILV_INT;
        if (v1->val.ptr)
            v->val.num = strlen((char *)v1->val.ptr);
        else
            v->val.num = 0;
        break;
    case DILV_SLP:
        v->atyp = DILA_NONE;
        v->type = DILV_INT;
        if (v1->val.ptr)
            v->val.num = ((cNamelist *)v1->val.ptr)->Length();
        else
            v->val.num = 0;
        break;
    case DILV_ILP:
        v->atyp = DILA_NONE;
        v->type = DILV_INT;
        if (v1->val.ptr)
            v->val.num = ((cintlist *)v1->val.ptr)->Length();
        else
            v->val.num = 0;
        break;
    default:
        v->type = DILV_ERR; /* Wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

long space_quick_count(char *src)
{
    char Buf[10];
    int rep = 0;

    src += 2;
    ;
    while (isdigit(*src) && rep < (int)sizeof(Buf) - 1)
        Buf[rep++] = *src++;
    Buf[rep] = 0;

    return (atoi(Buf));
}

long str_escape_size(char *sbuf, long ln)
{
    long count = 0, i = 0;

    while (i < ln)
    {
        if ((*sbuf == '&') && ((*(sbuf + 1) == 's') || (*(sbuf + 1) == 'S')))
        {
            count += space_quick_count(sbuf);
        }
        sbuf++;
        i++;
    }

    return (int(ln * 1.2) + (count * 6) + MAX_INPUT_LENGTH);
}

/* textformat */
void dilfe_txf(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char *dest;

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;

    case DILV_SP:
        if (v1->val.ptr)
        {
            int n = str_escape_size((char *)v1->val.ptr, strlen((char *)v1->val.ptr));
            CREATE(dest, char, n);

            v->atyp = DILA_EXP;
            v->type = DILV_SP;
            str_escape_format((char *)v1->val.ptr, dest, n, FALSE);

            v->val.ptr = str_dup(dest);
            FREE(dest);
        }
        else
            v->type = DILV_FAIL; /* NULL string */
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

/* sact - return string (call act_generate) */
void dilfe_sact(register class dilprg *p)
{
    dilval *v = new dilval; // Return variable
    v->type = DILV_FAIL;    // NULL string

    /* sact() function call */
    dilval *v6 = p->stack.pop();
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    /* 6 arguments */

    p->waitcmd--;

    if (dil_type_check("sact", p, 6,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, TYPEFAIL_NULL, 1, DILV_INT,
                       v3, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL,
                       v4, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL,
                       v5, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL,
                       v6, TYPEFAIL_NULL, 1, DILV_INT))
    {
        switch (v6->val.num)
        {
        case TO_CHAR:
        case TO_ROOM:
        case TO_ALL:
        case TO_REST:
            /* these require 1st argument */
            if (v3->val.ptr)
            {
                char dest[MAX_STRING_LENGTH];
                sact(dest, (char *)v1->val.ptr, v2->val.num, v3, v4, v5, v6->val.num);
                v->val.ptr = str_dup(dest);
                v->atyp = DILA_EXP;
                v->type = DILV_SP;
            }
            break;

        case TO_VICT:
        case TO_NOTVICT:
            if (v5->val.ptr)
            {
                char dest[MAX_STRING_LENGTH];
                sact(dest, (char *)v1->val.ptr, v2->val.num, v3, v4, v5, v6->val.num);
                v->val.ptr = str_dup(dest);
                v->atyp = DILA_EXP;
                v->type = DILV_SP;
            }
        } // switch
    }     // if
    else
        v->type = DILV_ERR; /* wrong type */

    p->stack.push(v);

    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
}


/* getinteger(idx, p_u, p_i) : index is the kind of int to get.
 * p_u / p_i are optional unitptr / integer parameters
 */
void dilfe_gint(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop(); // INT
    dilval *v2 = p->stack.pop(); // UNIT
    dilval *v1 = p->stack.pop(); // INT

    class unit_data *p_u = NULL;
    int p_i = 0;
    int idx;

    v->type = DILV_INT;
    v->val.num = 0;

    switch (dil_getval(v3))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;

    case DILV_INT:
        p_i = v3->val.num;
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;

    case DILV_UP:
    case DILV_NULL:
        p_u = (class unit_data *) v2->val.ptr;
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;

    case DILV_INT:
        idx = v1->val.num;
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    if (v->type == DILV_INT)
    {
        switch (idx)
        {
        case DIL_GINT_EFFDEX:
            if ((p_u != NULL) && IS_CHAR(p_u))
                v->val.num = effective_dex(p_u);
            break;

        case DIL_GINT_REQXP:
            int required_xp(int level);
            v->val.num = required_xp(p_i);
            break;

        case DIL_GINT_LEVELXP:
            int level_xp(int level);
            v->val.num = level_xp(p_i);
            break;

        case DIL_GINT_DESCRIPTOR:
            if ((p_u != NULL) && IS_PC(p_u))
                v->val.num = (CHAR_DESCRIPTOR(p_u) != NULL);
            else
                v->val.num = 1;
            break;
      
      	case DIL_GINT_CALLGUARDS:
            if ((p->sarg->owner != NULL) )
               {
                call_guards(p->sarg->owner);
                v->val.num = 1;
               }
           else
               v->val.num = 0;
            break;

        default:
            v->type = DILV_FAIL; /* failed */
            slog(LOG_ALL, 0, "getinteger() was given incorrect index %d by DIL %s@%s.",
                 idx, UNIT_FI_NAME(p->sarg->owner), UNIT_FI_ZONENAME(p->sarg->owner));
            break;
        }
    }

    p->stack.push(v);

    delete v1;
    delete v2;
    delete v3;
}

/* asctime */
void dilfe_ast(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char *c;

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;

    case DILV_INT:
        c = ctime((time_t *)&v1->val.num);
        assert(strlen(c) >= 1);

        v->atyp = DILA_EXP;
        v->type = DILV_SP;
        c[strlen(c) - 1] = 0;
        v->val.ptr = str_dup(c);
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_getw(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Get first word of a string */
    dilval *v1 = p->stack.pop();
    char *c;
    char buf1[MAX_STRING_LENGTH];

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;
    case DILV_SP:
        if (v1->val.ptr)
        {
            char *str_next_word_copy(const char *argument, char *first_arg);

            v->atyp = DILA_EXP;
            v->type = DILV_SP;
            c = str_next_word_copy((char *)v1->val.ptr, buf1);
            c = skip_spaces(c);
            v->val.ptr = str_dup(buf1);

            if (v1->atyp == DILA_NORM && v1->type == DILV_SPR)
                memmove(*(char **)v1->ref, c, strlen(c) + 1);
            else if (p->sarg->arg == (const char *)v1->val.ptr)
                p->sarg->arg = c; /* Update argument as in findunit :) */
        }
        else
            v->type = DILV_FAIL; /* NULL string */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_getws(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Get first word of a string */
    dilval *v1 = p->stack.pop();
    char *tmp, *c, *ptr;

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL; /* failed */
        break;

    case DILV_SP:
        if (v1->val.ptr)
        {
            cNamelist *words = new cNamelist;

            v->atyp = DILA_EXP;
            v->type = DILV_SLP;

            tmp = str_dup((char *)v1->val.ptr);
            ptr = tmp;
            for (c = strtok(tmp, " "); c && *c; c = strtok(NULL, " "))
                words->dAppendName(c);
            FREE(ptr);

            v->val.ptr = words;
        }
        else
            v->type = DILV_FAIL; /* NULL string */
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_split(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Get first word of a string */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int slen;
    char *buf = NULL, *c = NULL, *spbuf = NULL;

    v->type = DILV_SLP;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_SP:
            if ((v1->val.ptr) && (v2->val.ptr))
            {
                cNamelist *words = new cNamelist;
                v->atyp = DILA_EXP;
                v->type = DILV_SLP;

                buf = (char *)malloc(strlen((char *)v1->val.ptr) + 1);
                spbuf = (char *)malloc(strlen((char *)v1->val.ptr) + 1);

                strcpy(buf, (char *)v1->val.ptr);

                while ((c = str_str(buf, (char *)v2->val.ptr)))
                {
                    slen = strlen(buf);
                    strncpy(spbuf, buf, slen - strlen(c) + 1);
                    spbuf[slen - strlen(c)] = 0;
                    words->dAppendName(spbuf);
                    spbuf[0] = 0;
                    strcpy(spbuf, c + strlen((char *)v2->val.ptr));
                    strcpy(buf, spbuf);
                    spbuf[0] = 0;
                }

                if (buf)
                    words->dAppendName(buf);

                v->val.ptr = words;
                break;
            }
            else
            {
                v->type = DILV_FAIL;
            }
            break;
        case DILV_FAIL:
        case DILV_NULL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR;
            break;
        }

        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    if (buf)
    {
        free(buf);
        buf = NULL;
    }
    if (spbuf)
    {
        free(spbuf);
        spbuf = NULL;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

// BOOST VERSION
/*return a unit file directory listing */
void dilfe_udir(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();

    cNamelist *words = new cNamelist;
    string uPath, sPath;

    v->type = DILV_SLP;

    switch (dil_getval(v1))
    {
    case DILV_SP:
    {
        if (v1->val.ptr)
        {
            v->atyp = DILA_EXP;
            v->type = DILV_SLP;
            if (str_is_empty(p->frame[0].tmpl->zone->dilfilepath))
            {
                uPath = g_cServerConfig.m_dilfiledir;
                uPath = uPath + "/units";
            }
            else
            {
                uPath = p->frame[0].tmpl->zone->dilfilepath;
                uPath = uPath + "/units";
            }

            sPath = (char *)v1->val.ptr;
            if (sPath.empty())
                sPath = ".*";

            fs::path full_path(uPath);
            fs::directory_iterator end_iter;
            regex expression;

            try
            {
                expression.assign(sPath);
            }
            /*      catch(std::exception& e)
            {*/
            catch (...)
            {

                v->type = DILV_FAIL;
                break;
            }

            try
            {
                if ((fs::exists(full_path)) &&
                    (fs::is_directory(full_path)))
                    for (fs::directory_iterator dir_itr(full_path);
                         dir_itr != end_iter;
                         ++dir_itr)
                    {
                        cmatch what;

                        if (regex_match(dir_itr->path().filename().c_str(), what, expression))
                        {

                            words->dAppendName(dir_itr->path().filename().c_str());
                        }
                    }
            }
            catch (const std::exception &ex)
            {

                v->type = DILV_FAIL;
                break;
            }

            v->val.ptr = words;
            break;
        }
        else
            v->type = DILV_FAIL;
    }
    break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
}

// BOOST VERSION
/*return a string file directory listing */
void dilfe_sdir(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    cNamelist *words = new cNamelist;
    string uPath, sPath;
    v->type = DILV_SLP;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        if (v1->val.ptr)
        {
            v->atyp = DILA_EXP;
            v->type = DILV_SLP;
            if (str_is_empty(p->frame[0].tmpl->zone->dilfilepath))
            {
                uPath = g_cServerConfig.m_dilfiledir;
                uPath = uPath + "/strings";
            }
            else
            {
                uPath = p->frame[0].tmpl->zone->dilfilepath;
                uPath = uPath + "/strings";
            }

            sPath = (char *)v1->val.ptr;
            if (sPath.empty())
                sPath = ".*";

            fs::path full_path(uPath);
            fs::directory_iterator end_iter;

            regex expression;

            try
            {
                expression.assign(sPath);
            }
            catch (std::exception &e)
            {
                v->type = DILV_ERR;
                break;
            }

            try
            {
                if ((fs::exists(full_path)) &&
                    (fs::is_directory(full_path)))
                    for (fs::directory_iterator dir_itr(full_path);
                         dir_itr != end_iter;
                         ++dir_itr)
                    {
                        cmatch what;

                        if (regex_match(dir_itr->path().filename().c_str(), what, expression))
                            words->dAppendName(dir_itr->path().filename().c_str());
                    }
            }
            catch (std::exception &ex)
            {
                v->type = DILV_ERR;
                break;
            }

            v->val.ptr = words;
        }
        else
            v->type = DILV_FAIL;
        break;
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
}

void dilfe_wepinfo(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();

    v->type = DILV_ILP;

    switch (dil_getval(v1))
    {
    case DILV_INT:
        if (((int)v1->val.num >= 0) && ((int)v1->val.num < WPN_TREE_MAX))
        {
            cintlist *ilist = new cintlist;
            ilist->Append(wpn_info[(int)v1->val.num].hands);
            ilist->Append(wpn_info[(int)v1->val.num].speed);
            ilist->Append(wpn_info[(int)v1->val.num].type);
            ilist->Append(wpn_info[(int)v1->val.num].shield);
            ilist->Append(g_WpnColl.tree[(int)v1->val.num].parent);
            for (int idx2 = 0; idx2 < MAX_ARMOUR_TYPES; idx2++)
            {
                ilist->Append(weapon_chart[(int)v1->val.num].element[idx2].offset);
                ilist->Append(weapon_chart[(int)v1->val.num].element[idx2].basedam);
                ilist->Append(weapon_chart[(int)v1->val.num].element[idx2].alpha);
            }
            v->val.ptr = ilist;
        }
        else
        {
            v->val.ptr = NULL;
            v->type = DILV_NULL;
        }
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    p->stack.push(v);
    delete v1;
}

void dilfe_load(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Load a unit from database */
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_SP:
        if (v1->val.ptr)
        {
            v->val.ptr = read_unit(str_to_file_index((char *)v1->val.ptr));
            if (v->val.ptr)
            {
                if (IS_MONEY((class unit_data *)v->val.ptr))
                    set_money((class unit_data *)v->val.ptr,
                              MONEY_AMOUNT((class unit_data *)v->val.ptr));

                unit_to_unit((class unit_data *)v->val.ptr, p->sarg->owner);
                dil_loadtime_activate((class unit_data *)v->val.ptr);
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
            }
            else
                v->type = DILV_NULL; /* Unit not found */
        }
        else
            v->type = DILV_FAIL; /* NULL string */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_getcmd(register class dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_SP:
        if (v1->val.ptr)
        {
            v->val.ptr =
                (struct command_info *)search_trie((char *)v1->val.ptr,
                                                   intr_trie);
            if (v->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_CP;
            }
            else
                v->type = DILV_NULL; /* Unit not found */
        }
        else
            v->type = DILV_FAIL; /* NULL string */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_clone(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Load a unit from database */
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_UP:
        if (v1->val.ptr)
        {
            v->val.ptr = ((class unit_data *)v1->val.ptr)->copy();
            if (v->val.ptr)
            {
                unit_to_unit((class unit_data *)v->val.ptr, p->sarg->owner);
                dil_loadtime_activate((class unit_data *)v->val.ptr);
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
            }
            else
                v->type = DILV_NULL; /* Unit not found */
        }
        else
            v->type = DILV_FAIL; /* NULL string */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_plus(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Addition of strings or integers */

    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v2))
    {
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;

    case DILV_INT:
        v->type = DILV_INT;
        break;

    case DILV_SP:
        v->type = DILV_SP;
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;

    case DILV_SP:
        if (v->type != DILV_SP)
            v->type = DILV_ERR; /* wrong type */
        break;

    case DILV_INT:
        if (v->type != DILV_INT)
            v->type = DILV_ERR; /* wrong type */
        break;

    case DILV_NULL:
        if (v->type != DILV_SP)
            v->type = DILV_ERR; /* wrong type */
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (v->type)
    {
    case DILV_SP:
        v->atyp = DILA_EXP;
        CREATE(v->val.ptr, char, (strlen(STR((char *)v1->val.ptr)) + strlen(STR((char *)v2->val.ptr)) + 1));
        strcpy((char *)v->val.ptr, STR((char *)v1->val.ptr));
        strcat((char *)v->val.ptr, STR((char *)v2->val.ptr));
        strcat((char *)v->val.ptr, "\0");
        /*\\      v->val.ptr = str_dup(str_cc(STR((char *) v1->val.ptr),
        \\				  STR((char *) v2->val.ptr))); */
        break;

    case DILV_INT:
        v->atyp = DILA_NONE;
        v->val.num = v1->val.num + v2->val.num;
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}


// DIL destroy
void dilfe_dld(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Destruction of DIL programs */

    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
    case DILV_SP:
        switch (dil_getval(v2))
        {
        case DILV_UP:
            if (v1->val.ptr && v2->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->val.num = dil_destroy((char *)v1->val.ptr,
                                         (class unit_data *)v2->val.ptr);
            }
            break;
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        break;
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_dlf(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Detection of DIL programs (TRUE/FALSE) */

    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v2))
    {
    case DILV_UP:
        switch (dil_getval(v1))
        {
        case DILV_SP:
            if (v1->val.ptr && v2->val.ptr)
            {
                v->atyp = DILA_NONE;
                if (dil_find((char *)v1->val.ptr, (class unit_data *)v2->val.ptr))
                    v->val.num = TRUE;
                else
                    v->val.num = FALSE;
            }
            break;
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_min(register class dilprg *p)
{
    /* Subtraction of integers */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v2))
    {
    case DILV_INT:
        switch (dil_getval(v1))
        {
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = v1->val.num - v2->val.num;
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_mul(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v2))
    {
    case DILV_INT:
        switch (dil_getval(v1))
        {
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = v1->val.num * v2->val.num;
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_div(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v2))
    {
    case DILV_INT:
        switch (dil_getval(v1))
        {
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            if (v2->val.num == 0)
                v->type = DILV_FAIL;
            else
                v->val.num = v1->val.num / v2->val.num;
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_mod(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v2))
    {
    case DILV_INT:
        switch (dil_getval(v1))
        {
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            if (v2->val.num == 0)
                v->type = DILV_FAIL;
            else
                v->val.num = v1->val.num % v2->val.num;
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_and(register class dilprg *p)
{
    /* And two integers (or booleans) */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    if ((dil_getval(v2) == DILV_INT) && (dil_getval(v1) == DILV_INT))
    {
        v->type = DILV_INT;
        v->atyp = DILA_NONE;
        v->val.num = v1->val.num & v2->val.num;
    }
    else
        v->type = DILV_FAIL;

    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_land(register class dilprg *p)
{
    dilval *v = new dilval;
    /* And two integers (or booleans) */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = dil_getbool(v1, p) && dil_getbool(v2, p);
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_or(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Or two integers (or booleans) */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    v->atyp = DILA_NONE;

    if ((dil_getval(v1) == DILV_INT) && (dil_getval(v2) == DILV_INT))
        v->val.num = v1->val.num | v2->val.num;
    else
        v->type = DILV_FAIL;
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_lor(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Or two integers (or booleans) */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = dil_getbool(v1, p) || dil_getbool(v2, p);
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_isa(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Test if unit is affected by affect */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
    case DILV_UP:
        if (v1->val.ptr)
            switch (dil_getval(v2))
            {
            case DILV_INT:
                v->atyp = DILA_NONE;
                v->val.num = (affected_by_spell((class unit_data *)v1->val.ptr,
                                                v2->val.num) != NULL);
                break;
            case DILV_FAIL:
                v->type = DILV_FAIL;
                break;
            default:
                v->type = DILV_ERR; /* wrong type */
                break;
            }

        else
            v->type = DILV_FAIL;
        break;
    case DILV_NULL:
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_rnd(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Random in an integer range */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_INT:
            v->atyp = DILA_NONE;
            v->val.num = number(v1->val.num, v2->val.num);
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_fndr(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Find a room */
    dilval *v1 = p->stack.pop();
    char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_SP:
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        *buf1 = '\0';
        *buf2 = '\0';
        if (v1->val.ptr)
        {
            split_fi_ref((char *)v1->val.ptr, buf1, buf2);
            v->val.ptr = world_room(buf1, buf2);
            if (v->val.ptr == NULL)
                v->type = DILV_NULL; /* not found */
        }
        else
            v->type = DILV_NULL; /* not found */

        break;
    case DILV_NULL:
        v->type = DILV_NULL; /* not found */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_fnds2(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Find a symbolic unit */
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

    v->type = DILV_UP;
    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;

    case DILV_UP:
        if (v1->val.ptr)
            switch (dil_getval(v2))
            {
            case DILV_SP:
                if (v2->val.ptr == NULL)
                    v->type = DILV_FAIL;
                else
                    switch (dil_getval(v3))
                    {
                    case DILV_INT:
                    {
                        v->atyp = DILA_NORM;
                        *buf1 = '\0';
                        *buf2 = '\0';
                        split_fi_ref((char *)v2->val.ptr, buf1, buf2);
                        v->val.ptr =
                            find_symbolic_instance_ref((class unit_data *)v1->val.ptr, find_file_index(buf1, buf2),
                                                       v3->val.num);
                        if (v->val.ptr == NULL)
                            v->type = DILV_NULL; /* not found */
                    }
                    break;
                    case DILV_FAIL:
                        v->type = DILV_FAIL;
                        break;
                    case DILV_NULL:
                        v->type = DILV_NULL; /* not found */
                        break;
                    default:
                        v->type = DILV_ERR; /* wrong type */
                        break;
                    }

                break;
            case DILV_FAIL:
                v->type = DILV_FAIL;
                break;
            case DILV_NULL:
                v->type = DILV_NULL; /* not found */
                break;
            default:
                v->type = DILV_ERR; /* wrong type */
                break;
            }
        else
            v->type = DILV_FAIL;
        break;
    case DILV_NULL:
        v->type = DILV_NULL; /* not found */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_fnds(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Find a symbolic unit */
    dilval *v1 = p->stack.pop();
    char buf1[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];

    switch (dil_getval(v1))
    {
    case DILV_NULL:
        v->type = DILV_NULL; /* not found */
        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_SP:
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        *buf1 = '\0';
        *buf2 = '\0';
        split_fi_ref((char *)v1->val.ptr, buf1, buf2);
        v->val.ptr = find_symbolic(buf1, buf2);
        if (v->val.ptr == NULL)
            v->type = DILV_NULL; /* not found */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_gt(register class dilprg *p)
{
    /* Greater Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = (v1->val.num > v2->val.num);
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_lt(register class dilprg *p)
{
    /* Greater Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = (v1->val.num < v2->val.num);
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_ge(register class dilprg *p)
{
    /* Greater Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = (v1->val.num >= v2->val.num);
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_le(register class dilprg *p)
{
    /* Greater Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = (v1->val.num <= v2->val.num);
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_eq(register class dilprg *p)
{
    /* Greater Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = (v2->val.num == v1->val.num);
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_ne(register class dilprg *p)
{
    /* Greater Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        switch (dil_getval(v1))
        {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = (v1->val.num != v2->val.num);
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_slt(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Less Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *s1, *s2;
    v->type = DILV_INT;
    switch (dil_getval(v2))
    {
    case DILV_SP:
    case DILV_NULL:
        switch (dil_getval(v1))
        {
        case DILV_SP:
        case DILV_NULL:
            v->atyp = DILA_NONE;
            if (((char *)v1->val.ptr == NULL) || ((char *)v2->val.ptr == NULL))
                v->val.num = FALSE;
            else
            {
                s1 = str_dup((char *)v1->val.ptr);
                s2 = str_dup((char *)v2->val.ptr);
                str_lower(s1);
                str_lower(s2);
                if (strcmp(s1, s2) < 0)
                    v->val.num = TRUE;
                else
                    v->val.num = FALSE;
                FREE(s1);
                FREE(s2);
            }
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_sgt(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Less Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *s1, *s2;
    v->type = DILV_INT;
    switch (dil_getval(v2))
    {
    case DILV_SP:
    case DILV_NULL:
        switch (dil_getval(v1))
        {
        case DILV_SP:
        case DILV_NULL:
            v->atyp = DILA_NONE;
            if (((char *)v1->val.ptr == NULL) || ((char *)v2->val.ptr == NULL))
                v->val.num = FALSE;
            else
            {
                s1 = str_dup((char *)v1->val.ptr);
                s2 = str_dup((char *)v2->val.ptr);
                str_lower(s1);
                str_lower(s2);
                if (strcmp(s1, s2) > 0)
                    v->val.num = TRUE;
                else
                    v->val.num = FALSE;
                FREE(s1);
                FREE(s2);
            }
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_sle(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Less Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *s1, *s2;
    v->type = DILV_INT;
    switch (dil_getval(v2))
    {
    case DILV_SP:
    case DILV_NULL:
        switch (dil_getval(v1))
        {
        case DILV_SP:
        case DILV_NULL:
            v->atyp = DILA_NONE;
            if (((char *)v1->val.ptr == NULL) || ((char *)v2->val.ptr == NULL))
                v->val.num = FALSE;
            else
            {
                s1 = str_dup((char *)v1->val.ptr);
                s2 = str_dup((char *)v2->val.ptr);
                str_lower(s1);
                str_lower(s2);
                if (strcmp(s1, s2) <= 0)
                    v->val.num = TRUE;
                else
                    v->val.num = FALSE;
                FREE(s1);
                FREE(s2);
            }
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_sge(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Less Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *s1, *s2;
    v->type = DILV_INT;
    switch (dil_getval(v2))
    {
    case DILV_SP:
    case DILV_NULL:
        switch (dil_getval(v1))
        {
        case DILV_SP:
        case DILV_NULL:
            v->atyp = DILA_NONE;
            if (((char *)v1->val.ptr == NULL) || ((char *)v2->val.ptr == NULL))
                v->val.num = FALSE;
            else
            {
                s1 = str_dup((char *)v1->val.ptr);
                s2 = str_dup((char *)v2->val.ptr);
                str_lower(s1);
                str_lower(s2);
                if (strcmp(s1, s2) >= 0)
                    v->val.num = TRUE;
                else
                    v->val.num = FALSE;
                FREE(s1);
                FREE(s2);
            }
            break;
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        break;
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_se(register class dilprg *p)
{
    dilval *v = new dilval;
    /* String equal operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_SP:
        v->type = DILV_INT;
        break;
    case DILV_NULL:
        v->type = DILV_NULL; /* null pointer */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;
    case DILV_SP:
        break;
    case DILV_NULL:
        if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            v->type = DILV_NULL; /* null pointer */
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    switch (v->type)
    {
    case DILV_INT:
        v->atyp = DILA_NONE;
        if (str_is_empty((char *)v1->val.ptr) ||
            str_is_empty((char *)v2->val.ptr))
            v->val.num = (str_is_empty((char *)v1->val.ptr) &&
                          str_is_empty((char *)v2->val.ptr));
        else
            v->val.num = !str_ccmp((char *)v1->val.ptr, (char *)v2->val.ptr);
        break;
    case DILV_NULL:
        v->atyp = DILA_NONE;
        v->type = DILV_INT;
        v->val.num = FALSE;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_sne(register class dilprg *p)
{
    dilval *v = new dilval;
    /* String not equal operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;

    case DILV_NULL:
    case DILV_SP:
        v->type = DILV_INT;
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;

    case DILV_NULL:
    case DILV_SP:
        break;

    default:
        v->type = DILV_ERR;
        break;
    }

    switch (v->type)
    {
    case DILV_INT:
        v->atyp = DILA_NONE;
        if (str_is_empty((char *)v1->val.ptr) ||
            str_is_empty((char *)v2->val.ptr))
            v->val.num = (!str_is_empty((char *)v1->val.ptr) ||
                          !str_is_empty((char *)v2->val.ptr));
        else
            v->val.num =
                (str_ccmp((char *)v1->val.ptr, (char *)v2->val.ptr) != 0);
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_pe(register class dilprg *p)
{
    dilval *v = new dilval;
    int ilp1 = 0;
    int slp1 = 0;
    int ilp2 = 0;
    int slp2 = 0;
    int n1 = 0;
    int n2 = 0;
    /* Pointer Equality operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_NULL:
        n2 = 1;
        v->type = DILV_INT;
        break;
    case DILV_SLP:
        slp2 = 1;
        v->type = DILV_INT;
        break;
    case DILV_ILP:
        ilp2 = 1;
        v->type = DILV_INT;
        break;

    case DILV_EDP:
    case DILV_UP:
    case DILV_SP:
    case DILV_ZP:
        v->type = DILV_INT;
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_NULL:
        n1 = 1;
        v->type = DILV_INT;
        break;
    case DILV_SLP:
        slp1 = 1;
        v->type = DILV_INT;
        break;
    case DILV_ILP:
        ilp1 = 1;
        v->type = DILV_INT;
        break;

    case DILV_SP:
    case DILV_EDP:
    case DILV_UP:
    case DILV_ZP:
        v->type = DILV_INT;
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    if (v->type == DILV_INT)
    {
        v->atyp = DILA_NONE;
        if (v1->val.ptr && ilp1 && n2)
            v->val.num = ((((cintlist *)v1->val.ptr)->Length()) == 0);
        else if (v2->val.ptr && ilp2 && n1)
            v->val.num = ((((cintlist *)v2->val.ptr)->Length()) == 0);
        else if (v1->val.ptr && slp1 && n2)
            v->val.num = ((((cNamelist *)v1->val.ptr)->Length()) == 0);
        else if (v2->val.ptr && slp2 && n1)
            v->val.num = ((((cNamelist *)v2->val.ptr)->Length()) == 0);
        else
            v->val.num = (v1->val.ptr == v2->val.ptr);
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_pne(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Pointer Equality operator */
    int ilp1 = 0;
    int slp1 = 0;
    int ilp2 = 0;
    int slp2 = 0;
    int n1 = 0;
    int n2 = 0;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;

    case DILV_NULL:
        n2 = 1;
        break;
    case DILV_SLP:
        slp2 = 1;
        break;
    case DILV_ILP:
        ilp2 = 1;
        break;
    case DILV_EDP:
    case DILV_UP:
    case DILV_ZP:
    case DILV_SP:
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;

    case DILV_NULL:
        n1 = 1;
        break;
    case DILV_SLP:
        slp1 = 1;
        break;
    case DILV_ILP:
        ilp1 = 1;
        break;
    case DILV_EDP:
    case DILV_SP:
    case DILV_UP:
    case DILV_ZP:
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    if (v->type == DILV_INT)
    {
        v->atyp = DILA_NONE;
        if (v1->val.ptr && ilp1 && n2)
            v->val.num = ((((cintlist *)v1->val.ptr)->Length()) != 0);
        else if (v2->val.ptr && ilp2 && n1)
            v->val.num = ((((cintlist *)v2->val.ptr)->Length()) != 0);
        else if (v1->val.ptr && slp1 && n2)
            v->val.num = ((((cNamelist *)v1->val.ptr)->Length()) != 0);
        else if (v2->val.ptr && slp2 && n1)
            v->val.num = ((((cNamelist *)v2->val.ptr)->Length()) != 0);
        else
            v->val.num = (v1->val.ptr != v2->val.ptr);
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_iss(register class dilprg *p)
{
    // Test if bist are set
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    dilval *v = new dilval;
    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        switch (dil_getval(v1))
        {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        case DILV_INT:
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = IS_SET(v1->val.num, v2->val.num);
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_in(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Test if string in string, stringlist or extra description */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    int t2 = dil_getval(v2); 
    int t1 = dil_getval(v1); 

    switch (t2)
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;

    case DILV_SLP:
        if (v2->val.ptr)
            v->type = DILV_SLP;
        else
            v->type = DILV_NULL; /* not found */
        break;

    case DILV_EDP:
        if (v2->val.ptr)
            v->type = DILV_EDP;
        else
            v->type = DILV_NULL; /* not found */
        break;

    case DILV_SP:
        if (v2->val.ptr)
            v->type = DILV_INT;
        else
            v->type = DILV_NULL; /* not found */
        break;

    case DILV_NULL:
        v->type = DILV_NULL; /* not found */
        break;

    default:
        v->type = DILV_ERR;
        break;
    }

    switch (t1)
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;

    case DILV_SP:
        if (v1->val.ptr)
            break;
        else
            v->type = DILV_FAIL;

    case DILV_NULL:
        v->type = DILV_NULL;
        v->val.num = 0;
        break;

    default:
        v->type = DILV_ERR;
        break;
    }

    switch (v->type)
    {
    case DILV_SLP:
        v->atyp = DILA_NONE;
        v->type = DILV_INT;
        v->val.num = 0;
        if (v1->val.ptr)
            v->val.num = 1 + (((cNamelist *)v2->val.ptr)->IsNameIdx((char *)v1->val.ptr));
        break;

    case DILV_EDP:
        v->atyp = DILA_NORM;
        v->type = DILV_EDP;
        if (v1->val.ptr && v2->val.ptr) //MS2020 BUG
            v->val.ptr =
                ((class extra_descr_data *) v2->val.ptr)->find_raw(skip_spaces((char *)v1->val.ptr));
        else
            v->val.ptr = NULL;
        break;

    case DILV_INT:
        v->atyp = DILA_NONE;
        v->type = DILV_INT;
        v->val.num = FALSE;
        if ((t1 == DILV_SP) && (t2 == DILV_SP) && v1->val.ptr && v2->val.ptr)
            v->val.num = str_cstr((const char *)v2->val.ptr, (const char *)v1->val.ptr) != NULL;
        else
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_strcmp(register class dilprg *p)
{
    dilval *v = new dilval;
    /* String equal operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_SP:
        v->type = DILV_INT;
        break;
    case DILV_NULL:
        v->type = DILV_NULL; /* null pointer */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;
    case DILV_SP:
        break;
    case DILV_NULL:
        if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            v->type = DILV_NULL; /* null pointer */
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    switch (v->type)
    {
    case DILV_INT:
        v->atyp = DILA_NONE;
        v->val.num = strcmp((char *)v1->val.ptr, (char *)v2->val.ptr);
        break;
    case DILV_NULL:
        v->atyp = DILA_NONE;
        v->type = DILV_INT;
        v->val.num = FALSE;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_strncmp(register class dilprg *p)
{
    dilval *v = new dilval;
    /* String equal operator */
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v3))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_INT:
        v->type = DILV_INT;
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_SP:
        v->type = DILV_INT;
        break;
    case DILV_NULL:
        v->type = DILV_NULL; /* null pointer */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;
    case DILV_SP:
        break;
    case DILV_NULL:
        if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            v->type = DILV_NULL; /* null pointer */
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    switch (v->type)
    {
    case DILV_INT:
        v->atyp = DILA_NONE;
        v->val.num = strncmp((char *)v1->val.ptr, (char *)v2->val.ptr,
                             (int)v3->val.num);
        break;
    case DILV_NULL:
        v->atyp = DILA_NONE;
        v->type = DILV_INT;
        v->val.num = FALSE;
        break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_fndu2(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Find a unit */
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;

    case DILV_UP:
        if (v1->val.ptr)
            v->type = DILV_UP;
        else
            v->type = DILV_NULL; /* not applicable UP */
        break;

    case DILV_NULL:
        v->type = DILV_NULL;
        break;

    default:
        v->type = DILV_ERR;
        break;
    }

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;

    case DILV_SP:
        if (v->type != DILV_ERR)
        {
            if (!v2->val.ptr)
                v->type = DILV_NULL; /* not applicable SP */
            else if (!*((char *)v2->val.ptr))
                v->type = DILV_NULL; /* not applicable SP */
        }
        break;

    case DILV_NULL:
        if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            v->type = DILV_NULL; /* not applicable */
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v3))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;

    case DILV_INT:
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v4))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;

    case DILV_UP:
        break;

    case DILV_NULL: /* null pointer */
        if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            v4->val.ptr = NULL;
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    switch (dil_getval(v5))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;

    case DILV_INT:
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    if (v->type == DILV_UP)
    {
        char *c = (char *)v2->val.ptr;

        v->atyp = DILA_NORM;
        v->val.ptr = find_unit_dil((class unit_data *)v1->val.ptr,
                                   &c, (class unit_data *)v4->val.ptr,
                                   v3->val.num, v5->val.num);

        if (v2->atyp == DILA_NORM && v2->type == DILV_SPR)
            memmove(v2->val.ptr, c, strlen(c) + 1);
        else if (p->sarg->arg == (const char *)v2->val.ptr)
            p->sarg->arg = c; /* Update argument if argument is argument! :) */

        if (!v->val.ptr)
            v->type = DILV_NULL;
    }
    else
        v->type = DILV_FAIL;
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
}

void dilfe_fndu(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Find a unit */
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;

    case DILV_UP:
        if (v1->val.ptr)
            v->type = DILV_UP;
        else
            v->type = DILV_NULL; /* not applicable UP */
        break;

    case DILV_NULL:
        v->type = DILV_NULL;
        break;

    default:
        v->type = DILV_ERR;
        break;
    }

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;

    case DILV_SP:
        if (v->type != DILV_ERR)
        {
            if (!v2->val.ptr)
                v->type = DILV_NULL; /* not applicable SP */
            else if (!*((char *)v2->val.ptr))
                v->type = DILV_NULL; /* not applicable SP */
        }
        break;

    case DILV_NULL:
        if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            v->type = DILV_NULL; /* not applicable */
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v3))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;

    case DILV_INT:
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v4))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;

    case DILV_UP:
        break;

    case DILV_NULL: /* null pointer */
        if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            v4->val.ptr = NULL;
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    if (v->type == DILV_UP)
    {
        char *c = (char *)v2->val.ptr;

        v->atyp = DILA_NORM;
        v->val.ptr = find_unit_dil((class unit_data *)v1->val.ptr,
                                   &c, (class unit_data *)v4->val.ptr,
                                   v3->val.num);

        if (v2->atyp == DILA_NORM && v2->type == DILV_SPR)
            memmove(v2->val.ptr, c, strlen(c) + 1);
        else if (p->sarg->arg == (const char *)v2->val.ptr)
            p->sarg->arg = c; /* Update argument if argument is argument! :) */

        if (!v->val.ptr)
            v->type = DILV_NULL;
    }
    else
        v->type = DILV_FAIL;
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
}

void dilfe_fndru(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Find a unit */
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
        v->type = DILV_FAIL;
        break;
    case DILV_UP:
        if (v1->val.ptr)
            v->type = DILV_UP;
        else
            v->type = DILV_NULL; /* not applicable UP */
        break;
    case DILV_NULL:
        v->type = DILV_NULL;
        break;
    default:
        v->type = DILV_ERR;
        break;
    }

    switch (dil_getval(v2))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;
    case DILV_INT:
        break;

    case DILV_NULL:
        if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            v->type = DILV_NULL; /* not applicable */
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    switch (dil_getval(v3))
    {
    case DILV_FAIL:
        if (v->type != DILV_ERR)
            v->type = DILV_FAIL;
        break;
    case DILV_INT:
        break;
    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }

    if (v->type == DILV_UP)
    {
        v->atyp = DILA_NORM;
        v->val.ptr = random_unit((class unit_data *)v1->val.ptr,
                                 v2->val.num, v3->val.num);

        if (!v->val.ptr)
            v->type = DILV_NULL;
    }
    else
        v->type = DILV_FAIL;

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

// read a fixed string
void dilfe_fs(register class dilprg *p)
{
    dilval *v = new dilval;
    /* A Fixed String */
    v->type = DILV_SP;
    v->atyp = DILA_NORM;

    /* Reference directly into the core */
    /* assumes bwrite_string is same format */
    /* saves LOTS of time! */
    v->val.ptr = p->fp->pc;

    /* just skip single string */
    TAIL(p->fp->pc);

    (p->fp->pc)++; /* ready for next */
    p->stack.push(v);
}

void dilfe_fsl(register class dilprg *p)
{
    dilval *v = new dilval;
    cNamelist *namelist = new cNamelist;
    /* A Fixed String list */

    v->atyp = DILA_EXP; /* no other way, copy namelist from core */
    v->type = DILV_SLP;

    namelist->bread(&(p->fp->pc));
    v->val.ptr = namelist;
    p->stack.push(v);
}

void dilfe_fil(register class dilprg *p)
{
    dilval *v = new dilval;
    cintlist *intlist = new cintlist;
    /* A Fixed Int list */

    v->atyp = DILA_EXP; /* no other way, copy namelist from core */
    v->type = DILV_ILP;

    intlist->bread(&(p->fp->pc));
    v->val.ptr = intlist;
    p->stack.push(v);
}

void dilfe_var(register class dilprg *p)
{
    dilval *v = new dilval;
    /* A variable */
    int varno;

    varno = bread_ubit16(&(p->fp->pc));

    switch (p->fp->vars[varno].type)
    {
    /* normal variable */
    case DILV_UP: /* unit pointer */
        v->atyp = DILA_NORM;
        v->type = DILV_UPR;
        v->ref = &(p->fp->vars[varno].val.unitptr);
        break;
    case DILV_ZP: /* unit pointer */
        v->atyp = DILA_NORM;
        v->type = DILV_ZPR;
        v->ref = &(p->fp->vars[varno].val.zoneptr);
        break;
    case DILV_CP: /* cmd pointer */
        v->atyp = DILA_NORM;
        v->type = DILV_CPR;
        v->ref = &(p->fp->vars[varno].val.cmdptr);
        break;
    case DILV_SP: /* string pointer */
        v->atyp = DILA_NORM;
        v->type = DILV_SPR;
        v->ref = &(p->fp->vars[varno].val.string);
        break;
    case DILV_ILP: /* string list pointer */
        v->atyp = DILA_NORM;
        v->type = DILV_ILPR;
        v->ref = p->fp->vars[varno].val.intlist;
        break;
    case DILV_SLP: /* string list pointer */
        v->atyp = DILA_NORM;
        v->type = DILV_SLPR;
        v->ref = p->fp->vars[varno].val.namelist;
        break;
    case DILV_EDP: /* extra description pointer */
        v->atyp = DILA_NORM;
        v->type = DILV_EDPR;
        v->ref = &(p->fp->vars[varno].val.extraptr);
        break;
    case DILV_INT: /* integer */
        v->atyp = DILA_NONE;
        v->type = DILV_SINT4R;
        v->ref = &(p->fp->vars[varno].val.integer);
        break;
    default:
        assert(FALSE);
    }
    p->stack.push(v);
}

void dilfe_weat(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Self */

    v->type = DILV_INT;
    v->atyp = DILA_NORM;
    v->val.num = unit_zone(p->sarg->owner)->weather.sky;
    p->stack.push(v);
}

void dilfe_self(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Self */

    v->type = DILV_UP;
    v->atyp = DILA_NORM;
    v->val.ptr = p->sarg->owner;
    p->stack.push(v);
}

void dilfe_hrt(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Heartbeat */

    v->type = DILV_SINT2R;
    v->atyp = DILA_NONE;
    v->ref = (sbit16 *)&(p->sarg->fptr->heart_beat);
    p->stack.push(v);
}

void dilfe_tho(register class dilprg *p)
{
    dilval *v = new dilval;
    /* MudHour */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = mud_date().hours;
    p->stack.push(v);
}

void dilfe_tda(register class dilprg *p)
{
    dilval *v = new dilval;
    /* MudDay */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = mud_date().day;
    p->stack.push(v);
}

void dilfe_tmd(register class dilprg *p)
{
    dilval *v = new dilval;
    /* MudMonth */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = mud_date().month;
    p->stack.push(v);
}

void dilfe_tye(register class dilprg *p)
{
    dilval *v = new dilval;
    /* MudYear */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = mud_date().year;
    p->stack.push(v);
}

void dilfe_rti(register class dilprg *p)
{
    dilval *v = new dilval;
    /* RealTime */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = time(0);
    p->stack.push(v);
}

void dilfe_acti(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Activator */

    v->type = DILV_UP;
    v->atyp = DILA_NORM;
    v->val.ptr = p->sarg->activator;
    p->stack.push(v);
}

void dilfe_medi(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Medium */

    v->type = DILV_UP;
    v->atyp = DILA_NORM;
    v->val.ptr = p->sarg->medium;
    p->stack.push(v);
}

void dilfe_targ(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Target */

    v->type = DILV_UP;
    v->atyp = DILA_NORM;
    v->val.ptr = p->sarg->target;
    p->stack.push(v);
}

void dilfe_powe(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Power */

    static int dummy = 0;

    v->type = DILV_SINT4R;
    v->atyp = DILA_NONE;

    if (p->sarg->pInt)
        v->ref = (sbit32 *)p->sarg->pInt;
    else
        v->ref = (int *)&dummy;
    p->stack.push(v);
}

void dilfe_cmst(register class dilprg *p)
{
    dilval *v = new dilval;
    /* cmdstr */

    v->type = DILV_SP;
    v->atyp = DILA_NORM;

    if (p->sarg->cmd->cmd_str)
        v->val.ptr = (char *)p->sarg->cmd->cmd_str;
    else
    {
        v->type = DILV_NULL;
        v->val.ptr = NULL;
    }
    p->stack.push(v);
}

void dilfe_excmst(register class dilprg *p)
{
    dilval *v = new dilval;
    /* cmdstr */

    v->type = DILV_SP;
    v->atyp = DILA_NORM;

    if (p->sarg->cmd->excmd)
        v->val.ptr = (char *)p->sarg->cmd->excmd;
    else
    {
        v->type = DILV_NULL;
        v->val.ptr = NULL;
    }
    p->stack.push(v);
}

void dilfe_excmstc(register class dilprg *p)
{
    dilval *v = new dilval;
    /* excmdc */

    v->type = DILV_SP;
    v->atyp = DILA_NORM;

    if (p->sarg->cmd->excmdc)
        v->val.ptr = (char *)p->sarg->cmd->excmdc;
    else
    {
        v->type = DILV_NULL;
        v->val.ptr = NULL;
    }
    p->stack.push(v);
}

void dilfe_argm(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Argument */

    v->type = DILV_SP;
    v->atyp = DILA_NORM;
    if (p->sarg->arg)
        v->val.ptr = (char *)p->sarg->arg;
    else
    {
        v->type = DILV_NULL;
        v->val.ptr = NULL;
    }
    p->stack.push(v);
}

void dilfe_null(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Pointer value null */

    v->type = DILV_NULL;
    v->atyp = DILA_NONE;
    v->val.ptr = NULL;
    p->stack.push(v);
}

void dilfe_int(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Fixed integer */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = (sbit32)bread_ubit32(&(p->fp->pc));
    p->stack.push(v);
}

void dilfe_cmds(register class dilprg *p)
{
    dilval *v = new dilval;
    /* Check if the input command might the supplied argument */
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    case DILV_SP:
        if (v1->val.ptr)
        {
            v->type = DILV_INT;
            v->atyp = DILA_NONE;
            v->val.num = is_command(p->sarg->cmd, (char *)v1->val.ptr);
        }
        else
            v->type = DILV_FAIL;
        break;

    case DILV_INT:
        v->type = DILV_INT;
        v->val.num = (p->sarg->cmd->no == v1->val.num);
        break;

    default:
        v->type = DILV_ERR; /* wrong type */
        break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_pck(class dilprg *p)
{
    dilval *v = new dilval;
    extern int pay_point_charlie(class unit_data * ch, class unit_data * to); /* from act_movement.c */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
    case DILV_FAIL:
    case DILV_NULL:
        v->type = DILV_FAIL;
        break;
    case DILV_UP:
        if (!v1->val.ptr || !IS_CHAR((class unit_data *)v1->val.ptr))
            v->type = DILV_FAIL;
        else
            switch (dil_getval(v2))
            {
            case DILV_FAIL:
            case DILV_NULL:
                v->type = DILV_FAIL;
                break;
            case DILV_UP:
                if (!v2->val.ptr)
                    v->type = DILV_FAIL;
                else
                    v->val.num = pay_point_charlie((class unit_data *)v1->val.ptr,
                                                   (class unit_data *)v2->val.ptr);
                break;
            default:
                v->type = DILV_ERR;
                break;
            }

        break;
    default:
        v->type = DILV_ERR;
        break;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_act(register class dilprg *p)
{
    dilval *v = new dilval;
    char buf[1024];
    /* Conversion of integers to strings */
    dilval *v6 = p->stack.pop();
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_FAIL;

    if (dil_type_check("act", p, 6,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, TYPEFAIL_NULL, 1, DILV_INT,
                       v3, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL,
                       v4, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL,
                       v5, TYPEFAIL_NULL, 3, DILV_UP, DILV_SP, DILV_NULL,
                       v6, TYPEFAIL_NULL, 1, DILV_INT))
        switch (v6->val.num)
        {
        case TO_CHAR:
        case TO_ROOM:
        case TO_ALL:
        case TO_REST:
            /* these require 1st argument */
            if (v3->val.ptr)
                act_generate(buf, (char *)v1->val.ptr, v2->val.num, v3, v4, v5, 
                             v6->val.num, (class unit_data *)v3->val.ptr);
            v->type = DILV_SP;
            v->atyp = DILA_EXP;
            v->val.ptr = str_dup(buf);
            break;

        case TO_VICT:
        case TO_NOTVICT:
            if (v5->val.ptr)
                act_generate(buf, (char *)v1->val.ptr, v2->val.num, v3, v4, v5, 
                             v6->val.num, (class unit_data *)v5->val.ptr);
            v->type = DILV_SP;
            v->atyp = DILA_EXP;
            v->val.ptr = str_dup(buf);
            break;
        }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
}
