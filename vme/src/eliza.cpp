/*
 $Author: All $
 $RCSfile: eliza.cpp,v $
 $Date: 2004/01/09 20:34:23 $
 $Revision: 2.3 $
 */

#include "eliza.h"

#include "db.h"
#include "files.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "main_functions.h"
#include "slog.h"
#include "textutil.h"
#include "unit_fptr.h"
#include "utils.h"

#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define ELIZA_TALKFILE "talk.eli"
#define ELIZA_LOGFILE "log.eli"

#define MAX_HISTORY 4
struct oracle_data
{
    int *nextrep;
    unit_data *patient;
    unit_data *doctor;
    int oldkeywd[MAX_HISTORY];      /* queue of indices of most recent keywds */
    char own_name[FI_MAX_UNITNAME]; /* Lowercase name of Doc.                */
    char laststr[MAX_INPUT_LENGTH]; /* Don't reply to same string twice      */
    char lastrep[MAX_INPUT_LENGTH]; /* Don't make same reply twice!          */
};

struct subject_type
{
    char **replies; /* NULL terminated         */
} *eliza_subjects = nullptr;
int eliza_maxsubjects = 0;

struct template_type
{
    char **exp; /* What shall we react on? */
    int subjno;
} *eliza_template = nullptr;
int eliza_maxtemplates = 0;

struct keyword_type
{
    char **keyword; /* What shall we react on? */
    char priority;  /* how important is keywd: 0 worst, 9 best */
    int subjno;
} *eliza_keyword = nullptr;
int eliza_maxkeywords = 0;

int eliza_booted = FALSE;

static char words[400];

/* ============================================================= */

void preprocess_string(char *str, oracle_data *od)
{
    str_lower(str);
    str_rem(str, '\'');
    str_blank_punct(str);
    str_chraround(str, ' ');

    str_substitute(" cannot ", " can not ", str);
    str_substitute(" cant ", " can not ", str);
    str_substitute(" dont ", " do not ", str);
    str_substitute(" ive ", " i have ", str);
    str_substitute(" youve ", " you have ", str);
    str_substitute(" youre ", " you are ", str);
    str_substitute(" im ", " i am ", str);
    str_substitute(" id ", " i would ", str);
    str_substitute(" but ", " , ", str);
    str_substitute(od->own_name, " ", str);

    str_remspc(str);
}

/* ================================================================== */

/*
  Check to see if the input line (input) matches template
  patterns rexp and lexp. Both expressions are delimited by
  zero character.
  Case 1. Lexp is identical with the input.
  Case 2. Template is of form Lexp <anything>.
  Case 3. Template is of form Lexp <anything> Rexp.
*/

