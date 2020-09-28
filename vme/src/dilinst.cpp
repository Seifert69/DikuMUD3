/*
 $Author: All $
 $RCSfile: dilinst.cpp,v $
 $Date: 2004/05/16 04:34:34 $
 $Revision: 2.21 $
 */

#ifdef _WINDOWS
#include <direct.h>
#else

#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h> /* For type_check */

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
#include "db_file.h"
#include "vmelimits.h"
#include "common.h"
#include "spells.h"
#include "db_file.h"
#include "dilinst.h"
#include "dilrun.h"
#include "magic.h"
#include "trie.h"
#include "main.h"
#include "account.h"
#include "justice.h"
#include "files.h"
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "hookmud.h"

extern struct trie_type *intr_trie;
void stop_special(class unit_data *u, class unit_fptr *fptr);
void start_special(class unit_data *u, class unit_fptr *fptr);

/* report error in instruction */
void dil_stop_special(class unit_data *unt, class dilprg *aprg)
{
    class unit_fptr *fptr;
    class unit_data *u;
    for (u = unt; u; u = u->next)
    {
        DeactivateDil(u, aprg);
        for (fptr = UNIT_FUNC(u); fptr; fptr = fptr->next)
        {
            if (aprg && (aprg == fptr->data))
                continue;
            else
                stop_special(u, fptr);
        }

        if (UNIT_CONTAINS(u))
            dil_stop_special(UNIT_CONTAINS(u), aprg);
    }
}

void dil_start_special(class unit_data *unt, class dilprg *aprg)
{
    class unit_fptr *fptr;
    class unit_data *u;
    for (u = unt; u; u = u->next)
    {
        ActivateDil(u);
        for (fptr = UNIT_FUNC(u); fptr; fptr = fptr->next)
        {
            if (aprg && (aprg == fptr->data))
                continue;
            else
                start_special(u, fptr);
        }

        if (UNIT_CONTAINS(u))
            dil_start_special(UNIT_CONTAINS(u), aprg);
    }
}

void dil_insterr(class dilprg *p, char *where)
{
    /* instruction called as an expression! */
    /* This is serous! mess-up in the core.. stop the program */

    szonelog(UNIT_FI_ZONE(p->sarg->owner),
             "DIL %s@%s, Instruction error in %s\n",
             UNIT_FI_NAME(p->sarg->owner),
             UNIT_FI_ZONENAME(p->sarg->owner), where);
    p->waitcmd = WAITCMD_QUIT;
}

/* ************************************************************************ */
/* DIL-instructions							    */
/* ************************************************************************ */
void dilfi_edit(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();

    if (dil_type_check("beginedit", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (!IS_PC((class unit_data *)v1->val.ptr))
            dil_typeerr(p, "not a pc unit");
        else
        {
            CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr)->postedit =
                dil_edit_done;
            CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr)->editing =
                p->owner;
            CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr)->editref = NULL;

            set_descriptor_fptr(CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr),
                                interpreter_string_add, TRUE);
        }
    }

    delete v1;
}

void dilfi_kedit(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();
    class descriptor_data *d;

    if (dil_type_check("killedit", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (!IS_PC((class unit_data *)v1->val.ptr))
            dil_typeerr(p, "not a pc unit");
        else
        {
            d = CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr);
            if (d)
            {
                if (d->postedit)
                    d->postedit(d);
                if (d->localstr)
                    FREE(d->localstr);
                d->localstr = NULL;
                d->editref = NULL;
                d->postedit = NULL;
                d->editing = NULL;
                set_descriptor_fptr(CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr),
                                    descriptor_interpreter, TRUE);
            }
        }
    }
    delete v1;
}

void dilfi_gamestate(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    class unit_data *load_room;

    if (dil_type_check("gamestate", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, TYPEFAIL_NULL, 1, DILV_INT))
    {
        if (!IS_PC((class unit_data *)v1->val.ptr))
            dil_typeerr(p, "gamestate: Not a pc unit");
        else
        {
            switch (v2->val.num)
            {
            case GS_PLAY:
                if (!char_is_playing((class unit_data *)v1->val.ptr))
                {
                    insert_in_unit_list((class unit_data *)v1->val.ptr);

                    if (CHAR_LAST_ROOM((class unit_data *)v1->val.ptr))
                    {
                        load_room =
                            CHAR_LAST_ROOM((class unit_data *)v1->val.ptr);
                        CHAR_LAST_ROOM((class unit_data *)v1->val.ptr) = NULL;
                    }
                    else
                        load_room =
                            hometown_unit(PC_HOME((class unit_data *)v1->val.ptr));

                    unit_to_unit((class unit_data *)v1->val.ptr, load_room);
                    dil_start_special((class unit_data *)v1->val.ptr, p);
                }
                break;
            case GS_QUIT:
                extract_unit((class unit_data *)v1->val.ptr);
                break;
            case GS_MENU:
                if (char_is_playing((class unit_data *)v1->val.ptr))
                {

                    CHAR_LAST_ROOM((class unit_data *)v1->val.ptr) =
                        unit_room((class unit_data *)v1->val.ptr);
                    unit_from_unit((class unit_data *)v1->val.ptr);
                    remove_from_unit_list((class unit_data *)v1->val.ptr);
                    dil_stop_special((class unit_data *)v1->val.ptr, p);
                }
                break;
            case GS_LINK_DEAD:
                if (CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr))
                    descriptor_close(CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr));
                break;
            }
        }
    }
    delete v1;
    delete v2;
}

