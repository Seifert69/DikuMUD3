/*
 $Author: All $
 $RCSfile: account.cpp,v $
 $Date: 2003/12/28 22:02:44 $
 $Revision: 2.3 $
 */
#ifdef _WINDOWS
    #include <direct.h>
#else
    #include <dirent.h>
#endif

#include "account.h"
#include "comm.h"
#include "db.h"
#include "error.h"
#include "files.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "str_parse.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"
#include "zon_basis.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

/* This is a LOT nicer than using structs. Trust me, I tried to program it! */

#define MINUTE_GRANULARITY (15)
#define TIME_GRANULARITY ((24 * 60) / MINUTE_GRANULARITY)

static sbit32 day_charge[7][TIME_GRANULARITY];

static int next_crc = 0;

CAccountConfig g_cAccountConfig;

void account_cclog(unit_data *ch, int amount)
{
    FILE *f = nullptr;

    f = fopen(g_cServerConfig.getFileInLogDir(CREDITFILE_LOG).c_str(), "a+b");

    fprintf(f, "%-16s %6.2f %s\n", UNIT_NAME(ch), ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);

    fclose(f);
}

static void account_log(char action, unit_data *god, unit_data *pc, int amount)
{
    // If a unit test is being run, send results there too
    unit_tests::OutputCapture::account_log(action, god, pc, amount);

    time_t now = time(nullptr);
    char *c = nullptr;
    char buf[1024];
    ubit32 gid = 0;
    ubit32 pid = 0;
    ubit32 total = 0;
    ubit32 crc = 0;
    ubit32 vxor = 0;
    FILE *f = nullptr;

    vxor = number(0x10000000, 0xFFFFFF00 >> 1);

    next_crc ^= vxor;

    f = fopen_cache(g_cServerConfig.getFileInLogDir(ACCOUNT_LOG), "r+b");

    if (fseek(f, 8L, SEEK_SET) != 0)
    {
        error(HERE, "Unable to seek in account log.");
    }
    snprintf(buf, sizeof(buf), "%08x", next_crc);

    if (fwrite(buf, sizeof(char), 8, f) != 8)
    {
        error(HERE, "Unable to write 1 in account log.");
    }

    c = buf;
    sprintf(c, "%c %-15s %-15s %8d ", action, UNIT_NAME(god), UNIT_NAME(pc), amount);

    if (IS_PC(god))
    {
        gid = PC_ID(god) ^ (vxor);
    }
    else
    {
        gid = 0 ^ (vxor);
    }

    pid = PC_ID(pc) ^ (vxor << 1);
    total = PC_ACCOUNT(pc).total_credit;

    amount += 13;
    amount ^= vxor << 2;
    total += 17;
    total ^= vxor << 3;

    crc = gid + pid + total + amount + now;
    crc ^= vxor << 4;

    TAIL(c);
    sprintf(c, "%01x%08x%08x%08x%08x%08x%08x%08x%08x\n", number(0, 15), ~vxor, gid, crc, pid, amount, total, next_crc, (ubit32)now);

    fseek(f, 0L, SEEK_END);

    fprintf(f, "%s", buf);

    fflush(f);
}

/* Hours in 0..23, minutes in 0..59 */
int index_to_time(int index)
{
    return 100 * ((index * MINUTE_GRANULARITY) / 60) + (index * MINUTE_GRANULARITY) % 60;
}

/* Hours in 0..23, minutes in 0..59 */
int time_to_index(ubit8 hours, ubit8 minutes)
{
    return ((hours * 60) + minutes) / MINUTE_GRANULARITY;
}

void account_local_stat(const unit_data *ch, unit_data *u)
{
    if (!g_cServerConfig.isAccounting())
    {
        send_to_char("Game is not in accounting mode.\n\r", ch);
        return;
    }

    if (!IS_PC(u))
    {
        send_to_char("Only players have accounts.\n\r", ch);
        return;
    }

    char *pTmstr = ctime((time_t *)&PC_ACCOUNT(u).flatrate);
    time_t now = time(nullptr);

    if (IS_ADMINISTRATOR(ch))
    {
        auto msg = diku::format_to_str("Credit         : %5.2f\n\r"
                                       "Credit Limit   : %5.2f\n\r"
                                       "Credit to date : %5.2f\n\r"
                                       "Credit Card    : %s\n\r"
                                       "Discount       : %3d%%\n\r"
                                       "Flat Rate      : %s%s"
                                       "Crack counter  : %3d\n\r",
                                       (float)PC_ACCOUNT(u).credit / 100.0,
                                       (float)PC_ACCOUNT(u).credit_limit / 100.0,
                                       (float)PC_ACCOUNT(u).total_credit / 100.0,
                                       PC_ACCOUNT(u).last4 == -1 ? "NONE" : "REGISTERED",
                                       PC_ACCOUNT(u).discount,
                                       PC_ACCOUNT(u).flatrate < (ubit32)now ? "Expired" : "Expires on ",
                                       PC_ACCOUNT(u).flatrate < (ubit32)now ? " (none)\n\r" : pTmstr,
                                       PC_ACCOUNT(u).cracks);
        send_to_char(msg, ch);
    }
    else
    {
        if (PC_ACCOUNT(u).total_credit > 0)
        {
            send_to_char("Has paid for playing.\n\r", ch);
        }
        else
        {
            send_to_char("Has NOT yet paid for playing.\n\r", ch);
        }
    }
}