char *match_templ(char *input, template_type *tem)
{
    char *respons = nullptr;
    char *rp = nullptr;
    char *lp = nullptr;
    char *ip = nullptr;
    char *tcp = nullptr;
    char *cp = nullptr;
    int i = 0;
    int j = 0;

    respons = words;
    ip = input;
    respons[0] = 0;

    if (tem->exp[0] && !tem->exp[1]) /* Case with no % signs */
    {
        cp = str_str(ip, tem->exp[0]);
        if (cp == nullptr)
        {
            return nullptr;
        }

        /* Only OK if there is a punctation left of expression */
        for (tcp = cp - 1; tcp >= input; tcp--)
        {
            if (!*tcp || ispunct(*tcp))
            {
                break;
            }
            else if (isalnum(*tcp))
            {
                return nullptr;
            }
        }

        /* Only OK if there is a punctation right of expression */
        for (tcp = cp + strlen(tem->exp[0]); *tcp; tcp++)
        {
            if (ispunct(*tcp))
            {
                break;
            }
            else if (isalnum(*tcp))
            {
                return nullptr;
            }
        }

        return respons;
    }

    for (i = 0; tem->exp[i]; i++)
    {
        if (!*tem->exp[i])
        {
            if (i > 0) /* Case: "text %" */
            {
                lp = str_str(ip, tem->exp[i - 1]);
                if (lp == nullptr)
                {
                    return nullptr;
                }
                for (tcp = lp - 1; tcp >= ip; tcp--)
                {
                    if (!*tcp || ispunct(*tcp))
                    {
                        break;
                    }
                    else
                    {
                        return nullptr;
                    }
                }

                lp += strlen(tem->exp[i - 1]);
                ip = lp;
            }
            else
            {
                lp = ip;
            }

            if (tem->exp[i + 1]) /* Case: "% text" */
            {
                rp = str_str(ip, tem->exp[i + 1]);
                if (rp == nullptr)
                {
                    return nullptr;
                }

                /* String must end with the appropriate "text" */
                for (cp = rp + strlen(tem->exp[i + 1]); *cp; cp++)
                {
                    if (isalpha(*cp))
                    {
                        return nullptr;
                    }
                    else if (!isspace(*cp))
                    {
                        break;
                    }
                }

                for (cp = rp; cp > ip; cp--)
                {
                    if (ispunct(*cp))
                    {
                        return nullptr;
                    }
                }

                ip = rp + strlen(tem->exp[i + 1]);
            }
            else /* Case: "text %" with nothing more (find end of string) */
            {
                for (rp = ip; *rp; rp++)
                {
                    if (ispunct(*rp))
                    {
                        rp--;
                        break;
                    }
                }
            }

            for (j = 0, cp = lp; cp < rp; j++, cp++)
            {
                respons[j] = *cp;
            }
            respons[j] = 0;
        }
    }
    if (*respons)
    {
        return respons;
    }
    else
    {
        return nullptr;
    }
}

/*
try to match the current line to a template.  in turn, try all
the templates to see if the first two letters match.  if so,
read in the template and call match to check the entire template.
*/
int trytempl(char *line)
{
    int i = 0;

    for (i = 0; i < eliza_maxtemplates; i++)
    {
        /* compare input line (line[]) to template (t[]) */
        if (match_templ(line, &eliza_template[i]))
        {
            return i;
        }
    }

    return -1;
}

void shift(int base, int delta)
{
    int i = 0;
    int k = 0;

    if (delta == 0)
    {
        return;
    }

    if (delta > 0)
    {
        k = base;
        while (words[k] != 0)
        {
            k++;
        }
        for (i = k; i >= base; i--)
        {
            words[i + delta] = words[i];
        }
    }
    else
    { /* delta <0 */
        for (i = 0; i == 0 || words[base + i - 1] != 0; i++)
        {
            words[base + i] = words[base + i - delta];
        }
    }
}

/*
  scan the array "words" for the string OLD.  if it occurs,
  replace it by NEW.  also set the parity bits on in the
  replacement text to  mark them as already modified
  */
void subst(const char *old, const char *pnew)
{
    int i = 0;
    int nlen = 0;
    int olen = 0;
    int flag = 0;
    int base = 0;
    int delim = 0;

    olen = strlen(old);
    nlen = strlen(pnew);

    for (base = 0; words[base] != 0; base++)
    {
        flag = 1;
        for (i = 0; i < olen; i++)
        {
            if (old[i] != words[base + i])
            {
                flag = 0;
                break;
            }
        }

        delim = words[base + olen];
        if (flag == 1 && (base == 0 || words[base - 1] == ' ') && (delim == ' ' || delim == 0))
        {
            shift(base, nlen - olen);
            for (i = 0; i < nlen; i++)
            {
                words[base + i] = pnew[i] + 128;
            }
        }
    }
}

void fix()
{
    /*   subst("ive", "you've"); Preproc   essed */
    /*   subst("youve", "I've"); Preprocessed */
    /*   subst("youre", "I am"); Preprocessed */
    /*   subst("im", "you're");  Preprocessed */
    /*   subst("we are", "we are"); UH? */
    subst("my dear", "love");
    subst("you were", "I was");
    subst("i was", "you were");
    subst("that you", "that i");
    subst("my", "your");
    subst("you", "me"); /* Or perhaps, "I"??? */
    subst("your", "my");
    subst("me", "you");
    subst("mine", "yours");
    subst("am", "are");
    subst("yours", "mine");
    subst("yourself", "myself");
    subst("myself", "yourself");
    subst("are", "am");
    subst("i", "you");
}

