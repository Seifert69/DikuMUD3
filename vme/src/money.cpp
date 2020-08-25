/*
 $Author: All $
 $RCSfile: money.cpp,v $
 $Date: 2004/05/16 04:34:34 $
 $Revision: 2.5 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "textutil.h"
#include "handler.h"
#include "utility.h"
#include "db.h"
#include "money.h"
#include "vmelimits.h"

struct money_type money_types[MAX_MONEY + 1];
char *cur_strings[MAX_CURRENCY + 1];

/* procedures also used in the vmc part */

/* Returns the amount adjusted to closest payable value in the currency */
static amount_t
adjust_money(amount_t amt, currency_t currency)
{
    int i;

    for (i = 0; i <= MAX_MONEY; i++)
        if (money_types[i].currency == currency)
        {
            /* Calculate remainder of amount */
            amount_t tmp = amt % money_types[i].min_value;

            if (tmp >= money_types[i].min_value / 2)
                tmp -= money_types[i].min_value; /* Round up value */

            amt -= tmp; /* adjust amount */

            /* Zero isn't an acceptable outcome, so make it the minimum */
            if (amt < money_types[i].min_value)
                amt = money_types[i].min_value;

            return amt;
        }

    slog(LOG_OFF, 0, "Illegal currency (%d) used in call of adjust_money()",
         currency);

    /* assert(FALSE); I suppose that a DIL can set an illegal object value? */

    return 0;
}

currency_t local_currency(class unit_data *unit)
{
    /*  Well, I dunno...
     *  Any ideas?
     */
    return DEF_CURRENCY;
}

/*  Print out an optimal representation of currency
 *  e.g.: (1230, DEF_CURRENCY) -> '10 copper coins and 3 iron coins'
 */
char *
money_string(amount_t amt, currency_t currency, ubit1 verbose)
{
    static char buf[512];
    char tmp[256];
    struct money_type *money_tmp[MAX_MONEY + 1];
    sbit8 i, nr = 0, count = 0;
    amount_t times;

    /* Get an array of all types in this currency */
    for (i = 0; i <= MAX_MONEY; i++)
        if (money_types[i].currency == currency)
            money_tmp[nr++] = &money_types[i];

    *buf = *tmp = '\0';

    amt = adjust_money(amt, currency);

    while (nr--)
        if ((times = (amt / money_tmp[nr]->relative_value)))
        {
            strcat(buf, tmp);
            amt -= money_tmp[nr]->relative_value * times;

            if (verbose)
            {
                if (times == 1)
                    sprintf(tmp, "%s %s, ",
                            strchr("aeiou",
                                   *(money_tmp[nr]->strings[0]))
                                ? "an"
                                : "a",
                            money_tmp[nr]->strings[0]);
                else
                    sprintf(tmp, "%d %s, ", (int)times,
                            money_tmp[nr]->strings[money_tmp[nr]->pl_idx]);
            }
            else /* Short version for lists... */
                sprintf(tmp, "%d %s, ", (int)times, money_tmp[nr]->abbrev);

            count++;
        }

    if (count == 0)
    { /* This shouldn't happen (I guess) */
        strcpy(buf, "nothing");
        return buf;
    }
    tmp[strlen(tmp) - 2] = '\0'; /* Cut off last comma */

    if (count > 1) /* Kill buf's last comma and put in `and' */
        strcpy(&buf[strlen(buf) - 2], " and ");

    strcat(buf, tmp);
    return buf;
}

#ifndef VMC_SRC

/* Local procedures */

/* Money calculations. Assures we never get negative money */
static amount_t
calc_money(amount_t v1, char op, amount_t v2)
{
    register amount_t res = 0;

    if (v1 < 0 || v2 < 0)
        return 0;

    switch (op)
    {
    case '-':
        return v1 < v2 ? 0 : v1 - v2;
    case '+':
        res = v1 + v2;
        break;
    case '*':
        res = v1 * v2;
        break;
    default:
        slog(LOG_OFF, 0, "Illegal operator (%c) used in calc_money()", op);
        assert(FALSE);
    }

    if (res < 0) /* overflow */
        return MAX(v1, v2);

    return res;
}

