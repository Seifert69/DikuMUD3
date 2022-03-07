/*
 $Author: All $
 $RCSfile: zon_basis.cpp,v $
 $Date: 2003/12/28 22:02:45 $
 $Revision: 2.5 $
 */
#include "comm.h"
#include "db.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "slog.h"
#include "structs.h"
#include "textutil.h"
#include "unit_fptr.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define DEFAULT_ENTRY_ZONE "udgaard"
#define DEFAULT_ENTRY_NAME "temple"

#define PC_DEATHLOC_NAME "heaven_entry"
#define PC_PREDEATHLOC_NAME "deathseq_room"
#define PC_TIMEOUTLOC_NAME "timeout"
#define VOID_ROOM "void"
#define ZOMBIE_NAME "zombie"
#define MAIL_NOTE_NAME "letter"
#define DESTROY_ROOM "destroy_room"

unit_data *g_void_room = nullptr;
unit_data *g_destroy_room = nullptr;
unit_data *heaven_room = nullptr;
unit_data *seq_room = nullptr;
unit_data *time_room = nullptr;
unit_data *g_entry_room = nullptr;

file_index_type *g_letter_fi = nullptr;

void basis_boot()
{
    g_void_room = world_room(BASIS_ZONE, VOID_ROOM);
    assert(g_void_room);

    g_destroy_room = world_room(BASIS_ZONE, DESTROY_ROOM);
    assert(g_destroy_room);

    heaven_room = world_room(BASIS_ZONE, PC_DEATHLOC_NAME);
    assert(heaven_room);

    seq_room = world_room(BASIS_ZONE, PC_PREDEATHLOC_NAME);
    assert(seq_room);

    time_room = world_room(BASIS_ZONE, PC_TIMEOUTLOC_NAME);
    assert(time_room);

    g_letter_fi = find_file_index(BASIS_ZONE, MAIL_NOTE_NAME);
    assert(g_letter_fi);

    g_entry_room = world_room(DEFAULT_ENTRY_ZONE, DEFAULT_ENTRY_NAME);
    if (g_entry_room == nullptr)
    {
        slog(LOG_ALL, 0, "Entry room does not exist, using void.");
        g_entry_room = g_void_room;
    }
}

int error_rod(spec_arg *sarg)
{
    if ((!is_command(sarg->cmd, "use")) || (!IS_PC(sarg->activator)) || (OBJ_EQP_POS(sarg->owner) != WEAR_HOLD))
    {
        return SFR_SHARE;
    }

    zone_type *zone = unit_zone(sarg->activator);

    if (!IS_ADMINISTRATOR(sarg->activator) && !zone->getCreators().IsName(UNIT_NAME(sarg->activator)))
    {
        send_to_char("You are only allowed to erase errors "
                     "in your own zone.<br/>",
                     sarg->activator);
        return SFR_BLOCK;
    }

    std::string filename = g_cServerConfig.getZoneDir() + zone->getFilename() + ".err";

    FILE *fl = fopen(filename.c_str(), "w");
    if (!fl)
    {
        slog(LOG_ALL, 0, "Could not clear the zone error-file");
        send_to_char("Could not clear the zone error-file.<br/>", sarg->activator);
        return SFR_BLOCK;
    }
    fclose(fl);

    act("$1n uses $2n.", A_HIDEINV, sarg->activator, sarg->owner, cActParameter(), TO_ROOM);
    send_to_char("Error file was erased.<br/>", sarg->activator);
    slog(LOG_ALL, UNIT_MINV(sarg->activator), "%s cleared %s", UNIT_NAME(sarg->activator), filename);
    return SFR_BLOCK;
}