void dilfi_send_done(register class dilprg *p)
{
    struct command_info *cmd_ptr;
    dilval *v7 = p->stack.pop();
    dilval *v6 = p->stack.pop();
    dilval *v5 = p->stack.pop();
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("send_done", p, 7,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, TYPEFAIL_NULL, 2, DILV_UP, DILV_NULL,
                       v3, TYPEFAIL_NULL, 2, DILV_UP, DILV_NULL,
                       v4, TYPEFAIL_NULL, 2, DILV_UP, DILV_NULL,
                       v5, TYPEFAIL_NULL, 1, DILV_INT,
                       v6, TYPEFAIL_NULL, 1, DILV_SP,
                       v7, TYPEFAIL_NULL, 2, DILV_UP, DILV_NULL))
    {
        if ((cmd_ptr =
                 (struct command_info *)search_trie((char *)v1->val.ptr,
                                                    intr_trie)))
        {
            send_done((class unit_data *)v2->val.ptr,
                      (class unit_data *)v3->val.ptr,
                      (class unit_data *)v4->val.ptr,
                      v5->val.num,
                      cmd_ptr,
                      (const char *)v6->val.ptr,
                      (class unit_data *)v7->val.ptr);
        }
        else
        {
            szonelog(p->frame->tmpl->zone,
                     "send_done: DIL '%s' attempted to send a non-existent command '%s'.",
                     p->frame->tmpl->prgname, (char *)v1->val.ptr);
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
void dilfi_pgstr(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("pagestring", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (!IS_PC((class unit_data *)v2->val.ptr) && !IS_NPC((class unit_data *)v2->val.ptr))
            dil_typeerr(p, "not a pc/npc unit");
        else if (v1->val.ptr && v2->val.ptr)
            page_string(CHAR_DESCRIPTOR((class unit_data *)v2->val.ptr),
                        (char *)v1->val.ptr);
    }
    delete v2;
    delete v1;
}

void dilfi_setpwd(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("setpassword", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, TYPEFAIL_NULL, 1, DILV_SP))
    {
        if (!IS_PC((class unit_data *)v1->val.ptr))
            dil_typeerr(p, "Not a pc unit");
        else if (p->frame[0].tmpl->zone->access > 0)
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempt to violate system access security (setpassword).",
                     p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else if (v1->val.ptr && v2->val.ptr)
            strncpy(PC_PWD((class unit_data *)v1->val.ptr),
                    crypt((char *)v2->val.ptr,
                          PC_FILENAME((class unit_data *)v1->val.ptr)),
                    10);
    }
    delete v1;
    delete v2;
}

void dilfi_delpc(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();
    class descriptor_data *d;
    class unit_data *tmp;

    if (dil_type_check("delete_player", p, 1, v1, TYPEFAIL_NULL, 1, DILV_SP))
    {
        if (p->frame[0].tmpl->zone->access > 0)
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempt to violate system access security (delete_player).",
                     p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {
            int delete_player(const char *);

            if (v1->val.ptr)
            {
                if ((d = find_descriptor(((char *)v1->val.ptr), NULL)))
                {
                    extract_unit(d->character);
                    descriptor_close(d);
                }
                else
                {
                    for (tmp = unit_list; tmp; tmp = tmp->gnext)
                        if (IS_PC(tmp) && !str_ccmp(UNIT_NAME(tmp),
                                                    ((char *)v1->val.ptr)))
                            extract_unit(tmp);
                }

                delete_player((char *)v1->val.ptr);
            }
        }
    }
    delete v1;
}

void dilfi_reboot(register class dilprg *p)
{
    extern int mud_shutdown, mud_reboot;
    if (p->frame[0].tmpl->zone->access > 0)
    {
        szonelog(p->frame->tmpl->zone,
                 "DIL '%s' attempt to violate system access security (reboot).",
                 p->frame->tmpl->prgname);
        p->waitcmd = WAITCMD_QUIT;
        return;
    }

    mud_shutdown = mud_reboot = 1;
}

/* foreach - clear / build */
void dilfi_foe(class dilprg *p)
{
    dilval *v1 = p->stack.pop();
    int i;

    if (dil_type_check("foreach - build", p, 1,
                       v1, TYPEFAIL_NULL, 1, DILV_INT))
    {
        /* build environment into secures with NULL labels */

        dil_getval(v1);

        if (v1->val.num)
        {
            for (i = 0; i < p->fp->securecount; i++)
                if (p->fp->secure[i].lab == NULL)
                {
                    dil_sub_secure(p->fp, p->fp->secure[i].sup, TRUE);
                    i--; // Shit
                }

            if (UNIT_IN(p->sarg->owner))
                scan4_unit(p->sarg->owner, v1->val.num);
            else
                scan4_unit_room(p->sarg->owner, v1->val.num);

            for (i = 0; i < unit_vector.top; i++)
                dil_add_secure(p, UVI(i), NULL);
            dil_add_secure(p, p->sarg->owner, NULL);
        }
    }
    delete v1;
}

/* foreach - next */
void dilfi_fon(class dilprg *p)
{
    dilval *v1 = p->stack.pop();
    class unit_data *u;
    ubit32 adr;
    int i;

    adr = bread_ubit32(&(p->fp->pc));
    p->waitcmd--;

    if (v1->type != DILV_UPR)
        dil_typeerr(p, "foreach - next");
    else
    {
        dil_test_secure(p, TRUE);
        /* look for NULL references, remove first */
        u = NULL;
        for (i = 0; i < p->fp->securecount; i++)
            if (!p->fp->secure[i].lab)
            {
                u = p->fp->secure[i].sup;
                if (u->is_destructed())
                {
                    dil_sub_secure(p->fp, u, TRUE);
                    i--;
                    u = NULL;
                    continue;
                }
                break;
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

/* storeall */
void dilfi_stora(class dilprg *p)
{
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    char filename[512];
    void store_all_unit(class unit_data * u, char *fname, int svcont);

    if (dil_type_check("storeall", p, 3,
                       v1, FAIL_NULL, 1, DILV_UP,
                       v2, FAIL_NULL, 1, DILV_SP, v3, FAIL_NULL, 1, DILV_INT))
    {
        if (p->frame[0].tmpl->zone->access > 10)
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempt to violate system access security (storeall).",
                     p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {

            if ((v1->val.ptr) && (v2->val.ptr))
            {
                if (!store_name_test((char *)v2->val.ptr))
                {
                    szonelog(p->frame->tmpl->zone,
                             "DIL '%s' attempt to store illegal file name (storeall).",
                             p->frame->tmpl->prgname);
                }
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
                    strcat(filename, "/units/");
                    if (!file_exists(filename))
#ifdef _WINDOWS
                        _mkdir(filename);
#else
                        mkdir(filename,
                              S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP |
                                  S_IXGRP);
#endif
                    strcat(filename, (char *)v2->val.ptr);

                    if (v3->val.num >= 1)
                    {
                        if (IS_ROOM((class unit_data *)v1->val.ptr) ||
                            IS_PC((class unit_data *)v1->val.ptr))
                        {
                            szonelog(p->frame->tmpl->zone,
                                     "DIL '%s' attempt to save a container that is either a room or pc",
                                     p->frame->tmpl->prgname);
                        }
                        else
                            store_all_unit((class unit_data *)v1->val.ptr,
                                           filename, TRUE);
                    }
                    else
                    {
                        store_all_unit((class unit_data *)v1->val.ptr,
                                       filename, FALSE);
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
void dilfi_sbt(class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int dif;

    if (dil_type_check("setbright", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, FAIL_NULL, 1, DILV_INT))
        if (v1->val.ptr)
        {
            dif = v2->val.num - UNIT_BRIGHT((class unit_data *)v1->val.ptr);

            modify_bright((class unit_data *)v1->val.ptr, dif);
        }

    delete v1;
    delete v2;
}

/* acc_modify */
void dilfi_amod(class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("acc_modify", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, FAIL_NULL, 1, DILV_INT))
        if (g_cServerConfig.m_bAccounting &&
            v1->val.ptr && IS_PC((class unit_data *)v1->val.ptr))
        {
            if (p->frame[0].tmpl->zone->access != 0)
            {
                szonelog(p->frame->tmpl->zone,
                         "DIL '%s' attempt to violate system access security (amod).",
                         p->frame->tmpl->prgname);
                p->waitcmd = WAITCMD_QUIT;
            }
            else
            {

                if (v2->val.num > 0)
                    account_insert(p->owner,
                                   (class unit_data *)v1->val.ptr, v2->val.num);
                else if (v2->val.num < 0)
                    account_withdraw(p->owner,
                                     (class unit_data *)v1->val.ptr,
                                     -v2->val.num);
            }
        }
    delete v1;
    delete v2;
}

/* set weight */
void dilfi_swt(class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    int dif;

    if (dil_type_check("setweight", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, FAIL_NULL, 1, DILV_INT))
        if (v1->val.ptr)
        {
            dif =
                v2->val.num - UNIT_BASE_WEIGHT((class unit_data *)v1->val.ptr);

            /* set new baseweight */
            UNIT_BASE_WEIGHT((class unit_data *)v1->val.ptr) = v2->val.num;

            /* update weight */
            weight_change_unit((class unit_data *)v1->val.ptr, dif);
        }
    delete v1;
    delete v2;
}

/* change_speed */
void dilfi_chas(class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("change_speed", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, FAIL_NULL, 1, DILV_INT))
        if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr) &&
            CHAR_COMBAT((class unit_data *)v1->val.ptr))
            CHAR_COMBAT((class unit_data *)v1->val.ptr)->changeSpeed(v2->val.num);
    delete v1;
    delete v2;
}

/* reset_level */
void dilfi_rslv(class dilprg *p)
{
    dilval *v1 = p->stack.pop();

    if (dil_type_check("reset_level", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (p->frame[0].tmpl->zone->access > 1)
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempt to reset level w/o access.",
                     p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {
            if (IS_PC((class unit_data *)v1->val.ptr))
                if (!IS_IMMORTAL((class unit_data *)v1->val.ptr))
                    CHAR_LEVEL((class unit_data *)v1->val.ptr) = 1;
        }
    }
    delete v1;
}

/* reset_vlevel */
void dilfi_rsvlv(class dilprg *p)
{
    dilval *v1 = p->stack.pop();

    if (dil_type_check("reset_vlevel", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (p->frame[0].tmpl->zone->access > 1)
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempt to reset vlevel w/o access.",
                     p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {
            if (IS_PC((class unit_data *)v1->val.ptr))
                if (!IS_IMMORTAL((class unit_data *)v1->val.ptr))
                    PC_VIRTUAL_LEVEL((class unit_data *)v1->val.ptr) = 1;
        }
    }
    delete v1;
}

/* reset_race */
void dilfi_rsrce(class dilprg *p)
{
    dilval *v1 = p->stack.pop();
    void race_adjust(class unit_data * ch);

    if (dil_type_check("reset_race", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
    {
        if (p->frame[0].tmpl->zone->access > 1)
        {
            szonelog(p->frame->tmpl->zone,
                     "DIL '%s' attempt to reset race w/o access.",
                     p->frame->tmpl->prgname);
            p->waitcmd = WAITCMD_QUIT;
        }
        else
        {
            if (IS_PC((class unit_data *)v1->val.ptr))
            {
                race_adjust((class unit_data *)v1->val.ptr);
            }
        }
    }
    delete v1;
}

/* stop_fighting */
void dilfi_stopf(class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("stop_fighting", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, TYPEFAIL_NULL, 2, DILV_UP, DILV_NULL))
        if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
        {
            if (v2->val.ptr && IS_CHAR((class unit_data *)v2->val.ptr))
            {
                stop_fighting((class unit_data *)v1->val.ptr,
                              (class unit_data *)v2->val.ptr);
            }
            else
            {
                stop_fighting((class unit_data *)v1->val.ptr, NULL);
            }
        }
    delete v1;
    delete v2;
}

/* set_fighting */
void dilfi_setf(class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    if (dil_type_check("set_fighting", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, TYPEFAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr) &&
            v2->val.ptr && IS_CHAR((class unit_data *)v2->val.ptr))
        {
            if (CHAR_FIGHTING((class unit_data *)v1->val.ptr))
                set_fighting((class unit_data *)v1->val.ptr,
                             (class unit_data *)v2->val.ptr, FALSE);
            else
                set_fighting((class unit_data *)v1->val.ptr,
                             (class unit_data *)v2->val.ptr, TRUE);
        }
    delete v1;
    delete v2;
}

/* clear interrupt */
void dilfi_cli(class dilprg *p)
{
    dilval *v1 = p->stack.pop();

    if (dil_type_check("clear", p, 1, v1, FAIL_NULL, 1, DILV_INT))
        dil_intr_remove(p, v1->val.num);
    delete v1;
}

/* Return from proc */
void dilfi_rtf(class dilprg *p)
{
    struct dilframe *cfrm;
    ubit8 typ;
    int i;
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
            slog(LOG_ALL, 0,
                 "DIL %s@%s on %s@%s: Unexpected stack length after procedure call.",
                 p->fp->tmpl->prgname,
                 p->fp->tmpl->zone->name,
                 UNIT_FI_NAME(p->sarg->owner),
                 UNIT_FI_ZONENAME(p->sarg->owner));
            p->waitcmd = WAITCMD_QUIT;
            return;
        }
    }
    else
    {
        // There is a return variable lets get it and make it a copy on the stack
        if (p->stack.length() != p->fp->stacklen + 1)
        {
            slog(LOG_ALL, 0,
                 "DIL %s@%s on %s@%s: Unexpected stack length after function call.",
                 p->fp->tmpl->prgname,
                 p->fp->tmpl->zone->name,
                 UNIT_FI_NAME(p->sarg->owner),
                 UNIT_FI_ZONENAME(p->sarg->owner));
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
            v->val.ptr = (class unit_data *)v1->val.ptr;
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
            v->val.ptr = (class extra_descr_data *)v1->val.ptr;
            break;

        case DILV_ZP:
            v->type = DILV_ZP;
            v->atyp = DILA_NORM;
            v->val.ptr = (class zone_type *)v1->val.ptr;
            break;

        case DILV_CP:
            v->type = DILV_CP;
            v->atyp = DILA_NORM;
            v->val.ptr = (struct command_info *)v1->val.ptr;
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

void dil_pop_frame(class dilprg *p)
{
    dilfi_rtf(p); /* XXX Is this a potential memory leak rts / rtf? */
}

/*
   'p' is the program in which a new frame should be created.
   'rtmpl' is the template which the frame should be based upon.
   'av' are the arguments which are to be passed to the new frame.
*/

void dil_push_frame(class dilprg *p, struct diltemplate *rtmpl)
{
    int i;
    struct dilframe *frm;

    i = p->fp - p->frame;

    if (i + 2 > p->framesz)
    {
        p->framesz += DIL_FRAMEINC;
        RECREATE(p->frame, struct dilframe, p->framesz);
    }

    frm = &p->frame[i + 1];
    p->fp = frm;

    frm->tmpl = rtmpl;
    frm->pc = rtmpl->core;
    frm->securecount = 0;
    frm->secure = NULL;

    frm->intrcount = rtmpl->intrcount;

    if (rtmpl->intrcount)
    {
        CREATE(frm->intr, struct dilintr, rtmpl->intrcount);
    }
    else
        frm->intr = NULL;

    if (rtmpl->varc)
    {
        CREATE(frm->vars, struct dilvar, rtmpl->varc);

        for (i = 0; i < rtmpl->varc; i++)
        {
            frm->vars[i].type = rtmpl->vart[i];
            if (i >= rtmpl->argc)
            {
                if (frm->vars[i].type == DILV_SLP)
                    frm->vars[i].val.namelist = new cNamelist;
                else if (frm->vars[i].type == DILV_ILP)
                    frm->vars[i].val.intlist = new cintlist;
                else
                    frm->vars[i].val.string = NULL;
            }
            else
                frm->vars[i].val.string = NULL;
        }
    }
    else
        frm->vars = NULL;

    ubit8 tmp;

    for (i = 0; i < rtmpl->argc; i++)
    {
        tmp = dil_getval(p->stack[-(rtmpl->argc - i)]);

        if (tmp != frm->vars[i].type) /* Can fail, err, be null, etc */
        {
            if (tmp != DILV_NULL)
            {
                szonelog(UNIT_FI_ZONE(p->sarg->owner),
                         "DIL %s@%s Error in %s in remote call to %s where parameter %d has incorrect type. Stopping program.",
                         UNIT_FI_NAME(p->sarg->owner),
                         UNIT_FI_ZONENAME(p->sarg->owner),
                         p->frame[0].tmpl->prgname,
                         p->fp->tmpl->prgname, i+1);
                p->waitcmd = WAITCMD_QUIT;
                break;
            }
        }

        switch (frm->vars[i].type)
        {
        case DILV_UP:
            frm->vars[i].val.unitptr =
                (class unit_data *)p->stack[-(rtmpl->argc - i)]->val.ptr;
            break;

        case DILV_SP:
            frm->vars[i].val.string =
                str_dup((char *)p->stack[-(rtmpl->argc - i)]->val.ptr);
            break;

        case DILV_SLP:
            if (tmp != DILV_NULL)
                frm->vars[i].val.namelist =
                    ((cNamelist *)p->stack[-(rtmpl->argc - i)]->val.ptr)->Duplicate();
            break;

        case DILV_ILP:
            if (tmp != DILV_NULL)
                frm->vars[i].val.intlist =
                    ((cintlist *)p->stack[-(rtmpl->argc - i)]->val.ptr)->Duplicate();
            break;

        case DILV_EDP:
            frm->vars[i].val.extraptr = (class extra_descr_data *)
                                            p->stack[-(rtmpl->argc - i)]
                                                ->val.ptr;
            break;

        case DILV_CP:
            frm->vars[i].val.cmdptr =
                (struct command_info *)p->stack[-(rtmpl->argc - i)]->val.ptr;
            break;

        case DILV_ZP:
            frm->vars[i].val.zoneptr =
                (class zone_type *)p->stack[-(rtmpl->argc - i)]->val.ptr;
            break;

        case DILV_INT:
            frm->vars[i].val.integer = p->stack[-(rtmpl->argc - i)]->val.num;
            break;

        default:
            error(HERE, "Impossible!");
        }
    }
    for (i = 0; i < rtmpl->argc; i++)
        delete (p->stack.pop());

    frm->stacklen = p->stack.length();
}

/* Remote function call */
void dilfi_rfunc(register class dilprg *p)
{
    int xrefi;
    struct diltemplate *ctmpl;
    ubit8 argcnt;

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
        szonelog(UNIT_FI_ZONE(p->sarg->owner),
                 "DIL %s@%s, dil %s@%s Error in remote call function #%d not valid\n",
                 UNIT_FI_NAME(p->sarg->owner),
                 UNIT_FI_ZONENAME(p->sarg->owner),
                 p->fp->tmpl->prgname,
                 p->fp->tmpl->zone->name,
                 xrefi);
        p->waitcmd = WAITCMD_STOP;
        for (int i = 0; (i < argcnt); i++)
            delete (p->stack.pop());
        return;
    }
    /* expand frame */
    dil_push_frame(p, ctmpl->extprg[xrefi]);
}

/* Remote symbolic function call */
void dilfi_rsfunc(register class dilprg *p)
{
    int i;
    struct diltemplate /* *ctmpl,*/ *ntmpl;
    dilval *v1 = p->stack.pop();
    ubit8 argcnt;
    int fail;

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
                            fail = TRUE; /* type error, expect integer */
                        break;
                    case DILV_SP:
                        if ((t != DILV_NULL) && (t != DILV_SP))
                            fail = TRUE; /* type error, expect string */
                        break;
                    case DILV_UP:
                        if ((t != DILV_NULL) && (t != DILV_UP))
                            fail = TRUE; /* type error, expect unitptr */
                        break;
                    case DILV_ZP:
                        if ((t != DILV_NULL) && (t != DILV_ZP))
                            fail = TRUE; /* type error, expect unitptr */
                        break;
                    case DILV_CP:
                        if ((t != DILV_NULL) && (t != DILV_CP))
                            fail = TRUE; /* type error, expect unitptr */
                        break;
                    case DILV_EDP:
                        if ((t != DILV_NULL) && (t != DILV_EDP))
                            fail = TRUE; /* type error, expect extraptr */
                        break;
                    case DILV_SLP:
                        if ((t != DILV_NULL) && (t != DILV_SLP))
                            fail = TRUE; /* type error, expect stringlist */
                        break;
                    case DILV_ILP:
                        if ((t != DILV_NULL) && (t != DILV_SLP))
                            fail = TRUE; /* type error, expect stringlist */
                        break;
                    }
                }
            }
            else
                fail = TRUE;
        }
        else
            fail = TRUE;
    }
    else
        fail = TRUE;

    if (fail)
    {
        szonelog(UNIT_FI_ZONE(p->sarg->owner),
                 "DIL %s@%s, dil %s Error in symbolic remote call string=%s\n",
                 UNIT_FI_NAME(p->sarg->owner),
                 UNIT_FI_ZONENAME(p->sarg->owner), p->fp->tmpl->prgname,
                 (char *)v1->val.ptr);
        p->waitcmd = WAITCMD_STOP;

        delete v1;
        for (i = 0; (i < argcnt); i++)
            delete (p->stack.pop());
    }
    else
    {
        delete v1;
        dil_push_frame(p, ntmpl);
    }
}

/* Assignment of value to reference */
void dilfi_ass(register class dilprg *p)
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
            *((void **)v1->ref) = NULL;
            break;

        case DILV_UP:
            *((class unit_data **)v1->ref) = (class unit_data *)v2->val.ptr;
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
            *((void **)v1->ref) = NULL;
            break;

        case DILV_ZP:
            *((class zone_type **)v1->ref) = (class zone_type *)v2->val.ptr;
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
            *((void **)v1->ref) = NULL;
            break;

        case DILV_CP:
            *((struct command_info **)v1->ref) =
                (struct command_info *)v2->val.ptr;
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
            ((string *)v1->ref)->empty();
            break;

        case DILV_SP:
            (*(string *)v1->ref) = ((char *)v2->val.ptr);
            break;

        case DILV_HASHSTR:
            (*(string *)v1->ref) = (*(string *)v2->ref);
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
            dil_typeerr(p, "ordinary string assignment <- hash");

        switch (dil_getval(v2))
        {
        case DILV_FAIL:
            break;

        case DILV_NULL:
            *((void **)v1->ref) = NULL;
            break;

        case DILV_HASHSTR:
            if (v2->ref)
                *((char **)v1->ref) = str_dup(((string *)v2->ref)->c_str());
            else
                *((char **)v1->ref) = str_dup("");
            break;

        case DILV_SP:
            if (v2->val.ptr)
                *((char **)v1->ref) = str_dup((const char *) v2->val.ptr);
            else
                *((char **)v1->ref) = str_dup("");
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
                ((cNamelist *)v1->ref)->CopyList((cNamelist *)v2->val.ptr);
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
                ((cintlist *)v1->ref)->CopyList((cintlist *)v2->val.ptr);
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
            *((void **)v1->ref) = NULL;
            break;

        case DILV_EDP:
            *((class extra_descr_data **)v1->ref) =
                (class extra_descr_data *)v2->val.ptr;
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
void dilfi_lnk(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("link", p, 2,
                       v1, FAIL_NULL, 1, DILV_UP, v2, FAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr && v2->val.ptr)
        {
            if (IS_OBJ((class unit_data *)v1->val.ptr) &&
                OBJ_EQP_POS((class unit_data *)v1->val.ptr))
            {
                unequip_object((class unit_data *)v1->val.ptr);
            }
            if (!unit_recursive((class unit_data *)v1->val.ptr,
                                (class unit_data *)v2->val.ptr) &&
                (!IS_ROOM((class unit_data *)v1->val.ptr) ||
                 IS_ROOM((class unit_data *)v2->val.ptr)))
            {
                unit_from_unit((class unit_data *)v1->val.ptr);
                unit_to_unit((class unit_data *)v1->val.ptr,
                             (class unit_data *)v2->val.ptr);
            }
        }
    delete v1;
    delete v2;
}

/* dilcopy */
void dilfi_dlc(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("dilcopy", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, TYPEFAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr && v2->val.ptr)
            dil_copy((char *)v1->val.ptr, (class unit_data *)v2->val.ptr);
    delete v1;
    delete v2;
}

/* sendtext */
void dilfi_sete(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("sendtext", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, TYPEFAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr && v2->val.ptr && IS_CHAR((class unit_data *)v2->val.ptr))
        {
            if (*((char *)v1->val.ptr) == MULTI_UNIQUE_CHAR &&
                *((char *)v1->val.ptr + 1) == MULTI_PROMPT_CHAR)
            {
                if (CHAR_DESCRIPTOR((unit_data *)v2->val.ptr))
                {
                    protocol_send_text(CHAR_DESCRIPTOR((unit_data *)v2->val.ptr)->multi,
                                       CHAR_DESCRIPTOR((unit_data *)v2->val.ptr)->id,
                                       (((const char *)v1->val.ptr) + 2),
                                       MULTI_PROMPT_CHAR);
                }
            }
            else
            {
                send_to_char((char *)v1->val.ptr,
                             (class unit_data *)v2->val.ptr);
            }
        }
    delete v1;
    delete v2;
}

/* Set one char to follow another unconditionally */
void dilfi_folo(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("follow", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, TYPEFAIL_NULL, 2, DILV_UP, DILV_NULL))
    {
        if (v1->val.ptr && !(v2->val.ptr))
        {
            stop_following((class unit_data *)v1->val.ptr);
        }
        else
        {
            if (v1->val.ptr && v2->val.ptr &&
                IS_CHAR((class unit_data *)v1->val.ptr) &&
                IS_CHAR((class unit_data *)v2->val.ptr))
            {
                if (CHAR_MASTER((class unit_data *)v1->val.ptr))
                    stop_following((class unit_data *)v1->val.ptr);

                start_following((class unit_data *)v1->val.ptr,
                                (class unit_data *)v2->val.ptr);
            }
        }
    }
    delete v1;
    delete v2;
}

/* logcrime */
void dilfi_lcri(register class dilprg *p)
{
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("logcrime", p, 3,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, TYPEFAIL_NULL, 1, DILV_UP,
                       v3, TYPEFAIL_NULL, 1, DILV_INT))
        if (v1->val.ptr && v2->val.ptr &&
            IS_CHAR((class unit_data *)v1->val.ptr) &&
            IS_CHAR((class unit_data *)v2->val.ptr))
        {

            log_crime((class unit_data *)v1->val.ptr,
                      (class unit_data *)v2->val.ptr, v3->val.num);
        }
    delete v1;
    delete v2;
    delete v3;
}

/* Assign EXP to player */
void dilfi_exp(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    sbit32 value = 0;

    p->waitcmd--;

    if (dil_type_check("exp", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_INT,
                       v2, FAIL_NULL, 1, DILV_UP))
    {
        if (dil_getval(v1) == DILV_INT)
            value = v1->val.num;

        if (v2->val.ptr && IS_PC((class unit_data *)v2->val.ptr))
        {
            slog(LOG_ALL, 0, "%s gained %d experience from unit %s@%s.",
                 UNIT_NAME((class unit_data *)v2->val.ptr), value,
                 UNIT_FI_NAME(p->sarg->owner),
                 UNIT_FI_ZONENAME(p->sarg->owner));
            gain_exp((class unit_data *)v2->val.ptr, value);
        }
    }
    delete v1;
    delete v2;
}

/* Branch on expression */
void dilfi_if(register class dilprg *p)
{
    ubit32 coreptr;
    dilval *v1 = p->stack.pop();

    coreptr = bread_ubit32(&(p->fp->pc)); /* else branch */

    p->waitcmd--;
    if (!dil_getbool(v1, p))                       /* might be pointer, but ok! */
        p->fp->pc = &(p->fp->tmpl->core[coreptr]); /* choose else branch */
    delete v1;
}

/* Set bits in integer */
void dilfi_set(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("set", p, 1, v2, TYPEFAIL_NULL, 1, DILV_INT))
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
    delete v1;
    delete v2;
}

/* Unset bits in integer */
void dilfi_uset(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("unset", p, 1, v2, TYPEFAIL_NULL, 1, DILV_INT))
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
    delete v1;
    delete v2;
}