void grammar(char *str)
{
    int i = 0;

    for (i = 0; str[i]; i++)
    {
        str[i] = str[i] & 127;
    }

    str_substitute(" i are ", " I am ", str);
    str_substitute(" you am ", " you are ", str);
    str_substitute(" me am ", " I am ", str);
    /*   str_substitute(" me do  ", " I do ", str); */
}

/* Remember which keywords have previously been used... */
/* Register 'n' as the index                            */
void eliza_store_memory(oracle_data *od, int n, int pri)
{
    assert(n < eliza_maxsubjects);

    if ((pri > '5') && (number('5', pri) > '5'))
    {
        memmove(&od->oldkeywd[1], &od->oldkeywd[0], sizeof(int) * (MAX_HISTORY - 1));
        od->oldkeywd[0] = n;
    }
}

/* Remember which keywords have previously been used... */
/* Register 'n' as the index                            */
int eliza_retrieve_memory(oracle_data *od)
{
    int mem = 0;

    mem = od->oldkeywd[0];

    memmove(&od->oldkeywd[0], &od->oldkeywd[1], sizeof(int) * (MAX_HISTORY - 1));

    od->oldkeywd[MAX_HISTORY - 1] = eliza_maxsubjects - 1;

    return mem;
}

/*
  getscript is set up to return the correct response.
  words contains the extracted words.  make reply
  */
char *response(oracle_data *od, int subjno)
{
    static char resp[400];
    char c1 = 0;
    char *cp = nullptr;
    int i = 0;
    int k = 0;
    int thisrep = 0;

    thisrep = od->nextrep[subjno]++;
    if (eliza_subjects[subjno].replies[od->nextrep[subjno]] == nullptr)
    {
        od->nextrep[subjno] = 0;
    }

    cp = eliza_subjects[subjno].replies[thisrep];

    if (strcmp(od->lastrep, cp) == 0)
    {
        od->lastrep[0] = 0;
        return response(od, eliza_retrieve_memory(od));
    }
    else
    {
        strcpy(od->lastrep, cp);
    }

    fix(); /* replace i by you etc. */

    for (k = 0; *cp; cp++)
    {
        switch (*cp)
        {
            case '%':
                for (i = 0; words[i] != 0; i++)
                {
                    c1 = words[i] & 0177;
                    if (c1 != 0)
                    {
                        resp[k++] = c1;
                    }
                }
                break;

            case '&':
                for (i = 0; od->patient->getNames().Name()[i] != 0; i++)
                {
                    resp[k++] = od->patient->getNames().Name()[i];
                }
                break;

            default:
                resp[k++] = *cp;
                break;
        }
    }

    resp[k] = 0;

    /* fix gross errors in grammar like "you am", "i are" */
    grammar(resp);

    return resp;
}

/* the line does not match a template.  are there any keywords in it */
int trykeywd(char *line, int *score)
{
    int j = 0;
    int index = 0;
    int mi = 0;

    index = -1;
    *score = -1;

    for (j = 0; j < eliza_maxkeywords; j++)
    {
        for (mi = 0; eliza_keyword[j].keyword[mi]; mi++)
        {
            if (str_str(line, eliza_keyword[j].keyword[mi]) != nullptr)
            {
                if (eliza_keyword[j].priority >= *score)
                {
                    if ((eliza_keyword[j].priority > *score) || number(0, 1))
                    {
                        *score = eliza_keyword[j].priority;
                        index = j;
                    }
                }
            }
        }
    }

    return index;
}