void account_global_stat(unit_data *ch)
{
    char buf[100 * TIME_GRANULARITY];
    char *b = nullptr;
    int i = 0;
    int j = 0;

    if (!g_cServerConfig.isAccounting())
    {
        return;
    }

    auto msg = diku::format_to_str("\n\rAccounting mode:\n\r"
                                   "  Free from level : %d\n\r"
                                   "  Currency Name   : %s\n\r"
                                   "  Default limit   : %.2f\n\r"
                                   "  Default start   : %.2f\n\r\n\r",
                                   g_cAccountConfig.m_nFreeFromLevel,
                                   g_cAccountConfig.m_pCoinName,
                                   (float)g_cAccountConfig.m_nAccountLimit / 100.0,
                                   (float)g_cAccountConfig.m_nAccountFree / 100.0);
    send_to_char(msg, ch);

    b = buf;
    snprintf(buf, sizeof(buf), "    Time    Sun  Mon  Tir  Wed  Thu  Fri  Sat\n\r");
    TAIL(b);

    for (i = 0; i < TIME_GRANULARITY; i++)
    {
        int st = 0;
        int et = 0;

        st = index_to_time(i);
        et = st + MINUTE_GRANULARITY - 1;

        sprintf(b, "%4d-%4d", st, et);
        TAIL(b);

        for (j = 0; j < 7; j++)
        {
            sprintf(b, "%5d", day_charge[j][i]);
            TAIL(b);
        }
        strcat(b, "\n\r");
        TAIL(b);
    }

    page_string(CHAR_DESCRIPTOR(ch), buf);
}

void account_overdue(const unit_data *ch)
{
    int i = 0;
    int j = 0;

    if (g_cServerConfig.isAccounting())
    {
        ubit32 discount = PC_ACCOUNT(ch).discount;
        ubit32 lcharge = ((100 - discount) * g_cAccountConfig.m_nHourlyRate) / 100;

        if (lcharge == 0)
        {
            i = 0;
            j = 0;
        }
        else
        {
            i = (int)(PC_ACCOUNT(ch).credit_limit + PC_ACCOUNT(ch).credit);
            j = (int)(((float)(i % lcharge) / (float)((float)lcharge / 60.0)));
            i = i / lcharge;
        }

        auto msg = diku::format_to_str("Your account is overdue by %.2f %s with a "
                                       "limit of %.2f %s.\n\r"
                                       "The account will expire in %d hours and %d minutes.\n\r\n\r",
                                       (float)-PC_ACCOUNT(ch).credit / 100.0,
                                       g_cAccountConfig.m_pCoinName,
                                       (float)PC_ACCOUNT(ch).credit_limit / 100.0,
                                       g_cAccountConfig.m_pCoinName,
                                       i,
                                       j);
        send_to_char(msg, ch);
        send_to_char(g_cAccountConfig.m_pOverdueMessage, ch);
    }
}

void account_paypoint(unit_data *ch)
{
    send_to_char(g_cAccountConfig.m_pPaypointMessage, ch);
}

void account_closed(unit_data *ch)
{
    if (g_cServerConfig.isAccounting())
    {
        send_to_char(g_cAccountConfig.m_pClosedMessage, ch);
    }
}

static ubit32 seconds_used(ubit8 bhr, ubit8 bmi, ubit8 bse, ubit8 ehr, ubit8 emi, ubit8 ese)
{
    ubit32 secs = 0;

    secs = (ese - bse);

    secs += 60 * (emi - bmi);

    secs += 60 * 60 * (ehr - bhr);

    return secs;
}