/* Add element to string list (addstring) */
void dilfi_adl(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    switch (v1->type)
    {
    case DILV_SLPR:
        if (dil_type_check("addstring", p, 1, v2, FAIL_NULL, 1, DILV_SP))
            if (v2->val.ptr)
                ((cNamelist *)v1->ref)->dAppendName((char *)v2->val.ptr);
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

void dilfi_inslst(register class dilprg *p)
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
        if (dil_type_check("insert", p, 2,
                           v2, TYPEFAIL_NULL, 1, DILV_INT,
                           v3, TYPEFAIL_NULL, 2, DILV_SP, DILV_INT))
        {
            if (v1->type == DILV_SLPR)
            {
                if (dil_getval(v3) == DILV_INT)
                    ((cNamelist *)v1->ref)->InsertName(itoa(v3->val.num), (ubit32)v2->val.num);
                else if (v3->val.ptr)
                    ((cNamelist *)v1->ref)->InsertName(((char *)v3->val.ptr), (ubit32)v2->val.num);
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
void dilfi_remlst(register class dilprg *p)
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
void dilfi_sul(register class dilprg *p)
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
            if (v2->val.ptr)
                ((cNamelist *)v1->ref)->RemoveName((char *)v2->val.ptr);
        break;
    }
    delete v1;
    delete v2;
}

/* add element to extra description */
void dilfi_ade2(register class dilprg *p)
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
        if (dil_type_check("addextra", p, 3,
                           v2, TYPEFAIL_NULL, 2, DILV_SLP, DILV_NULL,
                           v4, TYPEFAIL_NULL, 1, DILV_ILP,
                           v3, TYPEFAIL_NULL, 1, DILV_SP))
            if (v1->ref && v3->val.ptr && v4->val.ptr &&
                (v2->val.ptr || v2->type == DILV_NULL))
            {
                // MS2020 XXX BUG HERE
                class extra_descr_data *e = new class extra_descr_data((char *)NULL, (char *)v3->val.ptr);
                rogue_push_front(((class extra_descr_data **)v1->ref), e);
                /**((class extra_descr_data **)v1->ref) =
                    (*((class extra_descr_data **)v1->ref))->add((char *)NULL, (char *)v3->val.ptr);*/
                if (v2->type == DILV_NULL)
                    (*((class extra_descr_data **)v1->ref))->names = new cNamelist;
                else
                    (*((class extra_descr_data **)v1->ref))->names.CopyList((cNamelist *)v2->val.ptr);
                (*((class extra_descr_data **)v1->ref))->vals.CopyList((cintlist *)v4->val.ptr);
            }

        break;
    }
    delete v1;
    delete v2;
    delete v3;
    delete v4;
}