char *eliza_process(oracle_data *od, char *s)
{
    int i = 0;
    int pri = 0;

    if (strlen(s) <= 1)
    {
        return nullptr;
    }

    preprocess_string(s, od);

    if (strcmp(s, od->laststr) == 0)
    {
        return nullptr;
    }
    else
    {
        strcpy(od->laststr, s);
    }

    words[0] = 0;

    i = trytempl(s);
    if (i >= 0)
    {
        return response(od, eliza_template[i].subjno);
    }

    i = trykeywd(s, &pri);
    if (i >= 0)
    {
        eliza_store_memory(od, eliza_keyword[i].subjno, pri);
        return response(od, eliza_keyword[i].subjno);
    }

    /* nothing matched.  use an old keyword */
    return response(od, eliza_retrieve_memory(od));
}

/* ====================================================================== */
/* ====================================================================== */

void delayed_action(void *p1, void *p2)
{
    unit_data *npc = (unit_data *)p1;
    char *str = (char *)p2;

    command_interpreter(npc, str);
    FREE(str);
}

void set_delayed_action(unit_data *npc, char *str)
{
    int when = 0;
    char *cp = nullptr;

    when = WAIT_SEC * 1;

    while ((cp = strchr(str, '@')))
    {
        *cp = 0;
        g_events.add(when, delayed_action, npc, str_dup(str));
        str = cp + 1;
        when += WAIT_SEC * 1;
    }

    when += WAIT_SEC * 3;

    g_events.add(when, delayed_action, npc, str_dup(str));
}

#define MAX_ELIBUF 50

void eliza_log(unit_data *who, const char *str, int comms)
{
    static int idx = -1;
    static char *buf[MAX_ELIBUF];

    FILE *f = nullptr;

    if (idx == -1)
    {
        memset(buf, 0, sizeof(buf));
        // WHAT THE FUCK... WHY *4      memset (buf, 0, sizeof (buf) * 4);
        idx = 0;
    }

    if (str == nullptr)
    {
        idx = 0;
        for (int i = 0; i < MAX_ELIBUF; i++)
        {
            if (buf[i])
                FREE(buf[i]);
            buf[i] = nullptr;
        }
        return;
    }

    if ((comms < 5) && (idx < MAX_ELIBUF))
    {
        auto tmp = diku::format_to_str("%-12s::  %s\n", who->getNames().Name(), str);
        buf[idx++] = str_dup(tmp.c_str());
        return;
    }

    if (!(f = fopen_cache(g_cServerConfig.getFileInLogDir(ELIZA_LOGFILE), "a+")))
    {
        abort();
    }

    if (idx <= MAX_ELIBUF)
    {
        for (int i = 0; i < idx; i++)
        {
            fprintf(f, "%s", buf[i]);
        }
        idx = MAX_ELIBUF + 1;
    }

    fprintf(f, "%-12s::  %s\n", who->getNames().Name(), str);
    fflush(f);
}

#define ELIZA_DEBUG TRUE

/* BUG: HAS MEMORY LEAK ERROR: WHEN KILLED, EVENTS ARE DEENQ'ed AND
        THE STRDUP'ED STRINGS ARE NOT FREED */