/* Set all the values on money correctly according to amount - return money */
class unit_data *
set_money(class unit_data *money, amount_t amt)
{
    char tmp[256];
    ubit32 i;

    assert(IS_MONEY(money));

    if (amt <= 0)
    {
        slog(LOG_ALL, 0, "Amount %d in set_money.", amt);
        amt = 1;
    }

    UNIT_NAMES(money).Free();

    /* Set relevant strings as names */
    for (i = 0; money_types[MONEY_TYPE(money)].strings[i]; ++i)
        if (amt == 1 && money_types[MONEY_TYPE(money)].pl_idx == i)
            break;
        else
            UNIT_NAMES(money).AppendName(money_types[MONEY_TYPE(money)].strings[i]);

    /* This isn't very pretty, but i couldn't come up with anything better
     * at this hour :-)
     * Suffice to say that the current string system forces me to sort the
     * strings, or chaos arises...
     */
    for (i = 0; i < UNIT_NAMES(money).Length(); ++i)
    {
        ubit32 j, m = i;

        for (j = i; j < UNIT_NAMES(money).Length(); ++j)
            if (strlen(UNIT_NAMES(money).Name(m)) <
                strlen(UNIT_NAMES(money).Name(j)))
                m = j;

        if (m != i)
        {
            char *max = str_dup(UNIT_NAMES(money).Name(m));
            char *t = str_dup(UNIT_NAMES(money).Name(i));

            UNIT_NAMES(money).Substitute(i, max);
            UNIT_NAMES(money).Substitute(m, t);

            FREE(max);
            FREE(t);
        }
    }

    OBJ_PRICE(money) = amt;

    weight_change_unit(money,
                       amt / MONEY_WEIGHT(money) - UNIT_WEIGHT(money));

    UNIT_TITLE(money) = (obj_money_string(money, amt));

    if (amt == 1)
        sprintf(tmp, "A single %s has been left here.",
                money_singularis(money));
    else
        sprintf(tmp, "A %s %s has been left here.",
                amt == 2 ? "couple of" : amt < 10 ? "few" : amt < 100 ? "small pile of" : amt < 1000 ? "pile of" : amt < 50000 ? "large pile of" : "mountain of", money_pluralis(money));

    UNIT_OUT_DESCR(money) = (tmp);

    return money;
}

static class unit_data *make_money(class file_index_type *fi, amount_t amt)
{
    class unit_data *money = read_unit(fi);
    char buf[512];

    assert(IS_OBJ(money));

    UNIT_WEIGHT(money) = 0; /* Init money-weight */

    sprintf(buf, cur_strings[MONEY_CURRENCY(money)],
            money_types[MONEY_TYPE(money)].tails);

    UNIT_EXTRA(money).add("", buf);

    return set_money(money, amt);
}

/*  This is the most non-trivial part of this module, and has certainly caused
 *  me a bit of head-scratching from its conception in '92 'til its completion
 *  in '94.  I may just be thick, of course.
 *
 *  It's IMPORTANT that you NEVER use this procedure to transfer money between
 *  two players, as it uses a bit of `magic' to solve the change problem.
 *  The players won't even notice if the other party is always a mobile.
 *  If players need to transfer money between them, let them use give/drop etc.
 *
 *  I wanted this one to make shops as simple and intuitive as they were in
 *  DikuI, despite the complexity of the system.
 */