/* add element to extra description */
void dilfi_ade(register class dilprg *p)
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
        if (dil_type_check("addextra", p, 2,
                           v2, TYPEFAIL_NULL, 2, DILV_SLP, DILV_NULL,
                           v3, TYPEFAIL_NULL, 1, DILV_SP))
            if (v1->ref && v3->val.ptr && (v2->val.ptr || v2->type == DILV_NULL))
            {
                class extra_descr_data *e = new class extra_descr_data((char *)NULL, (char *)v3->val.ptr);
                rogue_push_front(((class extra_descr_data **)v1->ref), e);

                /* *((class extra_descr_data **)v1->ref) =
                    (*((class extra_descr_data **)v1->ref))->add((char *)NULL, (char *)v3->val.ptr); */
                if (v2->type == DILV_NULL)
                    (*((class extra_descr_data **)v1->ref))->names = new cNamelist;
                else
                    (*((class extra_descr_data **)v1->ref))->names.CopyList((cNamelist *)v2->val.ptr);
            }

        break;
    }
    delete v1;
    delete v2;
    delete v3;
}

/* Substract elemnt from extra description */
void dilfi_sue(register class dilprg *p)
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
            if (v2->val.ptr && v1->ref)
            {
                //MS2020 bug, check if exd is NULL
                if (*((class extra_descr_data **)v1->ref) != NULL)
                {
                    /* *((class extra_descr_data **)v1->ref) =
                        (*((class extra_descr_data **)v1->ref))->remove((char *)v2->val.ptr); */

                    rogue_remove((class extra_descr_data **)v1->ref, (char *)v2->val.ptr);
                }
            }
        break;
    }
    delete v1;
    delete v2;
}