int oracle(spec_arg *sarg)
{
    static int comms = 0;
    char buf[2 * MAX_INPUT_LENGTH];
    char *response = nullptr;
    oracle_data *od = nullptr;
    int i = 0;

    od = (oracle_data *)sarg->fptr->getData();

    if (sarg->cmd->no == CMD_AUTO_EXTRACT)
    {
        if (od)
        {
            FREE(od->nextrep);
            FREE(od);
        }
        sarg->fptr->setData(nullptr);
        g_events.remove_relaxed(delayed_action, sarg->owner, nullptr);
        return SFR_BLOCK;
    }

    if (sarg->fptr->getData() == nullptr)
    {
        if (eliza_booted == FALSE)
        {
            eliza_boot();
            if (eliza_maxsubjects < 10)
            {
                destroy_fptr(sarg->owner, sarg->fptr);
                return SFR_SHARE;
            }
            eliza_booted = TRUE;
        }

        CREATE(od, oracle_data, 1);
        sarg->fptr->setData(od);
        od->laststr[0] = 0;
        od->lastrep[0] = 0;
        strcpy(od->own_name, sarg->owner->getNames().Name());
        str_lower(od->own_name);
        od->patient = nullptr;
        od->doctor = sarg->owner;

        CREATE(od->nextrep, int, eliza_maxsubjects);
        for (i = 0; i < eliza_maxsubjects; i++)
        {
            od->nextrep[i] = 0;
        }
        for (i = 0; i < MAX_HISTORY; i++)
        {
            od->oldkeywd[i] = eliza_maxsubjects - 1;
        }
    }

    if (!CHAR_IS_READY(sarg->owner))
    {
        return SFR_SHARE;
    }

    if (od->patient && !scan4_ref(sarg->owner, od->patient))
    {
        sarg->fptr->removeActivateOnEventFlag(SFB_PRIORITY);
        od->patient = nullptr;
    }

    if (od->patient != sarg->activator)
    {
        if ((od->patient == nullptr) && (!is_command(sarg->cmd, "north")) && (!is_command(sarg->cmd, "east")) &&
            (!is_command(sarg->cmd, "west")) && (!is_command(sarg->cmd, "south")) && (!is_command(sarg->cmd, "northeast")) &&
            (!is_command(sarg->cmd, "southeast")) && (!is_command(sarg->cmd, "northwest")) && (!is_command(sarg->cmd, "southwest")) &&
            (!is_command(sarg->cmd, "ne")) && (!is_command(sarg->cmd, "se")) && (!is_command(sarg->cmd, "sw")) &&
            (!is_command(sarg->cmd, "nw")) && (!is_command(sarg->cmd, "down")) && (!is_command(sarg->cmd, "up")) &&
            CHAR_CAN_SEE(sarg->owner, sarg->activator))
        {
            switch (number(0, 3))
            {
                case 0:
                    snprintf(buf, sizeof(buf), "smile %s", sarg->activator->getNames().Name());
                    break;

                case 1:
                    snprintf(buf, sizeof(buf), "hug %s", sarg->activator->getNames().Name());
                    break;

                case 2:
                    snprintf(buf, sizeof(buf), "ruffle %s", sarg->activator->getNames().Name());
                    break;

                case 3:
                    snprintf(buf, sizeof(buf), "bow %s", sarg->activator->getNames().Name());
                    break;
            }
            od->patient = sarg->activator;
            set_delayed_action(sarg->owner, buf);
            comms = 0;
            eliza_log(sarg->owner, nullptr, comms);
            eliza_log(sarg->owner, "========== oOo ============", comms);
            sarg->fptr->setActivateOnEventFlag(SFB_PRIORITY);
        }
        return SFR_SHARE;
    }

    if (sarg->activator->isPC() || ELIZA_DEBUG)
    {
        snprintf(buf, sizeof(buf), "%s %s", sarg->cmd->cmd_str, sarg->arg);
        eliza_log(sarg->activator, buf, comms);
    }

    if (is_command(sarg->cmd, "tell") || is_command(sarg->cmd, "ask") || is_command(sarg->cmd, "whisper"))
    {
        unit_data *u = nullptr;
        char *c = (char *)sarg->arg;

        u = find_unit(sarg->activator, &c, nullptr, FIND_UNIT_SURRO);
        if (u != sarg->owner)
        {
            return SFR_SHARE;
        }
    }
    else if (!is_command(sarg->cmd, "say") && (!is_command(sarg->cmd, "shout")))
    {
        return SFR_SHARE;
    }

    comms++;

    strcpy(buf, sarg->arg);
    response = eliza_process(od, buf);
    if (response == nullptr)
    {
        return SFR_SHARE;
    }

    if (sarg->activator->isPC() || ELIZA_DEBUG)
    {
        eliza_log(sarg->owner, response, comms);
    }

    set_delayed_action(sarg->owner, response);

    return SFR_SHARE;
}