int info_rod(spec_arg *sarg)
{
    if (!is_command(sarg->cmd, "wave") || !IS_PC(sarg->activator) || OBJ_EQP_POS(sarg->owner) != WEAR_HOLD)
    {
        return SFR_SHARE;
    }

    zone_type *zone = unit_zone(sarg->activator);

    if (!IS_ADMINISTRATOR(sarg->activator) && !zone->getCreators().IsName(UNIT_NAME(sarg->activator)))
    {
        send_to_char("You are only allowed to erase user-information"
                     " in your own zone.",
                     sarg->activator);
        return SFR_BLOCK;
    }

    std::string filename = g_cServerConfig.getZoneDir() + zone->getFilename() + ".inf";

    FILE *fl = fopen(filename.c_str(), "w");
    if (fl)
    {
        slog(LOG_ALL, 0, "Could not clear the zone user info-file");
        send_to_char("Could not clear the zone user info-file.<br/>", sarg->activator);
        return SFR_BLOCK;
    }
    fclose(fl);

    act("$1n uses $2n.", A_HIDEINV, sarg->activator, sarg->owner, cActParameter(), TO_ROOM);
    send_to_char("Zone user information file was erased.<br/>", sarg->activator);
    slog(LOG_ALL, UNIT_MINV(sarg->activator), "%s cleared %s", UNIT_NAME(sarg->activator), filename);
    return SFR_BLOCK;
}

/* Log stuff below */