/* destroy unit */
void dilfi_dst(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("purge", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr && !IS_ROOM((class unit_data *)v1->val.ptr))
        {
            if (v1->val.ptr == p->sarg->owner)
            {
                extract_unit((class unit_data *)v1->val.ptr);
                p->waitcmd = WAITCMD_DESTROYED;
            }
            else
            {
                extract_unit((class unit_data *)v1->val.ptr);
                dil_test_secure(p);
            }
            if (IS_PC((class unit_data *)v1->val.ptr))
                if (CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr))
                    descriptor_close(CHAR_DESCRIPTOR((class unit_data *)v1->val.ptr));
        }
    delete v1;
}

/* Execute command */
void dilfi_exec(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("exec", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, TYPEFAIL_NULL, 1, DILV_UP))

        if (v1->val.ptr && v2->val.ptr && IS_CHAR((class unit_data *)v2->val.ptr))
        {
            char cmd[MAX_INPUT_LENGTH + 1];

            strncpy(cmd, (char *)v1->val.ptr, MAX_INPUT_LENGTH);
            cmd[MAX_INPUT_LENGTH] = 0;

            if (strlen((char *)v1->val.ptr) > MAX_INPUT_LENGTH)
            {
                slog(LOG_ALL, 0,
                     "DIL %s@%s issued command which was too long: %s",
                     UNIT_FI_NAME(p->sarg->owner),
                     UNIT_FI_ZONENAME(p->sarg->owner), cmd);
            }

            if (IS_IMMORTAL((class unit_data *)v2->val.ptr))
            {
                char buf[MAX_INPUT_LENGTH];
                struct command_info *cmd_ptr;

                extern struct trie_type *intr_trie;

                str_next_word(cmd, buf);

                if ((cmd_ptr =
                         (struct command_info *)search_trie(buf, intr_trie)))
                {
                    if (cmd_ptr->minimum_level >= IMMORTAL_LEVEL)
                    {
                        slog(LOG_EXTENSIVE, 0, "DIL %s on %s tried "
                                               "to make %s do: %s",
                             p->fp->tmpl->prgname,
                             STR(UNIT_NAME(p->sarg->owner)),
                             UNIT_NAME((class unit_data *)v2->val.ptr), cmd);
                    }
                    else
                    {
                        command_interpreter((class unit_data *)v2->val.ptr,
                                            cmd);
                        if (p && p->frame)
                            dil_test_secure(p);
                    }
                }
                else
                {
                    command_interpreter((class unit_data *)v2->val.ptr, cmd);
                    if (p && p->frame)
                        dil_test_secure(p);
                }
            }
            else
            {
                command_interpreter((class unit_data *)v2->val.ptr, cmd);
                if (p && p->frame)
                    dil_test_secure(p);
            }
        }
    delete v1;
    delete v2;
}

