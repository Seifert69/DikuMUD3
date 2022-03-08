/*
 $Author: All $
 $RCSfile: cmdload.cpp,v $
 $Date: 2003/12/28 22:02:44 $
 $Revision: 2.9 $
*/

#include "cmdload.h"

#include "account.h"
#include "act_change.h"
#include "act_color.h"
#include "act_info.h"
#include "act_movement.h"
#include "act_offensive.h"
#include "act_other.h"
#include "act_skills.h"
#include "act_wizard.h"
#include "act_wstat.h"
#include "ban.h"
#include "db.h"
#include "experience.h"
#include "files.h"
#include "formatter.h"
#include "interpreter.h"
#include "main_functions.h"
#include "modify.h"
#include "reception.h"
#include "skills.h"
#include "slog.h"
#include "spell_parser.h"
#include "teach.h"
#include "textutil.h"
#include "trie.h"
#include "vmelimits.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

trie_type *cmd_trie = nullptr;
command_info *g_cmdlist = nullptr;
cmdload_struct cmdload[] = {{"north", do_move, 0, 0},        {"northeast", do_move, 0, 0},
                            {"ne", do_move, 0, 0},           {"northwest", do_move, 0, 0},
                            {"nw", do_move, 0, 0},           {"east", do_move, 0, 0},
                            {"south", do_move, 0, 0},        {"southeast", do_move, 0, 0},
                            {"se", do_move, 0, 0},           {"west", do_move, 0, 0},
                            {"southwest", do_move, 0, 0},    {"sw", do_move, 0, 0},
                            {"up", do_move, 0, 0},           {"down", do_move, 0, 0},

                            {"account", do_account, 0, 0},   {"at", do_at, 0, 0},

                            {"backstab", do_backstab, 0, 0}, {"ban", do_ban, 0, 0},

                            {"cast", do_cast, 0, 0},         {"change", do_change, 0, 0},
                            {"color", do_color, 0, 0},       {"consider", do_consider, 0, 0},
                            {"crash", do_crash, 0, 0},

                            {"execute", do_execute, 0, 0},

                            {"kill", do_kill, 0, 0},

                            {"level", do_level, 0, 0},       {"load", do_load, 0, 0},
                            {"timewarp", do_timewarp, 0, 0},

                            {"rent", do_rent, 0, 0},         {"reset", do_reset, 0, 0},
                            {"save", do_save, 0, 0},         {"set", do_set, 0, 0},
                            {"setskill", do_setskill, 0, 0}, {"shutdown", do_shutdown, 0, 0},
                            {"snoop", do_snoop, 0, 0},       {"switch", do_switch, 0, 0},

                            {"users", do_users, 0, 0},       {"where", do_where, 0, 0},

                            {"wizlock", do_wizlock, 0, 0},   {"wstat", do_wstat, 0, 0},
                            {"wedit", do_wedit, 0, 0},       {"", nullptr, 0, 0}};

void skill_dump_alternate()
{
    printf("Skill Profession Tables Alternate\n");
    for (int j = 0; j < PROFESSION_MAX; j++)
    {
        std::vector<std::pair<int, std::string>> vect;

        for (int i = 0; i < SKI_TREE_MAX; i++)
        {
            if (g_SkiColl.text[i] == nullptr)
            {
                continue;
            }

            std::string str = diku::format_to_str("%s,%s", g_SkiColl.text[i], spc(20 - strlen(g_SkiColl.text[i])));

            str += diku::format_to_str(".profession %s%s = %s%d\n",
                                       g_professions[j],
                                       spc(12 - strlen(g_professions[j])),
                                       (g_SkiColl.prof_table[i].profession_cost[j] >= 0) ? "+" : "",
                                       g_SkiColl.prof_table[i].profession_cost[j]);
            /*if (g_SkiColl.prof_table[i].min_level > 0)
            {
                s printf(buf, "restrict level          = %d\n", g_SkiColl.prof_table[i].min_level);
                str.append(buf);
            }

            for (int k=0; k < ABIL_TREE_MAX; k++)
                if (g_SkiColl.prof_table[i].min_abil[k] > 0)
                {
                    s printf(buf, "restrict %s%s    = %s%d\n", g_SkiColl.text[i], spc(12-strlen(g_SkiColl.text[i])),
                        (g_SkiColl.prof_table[i].min_abil[k] >= 0) ? "+" : "", g_SkiColl.prof_table[i].min_abil[k]);
                    str.append(buf);
                }*/

            vect.push_back(std::make_pair(g_SkiColl.prof_table[i].profession_cost[j], str));
        }

        std::sort(vect.begin(), vect.end(), pairISCompare);
        for (auto it = vect.begin(); it != vect.end(); ++it)
        {
            printf("%s", it->second.c_str());
        }
    }
}



