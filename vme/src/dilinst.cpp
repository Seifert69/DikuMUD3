/*
 $Author: All $
 $RCSfile: dilinst.cpp,v $
 $Date: 2004/05/16 04:34:34 $
 $Revision: 2.21 $
 */

#ifdef _WINDOWS
    #include <direct.h>
#endif
/* Per https://sourceforge.net/p/predef/wiki/OperatingSystems/, this identifies
 *  Mac OS X. This is needed since OS X doesn't have crypt.h and instead uses
 *  unistd.h for these mappings. */
#if defined __APPLE__ && __MACH__
    #include <unistd.h>
#elif defined LINUX
    #include <crypt.h>
#endif

#include "account.h"
#include "act_other.h"
#include "affect.h"
#include "comm.h"
#include "db.h"
#include "dil.h"
#include "dilinst.h"
#include "dilrun.h"
#include "error.h"
#include "files.h"
#include "handler.h"
#include "hookmud.h"
#include "interpreter.h"
#include "justice.h"
#include "main_functions.h"
#include "mobact.h"
#include "nanny.h"
#include "pcsave.h"
#include "pipe.h"
#include "reception.h"
#include "spell_parser.h"
#include "spells.h"
#include "system.h"
#include "szonelog.h"
#include "textutil.h"
#include "trie.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "utils.h"
#include "vmelimits.h"
#include "zon_basis.h"

#include <fcntl.h>
#include <sys/stat.h>

#include <cstdlib>
#include <cstring>

/* report error in instruction */
void dil_stop_special(unit_data *unt, dilprg *aprg)
{
    unit_fptr *fptr = nullptr;
    unit_data *u = nullptr;
    for (u = unt; u; u = u->getNext())
    {
        DeactivateDil(u, aprg);
        for (fptr = u->getFunctionPointer(); fptr; fptr = fptr->getNext())
        {
            if (aprg && (aprg == fptr->getData()))
            {
                continue;
            }
            else
            {
                stop_special(u, fptr);
            }
        }

        if (u->getUnitContains())
        {
            dil_stop_special(u->getUnitContains(), aprg);
        }
    }
}

void dil_start_special(unit_data *unt, dilprg *aprg)
{
    unit_fptr *fptr = nullptr;
    unit_data *u = nullptr;
    for (u = unt; u; u = u->getNext())
    {
        ActivateDil(u);
        for (fptr = u->getFunctionPointer(); fptr; fptr = fptr->getNext())
        {
            if (aprg && (aprg == fptr->getData()))
            {
                continue;
            }
            else
            {
                start_special(u, fptr);
            }
        }

        if (u->getUnitContains())
        {
            dil_start_special(u->getUnitContains(), aprg);
        }
    }
}

void dil_insterr(dilprg *p, char *where)
{
    /* instruction called as an expression! */
    /* This is serous! mess-up in the core.. stop the program */

    szonelog(p->sarg->owner->getFileIndex()->getZone(), "DIL %s, Instruction error in %s\n", p->sarg->owner->getFileIndexSymName(), where);
    p->waitcmd = WAITCMD_QUIT;
}

/* ************************************************************************ */
/* DIL-instructions							    */
/* ************************************************************************ */
void dilfi_edit(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    if (dil_type_check("beginedit", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (!((unit_data *)v1->val.ptr)->isPC())
        {
            dil_typeerr(p, "not a pc unit");
        }
        else if (!CHAR_DESCRIPTOR((unit_data *)v1->val.ptr))
        {
            dil_typeerr(p, "PC has no descriptor in edit()");
        }
        else
        {
            CHAR_DESCRIPTOR((unit_data *)v1->val.ptr)->setPostEditFunctionPtr(dil_edit_done);
            CHAR_DESCRIPTOR((unit_data *)v1->val.ptr)->setEditing(p->owner);
            CHAR_DESCRIPTOR((unit_data *)v1->val.ptr)->setEditReference(nullptr);

            set_descriptor_fptr(CHAR_DESCRIPTOR((unit_data *)v1->val.ptr), interpreter_string_add, TRUE);
        }
    }

    delete v1;
}

void dilfi_kedit(dilprg *p)
{
    dilval *v1 = p->stack.pop();
    descriptor_data *d = nullptr;

    if (dil_type_check("killedit", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (!((unit_data *)v1->val.ptr)->isPC())
        {
            dil_typeerr(p, "not a pc unit");
        }
        else
        {
            d = CHAR_DESCRIPTOR((unit_data *)v1->val.ptr);
            if (d)
            {
                if (d->hasPostEditFunctionPtr())
                {
                    d->callPostEditFunctionPtr(d);
                }
                d->clearLocalString();
                d->setEditReference(nullptr);
                d->setPostEditFunctionPtr(nullptr);
                d->setEditing(nullptr);
                set_descriptor_fptr(CHAR_DESCRIPTOR((unit_data *)v1->val.ptr), descriptor_interpreter, TRUE);
            }
        }
    }
    delete v1;
}

void dilfi_gamestate(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("gamestate", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, TYPEFAIL_NULL, 1, DILV_INT))
    {
        if (!((unit_data *)v1->val.ptr)->isPC())
        {
            dil_typeerr(p, "gamestate: Not a pc unit");
        }
        else
        {
            switch (v2->val.num)
            {
                case GS_PLAY:
                    UPC((unit_data *)v1->val.ptr)->gstate_togame(p);
                    break;
                case GS_QUIT:
                    extract_unit((unit_data *)v1->val.ptr);
                    break;
                case GS_MENU:
                    UPC((unit_data *)v1->val.ptr)->gstate_tomenu(p);
                    break;
                case GS_LINK_DEAD:
                    if (CHAR_DESCRIPTOR((unit_data *)v1->val.ptr))
                    {
                        descriptor_close(CHAR_DESCRIPTOR((unit_data *)v1->val.ptr));
                    }
                    break;
            }
        }
    }
    delete v1;
    delete v2;
}

void dilfi_send_done(dilprg *p)
{
    command_info *cmd_ptr = nullptr;
    dilval *v7 = p->stack.pop();
    dilval *v6 = p->stack.pop();
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("send_done",
                       p,
                       7,
                       v1,
                       TYPEFAIL_NULL,
                       1,
                       DILV_SP,
                       v2,
                       TYPEFAIL_NULL,
                       2,
                       DILV_UP,
                       DILV_NULL,
                       v3,
                       TYPEFAIL_NULL,
                       2,
                       DILV_UP,
                       DILV_NULL,
                       v4,
                       TYPEFAIL_NULL,
                       2,
                       DILV_UP,
                       DILV_NULL,
                       v5,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v6,
                       TYPEFAIL_NULL,
                       1,
                       DILV_SP,
                       v7,
                       TYPEFAIL_NULL,
                       2,
                       DILV_UP,
                       DILV_NULL))
    {
        if ((cmd_ptr = (command_info *)search_trie((char *)v1->val.ptr, g_intr_trie)))
        {
            send_done((unit_data *)v2->val.ptr,
                      (unit_data *)v3->val.ptr,
                      (unit_data *)v4->val.ptr,
                      v5->val.num,
                      cmd_ptr,
                      (const char *)v6->val.ptr,
                      (unit_data *)v7->val.ptr);
        }
        else
        {
            szonelog(p->frame->tmpl->zone,
                     "send_done: DIL '%s' attempted to send a non-existent command '%s'.",
                     p->frame->tmpl->prgname,
                     (char *)v1->val.ptr);
        }
    }
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
    delete v7;
}

/* pagestring */
void dilfi_pgstr(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("pagestring", p, 2, v1, TYPEFAIL_NULL, 1, DILV_SP, v2, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (!((unit_data *)v2->val.ptr)->isPC() && !((unit_data *)v2->val.ptr)->isNPC())
        {
            dil_typeerr(p, "not a pc/npc unit");
        }
        else if (v1->val.ptr && v2->val.ptr)
        {
            page_string(CHAR_DESCRIPTOR((unit_data *)v2->val.ptr), (char *)v1->val.ptr);
        }
    }
    delete v2;
    delete v1;
}

void dilfi_setpwd(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("setpassword", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, TYPEFAIL_NULL, 1, DILV_SP))
    {
        if (!((unit_data *)v1->val.ptr)->isPC())
        {
            dil_typeerr(p, "Not a pc unit");
        }
        else if (p->frame[0].tmpl->zone->getAccessLevel() > 0)
        {
            szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to violate system access security (setpassword).", p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else if (v1->val.ptr && v2->val.ptr)
        {
            auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
            pc->setPassword(crypt(reinterpret_cast<char *>(v2->val.ptr), PC_FILENAME(pc)));
        }
    }
    delete v1;
    delete v2;
}

void dilfi_delpc(dilprg *p)
{
    dilval *v1 = p->stack.pop();
    descriptor_data *d = nullptr;
    unit_data *tmp = nullptr;

    if (dil_type_check("delete_player", p, 1, v1, TYPEFAIL_NULL, 1, DILV_SP))
    {
        if (p->frame[0].tmpl->zone->getAccessLevel() > 0)
        {
            szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to violate system access security (delete_player).", p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {
            if (v1->val.ptr)
            {
                if ((d = find_descriptor(((char *)v1->val.ptr), nullptr)))
                {
                    extract_unit(d->getCharacter());
                    descriptor_close(d);
                }
                else
                {
                    for (tmp = g_unit_list; tmp; tmp = tmp->getGlobalNext())
                    {
                        if (tmp->isPC() && !str_ccmp(tmp->getNames().Name(), ((char *)v1->val.ptr)))
                        {
                            extract_unit(tmp);
                        }
                    }
                }

                delete_player((char *)v1->val.ptr);
            }
        }
    }
    delete v1;
}