/* Wait */
void dilfi_wit(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    ubit32 coreptr;
    ubit8 *oldpc;
    coreptr = bread_ubit32(&(p->fp->pc));
    oldpc = &(p->fp->tmpl->core[coreptr]);

    if (dil_type_check("wait", p, 1, v1, TYPEFAIL_NULL, 1, DILV_INT))
    {
        if (p->waitcmd != WAITCMD_MAXINST)
        {
            REMOVE_BIT(v1->val.num,
                       ~(SFB_DONE | SFB_PRE | SFB_CMD | SFB_TICK | SFB_RANTIME |
                         SFB_DEAD | SFB_COM | SFB_MSG | SFB_EDIT | SFB_PROMPT));

            REMOVE_BIT(p->sarg->fptr->flags,
                       SFB_CMD | SFB_TICK | SFB_DEAD | SFB_DONE | SFB_PRE |
                           SFB_COM | SFB_MSG | SFB_SAVE | SFB_ACTIVATE | SFB_EDIT |
                           SFB_PROMPT);

            SET_BIT(p->sarg->fptr->flags, v1->val.num);

            p->waitcmd = WAITCMD_FINISH;
            p->fp->pc = oldpc; /* rewind pc to just before wait command */
        }
        else
        {
            if (IS_SET(v1->val.num, p->sarg->mflags) && dil_getbool(v2, p))
                p->waitcmd--;
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
void dilfi_act(register class dilprg *p)
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
                act((char *)v1->val.ptr, v2->val.num, v3, v4, v5, v6->val.num);
            break;

        case TO_VICT:
        case TO_NOTVICT:
            if (v5->val.ptr)
                act((char *)v1->val.ptr, v2->val.num, v3, v4, v5, v6->val.num);
        }
    delete v1;
    delete v2;
    delete v3;
    delete v4;
    delete v5;
    delete v6;
}

/* Goto new command */
void dilfi_goto(register class dilprg *p)
{
    ubit32 adr;

    adr = bread_ubit32(&(p->fp->pc));
    p->fp->pc = &(p->fp->tmpl->core[adr]);
    p->waitcmd--;
}

/* Goto new command */
void dilfi_on(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();
    ubit32 adr;
    ubit16 maxlab;
    ubit8 *brkptr;

    p->waitcmd--;

    maxlab = bread_ubit16(&(p->fp->pc));
    brkptr = p->fp->pc + sizeof(ubit32) * (maxlab);

    if (dil_getval(v1) != DILV_INT)
        p->fp->pc = brkptr;
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
                p->fp->pc = brkptr;
            else
                p->fp->pc = &(p->fp->tmpl->core[adr]);
        }
    }
    delete v1;
}