int log_object(spec_arg *sarg)
{
    ubit8 *ip = nullptr;
    log_level lev = LOG_OFF;
    char c = 0;
    unit_data *ch = UNIT_IN(sarg->owner);

    if (sarg->fptr->getData() == nullptr)
    {
        CREATE(ip, ubit8, 1);
        *ip = 0;

        OBJ_VALUE(sarg->owner, 0) = 'a';
        sarg->fptr->setData(ip);
    }
    else
    {
        ip = (ubit8 *)sarg->fptr->getData();
    }

    c = OBJ_VALUE(sarg->owner, 0);

    switch (sarg->cmd->no)
    {
        case CMD_AUTO_EXTRACT:
            FREE(ip);
            sarg->fptr->setData(nullptr);
            return SFR_SHARE;

        case CMD_AUTO_TICK:
            switch (c)
            {
                case 'o':
                    lev = LOG_OFF;
                    break;
                case 'b':
                    lev = LOG_BRIEF;
                    break;
                case 'e':
                    lev = LOG_EXTENSIVE;
                    break;
                case 'a':
                    lev = LOG_ALL;
                    break;
                case 'd':
                    lev = LOG_DIL;
                    break;
            }

            if (LOG_OFF < lev && IS_PC(ch) && PC_IMMORTAL(ch))
            {
                while (!g_log_buf[*ip].getString().empty())
                {
                    if (g_log_buf[*ip].getLevel() <= lev && g_log_buf[*ip].getWizInvLevel() <= CHAR_LEVEL(ch))
                    {
                        cact("(LOG: $2t)", A_ALWAYS, ch, g_log_buf[*ip].getString().c_str(), cActParameter(), TO_CHAR, "log");
                    }
                    *ip = ((*ip + 1) % MAXLOG);
                }
                return SFR_BLOCK;
            }
            return SFR_SHARE;

        default:
            if (sarg->cmd->cmd_str && sarg->activator == UNIT_IN(sarg->owner) && !strcmp("log", sarg->cmd->cmd_str))
            {
                sarg->arg = skip_spaces(sarg->arg);
                if (is_abbrev(sarg->arg, "all"))
                {
                    c = 'a';
                }
                else if (is_abbrev(sarg->arg, "extensive"))
                {
                    c = 'e';
                }
                else if (is_abbrev(sarg->arg, "dil"))
                {
                    c = 'd';
                }
                else if (is_abbrev(sarg->arg, "brief"))
                {
                    c = 'b';
                }
                else if (is_abbrev(sarg->arg, "off"))
                {
                    cact("Ok, log is now off.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR, "log");
                    OBJ_VALUE(sarg->owner, 0) = 'o';
                    return SFR_BLOCK;
                }
                else if (is_abbrev(sarg->arg, "help"))
                {
                    act("Possible settings are:<br/> off, brief, extensive, all, dil.",
                        A_ALWAYS,
                        ch,
                        cActParameter(),
                        cActParameter(),
                        TO_CHAR);
                    return SFR_BLOCK;
                }
                else
                {
                    act("Current log level is `$2t'.",
                        A_ALWAYS,
                        ch,
                        c == 'd'   ? "dil"
                        : c == 'o' ? "off"
                        : c == 'b' ? "brief"
                        : c == 'a' ? "all"
                                   : "extensive",
                        cActParameter(),
                        TO_CHAR);
                    return SFR_BLOCK;
                }

                act("You will now see the $2t log.",
                    A_ALWAYS,
                    ch,
                    c == 'd'   ? "dil"
                    : c == 'b' ? "brief"
                    : c == 'a' ? "entire"
                               : "extensive",
                    cActParameter(),
                    TO_CHAR);
                OBJ_VALUE(sarg->owner, 0) = c;
                return SFR_BLOCK;
            }

            return SFR_SHARE;
    }
}

/* Return TRUE if ok, FALSE if not */
int system_check(unit_data *pc, const char *buf)
{
    /* Check for `` and ; in system-string */
    if (strchr(buf, '`') || strchr(buf, ';'))
    {
        send_to_char("You can not use the ' and ; characters<br/>", pc);
        slog(LOG_ALL, 0, "%s may have tried to break security with %s", UNIT_NAME(pc), buf);
        return FALSE;
    }

    return TRUE;
}

void execute_append(unit_data *pc, const char *str)
{
    FILE *f = nullptr;

    f = fopen(g_cServerConfig.getFileInLibDir(EXECUTE_FILE).c_str(), "ab+");

    if (f == nullptr)
    {
        slog(LOG_ALL, 0, "Error appending to execute file.");
        return;
    }

    fprintf(f, "%s\n", str);

    slog(LOG_ALL, UNIT_MINV(pc), "EXECUTE(%s): %s", UNIT_NAME(pc), str);

    fclose(f);
}

int admin_obj(spec_arg *sarg)
{
    int zonelist = 0;
    zone_type *zone = nullptr;
    extra_descr_data *exdp = nullptr;

    if (sarg->cmd->no != CMD_AUTO_UNKNOWN)
    {
        return SFR_SHARE;
    }

    if (!IS_PC(sarg->activator))
    {
        return SFR_SHARE;
    }

    if (str_ccmp(sarg->cmd->cmd_str, "email") == 0)
    {
        zonelist = FALSE;
    }
    else if (str_ccmp(sarg->cmd->cmd_str, "zonelist") == 0)
    {
        if (!IS_ADMINISTRATOR(sarg->activator))
        {
            send_to_char("Only administrators can use this function.<br/>", sarg->activator);
            return SFR_BLOCK;
        }
        zonelist = TRUE;
    }
    else
    {
        return SFR_SHARE;
    }

    if ((exdp = PC_INFO(sarg->activator).find_raw("$email")) == nullptr)
    {
        send_to_char("You do not have an email address registered.<br/>", sarg->activator);
        return SFR_BLOCK;
    }

    if (str_is_empty(exdp->descr.c_str()))
    {
        send_to_char("Your email is incorrectly registered.<br/>", sarg->activator);
        return SFR_BLOCK;
    }

    std::string msg;
    if (zonelist)
    {
        msg = diku::format_to_str("mail zone zonelist %s", exdp->descr.c_str());
    }
    else if ((zone = unit_zone(sarg->activator)) == nullptr)
    {
        send_to_char("You are inside no zone?", sarg->activator);
        return SFR_BLOCK;
    }
    else
    {
        if ((!zone->getCreators().IsName(UNIT_NAME(sarg->activator))) && (!IS_OVERSEER(sarg->activator)))
        {
            send_to_char("Only overseers can use this function.<br/>", sarg->activator);
            return SFR_BLOCK;
        }
        msg = diku::format_to_str("mail zone %s %s", zone->getFilename(), exdp->descr.c_str());
    }

    if (!system_check(sarg->activator, msg.c_str()))
    {
        return SFR_BLOCK;
    }

    execute_append(sarg->activator, msg.c_str());

    msg += "<br/>";
    send_to_char(msg, sarg->activator);

    return SFR_BLOCK;
}