void dilfi_reboot(dilprg *p)
{
    if (p->frame[0].tmpl->zone->getAccessLevel() > 0)
    {
        szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to violate system access security (reboot).", p->frame->tmpl->prgname);
        p->waitcmd = WAITCMD_QUIT;
        return;
    }

    g_mud_shutdown = g_mud_reboot = 1;
}

/* foreach - clear / build */
void dilfi_foe(dilprg *p)
{
    dilval *v1 = p->stack.pop();
    int i = 0;

    if (dil_type_check("foreach - build", p, 1, v1, TYPEFAIL_NULL, 1, DILV_INT))
    {
        /* build environment into secures with NULL labels */

        dil_getval(v1);

        if (v1->val.num)
        {
            for (i = 0; i < p->fp->securecount; i++)
            {
                if (p->fp->secure[i].lab == nullptr)
                {
                    dil_sub_secure(p->fp, p->fp->secure[i].sup, TRUE);
                    i--; // Shit
                }
            }

            if (p->sarg->owner->getUnitIn())
            {
                scan4_unit(p->sarg->owner, v1->val.num);
            }
            else
            {
                scan4_unit_room(p->sarg->owner, v1->val.num);
            }

            for (i = 0; i < g_unit_vector.top; i++)
            {
                dil_add_secure(p, UVI(i), nullptr);
            }

            // This statement is incorrect in Yamato when a room uses foreach() this
            // will cause the room to get added as one of the items to be looped
            // when it asked only for PCs
            // dil_add_secure(p, p->sarg->owner, NULL);

            if (IS_SET(p->sarg->owner->getUnitType(), v1->val.num))
            {
                dil_add_secure(p, p->sarg->owner, nullptr);
            }
        }
    }
    delete v1;
}

/* foreach - next */
void dilfi_fon(dilprg *p)
{
    dilval *v1 = p->stack.pop();
    unit_data *u = nullptr;
    ubit32 adr = 0;
    int i = 0;

    adr = bread_ubit32(&(p->fp->pc));
    p->waitcmd--;

    if (v1->type != DILV_UPR)
    {
        dil_typeerr(p, "foreach - next");
    }
    else
    {
        dil_test_secure(p, TRUE);
        /* look for NULL references, remove first */
        u = nullptr;
        for (i = 0; i < p->fp->securecount; i++)
        {
            if (!p->fp->secure[i].lab)
            {
                u = p->fp->secure[i].sup;
                if (u->is_destructed())
                {
                    dil_sub_secure(p->fp, u, TRUE);
                    i--;
                    u = nullptr;
                    continue;
                }
                break;
            }
        }

        if (!u)
        {
            /* no new in environment found, exit loop */
            p->fp->pc = &(p->fp->tmpl->core[adr]);
        }
        else
        {
            /* assign variable the new value */
            dil_sub_secure(p->fp, u, TRUE);
            *((unit_data **)v1->ref) = u;
        }
    }
    delete v1;
}

/* DIL store() */
void dilfi_stora(dilprg *p)
{
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char filename[512];

    if (dil_type_check("storeall", p, 3, v1, FAIL_NULL, 1, DILV_UP, v2, FAIL_NULL, 1, DILV_SP, v3, FAIL_NULL, 1, DILV_INT))
    {
        if (p->frame[0].tmpl->zone->getAccessLevel() > 10)
        {
            szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to violate system access security (storeall).", p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {
            if ((v1->val.ptr) && (v2->val.ptr))
            {
                if (!store_name_test((char *)v2->val.ptr))
                {
                    szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to store illegal file name (storeall).", p->frame->tmpl->prgname);
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
                    strcat(filename, "/units/");
                    if (!file_exists(filename))
                    {
#ifdef _WINDOWS
                        _mkdir(filename);
#else
                        mkdir(filename, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
#endif
                    }
                    strcat(filename, (char *)v2->val.ptr);

                    if (v3->val.num >= 1)
                    {
                        if (((unit_data *)v1->val.ptr)->isRoom() || ((unit_data *)v1->val.ptr)->isPC())
                        {
                            szonelog(p->frame->tmpl->zone,
                                     "DIL '%s' attempt to save a container that is either a room or pc",
                                     p->frame->tmpl->prgname);
                        }
                        else
                        {
                            store_all_unit((unit_data *)v1->val.ptr, filename, TRUE);
                        }
                    }
                    else
                    {
                        store_all_unit((unit_data *)v1->val.ptr, filename, FALSE);
                    }
                }
            }
        }
    }
    delete v1;
    delete v2;
    delete v3;
}

/* set bright */
void dilfi_sbt(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int dif = 0;

    if (dil_type_check("setbright", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, FAIL_NULL, 1, DILV_INT))
    {
        if (v1->val.ptr)
        {
            dif = v2->val.num - ((unit_data *)v1->val.ptr)->getLightOutput();

            modify_bright((unit_data *)v1->val.ptr, dif);
        }
    }

    delete v1;
    delete v2;
}

/* acc_modify */
void dilfi_amod(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("acc_modify", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, FAIL_NULL, 1, DILV_INT))
    {
        if (g_cServerConfig.isAccounting() && v1->val.ptr && ((unit_data *)v1->val.ptr)->isPC())
        {
            if (p->frame[0].tmpl->zone->getAccessLevel() != 0)
            {
                szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to violate system access security (amod).", p->frame->tmpl->prgname);
                p->waitcmd = WAITCMD_QUIT;
            }
            else
            {
                if (v2->val.num > 0)
                {
                    account_insert(p->owner, (unit_data *)v1->val.ptr, v2->val.num);
                }
                else if (v2->val.num < 0)
                {
                    account_withdraw(p->owner, (unit_data *)v1->val.ptr, -v2->val.num);
                }
            }
        }
    }
    delete v1;
    delete v2;
}

// set_weight_base
// Set unitptr param 1 base weight to param 2 int value
// Sets a unit's base weight and adjustes the weight of the unit and everything it is in.
//
void dilfi_dispatch(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    if (dil_type_check("set_weight_base", p, 1, v1, TYPEFAIL_NULL, 1, DILV_SP))
    {
        if (v1->val.ptr)
        {
            pipeMUD_write((const char *)v1->val.ptr);
        }
    }
    delete v1;
}

// set_weight_base
// Set unitptr param 1 base weight to param 2 int value
// Sets a unit's base weight and adjustes the weight of the unit and everything it is in.
//
void dilfi_set_weight_base(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("set_weight_base", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, FAIL_NULL, 1, DILV_INT))
    {
        auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
        if (unit)
        {
            int dif = v2->val.num - unit->getBaseWeight();

            /* set new baseweight */
            unit->setBaseWeight(v2->val.num);

            /* update weight */
            weight_change_unit(unit, dif);
        }
    }
    delete v1;
    delete v2;
}

// set_weight
// Set unitptr param 1 weight to param 2 int value
// Sets a unit's weight and the weight of everything it is in.
//
void dilfi_set_weight(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("set_weight", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, FAIL_NULL, 1, DILV_INT))
    {
        auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
        if (unit)
        {
            if (v2->val.num < unit->getBaseWeight())
            {
                szonelog(p->frame->tmpl->zone,
                         "DIL '%s' setting unit %s weight to %d less than base weight of %d.",
                         p->frame->tmpl->prgname,
                         unit->getFileIndexName(),
                         v2->val.num,
                         unit->getBaseWeight());
            }

            int dif = v2->val.num - unit->getWeight();

            /* update weight */
            weight_change_unit(unit, dif);
        }
    }
    delete v1;
    delete v2;
}

/* set weight */
void dilfi_swt(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("setweight", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, FAIL_NULL, 1, DILV_INT))
    {
        auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
        if (unit)
        {
            int dif = v2->val.num - unit->getBaseWeight();

            /* set new baseweight */
            unit->setBaseWeight(v2->val.num);

            /* update weight */
            weight_change_unit(unit, dif);
        }
    }
    delete v1;
    delete v2;
}

/* change_speed */
void dilfi_chas(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("change_speed", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, FAIL_NULL, 1, DILV_INT))
    {
        auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
        if (character && character->isChar() && CHAR_COMBAT(character))
        {
            CHAR_COMBAT(character)->changeSpeed(v2->val.num, character->getSpeedPercentage());
        }
    }
    delete v1;
    delete v2;
}