void skill_dump()
{
    printf("Skill Profession Tables\n");

    // Profession Headers
    for (int j = 0; j < PROFESSION_MAX; j++)
    {
        printf(",%s", g_professions[j]);
    }
    printf(",level,");
#undef PC_RACE_MAX
#define PC_RACE_MAX 11
    for (int ridx=0; ridx < PC_RACE_MAX; ridx++)
    {
        printf("%s,", g_pc_races[ridx]);
    }
    // Dump variable tail of ability restrictions
    for (int k=0; k < ABIL_TREE_MAX; k++)
    {
        printf("%s,", g_AbiColl.text[k]);
    }

    for (int i = 0; i < SKI_TREE_MAX; i++)
    {
        if (g_SkiColl.text[i] == nullptr)
        {
            continue;
        }

        printf("\n%s,", g_SkiColl.text[i]);

        for (int j = 0; j < PROFESSION_MAX; j++)
        {
            printf("%s%d,", (g_SkiColl.prof_table[i].profession_cost[j] >= 0) ? "+" : "",
                                       g_SkiColl.prof_table[i].profession_cost[j]);
        }

        printf("%s,", (g_SkiColl.prof_table[i].min_level == 0) ? "" : itoa(g_SkiColl.prof_table[i].min_level));

        for (int ridx=0; ridx < PC_RACE_MAX; ridx++)
        {
            if (g_SkiColl.racial[ridx][i] > 0)
            {
                printf("%d,", g_SkiColl.racial[ridx][i]);
            }
            else
            {
                printf(",");
            }
        }

        // Dump variable tail of ability restrictions
        for (int k=0; k < ABIL_TREE_MAX; k++)
        {
            if (g_SkiColl.prof_table[i].min_abil[k] > 0)
            {
                printf("%d,", g_SkiColl.prof_table[i].min_abil[k]);
            }
            else
            {
                printf(",");
            }
        }
    }
    printf("\n");

    printf("Progression training costs\n");
    printf(",");
    for (int i=7; i >= -7; i--)
        printf("%d,", i);
    printf("\n1st,");
    for (int i=7; i >= -7; i--)
        printf("%d,", actual_cost(i, 0, 0, 10));
    printf("\n2nd,");
    for (int i=7; i >= -7; i--)
        printf("%d,", actual_cost(i, 0, 1, 10));
    printf("\n3rd,");
    for (int i=7; i >= -7; i--)
        printf("%d,", actual_cost(i, 0, 2, 10));
    printf("\n");
}


void cmd_base_load()
{
    int i = 0;

    cmd_trie = nullptr;

    for (i = 0; *cmdload[i].cmd_str; i++)
    {
        cmd_trie = add_trienode(cmdload[i].cmd_str, cmd_trie);
    }

    qsort_triedata(cmd_trie);

    for (i = 0; *cmdload[i].cmd_str; i++)
    {
        set_triedata(cmdload[i].cmd_str, cmd_trie, &cmdload[i], TRUE);
    }

    command_read();
    if (g_dumptables)
    {
        skill_dump_alternate();
        skill_dump();
        exit(0);
    }
}