/* ====================================================================== */
template_type *eliza_find_template(int subjno)
{
    int i = 0;

    for (i = 0; i < eliza_maxtemplates; i++)
    {
        if (eliza_template[i].subjno == subjno)
        {
            return &eliza_template[i];
        }
    }

    eliza_maxtemplates++;
    if (eliza_maxtemplates == 1)
    {
        CREATE(eliza_template, template_type, 1);
    }
    else
    {
        RECREATE(eliza_template, template_type, eliza_maxtemplates);
    }
    eliza_template[eliza_maxtemplates - 1].subjno = subjno;
    eliza_template[eliza_maxtemplates - 1].exp = create_namelist();

    return &eliza_template[eliza_maxtemplates - 1];
}

keyword_type *eliza_find_keyword(int subjno)
{
    int i = 0;

    for (i = 0; i < eliza_maxkeywords; i++)
    {
        if (eliza_keyword[i].subjno == subjno)
        {
            return &eliza_keyword[i];
        }
    }

    eliza_maxkeywords++;
    if (eliza_maxkeywords == 1)
    {
        CREATE(eliza_keyword, keyword_type, 1);
    }
    else
    {
        RECREATE(eliza_keyword, keyword_type, eliza_maxkeywords);
    }
    eliza_keyword[eliza_maxkeywords - 1].keyword = create_namelist();
    eliza_keyword[eliza_maxkeywords - 1].priority = '5';
    eliza_keyword[eliza_maxkeywords - 1].subjno = subjno;

    return &eliza_keyword[eliza_maxkeywords - 1];
}

void eliza_get_template(char *buf, int subjno)
{
    template_type *tem = nullptr;
    char *b = nullptr;
    char tmp[400];
    int i = 0;

    tem = eliza_find_template(subjno);
    b = buf;

    for (; !str_is_empty(b);)
    {
        for (i = 0; b[i]; i++)
        {
            tmp[i] = b[i];
            if (b[i] == '%')
            {
                break;
            }
        }
        tmp[i] = 0;

        if (!str_is_empty(tmp))
        {
            tem->exp = add_name(tmp, tem->exp);
        }

        if (b[i] == '%')
        {
            tem->exp = add_name("", tem->exp);
            b += i + 1;
        }
        else
        {
            b += i;
        }
    }
}

void eliza_get_keyword(char *buf, int subjno, int priority)
{
    keyword_type *kwd = nullptr;

    kwd = eliza_find_keyword(subjno);

    kwd->priority = priority;
    kwd->keyword = add_name(buf, kwd->keyword);
}

void eliza_get_reacts(FILE *f, int subjno)
{
    char buf[240];
    int i = 0;
    int cnt = 0;
    int priority = 0;
    char c = 0;

    priority = '5';

    for (cnt = 1;; cnt++)
    {
        c = fgetc(f);
        if (c == '*')
        {
            ungetc(c, f);
            return;
        }

        char *mstmp = fgets(buf, 240, f);
        if (mstmp == nullptr)
        {
            slog(LOG_ALL, 0, "Eliza: unexpected fgets == NULL");
            assert(FALSE);
        }

        buf[strlen(buf) - 1] = 0; /* Destroy the newline from fgets */

        for (i = 0; buf[i]; i++)
        {
            if (isdigit(buf[i]))
            {
                priority = buf[i];
                buf[i] = 0;
            }
        }

        while (buf[--i] == ' ')
        {
            buf[i] = 0;
        }

        str_chraround(buf, ' ');
        str_remspc(buf);

        switch (c)
        {
            case '+':
                eliza_get_keyword(buf, subjno, priority);
                break;

            case '#':
                eliza_get_template(buf, subjno);
                break;

            default:
                slog(LOG_ALL, 0, "Illegal eliza-code: '%s'", buf);
                assert(FALSE);
        }
    }
}