/* reset_level */
void dilfi_rslv(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    if (dil_type_check("reset_level", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (p->frame[0].tmpl->zone->getAccessLevel() > 1)
        {
            szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to reset level w/o access.", p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {
            auto *character = reinterpret_cast<char_data *>(v1->val.ptr);
            if (character->isPC())
            {
                if (!IS_IMMORTAL(character))
                {
                    character->setLevel(1);
                }
            }
        }
    }
    delete v1;
}

/* reset_vlevel */
void dilfi_rsvlv(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    if (dil_type_check("reset_vlevel", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (p->frame[0].tmpl->zone->getAccessLevel() > 1)
        {
            szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to reset vlevel w/o access.", p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {
            auto *pc = reinterpret_cast<pc_data *>(v1->val.ptr);
            if (pc->isPC())
            {
                if (!IS_IMMORTAL(pc))
                {
                    pc->setVirtualPlayerLevel(1);
                }
            }
        }
    }
    delete v1;
}

/* reset_race */
void dilfi_rsrce(dilprg *p)
{
    dilval *v1 = p->stack.pop();
    if (dil_type_check("reset_race", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (p->frame[0].tmpl->zone->getAccessLevel() > 1)
        {
            szonelog(p->frame->tmpl->zone, "DIL '%s' attempt to reset race w/o access.", p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {
            if (((unit_data *)v1->val.ptr)->isPC())
            {
                race_adjust((unit_data *)v1->val.ptr);
            }
        }
    }
    delete v1;
}

/* stop_fighting */
void dilfi_stopf(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("stop_fighting", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, TYPEFAIL_NULL, 2, DILV_UP, DILV_NULL))
    {
        if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar())
        {
            if (v2->val.ptr && ((unit_data *)v2->val.ptr)->isChar())
            {
                stop_fighting((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr);
            }
            else
            {
                stop_fighting((unit_data *)v1->val.ptr, nullptr);
            }
        }
    }
    delete v1;
    delete v2;
}

/* set_fighting */
void dilfi_setf(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("set_fighting", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar() && v2->val.ptr && ((unit_data *)v2->val.ptr)->isChar())
        {
            if (CHAR_FIGHTING((unit_data *)v1->val.ptr))
            {
                set_fighting((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr, FALSE);
            }
            else
            {
                set_fighting((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr, TRUE);
            }
        }
    }
    delete v1;
    delete v2;
}

/* clear interrupt */
void dilfi_cli(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    if (dil_type_check("clear", p, 1, v1, FAIL_NULL, 1, DILV_INT))
    {
        dil_intr_remove(p, v1->val.num);
    }
    delete v1;
}

/* Return from proc */
void dilfi_rtf(dilprg *p)
{
    dilframe *cfrm = nullptr;
    ubit8 typ = 0;
    int i = 0;
    p->waitcmd--;

    if (p->fp == p->frame)
    {
        /* just stop execution, never discard last frameframe! */
        p->waitcmd = WAITCMD_QUIT;
        return;
    }

    if (p->fp->tmpl->rtnt == DILV_ERR)
    {
        if (p->stack.length() != p->fp->stacklen)
        {
            slog(LOG_ALL,
                 0,
                 "DIL %s@%s on %s: Unexpected stack length after procedure call.",
                 p->fp->tmpl->prgname,
                 p->fp->tmpl->zone->getName(),
                 p->sarg->owner->getFileIndexSymName());
            p->waitcmd = WAITCMD_QUIT;
            return;
        }
    }
    else
    {
        // There is a return variable lets get it and make it a copy on the stack
        if (p->stack.length() != p->fp->stacklen + 1)
        {
            slog(LOG_ALL,
                 0,
                 "DIL %s@%s on %s: Unexpected stack length after function call.",
                 p->fp->tmpl->prgname,
                 p->fp->tmpl->zone->getName(),
                 p->sarg->owner->getFileIndexSymName());
            p->waitcmd = WAITCMD_QUIT;
            return;
        }
        dilval *v1 = p->stack.pop();
        dilval *v = new dilval;

        typ = dil_getval(v1);
        if (typ != p->fp->tmpl->rtnt)
        {
            slog(LOG_ALL, 0, "DIL: Error return types do not match.");
            p->waitcmd = WAITCMD_QUIT;
            delete v1;
            delete v;
            return;
        }
        switch (typ)
        {
            case DILV_UP:
                v->type = DILV_UP;
                v->atyp = DILA_NORM;
                v->val.ptr = (unit_data *)v1->val.ptr;
                break;

            case DILV_SP:
                v->type = DILV_SP;
                v->atyp = DILA_EXP;
                v->val.ptr = str_dup((char *)v1->val.ptr);
                break;

            case DILV_SLP:
                v->type = DILV_SLP;
                v->atyp = DILA_EXP;
                v->val.ptr = ((cNamelist *)v1->val.ptr)->Duplicate();
                break;

            case DILV_ILP:
                v->type = DILV_ILP;
                v->atyp = DILA_EXP;
                v->val.ptr = ((cintlist *)v1->val.ptr)->Duplicate();
                break;

            case DILV_EDP:
                v->type = DILV_EDP;
                v->atyp = DILA_NORM;
                v->val.ptr = (extra_descr_data *)v1->val.ptr;
                break;

            case DILV_ZP:
                v->type = DILV_ZP;
                v->atyp = DILA_NORM;
                v->val.ptr = (zone_type *)v1->val.ptr;
                break;

            case DILV_CP:
                v->type = DILV_CP;
                v->atyp = DILA_NORM;
                v->val.ptr = (command_info *)v1->val.ptr;
                break;

            case DILV_INT:
                v->type = DILV_INT;
                v->atyp = DILA_NONE;
                v->val.num = v1->val.num;
                break;

            default:
                v->type = DILV_ERR;
                dil_typeerr(p, "function call evaltuated to failed result.");
                break;
        }
        delete v1;
        p->stack.push(v);
    }

    i = p->fp - p->frame - 1;
    cfrm = p->fp;

    /* pop stack frame */
    p->fp = &p->frame[i];

    dil_free_frame(cfrm);
}

void dil_pop_frame(dilprg *p)
{
    dilfi_rtf(p); /* XXX Is this a potential memory leak rts / rtf? */
}

/*
   'p' is the program in which a new frame should be created.
   'rtmpl' is the template which the frame should be based upon.
    the arguments must be pushed to the stack before the call.
*/

void dil_push_frame(dilprg *p, diltemplate *rtmpl)
{
    int i = 0;
    dilframe *frm = nullptr;

    i = p->fp - p->frame;

    if (i + 2 > p->framesz)
    {
        p->framesz += DIL_FRAMEINC;
        RECREATE(p->frame, dilframe, p->framesz);
    }

    frm = &p->frame[i + 1];
    p->fp = frm;

    frm->tmpl = rtmpl;
    frm->pc = rtmpl->core;
    frm->securecount = 0;
    frm->secure = nullptr;

    frm->intrcount = rtmpl->intrcount;

    if (rtmpl->intrcount)
    {
        CREATE(frm->intr, dilintr, rtmpl->intrcount);
    }
    else
    {
        frm->intr = nullptr;
    }

    if (rtmpl->varc)
    {
        CREATE(frm->vars, dilvar, rtmpl->varc);

        for (i = 0; i < rtmpl->varc; i++)
        {
            frm->vars[i].type = rtmpl->vart[i];
            if (i >= rtmpl->argc)
            {
                if (frm->vars[i].type == DILV_SLP)
                {
                    frm->vars[i].val.namelist = new cNamelist;
                }
                else if (frm->vars[i].type == DILV_ILP)
                {
                    frm->vars[i].val.intlist = new cintlist;
                }
                else
                {
                    frm->vars[i].val.string = nullptr;
                }
            }
            else
            {
                frm->vars[i].val.string = nullptr;
            }
        }
    }
    else
    {
        frm->vars = nullptr;
    }

    ubit8 tmp = 0;

    for (i = 0; i < rtmpl->argc; i++)
    {
        tmp = dil_getval(p->stack[-(rtmpl->argc - i)]);

        if (tmp != frm->vars[i].type) /* Can fail, err, be null, etc */
        {
            if (tmp != DILV_NULL)
            {
                szonelog(p->sarg->owner->getFileIndex()->getZone(),
                         "DIL %s Error in %s in remote call to %s where parameter %d has incorrect type. Stopping program.",
                         p->sarg->owner->getFileIndexSymName(),
                         p->frame[0].tmpl->prgname,
                         p->fp->tmpl->prgname,
                         i + 1);
                p->waitcmd = WAITCMD_QUIT;
                break;
            }
        }

        switch (frm->vars[i].type)
        {
            case DILV_UP:
                frm->vars[i].val.unitptr = (unit_data *)p->stack[-(rtmpl->argc - i)]->val.ptr;
                break;

            case DILV_SP:
                frm->vars[i].val.string = str_dup((char *)p->stack[-(rtmpl->argc - i)]->val.ptr);
                break;

            case DILV_SLP:
                if (tmp != DILV_NULL)
                {
                    frm->vars[i].val.namelist = ((cNamelist *)p->stack[-(rtmpl->argc - i)]->val.ptr)->Duplicate();
                }
                break;

            case DILV_ILP:
                if (tmp != DILV_NULL)
                {
                    frm->vars[i].val.intlist = ((cintlist *)p->stack[-(rtmpl->argc - i)]->val.ptr)->Duplicate();
                }
                break;

            case DILV_EDP:
                frm->vars[i].val.extraptr = (extra_descr_data *)p->stack[-(rtmpl->argc - i)]->val.ptr;
                break;

            case DILV_CP:
                frm->vars[i].val.cmdptr = (command_info *)p->stack[-(rtmpl->argc - i)]->val.ptr;
                break;

            case DILV_ZP:
                frm->vars[i].val.zoneptr = (zone_type *)p->stack[-(rtmpl->argc - i)]->val.ptr;
                break;

            case DILV_INT:
                frm->vars[i].val.integer = p->stack[-(rtmpl->argc - i)]->val.num;
                break;

            default:
                error(HERE, "Impossible!");
        }
    }
    for (i = 0; i < rtmpl->argc; i++)
    {
        delete (p->stack.pop());
    }

    frm->stacklen = p->stack.length();
}

/* Remote function call */
void dilfi_rfunc(dilprg *p)
{
    int xrefi = 0;
    diltemplate *ctmpl = nullptr;
    ubit8 argcnt = 0;

    p->waitcmd--;

    /*
     * <rfcall> <funcnumber> <nargs> <arg> <arg> ...
     */

    ctmpl = p->fp->tmpl;
    xrefi = bread_ubit16(&(p->fp->pc)); /* <funcnumber> */
    argcnt = bread_ubit8(&(p->fp->pc)); /* <nargs>, ignored */
    if (!ctmpl->extprg[xrefi])
    {
        /*
         * This function had a lost reference!
         * stop the DIL program executing,
         * write error, but do not slime it.
         */
        szonelog(p->sarg->owner->getFileIndex()->getZone(),
                 "DIL %s, dil %s@%s Error in remote call function #%d not valid\n",
                 p->sarg->owner->getFileIndexSymName(),
                 p->fp->tmpl->prgname,
                 p->fp->tmpl->zone->getName(),
                 xrefi);
        p->waitcmd = WAITCMD_STOP;
        for (int i = 0; (i < argcnt); i++)
        {
            delete (p->stack.pop());
        }
        return;
    }
    /* expand frame */
    dil_push_frame(p, ctmpl->extprg[xrefi]);
}

/* Remote symbolic function call */
void dilfi_rsfunc(dilprg *p)
{
    int i = 0;
    diltemplate /* *ctmpl,*/ *ntmpl = nullptr;
    dilval *v1 = p->stack.pop();
    ubit8 argcnt = 0;
    int fail = 0;

    p->waitcmd--;

    /*
     * <rsfcall>  <funcname> <nargs> <arg> <arg> ...
     */

    //	ctmpl = p->fp->tmpl;
    argcnt = bread_ubit8(&(p->fp->pc)); /* <nargs>, ignored */
    fail = FALSE;
    if (dil_getval(v1) == DILV_SP)
    {
        ntmpl = find_dil_template((const char *)v1->val.ptr);
        if (ntmpl)
        {
            if (ntmpl->argc == argcnt)
            {
                for (i = 0; ((!fail) && i < argcnt); i++)
                {
                    int t = dil_getval(p->stack[-(argcnt - i)]);

                    switch (ntmpl->argt[i])
                    {
                        case DILV_INT:
                            if (t != DILV_INT)
                            {
                                fail = TRUE; /* type error, expect integer */
                            }
                            break;
                        case DILV_SP:
                            if ((t != DILV_NULL) && (t != DILV_SP))
                            {
                                fail = TRUE; /* type error, expect string */
                            }
                            break;
                        case DILV_UP:
                            if ((t != DILV_NULL) && (t != DILV_UP))
                            {
                                fail = TRUE; /* type error, expect unitptr */
                            }
                            break;
                        case DILV_ZP:
                            if ((t != DILV_NULL) && (t != DILV_ZP))
                            {
                                fail = TRUE; /* type error, expect unitptr */
                            }
                            break;
                        case DILV_CP:
                            if ((t != DILV_NULL) && (t != DILV_CP))
                            {
                                fail = TRUE; /* type error, expect unitptr */
                            }
                            break;
                        case DILV_EDP:
                            if ((t != DILV_NULL) && (t != DILV_EDP))
                            {
                                fail = TRUE; /* type error, expect extraptr */
                            }
                            break;
                        case DILV_SLP:
                            if ((t != DILV_NULL) && (t != DILV_SLP))
                            {
                                fail = TRUE; /* type error, expect stringlist */
                            }
                            break;
                        case DILV_ILP:
                            if ((t != DILV_NULL) && (t != DILV_SLP))
                            {
                                fail = TRUE; /* type error, expect stringlist */
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            else
            {
                fail = TRUE;
            }
        }
        else
        {
            fail = TRUE;
        }
    }
    else
    {
        fail = TRUE;
    }

    if (fail)
    {
        szonelog(p->sarg->owner->getFileIndex()->getZone(),
                 "DIL %s, dil %s Error in symbolic remote call string=%s\n",
                 p->sarg->owner->getFileIndexSymName(),
                 p->fp->tmpl->prgname,
                 (char *)v1->val.ptr);
        p->waitcmd = WAITCMD_STOP;

        delete v1;
        for (i = 0; (i < argcnt); i++)
        {
            delete (p->stack.pop());
        }
    }
    else
    {
        delete v1;
        dil_push_frame(p, ntmpl);
    }
}

/* Assignment of value to reference */
void dilfi_ass(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    switch (v1->type)
    {
        case DILV_NULL:
        case DILV_FAIL: /* just ignore */
            break;

        /* unit pointer assignment */
        case DILV_UPR:
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;
                case DILV_NULL:
                    *((void **)v1->ref) = nullptr;
                    break;

                case DILV_UP:
                    *((unit_data **)v1->ref) = (unit_data *)v2->val.ptr;
                    break;

                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "unit assignemt");
                    break;
            }
            break;
        /* unit pointer assignment */
        case DILV_ZPR:
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;
                case DILV_NULL:
                    *((void **)v1->ref) = nullptr;
                    break;

                case DILV_ZP:
                    *((zone_type **)v1->ref) = (zone_type *)v2->val.ptr;
                    break;

                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "zoneptr assignemt");
                    break;
            }
            break;

        case DILV_CPR:
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;
                case DILV_NULL:
                    *((void **)v1->ref) = nullptr;
                    break;

                case DILV_CP:
                    *((command_info **)v1->ref) = (command_info *)v2->val.ptr;
                    break;

                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "cmdptr assignemt");
                    break;
            }
            break;

        case DILV_HASHSTR:
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;

                case DILV_NULL:
                    ((std::string *)v1->ref)->clear();
                    break;

                case DILV_SP:
                    if (v2->val.ptr == nullptr)
                    {
                        ((std::string *)v1->ref)->clear();
                    }
                    else
                    {
                        (*(std::string *)v1->ref) = ((char *)v2->val.ptr);
                    }
                    break;

                case DILV_HASHSTR:
                    if (v2->ref == nullptr)
                    {
                        ((std::string *)v1->ref)->clear();
                    }
                    else
                    {
                        (*(std::string *)v1->ref) = (*(std::string *)v2->ref);
                    }
                    break;

                default:
                    dil_typeerr(p, "hash-string assignment");
                    break;
            }
            break;

        /* string assignment */
        case DILV_SPR:
            /* free potential old string */
            if (v1->atyp == DILA_NORM)
            {
                if (*((char **)v1->ref))
                    FREE(*((char **)v1->ref));
            }
            else
            {
                dil_typeerr(p, "ordinary string assignment <- hash");
            }

            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;

                case DILV_NULL:
                    *((void **)v1->ref) = nullptr;
                    break;

                case DILV_HASHSTR:
                    if (v2->ref)
                    {
                        *((char **)v1->ref) = str_dup(((std::string *)v2->ref)->c_str());
                    }
                    else
                    {
                        *((char **)v1->ref) = str_dup("");
                    }
                    break;

                case DILV_SP:
                    if (v2->val.ptr)
                    {
                        *((char **)v1->ref) = str_dup((const char *)v2->val.ptr);
                    }
                    else
                    {
                        *((char **)v1->ref) = str_dup("");
                    }
                    break;

                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "string assignment");
                    break;
            }
            break;

        case DILV_SLPR:
            /* String list assignment. The old stringlist is */
            /* discarded, and replaced with the new */
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;

                case DILV_NULL:
                    /* assign empty list */
                    assert((cNamelist *)v1->ref);
                    ((cNamelist *)v1->ref)->Free();
                    break;

                case DILV_SLP:
                    /* string list assignment */
                    assert((cNamelist *)v1->ref);

                    ((cNamelist *)v1->ref)->Free();

                    if (v2->val.ptr)
                    {
                        ((cNamelist *)v1->ref)->CopyList((cNamelist *)v2->val.ptr);
                    }
                    break;

                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "namelist SPLR assignemt");
                    break;
            }
            break;

        case DILV_ILPR:
            /* int list assignment. The old intlist is */
            /* discarded, and replaced with the new */
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;

                case DILV_NULL:
                    /* assign empty list */
                    assert((cintlist *)v1->ref);
                    ((cintlist *)v1->ref)->Free();
                    break;

                case DILV_ILP:
                    /* string list assignment */
                    assert((cintlist *)v1->ref);

                    ((cintlist *)v1->ref)->Free();

                    if (v2->val.ptr)
                    {
                        ((cintlist *)v1->ref)->CopyList((cintlist *)v2->val.ptr);
                    }
                    //         v1->ref=((cintlist *) v2->val.ptr);
                    break;

                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "namelist IPLR assignemt");
                    break;
            }
            break;

        case DILV_EDPR:
            /* assignment of extra decr pointer variable */
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;

                case DILV_NULL:
                    /* assign empty list */
                    *((void **)v1->ref) = nullptr;
                    break;

                case DILV_EDP:
                    *((extra_descr_data **)v1->ref) = (extra_descr_data *)v2->val.ptr;
                    break;

                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "exdp assignemt");
                    break;
            }
            break;

        case DILV_SINT1R:
            /* assignment of signed 8bit integer */
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;
                case DILV_INT:
                    *((sbit8 *)v1->ref) = v2->val.num;
                    break;
                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "integer assignemt");
                    break;
            }
            break;

        case DILV_SINT2R:
            /* assignment of signed 16bit integer */
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;
                case DILV_INT:
                    *((sbit16 *)v1->ref) = v2->val.num;
                    break;
                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "integer assignemt");
                    break;
            }
            break;

        case DILV_SINT4R:
            /* assignment of signed 32bit integer */
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;
                case DILV_INT:
                    *((sbit32 *)v1->ref) = v2->val.num;
                    break;
                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "integer assignemt");
                    break;
            }
            break;

        case DILV_UINT1R:
            /* assignment of unsigned 8bit integer */
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;
                case DILV_INT:
                    *((ubit8 *)v1->ref) = v2->val.num;
                    break;
                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "integer assignemt");
                    break;
            }
            break;

        case DILV_UINT2R:
            /* assignment of unsigned 16bit integer */
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;
                case DILV_INT:
                    *((ubit16 *)v1->ref) = v2->val.num;
                    break;
                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "integer assignemt");
                    break;
            }
            break;

        case DILV_UINT4R:
            /* assignment of unsigned 32bit integer */
            switch (dil_getval(v2))
            {
                case DILV_FAIL:
                    break;
                case DILV_INT:
                    *((ubit32 *)v1->ref) = v2->val.num;
                    break;
                default:
                    /* ERROR incompatible types */
                    dil_typeerr(p, "integer assignemt");
                    break;
            }
            break;

        default:
            /* not an lvalue! */
            dil_typeerr(p, "lvalue assignemt");
            break;
    }
    delete v1;
    delete v2;
}

