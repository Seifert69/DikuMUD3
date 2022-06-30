/*
 $Author: All $
 $RCSfile: dilfld.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.11 $
 */

#include "char_follow_type.h"
#include "combat.h"
#include "common.h"
#include "config.h"
#include "db.h"
#include "dil.h"
#include "dilexp.h"
#include "dilrun.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "main_functions.h"
#include "skills.h"
#include "szonelog.h"
#include "textutil.h"
#include "utils.h"
#include "vmelimits.h"

void dilfe_fld(dilprg *p)
{
    /* Get a structure field */
    dilval *v1 = nullptr;
    dilval *v2 = nullptr;
    v1 = p->stack.pop();
    v2 = nullptr;
    dilval *v = new dilval;
    int fldno = 0;

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
                    {
                        v->type = DILV_SP;
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* NULL list */
                    }
                    break;
                case DILV_ILP:
                    if (v1->val.ptr)
                    {
                        v->type = DILV_INT;
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* NULL list */
                    }
                    break;
                case DILV_EDP:
                    if (v1->val.ptr)
                    {
                        v->type = DILV_EDP;
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    if (v->type != DILV_ERR)
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_INT:
                case DILV_SP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
            }

            if (v->type == DILV_INT)
            {
                if (((sbit32)v2->val.num < ((cintlist *)v1->val.ptr)->Length()) && (v2->val.num >= 0))
                {
                    v->atyp = DILA_NONE; // Dont dealloc!
                    v->type = DILV_SINT4R;
                    v->ref = (((cintlist *)v1->val.ptr)->ValuePtr(v2->val.num));
                }
                else
                {
                    v->type = DILV_FAIL; /* illegal index */
                }
            }
            else if (v->type == DILV_EDP)
            {
                v->atyp = DILA_NORM;
                v->type = DILV_EDP;
                if (v1->val.ptr)
                {
                    v->val.ptr = ((extra_descr_data *)v1->val.ptr)->find_raw(skip_spaces((char *)v2->val.ptr));
                }
                else
                {
                    v->val.ptr = nullptr;
                }
            }
            else if (v->type == DILV_SP)
            {
                if (dil_getval(v1) == DILV_SP)
                {
                    if ((v2->val.num >= 0) && (v2->val.num < (int)strlen((char *)v1->val.ptr)))
                    {
                        v->atyp = DILA_EXP;
                        v->type = DILV_SP;
                        v->val.ptr = (char *)malloc(2);
                        ((char *)v->val.ptr)[0] = ((char *)v1->val.ptr)[v2->val.num];
                        ((char *)v->val.ptr)[1] = 0;
                    }
                    else
                    {
                        szonelog(p->sarg->owner->getFileIndex()->getZone(),
                                 "DIL %s %s@%s, index of stringlist out of bounds: %d\n",
                                 p->sarg->owner->getFileIndexSymName(),
                                 p->fp->tmpl->prgname,
                                 p->fp->tmpl->zone->getName(),
                                 v2->val.num);

                        v->type = DILV_FAIL;
                    }
                }
                else
                {
                    v->atyp = DILA_EXP;

                    if (((ubit32)v2->val.num < ((cNamelist *)v1->val.ptr)->Length()) && (v2->val.num >= 0))
                    {
                        v->atyp = DILA_NONE; // Dont dealloc!
                        v->type = DILV_HASHSTR;
                        v->ref = ((cNamelist *)v1->val.ptr)->InstanceName(v2->val.num);
                    }
                    else
                    {
                        szonelog(p->sarg->owner->getFileIndex()->getZone(),
                                 "DIL %s %s@%s, index of stringlist out of bounds: %d\n",
                                 p->sarg->owner->getFileIndexSymName(),
                                 p->fp->tmpl->prgname,
                                 p->fp->tmpl->zone->getName(),
                                 v2->val.num);
                        v->type = DILV_FAIL; /* illegal index */
                    }
                }
            }
            else
            {
                v->type = DILV_FAIL; /* illegal index */
            }
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
                        if (((unit_data *)v1->val.ptr)->isPC())
                        {
                            v->val.num = (int)PC_ID((unit_data *)v1->val.ptr);
                        }
                        else
                        {
                            v->val.ptr = v1->val.ptr; // Since it is a union, the int will be set
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                    if ((v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar()))
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_INT;
                        v->val.num = CHAR_SPEED((unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_CP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_INT;
                        v->val.num = (((command_info *)v1->val.ptr)->combat_speed);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                    if ((v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar()))
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_INT;
                        if (CHAR_COMBAT((unit_data *)v1->val.ptr))
                        {
                            v->val.num = CHAR_COMBAT((unit_data *)v1->val.ptr)->NoOpponents();
                        }
                        else
                        {
                            v->val.num = 0;
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                    if ((v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar()))
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_INT;
                        v->val.num = follower_count((unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.num = (((command_info *)v1->val.ptr)->log_level);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                {
                    auto *player = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (player && player->isPC())
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_INT;
                        if (g_cServerConfig.isAccounting())
                        {
                            v->val.num = player->getPCAccountData().getTotalCredit();
                        }
                        else
                        {
                            v->val.num = 0;
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *player = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (player && player->isPC())
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_INT;
                        if (g_cServerConfig.isAccounting())
                        {
                            v->val.num = static_cast<int>(player->getPCAccountData().getAccountBalance());
                        }
                        else
                        {
                            v->val.num = 0;
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                        v->val.ptr = (char *)((unit_data *)v1->val.ptr)->getFileIndexZoneName();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }
            break;

        /* *********************************** */
        /* .nameidx                            */
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
                        v->val.ptr = (char *)((unit_data *)v1->val.ptr)->getFileIndexName();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }
            break;

        /* *********************************** */
        /* .symname                            */
        case DILF_SYMNAME:
            switch (dil_getval(v1))
            {
                case DILV_NULL:
                case DILV_FAIL:
                    v->type = DILV_FAIL; /* not applicable */
                    break;
                case DILV_UP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_EXP;
                        v->type = DILV_SP;

                        auto buf = diku::format_to_str("%s", ((unit_data *)v1->val.ptr)->getFileIndexSymName());
                        v->val.ptr = strdup(buf.c_str());
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.num = ((unit_data *)v1->val.ptr)->getUnitType();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_CP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = (((command_info *)v1->val.ptr)->type);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                    {
                        v->val.ptr = ((extra_descr_data *)v1->val.ptr)->next;
                    }
                    else
                    {
                        v->val.ptr = nullptr;
                    }
                    break;
                case DILV_UP:
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_UP;
                        v->val.ptr = unit->getNext();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
                break;
                case DILV_ZP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_ZP;
                        v->val.ptr = nullptr;

                        zone_type *z = (zone_type *)v1->val.ptr;

                        if (z)
                        {
                            auto it = g_zone_info.mmp.find(z->getName());
                            if (it != g_zone_info.mmp.end())
                            {
                                it++;
                                if (it != g_zone_info.mmp.end())
                                {
                                    v->val.ptr = it->second;
                                }
                            }
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_CP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_CP;
                        v->val.ptr = ((command_info *)v1->val.ptr)->next;
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        if (v1->val.ptr)
                        { // MS2020 bug
                            v->ref = &(((extra_descr_data *)v1->val.ptr)->names);
                        }
                        else
                        {
                            v->ref = nullptr;
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_SLPR;
                        v->ref = &unit->getNames();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                        v->ref = &(((zone_type *)v1->val.ptr)->getCreators());
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        {
                            v->val.ptr = (void *)IF_STR(((extra_descr_data *)v1->val.ptr)->names.Name());
                        }
                        else
                        {
                            v->val.ptr = (void *)""; // MS2020 SPOOKY? IS THIS RIGHT?IF_STR returns ""
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;

                case DILV_UP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_SP;
                        v->val.ptr = (void *)IF_STR(((unit_data *)v1->val.ptr)->getNames().Name());
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_ZP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_SP;
                        v->val.ptr = (void *)IF_STR(((zone_type *)v1->val.ptr)->getName());
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                    break;
                case DILV_CP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_SP;
                        v->val.ptr = (void *)IF_STR(((command_info *)v1->val.ptr)->cmd_str);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
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
                        if (v1->val.ptr)
                        { // MS2020
                            v->ref = &(((extra_descr_data *)v1->val.ptr)->vals);
                        }
                        else
                        {
                            v->ref = nullptr;
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        {
                            v->ref = &(((extra_descr_data *)v1->val.ptr)->descr);
                        }
                        else
                        {
                            v->ref = nullptr;
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NONE; // Dont dealloc!
                        v->type = DILV_HASHSTR;
                        v->ref = unit->getDescriptionOfOutsidePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NONE; // Dont dealloc!
                        v->type = DILV_HASHSTR;
                        v->ref = unit->getDescriptionOfInsidePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NONE; // Dont dealloc!
                        v->type = DILV_HASHSTR;
                        v->ref = unit->getTitlePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
                break;
                case DILV_ZP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NONE; // Dont dealloc!
                        v->type = DILV_SPR;
                        v->ref = ((zone_type *)v1->val.ptr)->getTitlePtrPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_SPR;
                        v->ref = pc->getHometownPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isPC())
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_UPR;
                        v->ref = character->getLastLocation();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                        if (v1->val.ptr)
                        { // MS2020 BUG
                            v->ref = &(((unit_data *)v1->val.ptr)->getExtraList().m_pList);
                        }
                        else
                        {
                            v->ref = nullptr;
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.ptr = ((unit_data *)v1->val.ptr)->getUnitIn();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.ptr = ((unit_data *)v1->val.ptr)->getUnitContains();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        zone_type *z = (zone_type *)v1->val.ptr;

                        for (auto fi = z->cgetFileIndexMap().begin(); fi != z->cgetFileIndexMap().end(); fi++)
                        {
                            if (fi->second->getType() == UNIT_ST_ROOM)
                            {
                                if (fi->second->Empty())
                                {
                                    v->type = DILV_FAIL;
                                    v->val.ptr = nullptr;
                                }
                                else
                                {
                                    v->val.ptr = fi->second->Front();
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        zone_type *z = (zone_type *)v1->val.ptr;

                        for (auto fi = z->cgetFileIndexMap().begin(); fi != z->cgetFileIndexMap().end(); fi++)
                        {
                            if (fi->second->getType() == UNIT_ST_NPC)
                            {
                                if (fi->second->Empty())
                                {
                                    v->type = DILV_FAIL;
                                    v->val.ptr = nullptr;
                                }
                                else
                                {
                                    v->val.ptr = fi->second->Front();
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        zone_type *z = (zone_type *)v1->val.ptr;

                        for (auto fi = z->cgetFileIndexMap().begin(); fi != z->cgetFileIndexMap().end(); fi++)
                        {
                            if (fi->second->getType() == UNIT_ST_OBJ)
                            {
                                if (fi->second->Empty())
                                {
                                    v->type = DILV_FAIL;
                                    v->val.ptr = nullptr;
                                }
                                else
                                {
                                    v->val.ptr = fi->second->Front();
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_UP;
                        v->val.ptr = unit->getGlobalNext();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                        v->val.num =
                            ((unit_data *)v1->val.ptr)->getFileIndex() ? ((unit_data *)v1->val.ptr)->getFileIndex()->getNumInMem() : 1;
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.num = ((zone_type *)v1->val.ptr)->getNumOfRooms();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.num = ((zone_type *)v1->val.ptr)->getNumOfObjects();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.num = ((zone_type *)v1->val.ptr)->getNumOfNPCs();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_UP;
                        v->val.ptr = unit->getGlobalPrevious();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                        v->val.num = ((unit_data *)v1->val.ptr)->getNumberOfActiveLightSources();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.num = ((unit_data *)v1->val.ptr)->getLightOutput();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.num = ((unit_data *)v1->val.ptr)->getTransparentLightOutput();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        /* ubit16 flags; */
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT2R;
                        v->ref = unit->getUnitFlagsPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT4R;
                        v->ref = unit->getManipulatePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                        v->ref = ((zone_type *)v1->val.ptr)->getPayOnlyPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->ref = ((zone_type *)v1->val.ptr)->getLevelRequiredToLoadItemsPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->ref = ((zone_type *)v1->val.ptr)->getResetModePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->ref = ((zone_type *)v1->val.ptr)->getZoneResetTimePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        /* ubit8 openflags */
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = unit->getOpenFlagsPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        /* ubit8 openflags */
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = unit->getOpenDifficultyPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NONE;

                        if (p->frame[0].tmpl->zone->getAccessLevel() != 0)
                        {
                            v->type = DILV_INT;
                            v->val.num = pc->getLifespan();
                        }
                        else
                        {
                            v->type = DILV_UINT2R;
                            v->ref = pc->getLifespanPtr();
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                        v->val.num = ((zone_type *)v1->val.ptr)->getAccessLevel();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                    if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = mana_limit((unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                    if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = move_limit((unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NONE;

                        if (unit->isPC())
                        {
                            if (p->frame[0].tmpl->zone->getAccessLevel() != 0)
                            {
                                v->type = DILV_INT;
                                v->val.num = unit->getMaximumHitpoints();
                            }
                            else
                            {
                                v->type = DILV_SINT4R;
                                v->ref = unit->getMaximumHitpointsPtr();
                            }
                        }
                        else
                        {
                            v->type = DILV_SINT4R;
                            v->ref = unit->getMaximumHitpointsPtr();
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        /* sbit32 hp; */
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT4R;
                        v->ref = unit->getCurrentHitpointsPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                        v->type = DILV_INT;
                        v->val.num = ((unit_data *)v1->val.ptr)->getBaseWeight();

                        // v->atyp = DILA_NONE;
                        // v->type = DILV_SINT2R;
                        // v->ref = &UNIT_BASE_WEIGHT((class unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        unit_data *vict = nullptr;
                        vict = ((unit_data *)v1->val.ptr);
                        if (vict->isPC() && CHAR_DESCRIPTOR(vict) == nullptr)
                        {
                            descriptor_data *d = nullptr;
                            for (d = g_descriptor_list; d; d = d->getNext())
                            {
                                if (descriptor_is_playing(d) && d->cgetOriginalCharacter() == vict)
                                {
                                    if (d->cgetEditing())
                                    {
                                        editing = TRUE;
                                    }
                                    break;
                                }
                            }
                        }
                        else
                        {
                            if (vict->isPC() && CHAR_DESCRIPTOR(vict))
                            {
                                if (CHAR_DESCRIPTOR(vict)->cgetEditing())
                                {
                                    editing = TRUE;
                                }
                            }
                        }
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = editing;
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        unit_data *switched = nullptr;
                        unit_data *vict = nullptr;
                        vict = ((unit_data *)v1->val.ptr);
                        if (vict->isPC() && CHAR_DESCRIPTOR(vict) == nullptr)
                        {
                            descriptor_data *d = nullptr;
                            for (d = g_descriptor_list; d; d = d->getNext())
                            {
                                if (descriptor_is_playing(d) && d->cgetOriginalCharacter() == vict)
                                {
                                    switched = d->getCharacter();
                                    break;
                                }
                            }
                        }
                        else
                        {
                            if (vict->isNPC() && CHAR_DESCRIPTOR(vict))
                            {
                                if (CHAR_DESCRIPTOR(vict)->cgetOriginalCharacter())
                                {
                                    switched = CHAR_DESCRIPTOR(vict)->getCharacter();
                                }
                            }
                        }
                        v->atyp = DILA_NORM;
                        v->type = DILV_UP;
                        v->val.ptr = switched;
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->type = DILV_INT;
                        v->val.num = ((unit_data *)v1->val.ptr)->getWeight();

                        // v->atyp = DILA_NONE;
                        // v->type = DILV_SINT2R;
                        // v->ref = &UNIT_WEIGHT((class unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        /* sbit16 capacity */
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref = unit->getCapacityPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        /* sbit16 capacity */
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref = unit->getAlignmentPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                    switch (((unit_data *)v1->val.ptr)->getUnitType())
                    {
                        case UNIT_ST_ROOM:
                        {
                            auto *room = reinterpret_cast<room_data *>(v1->val.ptr);
                            v->atyp = DILA_NONE;
                            v->type = DILV_UINT1R;
                            v->ref = room->getRoomMagicalResistancePtr();
                        }
                        break;

                        case UNIT_ST_OBJ:
                        {
                            auto *object = reinterpret_cast<obj_data *>(v1->val.ptr);
                            v->atyp = DILA_NONE;
                            v->type = DILV_UINT1R;
                            v->ref = object->getMagicResistancePtr();
                        }
                        break;

                        case UNIT_ST_NPC:
                        {
                            auto *npc = reinterpret_cast<npc_data *>(v1->val.ptr);
                            if (is_in(v2->val.num, 0, SPL_GROUP_MAX - 1))
                            {
                                v->atyp = DILA_NONE;
                                v->type = DILV_SINT2R;
                                v->ref = npc->getSpellSkillAtIndexPtr(v2->val.num);
                            }
                            else
                            {
                                v->type = DILV_FAIL; /* illegal index */
                            }
                        }
                        break;

                        case UNIT_ST_PC:
                            if (is_in(v2->val.num, 0, SPL_TREE_MAX - 1))
                            {
                                auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                                if (p->frame[0].tmpl->zone->getAccessLevel() == 0)
                                {
                                    v->atyp = DILA_NONE;
                                    v->type = DILV_SINT2R;
                                    v->ref = pc->getSpellSkillAtIndexPtr(v2->val.num);
                                }
                                else
                                {
                                    v->atyp = DILA_NONE;
                                    v->type = DILV_INT;
                                    v->val.num = pc->getSpellSkillAtIndex(v2->val.num);
                                }
                            }
                            else
                            {
                                v->type = DILV_FAIL; /* illegal index */
                            }
                            break;
                        default:
                            v->type = DILV_ERR; /* illegal type */
                            break;
                    }
                }
                else
                {
                    v->type = DILV_FAIL; /* illegal type */
                }
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
                        v->val.num = (((unit_data *)v1->val.ptr)->getFunctionPointer() != nullptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                        v->val.ptr = unit_zone((unit_data *)v1->val.ptr)->getNamePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                {
                    auto *object = reinterpret_cast<obj_data *>(v1->val.ptr);
                    if (object && object->isObj())
                    {
                        v->type = DILV_UINT1R;
                        v->atyp = DILA_NONE;
                        v->ref = object->getObjectItemTypePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_INT:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_UINT4R)
            {
                auto *object = reinterpret_cast<obj_data *>(v1->val.ptr);
                if (object && object->isObj() && (v2->val.num >= 0 && v2->val.num < static_cast<sbit32>(object->getValueArraySize())))
                {
                    v->atyp = DILA_NONE;
                    v->ref = object->getValueAtIndexPtr(v2->val.num);
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                {
                    auto *object = reinterpret_cast<obj_data *>(v1->val.ptr);
                    if (object && object->isObj())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = object->getObjectFlagsPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *object = reinterpret_cast<obj_data *>(v1->val.ptr);
                    if (object && object->isObj())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT4R;
                        v->ref = object->getPriceInGPPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                {
                    auto *object = reinterpret_cast<obj_data *>(v1->val.ptr);
                    if (object && object->isObj())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT4R;
                        v->ref = object->getPricePerDayPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                }
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
                    if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isObj())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = OBJ_EQP_POS((unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_INT:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_SPR)
            {
                if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isRoom() && is_in(v2->val.num, 0, MAX_EXIT) &&
                    ROOM_EXIT((unit_data *)v1->val.ptr, v2->val.num))
                {
                    v->atyp = DILA_NORM;
                    v->ref = ROOM_EXIT((unit_data *)v1->val.ptr, v2->val.num)->getKeyPtr();
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_INT:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_SLPR)
            {
                if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isRoom() && is_in(v2->val.num, 0, MAX_EXIT) &&
                    ROOM_EXIT((unit_data *)v1->val.ptr, v2->val.num))
                {
                    v->atyp = DILA_NORM;
                    v->ref = &(ROOM_EXIT((unit_data *)v1->val.ptr, v2->val.num)->getOpenName());
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_INT:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_UINT1R)
            {
                if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isRoom() && is_in(v2->val.num, 0, MAX_EXIT) &&
                    ROOM_EXIT((unit_data *)v1->val.ptr, v2->val.num))
                {
                    v->atyp = DILA_NONE;
                    v->ref = ROOM_EXIT((unit_data *)v1->val.ptr, v2->val.num)->getDoorFlagsPtr();
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_INT:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_UINT1R)
            {
                if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isRoom() && is_in(v2->val.num, 0, MAX_EXIT) &&
                    ROOM_EXIT((unit_data *)v1->val.ptr, v2->val.num))
                {
                    v->atyp = DILA_NONE;
                    v->ref = ROOM_EXIT((unit_data *)v1->val.ptr, v2->val.num)->getSkillDifficultyPtr();
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
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
                auto *room = reinterpret_cast<room_data *>(v1->val.ptr);

                if (room && room->isRoom() && is_in(v2->val.num, 0, MAX_EXIT))
                {
                    if (ROOM_EXIT(room, v2->val.num))
                    {
                        v->atyp = DILA_NORM;
                        v->val.ptr = ROOM_EXIT(room, v2->val.num)->getToRoom();
                    }
                    else // There is no exit, the result is the null pointer
                    {
                        v->type = DILV_NULL;
                        v->val.ptr = nullptr;
                    }
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                {
                    auto *room = reinterpret_cast<room_data *>(v1->val.ptr);
                    if (room && room->isRoom())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = room->getRoomFlagsPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *room = reinterpret_cast<room_data *>(v1->val.ptr);
                    v->atyp = DILA_NORM;
                    v->type = DILV_INT;
                    if (room && room->isRoom())
                    {
                        v->val.num = room->getMapXCoordinate();
                    }
                    else
                    {
                        v->val.num = -1;
                    }
                }
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
                {
                    v->atyp = DILA_NORM;
                    v->type = DILV_INT;
                    auto *room = reinterpret_cast<room_data *>(v1->val.ptr);
                    if (room && room->isRoom())
                    {
                        v->val.num = room->getMapYCoordinate();
                    }
                    else
                    {
                        v->val.num = -1;
                    }
                }
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
                {
                    auto *room = reinterpret_cast<room_data *>(v1->val.ptr);
                    if (room && room->isRoom())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = room->getLandscapeTerrainPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref = character->getOffensiveBonusPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref = character->getDefensiveBonusPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT1R;
                        v->ref = character->getSexPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT2R;
                        v->ref = character->getRacePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                if (character && character->isChar() && is_in(v2->val.num, 0, ABIL_TREE_MAX - 1))
                {
                    if (character->isPC())
                    {
                        if (p->frame[0].tmpl->zone->getAccessLevel() == 0)
                        {
                            v->atyp = DILA_NONE;
                            v->type = DILV_SINT2R;
                            v->ref = character->getAbilityAtIndexPtr(v2->val.num);
                        }
                        else
                        {
                            v->atyp = DILA_NONE;
                            v->type = DILV_INT;
                            v->val.num = character->getAbilityAtIndex(v2->val.num);
                        }
                    }
                    else
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT2R;
                        v->ref = character->getAbilityAtIndexPtr(v2->val.num);
                    }
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                if (pc && pc->isPC() && is_in(v2->val.num, 0, ABIL_TREE_MAX - 1))
                {
                    if (p->frame[0].tmpl->zone->getAccessLevel() == 0)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = pc->getAbilityLevelAtIndexPtr(v2->val.num);
                    }
                    else
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = pc->getAbilityLevelAtIndex(v2->val.num);
                    }
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isPC() && is_in(v2->val.num, 0, ABIL_TREE_MAX - 1))
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = get_racial_ability(CHAR_RACE((unit_data *)v1->val.ptr), v2->val.num);

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
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                if (pc && pc->isPC() && is_in(v2->val.num, 0, SPL_TREE_MAX - 1))
                {
                    if (p->frame[0].tmpl->zone->getAccessLevel() == 0)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;

                        v->ref = pc->getSpellLevelAtIndexPtr(v2->val.num);
                    }
                    else
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = pc->getSpellLevelAtIndex(v2->val.num);
                    }
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isPC() && is_in(v2->val.num, 0, SPL_TREE_MAX - 1))
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = get_racial_spells(CHAR_RACE((unit_data *)v1->val.ptr), v2->val.num);

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
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                if (pc && pc->isPC() && is_in(v2->val.num, 0, SKI_TREE_MAX - 1))
                {
                    if (p->frame[0].tmpl->zone->getAccessLevel() == 0)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = pc->getSkillLevelAtIndexPtr(v2->val.num);
                    }
                    else
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = pc->getSkillLevelAtIndex(v2->val.num);
                    }
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isPC() && is_in(v2->val.num, 0, SKI_TREE_MAX - 1))
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = get_racial_skill(CHAR_RACE((unit_data *)v1->val.ptr), v2->val.num);
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
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                if (pc && pc->isPC() && is_in(v2->val.num, 0, WPN_TREE_MAX - 1))
                {
                    if (p->frame[0].tmpl->zone->getAccessLevel() == 0)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = pc->getWeaponSkillLevelAtIndexPtr(v2->val.num);
                    }
                    else
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = pc->getWeaponSkillLevelAtIndex(v2->val.num);
                    }
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_UP:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isPC() && is_in(v2->val.num, 0, WPN_TREE_MAX - 1))
                {
                    v->atyp = DILA_NONE;
                    v->type = DILV_INT;
                    v->val.num = get_racial_weapon(CHAR_RACE((unit_data *)v1->val.ptr), v2->val.num);
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
                {
                    v->type = DILV_FAIL;
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        if (p->frame[0].tmpl->zone->getAccessLevel() == 0)
                        {
                            v->atyp = DILA_NONE;
                            v->type = DILV_SINT4R;
                            v->ref = character->getPlayerExperiencePtr();
                        }
                        else
                        {
                            v->atyp = DILA_NONE;
                            v->type = DILV_INT;
                            v->val.num = character->getPlayerExperience();
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                    if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = required_xp(PC_VIRTUAL_LEVEL((unit_data *)v1->val.ptr) + 1) - CHAR_EXP((unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        if (character->isPC() && (p->frame[0].tmpl->zone->getAccessLevel() != 0))
                        {
                            v->type = DILV_INT;
                            v->val.num = CHAR_LEVEL(character);
                        }
                        else
                        {
                            v->type = DILV_UINT1R;
                            v->ref = character->getLevelPtr();
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
                break;

                case DILV_CP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = (((command_info *)v1->val.ptr)->minimum_level);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        if (p->frame[0].tmpl->zone->getAccessLevel() != 0)
                        {
                            v->atyp = DILA_NONE;
                            v->type = DILV_INT;
                            v->val.num = pc->getProfession();
                        }
                        else
                        {
                            v->type = DILV_UINT1R;
                            v->ref = pc->getProfessionPtr();
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NONE;
                        if (p->frame[0].tmpl->zone->getAccessLevel() != 0)
                        {
                            v->type = DILV_INT;
                            v->val.num = pc->getVirtualPlayerLevel();
                        }
                        else
                        {
                            v->type = DILV_UINT2R;
                            v->ref = pc->getVirtualPlayerLevelPtr();
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT2R;
                        v->ref = unit->getSizePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT1R;
                        v->ref = character->getPositionPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
                break;
                case DILV_CP:
                    if (v1->val.ptr)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = &(((command_info *)v1->val.ptr)->minimum_position);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = character->getAttackTypePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref = character->getManaPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref = character->getEndurancePtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = character->getNaturalArmorPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT4R;
                        v->ref = character->getCharacterFlagsPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                    if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UP;
                        v->val.ptr = CHAR_FIGHTING((unit_data *)v1->val.ptr);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
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
                    switch (((unit_data *)v1->val.ptr)->getUnitType())
                    {
                        case UNIT_ST_NPC:
                        {
                            auto *npc = reinterpret_cast<npc_data *>(v1->val.ptr);
                            if (is_in(v2->val.num, 0, WPN_GROUP_MAX - 1))
                            {
                                v->atyp = DILA_NONE;
                                v->type = DILV_SINT2R;
                                v->ref = npc->getWeaponSkillAtIndexPtr(v2->val.num);
                            }
                            else
                            {
                                v->type = DILV_FAIL;
                            }
                        }
                        break;
                        case UNIT_ST_PC:
                            if (is_in(v2->val.num, 0, WPN_TREE_MAX - 1))
                            {
                                auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                                if (p->frame[0].tmpl->zone->getAccessLevel() == 0)
                                {
                                    v->atyp = DILA_NONE;
                                    v->type = DILV_SINT2R;
                                    v->ref = pc->getWeaponSkillAtIndexPtr(v2->val.num);
                                }
                                else
                                {
                                    v->atyp = DILA_NONE;
                                    v->type = DILV_INT;
                                    v->val.num = pc->getWeaponSkillAtIndex(v2->val.num);
                                }
                            }
                            else
                            {
                                v->type = DILV_FAIL;
                            }
                            break;

                        default:
                            v->type = DILV_ERR; /* illegal type */
                            break;
                    }
                }
                else
                {
                    v->type = DILV_FAIL; /* illegal type */
                }
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
                {
                    auto *npc = reinterpret_cast<npc_data *>(v1->val.ptr);
                    if (npc && npc->isNPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = npc->getDefaultPositionPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *npc = reinterpret_cast<npc_data *>(v1->val.ptr);
                    if (npc && npc->isNPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = npc->getAllNPCFlagsPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT2R;
                        v->ref = pc->getAllPCFlagsPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *player = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (player && player->isPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT4R;
                        v->ref = player->getPCTimeInformation().getPlayerBirthdayPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *player = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (player && player->isPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = player->getPCTimeInformation().getTotalTimePlayedInSeconds();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT1R;
                        v->ref = pc->getConditionAtIndexPtr(FULL);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT1R;
                        v->ref = pc->getConditionAtIndexPtr(THIRST);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT1R;
                        v->ref = pc->getConditionAtIndexPtr(DRUNK);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT4R;
                        v->ref = pc->getSkillPointsPtr();
                        /*	    v->val.num = &PC_SKILL_POINTS((class unit_data *) v1->val.ptr);*/
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT4R;
                        v->ref = pc->getAbilityPointsPtr();
                        /*	    v->val.num = &PC_ABILITY_POINTS((class unit_data *) v1->val.ptr);*/
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_SPR;
                        v->ref = pc->getGuildPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);

                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_SPR;
                        v->ref = pc->getPromptStringPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_SPR;
                        v->ref = unit->getKeyPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                        v->ref = ((zone_type *)v1->val.ptr)->getFilenamePtrPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
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
                        v->ref = ((zone_type *)v1->val.ptr)->getNotesPtrPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
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
                        v->ref = ((zone_type *)v1->val.ptr)->getHelpPtrPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
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
                {
                    auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                    if (pc && pc->isPC())
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT2R;
                        v->ref = pc->getNumberOfCrimesCommittedPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit && unit->isPC())
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_EDPR;
                        v->ref = &(PC_QUEST(unit).m_pList);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit && unit->isPC())
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_EDPR;
                        v->ref = &(PC_INFO(unit).m_pList);
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                    {
                        v->type = DILV_FAIL; /* not applicable */
                    }
                    break;
                case DILV_INT:
                    break;
                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }

            if (v->type == DILV_INT)
            {
                auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
                if (pc && pc->isPC() && is_in(v2->val.num, 0, SKI_TREE_MAX - 1))
                {
                    if (p->frame[0].tmpl->zone->getAccessLevel() == 0)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_SINT2R;
                        v->ref = pc->getSkillAtIndexPtr(v2->val.num);
                    }
                    else
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_INT;
                        v->val.num = pc->getSkillAtIndex(v2->val.num);
                    }
                }
                else
                {
                    v->type = DILV_FAIL;
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        v->atyp = DILA_NORM;
                        v->type = DILV_UPR;
                        v->ref = character->getMasterPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
                    if (character && character->isChar())
                    {
                        if (CHAR_FOLLOWERS(character))
                        {
                            v->atyp = DILA_NORM;
                            v->type = DILV_UPR;
                            v->ref = character->getFollowers()->getFollowerPtr();
                        }
                        else
                        {
                            v->atyp = DILA_NORM;
                            v->type = DILV_NULL;
                            v->val.ptr = nullptr;
                        }
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
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
                {
                    auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
                    if (unit)
                    {
                        v->atyp = DILA_NONE;
                        v->type = DILV_UINT1R;
                        v->ref = unit->getLevelOfWizardInvisibilityPtr();
                    }
                    else
                    {
                        v->type = DILV_FAIL;
                    }
                }
                break;

                default:
                    v->type = DILV_ERR; /* wrong type */
                    break;
            }
            break;

        default:
            szonelog(p->sarg->owner->getFileIndex()->getZone(),
                     "DIL %s, unknown field type: %d\n",
                     p->sarg->owner->getFileIndexSymName(),
                     fldno);
            p->waitcmd = WAITCMD_QUIT;
            v->type = DILV_ERR;
            break;
    }
    p->stack.push(v);
    delete v1;
    if (v2)
    {
        delete v2;
    }
}
