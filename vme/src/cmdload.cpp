/*
 $Author: All $
 $RCSfile: cmdload.cpp,v $
 $Date: 2003/12/28 22:02:44 $
 $Revision: 2.9 $
*/

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmdload.h"
#include "structs.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "utils.h"
#include "vmelimits.h"
#include "skills.h"
#include "trie.h"
#include "utility.h"
#include "files.h"
#include "common.h"
#include "constants.h"
#include "unitfind.h"
#include "dilrun.h"

struct trie_type *cmd_trie = NULL;
struct command_info *cmdlist = NULL;
struct cmdload_struct cmdload[] = {
    {"north", do_move, 0, 0},
    {"northeast", do_move, 0, 0},
    {"ne", do_move, 0, 0},
    {"northwest", do_move, 0, 0},
    {"nw", do_move, 0, 0},
    {"east", do_move, 0, 0},
    {"south", do_move, 0, 0},
    {"southeast", do_move, 0, 0},
    {"se", do_move, 0, 0},
    {"west", do_move, 0, 0},
    {"southwest", do_move, 0, 0},
    {"sw", do_move, 0, 0},
    {"up", do_move, 0, 0},
    {"down", do_move, 0, 0},

    {"account", do_account, 0, 0},
    {"at", do_at, 0, 0},

    {"backstab", do_backstab, 0, 0},
    {"ban", do_ban, 0, 0},

    {"cast", do_cast, 0, 0},
    {"change", do_change, 0, 0},
    {"color", do_color, 0, 0},
    {"consider", do_consider, 0, 0},
    {"crash", do_crash, 0, 0},

    {"execute", do_execute, 0, 0},

    {"kill", do_kill, 0, 0},

    {"level", do_level, 0, 0},
    {"load", do_load, 0, 0},
    {"timewarp", do_timewarp, 0, 0},

    {"rent", do_rent, 0, 0},
    {"reset", do_reset, 0, 0},
    {"save", do_save, 0, 0},
    {"set", do_set, 0, 0},
    {"setskill", do_setskill, 0, 0},
    {"shutdown", do_shutdown, 0, 0},
    {"snoop", do_snoop, 0, 0},
    {"switch", do_switch, 0, 0},

    {"users", do_users, 0, 0},
    {"where", do_where, 0, 0},

    {"wizlock", do_wizlock, 0, 0},
    {"wstat", do_wstat, 0, 0},
    {"", NULL, 0, 0}};


void skill_dump(void)
{
    string str;
    char buf[MAX_STRING_LENGTH];

    bool pairISCompare(const std::pair<int, string>& firstElem, const std::pair<int, string>& secondElem);

    for (int j = 0; j < PROFESSION_MAX; j++)
    {
        vector< pair <int,string> > vect;

        for (int i = 0; i < SKI_TREE_MAX; i++)
        {
            if (ski_text[i] == NULL)
                continue;

            str = "";

            sprintf(buf, "%s,%s", ski_text[i], spc(20-strlen(ski_text[i])));
            str.append(buf);

            sprintf(buf, ".profession %s%s = %s%d\n", professions[j], spc(12-strlen(professions[j])),
                (skill_prof_table[i].profession_cost[j] >= 0) ? "+" : "", skill_prof_table[i].profession_cost[j]);
            str.append(buf);


            /*if (skill_prof_table[i].min_level > 0)
            {
                sprintf(buf, "restrict level          = %d\n", skill_prof_table[i].min_level);
                str.append(buf);
            }

            for (int k=0; k < ABIL_TREE_MAX; k++)
                if (skill_prof_table[i].min_abil[k] > 0)
                {
                    sprintf(buf, "restrict %s%s    = %s%d\n", ski_text[i], spc(12-strlen(ski_text[i])),
                        (skill_prof_table[i].min_abil[k] >= 0) ? "+" : "", skill_prof_table[i].min_abil[k]);
                    str.append(buf);
                }*/

            vect.push_back(std::make_pair(skill_prof_table[i].profession_cost[j], str));
        }

        std::sort(vect.begin(), vect.end(), pairISCompare);
        for (auto it = vect.begin(); it != vect.end(); ++it)
            printf("%s", it->second.c_str());
    }
        
    exit(0);
}

