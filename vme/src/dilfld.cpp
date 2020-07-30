/*
 $Author: All $
 $RCSfile: dilfld.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.11 $
 */

#include "structs.h"
#include "utils.h"
#include "dil.h"
#include "dilexp.h"
#include "dilrun.h"
#include "textutil.h"
#include "db_file.h"
#include "handler.h"
#include "interpreter.h"
#include "utility.h"
#include "common.h"
#include "main.h"
#include "vmelimits.h"
#include "config.h"
#include "combat.h"
#include "skills.h"

void dilfe_fld(register class dilprg *p)
{
    /* Get a structure field */
    class dilval *v1, *v2;
    v1 = p->stack.pop();
    v2 = NULL;
    dilval *v = new dilval;
    int fldno;

    fldno = bread_ubit8(&(p->fp->pc));

    switch (fldno)
    {
    /* *********************************** */
    /* . [ index ]                            */
    case DILF_IDX:
        v2 = p->stack.pop();

        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL;
            break;
        case DILV_SP:
        case DILV_SLP:
            if (v1->val.ptr)
                v->type = DILV_SP;
            else
                v->type = DILV_FAIL; /* NULL list */
            break;
        case DILV_ILP:
            if (v1->val.ptr)
                v->type = DILV_INT;
            else
                v->type = DILV_FAIL; /* NULL list */
            break;
        case DILV_EDP:
            if (v1->val.ptr)
                v->type = DILV_EDP;
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
        case DILV_SP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
        }

        if (v->type == DILV_INT)
        {

            if (((sbit32)v2->val.num <
                 ((class cintlist *)v1->val.ptr)->Length()) &&
                (v2->val.num >= 0))
            {
                v->atyp = DILA_NONE; // Dont dealloc!
                v->type = DILV_SINT4R;
                v->ref = (((cintlist *)v1->val.ptr)->ValuePtr(v2->val.num));
            }
            else
                v->type = DILV_FAIL; /* illegal index */
        }
        else if (v->type == DILV_EDP)
        {
            v->atyp = DILA_NORM;
            v->type = DILV_EDP;
            if (v1->val.ptr)
                v->val.ptr =
                    ((class extra_descr_data *)v1->val.ptr)->find_raw(skip_spaces((char *)v2->val.ptr));
            else
                v->val.ptr = NULL;
        }
        else if (v->type == DILV_SP)
        {

            if (dil_getval(v1) == DILV_SP)
            {

                if ((v2->val.num >= 0) &&
                    (v2->val.num < (int)strlen((char *)v1->val.ptr)))
                {
                    v->atyp = DILA_EXP;
                    v->type = DILV_SP;
                    v->val.ptr = (char *)malloc(2);
                    ((char *)v->val.ptr)[0] =
                        ((char *)v1->val.ptr)[v2->val.num];
                    ((char *)v->val.ptr)[1] = 0;
                }
                else
                {
                    szonelog(UNIT_FI_ZONE(p->sarg->owner),
                             "DIL %s@%s %s@%s, index of stringlist out of bounds: %d\n",
                             UNIT_FI_NAME(p->sarg->owner),
                             UNIT_FI_ZONENAME(p->sarg->owner),
                             p->fp->tmpl->prgname, p->fp->tmpl->zone->name,
                             v2->val.num);

                    v->type = DILV_FAIL;
                }
            }
            else
            {
                v->atyp = DILA_EXP;

                if (((ubit32)v2->val.num <
                     ((class cNamelist *)v1->val.ptr)->Length()) &&
                    (v2->val.num >= 0))
                {
                    v->atyp = DILA_NONE; // Dont dealloc!
                    v->type = DILV_HASHSTR;
                    v->ref =
                        ((cNamelist *)v1->val.ptr)->InstanceName(v2->val.num);
                }
                else
                {
                    szonelog(UNIT_FI_ZONE(p->sarg->owner),
                             "DIL %s@%s %s@%s, index of stringlist out of bounds: %d\n",
                             UNIT_FI_NAME(p->sarg->owner),
                             UNIT_FI_ZONENAME(p->sarg->owner),
                             p->fp->tmpl->prgname, p->fp->tmpl->zone->name,
                             v2->val.num);
                    v->type = DILV_FAIL; /* illegal index */
                }
            }
        }
        else
            v->type = DILV_FAIL; /* illegal index */
        break;

    /* *********************************** */
    /* .idx                                */
    case DILF_SID:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                if (IS_PC((class unit_data *)v1->val.ptr))
                    v->val.num = (int)PC_ID((class unit_data *)v1->val.ptr);
                else
                    v->val.ptr = (file_index_type *)UNIT_FILE_INDEX((class unit_data *)
                                                                        v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .speed                              */
    case DILF_SPD:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if ((v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr)))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                v->val.num = CHAR_SPEED((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_CP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                v->val.num =
                    (((struct command_info *)v1->val.ptr)->combat_speed);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .opponentcount                              */
    case DILF_OPPCT:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if ((v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr)))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                if (CHAR_COMBAT((class unit_data *)v1->val.ptr))
                    v->val.num =
                        CHAR_COMBAT((class unit_data *)v1->val.ptr)->NoOpponents();
                else
                    v->val.num = 0;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
            v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .followercount                              */
    case DILF_FOLCT:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if ((v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr)))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                v->val.num = follower_count((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .loglevel                           */
    case DILF_LOGLVL:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_CP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                v->val.num = (((struct command_info *)v1->val.ptr)->log_level);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .acc_total                          */
    case DILF_ATOT:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if ((v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr)))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                if (g_cServerConfig.m_bAccounting)
                    v->val.num =
                        PC_ACCOUNT((class unit_data *)v1->val.ptr).total_credit;
                else
                    v->val.num = 0;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .acc_balance                        */
    case DILF_ABAL:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if ((v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr)))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                if (g_cServerConfig.m_bAccounting)
                    v->val.num =
                        (int)PC_ACCOUNT((class unit_data *)v1->val.ptr).credit;
                else
                    v->val.num = 0;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .zoneidx                               */
    case DILF_ZOI:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SP;
                v->val.ptr =
                    (char *)UNIT_FI_ZONENAME((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .nameidx                               */
    case DILF_NMI:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SP;
                v->val.ptr =
                    (char *)UNIT_FI_NAME((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .type                                  */
    case DILF_TYP:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = UNIT_TYPE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_CP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = (((struct command_info *)v1->val.ptr)->type);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .next                                 */
    case DILF_NXT:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_EDP:
            v->atyp = DILA_NORM;
            v->type = DILV_EDP;
            if (v1->val.ptr)
                v->val.ptr = ((class extra_descr_data *)v1->val.ptr)->next;
            else
                v->val.ptr = NULL;
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
                v->val.ptr = ((class unit_data *)v1->val.ptr)->next;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_ZP;
                v->val.ptr = ((class zone_type *)v1->val.ptr)->next;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_CP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_CP;
                v->val.ptr = ((struct command_info *)v1->val.ptr)->next;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .names                              */
    case DILF_NMS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_EDP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SLPR;
                if (v1->val.ptr) // MS2020 bug
                    v->ref = &(((class extra_descr_data *)v1->val.ptr)->names);
                else
                    v->ref = NULL;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SLPR;
                v->ref = &UNIT_NAMES((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .creators                                   */
    case DILF_CREATORS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SLPR;
                v->ref = &(((class zone_type *)v1->val.ptr)->creators);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .name                               */
    case DILF_NAM:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_EDP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SP;
                if (v1->val.ptr)
                    v->val.ptr = (void *)
                        IF_STR(((class extra_descr_data *)v1->val.ptr)->names.Name());
                else
                    v->val.ptr = (void *)""; // MS2020 SPOOKY? IS THIS RIGHT?IF_STR returns ""
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SP;
                v->val.ptr = (void *)
                    IF_STR(UNIT_NAME((class unit_data *)v1->val.ptr));
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SP;
                v->val.ptr = (void *)
                    IF_STR(((class zone_type *)v1->val.ptr)->name);
            }
            else
                v->type = DILV_FAIL;
            break;
        case DILV_CP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SP;
                v->val.ptr = (void *)
                    IF_STR(((struct command_info *)v1->val.ptr)->cmd_str);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .vals (extra's)                     */
    case DILF_VALS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_EDP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_ILPR;
                if (v1->val.ptr) //MS2020
                    v->ref = &(((class extra_descr_data *)v1->val.ptr)->vals);
                else
                    v->ref = NULL;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    /* *********************************** */
    /* .descr (extra's)                    */
    case DILF_DES:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_EDP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE; // Dont dealloc!
                v->type = DILV_HASHSTR;
                if (v1->val.ptr)
                    v->ref = &(((class extra_descr_data *)v1->val.ptr)->descr);
                else
                    v->ref = NULL;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .outside_descr                      */
    case DILF_ODES:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE; // Dont dealloc!
                v->type = DILV_HASHSTR;
                v->ref = &UNIT_OUT_DESCR((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .inside_descr                       */
    case DILF_IDES:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE; // Dont dealloc!
                v->type = DILV_HASHSTR;
                v->ref = &UNIT_IN_DESCR((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .title                              */
    case DILF_TIT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE; // Dont dealloc!
                v->type = DILV_HASHSTR;
                v->ref = &UNIT_TITLE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE; // Dont dealloc!
                v->type = DILV_SPR;
                v->ref = &(((class zone_type *)v1->val.ptr)->title);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .hometown                           */
    case DILF_HOME:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SPR;
                v->ref = &PC_HOME((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .last_room                          */
    case DILF_LASTROOM:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UPR;
                v->ref = &CHAR_LAST_ROOM((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .extra                              */
    case DILF_EXT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_EDPR;
                if (v1->val.ptr) // MS2020 BUG
                    v->ref = &(UNIT_EXTRA((class unit_data *)v1->val.ptr).m_pList);
                else
                    v->ref = NULL;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .outside                              */
    case DILF_OUT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
                v->val.ptr = UNIT_IN((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .inside                             */
    case DILF_INS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
                v->val.ptr = UNIT_CONTAINS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .rooms                      */
    case DILF_ROOMS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
                v->val.ptr = ((class zone_type *)v1->val.ptr)->rooms;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .npcs                       */
    case DILF_NPCS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
                v->val.ptr = ((class zone_type *)v1->val.ptr)->npcs;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .objects                            */
    case DILF_OBJS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
                v->val.ptr = ((class zone_type *)v1->val.ptr)->objects;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .gnext                              */
    case DILF_GNX:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
                v->val.ptr = ((class unit_data *)v1->val.ptr)->gnext;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .loadcount                          */
    case DILF_LCN:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                v->val.num = UNIT_FILE_INDEX((class unit_data *)v1->val.ptr) ? UNIT_FILE_INDEX((class unit_data *)v1->val.ptr)->no_in_mem : 1;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .roomcount                          */
    case DILF_NROOMS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                v->val.num = ((class zone_type *)v1->val.ptr)->no_rooms;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .objcount                           */
    case DILF_NOBJS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                v->val.num = ((class zone_type *)v1->val.ptr)->no_objs;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .npccount                           */
    case DILF_NNPCS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_INT;
                v->val.num = ((class zone_type *)v1->val.ptr)->no_npcs;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .gprevious                          */
    case DILF_GPR:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
                v->val.ptr = ((class unit_data *)v1->val.ptr)->gprevious;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .light                              */
    case DILF_LGT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = UNIT_LIGHTS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
            break;
        }
        break;

    /* *********************************** */
    /* .bright                             */
    case DILF_BGT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = UNIT_BRIGHT((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .illum                              */
    case DILF_ILL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = UNIT_ILLUM((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .flags                              */
    case DILF_FL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                /* ubit16 flags; */
                v->atyp = DILA_NONE;
                v->type = DILV_UINT2R;
                v->ref = &UNIT_FLAGS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .manipulate                         */
    case DILF_MAN:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT4R;
                v->ref = &UNIT_MANIPULATE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .payonly                                    */
    case DILF_PAYONLY:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &(((class zone_type *)v1->val.ptr)->payonly);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .loadlevel                                  */
    case DILF_LDLVL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &(((class zone_type *)v1->val.ptr)->loadlevel);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .reset_mode                                         */
    case DILF_RSTMD:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &(((class zone_type *)v1->val.ptr)->reset_mode);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .reset_time                                         */
    case DILF_RSTTM:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT2R;
                v->ref = &(((class zone_type *)v1->val.ptr)->zone_time);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .flags openflags                    */
    case DILF_OFL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                /* ubit8 openflags */
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &UNIT_OPEN_FLAGS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .opendiff                      */
    case DILF_ODI:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                /* ubit8 openflags */
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &UNIT_OPEN_DIFF((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .lifespan                           */
    case DILF_LSA:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;

                if (p->frame[0].tmpl->zone->access != 0)
                {
                    v->type = DILV_INT;
                    v->val.num = PC_LIFESPAN((class unit_data *)v1->val.ptr);
                }
                else
                {
                    v->type = DILV_UINT2R;
                    v->ref = &PC_LIFESPAN((class unit_data *)v1->val.ptr);
                }
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .access                             */
    case DILF_ACCESS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = ((class zone_type *)v1->val.ptr)->access;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    /* *********************************** */
    /* .max_mana                           */
    case DILF_MMA:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = mana_limit((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .max_endurance                      */
    case DILF_MED:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = move_limit((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .max_hp                             */
    case DILF_MHP:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;

                if (IS_PC((class unit_data *)v1->val.ptr))
                {
                    if (p->frame[0].tmpl->zone->access != 0)
                    {
                        v->type = DILV_INT;
                        v->val.num =
                            UNIT_MAX_HIT((class unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_SINT4R;
                        v->ref =
                            &UNIT_MAX_HIT((class unit_data *)v1->val.ptr);
                    }
                }
                else
                {
                    v->type = DILV_SINT4R;
                    v->ref = &UNIT_MAX_HIT((class unit_data *)v1->val.ptr);
                }
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .hp                                 */
    case DILF_CHP:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                /* sbit32 hp; */
                v->atyp = DILA_NONE;
                v->type = DILV_SINT4R;
                v->ref = &UNIT_HIT((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

        /* *********************************** */
        /* .baseweight                         */

    case DILF_BWT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT2R;
                v->ref = &UNIT_BASE_WEIGHT((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

        /* *********************************** */
        /* .editing                             */

    case DILF_EDT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                int editing = FALSE;
                class unit_data *vict;
                vict = ((class unit_data *)v1->val.ptr);
                if (IS_PC(vict) && CHAR_DESCRIPTOR(vict) == NULL)
                {
                    class descriptor_data *d;
                    for (d = descriptor_list; d; d = d->next)
                    {
                        if (descriptor_is_playing(d) && d->original == vict)
                        {
                            if (d->editing)
                                editing = TRUE;
                            break;
                        }
                    }
                }
                else
                {
                    if (IS_PC(vict) && CHAR_DESCRIPTOR(vict))
                    {
                        if (CHAR_DESCRIPTOR(vict)->editing)
                            editing = TRUE;
                    }
                }
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = editing;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

        /* *********************************** */
        /* .switched                            */

    case DILF_SWT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                class unit_data *switched = NULL;
                class unit_data *vict;
                vict = ((class unit_data *)v1->val.ptr);
                if (IS_PC(vict) && CHAR_DESCRIPTOR(vict) == NULL)
                {
                    class descriptor_data *d;
                    for (d = descriptor_list; d; d = d->next)
                    {
                        if (descriptor_is_playing(d) && d->original == vict)
                        {
                            switched = d->character;
                            break;
                        }
                    }
                }
                else
                {
                    if (IS_NPC(vict) && CHAR_DESCRIPTOR(vict))
                    {
                        if (CHAR_DESCRIPTOR(vict)->original)
                            switched = CHAR_DESCRIPTOR(vict)->character;
                    }
                }
                v->atyp = DILA_NORM;
                v->type = DILV_UP;
                v->val.ptr = switched;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .weight                             */
    case DILF_WGT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT2R;
                v->ref = &UNIT_WEIGHT((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .capacity                           */
    case DILF_CAP:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                /* sbit16 capacity */
                v->atyp = DILA_NONE;
                v->type = DILV_SINT2R;
                v->ref = &UNIT_CAPACITY((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .alignment                         */
    case DILF_ALG:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                /* sbit16 capacity */
                v->atyp = DILA_NONE;
                v->type = DILV_SINT2R;
                v->ref = &UNIT_ALIGNMENT((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .spells [ index ]                   */
    case DILF_SPL:
        v2 = p->stack.pop();
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->type = DILV_INT;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr)
            {
                switch (UNIT_TYPE((class unit_data *)v1->val.ptr))
                {
                case UNIT_ST_ROOM:
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;
                    v->ref = &ROOM_RESISTANCE((class unit_data *)v1->val.ptr);
                    break;

                case UNIT_ST_OBJ:
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;
                    v->ref = &OBJ_RESISTANCE((class unit_data *)v1->val.ptr);
                    break;

                case UNIT_ST_NPC:
                    if (is_in(v2->val.num, 0, SPL_GROUP_MAX - 1))
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref =
                            &NPC_SPL_SKILL((class unit_data *)v1->val.ptr,
                                           v2->val.num);
                    }
                    else
                        v->type = DILV_FAIL; /* illegal index */
                    break;

                case UNIT_ST_PC:
                    if (is_in(v2->val.num, 0, SPL_TREE_MAX - 1))
                    {
                        if (p->frame[0].tmpl->zone->access == 0)
                        {
                            v->atyp = DILA_NONE;
                            v->type = DILV_SINT2R;
                            v->ref =
                                &PC_SPL_SKILL((class unit_data *)v1->val.ptr,
                                              v2->val.num);
                        }
                        else
                        {
                            v->atyp = DILA_NONE;
                            v->type = DILV_INT;
                            v->val.num =
                                PC_SPL_SKILL((class unit_data *)v1->val.ptr,
                                             v2->val.num);
                        }
                    }
                    else
                        v->type = DILV_FAIL; /* illegal index */
                    break;
                default:
                    v->type = DILV_ERR; /* illegal type */
                    break;
                }
            }
            else
                v->type = DILV_FAIL; /* illegal type */
        }
        break;

    /* *********************************** */
    /* .hasfunc                          */
    case DILF_FUN:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num =
                    (UNIT_FUNC((class unit_data *)v1->val.ptr) != NULL);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .zone                             */
    case DILF_ZON:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SP;
                v->val.ptr = unit_zone((class unit_data *)v1->val.ptr)->name;
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .objecttype                       */
    case DILF_OTY:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_OBJ((class unit_data *)v1->val.ptr))
            {
                v->type = DILV_UINT1R;
                v->atyp = DILA_NONE;
                v->ref = &OBJ_TYPE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .value [ index ]                   */
    case DILF_VAL:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->type = DILV_UINT4R;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_UINT4R)
        {
            if ((v1->val.ptr) && IS_OBJ((class unit_data *)v1->val.ptr) &&
                (v2->val.num >= 0) && (v2->val.num < 5))
            {
                v->atyp = DILA_NONE;
                v->ref =
                    &OBJ_VALUE((class unit_data *)v1->val.ptr, v2->val.num);
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .objectflags                        */
    case DILF_EFL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_OBJ((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &OBJ_FLAGS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .cost                             */
    case DILF_CST:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_OBJ((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT4R;
                v->ref = &OBJ_PRICE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .rent                             */
    case DILF_RNT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_OBJ((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT4R;
                v->ref = &OBJ_PRICE_DAY((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .equip                              */
    case DILF_EQP:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_OBJ((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = OBJ_EQP_POS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .exit_key [ index ]                 */
    case DILF_EXITKEY:
        v2 = p->stack.pop(); /* evaluate index */

        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->type = DILV_SPR;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_SPR)
        {
            if (v1->val.ptr && IS_ROOM((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, MAX_EXIT) &&
                ROOM_EXIT((class unit_data *)v1->val.ptr, v2->val.num))
            {
                v->atyp = DILA_NORM;
                v->ref = &(ROOM_EXIT((class unit_data *)v1->val.ptr,
                                     v2->val.num)
                               ->key);
            }
            else
                v->type = DILV_FAIL;
        }

        break;

    /* *********************************** */
    /* .exit_names [ index ]               */
    case DILF_ONM:
        v2 = p->stack.pop(); /* evaluate index */

        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->type = DILV_SLPR;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_SLPR)
        {
            if (v1->val.ptr && IS_ROOM((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, MAX_EXIT) &&
                ROOM_EXIT((class unit_data *)v1->val.ptr, v2->val.num))
            {
                v->atyp = DILA_NORM;
                v->ref = &(ROOM_EXIT((class unit_data *)v1->val.ptr,
                                     v2->val.num)
                               ->open_name);
            }
            else
                v->type = DILV_FAIL;
        }

        break;

    /* *********************************** */
    /* .exit_info [ index ]                */
    case DILF_XNF:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->type = DILV_UINT1R;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_UINT1R)
        {
            if (v1->val.ptr && IS_ROOM((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, MAX_EXIT) &&
                ROOM_EXIT((class unit_data *)v1->val.ptr, v2->val.num))
            {
                v->atyp = DILA_NONE;
                v->ref = &(ROOM_EXIT((class unit_data *)v1->val.ptr,
                                     v2->val.num)
                               ->exit_info);
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .exit_diff [ index ]                */
    case DILF_XDIFF:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->type = DILV_UINT1R;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_UINT1R)
        {
            if (v1->val.ptr && IS_ROOM((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, MAX_EXIT) &&
                ROOM_EXIT((class unit_data *)v1->val.ptr, v2->val.num))
            {
                v->atyp = DILA_NONE;
                v->ref = &(ROOM_EXIT((class unit_data *)v1->val.ptr,
                                     v2->val.num)
                               ->difficulty);
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .exit_to[ [ index ]                 */
    case DILF_TOR:
        v2 = p->stack.pop();
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->type = DILV_UP;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_UP)
        {
            if (v1->val.ptr && IS_ROOM((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, MAX_EXIT) &&
                ROOM_EXIT((class unit_data *)v1->val.ptr, v2->val.num))
            {
                v->atyp = DILA_NORM;
                v->val.ptr = ROOM_EXIT((class unit_data *)v1->val.ptr,
                                       v2->val.num)->to_room;
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .roomflags                          */
    case DILF_RFL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_ROOM((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &ROOM_FLAGS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .mapx                               */
    case DILF_MAPX:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->atyp = DILA_NORM;
            v->type = DILV_INT;
            if ((v1->val.ptr && IS_ROOM((class unit_data *)v1->val.ptr)))
                v->val.num = UROOM((class unit_data *)v1->val.ptr)->mapx;
            else
                v->val.num = -1;

            break;
            v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .mapy                               */
    case DILF_MAPY:
        switch (dil_getval(v1))
        {
        case DILV_NULL:
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->atyp = DILA_NORM;
            v->type = DILV_INT;
            if ((v1->val.ptr && IS_ROOM((class unit_data *)v1->val.ptr)))
                v->val.num = UROOM((class unit_data *)v1->val.ptr)->mapy;
            else
                v->val.num = -1;

            break;
            v->type = DILV_FAIL; /* not applicable */
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .movement                           */
    case DILF_MOV:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_ROOM((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &ROOM_LANDSCAPE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .offensive                                */
    case DILF_OFFEN:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT2R;
                v->ref = &CHAR_OFFENSIVE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .defensive                                */
    case DILF_DEFEN:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT2R;
                v->ref = &CHAR_DEFENSIVE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .sex                                */
    case DILF_SEX:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT1R;
                v->ref = &CHAR_SEX((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .race                               */
    case DILF_RCE:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT2R;
                v->ref = &CHAR_RACE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .abilities [ index ]                */
    case DILF_ABL:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_INT:
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
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, ABIL_TREE_MAX - 1))
            {
                if (IS_PC((class unit_data *)v1->val.ptr))
                {
                    if (p->frame[0].tmpl->zone->access == 0)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref = &CHAR_ABILITY((class unit_data *)
                                                   v1->val.ptr,
                                               v2->val.num);
                    }
                    else
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = CHAR_ABILITY((class unit_data *)
                                                      v1->val.ptr,
                                                  v2->val.num);
                    }
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT2R;
                    v->ref = &CHAR_ABILITY((class unit_data *)
                                               v1->val.ptr,
                                           v2->val.num);
                }
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .ability_levels [ index ]                */
    case DILF_ABL_L:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_INT:
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
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr &&
                IS_PC((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, ABIL_TREE_MAX - 1))
            {
                if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;
                    v->ref = &PC_ABI_LVL((class unit_data *)
                                             v1->val.ptr,
                                         v2->val.num);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_ABI_LVL((class unit_data *)
                                                v1->val.ptr,
                                            v2->val.num);
                }
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .ability_costs [ index ]                */
    case DILF_ABL_C:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_INT:
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
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr &&
                IS_PC((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, ABIL_TREE_MAX - 1))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = get_racial_ability(CHAR_RACE((class unit_data *)v1->val.ptr), v2->val.num);

                /*if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;
                    v->ref = &PC_ABI_COST((class unit_data *)
                                              v1->val.ptr,
                                          v2->val.num);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_ABI_COST((class unit_data *)
                                                 v1->val.ptr,
                                             v2->val.num);
                }*/
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .spell_levels [ index ]                */
    case DILF_SPL_L:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_INT:
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
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr &&
                IS_PC((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, SPL_TREE_MAX - 1))
            {
                if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;

                    v->ref = &PC_SPL_LVL((class unit_data *)v1->val.ptr, v2->val.num);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_SPL_LVL((class unit_data *)v1->val.ptr, v2->val.num);
                }
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .spell_costs [ index ]                */
    case DILF_SPL_C:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_INT:
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
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr &&
                IS_PC((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, SPL_TREE_MAX - 1))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = get_racial_spells(CHAR_RACE((class unit_data *)v1->val.ptr), v2->val.num);

                /*if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;
                    v->ref = &PC_SPL_COST((class unit_data *)v1->val.ptr,
                                          v2->val.num);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_SPL_COST((class unit_data *)v1->val.ptr,
                                             v2->val.num);
                }*/
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .skill_levels [ index ]                */
    case DILF_SKL_L:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_INT:
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
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr &&
                IS_PC((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, SKI_TREE_MAX - 1))
            {
                if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;
                    v->ref = &PC_SKI_LVL((class unit_data *)v1->val.ptr,
                                         v2->val.num);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_SKI_LVL((class unit_data *)v1->val.ptr,
                                            v2->val.num);
                }
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .skill_costs [ index ]                */
    case DILF_SKL_C:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_INT:
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
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr &&
                IS_PC((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, SKI_TREE_MAX - 1))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = get_racial_skill(CHAR_RACE((class unit_data *)v1->val.ptr), v2->val.num);
                /*if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;
                    v->ref = &PC_SKI_COST((class unit_data *)v1->val.ptr,
                                          v2->val.num);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_SKI_COST((class unit_data *)v1->val.ptr,
                                             v2->val.num);
                }*/
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .weapon_levels [ index ]                */
    case DILF_WPN_L:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_INT:
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
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr &&
                IS_PC((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, WPN_TREE_MAX - 1))
            {
                if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;
                    v->ref = &PC_WPN_LVL((class unit_data *)v1->val.ptr,
                                         v2->val.num);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_WPN_LVL((class unit_data *)v1->val.ptr,
                                            v2->val.num);
                }
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .weapon_costs [ index ]                */
    case DILF_WPN_C:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v2))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_INT:
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
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr &&
                IS_PC((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, WPN_TREE_MAX - 1))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = get_racial_weapon(CHAR_RACE((class unit_data *)v1->val.ptr), v2->val.num);
                /*if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_UINT1R;
                    v->ref = &PC_WPN_COST((class unit_data *)v1->val.ptr,
                                          v2->val.num);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_WPN_COST((class unit_data *)v1->val.ptr,
                                             v2->val.num);
                }*/
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .exp                                */
    case DILF_EXP:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_SINT4R;
                    v->ref = &CHAR_EXP((class unit_data *)v1->val.ptr);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = CHAR_EXP((class unit_data *)v1->val.ptr);
                }
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;
    /* *********************************** */
    /* .exp                                */
    case DILF_EXPTOL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = required_xp(PC_VIRTUAL_LEVEL((class unit_data *)
                                                              v1->val.ptr) +
                                         1) -
                             CHAR_EXP((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .level                              */
    case DILF_LVL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                if ((IS_PC((class unit_data *)v1->val.ptr)) && (p->frame[0].tmpl->zone->access != 0))
                {
                    v->type = DILV_INT;
                    v->val.num = CHAR_LEVEL((class unit_data *)v1->val.ptr);
                }
                else
                {
                    v->type = DILV_UINT1R;
                    v->ref = &CHAR_LEVEL((class unit_data *)v1->val.ptr);
                }
            }
            else
                v->type = DILV_FAIL;
            break;

        case DILV_CP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num =
                    (((struct command_info *)v1->val.ptr)->minimum_level);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .profession                         */
    case DILF_PROF:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                if ((IS_PC((class unit_data *)v1->val.ptr)) && (p->frame[0].tmpl->zone->access != 0))
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_PROFESSION((class unit_data *)v1->val.ptr);
                }
                else
                {
                    v->type = DILV_UINT1R;
                    v->ref = &PC_PROFESSION((class unit_data *)v1->val.ptr);
                }
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .vlevel                             */
    case DILF_VLVL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                if (p->frame[0].tmpl->zone->access != 0)
                {
                    v->type = DILV_INT;
                    v->val.num =
                        PC_VIRTUAL_LEVEL((class unit_data *)v1->val.ptr);
                }
                else
                {
                    v->type = DILV_UINT2R;
                    v->ref =
                        &PC_VIRTUAL_LEVEL((class unit_data *)v1->val.ptr);
                }
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .size                               */
    case DILF_HGT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT2R;
                v->ref = &UNIT_SIZE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .position                           */
    case DILF_POS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;

        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT1R;
                v->ref = &CHAR_POS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;
        case DILV_CP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref =
                    &(((struct command_info *)v1->val.ptr)->minimum_position);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .attack_type                        */
    case DILF_ATY:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &CHAR_ATTACK_TYPE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .mana                               */
    case DILF_MNA:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT2R;
                v->ref = &CHAR_MANA((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .endurance                          */
    case DILF_END:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT2R;
                v->ref = &CHAR_ENDURANCE((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .naturalarmour                      */
    case DILF_NARM:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &CHAR_NATURAL_ARMOUR((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .charflags                          */
    case DILF_AFF:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT4R;
                v->ref = &CHAR_FLAGS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .fighting                           */
    case DILF_FGT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UP;
                v->val.ptr = CHAR_FIGHTING((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .weapons [ index ]                  */
    case DILF_WPN:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->type = DILV_INT;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr)
            {
                switch (UNIT_TYPE((class unit_data *)v1->val.ptr))
                {
                case UNIT_ST_NPC:
                    if (is_in(v2->val.num, 0, WPN_GROUP_MAX - 1))
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref = &NPC_WPN_SKILL((class unit_data *)
                                                    v1->val.ptr,
                                                v2->val.num);
                    }
                    else
                        v->type = DILV_FAIL;
                    break;
                case UNIT_ST_PC:
                    if (is_in(v2->val.num, 0, WPN_TREE_MAX - 1))
                    {
                        if (p->frame[0].tmpl->zone->access == 0)
                        {
                            v->atyp = DILA_NONE;
                            v->type = DILV_SINT2R;
                            v->ref = &PC_WPN_SKILL((class unit_data *)
                                                       v1->val.ptr,
                                                   v2->val.num);
                        }
                        else
                        {
                            v->atyp = DILA_NONE;
                            v->type = DILV_INT;
                            v->val.num = PC_WPN_SKILL((class unit_data *)
                                                          v1->val.ptr,
                                                      v2->val.num);
                        }
                    }
                    else
                        v->type = DILV_FAIL;
                    break;

                default:
                    v->type = DILV_ERR; /* illegal type */
                    break;
                }
            }
            else
                v->type = DILV_FAIL; /* illegal type */
        }
        break;

    /* *********************************** */
    /* .defaultpos                         */
    case DILF_DEF:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_NPC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &NPC_DEFAULT((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .npcflags                           */
    case DILF_ACT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_NPC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &NPC_FLAGS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .pcflags                            */
    case DILF_PCF:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT2R;
                v->ref = &PC_FLAGS((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .birth                              */
    case DILF_BIR:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT4R;
                v->ref = &PC_TIME((class unit_data *)v1->val.ptr).birth;
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .playtime                           */
    case DILF_PTI:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_INT;
                v->val.num = PC_TIME((class unit_data *)v1->val.ptr).played;
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .full                               */
    case DILF_FLL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT1R;
                v->ref = &PC_COND((class unit_data *)v1->val.ptr, FULL);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .thirst                             */
    case DILF_THR:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT1R;
                v->ref = &PC_COND((class unit_data *)v1->val.ptr, THIRST);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .drunk                              */
    case DILF_DRK:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT1R;
                v->ref = &PC_COND((class unit_data *)v1->val.ptr, DRUNK);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .skill_points                           */
    case DILF_SPT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT4R;
                v->ref = &PC_SKILL_POINTS((class unit_data *)v1->val.ptr);
                /*	    v->val.num = &PC_SKILL_POINTS((class unit_data *) v1->val.ptr);*/
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .ability_points                     */
    case DILF_APT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_SINT4R;
                v->ref = &PC_ABILITY_POINTS((class unit_data *)v1->val.ptr);
                /*	    v->val.num = &PC_ABILITY_POINTS((class unit_data *) v1->val.ptr);*/
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .guild                              */
    case DILF_GLD:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SPR;
                v->ref = &PC_GUILD((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .prompt                                     */
    case DILF_PROMPT:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SPR;
                v->ref = &PC_PROMPTSTR((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .key                                */
    case DILF_KEY:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SPR;
                v->ref = &UNIT_KEY((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .filename                                   */
    case DILF_FNAME:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SPR;
                v->ref = &(((class zone_type *)v1->val.ptr)->filename);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .notes                              */
    case DILF_NOTES:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SPR;
                v->ref = &(((class zone_type *)v1->val.ptr)->notes);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .help                               */
    case DILF_HELP:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_ZP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_SPR;
                v->ref = &(((class zone_type *)v1->val.ptr)->help);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .crimes                             */
    case DILF_CRM:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT2R;
                v->ref = &PC_CRIMES((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .quests                             */
    case DILF_QST:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_EDPR;
                v->ref = &(PC_QUEST((class unit_data *)v1->val.ptr).m_pList);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .info                               */
    case DILF_INFO:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_EDPR;
                v->ref = &(PC_INFO((class unit_data *)v1->val.ptr).m_pList);
            }
            else
                v->type = DILV_FAIL;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .skills [ index ]                  */
    case DILF_SKL:
        v2 = p->stack.pop(); /* evaluate index */
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            v->type = DILV_INT;
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            if (v->type != DILV_ERR)
                v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_INT:
            break;
        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }

        if (v->type == DILV_INT)
        {
            if (v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr) &&
                is_in(v2->val.num, 0, SKI_TREE_MAX - 1))
            {
                if (p->frame[0].tmpl->zone->access == 0)
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_SINT2R;
                    v->ref = &PC_SKI_SKILL((class unit_data *)v1->val.ptr,
                                           v2->val.num);
                }
                else
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = PC_SKI_SKILL((class unit_data *)v1->val.ptr,
                                              v2->val.num);
                }
            }
            else
                v->type = DILV_FAIL;
        }
        break;

    /* *********************************** */
    /* .master                             */
    case DILF_MAS:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                v->atyp = DILA_NORM;
                v->type = DILV_UPR;
                v->ref = &CHAR_MASTER((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .follower                           */
    case DILF_FOL:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
            {
                if (CHAR_FOLLOWERS((class unit_data *)v1->val.ptr))
                {
                    v->atyp = DILA_NORM;
                    v->type = DILV_UPR;
                    v->ref =
                        &CHAR_FOLLOWERS((class unit_data *)v1->val.ptr)->follower;
                }
                else
                {
                    v->atyp = DILA_NORM;
                    v->type = DILV_NULL;
                    v->val.ptr = NULL;
                }
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    /* *********************************** */
    /* .minv                               */
    case DILF_MIV:
        switch (dil_getval(v1))
        {
        case DILV_NULL: /* not applicable */
        case DILV_FAIL:
            v->type = DILV_FAIL; /* not applicable */
            break;
        case DILV_UP:
            if (v1->val.ptr)
            {
                v->atyp = DILA_NONE;
                v->type = DILV_UINT1R;
                v->ref = &UNIT_MINV((class unit_data *)v1->val.ptr);
            }
            else
                v->type = DILV_FAIL;
            break;

        default:
            v->type = DILV_ERR; /* wrong type */
            break;
        }
        break;

    default:
        szonelog(UNIT_FI_ZONE(p->sarg->owner),
                 "DIL %s@%s, unknown field type: %d\n",
                 UNIT_FI_NAME(p->sarg->owner),
                 UNIT_FI_ZONENAME(p->sarg->owner), fldno);
        p->waitcmd = WAITCMD_QUIT;
        v->type = DILV_ERR;
        break;
    }
    p->stack.push(v);
    delete v1;
    if (v2)
        delete v2;
}