void money_transfer(class unit_data *from, class unit_data *to,
                    amount_t amt, currency_t currency)
{
    amt = adjust_money(amt, currency);

    if (from)
    {
        /* amt <= money in from... */

        struct
        {
            amount_t take, have, value;
            currency_t cur;
            class unit_data *unit;
        } mon_array[MAX_MONEY + 1];

        class unit_data *tmp;
        int i, last;
        amount_t temp, calc = amt;

        /* Initialize computation */
        for (i = 0; i <= MAX_MONEY; i++)
        {
            mon_array[i].take = mon_array[i].have = 0;
            mon_array[i].value = money_types[i].relative_value;
            mon_array[i].unit = NULL;
            mon_array[i].cur = DEF_CURRENCY;
        }

        /* Note down money-objects in from, and their values */
        for (tmp = UNIT_CONTAINS(from); tmp; tmp = tmp->next)
            if (IS_MONEY(tmp) && MONEY_CURRENCY(tmp) == currency)
            {
                mon_array[MONEY_TYPE(tmp)].have = MONEY_AMOUNT(tmp);
                mon_array[MONEY_TYPE(tmp)].unit = tmp;
                mon_array[MONEY_TYPE(tmp)].cur = MONEY_CURRENCY(tmp);
            }

        /* Take as many coins (in number) as possible.
         * Stop when we have one `big' coin too many
         */
        for (i = last = 0; i <= MAX_MONEY && 0 < calc; i++)
            if (0 < mon_array[i].have)
            {
                /* Is there more than we need here? */
                if (calc <
                    calc_money(mon_array[i].have, '*', mon_array[i].value))
                    /* Yes, take just more than enough */
                    mon_array[i].take = calc / mon_array[i].value + 1;
                else /* No, take them all */
                    mon_array[i].take = mon_array[i].have;

                last = i; /* Biggest type taken yet */

                /* Update calc with money taken */
                calc -= calc_money(mon_array[i].take, '*', mon_array[i].value);
            }

        assert(calc <= 0); /* We have enough now; maybe too much */

        /* Now try to see if we can put anything back */
        for (i = last; 0 <= i && calc < 0; --i)
            if (0 < mon_array[i].take) /* Did we take any of these? */
                /* Put back as many as we can `afford' or have */
                if (0 <
                    (temp = MIN((-calc) / mon_array[i].value, mon_array[i].take)))
                {
                    mon_array[i].take -= temp;
                    calc += calc_money(mon_array[i].value, '*', temp);
                }

        assert(calc <= 0); /* We still have enough; maybe too much */

        /* Now take the money we calculated could be taken */
        for (i = 0; i <= last; i++)
            if (mon_array[i].take)
            {
                tmp = split_money(mon_array[i].unit, mon_array[i].take);
                unit_from_unit(tmp);

                if (to)
                    unit_to_unit(tmp, to);
                else
                    extract_unit(tmp);
            }

        /*  If we took too much, give change.
         *  The magic involved here should be invisible to the players...
         */
        if (calc < 0)
        {
            calc = -calc; /* Make it positive */

            if (to)
            {
                /* Find the coin that needs to be split up */
                for (i = 0; i <= last; ++i)
                    if (calc < mon_array[i].value && mon_array[i].take)
                        break;
                assert(0 < i && i <= last);

                /* Get that coin from `to' */
                for (tmp = UNIT_CONTAINS(to); tmp; tmp = tmp->next)
                    if (IS_MONEY(tmp) && MONEY_TYPE(tmp) == i)
                        break;
                assert(tmp);

                tmp = split_money(tmp, 1);
                unit_from_unit(tmp);
                extract_unit(tmp);

                /* Now split the above coin into apropriate change to both parties */

                money_to_unit(to, mon_array[i].value - calc, mon_array[i].cur);
            }
            money_to_unit(from, calc, DEF_CURRENCY);
        }
    }
    else if (to)
    { /* Create the money according to arguments and give to `to' */
        struct money_type *money_tmp[MAX_MONEY + 1];
        class unit_data *tmp;
        int i, nr;
        amount_t times;

        for (i = 0, nr = 0; i <= MAX_MONEY; i++)
            if (money_types[i].currency == currency)
                money_tmp[nr++] = &money_types[i];

        for (i = nr - 1; 0 <= i; --i)
            if (0 < (times = amt / money_tmp[i]->relative_value))
            {
                amt -= money_tmp[i]->relative_value * times;
                tmp = make_money(money_tmp[i]->fi, times);
                unit_to_unit(tmp, to);
            }
        if (amt)
            slog(LOG_ALL, 0, "Unadjusted or negative amount given as argument"
                             " to money_to_unit() (left: %d)",
                 amt);
    }
    else
    {
        slog(LOG_OFF, 0, "Call of money_transfer() with two NULL arguments");
        assert(FALSE);
    }
}

void coins_to_unit(class unit_data *unit, amount_t amt, int type)
{
    if (type == -1)
    {
        currency_t cur = local_currency(unit);
        money_to_unit(unit, amt, cur);
    }
    else
    {
        class unit_data *tmp = make_money(money_types[type].fi, amt);
        unit_to_unit(tmp, unit);
    }
}

/*  Counts up what amount of a given currency a unit holds recursively in
 *  inventory.
 *  Use ANY_CURRENCY as currency-type to count up ALL money...
 */
