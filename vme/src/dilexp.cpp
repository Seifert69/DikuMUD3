
/*
$Author: All $
$RCSfile: dilexp.cpp,v $
$Date: 2005/06/28 20:17:48 $
$Revision: 2.18 $
*/
/* Per https://sourceforge.net/p/predef/wiki/OperatingSystems/, this identifies
 *  Mac OS X. This is needed since OS X doesn't have crypt.h and instead uses
 *  unistd.h for these mappings. */
#if defined __APPLE__ && __MACH__
    #include <unistd.h>
#endif
#ifdef _WINDOWS
    #include <direct.h>
#endif

#include "affect.h"
#include "cmdload.h"
#include "comm.h"
#include "common.h"
#include "db.h"
#include "dbfind.h"
#include "dil.h"
#include "dilexp.h"
#include "dilinst.h"
#include "dilrun.h"
#include "dilsup.h"
#include "fight.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "intlist.h"
#include "justice.h"
#include "magic.h"
#include "modify.h"
#include "money.h"
#include "path.h"
#include "pcsave.h"
#include "reception.h"
#include "skills.h"
#include "spell_parser.h"
#include "spells.h"
#include "structs.h"
#include "szonelog.h"
#include "textutil.h"
#include "trie.h"
#include "unit_fptr.h"
#include "utils.h"
#include "vmelimits.h"
#include "weather.h"

#include <crypt.h>
#include <sys/stat.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>

/* ************************************************************************ */
/* DIL-expressions							    */
/* ************************************************************************ */

void dilfe_illegal(dilprg *p)
{
    szonelog(UNIT_FI_ZONE(p->sarg->owner),
             "DIL %s@%s, Illegal Expression/Instruction Node.\n",
             UNIT_FI_NAME(p->sarg->owner),
             UNIT_FI_ZONENAME(p->sarg->owner));
    p->waitcmd = WAITCMD_QUIT;
}

/* replace all old with new in  string 3
   replace(old, new, string 3)*/
