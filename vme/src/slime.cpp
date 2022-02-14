/*
 $Author: All $
 $RCSfile: slime.cpp,v $
 $Date: 2003/12/28 22:02:45 $
 $Revision: 2.2 $
 */

#include "comm.h"
#include "db.h"
#include "files.h"
#include "formatter.h"
#include "interpreter.h"
#include "slog.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static int slime_count = 0;
file_index_type **slime_list = nullptr;

static void slime_save()
{
    int i = 0;
    FILE *f = nullptr;

    if (!(f = fopen(g_cServerConfig.getFileInLibDir(SLIME_FILE).c_str(), "wb")))
    {
        slog(LOG_ALL, 0, "Slime file could not be opened.");
        assert(FALSE);
    }

    for (i = 0; i < slime_count; i++)
    {
        fputs(slime_list[i]->getZone()->getName(), f);
        fputc(0, f);
        fputs(slime_list[i]->getName(), f);
        fputc(0, f);
    }
    fclose(f);
}

static void slime_add(file_index_type *sp)
{
    if (sp == nullptr)
    {
        return;
    }

    if (slime_count++ == 0)
    {
        CREATE(slime_list, file_index_type *, slime_count);
    }
    else
    {
        RECREATE(slime_list, file_index_type *, slime_count);
    }
    slime_list[slime_count - 1] = sp;
}

static void slime_remove(file_index_type *sp)
{
    int i = 0;

    for (i = 0; i < slime_count; i++)
    {
        if (slime_list[i] == sp)
        {
            slime_list[i] = slime_list[slime_count - 1];
            slime_count--;
            if (slime_count == 0)
            {
                FREE(slime_list);
                slime_list = nullptr;
            }
            break;
        }
    }
}

int is_slimed(file_index_type *sp)
{
    int i = 0;

    for (i = 0; i < slime_count; i++)
    {
        if (slime_list[i] == sp)
        {
            return TRUE;
        }
    }

    return FALSE;
}

int slime_obj(spec_arg *sarg)
{
    char buf[MAX_INPUT_LENGTH];
    char fi_name[MAX_INPUT_LENGTH];
    file_index_type *fi = nullptr;

    if (!is_command(sarg->cmd, "slime"))
    {
        return SFR_SHARE;
    }

    if (!IS_OVERSEER(sarg->activator))
    {
        send_to_char("Only overseers can use this function.<br/>", sarg->activator);
        return SFR_BLOCK;
    }

    sarg->arg = one_argument(sarg->arg, buf);

    if (is_abbrev(buf, "list"))
    {
        int i = 0;
        send_to_char("List of slimed units:<br/>", sarg->activator);
        for (i = 0; i < slime_count; i++)
        {
            auto msg = diku::format_to_str("%s@%s<br/>", slime_list[i]->getName(), slime_list[i]->getZone()->getName());
            send_to_char(msg, sarg->activator);
        }

        return SFR_BLOCK;
    }

    sarg->arg = one_argument(sarg->arg, fi_name);

    fi = str_to_file_index(fi_name);

    if (fi == nullptr)
    {
        act("No such file index '$2t'.", A_ALWAYS, sarg->activator, fi_name, cActParameter(), TO_CHAR);
        return SFR_BLOCK;
    }

    if (fi->getZone() == find_zone(BASIS_ZONE))
    {
        act("Basis zone is not allowed slimed.", A_ALWAYS, sarg->activator, cActParameter(), cActParameter(), TO_CHAR);
        return SFR_BLOCK;
    }

    if (is_abbrev(buf, "add"))
    {
        if (is_slimed(fi))
        {
            send_to_char("Already slimed.<br/>", sarg->activator);
        }
        else
        {
            slime_add(fi);
            slime_save();
            send_to_char("Added.<br/>", sarg->activator);
        }
    }
    else if (is_abbrev(buf, "remove"))
    {
        if (!is_slimed(fi))
        {
            send_to_char("No such file index is slimed!<br/>", sarg->activator);
        }
        else
        {
            slime_remove(fi);
            slime_save();
            send_to_char("Removed.<br/>", sarg->activator);
        }
    }
    else
    {
        act("Please specify 'add' or 'remove'.", A_ALWAYS, sarg->activator, cActParameter(), cActParameter(), TO_CHAR);
    }

    return SFR_BLOCK;
}

void slime_boot()
{
    file_index_type *fi = nullptr;
    CByteBuffer cBuf(100);
    char buf1[256];
    char buf2[256];
    FILE *f = nullptr;

    touch_file(g_cServerConfig.getFileInLibDir(SLIME_FILE));
    if (!(f = fopen(g_cServerConfig.getFileInLibDir(SLIME_FILE).c_str(), "rb")))
    {
        slog(LOG_ALL, 0, "Slime file could not be opened.");
        assert(FALSE);
    }

    while (!feof(f))
    {
        fstrcpy(&cBuf, f);
        strcpy(buf1, (char *)cBuf.GetData());

        fstrcpy(&cBuf, f);
        strcpy(buf2, (char *)cBuf.GetData());

        fi = find_file_index(buf1, buf2);
        slime_add(fi);
    }
    fclose(f);
}