void command_read()
{
    int dummy = 0;
    int idx = -1;
    int ignore = FALSE;
    char pTmp[256];
    char *pCh = nullptr;
    FILE *fl = nullptr;
    command_info *cmdptr = nullptr;
    command_info *lastptr = nullptr;
    cmdload_struct *intcmd = nullptr;

    touch_file(g_cServerConfig.getFileInEtcDir(COMMAND_DEFS));
    if (!(fl = fopen(g_cServerConfig.getFileInEtcDir(COMMAND_DEFS).c_str(), "rb")))
    {
        slog(LOG_ALL, 0, "FATAL: Unable to open etc/ " COMMAND_DEFS);
        exit(0);
    }
    slog(LOG_ALL, 0, "Booting Commands from: " COMMAND_DEFS);

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);
        if (mstmp == nullptr)
        {
            continue;
        }

        str_remspc(pTmp);

        if ((pCh = strchr(pTmp, '=')))
        {
            *pCh = 0;
            pCh = skip_blanks(pCh + 1);
            strip_trailing_blanks(pCh);
        }

        str_lower(pTmp);
        strip_trailing_blanks(pTmp);

        if (pCh == nullptr || str_is_empty(pCh))
        {
            continue;
        }

        if (strncmp(pTmp, "command", 5) == 0)
        {
            if (cmdptr)
            {
                if (cmdptr->cmd_str && (cmdptr->cmd_fptr || cmdptr->tmpl))
                {
                    if (g_cmdlist == nullptr)
                    {
                        g_cmdlist = cmdptr;
                        cmdptr->prev = nullptr;
                        cmdptr->next = nullptr;
                        lastptr = cmdptr;
                        cmdptr = nullptr;
                    }
                    else
                    {
                        lastptr->next = cmdptr;
                        cmdptr->prev = lastptr;
                        cmdptr->next = nullptr;
                        lastptr = cmdptr;
                        cmdptr = nullptr;
                    }
                }
            }

            CREATE(cmdptr, command_info, 1);
            cmdptr->next = nullptr;
            cmdptr->prev = nullptr;
            cmdptr->minimum_level = 0;
            cmdptr->minimum_position = POSITION_DEAD;
            cmdptr->log_level = 0;
            cmdptr->tmpl = nullptr;
            cmdptr->cmd_fptr = nullptr;
            cmdptr->type = 0;
            cmdptr->no = 0;
            cmdptr->inttype = 0;
            cmdptr->dir = 0;
            cmdptr->cmd_str = nullptr;
            cmdptr->cmd_str = str_dup(pCh);
            ignore = FALSE;
            continue;
        }
        if (ignore)
        {
            continue;
        }

        if (strncmp(pTmp, "internal", 4) == 0)
        {
            if ((intcmd = ((cmdload_struct *)search_trie(pCh, cmd_trie))))
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
                cmdptr = nullptr;
                slog(LOG_ALL, 0, "COMMAND LOAD ERROR: %s not a defined internal funciton.", pCh);
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
            if (g_SkiColl.text[idx])
            {
                free((char *)g_SkiColl.text[idx]);
                g_SkiColl.text[idx] = nullptr;
            }
            g_SkiColl.text[idx] = str_dup(pCh);
            continue;
        }

        if (strncmp(pTmp, "auto train", 10) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, 0, 1))
            {
                g_SkiColl.tree[idx].bAutoTrain = dummy;
            }
            continue;
        }

        if (strncmp(pTmp, "auto teacher no add", 19) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, 0, 1))
            {
                g_SkiColl.tree[idx].bAutoTeacherNoAdd = dummy;
            }
            continue;
        }

        if (strncmp(pTmp, "race ", 5) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -3, +3))
            {
                continue;
            }

            int ridx = search_block(pTmp + 5, g_pc_races, TRUE);

            if (ridx == -1)
            {
                slog(LOG_ALL, 0, "Skills: Illegal race in: %s", pTmp);
            }
            else
            {
                g_SkiColl.racial[ridx][idx] = dummy;
            }
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

            int ridx = search_block(pTmp + 11, g_professions, TRUE);

            if (ridx == -1)
            {
                slog(LOG_ALL, 0, "Skills: Illegal profession %s", pTmp);
            }
            else
            {
                g_SkiColl.prof_table[idx].profession_cost[ridx] = dummy;
            }
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

            if (strncmp(pTmp + 9, "level", 5) == 0)
            {
                g_SkiColl.prof_table[idx].min_level = dummy;
            }
            else
            {
                int ridx = search_block(pTmp + 9, g_AbiColl.text, TRUE);

                if (ridx == -1)
                {
                    slog(LOG_ALL, 0, "Weapons: Illegal restrict %s", pTmp);
                }
                else
                {
                    g_SkiColl.prof_table[idx].min_abil[ridx] = dummy;
                }
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
                {
                    cmdptr->minimum_position = dummy;
                }
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
                {
                    slog(LOG_ALL, 0, "COMMAND LOAD WARNING: No such DIL template %s.", pCh);
                }
            }
            continue;
        }
        if (*pTmp)
        {
            slog(LOG_ALL, 0, "COMMAND LOAD ERROR: Unexpected text: %s", pTmp);
        }
    }

    if (cmdptr)
    {
        if (cmdptr->cmd_str && (cmdptr->cmd_fptr || cmdptr->tmpl))
        {
            if (g_cmdlist == nullptr)
            {
                g_cmdlist = cmdptr;
                cmdptr->prev = nullptr;
                cmdptr->next = nullptr;
                lastptr = cmdptr;
                cmdptr = nullptr;
            }
            else
            {
                lastptr->next = cmdptr;
                cmdptr->prev = lastptr;
                cmdptr->next = nullptr;
                lastptr = cmdptr;
                cmdptr = nullptr;
            }
        }
    }

    fclose(fl);
}