void dilfe_replace(dilprg *p)
{
    dilval *v = new dilval;
    // char *buf;
    // int olen, nlen, buflen, i;
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
                                std::string sch((char *)v1->val.ptr);
                                std::string rpl((char *)v2->val.ptr);
                                std::string mystr((char *)v3->val.ptr);

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
                            {
                                v->type = DILV_FAIL;
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

void dilfe_tolower(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char *s1 = nullptr;

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
            {
                v->val.ptr = nullptr;
            }
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

void dilfe_toupper(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char *s1 = nullptr;

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
            {
                v->val.ptr = nullptr;
            }
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

void dilfe_left(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int strl = 0;
    int x = 0;

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
                        v->val.ptr = nullptr;
                        break;
                    }
                    if ((int)strlen((char *)v1->val.ptr) < v2->val.num)
                    {
                        strl = strlen((char *)v1->val.ptr);
                    }
                    else
                    {
                        strl = v2->val.num;
                    }

                    if (str_is_empty((char *)v1->val.ptr) || strl == 0)
                    {
                        v->val.ptr = nullptr;
                    }
                    else
                    {
                        v->atyp = DILA_EXP;
                        v->val.ptr = (char *)malloc(strl + 1);
                        for (x = 0; x < strl; x++)
                        {
                            ((char *)v->val.ptr)[x] = ((char *)v1->val.ptr)[x];
                        }
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

void dilfe_right(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int strl = 0;
    int x = 0;
    int y = 0;

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
                        v->val.ptr = nullptr;
                        break;
                    }

                    if ((int)strlen((char *)v1->val.ptr) < v2->val.num)
                    {
                        strl = strlen((char *)v1->val.ptr);
                    }
                    else
                    {
                        strl = v2->val.num;
                    }

                    if (str_is_empty((char *)v1->val.ptr) || strl == 0)
                    {
                        v->val.ptr = nullptr;
                    }
                    else
                    {
                        v->atyp = DILA_EXP;
                        v->val.ptr = malloc(strl + 1);
                        for (x = 0, y = strlen((char *)v1->val.ptr) - strl; x < strl; y++, x++)
                        {
                            ((char *)v->val.ptr)[x] = ((char *)v1->val.ptr)[y];
                        }
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

void dilfe_mid(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int strl = 0;
    int l = 0;
    int r = 0;
    int x = 0;

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
                                v1->val.ptr = nullptr;
                                break;
                            }
                            else
                            {
                                l = v2->val.num;
                            }

                            if ((v2->val.num >= 0) && ((int)strlen((char *)v1->val.ptr) < v3->val.num))
                            {
                                r = strlen((char *)v1->val.ptr);
                            }
                            else
                            {
                                r = v3->val.num;
                            }

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
                            {
                                v->val.ptr = nullptr;
                            }
                            else
                            {
                                v->atyp = DILA_EXP;
                                v->val.ptr = malloc(strl + 1);
                                for (x = 0; l <= r; l++, x++)
                                {
                                    ((char *)v->val.ptr)[x] = ((char *)v1->val.ptr)[l - 1];
                                }
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
void dilfe_islt(dilprg *p)
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
            {
                v->type = DILV_FAIL;
            }
            else
            {
                if (UNIT_IS_LIGHT((unit_data *)v1->val.ptr))
                {
                    v->val.num = 1;
                }
                else
                {
                    v->val.num = 0;
                }
            }
            break;
        default:
            v->type = DILV_ERR;
            break;
    }

    p->stack.push(v);
    delete v1;
}

void dilfe_ghead(dilprg *p)
{
    dilval *v = new dilval;
    v->atyp = DILA_NORM;
    v->type = DILV_UP;
    v->val.ptr = g_unit_list;
    p->stack.push(v);
}

void dilfe_phead(dilprg *p)
{
    dilval *v = new dilval;
    if (IS_PC(g_unit_list))
    {
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        v->val.ptr = g_unit_list;
    }
    else
    {
        v->type = DILV_NULL;
    }
    p->stack.push(v);
}

void dilfe_ohead(dilprg *p)
{
    dilval *v = new dilval;
    if (IS_OBJ(g_obj_head))
    {
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        v->val.ptr = g_obj_head;
    }
    else
    {
        v->type = DILV_NULL;
    }
    p->stack.push(v);
}

void dilfe_nhead(dilprg *p)
{
    dilval *v = new dilval;
    if (IS_NPC(g_npc_head))
    {
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        v->val.ptr = g_npc_head;
    }
    else
    {
        v->type = DILV_NULL;
    }
    p->stack.push(v);
}

void dilfe_rhead(dilprg *p)
{
    dilval *v = new dilval;
    if (IS_ROOM(g_room_head))
    {
        v->atyp = DILA_NORM;
        v->type = DILV_UP;
        v->val.ptr = g_room_head;
    }
    else
    {
        v->type = DILV_NULL;
    }
    p->stack.push(v);
}

void dilfe_zhead(dilprg *p)
{
    dilval *v = new dilval;
    v->atyp = DILA_NORM;
    v->type = DILV_ZP;
    v->val.ptr = g_zone_info.mmp.begin()->second;
    p->stack.push(v);
}

void dilfe_chead(dilprg *p)
{
    dilval *v = new dilval;
    v->atyp = DILA_NORM;
    v->type = DILV_CP;
    v->val.ptr = g_cmdlist;
    p->stack.push(v);
}

void dilfe_clr(dilprg *p)
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

void dilfe_sendpre(dilprg *p)
{
    dilval *v = new dilval;
    command_info *cmd = nullptr;
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
                                                            cmd = ((command_info *)search_trie((char *)v1->val.ptr, g_intr_trie));
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
                                                                v->val.num = send_ack((unit_data *)v2->val.ptr,
                                                                                      (unit_data *)v3->val.ptr,
                                                                                      (unit_data *)v4->val.ptr,
                                                                                      &(v5->val.num),
                                                                                      cmd,
                                                                                      (const char *)v6->val.ptr,
                                                                                      (unit_data *)v7->val.ptr);
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

void dilfe_clradd(dilprg *p)
{
    dilval *v = new dilval;

    char full_name[21];
    char *color = nullptr;
    unsigned int x = 0;
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
                                if ((!isalnum(*((char *)v2->val.ptr + x))) && (*((char *)v2->val.ptr + x) != '_'))
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
                                if (UPC((unit_data *)v1->val.ptr)->color.get((char *)v2->val.ptr, full_name).empty() == false)
                                {
                                    v->type = DILV_INT;
                                    v->val.num = FALSE;
                                }
                                else
                                {
                                    color = str_escape_format((char *)v3->val.ptr, FALSE);
                                    auto unused = UPC((unit_data *)v1->val.ptr)->color.insert((char *)v2->val.ptr, color);
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

void dilfe_clrchg(dilprg *p)
{
    dilval *v = new dilval;

    char full_name[21];
    char *color = nullptr;
    unsigned int x = 0;
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
                                if ((!isalnum(*((char *)v2->val.ptr + x))) && (*((char *)v2->val.ptr + x) != '_'))
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
                                if (UPC((unit_data *)v1->val.ptr)->color.get((char *)v2->val.ptr, full_name).empty() == false)
                                {
                                    color = str_escape_format((char *)v3->val.ptr, FALSE);
                                    auto unused = UPC((unit_data *)v1->val.ptr)->color.change((char *)v2->val.ptr, color);
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

void dilfe_clrdel(dilprg *p)
{
    dilval *v = new dilval;

    char full_name[21];
    unsigned int x = 0;
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
                        if ((!isalnum(*((char *)v2->val.ptr + x))) && (*((char *)v2->val.ptr + x) != '_'))
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
                        if (UPC((unit_data *)v1->val.ptr)->color.get((char *)v2->val.ptr, full_name).empty() == false)
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

void dilfe_ckpwd(dilprg *p)
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
                    if (pwdcompare(crypt((char *)v2->val.ptr, PC_FILENAME((unit_data *)v1->val.ptr)),
                                   PC_PWD((unit_data *)v1->val.ptr),
                                   PC_MAX_PASSWORD))
                    {
                        v->val.num = FALSE;
                    }
                    else
                    {
                        v->val.num = TRUE;
                    }

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

void dilfe_atsp(dilprg *p)
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
                    if (!v2->val.ptr || !IS_CHAR((unit_data *)v2->val.ptr))
                    {
                        v->type = DILV_FAIL;
                    }
                    else
                    {
                        switch (dil_getval(v3))
                        {
                            case DILV_UP:
                                if (!v3->val.ptr)
                                {
                                    v->type = DILV_FAIL;
                                }
                                else
                                {
                                    switch (dil_getval(v4))
                                    {
                                        case DILV_UP:
                                            if (!v4->val.ptr)
                                            {
                                                v->type = DILV_FAIL;
                                            }
                                            else
                                            {
                                                switch (dil_getval(v5))
                                                {
                                                    case DILV_INT:

                                                        switch (dil_getval(v6))
                                                        {
                                                            case DILV_SP:
                                                                if (is_in(v1->val.num, SPL_GROUP_MAX, SPL_TREE_MAX - 1) &&
                                                                    (g_spell_info[v1->val.num].spell_pointer ||
                                                                     g_spell_info[v1->val.num].tmpl))
                                                                {
                                                                    spell_args sa;

                                                                    set_spellargs(&sa,
                                                                                  (unit_data *)v2->val.ptr,
                                                                                  (unit_data *)v3->val.ptr,
                                                                                  (unit_data *)v4->val.ptr,
                                                                                  nullptr,
                                                                                  0);
                                                                    if (str_is_empty((char *)v6->val.ptr) ||
                                                                        ((char *)v6->val.ptr == nullptr))
                                                                    {
                                                                        sa.pEffect = nullptr;
                                                                    }
                                                                    else
                                                                    {
                                                                        sa.pEffect = str_dup((char *)v6->val.ptr);
                                                                    }

                                                                    /* cast the spell */
                                                                    v->val.num = spell_offensive(&sa, v1->val.num, v5->val.num);

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

void dilfe_cast2(dilprg *p)
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
                    if (!v2->val.ptr || !IS_CHAR((unit_data *)v2->val.ptr))
                    {
                        v->type = DILV_FAIL;
                    }
                    else
                    {
                        switch (dil_getval(v3))
                        {
                            case DILV_UP:
                                if (!v3->val.ptr)
                                {
                                    v->type = DILV_FAIL;
                                }
                                else
                                {
                                    switch (dil_getval(v4))
                                    {
                                        case DILV_UP:
                                            if (!v4->val.ptr)
                                            {
                                                v->type = DILV_FAIL;
                                            }
                                            else
                                            {
                                                switch (dil_getval(v5))
                                                {
                                                    case DILV_SP:
                                                        if (is_in(v1->val.num, SPL_GROUP_MAX, SPL_TREE_MAX - 1) &&
                                                            (g_spell_info[v1->val.num].spell_pointer || g_spell_info[v1->val.num].tmpl))
                                                        {
                                                            /* cast the spell */
                                                            v->val.num = spell_perform(v1->val.num,
                                                                                       MEDIA_SPELL,
                                                                                       (unit_data *)v2->val.ptr,
                                                                                       (unit_data *)v3->val.ptr,
                                                                                       (unit_data *)v4->val.ptr,
                                                                                       "",
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

void dilfe_resta(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char filename[512];

    v->type = DILV_UP;

    switch (dil_getval(v1))
    {
        case DILV_SP:
            switch (dil_getval(v2))
            {
                case DILV_UP:
                case DILV_NULL:
                    if (p->frame[0].tmpl->zone->getAccessLevel() > 10)
                    {
                        szonelog(p->frame->tmpl->zone,
                                 "DIL '%s' attempt to violate system access security (restorall).",
                                 p->frame->tmpl->prgname);
                        p->waitcmd = WAITCMD_QUIT;
                        break;
                    }

                    if (!store_name_test((char *)v1->val.ptr))
                    {
                        szonelog(p->frame->tmpl->zone, "DIL '%s' attempted to restore an illegal file name.", p->frame->tmpl->prgname);
                        v->val.ptr = nullptr;
                        break;
                    }

                    if (str_is_empty(p->frame[0].tmpl->zone->getDILFilePath()))
                    {
                        strcpy(filename, g_cServerConfig.getDILFileDir().c_str());
                    }
                    else
                    {
                        strcpy(filename, p->frame[0].tmpl->zone->getDILFilePath());
                    }
                    strcat(filename, "/units/");
                    strcat(filename, (char *)v1->val.ptr);
                    if (v2->val.ptr)
                    {
                        v->val.ptr = restore_all_unit(filename, (unit_data *)v2->val.ptr);
                    }
                    else
                    {
                        v->val.ptr = restore_all_unit(filename, p->owner);
                    }
                    if (v->val.ptr == nullptr)
                    {
                        v->type = DILV_NULL;
                    }
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

void dilfe_opro(dilprg *p)
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
                    if ((v1->val.num > 0) && (v2->val.num > 0) && (v2->val.num < v1->val.num / 2 - 1))
                    {
                        v->val.num = open_ended_roll(v1->val.num, v2->val.num);
                    }
                    else
                    {
                        v->val.num = 0;
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
}

void dilfe_eqpm(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_UP;

    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_INT:
                        v->val.ptr = equipment((unit_data *)v1->val.ptr, v2->val.num);
                        if (v->val.ptr == nullptr)
                        {
                            v->type = DILV_NULL;
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
void dilfe_mel(dilprg *p)
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
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_UP:
                        if (!v2->val.ptr || !IS_CHAR((unit_data *)v2->val.ptr))
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            switch (dil_getval(v3))
                            {
                                case DILV_INT:
                                    switch (dil_getval(v4))
                                    {
                                        case DILV_INT:
                                            v->val.num = one_hit((unit_data *)v1->val.ptr,
                                                                 (unit_data *)v2->val.ptr,
                                                                 v3->val.num,
                                                                 v4->val.num,
                                                                 TRUE,
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
void dilfe_meldam(dilprg *p)
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
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_UP:
                        if (!v2->val.ptr || !IS_CHAR((unit_data *)v2->val.ptr))
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            switch (dil_getval(v3))
                            {
                                case DILV_INT:
                                    switch (dil_getval(v4))
                                    {
                                        case DILV_INT:
                                            v->val.num = one_hit((unit_data *)v1->val.ptr,
                                                                 (unit_data *)v2->val.ptr,
                                                                 v3->val.num,
                                                                 v4->val.num,
                                                                 TRUE,
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

void dilfe_flog(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *sstr = nullptr;
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
                            if (p->frame[0].tmpl->zone->getAccessLevel() > 1)
                            {
                                szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to access logs w/o access.", p->frame->tmpl->prgname);
                                p->waitcmd = WAITCMD_QUIT;
                            }
                            else
                            {
                                strcpy(filename, g_cServerConfig.getLogDir().c_str());
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

// loadstr()
void dilfe_ldstr(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *sstr = nullptr;
    char filename[512];

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
        case DILV_SP:
            switch (dil_getval(v2))
            {
                case DILV_SP:
                    if (p->frame[0].tmpl->zone->getAccessLevel() > 10)
                    {
                        szonelog(p->frame->tmpl->zone,
                                 "DIL '%s' attempt to violate system access security (loadstr).",
                                 p->frame->tmpl->prgname);
                        p->waitcmd = WAITCMD_QUIT;
                    }
                    else if (!store_name_test((char *)v1->val.ptr))
                    {
                        szonelog(p->frame->tmpl->zone, "DIL '%s' attempted to loadstr n illegal file name.", p->frame->tmpl->prgname);
                        v->val.num = FALSE;
                        ;
                    }
                    else
                    {
                        sstr = nullptr;
                        if (p->frame[0].tmpl->zone->getDILFilePath())
                        {
                            strcpy(filename, p->frame[0].tmpl->zone->getDILFilePath());
                        }
                        else
                        {
                            strcpy(filename, g_cServerConfig.getDILFileDir().c_str());
                        }
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
                        {
                            v2->val.ptr = nullptr;
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

void dilfe_delstr(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    int ret = 0;
    char filename[512];

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
        case DILV_SP:

            if (p->frame[0].tmpl->zone->getAccessLevel() > 10)
            {
                szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to violate system access security (delstr).", p->frame->tmpl->prgname);
                p->waitcmd = WAITCMD_QUIT;
            }
            else if (!store_name_test((char *)v1->val.ptr))
            {
                szonelog(p->frame->tmpl->zone, "DIL '%s' attempted to delstr an illegal file name.", p->frame->tmpl->prgname);
                v->val.num = FALSE;
            }
            else
            {
                if (str_is_empty(p->frame[0].tmpl->zone->getDILFilePath()))
                {
                    strcpy(filename, g_cServerConfig.getDILFileDir().c_str());
                }
                else
                {
                    strcpy(filename, p->frame[0].tmpl->zone->getDILFilePath());
                }
                strcat(filename, "/strings/");
                strcat(filename, (char *)v1->val.ptr);
                ret = remove(filename);
                if (ret)
                {
                    v->val.num = TRUE;
                }
                else
                {
                    v->val.num = FALSE;
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

    p->stack.push(v);
    delete v1;
}

void dilfe_delunit(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    int ret = 0;
    char filename[512];

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
        case DILV_SP:
            if (p->frame[0].tmpl->zone->getAccessLevel() > 10)
            {
                szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to violate system access security (delunit).", p->frame->tmpl->prgname);
                p->waitcmd = WAITCMD_QUIT;
            }
            else if (!store_name_test((char *)v1->val.ptr))
            {
                szonelog(p->frame->tmpl->zone, "DIL '%s' attempted to delunit an illegal file name.", p->frame->tmpl->prgname);
                v->val.num = FALSE;
            }
            else
            {
                if (str_is_empty(p->frame[0].tmpl->zone->getDILFilePath()))
                {
                    strcpy(filename, g_cServerConfig.getDILFileDir().c_str());
                }
                else
                {
                    strcpy(filename, p->frame[0].tmpl->zone->getDILFilePath());
                }
                strcat(filename, "/units/");
                strcat(filename, (char *)v1->val.ptr);
                ret = remove(filename);
                if (ret)
                {
                    v->val.num = TRUE;
                }
                else
                {
                    v->val.num = FALSE;
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

    p->stack.push(v);
    delete v1;
}

/* savestr - write a string to a file */
void dilfe_svstr(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *sstr = nullptr;
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
                            if (p->frame[0].tmpl->zone->getAccessLevel() > 10)
                            {
                                szonelog(p->frame->tmpl->zone,
                                         "DIL '%s' attempt to violate system access security (savestr).",
                                         p->frame->tmpl->prgname);
                                p->waitcmd = WAITCMD_QUIT;
                            }
                            else if (str_is_empty((char *)v1->val.ptr) || str_is_empty((char *)v2->val.ptr) ||
                                     str_is_empty((char *)v3->val.ptr))
                            {
                                v->type = DILV_FAIL;
                            }
                            else
                            {
                                if (str_is_empty(p->frame[0].tmpl->zone->getDILFilePath()))
                                {
                                    if (!file_exists(g_cServerConfig.getDILFileDir()))
                                    {
#ifdef _WINDOWS
                                        _mkdir(g_cServerConfig.m_dilfiledir);
#else
                                        mkdir(g_cServerConfig.getDILFileDir().c_str(),
                                              S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
#endif
                                    }
                                    strcpy(filename, g_cServerConfig.getDILFileDir().c_str());
                                }
                                else
                                {
                                    if (!file_exists(p->frame[0].tmpl->zone->getDILFilePath()))
                                    {
#ifdef _WINDOWS
                                        _mkdir(p->frame[0].tmpl->zone->getDILFilePath());
#else
                                        mkdir(p->frame[0].tmpl->zone->getDILFilePath(),
                                              S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
#endif
                                    }
                                    strcpy(filename, p->frame[0].tmpl->zone->getDILFilePath());
                                }
                                strcat(filename, "/strings/");
                                if (!file_exists(filename))
                                {
#ifdef _WINDOWS
                                    _mkdir(filename);
#else
                                    mkdir(filename, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
#endif
                                }
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

void dilfe_filesz(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char filename[511];
    FILE *fp = nullptr;

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
        case DILV_SP:
            if (str_is_empty(p->frame[0].tmpl->zone->getDILFilePath()))
            {
                if (!file_exists(g_cServerConfig.getDILFileDir()))
                {
#ifdef _WINDOWS
                    _mkdir(g_cServerConfig.m_dilfiledir);
#else
                    mkdir(g_cServerConfig.getDILFileDir().c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
#endif
                }
                strcpy(filename, g_cServerConfig.getDILFileDir().c_str());
            }
            else
            {
                if (!file_exists(p->frame[0].tmpl->zone->getDILFilePath()))
                {
#ifdef _WINDOWS
                    _mkdir(p->frame[0].tmpl->zone->getDILFilePath());
#else
                    mkdir(p->frame[0].tmpl->zone->getDILFilePath(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
#endif
                }

                strcpy(filename, p->frame[0].tmpl->zone->getDILFilePath());
            }
            strcat(filename, "/strings/");
            if (!file_exists(filename))
            {
#ifdef _WINDOWS
                _mkdir(filename);
#else
                mkdir(filename, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
#endif
            }
            strcat(filename, (char *)v1->val.ptr);
            if ((fp = fopen(filename, "r")) == nullptr)
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
void dilfe_visi(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_UP:
                        if (!v2->val.ptr)
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            v->val.num = CHAR_CAN_SEE((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr);
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
void dilfe_oppo(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_UP:
                        if (!v2->val.ptr || !IS_CHAR((unit_data *)v2->val.ptr))
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            v->val.num = (CHAR_COMBAT((unit_data *)v1->val.ptr)
                                              ? CHAR_COMBAT((unit_data *)v1->val.ptr)->FindOpponent((unit_data *)v2->val.ptr) != nullptr
                                              : FALSE);
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
void dilfe_gopp(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_INT:

                        if (v2->val.num >= 0)
                        {
                            if (CHAR_COMBAT((unit_data *)v1->val.ptr))
                            {
                                v->val.ptr = CHAR_COMBAT((unit_data *)v1->val.ptr)->Opponent((int)v2->val.num);
                                if (v->val.ptr)
                                {
                                    v->atyp = DILA_NORM;
                                    v->type = DILV_UP;
                                }
                                else
                                {
                                    v->val.ptr = nullptr;
                                    v->atyp = DILA_NORM;
                                    v->type = DILV_NULL;
                                }
                            }
                        }
                        else
                        {
                            v->val.ptr = nullptr;
                            v->atyp = DILA_NORM;
                            v->type = DILV_NULL;
                        }
                        break;
                    default:
                        v->type = DILV_ERR;
                        break;
                }
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
void dilfe_gfol(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_INT:
                        if (v2->val.num < 0)
                        {
                            v->val.ptr = nullptr;
                            v->atyp = DILA_NORM;
                            v->type = DILV_NULL;
                        }
                        else
                        {
                            v->val.ptr = get_follower((unit_data *)v1->val.ptr, v2->val.num);
                            if (v->val.ptr)
                            {
                                v->atyp = DILA_NORM;
                                v->type = DILV_UP;
                            }
                            else
                            {
                                v->val.ptr = nullptr;
                                v->atyp = DILA_NORM;
                                v->type = DILV_NULL;
                            }
                        }
                        break;
                    default:
                        v->type = DILV_ERR;
                        break;
                }
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
void dilfe_splx(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
        case DILV_SP:
            if (!v1->val.ptr)
            {
                v->type = DILV_FAIL;
            }
            else
            {
                const char *c = (const char *)v1->val.ptr;
                if (str_is_empty(c))
                {
                    v->val.num = -1;
                }
                else
                {
                    v->val.num = search_block_abbrevs(c, g_SplColl.text, &c);
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

    p->stack.push(v);
    delete v1;
}

/* spellinfo */
void dilfe_spli(dilprg *p)
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
            {
                v->type = DILV_FAIL;
            }
            else
            {
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
                                                                        /* pName := spellinfo(SPL_LIGHTNING_3, nRealm, nSphere, nMana,
                                                                         * bOffensive, nResistType, bvMedia, bvTargets); */

                                                                        /* We're home free... */

                                                                        *((ubit32 *)v2->ref) = g_spell_info[v1->val.num].realm;
                                                                        *((ubit32 *)v3->ref) = g_SplColl.tree[v1->val.num].parent;
                                                                        *((ubit32 *)v4->ref) = g_spell_info[v1->val.num].usesmana;
                                                                        *((ubit32 *)v5->ref) = g_spell_info[v1->val.num].offensive;
                                                                        *((ubit32 *)v6->ref) = g_spell_info[v1->val.num].cast_type;
                                                                        *((ubit32 *)v7->ref) = g_spell_info[v1->val.num].media;
                                                                        *((ubit32 *)v8->ref) = g_spell_info[v1->val.num].targets;

                                                                        v->type = DILV_SP;
                                                                        v->atyp = DILA_EXP;
                                                                        v->val.ptr = str_dup(g_SplColl.text[v1->val.num] == nullptr
                                                                                                 ? ""
                                                                                                 : g_SplColl.text[v1->val.num]);
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
void dilfe_purs(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (!v1->val.ptr)
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_SP:
                        if ((!v2->val.ptr) || (!*((char *)v2->val.ptr)))
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            int i = 0;

                            v->val.num = 0;

                            for (i = 0; i <= MAX_MONEY; i++)
                            {
                                if (strcmp((char *)v2->val.ptr, g_money_types[i].abbrev) == 0)
                                {
                                    break;
                                }
                            }

                            if (i <= MAX_MONEY)
                            {
                                /* Note down money-objects in from, and their values */
                                for (unit_data *tmp = UNIT_CONTAINS((unit_data *)v1->val.ptr); tmp; tmp = tmp->next)
                                {
                                    if (IS_MONEY(tmp) && MONEY_TYPE(tmp) == i)
                                    {
                                        v->val.num = MONEY_AMOUNT(tmp);
                                        break;
                                    }
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
void dilfe_mons(dilprg *p)
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
                    v->val.ptr = str_dup(money_string(v1->val.num, DEF_CURRENCY, v2->val.num));
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
void dilfe_path(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (!v1->val.ptr)
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_UP:
                        if (!v2->val.ptr)
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            unit_data *u1 = nullptr;
                            unit_data *u2 = nullptr;
                            u1 = unit_room((unit_data *)v1->val.ptr);
                            u2 = unit_room((unit_data *)v2->val.ptr);

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
void dilfe_cary(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;

    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_UP:
                        if (!v2->val.ptr)
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            switch (dil_getval(v3))
                            {
                                case DILV_INT:
                                    if (v3->val.num < 1)
                                    {
                                        v->type = DILV_FAIL;
                                    }
                                    else
                                    {
                                        if (!char_can_carry_n((unit_data *)v1->val.ptr, v3->val.num))
                                        {
                                            v->val.num = 1;
                                        }
                                        else if (!char_can_carry_w((unit_data *)v1->val.ptr,
                                                                   v3->val.num * UNIT_WEIGHT((unit_data *)v2->val.ptr)))
                                        {
                                            v->val.num = 2;
                                        }
                                        else
                                        {
                                            v->val.num = 0;
                                        }
                                    }
                                    break;
                                default:
                                    v->type = DILV_ERR;
                                    break;
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
void dilfe_trmo(dilprg *p)
{
    dilval *v = new dilval;
    char buf[1024];
    int i = 0;
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
            if (v1->val.ptr && !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_FAIL:
                        v->type = DILV_FAIL;
                        break;
                    case DILV_NULL:
                    case DILV_UP:
                        if (v2->val.ptr && !IS_CHAR((unit_data *)v2->val.ptr))
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
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
                        }

                        break;
                    default:
                        v->type = DILV_ERR;
                        break;
                }
            }
            break;
        default:
            v->type = DILV_ERR;
            break;
    }

    v->val.num = 0;

    if (v->type == DILV_INT && (v1->val.ptr || v2->val.ptr))
    {
        if (v1->val.ptr == nullptr)
        {
            for (i = 0; i <= MAX_CURRENCY; ++i)
            {
                strcat(buf, " ");
                strcat(buf, money_string(v3->val.num, i, TRUE));
            }

            slog(LOG_ALL,
                 0,
                 "%s was given %s by DIL %s@%s.",
                 UNIT_NAME((unit_data *)v2->val.ptr),
                 buf,
                 UNIT_FI_NAME(p->sarg->owner),
                 UNIT_FI_ZONENAME(p->sarg->owner));
            money_transfer(nullptr, (unit_data *)v2->val.ptr, v3->val.num, local_currency((unit_data *)v2->val.ptr));
            v->val.num = 1;
        }
        else if (v2->val.ptr == nullptr)
        {
            if (char_can_afford((unit_data *)v1->val.ptr, v3->val.num, local_currency((unit_data *)v1->val.ptr)))
            {
                money_transfer((unit_data *)v1->val.ptr, nullptr, v3->val.num, local_currency((unit_data *)v1->val.ptr));
                v->val.num = 1;
            }
        }
        else
        {
            if (char_can_afford((unit_data *)v1->val.ptr, v3->val.num, local_currency((unit_data *)v2->val.ptr)))
            {
                money_transfer((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr, v3->val.num, local_currency((unit_data *)v2->val.ptr));
                v->val.num = 1;
            }
        }
    }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_fits(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *c = nullptr;

    v->type = DILV_SP;

    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_UP:
                        if (!v2->val.ptr || !IS_OBJ((unit_data *)v2->val.ptr))
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            switch (dil_getval(v3))
                            {
                                case DILV_INT:
                                    c = obj_wear_size((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr, v3->val.num);

                                    v->atyp = DILA_EXP;
                                    v->val.ptr = str_dup(c == nullptr ? "" : c);
                                    break;
                                default:
                                    v->type = DILV_ERR;
                                    break;
                            }
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

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

void dilfe_intr(dilprg *p)
{
    dilval *v = new dilval;
    /* add interrupt to current frame */
    ubit16 intnum = 0;
    ubit8 *beg = nullptr;
    ubit8 *end = nullptr;
    ubit32 endptr = 0;
    ubit32 begptr = 0;

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

void dilfe_not(dilprg *p)
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

void dilfe_umin(dilprg *p)
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

void dilfe_skitxt(dilprg *p)
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
            {
                v->val.ptr = str_dup(g_SkiColl.text[v1->val.num]);
            }
            else
            {
                v->val.ptr = str_dup("");
            }
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_wpntxt(dilprg *p)
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
                {
                    v->val.ptr = str_dup(g_WpnColl.text[v1->val.num]);
                }
                else
                {
                    v->val.ptr = str_dup("");
                }
            }
            else
            {
                v->val.ptr = nullptr; // OOB
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

void dilfe_itoa(dilprg *p)
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

void dilfe_atoi(dilprg *p)
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
            {
                v->type = DILV_FAIL; /* failed */
            }
            break;
        default:
            v->type = DILV_ERR; /* Wrong type */
            break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_isplayer(dilprg *p)
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
                if (find_player_id((char *)v1->val.ptr) != -1)
                {
                    v->val.num = TRUE;
                }
                else
                {
                    v->val.num = FALSE;
                }
            }
            else
            {
                v->type = DILV_FAIL; /* failed */
            }
            break;
        default:
            v->type = DILV_ERR; /* Wrong type */
            break;
    }

    p->stack.push(v);
    delete v1;
}

void *threadcallout(void *p)
{
    const char *str = (char *)p;

    // Process the string here, to do some basic security.
    char buf[MAX_STRING_LENGTH];
    int ok = true;

    str_next_word_copy(str, buf);

    if (strchr(buf, '/') || strchr(buf, ' ') || strchr(buf, ';') || strstr(buf, ".."))
    {
        slog(LOG_BRIEF, 0, "bin command %s contains an illegal character (/ ;) or .. ", buf);
        ok = false;
    }

    if (ok)
    {
        std::string s{"./allow/"}; // current dir iswhere vme/bin is located, set to bin/allow/
        s += str;
        slog(LOG_BRIEF, 0, "system('%s'); ", s);
        int rc = ::system((const char *)s.c_str());

        if (rc == -1 || WEXITSTATUS(rc) != 0)
        {
            slog(LOG_BRIEF, 0, "fail system('%s') rc=%d exitstatus=%d", str, rc, WEXITSTATUS(rc));
        }
        else
        {
            slog(LOG_BRIEF, 0, "success system('%s') rc=%d exitstatus=%d", str, rc, WEXITSTATUS(rc));
        }
    }

    pthread_exit(nullptr);
}

// DIL shell()
void dilfe_shell(dilprg *p)
{
    dilval *v = new dilval;
    /* Get the ID number of a player. */
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
                if (p->frame[0].tmpl->zone->getAccessLevel() != 0) // 0 is the highest access
                {
                    szonelog(p->frame->tmpl->zone, "DIL '%s' attempt run shell() w/o access.", p->frame->tmpl->prgname);
                    p->waitcmd = WAITCMD_QUIT;
                }
                else
                {
                    pthread_t mythread = 0;
                    v->val.num = pthread_create(&mythread, nullptr, threadcallout, v1->val.ptr);
                    if (v->val.num)
                    {
                        slog(LOG_ALL, 0, "DIL shell pthread create error code %d.", v->val.num);
                    }
                    // threadcallout((char *) v1->val.ptr);
                }
            }
            break;
        default:
            v->type = DILV_ERR; /* Wrong type */
            break;
    }

    p->stack.push(v);
    delete v1;
}

void dilfe_len(dilprg *p)
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
            {
                v->val.num = strlen((char *)v1->val.ptr);
            }
            else
            {
                v->val.num = 0;
            }
            break;
        case DILV_SLP:
            v->atyp = DILA_NONE;
            v->type = DILV_INT;
            if (v1->val.ptr)
            {
                v->val.num = ((cNamelist *)v1->val.ptr)->Length();
            }
            else
            {
                v->val.num = 0;
            }
            break;
        case DILV_ILP:
            v->atyp = DILA_NONE;
            v->type = DILV_INT;
            if (v1->val.ptr)
            {
                v->val.num = ((cintlist *)v1->val.ptr)->Length();
            }
            else
            {
                v->val.num = 0;
            }
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
    {
        Buf[rep++] = *src++;
    }
    Buf[rep] = 0;

    return (atoi(Buf));
}

long str_escape_size(char *sbuf, long ln)
{
    long count = 0;
    long i = 0;

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
void dilfe_txf(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char *dest = nullptr;

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
            {
                v->type = DILV_FAIL; /* NULL string */
            }
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }
    p->stack.push(v);
    delete v1;
}

/* sact - return string (call act_generate) */
void dilfe_sact(dilprg *p)
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

    if (dil_type_check("sact",
                       p,
                       6,
                       v1,
                       TYPEFAIL_NULL,
                       1,
                       DILV_SP,
                       v2,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v3,
                       TYPEFAIL_NULL,
                       3,
                       DILV_UP,
                       DILV_SP,
                       DILV_NULL,
                       v4,
                       TYPEFAIL_NULL,
                       3,
                       DILV_UP,
                       DILV_SP,
                       DILV_NULL,
                       v5,
                       TYPEFAIL_NULL,
                       3,
                       DILV_UP,
                       DILV_SP,
                       DILV_NULL,
                       v6,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT))
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
    {
        v->type = DILV_ERR; /* wrong type */
    }

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
void dilfe_gint(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v3 = p->stack.pop(); // INT
    dilval *v2 = p->stack.pop(); // UNIT
    dilval *v1 = p->stack.pop(); // INT

    unit_data *p_u = nullptr;
    int p_i = 0;
    int idx = 0;

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
            p_u = (unit_data *)v2->val.ptr;
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
            case DIL_GINT_MANAREG:
                if ((p_u != nullptr) && IS_CHAR(p_u))
                {
                    v->val.num = mana_gain(p_u);
                }
                break;

            case DIL_GINT_HITREG:
                if ((p_u != nullptr) && IS_CHAR(p_u))
                {
                    v->val.num = hit_gain(p_u);
                }
                break;

            case DIL_GINT_MOVEREG:
                if ((p_u != nullptr) && IS_CHAR(p_u))
                {
                    v->val.num = move_gain(p_u);
                }
                break;

            case DIL_GINT_EFFDEX:
                if ((p_u != nullptr) && IS_CHAR(p_u))
                {
                    v->val.num = effective_dex(p_u);
                }
                break;

            case DIL_GINT_REQXP:
                v->val.num = required_xp(p_i);
                break;

            case DIL_GINT_LEVELXP:
                v->val.num = level_xp(p_i);
                break;

            case DIL_GINT_DESCRIPTOR:
                if ((p_u != nullptr) && IS_PC(p_u))
                {
                    v->val.num = (CHAR_DESCRIPTOR(p_u) != nullptr);
                }
                else
                {
                    v->val.num = 1;
                }
                break;

            case DIL_GINT_CRIMENO:
                v->val.num = new_crime_serial_no();
                break;

            case DIL_GINT_BONUS_A:
                v->val.num = bonus_map_a(p_i);
                break;

            case DIL_GINT_BONUS_B:
                v->val.num = bonus_map_b(p_i);
                break;

            default:
                v->type = DILV_FAIL; /* failed */
                slog(LOG_ALL,
                     0,
                     "getinteger() was given incorrect index %d by DIL %s@%s.",
                     idx,
                     UNIT_FI_NAME(p->sarg->owner),
                     UNIT_FI_ZONENAME(p->sarg->owner));
                break;
        }
    }

    p->stack.push(v);

    delete v1;
    delete v2;
    delete v3;
}

/* asctime */
void dilfe_ast(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    char *c = nullptr;

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

void dilfe_getw(dilprg *p)
{
    dilval *v = new dilval;
    /* Get first word of a string */
    dilval *v1 = p->stack.pop();
    char *c = nullptr;
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
                v->atyp = DILA_EXP;
                v->type = DILV_SP;
                c = str_next_word_copy((char *)v1->val.ptr, buf1);
                c = skip_spaces(c);
                v->val.ptr = str_dup(buf1);

                if (v1->atyp == DILA_NORM && v1->type == DILV_SPR)
                {
                    memmove(*(char **)v1->ref, c, strlen(c) + 1);
                }
                else if (p->sarg->arg == (const char *)v1->val.ptr)
                {
                    p->sarg->arg = c; /* Update argument as in findunit :) */
                }
            }
            else
            {
                v->type = DILV_FAIL; /* NULL string */
            }
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_getws(dilprg *p)
{
    dilval *v = new dilval;
    /* Get first word of a string */
    dilval *v1 = p->stack.pop();
    char *tmp = nullptr;
    char *c = nullptr;
    char *ptr = nullptr;

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
                for (c = strtok(tmp, " "); c && *c; c = strtok(nullptr, " "))
                {
                    words->dAppendName(c);
                }
                FREE(ptr);

                v->val.ptr = words;
            }
            else
            {
                v->type = DILV_FAIL; /* NULL string */
            }
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }
    p->stack.push(v);
    delete v1;
}


void dilfe_getaffects(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    dilval *v = new dilval;
    v->type = DILV_FAIL;

    switch (dil_getval(v1))
    {
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_EXP;
                v->type = DILV_SLP;
                cNamelist *words = new cNamelist;
                get_affects((unit_data *) v1->val.ptr, words);
                v->val.ptr = words;
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



void dilfe_split(dilprg *p)
{
    dilval *v = new dilval;
    /* Get first word of a string */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int slen = 0;
    char *buf = nullptr;
    char *c = nullptr;
    char *spbuf = nullptr;

    v->type = DILV_FAIL;

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
                        {
                            words->dAppendName(buf);
                        }

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
        buf = nullptr;
    }
    if (spbuf)
    {
        free(spbuf);
        spbuf = nullptr;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

// BOOST VERSION
/*return a unit file directory listing */
void dilfe_udir(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();

    cNamelist *words = new cNamelist;
    std::string uPath;
    std::string sPath;

    v->type = DILV_SLP;

    switch (dil_getval(v1))
    {
        case DILV_SP:
        {
            if (v1->val.ptr)
            {
                v->atyp = DILA_EXP;
                v->type = DILV_SLP;
                if (str_is_empty(p->frame[0].tmpl->zone->getDILFilePath()))
                {
                    uPath = g_cServerConfig.getDILFileDir();
                    uPath = uPath + "/units";
                }
                else
                {
                    uPath = p->frame[0].tmpl->zone->getDILFilePath();
                    uPath = uPath + "/units";
                }

                sPath = (char *)v1->val.ptr;
                if (sPath.empty())
                {
                    sPath = ".*";
                }

                boost::filesystem::path full_path(uPath);
                boost::filesystem::directory_iterator end_iter;
                boost::regex expression;

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
                    if ((boost::filesystem::exists(full_path)) && (boost::filesystem::is_directory(full_path)))
                    {
                        for (boost::filesystem::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
                        {
                            boost::cmatch what;

                            if (regex_match(dir_itr->path().filename().c_str(), what, expression))
                            {
                                words->dAppendName(dir_itr->path().filename().c_str());
                            }
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
            {
                v->type = DILV_FAIL;
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

    p->stack.push(v);
    delete v1;
}

// BOOST VERSION
/*return a string file directory listing */
void dilfe_sdir(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();
    cNamelist *words = new cNamelist;
    std::string uPath;
    std::string sPath;
    v->type = DILV_SLP;

    switch (dil_getval(v1))
    {
        case DILV_SP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_EXP;
                v->type = DILV_SLP;
                if (str_is_empty(p->frame[0].tmpl->zone->getDILFilePath()))
                {
                    uPath = g_cServerConfig.getDILFileDir();
                    uPath = uPath + "/strings";
                }
                else
                {
                    uPath = p->frame[0].tmpl->zone->getDILFilePath();
                    uPath = uPath + "/strings";
                }

                sPath = (char *)v1->val.ptr;
                if (sPath.empty())
                {
                    sPath = ".*";
                }

                boost::filesystem::path full_path(uPath);
                boost::filesystem::directory_iterator end_iter;

                boost::regex expression;

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
                    if ((boost::filesystem::exists(full_path)) && (boost::filesystem::is_directory(full_path)))
                    {
                        for (boost::filesystem::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
                        {
                            boost::cmatch what;

                            if (regex_match(dir_itr->path().filename().c_str(), what, expression))
                            {
                                words->dAppendName(dir_itr->path().filename().c_str());
                            }
                        }
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

    p->stack.push(v);
    delete v1;
}

void dilfe_wepinfo(dilprg *p)
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
                ilist->Append(g_wpn_info[(int)v1->val.num].hands);
                ilist->Append(g_wpn_info[(int)v1->val.num].speed);
                ilist->Append(g_wpn_info[(int)v1->val.num].type);
                ilist->Append(g_wpn_info[(int)v1->val.num].shield);
                ilist->Append(g_WpnColl.tree[(int)v1->val.num].parent);
                for (int idx2 = 0; idx2 < MAX_ARMOUR_TYPES; idx2++)
                {
                    ilist->Append(g_weapon_chart[(int)v1->val.num].element[idx2].offset);
                    ilist->Append(g_weapon_chart[(int)v1->val.num].element[idx2].basedam);
                    ilist->Append(g_weapon_chart[(int)v1->val.num].element[idx2].alpha);
                }
                v->val.ptr = ilist;
            }
            else
            {
                v->val.ptr = nullptr;
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

void dilfe_load(dilprg *p)
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
                v->val.ptr = nullptr;

                file_index_type *fi = str_to_file_index((char *)v1->val.ptr);

                if (fi)
                {
                    if (fi->getType() == UNIT_ST_ROOM)
                    {
                        slog(LOG_ALL, 0, "DIL trying to load a room %s@%s.", fi->getName(), fi->getZone()->getName());
                    }
                    else
                    {
                        v->val.ptr = read_unit(fi);
                    }
                }
                else
                {
                    slog(LOG_ALL, 0, "DIL load(%s) - unit does not exist.", (char *)v1->val.ptr);
                }

                if (v->val.ptr)
                {
                    if (IS_MONEY((unit_data *)v->val.ptr))
                    {
                        set_money((unit_data *)v->val.ptr, MONEY_AMOUNT((unit_data *)v->val.ptr));
                    }

                    unit_to_unit((unit_data *)v->val.ptr, p->sarg->owner);
                    dil_loadtime_activate((unit_data *)v->val.ptr);
                    v->atyp = DILA_NORM;
                    v->type = DILV_UP;
                }
                else
                {
                    v->type = DILV_NULL; /* Unit not found */
                }
            }
            else
            {
                v->type = DILV_FAIL; /* NULL string */
            }
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_getcmd(dilprg *p)
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
                v->val.ptr = (command_info *)search_trie((char *)v1->val.ptr, g_intr_trie);
                if (v->val.ptr)
                {
                    v->atyp = DILA_NORM;
                    v->type = DILV_CP;
                }
                else
                {
                    v->type = DILV_NULL; /* Unit not found */
                }
            }
            else
            {
                v->type = DILV_FAIL; /* NULL string */
            }
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_clone(dilprg *p)
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
                v->val.ptr = ((unit_data *)v1->val.ptr)->copy();
                if (v->val.ptr)
                {
                    unit_to_unit((unit_data *)v->val.ptr, p->sarg->owner);
                    dil_loadtime_activate((unit_data *)v->val.ptr);
                    v->atyp = DILA_NORM;
                    v->type = DILV_UP;
                }
                else
                {
                    v->type = DILV_NULL; /* Unit not found */
                }
            }
            else
            {
                v->type = DILV_FAIL; /* NULL string */
            }
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_plus(dilprg *p)
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
            {
                v->type = DILV_FAIL;
            }
            break;

        case DILV_SP:
            if (v->type != DILV_SP)
            {
                v->type = DILV_ERR; /* wrong type */
            }
            break;

        case DILV_INT:
            if (v->type != DILV_INT)
            {
                v->type = DILV_ERR; /* wrong type */
            }
            break;

        case DILV_NULL:
            if (v->type != DILV_SP)
            {
                v->type = DILV_ERR; /* wrong type */
            }
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
void dilfe_dld(dilprg *p)
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
                        v->val.num = dil_destroy((char *)v1->val.ptr, (unit_data *)v2->val.ptr);
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

void dilfe_dlf(dilprg *p)
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
                        if (dil_find((char *)v1->val.ptr, (unit_data *)v2->val.ptr))
                        {
                            v->val.num = TRUE;
                        }
                        else
                        {
                            v->val.num = FALSE;
                        }
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

void dilfe_call(dilprg *p)
{
    dilval *v = new dilval;
    /* Detection of DIL programs (TRUE/FALSE) */

    dilval *v4 = p->stack.pop(); // string
    dilval *v3 = p->stack.pop(); // integer
    dilval *v2 = p->stack.pop(); // unitptr
    dilval *v1 = p->stack.pop(); // The DIL to call

    v->type = DILV_ERR; /* wrong type */

    switch (dil_getval(v4))
    {
        case DILV_SP:
            switch (dil_getval(v3))
            {
                case DILV_INT:
                    switch (dil_getval(v2))
                    {
                        case DILV_UP:
                            switch (dil_getval(v1))
                            {
                                case DILV_SP:
                                    if (v1->val.ptr)
                                    {
                                        v->type = DILV_INT;
                                        v->atyp = DILA_NONE;
                                        v->val.num = -1;

                                        diltemplate *tmpl = nullptr;
                                        tmpl = find_dil_template((char *)v1->val.ptr);

                                        if (tmpl)
                                        {
                                            //
                                            // Push frame
                                            //
                                            p->stack.push(v2);
                                            p->stack.push(v3);
                                            p->stack.push(v4);

                                            delete v1;
                                            // Don't delete v2-v4 they now live on the stack.

                                            dil_push_frame(p, tmpl);
                                            return;
                                        }
                                        else
                                        {
                                            szonelog(UNIT_FI_ZONE(p->sarg->owner),
                                                     "DIL %s@%s, Unable to find template %s",
                                                     UNIT_FI_NAME(p->sarg->owner),
                                                     UNIT_FI_ZONENAME(p->sarg->owner),
                                                     (char *)v1->val.ptr);
                                        }
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
            }
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
}

void dilfe_min(dilprg *p)
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

void dilfe_mul(dilprg *p)
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

void dilfe_div(dilprg *p)
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
                    {
                        v->type = DILV_FAIL;
                    }
                    else
                    {
                        v->val.num = v1->val.num / v2->val.num;
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
    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_mod(dilprg *p)
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
                    {
                        v->type = DILV_FAIL;
                    }
                    else
                    {
                        v->val.num = v1->val.num % v2->val.num;
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
    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_and(dilprg *p)
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
    {
        v->type = DILV_FAIL;
    }

    delete v1;
    delete v2;
    p->stack.push(v);
}

void dilfe_land(dilprg *p)
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

void dilfe_or(dilprg *p)
{
    dilval *v = new dilval;
    /* Or two integers (or booleans) */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    v->type = DILV_INT;
    v->atyp = DILA_NONE;

    if ((dil_getval(v1) == DILV_INT) && (dil_getval(v2) == DILV_INT))
    {
        v->val.num = v1->val.num | v2->val.num;
    }
    else
    {
        v->type = DILV_FAIL;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_lor(dilprg *p)
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

void dilfe_isa(dilprg *p)
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
            {
                switch (dil_getval(v2))
                {
                    case DILV_INT:
                        v->atyp = DILA_NONE;
                        v->val.num = (affected_by_spell((unit_data *)v1->val.ptr, v2->val.num) != nullptr);
                        break;
                    case DILV_FAIL:
                        v->type = DILV_FAIL;
                        break;
                    default:
                        v->type = DILV_ERR; /* wrong type */
                        break;
                }
            }
            else
            {
                v->type = DILV_FAIL;
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

void dilfe_rnd(dilprg *p)
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

// findroom(#)
void dilfe_fndr(dilprg *p)
{
    dilval *v = new dilval;
    /* Find a room */
    dilval *v1 = p->stack.pop();
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

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
                if (v->val.ptr == nullptr)
                {
                    v->type = DILV_NULL; /* not found */
                }
            }
            else
            {
                v->type = DILV_NULL; /* not found */
            }

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

// findzone(#)
void dilfe_fndz(dilprg *p)
{
    dilval *v = new dilval;
    dilval *v1 = p->stack.pop();

    switch (dil_getval(v1))
    {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        case DILV_SP:
            v->atyp = DILA_NORM;
            v->type = DILV_ZP;
            if (v1->val.ptr)
            {
                v->val.ptr = find_zone((const char *)v1->val.ptr);
                if (v->val.ptr == nullptr)
                {
                    v->type = DILV_NULL; /* not found */
                }
            }
            else
            {
                v->type = DILV_NULL; /* not found */
            }

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

// findsymbolic(#,#,#)
void dilfe_fnds2(dilprg *p)
{
    dilval *v = new dilval;
    /* Find a symbolic unit */
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

    v->type = DILV_UP;
    switch (dil_getval(v1))
    {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;

        case DILV_UP:
            if (v1->val.ptr)
            {
                switch (dil_getval(v2))
                {
                    case DILV_SP:
                        if (v2->val.ptr == nullptr)
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            switch (dil_getval(v3))
                            {
                                case DILV_INT:
                                {
                                    v->atyp = DILA_NORM;
                                    *buf1 = '\0';
                                    *buf2 = '\0';
                                    split_fi_ref((char *)v2->val.ptr, buf1, buf2);
                                    file_index_type *fi = find_file_index(buf1, buf2);
                                    if (fi)
                                    {
                                        v->val.ptr = fi->find_symbolic_instance_ref((unit_data *)v1->val.ptr, v3->val.num);
                                    }
                                    else
                                    {
                                        v->val.ptr = nullptr;
                                        v->type = DILV_NULL; /* not found */
                                    }
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
            }
            else
            {
                v->type = DILV_FAIL;
            }
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

// findsymbolic(#,#)
void dilfe_fndsidx(dilprg *p)
{
    dilval *v = new dilval;
    /* Find a symbolic unit */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

    v->type = DILV_UP;
    switch (dil_getval(v1))
    {
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;

        case DILV_SP:
            if (v1->val.ptr)
            {
                switch (dil_getval(v2))
                {
                    case DILV_INT:
                        v->atyp = DILA_NORM;
                        *buf1 = '\0';
                        *buf2 = '\0';
                        split_fi_ref((const char *)v1->val.ptr, buf1, buf2);

                        v->val.ptr = find_symbolic_idx(buf1, buf2, v2->val.num);

                        if (v->val.ptr == nullptr)
                        {
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
            }
            else
            {
                v->type = DILV_FAIL;
            }
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
}

// findsymbolic(#)
void dilfe_fnds(dilprg *p)
{
    dilval *v = new dilval;
    /* Find a symbolic unit */
    dilval *v1 = p->stack.pop();
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

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
            if (v->val.ptr == nullptr)
            {
                v->type = DILV_NULL; /* not found */
            }
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }
    p->stack.push(v);
    delete v1;
}

void dilfe_gt(dilprg *p)
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

void dilfe_lt(dilprg *p)
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

void dilfe_ge(dilprg *p)
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

void dilfe_le(dilprg *p)
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

void dilfe_eq(dilprg *p)
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

void dilfe_ne(dilprg *p)
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

void dilfe_slt(dilprg *p)
{
    dilval *v = new dilval;
    /* Less Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *s1 = nullptr;
    char *s2 = nullptr;
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
                    if (((char *)v1->val.ptr == nullptr) || ((char *)v2->val.ptr == nullptr))
                    {
                        v->val.num = FALSE;
                    }
                    else
                    {
                        s1 = str_dup((char *)v1->val.ptr);
                        s2 = str_dup((char *)v2->val.ptr);
                        str_lower(s1);
                        str_lower(s2);
                        if (strcmp(s1, s2) < 0)
                        {
                            v->val.num = TRUE;
                        }
                        else
                        {
                            v->val.num = FALSE;
                        }
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

void dilfe_sgt(dilprg *p)
{
    dilval *v = new dilval;
    /* Less Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *s1 = nullptr;
    char *s2 = nullptr;
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
                    if (((char *)v1->val.ptr == nullptr) || ((char *)v2->val.ptr == nullptr))
                    {
                        v->val.num = FALSE;
                    }
                    else
                    {
                        s1 = str_dup((char *)v1->val.ptr);
                        s2 = str_dup((char *)v2->val.ptr);
                        str_lower(s1);
                        str_lower(s2);
                        if (strcmp(s1, s2) > 0)
                        {
                            v->val.num = TRUE;
                        }
                        else
                        {
                            v->val.num = FALSE;
                        }
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

void dilfe_sle(dilprg *p)
{
    dilval *v = new dilval;
    /* Less Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *s1 = nullptr;
    char *s2 = nullptr;
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
                    if (((char *)v1->val.ptr == nullptr) || ((char *)v2->val.ptr == nullptr))
                    {
                        v->val.num = FALSE;
                    }
                    else
                    {
                        s1 = str_dup((char *)v1->val.ptr);
                        s2 = str_dup((char *)v2->val.ptr);
                        str_lower(s1);
                        str_lower(s2);
                        if (strcmp(s1, s2) <= 0)
                        {
                            v->val.num = TRUE;
                        }
                        else
                        {
                            v->val.num = FALSE;
                        }
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

void dilfe_sge(dilprg *p)
{
    dilval *v = new dilval;
    /* Less Than operator */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char *s1 = nullptr;
    char *s2 = nullptr;
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
                    if (((char *)v1->val.ptr == nullptr) || ((char *)v2->val.ptr == nullptr))
                    {
                        v->val.num = FALSE;
                    }
                    else
                    {
                        s1 = str_dup((char *)v1->val.ptr);
                        s2 = str_dup((char *)v2->val.ptr);
                        str_lower(s1);
                        str_lower(s2);
                        if (strcmp(s1, s2) >= 0)
                        {
                            v->val.num = TRUE;
                        }
                        else
                        {
                            v->val.num = FALSE;
                        }
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

void dilfe_se(dilprg *p)
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
            {
                v->type = DILV_FAIL;
            }
            break;
        case DILV_SP:
            break;
        case DILV_NULL:
            if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            {
                v->type = DILV_NULL; /* null pointer */
            }
            break;
        default:
            v->type = DILV_ERR;
            break;
    }

    switch (v->type)
    {
        case DILV_INT:
            v->atyp = DILA_NONE;
            if (str_is_empty((char *)v1->val.ptr) || str_is_empty((char *)v2->val.ptr))
            {
                v->val.num = (str_is_empty((char *)v1->val.ptr) && str_is_empty((char *)v2->val.ptr));
            }
            else
            {
                v->val.num = !str_ccmp((char *)v1->val.ptr, (char *)v2->val.ptr);
            }
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

void dilfe_sne(dilprg *p)
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
            {
                v->type = DILV_FAIL;
            }
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
            if (str_is_empty((char *)v1->val.ptr) || str_is_empty((char *)v2->val.ptr))
            {
                v->val.num = (!str_is_empty((char *)v1->val.ptr) || !str_is_empty((char *)v2->val.ptr));
            }
            else
            {
                v->val.num = (str_ccmp((char *)v1->val.ptr, (char *)v2->val.ptr) != 0);
            }
            break;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_pe(dilprg *p)
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
        {
            v->val.num = ((((cintlist *)v1->val.ptr)->Length()) == 0);
        }
        else if (v2->val.ptr && ilp2 && n1)
        {
            v->val.num = ((((cintlist *)v2->val.ptr)->Length()) == 0);
        }
        else if (v1->val.ptr && slp1 && n2)
        {
            v->val.num = ((((cNamelist *)v1->val.ptr)->Length()) == 0);
        }
        else if (v2->val.ptr && slp2 && n1)
        {
            v->val.num = ((((cNamelist *)v2->val.ptr)->Length()) == 0);
        }
        else
        {
            v->val.num = (v1->val.ptr == v2->val.ptr);
        }
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_pne(dilprg *p)
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
        {
            v->val.num = ((((cintlist *)v1->val.ptr)->Length()) != 0);
        }
        else if (v2->val.ptr && ilp2 && n1)
        {
            v->val.num = ((((cintlist *)v2->val.ptr)->Length()) != 0);
        }
        else if (v1->val.ptr && slp1 && n2)
        {
            v->val.num = ((((cNamelist *)v1->val.ptr)->Length()) != 0);
        }
        else if (v2->val.ptr && slp2 && n1)
        {
            v->val.num = ((((cNamelist *)v2->val.ptr)->Length()) != 0);
        }
        else
        {
            v->val.num = (v1->val.ptr != v2->val.ptr);
        }
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_iss(dilprg *p)
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

void dilfe_in(dilprg *p)
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
            {
                v->type = DILV_SLP;
            }
            else
            {
                v->type = DILV_NULL; /* not found */
            }
            break;

        case DILV_EDP:
            if (v2->val.ptr)
            {
                v->type = DILV_EDP;
            }
            else
            {
                v->type = DILV_NULL; /* not found */
            }
            break;

        case DILV_SP:
            if (v2->val.ptr)
            {
                v->type = DILV_INT;
            }
            else
            {
                v->type = DILV_NULL; /* not found */
            }
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
            {
                break;
            }
            else
            {
                v->type = DILV_FAIL;
            }

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
            {
                v->val.num = 1 + (((cNamelist *)v2->val.ptr)->IsNameIdx((char *)v1->val.ptr));
            }
            break;

        case DILV_EDP:
            v->atyp = DILA_NORM;
            v->type = DILV_EDP;
            if (v1->val.ptr && v2->val.ptr)
            { // MS2020 BUG
                v->val.ptr = ((extra_descr_data *)v2->val.ptr)->find_raw(skip_spaces((char *)v1->val.ptr));
            }
            else
            {
                v->val.ptr = nullptr;
            }
            break;

        case DILV_INT:
            v->atyp = DILA_NONE;
            v->type = DILV_INT;
            v->val.num = FALSE;
            if ((t1 == DILV_SP) && (t2 == DILV_SP) && v1->val.ptr && v2->val.ptr)
            {
                v->val.num = str_cstr((const char *)v2->val.ptr, (const char *)v1->val.ptr) != nullptr;
            }
            else
            {
                break;
            }
    }
    p->stack.push(v);
    delete v1;
    delete v2;
}

void dilfe_strcmp(dilprg *p)
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
            {
                v->type = DILV_FAIL;
            }
            break;
        case DILV_SP:
            break;
        case DILV_NULL:
            if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            {
                v->type = DILV_NULL; /* null pointer */
            }
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

void dilfe_strncmp(dilprg *p)
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
            {
                v->type = DILV_FAIL;
            }
            break;
        case DILV_SP:
            break;
        case DILV_NULL:
            if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            {
                v->type = DILV_NULL; /* null pointer */
            }
            break;
        default:
            v->type = DILV_ERR;
            break;
    }

    switch (v->type)
    {
        case DILV_INT:
            v->atyp = DILA_NONE;
            v->val.num = strncmp((char *)v1->val.ptr, (char *)v2->val.ptr, (int)v3->val.num);
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

void dilfe_fndu2(dilprg *p)
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
            {
                v->type = DILV_UP;
            }
            else
            {
                v->type = DILV_NULL; /* not applicable UP */
            }
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
            {
                v->type = DILV_FAIL;
            }
            break;

        case DILV_SP:
            if (v->type != DILV_ERR)
            {
                if (!v2->val.ptr)
                {
                    v->type = DILV_NULL; /* not applicable SP */
                }
                else if (!*((char *)v2->val.ptr))
                {
                    v->type = DILV_NULL; /* not applicable SP */
                }
            }
            break;

        case DILV_NULL:
            if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            {
                v->type = DILV_NULL; /* not applicable */
            }
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }

    switch (dil_getval(v3))
    {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
            {
                v->type = DILV_FAIL;
            }
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
            {
                v->type = DILV_FAIL;
            }
            break;

        case DILV_UP:
            break;

        case DILV_NULL: /* null pointer */
            if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            {
                v4->val.ptr = nullptr;
            }
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }
    switch (dil_getval(v5))
    {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
            {
                v->type = DILV_FAIL;
            }
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
        v->val.ptr = find_unit_dil((unit_data *)v1->val.ptr, &c, (unit_data *)v4->val.ptr, v3->val.num, v5->val.num);

        if (v2->atyp == DILA_NORM && v2->type == DILV_SPR)
        {
            memmove(v2->val.ptr, c, strlen(c) + 1);
        }
        else if (p->sarg->arg == (const char *)v2->val.ptr)
        {
            p->sarg->arg = c; /* Update argument if argument is argument! :) */
        }

        if (!v->val.ptr)
        {
            v->type = DILV_NULL;
        }
    }
    else
    {
        v->type = DILV_FAIL;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
}

void dilfe_fndu(dilprg *p)
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
            {
                v->type = DILV_UP;
            }
            else
            {
                v->type = DILV_NULL; /* not applicable UP */
            }
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
            {
                v->type = DILV_FAIL;
            }
            break;

        case DILV_SP:
            if (v->type != DILV_ERR)
            {
                if (!v2->val.ptr)
                {
                    v->type = DILV_NULL; /* not applicable SP */
                }
                else if (!*((char *)v2->val.ptr))
                {
                    v->type = DILV_NULL; /* not applicable SP */
                }
            }
            break;

        case DILV_NULL:
            if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            {
                v->type = DILV_NULL; /* not applicable */
            }
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }

    switch (dil_getval(v3))
    {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
            {
                v->type = DILV_FAIL;
            }
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
            {
                v->type = DILV_FAIL;
            }
            break;

        case DILV_UP:
            break;

        case DILV_NULL: /* null pointer */
            if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            {
                v4->val.ptr = nullptr;
            }
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }

    if (v->type == DILV_UP)
    {
        char *c = (char *)v2->val.ptr;

        v->atyp = DILA_NORM;
        v->val.ptr = find_unit_dil((unit_data *)v1->val.ptr, &c, (unit_data *)v4->val.ptr, v3->val.num);

        if (v2->atyp == DILA_NORM && v2->type == DILV_SPR)
        {
            memmove(v2->val.ptr, c, strlen(c) + 1);
        }
        else if (p->sarg->arg == (const char *)v2->val.ptr)
        {
            p->sarg->arg = c; /* Update argument if argument is argument! :) */
        }

        if (!v->val.ptr)
        {
            v->type = DILV_NULL;
        }
    }
    else
    {
        v->type = DILV_FAIL;
    }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
}

// findrndunit(#,#,#)
void dilfe_fndru(dilprg *p)
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
            {
                v->type = DILV_UP;
            }
            else
            {
                v->type = DILV_NULL; /* not applicable UP */
            }
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
            {
                v->type = DILV_FAIL;
            }
            break;
        case DILV_INT:
            break;

        case DILV_NULL:
            if ((v->type != DILV_ERR) && (v->type != DILV_FAIL))
            {
                v->type = DILV_NULL; /* not applicable */
            }
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
    }

    switch (dil_getval(v3))
    {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
            {
                v->type = DILV_FAIL;
            }
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
        v->val.ptr = random_unit((unit_data *)v1->val.ptr, v2->val.num, v3->val.num);

        if (!v->val.ptr)
        {
            v->type = DILV_NULL;
        }
    }
    else
    {
        v->type = DILV_FAIL;
    }

    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
}

// read a fixed string
void dilfe_fs(dilprg *p)
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

void dilfe_fsl(dilprg *p)
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

void dilfe_fil(dilprg *p)
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

void dilfe_var(dilprg *p)
{
    dilval *v = new dilval;
    /* A variable */
    int varno = 0;

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

void dilfe_weat(dilprg *p)
{
    dilval *v = new dilval;
    /* Self */

    v->type = DILV_INT;
    v->atyp = DILA_NORM;
    v->val.num = unit_zone(p->sarg->owner)->cgetWeather().getSky();
    p->stack.push(v);
}

void dilfe_self(dilprg *p)
{
    dilval *v = new dilval;
    /* Self */

    v->type = DILV_UP;
    v->atyp = DILA_NORM;
    v->val.ptr = p->sarg->owner;
    p->stack.push(v);
}

void dilfe_hrt(dilprg *p)
{
    dilval *v = new dilval;
    /* Heartbeat */

    v->type = DILV_SINT2R;
    v->atyp = DILA_NONE;
    v->ref = p->sarg->fptr->getHeartBeatPtr();
    p->stack.push(v);
}

void dilfe_tho(dilprg *p)
{
    dilval *v = new dilval;
    /* MudHour */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = mud_date().getHours();
    p->stack.push(v);
}

void dilfe_tda(dilprg *p)
{
    dilval *v = new dilval;
    /* MudDay */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = mud_date().getDay();
    p->stack.push(v);
}

void dilfe_tmd(dilprg *p)
{
    dilval *v = new dilval;
    /* MudMonth */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = mud_date().getMonth();
    p->stack.push(v);
}

void dilfe_tye(dilprg *p)
{
    dilval *v = new dilval;
    /* MudYear */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = mud_date().getYear();
    p->stack.push(v);
}

void dilfe_rti(dilprg *p)
{
    dilval *v = new dilval;
    /* RealTime */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = time(nullptr);
    p->stack.push(v);
}

void dilfe_acti(dilprg *p)
{
    dilval *v = new dilval;
    /* Activator */

    v->type = DILV_UP;
    v->atyp = DILA_NORM;
    v->val.ptr = p->sarg->activator;
    p->stack.push(v);
}

void dilfe_medi(dilprg *p)
{
    dilval *v = new dilval;
    /* Medium */

    v->type = DILV_UP;
    v->atyp = DILA_NORM;
    v->val.ptr = p->sarg->medium;
    p->stack.push(v);
}

void dilfe_targ(dilprg *p)
{
    dilval *v = new dilval;
    /* Target */

    v->type = DILV_UP;
    v->atyp = DILA_NORM;
    v->val.ptr = p->sarg->target;
    p->stack.push(v);
}

void dilfe_powe(dilprg *p)
{
    dilval *v = new dilval;
    /* Power */

    static int dummy = 0;

    v->type = DILV_SINT4R;
    v->atyp = DILA_NONE;

    if (p->sarg->pInt)
    {
        v->ref = (sbit32 *)p->sarg->pInt;
    }
    else
    {
        v->ref = (int *)&dummy;
    }
    p->stack.push(v);
}

void dilfe_cmst(dilprg *p)
{
    dilval *v = new dilval;
    /* cmdstr */

    v->type = DILV_SP;
    v->atyp = DILA_NORM;

    if (p->sarg->cmd->cmd_str)
    {
        v->val.ptr = (char *)p->sarg->cmd->cmd_str;
    }
    else
    {
        v->type = DILV_NULL;
        v->val.ptr = nullptr;
    }
    p->stack.push(v);
}

void dilfe_excmst(dilprg *p)
{
    dilval *v = new dilval;
    /* cmdstr */

    v->type = DILV_SP;
    v->atyp = DILA_NORM;

    if (p->sarg->cmd->excmd)
    {
        v->val.ptr = (char *)p->sarg->cmd->excmd;
    }
    else
    {
        v->type = DILV_NULL;
        v->val.ptr = nullptr;
    }
    p->stack.push(v);
}

void dilfe_excmstc(dilprg *p)
{
    dilval *v = new dilval;
    /* excmdc */

    v->type = DILV_SP;
    v->atyp = DILA_NORM;

    if (p->sarg->cmd->excmdc)
    {
        v->val.ptr = (char *)p->sarg->cmd->excmdc;
    }
    else
    {
        v->type = DILV_NULL;
        v->val.ptr = nullptr;
    }
    p->stack.push(v);
}

void dilfe_argm(dilprg *p)
{
    dilval *v = new dilval;
    /* Argument */

    v->type = DILV_SP;
    v->atyp = DILA_NORM;
    if (p->sarg->arg)
    {
        v->val.ptr = (char *)p->sarg->arg;
    }
    else
    {
        v->type = DILV_NULL;
        v->val.ptr = nullptr;
    }
    p->stack.push(v);
}

void dilfe_null(dilprg *p)
{
    dilval *v = new dilval;
    /* Pointer value null */

    v->type = DILV_NULL;
    v->atyp = DILA_NONE;
    v->val.ptr = nullptr;
    p->stack.push(v);
}

void dilfe_int(dilprg *p)
{
    dilval *v = new dilval;
    /* Fixed integer */

    v->type = DILV_INT;
    v->atyp = DILA_NONE;
    v->val.num = (sbit32)bread_ubit32(&(p->fp->pc));
    p->stack.push(v);
}

void dilfe_cmds(dilprg *p)
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
            {
                v->type = DILV_FAIL;
            }
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

void dilfe_pck(dilprg *p)
{
    dilval *v = new dilval;
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
            if (!v1->val.ptr || !IS_CHAR((unit_data *)v1->val.ptr))
            {
                v->type = DILV_FAIL;
            }
            else
            {
                switch (dil_getval(v2))
                {
                    case DILV_FAIL:
                    case DILV_NULL:
                        v->type = DILV_FAIL;
                        break;
                    case DILV_UP:
                        if (!v2->val.ptr)
                        {
                            v->type = DILV_FAIL;
                        }
                        else
                        {
                            v->val.num = pay_point_charlie((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr);
                        }
                        break;
                    default:
                        v->type = DILV_ERR;
                        break;
                }
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

void dilfe_act(dilprg *p)
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

    if (dil_type_check("act",
                       p,
                       6,
                       v1,
                       TYPEFAIL_NULL,
                       1,
                       DILV_SP,
                       v2,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v3,
                       TYPEFAIL_NULL,
                       3,
                       DILV_UP,
                       DILV_SP,
                       DILV_NULL,
                       v4,
                       TYPEFAIL_NULL,
                       3,
                       DILV_UP,
                       DILV_SP,
                       DILV_NULL,
                       v5,
                       TYPEFAIL_NULL,
                       3,
                       DILV_UP,
                       DILV_SP,
                       DILV_NULL,
                       v6,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT))
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
                    act_generate(buf, (char *)v1->val.ptr, v2->val.num, v3, v4, v5, v6->val.num, (unit_data *)v3->val.ptr);
                }
                v->type = DILV_SP;
                v->atyp = DILA_EXP;
                v->val.ptr = str_dup(buf);
                break;

            case TO_VICT:
            case TO_NOTVICT:
                if (v5->val.ptr)
                {
                    act_generate(buf, (char *)v1->val.ptr, v2->val.num, v3, v4, v5, v6->val.num, (unit_data *)v5->val.ptr);
                }
                v->type = DILV_SP;
                v->atyp = DILA_EXP;
                v->val.ptr = str_dup(buf);
                break;
        }
    }
    p->stack.push(v);
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
}