/* Substract affect from unit */
void dilfi_sua(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    class unit_affected_type *af;

    p->waitcmd--;

    if (dil_type_check("subaff", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, TYPEFAIL_NULL, 1, DILV_INT))
        if (v1->val.ptr)
        {
            af = affected_by_spell((class unit_data *)v1->val.ptr, v2->val.num);
            if (af)
                destroy_affect(af);
        }
    delete v1;
    delete v2;
}

/* Add affect */
void dilfi_ada(register class dilprg *p)
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

    if (dil_type_check("addaff", p, 11,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, TYPEFAIL_NULL, 1, DILV_INT,
                       v3, TYPEFAIL_NULL, 1, DILV_INT,
                       v4, TYPEFAIL_NULL, 1, DILV_INT,
                       v5, TYPEFAIL_NULL, 1, DILV_INT,
                       v6, TYPEFAIL_NULL, 1, DILV_INT,
                       v7, TYPEFAIL_NULL, 1, DILV_INT,
                       v8, TYPEFAIL_NULL, 1, DILV_INT,
                       v9, TYPEFAIL_NULL, 1, DILV_INT,
                       v10, TYPEFAIL_NULL, 1, DILV_INT,
                       v11, TYPEFAIL_NULL, 1, DILV_INT))
        if ((v1->val.ptr) &&
            is_in(v2->val.num, 1, ID_TOP_IDX) &&
            is_in(v8->val.num, TIF_NONE, TIF_MAX) &&
            is_in(v9->val.num, TIF_NONE, TIF_MAX) &&
            is_in(v10->val.num, TIF_NONE, TIF_MAX) &&
            is_in(v11->val.num, APF_NONE, APF_MAX))
        {
            if (p->frame[0].tmpl->zone->access != 0)
            {
                szonelog(p->frame->tmpl->zone,
                         "DIL '%s' attempt to violate system access security (ada).",
                         p->frame->tmpl->prgname);
                p->waitcmd = WAITCMD_QUIT;
            }
            else
            {
                class unit_affected_type af;

                af.id = v2->val.num;
                af.duration = v3->val.num;
                af.beat = v4->val.num;

                af.data[0] = v5->val.num;
                af.data[1] = v6->val.num;
                af.data[2] = v7->val.num;

                af.firstf_i = v8->val.num;
                af.tickf_i = v9->val.num;
                af.lastf_i = v10->val.num;
                af.applyf_i = v11->val.num;
                create_affect((class unit_data *)v1->val.ptr, &af);
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
void dilfi_pri(register class dilprg *p)
{
    p->waitcmd--;
    SET_BIT(p->sarg->fptr->flags, SFB_PRIORITY);
}

/* No Priority */
void dilfi_npr(register class dilprg *p)
{
    REMOVE_BIT(p->sarg->fptr->flags, SFB_PRIORITY);
}

/* Send message to DIL programs in local environment */
void dilfi_snd(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("send", p, 1, v1, TYPEFAIL_NULL, 1, DILV_SP))
        if (v1->val.ptr)
        {
            send_message(p->sarg->owner, (char *)v1->val.ptr);
            dil_test_secure(p);
        }
    delete v1;
}

/* Send message to DIL programs in the specified unit */
void dilfi_snt(register class dilprg *p)
{
    /* sendto (string, uptr) */
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    extern struct command_info cmd_auto_msg;

    p->waitcmd--;

    if (dil_type_check("sendto", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, FAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr && v2->val.ptr)
        {
            struct spec_arg sarg;

            sarg.activator = p->sarg->owner;
            sarg.medium = NULL;
            sarg.target = NULL;
            sarg.pInt = NULL;
            sarg.fptr = NULL; /* Set by unit_function_scan */
            sarg.cmd = &cmd_auto_msg;
            sarg.arg = (char *)v1->val.ptr;
            sarg.mflags = SFB_MSG | SFB_AWARE;

            unit_function_scan((class unit_data *)v2->val.ptr, &sarg);

            dil_test_secure(p);
        }
    delete v1;
    delete v2;
}

void dilfi_snta(register class dilprg *p)
{
    /* Send message to DIL programs in all units of type specified */
    /* sendtoall (string, string) */

    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    extern struct command_info cmd_auto_msg;

    p->waitcmd--;

    if (dil_type_check("sendtoall", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, TYPEFAIL_NULL, 1, DILV_SP))
        if (v1->val.ptr && v2->val.ptr)
        {
            extern class unit_data *unit_list;

            class unit_data *u;
            class file_index_type *fi;

            if ((fi = str_to_file_index((char *)v2->val.ptr)))
            {
                struct spec_arg sarg;

                sarg.activator = p->sarg->owner;
                sarg.medium = p->sarg->owner;
                sarg.target = NULL;
                sarg.pInt = NULL;
                sarg.fptr = NULL; /* Set by unit_function_scan */

                sarg.fptr = NULL;
                sarg.cmd = &cmd_auto_msg;
                sarg.arg = (char *)v1->val.ptr;
                sarg.mflags = SFB_MSG;

                for (u = unit_list; u; u = u->gnext)
                {
                    if (UNIT_FILE_INDEX(u) == fi)
                        unit_function_scan(u, &sarg);
                }
                dil_test_secure(p);
            }
        }
    delete v1;
    delete v2;
}

void dilfi_sntadil(register class dilprg *p)
{
    /* Send message to DIL programs in all units of type specified */
    /* sendtoall (string<message>, string<template> ) */

    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("sendtoalldil", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_SP,
                       v2, TYPEFAIL_NULL, 1, DILV_SP))
        if (v1->val.ptr && !str_is_empty((char *)v2->val.ptr))
        {
            struct diltemplate *tmpl;

            tmpl = find_dil_template((const char *)v2->val.ptr);

            if (tmpl)
            {
                class dilprg *tp;

                for (tp = dil_list; tp; tp = dil_list_nextdude)
                {
                    dil_list_nextdude = tp->next;

                    if (tp->fp && tp->fp->tmpl == tmpl && tp != p)
                    {
                        class unit_fptr *fptr;

                        /* If it is destructed, then it cant be found because data
                           will be null */

                        for (fptr = UNIT_FUNC(tp->owner); fptr;
                             fptr = fptr->next)
                            if (fptr->index == SFUN_DIL_INTERNAL && fptr->data && ((class dilprg *)fptr->data)->fp && ((class dilprg *)fptr->data)->fp->tmpl == tmpl)
                                break;

                        if (!fptr)
                            continue;

                        struct spec_arg sarg;

                        sarg.owner = tp->owner;
                        sarg.activator = p->owner;
                        sarg.medium = p->owner;
                        sarg.target = tp->owner;
                        sarg.pInt = NULL;
                        sarg.fptr = fptr;

                        sarg.cmd = &cmd_auto_msg;
                        sarg.arg = (char *)v1->val.ptr;
                        sarg.mflags = SFB_MSG;

                        function_activate(tp->owner, &sarg);
                    }
                } /* for */
                dil_test_secure(p);
            }
        }
    delete v1;
    delete v2;
}

// DIL log string
void dilfi_log(register class dilprg *p)
{
    /* Log a string to the logs. */

    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("log", p, 1, v1, TYPEFAIL_NULL, 1, DILV_SP))
        if (v1->val.ptr)
            szonelog(UNIT_FI_ZONE(p->owner), "%s", (char *)v1->val.ptr);
    delete v1;
}