/* Link unit into other unit */
void dilfi_lnk(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("link", p, 2, v1, FAIL_NULL, 1, DILV_UP, v2, FAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr && v2->val.ptr)
        {
            if (((unit_data *)v1->val.ptr)->isObj() && OBJ_EQP_POS((unit_data *)v1->val.ptr))
            {
                unequip_object((unit_data *)v1->val.ptr);
            }
            if (!unit_recursive((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr) &&
                (!((unit_data *)v1->val.ptr)->isRoom() || ((unit_data *)v2->val.ptr)->isRoom()))
            {
                unit_from_unit((unit_data *)v1->val.ptr);
                unit_to_unit((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr);
            }
        }
    }
    delete v1;
    delete v2;
}

/* dilcopy */
void dilfi_dlc(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("dilcopy", p, 2, v1, TYPEFAIL_NULL, 1, DILV_SP, v2, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr && v2->val.ptr)
        {
            dil_copy((char *)v1->val.ptr, (unit_data *)v2->val.ptr);
        }
    }
    delete v1;
    delete v2;
}

/* sendtext */
void dilfi_sete(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("sendtext", p, 2, v1, TYPEFAIL_NULL, 1, DILV_SP, v2, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr && v2->val.ptr && ((unit_data *)v2->val.ptr)->isChar())
        {
            if (*((char *)v1->val.ptr) == MULTI_UNIQUE_CHAR && *((char *)v1->val.ptr + 1) == MULTI_PROMPT_CHAR)
            {
                if (CHAR_DESCRIPTOR((unit_data *)v2->val.ptr))
                {
                    protocol_send_text(CHAR_DESCRIPTOR((unit_data *)v2->val.ptr)->getMultiHookPtr(),
                                       CHAR_DESCRIPTOR((unit_data *)v2->val.ptr)->getMultiHookID(),
                                       (((const char *)v1->val.ptr) + 2),
                                       MULTI_PROMPT_CHAR);
                }
            }
            else
            {
                send_to_char((char *)v1->val.ptr, (unit_data *)v2->val.ptr);
            }
        }
    }
    delete v1;
    delete v2;
}