static int tm_less_than(tm *b, tm *e)
{
    if (b->tm_wday != e->tm_wday)
    {
        return TRUE;
    }

    if (b->tm_hour > e->tm_hour)
    {
        return FALSE;
    }

    if (b->tm_hour == e->tm_hour)
    {
        if (b->tm_min > e->tm_min)
        {
            return FALSE;
        }
        else if (b->tm_min == e->tm_min)
        {
            if (b->tm_sec > e->tm_sec)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }

    return TRUE;
}

static void account_calc(unit_data *pc, tm *b, tm *e)
{
    int bidx = 0;
    int eidx = 0;
    tm t;
    ubit32 secs = 0;

    if (PC_ACCOUNT(pc).flatrate > (ubit32)time(nullptr))
    {
        return;
    }

    bidx = time_to_index(b->tm_hour, b->tm_min);
    assert(bidx < TIME_GRANULARITY);

    eidx = time_to_index(e->tm_hour, e->tm_min);
    assert(eidx < TIME_GRANULARITY);

    if (bidx != eidx)
    {
        int ntime = index_to_time(bidx);
        t.tm_hour = ntime / 100;
        t.tm_min = ntime % 100 + 14;
        t.tm_sec = 59;
        t.tm_wday = b->tm_wday;
        secs = 1;
    }
    else
    {
        t = *e;
        secs = 0;
    }

    secs += seconds_used(b->tm_hour, b->tm_min, b->tm_sec, t.tm_hour, t.tm_min, t.tm_sec);

    float amt = (((float)secs) * ((float)day_charge[b->tm_wday][bidx]) / (float)3600.0);

    if (is_in(PC_ACCOUNT(pc).discount, 1, 99))
    {
        PC_ACCOUNT(pc).credit -= (((float)(100 - PC_ACCOUNT(pc).discount)) * amt) / (float)100.0;
    }
    else
    {
        PC_ACCOUNT(pc).credit -= amt;
    }

#ifdef ACCOUNT_DEBUG
    {
        auto msg = diku::format_to_str("%d:%d.%d (%d) to %d:%d.%d (%d) = %d -- "
                                       "charge %d / %.2f\n\r",
                                       b->tm_hour,
                                       b->tm_min,
                                       b->tm_sec,
                                       b->tm_wday,
                                       t.tm_hour,
                                       t.tm_min,
                                       t.tm_sec,
                                       e->tm_wday,
                                       secs,
                                       day_charge[b->tm_wday][bidx],
                                       (((float)secs) * ((float)day_charge[b->tm_wday][bidx]) / 3600.0));
        send_to_char(msg, pc);
    }
#endif

    *b = t;
    b->tm_sec = 0;
    b->tm_min++;
    if (b->tm_min > 59)
    {
        b->tm_min = 0;
        b->tm_hour++;
        if (b->tm_hour > 23)
        {
            if (b->tm_wday != e->tm_wday) /* To make <= to work */
            {
                b->tm_wday = (b->tm_wday + 1) % 7;
                b->tm_hour = 0;
            }
        }
    }

    if (tm_less_than(b, e))
    {
        account_calc(pc, b, e);
    }
}

void account_subtract(unit_data *pc, time_t from, time_t to)
{
    tm bt;
    tm et;

    assert(IS_PC(pc));

    if (!g_cServerConfig.isAccounting())
    {
        return;
    }

    if (CHAR_LEVEL(pc) >= g_cAccountConfig.m_nFreeFromLevel)
    {
        return;
    }

    bt = *localtime(&from);
    et = *localtime(&to);

    /* DEBUG:
       snprintf(buf, sizeof(buf), "%d:%d.%d (%d) to %d:%d.%d (%d)\n\r",
       bt.tm_hour, bt.tm_min, bt.tm_sec, bt.tm_wday,
       et.tm_hour, et.tm_min, et.tm_sec, et.tm_wday);
       send_to_char(buf, pc); */

    account_calc(pc, &bt, &et);
}

int account_is_overdue(const unit_data *ch)
{
    if (g_cServerConfig.isAccounting() && (CHAR_LEVEL(ch) < g_cAccountConfig.m_nFreeFromLevel))
    {
        if (PC_ACCOUNT(ch).flatrate > (ubit32)time(nullptr))
        {
            return FALSE;
        }

        return (PC_ACCOUNT(ch).credit < 0.0);
    }

    return FALSE;
}

static void account_status(const unit_data *ch)
{
    int j = 0;
    int i = 0;
    char *pTmstr = nullptr;
    ubit32 discount = PC_ACCOUNT(ch).discount;
    ubit32 lcharge = ((100 - discount) * g_cAccountConfig.m_nHourlyRate) / 100;

    if (account_is_overdue(ch))
    {
        send_to_char("Your account is overdue.\n\r", ch);
        send_to_char(g_cAccountConfig.m_pOverdueMessage, ch);
    }

    if (discount > 0)
    {
        auto msg = diku::format_to_str("You have an overall discount of %d%%.\n\r", discount);
        send_to_char(msg, ch);
    }

    if (PC_ACCOUNT(ch).flatrate > (ubit32)time(nullptr))
    {
        pTmstr = ctime((time_t *)&PC_ACCOUNT(ch).flatrate);
        auto msg = diku::format_to_str("Your account is on a flat rate until %s", pTmstr);
        send_to_char(msg, ch);

        if (PC_ACCOUNT(ch).credit >= 0.0)
        {
            auto msg2 = diku::format_to_str("You have a positive balance of %.2f %s.\n\r",
                                            PC_ACCOUNT(ch).credit / 100.0,
                                            g_cAccountConfig.m_pCoinName);
            send_to_char(msg2, ch);
        }
        return;
    }

    if (PC_ACCOUNT(ch).credit >= 0.0)
    {
        auto msg =
            diku::format_to_str("You have a positive balance of %.2f %s.\n\r", PC_ACCOUNT(ch).credit / 100.0, g_cAccountConfig.m_pCoinName);
        send_to_char(msg, ch);

        if (lcharge > 0)
        {
            i = (int)PC_ACCOUNT(ch).credit;
            j = (int)(((float)(i % lcharge) / (float)((float)lcharge / 60.0)));
            i = i / lcharge;

            act("At the current rate that is $2d hours and $3d minutes.", A_ALWAYS, ch, &i, &j, TO_CHAR);

            i = (int)(((float)PC_ACCOUNT(ch).credit_limit / (float)(lcharge)));

            auto msg = diku::format_to_str("Your credit limit is %d hours (%.2f %s).\n\r",
                                           i,
                                           (float)PC_ACCOUNT(ch).credit_limit / 100.0,
                                           g_cAccountConfig.m_pCoinName);
            send_to_char(msg, ch);
        }
    }
    else
    {
        if (lcharge > 0)
        {
            i = (int)(PC_ACCOUNT(ch).credit_limit + PC_ACCOUNT(ch).credit);
            j = (int)(((float)(i % lcharge) / (float)((float)lcharge / 60.0)));
            i = i / lcharge;

            auto msg = diku::format_to_str("Your account is overdue by %.2f %s with a "
                                           "limit of %.2f %s.\n\r"
                                           "The account will expire in %d hours and %d minutes.\n\r",
                                           (float)-PC_ACCOUNT(ch).credit / 100.0,
                                           g_cAccountConfig.m_pCoinName,
                                           (float)PC_ACCOUNT(ch).credit_limit / 100.0,
                                           g_cAccountConfig.m_pCoinName,
                                           i,
                                           j);
            send_to_char(msg, ch);
        }
        else
        {
            auto msg = diku::format_to_str("You have a negative balance of %.2f %s.\n\r",
                                           PC_ACCOUNT(ch).credit / 100.0,
                                           g_cAccountConfig.m_pCoinName);
            send_to_char(msg, ch);
        }
    }
}

int account_is_closed(unit_data *ch)
{
    int i = 0;
    int j = 0;

    if (g_cServerConfig.isAccounting() && (CHAR_LEVEL(ch) < g_cAccountConfig.m_nFreeFromLevel))
    {
        if (PC_ACCOUNT(ch).flatrate > (ubit32)time(nullptr))
        {
            return FALSE;
        }

        i = (int)PC_ACCOUNT(ch).credit;
        j = PC_ACCOUNT(ch).credit_limit;

        return (i < -j);
    }

    return FALSE;
}

void account_insert(unit_data *god, unit_data *whom, ubit32 amount)
{
    PC_ACCOUNT(whom).credit += (float)amount;
    PC_ACCOUNT(whom).total_credit += amount;

    slog(LOG_ALL, 255, "%s inserted %d on account %s.", UNIT_NAME(god), amount, UNIT_NAME(whom));
    account_log('I', god, whom, amount);
}

void account_withdraw(unit_data *god, unit_data *whom, ubit32 amount)
{
    PC_ACCOUNT(whom).credit -= (float)amount;
    if ((ubit32)amount > PC_ACCOUNT(whom).total_credit)
    {
        PC_ACCOUNT(whom).total_credit = 0;
    }
    else
    {
        PC_ACCOUNT(whom).total_credit -= amount;
    }

    slog(LOG_ALL, 255, "%s withdrew %d from account %s.", UNIT_NAME(god), amount, UNIT_NAME(whom));

    account_log('W', god, whom, amount);
}

void account_flatrate_change(unit_data *god, unit_data *whom, sbit32 days)
{
    sbit32 add = days * SECS_PER_REAL_DAY;

    time_t now = time(nullptr);

    std::string msg;
    if (days > 0)
    {
        if (PC_ACCOUNT(whom).flatrate > (ubit32)now)
        {
            msg = diku::format_to_str("\n\rAdding %d days to the flatrate.\n\r\n\r", days);
            PC_ACCOUNT(whom).flatrate += add;
        }
        else
        {
            assert(add > 0);
            msg = diku::format_to_str("\n\rSetting flatrate to %d days.\n\r\n\r", days);
            PC_ACCOUNT(whom).flatrate = now + add;
        }
    }
    else /* days < 0 */
    {
        if ((sbit32)PC_ACCOUNT(whom).flatrate + add < now)
        {
            msg = "\n\rDisabling flatrate, enabling measure rate.\n\r\n\r";
            PC_ACCOUNT(whom).flatrate = 0;
        }
        else
        {
            msg = diku::format_to_str("\n\rSubtracting %d days from the flatrate.\n\r\n\r", days);
            PC_ACCOUNT(whom).flatrate += add;
        }
    }

    slog(LOG_ALL, 255, "%s change flatrate with %d on account %s.", UNIT_NAME(god), days, UNIT_NAME(whom));
    account_log('F', god, whom, days);

    send_to_char(msg, god);
}

void do_account(unit_data *ch, char *arg, const command_info *cmd)
{
    char word[MAX_INPUT_LENGTH];
    unit_data *u = nullptr;
    unit_data *note = nullptr;
    char *c = (char *)arg;

    const char *operations[] = {"insert", "withdraw", "limit", "discount", "flatrate", nullptr};
    int i = 0;
    int amount = 0;

    if (!g_cServerConfig.isAccounting() || !IS_PC(ch))
    {
        send_to_char("That command is not available.<br/>", ch);
        return;
    }

    if (str_is_empty(arg) || !IS_ADMINISTRATOR(ch))
    {
        account_status(ch);
        return;
    }

    c = skip_spaces(c);

#ifdef ACCOUNT_DEBUG

    if (isdigit(*c))
    {
        tm btm, etm;

        char bhr[200], bmi[200], bse[200], bda[200];
        char ehr[200], emi[200], ese[200], eda[200];

        c = str_next_word_copy(c, bhr);
        c = str_next_word_copy(c, bmi);
        c = str_next_word_copy(c, bse);
        c = str_next_word_copy(c, bda);

        c = str_next_word_copy(c, ehr);
        c = str_next_word_copy(c, emi);
        c = str_next_word_copy(c, ese);
        c = str_next_word_copy(c, eda);

        btm.tm_hour = atoi(bhr);
        btm.tm_min = atoi(bmi);
        btm.tm_sec = atoi(bse);
        btm.tm_wday = atoi(bda);

        etm.tm_hour = atoi(ehr);
        etm.tm_min = atoi(emi);
        etm.tm_sec = atoi(ese);
        etm.tm_wday = atoi(eda);

        account_calc(ch, &btm, &etm);

        return;
    }

#endif

    u = find_unit(ch, &c, nullptr, FIND_UNIT_SURRO | FIND_UNIT_WORLD);

    if ((u == nullptr) || !IS_PC(u))
    {
        send_to_char("No such player found.\n\r", ch);
        return;
    }

    c = str_next_word(c, word);

    i = search_block(word, operations, TRUE);

    if (i == -1)
    {
        send_to_char("No such operation available.\n\r", ch);
        return;
    }

    c = str_next_word(c, word);

    amount = atoi(word);

    if (is_in(i, 0, 2))
    {
        if (!is_in(amount, 0, g_cAccountConfig.m_nMaxCharge))
        {
            send_to_char("Invalid amount.\n\r", ch);
            return;
        }
    }

    switch (i)
    {
        case 0: /* Insert amount   */
        {
            account_local_stat(ch, u);

            auto msg = diku::format_to_str("\n\rInserting %.2f %s.\n\r\n\r", (float)amount / 100.0, g_cAccountConfig.m_pCoinName);
            send_to_char(msg, ch);

            account_insert(ch, u, amount);

            account_local_stat(ch, u);

            msg = diku::format_to_str("%s inserted %.2f %s on your account.\n\r",
                                      UNIT_NAME(ch),
                                      (float)amount / 100.0,
                                      g_cAccountConfig.m_pCoinName);
            note = read_unit(g_letter_fi);
            UNIT_EXTRA(note).add("", msg.c_str());
            unit_to_unit(note, u);
        }
        break;

        case 1: /* Withdraw amount */
        {
            account_local_stat(ch, u);

            auto msg = diku::format_to_str("\n\rWithdrawing %.2f %s.\n\r\n\r", ((float)amount) / 100.0, g_cAccountConfig.m_pCoinName);
            send_to_char(msg, ch);

            account_withdraw(ch, u, amount);

            account_local_stat(ch, u);
        }
        break;

        case 2: /* Change limit    */
        {
            account_local_stat(ch, u);

            auto msg = diku::format_to_str("\n\rSetting limit to %.2f %s.\n\r\n\r", (float)amount / 100.0, g_cAccountConfig.m_pCoinName);

            send_to_char(msg, ch);

            PC_ACCOUNT(u).credit_limit = amount;

            account_local_stat(ch, u);

            slog(LOG_ALL, 255, "%s changed limit of %s to %d.", UNIT_NAME(ch), UNIT_NAME(u), amount);
            account_log('L', ch, u, amount);
        }
        break;

        case 3: /* Discount */
        {
            if (!is_in(amount, 0, 100))
            {
                send_to_char("Invalid discount, select 0% to 100%.\n\r", ch);
                return;
            }

            account_local_stat(ch, u);

            auto msg = diku::format_to_str("\n\rSetting discount to %3d%%.\n\r\n\r", amount);

            send_to_char(msg, ch);

            PC_ACCOUNT(u).discount = amount;

            account_local_stat(ch, u);

            slog(LOG_ALL, 255, "%s changed discount of %s to %d.", UNIT_NAME(ch), UNIT_NAME(u), amount);
            account_log('D', ch, u, amount);
        }
        break;

        case 4: /* Flatrate: account papi flatrate 30 add, 20 remove, etc. */
            if (!is_in(amount, 1, 365))
            {
                send_to_char("Invalid number of days, select 1 to 365.\n\r", ch);
                return;
            }

            c = str_next_word(c, word);
            if (is_abbrev(word, "add"))
            {
                ;
            }
            else if (is_abbrev(word, "remove"))
            {
                amount = -amount;
            }
            else
            {
                send_to_char("You must either add or remove.\n\r", ch);
                return;
            }

            account_local_stat(ch, u);

            account_flatrate_change(ch, u, amount);

            account_local_stat(ch, u);
            break;

        default:
            abort();
    }
}

void account_defaults(unit_data *pc)
{
    PC_ACCOUNT(pc).credit = (float)g_cAccountConfig.m_nAccountFree;
    PC_ACCOUNT(pc).credit_limit = (int)g_cAccountConfig.m_nAccountLimit;
    PC_ACCOUNT(pc).total_credit = 0;
    PC_ACCOUNT(pc).last4 = -1;
    PC_ACCOUNT(pc).discount = 0;
    PC_ACCOUNT(pc).cracks = 0;
    PC_ACCOUNT(pc).flatrate = 0;
}

void charge_sanity(ubit8 b_hr, ubit8 b_min, ubit8 e_hr, ubit8 e_min, int charge)
{
    if (charge < 0)
    {
        slog(LOG_ALL, 0, "Account error: Charge %d is < 0.", charge);
        exit(0);
    }

    if ((b_hr > e_hr) || ((b_hr >= e_hr) && (b_min >= e_min)))
    {
        slog(LOG_ALL, 0, "Account error: %d:%d >= %d:%d.", b_hr, b_min, e_hr, e_min);
        exit(0);
    }

    if (b_min % MINUTE_GRANULARITY != 0)
    {
        slog(LOG_ALL,
             0,
             "Account error: Begin time (%d:%d), minute not "
             "divisible by %d.",
             b_hr,
             b_min,
             MINUTE_GRANULARITY);
        exit(0);
    }

    if (e_min % MINUTE_GRANULARITY != (MINUTE_GRANULARITY - 1))
    {
        slog(LOG_ALL,
             0,
             "Account error: End time (%d:%d), minute not "
             "at border to %d.",
             e_hr,
             e_min,
             MINUTE_GRANULARITY);
        exit(0);
    }
}

void numlist_to_charge(int *numlist, int len, int *day_charge)
{
    int i = 0;
    int j = 0;
    int start_idx = 0;
    int end_idx = 0;

    for (i = 0; i < len; i += 3)
    {
        start_idx = time_to_index(numlist[i + 1] / 100, numlist[i + 1] % 100);
        end_idx = time_to_index(numlist[i + 2] / 100, numlist[i + 2] % 100);

        if (start_idx >= end_idx)
        {
            slog(LOG_ALL, 0, "Illegal time range %d - %d.", numlist[i + 1], numlist[i + 2]);
            exit(0);
        }

        if (end_idx >= TIME_GRANULARITY)
        {
            slog(LOG_ALL, 0, "Illegal time range %d - %d.", numlist[i + 1], numlist[i + 2]);
            exit(0);
        }

        for (j = start_idx; j <= end_idx; j++)
        {
            day_charge[j] = numlist[i + 0];
        }
    }
}

/* exits if something is wrong */
void numlist_sanity(int *numlist, int numlen)
{
    int i = 0;

    if (numlist == nullptr)
    {
        return;
    }

    if (numlen % 3 != 0)
    {
        slog(LOG_ALL, 0, "Account range illegal length - not divisible by 3.");
        exit(0);
    }

    for (i = 0; i < numlen; i += 3)
    {
        charge_sanity(numlist[i + 1] / 100, numlist[i + 1] % 100, numlist[i + 2] / 100, numlist[i + 2] % 100, numlist[i + 0]);
    }
}

int flatrate_sanity(int *numlist, int numlen)
{
    if (numlist == nullptr)
    {
        return FALSE;
    }

    if (numlen != 2)
    {
        slog(LOG_ALL, 0, "Account flatrate illegal length, must be 2.");
        exit(0);
    }

    if (!is_in(numlist[0], 30, 365))
    {
        slog(LOG_ALL, 0, "Account flatrate illegal days, must be 30..365.");
        exit(0);
    }

    if (!is_in(numlist[1], 1, 1000000))
    {
        slog(LOG_ALL, 0, "Account flatrate illegal amount.");
        exit(0);
    }

    return TRUE;
}

CAccountConfig::CAccountConfig()
{
    m_pOverdueMessage = nullptr;
    m_pClosedMessage = nullptr;
    m_pPaypointMessage = nullptr;
    m_pCoinName = nullptr;
    m_nFreeFromLevel = 200;
    m_nAccountLimit = 1500;
    m_nAccountFree = 1000;
    m_nMaxCharge = 10000;
    m_nMinCharge = 1000;
    m_nHourlyRate = 1000;
    m_bCreditCard = FALSE;

    memset(&m_flatrate, 0, sizeof(m_flatrate));
}

void CAccountConfig::Boot()
{
    char Buf[2 * MAX_STRING_LENGTH];
    char *c = nullptr;
    int len = 0;
    int i = 0;
    int j = 0;
    int *numlist = nullptr;
    FILE *f = nullptr;

    if (!g_cServerConfig.isAccounting())
    {
        return;
    }

    slog(LOG_OFF, 0, "Booting account system.");

    if (!file_exists(g_cServerConfig.getFileInLogDir(ACCOUNT_LOG)))
    {
        time_t now = time(nullptr);

        f = fopen(g_cServerConfig.getFileInLogDir(ACCOUNT_LOG).c_str(), "wb");

        if (f == nullptr)
        {
            slog(LOG_ALL, 0, "Can't create account log file.");
            exit(0);
        }

        slog(LOG_ALL, 0, "No account log existed - a new was created.");

        now ^= 0xAF876162;
        fprintf(f, "%08x%08x\n", (ubit32)now, (ubit32)now);

        fclose(f);
    }

    f = fopen_cache(g_cServerConfig.getFileInLogDir(ACCOUNT_LOG), "rb");

    int mstmp = fscanf(f, "%*08x%08x", &next_crc);
    if (mstmp < 1)
    {
        slog(LOG_ALL, 0, "ERROR: Unexpected bytes read");
        assert(FALSE);
    }

    touch_file(g_cServerConfig.getFileInEtcDir(ACCOUNT_FILE));

    config_file_to_string(g_cServerConfig.getFileInEtcDir(ACCOUNT_FILE), Buf, sizeof(Buf));

    c = Buf;

    m_pCoinName = parse_match_name((const char **)&c, "Coinage Name");

    if (m_pCoinName == nullptr)
    {
        slog(LOG_ALL, 0, "Error reading coin name.");
        exit(0);
    }

    m_pOverdueMessage = parse_match_name((const char **)&c, "Account Overdue");

    if (m_pOverdueMessage == nullptr)
    {
        slog(LOG_ALL, 0, "Error reading overdue message.");
        exit(0);
    }
    else
    {
        str_escape_format(m_pOverdueMessage, Buf, sizeof(Buf));
        FREE(m_pOverdueMessage);
        m_pOverdueMessage = str_dup((char *)Buf);
    }

    m_pClosedMessage = parse_match_name((const char **)&c, "Account Closed");
    if (m_pClosedMessage == nullptr)
    {
        slog(LOG_ALL, 0, "Error reading closed message.");
        exit(0);
    }
    else
    {
        str_escape_format(m_pClosedMessage, Buf, sizeof(Buf));
        FREE(m_pClosedMessage);
        m_pClosedMessage = str_dup((char *)Buf);
    }

    if (!parse_match_num((const char **)&c, "Min Charge", &m_nMinCharge))
    {
        slog(LOG_ALL, 0, "Error reading 'Min Charge'.");
        exit(0);
    }

    if (!parse_match_num((const char **)&c, "Max Charge", &m_nMaxCharge))
    {
        slog(LOG_ALL, 0, "Error reading 'Max Charge'.");
        exit(0);
    }

    if (!parse_match_num((const char **)&c, "Charge Level", &m_nFreeFromLevel))
    {
        slog(LOG_ALL, 0, "Error reading 'Charge Level'.");
        exit(0);
    }

    if (!parse_match_num((const char **)&c, "Account Free", &m_nAccountFree))
    {
        slog(LOG_ALL, 0, "Error reading account free.");
        exit(0);
    }

    if (!parse_match_num((const char **)&c, "Account Limit", &m_nAccountLimit))
    {
        slog(LOG_ALL, 0, "Error reading account limit");
        exit(0);
    }

    if (!parse_match_num((const char **)&c, "Credit Card", &m_bCreditCard))
    {
        slog(LOG_ALL, 0, "Error reading Credit Card usage.");
        exit(0);
    }

    if (!parse_match_num((const char **)&c, "Base Charge", &m_nHourlyRate))
    {
        slog(LOG_ALL, 0, "Error reading base charge.");
        exit(0);
    }

    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < TIME_GRANULARITY; j++)
        {
            day_charge[i][j] = m_nHourlyRate;
        }
    }

    numlist = parse_match_numlist((const char **)&c, "Base Range", &len);

    if (numlist)
    {
        numlist_sanity(numlist, len);
        for (i = 0; i < 7; i++)
        {
            numlist_to_charge(numlist, len, day_charge[i]);
        }
        FREE(numlist);
    }

    numlist = parse_match_numlist((const char **)&c, "ChargeSun", &len);
    if (numlist)
    {
        numlist_sanity(numlist, len);
        numlist_to_charge(numlist, len, day_charge[0]);
        FREE(numlist);
    }

    numlist = parse_match_numlist((const char **)&c, "ChargeMon", &len);
    if (numlist)
    {
        numlist_sanity(numlist, len);
        numlist_to_charge(numlist, len, day_charge[1]);
        FREE(numlist);
    }

    numlist = parse_match_numlist((const char **)&c, "ChargeTue", &len);
    if (numlist)
    {
        numlist_sanity(numlist, len);
        numlist_to_charge(numlist, len, day_charge[2]);
        FREE(numlist);
    }

    numlist = parse_match_numlist((const char **)&c, "ChargeWed", &len);
    if (numlist)
    {
        numlist_sanity(numlist, len);
        numlist_to_charge(numlist, len, day_charge[3]);
        FREE(numlist);
    }

    numlist = parse_match_numlist((const char **)&c, "ChargeThu", &len);
    if (numlist)
    {
        numlist_sanity(numlist, len);
        numlist_to_charge(numlist, len, day_charge[4]);
        FREE(numlist);
    }

    numlist = parse_match_numlist((const char **)&c, "ChargeFri", &len);
    if (numlist)
    {
        numlist_sanity(numlist, len);
        numlist_to_charge(numlist, len, day_charge[5]);
        FREE(numlist);
    }

    numlist = parse_match_numlist((const char **)&c, "ChargeSat", &len);
    if (numlist)
    {
        numlist_sanity(numlist, len);
        numlist_to_charge(numlist, len, day_charge[6]);
        FREE(numlist);
    }

    memset(m_flatrate, 0, sizeof(m_flatrate));

    for (i = 0; i < MAX_FLATRATE; i++)
    {
        snprintf(Buf, sizeof(Buf), "Flatrate%d", i + 1);

        numlist = parse_match_numlist((const char **)&c, Buf, &len);
        if (numlist)
        {
            if (flatrate_sanity(numlist, len))
            {
                m_flatrate[i].days = numlist[0];
                m_flatrate[i].price = numlist[1];
            }

            FREE(numlist);

            strcat(Buf, " Message");

            m_flatrate[i].pMessage = parse_match_name((const char **)&c, Buf);
            if (m_flatrate[i].pMessage == nullptr)
            {
                slog(LOG_ALL, 0, "Error reading flatrate message.");
                exit(0);
            }
            else
            {
                str_escape_format(m_flatrate[i].pMessage, Buf, sizeof(Buf));
                FREE(m_flatrate[i].pMessage);
                m_flatrate[i].pMessage = str_dup((char *)Buf);
            }
        }
    }

    numlist = parse_match_numlist((const char **)&c, "Flatrate2", &len);
    if (numlist)
    {
        if (flatrate_sanity(numlist, len))
        {
            m_flatrate[1].days = numlist[0];
            m_flatrate[1].price = numlist[1];
        }

        FREE(numlist);
    }

    m_pPaypointMessage = parse_match_name((const char **)&c, "Account Paypoint");
    if (m_pPaypointMessage == nullptr)
    {
        slog(LOG_ALL, 0, "Error reading paypoint message.");
        exit(0);
    }
    else
    {
        str_escape_format(m_pPaypointMessage, Buf, sizeof(Buf));
        FREE(m_pPaypointMessage);
        m_pPaypointMessage = str_dup((char *)Buf);
    }
}