amount_t unit_holds_total(class unit_data *u, currency_t currency)
{
    class unit_data *tmp;
    amount_t amt = 0, rec;

    if (IS_ROOM(u) || IS_CHAR(u) || (IS_OBJ(u) && OBJ_TYPE(u) == ITEM_CONTAINER))

        for (tmp = UNIT_CONTAINS(u); tmp; tmp = tmp->next)
        {
            if (IS_MONEY(tmp) && (currency == ANY_CURRENCY || MONEY_CURRENCY(tmp) == currency))
            {
                if (amt < amt + MONEY_VALUE(tmp))
                { /* primitive overflow check */
                    amt += MONEY_VALUE(tmp);
                }
            }
            else
            {
                if (IS_ROOM(tmp) || IS_CHAR(tmp) || (IS_OBJ(tmp) && OBJ_TYPE(tmp) == ITEM_CONTAINER))
                {
                    rec = unit_holds_total(tmp, currency);
                    if (amt < amt + rec)
                    { /* primitive overflow check */
                        amt += rec;
                    }
                }
            }
        }
    return amt;
}

/*  Counts up what amount of a given currency char holds in inventory.
 *  Use ANY_CURRENCY as currency-type to count up ALL money...
 */
amount_t char_holds_amount(class unit_data *ch, currency_t currency)
{
    class unit_data *tmp;
    amount_t amt = 0;

    assert(IS_CHAR(ch));

    for (tmp = UNIT_CONTAINS(ch); tmp; tmp = tmp->next)
        if (IS_MONEY(tmp) && (currency == ANY_CURRENCY || MONEY_CURRENCY(tmp) == currency))
            if (amt < amt + MONEY_VALUE(tmp)) /* primitive overflow check */
                amt += MONEY_VALUE(tmp);

    return amt;
}

/*  Checks if the character is able to pay the amount with the currency
 *  (Currently) based on what money he has in inventory.
 */
ubit1 char_can_afford(class unit_data *ch, amount_t amt, currency_t currency)
{
    class unit_data *tmp;

    assert(IS_CHAR(ch));

    amt = adjust_money(amt, currency);

    for (tmp = UNIT_CONTAINS(ch); tmp; tmp = tmp->next)
    {
        if (IS_MONEY(tmp) && MONEY_CURRENCY(tmp) == currency)
        {
            if (amt < amt - MONEY_VALUE(tmp)) /* primitive overflow check */
            {
                return TRUE;
            }
            else
            {
                amt -= MONEY_VALUE(tmp);
            }
        }
    }

    return amt <= 0;
}

/* Check if there is some money of `type' in unit. (For piling purposes.) */
class unit_data *
unit_has_money_type(class unit_data *unit, ubit8 type)
{
    class unit_data *tmp;

    for (tmp = UNIT_CONTAINS(unit); tmp; tmp = tmp->next)
        if (IS_MONEY(tmp) && MONEY_TYPE(tmp) == type)
            return tmp;

    return NULL;
}

/*  Split `money' into two objects.
 *  Return object with amount `amt' which will be `in' same unit as money
 *  (which is why you must ALWAYS make sure the new object is either piled,
 *  or moved!)
 */
class unit_data *
split_money(class unit_data *money, amount_t amt)
{
    assert(IS_MONEY(money));

    if (amt <= 0 || amt > MONEY_AMOUNT(money))
    {
        slog(LOG_ALL, 0, "Illegal amount %d in split_money.", amt);
        amt = 1;
    }

    if (MONEY_AMOUNT(money) > amt)
    {
        /* Not very pretty to use this, but I really can't find an alternative */
        void intern_unit_to_unit(class unit_data *, class unit_data *, ubit1);

        class unit_data *pnew =
            make_money(money_types[MONEY_TYPE(money)].fi, amt);
        set_money(money, calc_money(MONEY_AMOUNT(money), '-', amt));

        if (UNIT_IN(money))
            intern_unit_to_unit(pnew, UNIT_IN(money), FALSE);

        return pnew;
    }

    /* MONEY_AMOUNT(money) == amt */
    return money;
}

/*  Make sure that _if_ there is another money-object of `money's type in
 *  the same object, that they're fused
 */
void pile_money(class unit_data *money)
{
    class unit_data *tmp, *unit = UNIT_IN(money);

    assert(IS_MONEY(money) && unit);

    for (tmp = UNIT_CONTAINS(unit); tmp; tmp = tmp->next)
        if (tmp != money && IS_MONEY(tmp) && MONEY_TYPE(tmp) == MONEY_TYPE(money))
        {
            set_money(money,
                      calc_money(MONEY_AMOUNT(money), '+',
                                 MONEY_AMOUNT(tmp)));
            extract_unit(tmp);
            return;
        }
}

/*  Round amount down/up to nearest `types' number of coins
 */