/* Secure */
void dilfi_sec(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();
    ubit32 adr; /* address */

    adr = bread_ubit32(&(p->fp->pc));

    p->waitcmd--;

    if (dil_type_check("secure", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr)
        {
            dil_sub_secure(p->fp, (class unit_data *)v1->val.ptr);
            dil_add_secure(p, (class unit_data *)v1->val.ptr,
                           &(p->fp->tmpl->core[adr]));
        }
    delete v1;
}

/* Unsecure */
void dilfi_use(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("unsecure", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr)
            dil_sub_secure(p->fp, (class unit_data *)v1->val.ptr);
    delete v1;
}

void dilfi_popstk(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();
    delete v1;
}

/* Equip unit in inventory of PC/NPC */
void dilfi_eqp(register class dilprg *p)
{
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("equip", p, 2,
                       v1, TYPEFAIL_NULL, 1, DILV_UP,
                       v2, TYPEFAIL_NULL, 1, DILV_INT))
        if (v1->val.ptr && UNIT_IN((class unit_data *)v1->val.ptr) &&
            IS_CHAR(UNIT_IN((class unit_data *)v1->val.ptr)) &&
            IS_OBJ((class unit_data *)v1->val.ptr) &&
            !equipment(UNIT_IN((class unit_data *)v1->val.ptr), v2->val.num))
        {
            /* Then equip char */
            equip_char(UNIT_IN((class unit_data *)v1->val.ptr),
                       (class unit_data *)v1->val.ptr, v2->val.num);
        }
    delete v1;
    delete v2;
}

/* Unequip unit in inventory of PC/NPC */
void dilfi_ueq(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();

    p->waitcmd--;

    if (dil_type_check("unequip", p, 1, v1, FAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr && IS_OBJ((class unit_data *)v1->val.ptr) &&
            OBJ_EQP_POS((class unit_data *)v1->val.ptr))
            unequip_object((class unit_data *)v1->val.ptr);
    delete v1;
}

void dilfi_quit(register class dilprg *p)
{
    p->waitcmd = WAITCMD_QUIT;
}

/* Block */
void dilfi_blk(register class dilprg *p)
{
    p->waitcmd--;
    SET_BIT(p->flags, DILFL_CMDBLOCK);
}

void dilfi_pup(register class dilprg *p)
{
    dilval *v1 = p->stack.pop();

    void update_pos(class unit_data * victim);
    void die(class unit_data * ch);

    p->waitcmd--;

    if (dil_type_check("updatepos", p, 1, v1, TYPEFAIL_NULL, 1, DILV_UP))
        if (v1->val.ptr && IS_CHAR((class unit_data *)v1->val.ptr))
        {
            update_pos((class unit_data *)v1->val.ptr);
            if (CHAR_POS((class unit_data *)v1->val.ptr) == POSITION_DEAD)
            {
                die((class unit_data *)v1->val.ptr);
                dil_test_secure(p);
            }
        }
    delete v1;
}

void dilfi_cast(register class dilprg *p)
{
    dilval *v4 = p->stack.pop();
    dilval *v3 = p->stack.pop();
    dilval *v2 = p->stack.pop();
    dilval *v1 = p->stack.pop();
    class unit_data *caster = NULL, *medium = NULL, *target = NULL;
    extern struct spell_info_type spell_info[SPL_TREE_MAX];

    p->waitcmd--;

    if (dil_type_check("cast", p, 4,
                       v1, TYPEFAIL_NULL, 1, DILV_INT,
                       v2, TYPEFAIL_NULL, 1, DILV_UP,
                       v3, TYPEFAIL_NULL, 1, DILV_UP,
                       v4, TYPEFAIL_NULL, 1, DILV_UP))
    {
        caster = (class unit_data *)v2->val.ptr;
        medium = (class unit_data *)v3->val.ptr;
        target = (class unit_data *)v4->val.ptr;

        if (is_in(v1->val.num, SPL_GROUP_MAX, SPL_TREE_MAX - 1) &&
            caster && IS_CHAR(caster) && medium &&
            (spell_info[v1->val.num].spell_pointer || spell_info[v1->val.num].tmpl))
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

class unit_data *hometown_unit(char *str)
{
    extern class unit_data *entry_room;

    if (str)
    {
        char name[80], zone[80];
        class unit_data *u;

        split_fi_ref(str, zone, name);
        if ((u = world_room(zone, name)))
            return u;
    }

    return entry_room;
}