/* Set one char to follow another unconditionally */
void dilfi_folo(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("follow", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, TYPEFAIL_NULL, 2, DILV_UP, DILV_NULL))
    {
        if (v1->val.ptr && !(v2->val.ptr))
        {
            stop_following((unit_data *)v1->val.ptr);
        }
        else
        {
            if (v1->val.ptr && v2->val.ptr && ((unit_data *)v1->val.ptr)->isChar() && ((unit_data *)v2->val.ptr)->isChar())
            {
                if (CHAR_MASTER((unit_data *)v1->val.ptr))
                {
                    stop_following((unit_data *)v1->val.ptr);
                }

                start_following((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr);
            }
        }
    }
    delete v1;
    delete v2;
}

/* logcrime */
void dilfi_lcri(dilprg *p)
{
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("logcrime", p, 3, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, TYPEFAIL_NULL, 1, DILV_UP, v3, TYPEFAIL_NULL, 1, DILV_INT))
    {
        if (v1->val.ptr && v2->val.ptr && ((unit_data *)v1->val.ptr)->isChar() && ((unit_data *)v2->val.ptr)->isChar())
        {
            log_crime((unit_data *)v1->val.ptr, (unit_data *)v2->val.ptr, v3->val.num);
        }
    }
    delete v1;
    delete v2;
    delete v3;
}

/* Assign EXP to player */
void dilfi_exp(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    sbit32 value = 0;

    p->waitcmd--;

    if (dil_type_check("exp", p, 2, v1, TYPEFAIL_NULL, 1, DILV_INT, v2, FAIL_NULL, 1, DILV_UP))
    {
        if (dil_getval(v1) == DILV_INT)
        {
            value = v1->val.num;
        }

        if (v2->val.ptr && ((unit_data *)v2->val.ptr)->isPC())
        {
            slog(LOG_ALL,
                 0,
                 "%s gained %d experience from unit %s.",
                 ((unit_data *)v2->val.ptr)->getNames().Name(),
                 value,
                 p->sarg->owner->getFileIndexSymName());
            gain_exp((unit_data *)v2->val.ptr, value);
        }
    }
    delete v1;
    delete v2;
}

/* Branch on expression */
void dilfi_if(dilprg *p)
{
    ubit32 coreptr = 0;
    dilval *v1 = p->stack.pop();

    coreptr = bread_ubit32(&(p->fp->pc)); /* else branch */

    p->waitcmd--;
    if (!dil_getbool(v1, p))
    {                                              /* might be pointer, but ok! */
        p->fp->pc = &(p->fp->tmpl->core[coreptr]); /* choose else branch */
    }
    delete v1;
}

/* Set bits in integer */
void dilfi_set(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("set", p, 1, v2, TYPEFAIL_NULL, 1, DILV_INT))
    {
        switch (v1->type)
        {
            case DILV_SINT1R:
                *((sbit8 *)v1->ref) |= v2->val.num;
                break;
            case DILV_SINT2R:
                *((sbit16 *)v1->ref) |= v2->val.num;
                break;
            case DILV_SINT4R:
                *((sbit32 *)v1->ref) |= v2->val.num;
                break;
            case DILV_UINT1R:
                *((ubit8 *)v1->ref) |= v2->val.num;
                break;
            case DILV_UINT2R:
                *((ubit16 *)v1->ref) |= v2->val.num;
                break;
            case DILV_UINT4R:
                *((ubit32 *)v1->ref) |= v2->val.num;
                break;
            case DILV_FAIL:
                break;
            default:
                /* not an lvalue! */
                dil_typeerr(p, "lvalue set");
                break;
        }
    }
    delete v1;
    delete v2;
}

/* Unset bits in integer */
void dilfi_uset(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("unset", p, 1, v2, TYPEFAIL_NULL, 1, DILV_INT))
    {
        switch (v1->type)
        {
            case DILV_FAIL:
                break;
            case DILV_SINT1R:
                REMOVE_BIT(*((sbit8 *)v1->ref), v2->val.num);
                break;
            case DILV_SINT2R:
                REMOVE_BIT(*((sbit16 *)v1->ref), v2->val.num);
                break;
            case DILV_SINT4R:
                REMOVE_BIT(*((sbit32 *)v1->ref), v2->val.num);
                break;
            case DILV_UINT1R:
                REMOVE_BIT(*((ubit8 *)v1->ref), v2->val.num);
                break;
            case DILV_UINT2R:
                REMOVE_BIT(*((ubit16 *)v1->ref), v2->val.num);
                break;
            case DILV_UINT4R:
                REMOVE_BIT(*((ubit32 *)v1->ref), v2->val.num);
                break;

            default:
                /* not an lvalue! */
                dil_typeerr(p, "lvalue unset");
                break;
        }
    }
    delete v1;
    delete v2;
}

/* Add element to string list (addstring) */
void dilfi_adl(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    switch (v1->type)
    {
        case DILV_SLPR:
            if (dil_type_check("addstring", p, 1, v2, FAIL_NULL, 1, DILV_SP))
            {
                if (v2->val.ptr)
                {
                    if (isblank(*(char *)v2->val.ptr))
                        slog(LOG_ALL, 0, "DIL addstring [%s] has whitespace", (char *)v2->val.ptr);

                    ((cNamelist *)v1->ref)->AppendNameTrim((char *)v2->val.ptr);
                }
            }
            break;

        case DILV_FAIL:
            break;
        default:
            /* ERROR not right lvalue */
            dil_typeerr(p, "lvalue addstring");
            break;
    }
    delete v1;
    delete v2;
}