void cmd_base_load(void)
{
    int i;

    cmd_trie = 0;

    for (i = 0; *cmdload[i].cmd_str; i++)
        cmd_trie = add_trienode(cmdload[i].cmd_str, cmd_trie);

    qsort_triedata(cmd_trie);

    for (i = 0; *cmdload[i].cmd_str; i++)
        set_triedata(cmdload[i].cmd_str, cmd_trie, &cmdload[i], TRUE);


    command_read();
    //skill_dump();
}

void command_read(void)
{
    int dummy;
    int idx = -1;
    int ignore = FALSE;
    char pTmp[256];
    char *pCh;
    FILE *fl;
    struct command_info *cmdptr = NULL, *lastptr = NULL;
    struct cmdload_struct *intcmd;

    touch_file(str_cc(g_cServerConfig.m_etcdir, COMMAND_DEFS));
    if (!(fl = fopen(str_cc(g_cServerConfig.m_etcdir, COMMAND_DEFS), "rb")))
    {
        slog(LOG_ALL, 0, "FATAL: Unable to open etc/ " COMMAND_DEFS);
        exit(0);
    }
    slog(LOG_ALL, 0, "Booting Commands from: " COMMAND_DEFS);

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);
        if (mstmp == NULL)
            continue;

        str_remspc(pTmp);

        if ((pCh = strchr(pTmp, '=')))
        {
            *pCh = 0;
            pCh = skip_blanks(pCh + 1);
            strip_trailing_blanks(pCh);
        }

        str_lower(pTmp);
        strip_trailing_blanks(pTmp);

        if (pCh == NULL || str_is_empty(pCh))
            continue;

        if (strncmp(pTmp, "command", 5) == 0)
        {
            if (cmdptr)
            {
                if (cmdptr->cmd_str && (cmdptr->cmd_fptr || cmdptr->tmpl))
                {
                    if (cmdlist == NULL)
                    {
                        cmdlist = cmdptr;
                        cmdptr->prev = NULL;
                        cmdptr->next = NULL;
                        lastptr = cmdptr;
                        cmdptr = NULL;
                    }
                    else
                    {
                        lastptr->next = cmdptr;
                        cmdptr->prev = lastptr;
                        cmdptr->next = NULL;
                        lastptr = cmdptr;
                        cmdptr = NULL;
                    }
                }
            }

            CREATE(cmdptr, struct command_info, 1);
            cmdptr->next = NULL;
            cmdptr->prev = NULL;
            cmdptr->minimum_level = 0;
            cmdptr->minimum_position = POSITION_DEAD;
            cmdptr->log_level = 0;
            cmdptr->tmpl = NULL;
            cmdptr->cmd_fptr = NULL;
            cmdptr->type = 0;
            cmdptr->no = 0;
            cmdptr->inttype = 0;
            cmdptr->dir = 0;
            cmdptr->cmd_str = NULL;
            cmdptr->cmd_str = str_dup(pCh);
            ignore = FALSE;
            continue;
        }
        if (ignore)
            continue;

        if (strncmp(pTmp, "internal", 4) == 0)
        {
            if (
                (intcmd =
                     ((struct cmdload_struct *)search_trie(pCh, cmd_trie))))
            {
                if (cmdptr)
                {
                    cmdptr->cmd_fptr = intcmd->cmd_fptr;
                    cmdptr->inttype = intcmd->inttype;
                    cmdptr->dir = intcmd->dir;
                }
            }
            else
            {
                if (cmdptr->cmd_str)
                    FREE(cmdptr->cmd_str)
                    cmdptr = NULL;
                slog(LOG_ALL, 0,
                     "COMMAND LOAD ERROR: %s not a defined internal funciton.",
                     pCh);
                ignore = TRUE;
            }
            continue;
        }

        if (strncmp(pTmp, "index", 8) == 0)
        {
            idx = atoi(pCh);
            if (!str_is_number(pCh) || !is_in(idx, 0, SKI_TREE_MAX - 1))
            {
                slog(LOG_ALL, 0, "Skill boot error: %s", pCh);
                idx = -1;
            }
            continue;
        }

        if (strncmp(pTmp, "name", 4) == 0)
        {
            if (ski_text[idx])
            {
                free((char *)ski_text[idx]);
                ski_text[idx] = NULL;
            }
            ski_text[idx] = str_dup(pCh);
            continue;
        }

        if (strncmp(pTmp, "race ", 5) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -3, +3))
                continue;

            int ridx = search_block(pTmp + 5, pc_races, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Skills: Illegal race in: %s", pTmp);
            else
                racial_skills[ridx][idx] = dummy;
            continue;
        }

        if (strncmp(pTmp, "profession ", 11) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -9, +5))
            {
                slog(LOG_ALL, 0, "Skills: profession modifier %d for %s not in [-9..+5]", dummy, pTmp);
                continue;
            }

            int ridx = search_block(pTmp + 11, professions, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Skills: Illegal profession %s", pTmp);
            else
                skill_prof_table[idx].profession_cost[ridx] = dummy;
            continue;
        }
        
        if (strncmp(pTmp, "restrict ", 9) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, 0, 250))
            {
                slog(LOG_ALL, 0, "Skills: restrict modifier %d for %s not in [0..250]", dummy, pTmp);
                continue;
            }

            if (strncmp(pTmp+9, "level", 5) == 0)
            {
                skill_prof_table[idx].min_level = dummy;
            }
            else
            {
                int ridx = search_block(pTmp + 9, abil_text, TRUE);

                if (ridx == -1)
                    slog(LOG_ALL, 0, "Weapons: Illegal restrict %s", pTmp);
                else
                    skill_prof_table[idx].min_abil[ridx] = dummy;
            }
            continue;
        }

        if (strncmp(pTmp, "turns", 5) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, 0, 4 * PULSE_VIOLENCE))
            {
                cmdptr->combat_speed = dummy;
                cmdptr->combat_buffer = TRUE;
            }
            continue;
        }

        if (strncmp(pTmp, "minpos", 6) == 0)
        {
            if (cmdptr)
            {
                dummy = atoi(pCh);
                if (is_in(dummy, POSITION_DEAD, POSITION_STANDING))
                    cmdptr->minimum_position = dummy;
            }
            continue;
        }

        if (strncmp(pTmp, "minlevel", 8) == 0)
        {
            if (cmdptr)
            {
                dummy = atoi(pCh);
                cmdptr->minimum_level = dummy;
            }
            continue;
        }

        if (strncmp(pTmp, "loglevel", 8) == 0)
        {
            if (cmdptr)
            {
                dummy = atoi(pCh);
                cmdptr->log_level = dummy;
            }
            continue;
        }
        if (strncmp(pTmp, "type", 4) == 0)
        {
            if (cmdptr)
            {
                dummy = atoi(pCh);
                cmdptr->type = dummy;
            }
            continue;
        }

        if (strncmp(pTmp, "func", 4) == 0)
        {
            if (cmdptr)
            {
                if (cmdptr->tmpl)
                    FREE(cmdptr->tmpl);

                if (!(cmdptr->tmpl = find_dil_template(pCh)))
                    slog(LOG_ALL, 0, "COMMAND LOAD WARNING: No such DIL template %s.", pCh);
            }
            continue;
        }
        if (*pTmp)
            slog(LOG_ALL, 0, "COMMAND LOAD ERROR: Unexpected text: %s", pTmp);
    }

    if (cmdptr)
    {
        if (cmdptr->cmd_str && (cmdptr->cmd_fptr || cmdptr->tmpl))
        {
            if (cmdlist == NULL)
            {
                cmdlist = cmdptr;
                cmdptr->prev = NULL;
                cmdptr->next = NULL;
                lastptr = cmdptr;
                cmdptr = NULL;
            }
            else
            {
                lastptr->next = cmdptr;
                cmdptr->prev = lastptr;
                cmdptr->next = NULL;
                lastptr = cmdptr;
                cmdptr = NULL;
            }
        }
    }

    fclose(fl);
}