amount_t money_round(ubit1 up, amount_t amt, currency_t currency, int types)
{
    struct money_type *money_tmp[MAX_MONEY + 1];
    int i, nr = 0;
    amount_t times, remainder = amt;

    assert(0 < types);

    /* Get an array of all types in this currency */
    for (i = 0; i <= MAX_MONEY; i++)
        if (money_types[i].currency == currency)
            money_tmp[nr++] = &money_types[i];

    /*  Loop while we have coin-types left,
     *  but don't select a `type' more than `types' times...
     */
    while (nr-- && types)
        if ((times = (remainder / money_tmp[nr]->relative_value)))
        {
            --types;
            remainder -= money_tmp[nr]->relative_value * times;
        }

    /* After this loop, remainder is guaranteed to be non-negative
     * Its value represents the MOST coins of the top `types' values which can
     * be removed without taking too much, meaning that it is a rounded down
     * value.  To round up, just take a single coin of last type used.
     */
    if (up && remainder > 0)
        remainder -= money_tmp[nr + 1]->relative_value;

    return amt - remainder; /* This value IS adjusted! */
}

/* Print out representation of supplied money-object with the amount amt */
char *
obj_money_string(class unit_data *obj, amount_t amt)
{
    static char buf[128];
    struct money_type *money_tmp;

    assert(IS_MONEY(obj));

    money_tmp = &money_types[MONEY_TYPE(obj)];

    if (amt == 0)
        amt = MONEY_AMOUNT(obj);

    if (amt == 1)
        sprintf(buf, "%s %s",
                strchr("aeiou", *(money_tmp->strings[0])) ? "an" : "a",
                money_tmp->strings[0]);
    else
        sprintf(buf, "%d %s", (int)amt, money_tmp->strings[money_tmp->pl_idx]);

    return buf;
}

amount_t char_can_carry_amount(class unit_data *ch, class unit_data *money)
{
    int d_wgt = char_carry_w_limit(ch) - UNIT_CONTAINING_W(ch);

    return MIN((amount_t)(d_wgt * MONEY_WEIGHT(money)),
               MONEY_AMOUNT(money));
}

amount_t
unit_can_hold_amount(class unit_data *unit, class unit_data *money)
{
    int d_wgt = UNIT_CAPACITY(unit) - UNIT_CONTAINING_W(unit);

    return MIN((amount_t)(d_wgt * MONEY_WEIGHT(money)),
               MONEY_AMOUNT(money));
}

/* Temporary (?) wizcommand to create money */
#include "interpreter.h"
#include "comm.h"

void do_makemoney(class unit_data *ch, char *arg, const struct command_info *cmd)
{
    currency_t cur;
    amount_t amt = 0;

    if (!IS_PC(ch) || str_is_empty(arg) || (amt = atol(arg)) < 0)
    {
        send_to_char("Rather silly, isn't it?<br/>", ch);
        return;
    }

    cur = local_currency(ch);
    money_to_unit(ch, amt, cur);
    act("You just made $2t.",
        A_ALWAYS, ch, money_string(amt, cur, TRUE), cActParameter(), TO_CHAR);
}

#endif /* VMC_SRC */

/*
 *  DB boot routines
 */

static void
set_money_strings(FILE *fl, int idx)
{
    char buf[512], *tmp[32], *c, *s, *sc;
    int i = 0;

    if (fgets(buf, sizeof buf, fl) == NULL)
    {
        slog(LOG_OFF, 0, "Error reading money strings.");
        assert(FALSE);
    }

    s = buf;

    sc = strchr(s, ';');
    assert(sc);

    *(sc++) = '\0';

    do
    {
        s = skip_spaces(s);

        if ((c = strchr(s, ',')))
            *(c++) = '\0';

        while (isspace(*(s + strlen(s) - 1)))
            *(s + strlen(s) - 1) = '\0';

        tmp[i++] = str_dup(s);

        if ((s = c) == NULL && sc)
        {
            s = sc;
            sc = NULL;
            money_types[idx].pl_idx = i;
        }
    } while (s);

    tmp[i] = NULL;

    CREATE(money_types[idx].strings, char *, i + 1);
    for (; 0 <= i; --i)
        money_types[idx].strings[i] = tmp[i];
}

static void
set_relval(FILE *fl, int idx)
{
    char buf[128];
    long res;

    if (fgets(buf, sizeof buf, fl) == NULL || sscanf(buf, "%ld", &res) != 1)
    {
        slog(LOG_OFF, 0, "Error reading relative value.");
        assert(FALSE);
    }

    if (strchr(buf, '*'))
        res *= money_types[idx - 1].relative_value;

    money_types[idx].relative_value = res;
}