void dilfi_inslst(dilprg *p)
{
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    switch (v1->type)
    {
        default:
            /* ERROR not right lvalue */
            dil_typeerr(p, "lvalue addstring");
            break;
        case DILV_FAIL:
            break;
        case DILV_SLPR:
        case DILV_ILPR:
            if (dil_type_check("insert", p, 2, v2, TYPEFAIL_NULL, 1, DILV_INT, v3, TYPEFAIL_NULL, 2, DILV_SP, DILV_INT))
            {
                if (v1->type == DILV_SLPR)
                {
                    if (dil_getval(v3) == DILV_INT)
                    {
                        ((cNamelist *)v1->ref)->InsertName(itoa(v3->val.num), (ubit32)v2->val.num);
                    }
                    else if (v3->val.ptr)
                    {
                        ((cNamelist *)v1->ref)->InsertName(((char *)v3->val.ptr), (ubit32)v2->val.num);
                    }
                }
                else if (v1->type == DILV_ILPR)
                {
                    if (dil_getval(v3) == DILV_INT)
                    {
                        ((cintlist *)v1->ref)->Insert(v3->val.num, (sbit32)v2->val.num);
                    }
                    else
                    {
                        if (v3->val.ptr)
                        {
                            ((cintlist *)v1->ref)->Insert(atoi(skip_spaces((char *)v3->val.ptr)), (sbit32)v2->val.num);
                        }
                    }
                }
            }
            break;
    }

    delete v1;
    delete v2;
    delete v3;
}

/* Substract element from a list */
void dilfi_remlst(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    switch (v1->type)
    {
        case DILV_FAIL:
            break;

        case DILV_SLPR:
        case DILV_ILPR:
            if (dil_type_check("remove", p, 1, v2, TYPEFAIL_NULL, 1, DILV_INT))
            {
                if (v1->type == DILV_SLPR)
                {
                    ((cNamelist *)v1->ref)->Remove(v2->val.num);
                }
                else
                {
                    if (v1->type == DILV_ILPR)
                    {
                        ((cintlist *)v1->ref)->Remove(v2->val.num);
                    }
                }
            }
            break;

        default:
            /* ERROR not right lvalue */
            dil_typeerr(p, "lvalue substring");
            break;
    }
    delete v1;
    delete v2;
}

/* Substract element from stringlist */
void dilfi_sul(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    switch (v1->type)
    {
        case DILV_FAIL:
            break;

        default:
            /* ERROR not right lvalue */
            dil_typeerr(p, "lvalue substring");
            break;

        case DILV_SLPR:
            if (dil_type_check("substring", p, 1, v2, TYPEFAIL_NULL, 1, DILV_SP))
            {
                if (v2->val.ptr)
                {
                    ((cNamelist *)v1->ref)->RemoveName((char *)v2->val.ptr);
                }
            }
            break;
    }
    delete v1;
    delete v2;
}

/* add element to extra description */
void dilfi_ade2(dilprg *p)
{
    /* add entry to extradescription */
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    /* three arguments */

    p->waitcmd--;

    switch (v1->type)
    {
        case DILV_FAIL:
        case DILV_EDP:
            break;

        default:
            /* ERROR not right lvalue */
            dil_typeerr(p, "lvalue addextra");
            break;

        case DILV_EDPR:
            if (dil_type_check("addextra",
                               p,
                               3,
                               v2,
                               TYPEFAIL_NULL,
                               2,
                               DILV_SLP,
                               DILV_NULL,
                               v4,
                               TYPEFAIL_NULL,
                               1,
                               DILV_ILP,
                               v3,
                               TYPEFAIL_NULL,
                               1,
                               DILV_SP))
            {
                if (v1->ref && v3->val.ptr && v4->val.ptr && (v2->val.ptr || v2->type == DILV_NULL))
                {
                    // MS2020 XXX BUG HERE
                    extra_descr_data *e = new extra_descr_data((char *)nullptr, (char *)v3->val.ptr);
                    rogue_push_front(((extra_descr_data **)v1->ref), e);
                    /**((extra_descr_data **)v1->ref) =
                        (*((class extra_descr_data **)v1->ref))->add((char *)NULL, (char *)v3->val.ptr);*/
                    if (v2->type == DILV_NULL)
                    {
                        (*((extra_descr_data **)v1->ref))->names = new cNamelist;
                    }
                    else
                    {
                        (*((extra_descr_data **)v1->ref))->names.CopyList((cNamelist *)v2->val.ptr);
                    }
                    (*((extra_descr_data **)v1->ref))->vals.CopyList((cintlist *)v4->val.ptr);
                }
            }

            break;
    }
    delete v1;
    delete v2;
    delete v3;
    delete v4;
}

/* add element to extra description */
void dilfi_ade(dilprg *p)
{
    /* add entry to extra description */
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    switch (v1->type)
    {
        case DILV_FAIL:
        case DILV_EDP:
            break;

        default:
            /* ERROR not right lvalue */
            dil_typeerr(p, "lvalue addextra");
            break;

        case DILV_EDPR:
            if (dil_type_check("addextra", p, 2, v2, TYPEFAIL_NULL, 2, DILV_SLP, DILV_NULL, v3, TYPEFAIL_NULL, 1, DILV_SP))
            {
                if (v1->ref && v3->val.ptr && (v2->val.ptr || v2->type == DILV_NULL))
                {
                    extra_descr_data *e = new extra_descr_data((char *)nullptr, (char *)v3->val.ptr);
                    rogue_push_front(((extra_descr_data **)v1->ref), e);

                    /* *((class extra_descr_data **)v1->ref) =
                        (*((class extra_descr_data **)v1->ref))->add((char *)NULL, (char *)v3->val.ptr); */
                    if (v2->type == DILV_NULL)
                    {
                        (*((extra_descr_data **)v1->ref))->names = new cNamelist;
                    }
                    else
                    {
                        (*((extra_descr_data **)v1->ref))->names.CopyList((cNamelist *)v2->val.ptr);
                    }
                }
            }

            break;
    }
    delete v1;
    delete v2;
    delete v3;
}

/* Substract elemnt from extra description */
void dilfi_sue(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    switch (v1->type)
    {
        case DILV_FAIL:
        case DILV_EDP:
            break;

        default:
            /* ERROR not right lvalue */
            dil_typeerr(p, "lvalue subextra");
            break;

        case DILV_EDPR:
            if (dil_type_check("subextra", p, 1, v2, TYPEFAIL_NULL, 1, DILV_SP))
            {
                if (v2->val.ptr && v1->ref)
                {
                    // MS2020 bug, check if exd is NULL
                    if (*((extra_descr_data **)v1->ref) != nullptr)
                    {
                        /* *((class extra_descr_data **)v1->ref) =
                            (*((class extra_descr_data **)v1->ref))->remove((char *)v2->val.ptr); */

                        // Save our friend exd here, because rogue_delete is going to change the ref value
                        extra_descr_data *exd = *(extra_descr_data **)v1->ref;

                        rogue_remove((extra_descr_data **)v1->ref, (char *)v2->val.ptr);

                        // Now reuse exd here to clear out any invalid pointers
                        dil_clear_extras(p, exd);

                        // So we can get into a really messy situation here.
                        // If in a DIL program you have two extraptr, e.g. u.extra and myextra := u.extra.
                        // Then call this sub() and delete the head element from u. Next check myextra.name.
                        // It will now reference a deleted memory space. Not sure why this hasn't been an
                        // issue before.
                        //
                        // We could of course now scan for local variables and NULL them out. But what if
                        // there are other DILs that have saved a reference to it? (and are extras even
                        // volatile)
                    }
                }
            }
            break;
    }
    delete v1;
    delete v2;
}

/* destroy unit */
void dilfi_dst(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("purge", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr && !((unit_data *)v1->val.ptr)->isRoom())
        {
            if (v1->val.ptr == p->sarg->owner)
            {
                extract_unit((unit_data *)v1->val.ptr);
                p->waitcmd = WAITCMD_DESTROYED;
            }
            else
            {
                extract_unit((unit_data *)v1->val.ptr);
                dil_test_secure(p);
            }
            if (((unit_data *)v1->val.ptr)->isPC())
            {
                if (CHAR_DESCRIPTOR((unit_data *)v1->val.ptr))
                {
                    descriptor_close(CHAR_DESCRIPTOR((unit_data *)v1->val.ptr));
                }
            }
        }
    }
    delete v1;
}

/* Execute command */
void dilfi_exec(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("exec", p, 2, v1, TYPEFAIL_NULL, 1, DILV_SP, v2, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr && v2->val.ptr && ((unit_data *)v2->val.ptr)->isChar())
        {
            char cmd[MAX_INPUT_LENGTH + 1];

            strncpy(cmd, (char *)v1->val.ptr, MAX_INPUT_LENGTH);
            cmd[MAX_INPUT_LENGTH] = 0;

            if (strlen((char *)v1->val.ptr) > MAX_INPUT_LENGTH)
            {
                slog(LOG_ALL, 0, "DIL %s issued command which was too long: %s", p->sarg->owner->getFileIndexSymName(), cmd);
            }

            if (IS_IMMORTAL((unit_data *)v2->val.ptr))
            {
                char buf[MAX_INPUT_LENGTH];
                command_info *cmd_ptr = nullptr;

                str_next_word(cmd, buf);

                if ((cmd_ptr = (command_info *)search_trie(buf, g_intr_trie)))
                {
                    if (cmd_ptr->minimum_level >= IMMORTAL_LEVEL)
                    {
                        slog(LOG_EXTENSIVE,
                             0,
                             "DIL %s on %s tried "
                             "to make %s do: %s",
                             p->fp->tmpl->prgname,
                             STR(p->sarg->owner->getNames().Name()),
                             ((unit_data *)v2->val.ptr)->getNames().Name(),
                             cmd);
                    }
                    else
                    {
                        command_interpreter((unit_data *)v2->val.ptr, cmd);
                        if (p && p->frame)
                        {
                            dil_test_secure(p);
                        }
                    }
                }
                else
                {
                    command_interpreter((unit_data *)v2->val.ptr, cmd);
                    if (p && p->frame)
                    {
                        dil_test_secure(p);
                    }
                }
            }
            else
            {
                command_interpreter((unit_data *)v2->val.ptr, cmd);
                if (p && p->frame)
                {
                    dil_test_secure(p);
                }
            }
        }
    }
    delete v1;
    delete v2;
}