void eliza_get_subjects(FILE *f)
{
    char buf[240];

    if (feof(f))
    {
        return;
    }

    eliza_maxsubjects++;
    if (eliza_maxsubjects == 1)
    {
        CREATE(eliza_subjects, subject_type, 1);
    }
    else
    {
        RECREATE(eliza_subjects, subject_type, eliza_maxsubjects);
    }

    eliza_subjects[eliza_maxsubjects - 1].replies = create_namelist();

    for (;;)
    {
        buf[0] = fgetc(f);
        if (feof(f))
        {
            return;
        }
        if (buf[0] != '*')
        {
            ungetc(buf[0], f);
            break;
        }

        char *mstmp = fgets(buf, 240, f);
        if (mstmp == nullptr)
        {
            slog(LOG_ALL, 0, "Eliza: unexpected fgets == NULL");
            assert(FALSE);
        }

        if (*buf)
        {
            buf[strlen(buf) - 1] = 0; /* Destroy the newline from fgets */
        }

        eliza_subjects[eliza_maxsubjects - 1].replies = add_name(buf, eliza_subjects[eliza_maxsubjects - 1].replies);
    }
}

void eliza_gen_test_template(char *buf, template_type *tem)
{
    int j = 0;

    for (j = 0; tem->exp[j]; j++)
    {
        if (!*tem->exp[j])
        {
            strcpy(buf, " michael ");
        }
        else
        {
            strcpy(buf, tem->exp[j]);
        }
        TAIL(buf);
    }
}

void eliza_integrity()
{
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    char buf[240];

    /* Test if any templates are overshadowed by previous templates */
    for (i = 1; i < eliza_maxtemplates; i++)
    {
        eliza_gen_test_template(buf, &eliza_template[i]);
        j = trytempl(buf);

        if ((j != -1) && (j < i))
        {
            slog(LOG_ALL,
                 0,
                 "Eliza Template: '%s' conflicts with '%s/%s'",
                 buf,
                 STR(eliza_template[j].exp[0]),
                 STR(eliza_template[j].exp[1]));
        }
    }

    /* Test if any keywords are overshadowed by templates */
    for (i = 1; i < eliza_maxkeywords; i++)
    {
        for (j = 0; eliza_keyword[i].keyword[j]; j++)
        {
            strcpy(buf, eliza_keyword[i].keyword[j]);
            k = trytempl(buf);

            if (k != -1)
            {
                slog(LOG_ALL,
                     0,
                     "Eliza Keyword '%s' may be shadowed by '%s/%s'",
                     buf,
                     STR(eliza_template[k].exp[0]),
                     STR(eliza_template[k].exp[1]));
            }
        }
    }

    /* Test if any keywords are overshadowed by keywords */
    for (i = 1; i < eliza_maxkeywords; i++)
    {
        for (j = 0; eliza_keyword[i].keyword[j]; j++)
        {
            strcpy(buf, eliza_keyword[i].keyword[j]);
            k = trykeywd(buf, &l);

            if (k != i)
            {
                slog(LOG_ALL, 0, "Eliza Keyword '%s' may be shadowed by keywd '%s'", buf, STR(eliza_keyword[k].keyword[j]));
            }
        }
    }
}

void eliza_boot()
{
    FILE *f = nullptr;
    char c = 0;
    int i = 0;

    slog(LOG_ALL, 0, "Booting Eliza");

    if (!(f = fopen(g_cServerConfig.getFileInEtcDir(ELIZA_TALKFILE).c_str(), "r")))
    {
        slog(LOG_ALL, 0, "ELIZA BOOT FILE NOT FOUND.");
        return;
    }

    for (i = 0; !feof(f); i++)
    {
        for (; !feof(f);)
        {
            c = fgetc(f);

            if (!isspace(c))
            {
                ungetc(c, f);
                break;
            }
        }

        if (feof(f))
        {
            break;
        }

        eliza_get_reacts(f, i);
        eliza_get_subjects(f);
    }

    fclose(f);

    /* eliza_integrity(); I know it's ok now... Use once when mod. talk.eli */

    slog(LOG_ALL, 0, "Booting Eliza Done.");
}