/*  flag has the bits:
 *    1: cut of trailing whitespace from line
 *    2: return a copy of read string
 */
static char *
get_string(FILE *fl, const char *err, ubit8 flag)
{
    static char buf[256];

    if (fgets(buf, sizeof buf, fl) == NULL)
    {
        slog(LOG_OFF, 0, "get_string(): %s", err);
        assert(FALSE);
    }

    if (flag & 1)
        while (isspace(*(buf + strlen(buf) - 1)))
            *(buf + strlen(buf) - 1) = '\0';

    return flag & 2 ? str_dup(buf) : buf;
}

void
#ifdef VMC_SRC
boot_money(char *moneyfile)
#else
boot_money(void)
#endif
{
    FILE *fl;
    char buf[256];
    int tmp, idx, prev_idx;
    currency_t cur;
    ubit1 currencies = TRUE;
    std::string myfile = "";

#ifdef VMC_SRC
    myfile = moneyfile;
    myfile.append(MONEYDEF_FILE);

    //if ((fl = fopen (str_cc (moneyfile, MONEYDEF_FILE), "r")) == NULL)
#else
    myfile = g_cServerConfig.m_etcdir;
    myfile.append(MONEYDEF_FILE);
    //if ((fl = fopen (str_cc (g_cServerConfig.m_etcdir, MONEYDEF_FILE), "r")) == NULL)
#endif
    if ((fl = fopen(myfile.c_str(), "r")) == NULL)
    {
        perror(str_cc("boot_money: ", myfile.c_str()));
        exit(3);
    }

    for (prev_idx = idx = -1;; prev_idx = idx)
    {
        do
        {
            if (fgets(buf, sizeof buf, fl) == NULL)
            {
                if (feof(fl))
                {
                    fclose(fl);
                    goto done; /* My first goto _ever!_ */
                }

                error(HERE, "Problem while reading money definition file.");
            }
        } while (buf[0] == '\n' || buf[0] == '#'); /* Skip empty lines & comments */

        if (currencies)
        {
            if (buf[0] == '~')
            {
                currencies = FALSE;
                continue;
            }

            sscanf(buf, "%d", &tmp);
            if (0 <= tmp && tmp <= MAX_CURRENCY)
                cur_strings[tmp] = get_string(fl, "currency strings", 2);
            else
            {
                slog(LOG_OFF, 0, "Illegal index of currency strings. (%d)",
                     tmp);
                assert(FALSE);
            }
            continue;
        }

        sscanf(buf, "%d %d", &tmp, &idx);

        if (idx != prev_idx + 1)
        {
            slog(LOG_OFF, 0,
                 "Money-file is expected to be in incremental order.");
            assert(FALSE);
        }

        if (MAX_MONEY < idx)
        {
            slog(LOG_OFF, 0,
                 "Money-file contains index larger than MAX_MONEY.");
            assert(FALSE);
        }

        money_types[idx].currency = (currency_t)tmp;

        set_money_strings(fl, idx);

        money_types[idx].abbrev = get_string(fl, "abbrev string", 3);

        set_relval(fl, idx);

        if (fgets(buf, sizeof buf, fl) == NULL || sscanf(buf, "%d", &tmp) != 1)
        {
            slog(LOG_OFF, 0, "Error reading coins_per_weight");
            assert(FALSE);
        }
        money_types[idx].coins_per_weight = (ubit8)tmp;

#ifndef VMC_SRC
        money_types[idx].fi = str_to_file_index(get_string(fl,
                                                           "unit-string", 1));
        assert(money_types[idx].fi);
#else
        get_string(fl, "unit-string (not used)", 0);
#endif

        money_types[idx].tails = get_string(fl, "tails-string", 3);
    }

done:
    for (cur = 0; cur <= MAX_CURRENCY; ++cur)
    {
        tmp = -1;

        for (idx = 0; idx <= MAX_MONEY; ++idx)
            if (money_types[idx].currency == cur)
                tmp = (tmp < 0) ? money_types[idx].relative_value : MIN(tmp, money_types[idx].relative_value);

        for (idx = 0; idx <= MAX_MONEY; ++idx)
            if (money_types[idx].currency == cur)
                money_types[idx].min_value = tmp;
    }
}