/* Wait */
void dilfi_wit(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    ubit32 coreptr = 0;
    ubit8 *oldpc = nullptr;
    coreptr = bread_ubit32(&(p->fp->pc));
    oldpc = &(p->fp->tmpl->core[coreptr]);

    if (dil_type_check("wait", p, 1, v1, TYPEFAIL_NULL, 1, DILV_INT))
    {
        if (p->waitcmd != WAITCMD_MAXINST)
        {
            REMOVE_BIT(v1->val.num,
                       ~(SFB_DONE | SFB_PRE | SFB_CMD | SFB_TICK | SFB_RANTIME | SFB_DEAD | SFB_COM | SFB_MSG | SFB_EDIT | SFB_PROMPT));

            p->sarg->fptr->removeActivateOnEventFlag(SFB_CMD | SFB_TICK | SFB_DEAD | SFB_DONE | SFB_PRE | SFB_COM | SFB_MSG | SFB_SAVE |
                                                     SFB_ACTIVATE | SFB_EDIT | SFB_PROMPT);

            p->sarg->fptr->setActivateOnEventFlag(v1->val.num);

            p->waitcmd = WAITCMD_FINISH;
            p->fp->pc = oldpc; /* rewind pc to just before wait command */
        }
        else
        {
            if (IS_SET(v1->val.num, p->sarg->mflags) && dil_getbool(v2, p))
            {
                p->waitcmd--;
            }
            else
            {
                p->fp->pc = oldpc; /* rewind pc to just before command */
                p->waitcmd = WAITCMD_FINISH;
            }
        }
    }
    delete v1;
    delete v2;
}

/* Act call */
void dilfi_act(dilprg *p)
{
    /* Act call */

    dilval *v6 = p->stack.pop();
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    /* 6 arguments */

    p->waitcmd--;

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
                    act((char *)v1->val.ptr, v2->val.num, v3, v4, v5, v6->val.num);
                }
                break;

            case TO_VICT:
            case TO_NOTVICT:
                if (v5->val.ptr)
                {
                    act((char *)v1->val.ptr, v2->val.num, v3, v4, v5, v6->val.num);
                }
        }
    }
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
}

/* Goto new command */
void dilfi_goto(dilprg *p)
{
    ubit32 adr = 0;

    adr = bread_ubit32(&(p->fp->pc));
    p->fp->pc = &(p->fp->tmpl->core[adr]);
    p->waitcmd--;
}

/* Goto new command */
void dilfi_on(dilprg *p)
{
    dilval *v1 = p->stack.pop();
    ubit32 adr = 0;
    ubit16 maxlab = 0;
    ubit8 *brkptr = nullptr;

    p->waitcmd--;

    maxlab = bread_ubit16(&(p->fp->pc));
    brkptr = p->fp->pc + sizeof(ubit32) * (maxlab);

    if (dil_getval(v1) != DILV_INT)
    {
        p->fp->pc = brkptr;
    }
    else
    {
        if ((v1->val.num >= maxlab) || (v1->val.num < 0))
        {
            p->fp->pc = brkptr;
        }
        else
        {
            p->fp->pc += sizeof(ubit32) * (v1->val.num);
            adr = bread_ubit32(&(p->fp->pc));
            if (adr == SKIP)
            {
                p->fp->pc = brkptr;
            }
            else
            {
                p->fp->pc = &(p->fp->tmpl->core[adr]);
            }
        }
    }
    delete v1;
}

/* Substract affect from unit */
void dilfi_sua(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    unit_affected_type *af = nullptr;

    p->waitcmd--;

    if (dil_type_check("subaff", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, TYPEFAIL_NULL, 1, DILV_INT))
    {
        if (v1->val.ptr)
        {
            af = affected_by_spell((unit_data *)v1->val.ptr, v2->val.num);
            if (af)
            {
                destroy_affect(af);
            }
        }
    }
    delete v1;
    delete v2;
}

/* Add affect */
void dilfi_ada(dilprg *p)
{
    dilval *v11 = p->stack.pop();
    dilval *v10 = p->stack.pop();
    dilval *v9 = p->stack.pop();
    dilval *v8 = p->stack.pop();
    dilval *v7 = p->stack.pop();
    dilval *v6 = p->stack.pop();
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("addaff",
                       p,
                       11,
                       v1,
                       TYPEFAIL_NULL,
                       1,
                       DILV_UP,
                       v2,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v3,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v4,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v5,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v6,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v7,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v8,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v9,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v10,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v11,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT))
    {
        if (v1->val.ptr)
        {
            if (((unit_data *)v1->val.ptr)->isChar())
            {
                if (is_in(v2->val.num, 1, ID_TOP_IDX) && is_in(v8->val.num, TIF_NONE, TIF_MAX) && is_in(v9->val.num, TIF_NONE, TIF_MAX) &&
                    is_in(v10->val.num, TIF_NONE, TIF_MAX) && is_in(v11->val.num, APF_NONE, APF_MAX))
                {
                    if (p->frame[0].tmpl->zone->getAccessLevel() != 0)
                    {
                        szonelog(p->frame->tmpl->zone,
                                 "DIL '%s' attempt to violate system access security (ada).",
                                 p->frame->tmpl->prgname);
                        p->waitcmd = WAITCMD_QUIT;
                    }
                    else
                    {
                        unit_affected_type af;

                        af.setID(v2->val.num);
                        af.setDuration(v3->val.num);
                        af.setBeat(v4->val.num);

                        af.setDataAtIndex(0, v5->val.num);
                        af.setDataAtIndex(1, v6->val.num);
                        af.setDataAtIndex(2, v7->val.num);

                        af.setFirstFI(v8->val.num);
                        af.setTickFI(v9->val.num);
                        af.setLastFI(v10->val.num);
                        af.setApplyFI(v11->val.num);
                        create_affect((unit_data *)v1->val.ptr, &af);
                    }
                }
                else
                {
                    szonelog(p->frame->tmpl->zone,
                             "DIL '%s' addaffect parameters OOB (ada) v2=%d, v8=%d, v9=%d, v10=%d, v11=%d.",
                             p->frame->tmpl->prgname,
                             v2->val.num,
                             v8->val.num,
                             v9->val.num,
                             v10->val.num,
                             v11->val.num);
                }
            }
            else if (((unit_data *)v1->val.ptr)->isObj())
            {
                if ((is_in(-v2->val.num, 1, ID_TOP_IDX) || is_in(v2->val.num, 1, ID_TOP_IDX)) && is_in(v8->val.num, TIF_NONE, TIF_MAX) &&
                    is_in(v9->val.num, TIF_NONE, TIF_MAX) && is_in(v10->val.num, TIF_NONE, TIF_MAX) &&
                    is_in(v11->val.num, APF_NONE, APF_MAX))
                {
                    if (p->frame[0].tmpl->zone->getAccessLevel() != 0)
                    {
                        szonelog(p->frame->tmpl->zone,
                                 "DIL '%s' attempt to violate system access security (ada).",
                                 p->frame->tmpl->prgname);
                        p->waitcmd = WAITCMD_QUIT;
                    }
                    else
                    {
                        // This is a transfer affect

                        unit_affected_type af;

                        af.setID(v2->val.num); // Negative for object transfers
                        af.setDuration(v3->val.num);
                        af.setBeat(v4->val.num);

                        af.setDataAtIndex(0, v5->val.num);
                        af.setDataAtIndex(1, v6->val.num);
                        af.setDataAtIndex(2, v7->val.num);

                        af.setFirstFI(v8->val.num);
                        af.setTickFI(v9->val.num);
                        af.setLastFI(v10->val.num);
                        af.setApplyFI(v11->val.num);
                        create_affect((unit_data *)v1->val.ptr, &af);
                    }
                }
                else
                {
                    szonelog(p->frame->tmpl->zone,
                             "DIL '%s' addaffect parameters OOB (ada) v2=%d, v8=%d, v9=%d, v10=%d, v11=%d.",
                             p->frame->tmpl->prgname,
                             v2->val.num,
                             v8->val.num,
                             v9->val.num,
                             v10->val.num,
                             v11->val.num);
                }
            }
            else
            {
                szonelog(p->frame->tmpl->zone, "DIL '%s' addaffect unit is neither char nor object (ada).", p->frame->tmpl->prgname);
            }
        }
        else
        {
            szonelog(p->frame->tmpl->zone, "DIL '%s' addaffect, NULL unit pointer (ada).", p->frame->tmpl->prgname);
        }
    }
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
    delete v7;
    delete v8;
    delete v9;
    delete v10;
    delete v11;
}

/* Priority */
void dilfi_pri(dilprg *p)
{
    p->waitcmd--;
    p->sarg->fptr->setActivateOnEventFlag(SFB_PRIORITY);
}

/* No Priority */
void dilfi_npr(dilprg *p)
{
    p->sarg->fptr->removeActivateOnEventFlag(SFB_PRIORITY);
}

/* Send message to DIL programs in local environment */
void dilfi_snd(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("send", p, 1, v1, TYPEFAIL_NULL, 1, DILV_SP))
    {
        if (v1->val.ptr)
        {
            send_message(p->sarg->owner, (char *)v1->val.ptr);
            dil_test_secure(p);
        }
    }
    delete v1;
}

/* Send message to DIL programs in the specified unit */
void dilfi_snt(dilprg *p)
{
    /* sendto (string, uptr) */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("sendto", p, 2, v1, TYPEFAIL_NULL, 1, DILV_SP, v2, FAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr && v2->val.ptr)
        {
            spec_arg sarg;

            sarg.activator = p->sarg->owner;
            sarg.medium = nullptr;
            sarg.target = nullptr;
            sarg.pInt = nullptr;
            sarg.fptr = nullptr; /* Set by unit_function_scan */
            sarg.cmd = &g_cmd_auto_msg;
            sarg.arg = (char *)v1->val.ptr;
            sarg.mflags = SFB_MSG | SFB_AWARE;

            unit_function_scan((unit_data *)v2->val.ptr, &sarg);

            dil_test_secure(p);
        }
    }
    delete v1;
    delete v2;
}

void dilfi_snta(dilprg *p)
{
    /* Send message to DIL programs in all units of type specified */
    /* sendtoall (string, string) */

    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("sendtoall", p, 2, v1, TYPEFAIL_NULL, 1, DILV_SP, v2, TYPEFAIL_NULL, 1, DILV_SP))
    {
        if (v1->val.ptr && v2->val.ptr)
        {
            file_index_type *fi = nullptr;

            if ((fi = str_to_file_index((char *)v2->val.ptr)))
            {
                spec_arg sarg;

                sarg.activator = p->sarg->owner;
                sarg.medium = p->sarg->owner;
                sarg.target = nullptr;
                sarg.pInt = nullptr;
                sarg.fptr = nullptr; /* Set by unit_function_scan */

                sarg.fptr = nullptr;
                sarg.cmd = &g_cmd_auto_msg;
                sarg.arg = (char *)v1->val.ptr;
                sarg.mflags = SFB_MSG;

                if (!fi->Empty())
                {
                    std::forward_list<unit_data *>::iterator it;
                    std::forward_list<unit_data *>::iterator next_it;
                    for (it = fi->Begin(); it != fi->End(); it = next_it)
                    {
                        next_it = it;
                        next_it++;
                        unit_function_scan(*it, &sarg);
                    }
                }

                /*for (u = g_unit_list; u; u = u->gnext)
                {
                    if (UNIT_FILE_INDEX(u) == fi)
                        unit_function_scan(u, &sarg);
                }*/
                dil_test_secure(p);
            }
        }
    }
    delete v1;
    delete v2;
}

void dilfi_sntadil(dilprg *p)
{
    /* Send message to DIL programs in all units of type specified */
    /* sendtoall (string<message>, string<template> ) */

    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("sendtoalldil", p, 2, v1, TYPEFAIL_NULL, 1, DILV_SP, v2, TYPEFAIL_NULL, 1, DILV_SP))
    {
        if (v1->val.ptr && !str_is_empty((char *)v2->val.ptr))
        {
            diltemplate *tmpl = nullptr;

            tmpl = find_dil_template((const char *)v2->val.ptr);

            if (tmpl)
            {
                dilprg *tp = nullptr;
                if (tmpl->nextdude)
                {
                    slog(LOG_ALL, 0, "INVESTIGATE: DIL sendtoall() we appear to have a nested sendtoall() with nextdude.");
                }

                for (tp = tmpl->prg_list; tp; tp = tmpl->nextdude)
                {
                    tmpl->nextdude = tp->next;

                    if (tp->fp && tp->fp->tmpl == tmpl && tp != p)
                    {
                        unit_fptr *fptr = nullptr;

                        /* If it is destructed, then it cant be found because data
                           will be null */

                        for (fptr = tp->owner->getFunctionPointer(); fptr; fptr = fptr->getNext())
                        {
                            if (fptr->getFunctionPointerIndex() == SFUN_DIL_INTERNAL && fptr->getData() &&
                                ((dilprg *)fptr->getData())->fp && ((dilprg *)fptr->getData())->fp->tmpl == tmpl)
                            {
                                break;
                            }
                        }

                        if (!fptr)
                        {
                            continue;
                        }

                        spec_arg sarg;

                        sarg.owner = tp->owner;
                        sarg.activator = p->owner;
                        sarg.medium = p->owner;
                        sarg.target = tp->owner;
                        sarg.pInt = nullptr;
                        sarg.fptr = fptr;

                        sarg.cmd = &g_cmd_auto_msg;
                        sarg.arg = (char *)v1->val.ptr;
                        sarg.mflags = SFB_MSG;

                        function_activate(tp->owner, &sarg);
                    }
                } /* for */
                tmpl->nextdude = nullptr;
                dil_test_secure(p);
            }
        }
    }
    delete v1;
    delete v2;
}

// DIL log string
void dilfi_log(dilprg *p)
{
    /* Log a string to the logs. */

    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("log", p, 1, v1, TYPEFAIL_NULL, 1, DILV_SP))
    {
        if (v1->val.ptr)
        {
            szonelog(p->owner->getFileIndex()->getZone(), "%s", (char *)v1->val.ptr);
        }
    }
    delete v1;
}

/* Secure */
void dilfi_sec(dilprg *p)
{
    dilval *v1 = p->stack.pop();
    ubit32 adr = 0; /* address */

    adr = bread_ubit32(&(p->fp->pc));

    p->waitcmd--;

    if (dil_type_check("secure", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr)
        {
            dil_sub_secure(p->fp, (unit_data *)v1->val.ptr);
            dil_add_secure(p, (unit_data *)v1->val.ptr, &(p->fp->tmpl->core[adr]));
        }
    }
    delete v1;
}

/* Unsecure */
void dilfi_use(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("unsecure", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr)
        {
            dil_sub_secure(p->fp, (unit_data *)v1->val.ptr);
        }
    }
    delete v1;
}

void dilfi_popstk(dilprg *p)
{
    dilval *v1 = p->stack.pop();
    delete v1;
}

/* Equip unit in inventory of PC/NPC */
void dilfi_eqp(dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("equip", p, 2, v1, TYPEFAIL_NULL, 1, DILV_UP, v2, TYPEFAIL_NULL, 1, DILV_INT))
    {
        auto *unit = reinterpret_cast<unit_data *>(v1->val.ptr);
        if (unit && unit->getUnitIn() && unit->getUnitIn()->isChar() && unit->isObj() && !equipment(unit->getUnitIn(), v2->val.num))
        {
            /* Then equip char */
            equip_char(unit->getUnitIn(), unit, v2->val.num);
        }
    }
    delete v1;
    delete v2;
}

/* Unequip unit in inventory of PC/NPC */
void dilfi_ueq(dilprg *p)
{
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("unequip", p, 1, v1, FAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isObj() && OBJ_EQP_POS((unit_data *)v1->val.ptr))
        {
            unequip_object((unit_data *)v1->val.ptr);
        }
    }
    delete v1;
}

void dilfi_quit(dilprg *p)
{
    p->waitcmd = WAITCMD_QUIT;
}

/* Block */
void dilfi_blk(dilprg *p)
{
    p->waitcmd--;
    SET_BIT(p->flags, DILFL_CMDBLOCK);
}

void dilfi_pup(dilprg *p)
{
    dilval *v1 = p->stack.pop();
    p->waitcmd--;

    if (dil_type_check("updatepos", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (v1->val.ptr && ((unit_data *)v1->val.ptr)->isChar())
        {
            update_pos((unit_data *)v1->val.ptr);
            if (CHAR_POS((unit_data *)v1->val.ptr) == POSITION_DEAD)
            {
                die((unit_data *)v1->val.ptr);
                dil_test_secure(p);
            }
        }
    }
    delete v1;
}

void dilfi_cast(dilprg *p)
{
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    unit_data *caster = nullptr;
    unit_data *medium = nullptr;
    unit_data *target = nullptr;

    p->waitcmd--;

    if (dil_type_check("cast",
                       p,
                       4,
                       v1,
                       TYPEFAIL_NULL,
                       1,
                       DILV_INT,
                       v2,
                       TYPEFAIL_NULL,
                       1,
                       DILV_UP,
                       v3,
                       TYPEFAIL_NULL,
                       1,
                       DILV_UP,
                       v4,
                       TYPEFAIL_NULL,
                       1,
                       DILV_UP))
    {
        caster = (unit_data *)v2->val.ptr;
        medium = (unit_data *)v3->val.ptr;
        target = (unit_data *)v4->val.ptr;

        if (is_in(v1->val.num, SPL_GROUP_MAX, SPL_TREE_MAX - 1) && caster && caster->isChar() && medium &&
            (g_spell_info[v1->val.num].spell_pointer || g_spell_info[v1->val.num].tmpl))
        {
            /* cast the spell */
            spell_perform(v1->val.num, MEDIA_SPELL, caster, medium, target, "");
            dil_test_secure(p);
        }
    }

    delete v1;
    delete v2;
    delete v3;
    delete v4;
}

unit_data *hometown_unit(const char *str)
{
    if (str)
    {
        char name[80];
        char zone[80];
        unit_data *u = nullptr;

        split_fi_ref(str, zone, name);
        if ((u = world_room(zone, name)))
        {
            return u;
        }
    }

    return g_entry_room;
}
