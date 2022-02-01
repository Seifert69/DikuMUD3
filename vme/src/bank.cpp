/*
 $Author: All $
 $RCSfile: bank.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */

#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cstdio>
#include <cstring>

static float INTEREST = .95;
static int INT_INTEREST = 5;

static amount_t balance[MAX_CURRENCY + 1];
static bool changed_balance;

static bool init_bank(const unit_data *pc, unit_data *clerk, bool init)
{
    if (clerk && !CHAR_IS_READY(clerk))
    {
        act("It seems that $3e isn't paying attention!", A_SOMEONE, pc, cActParameter(), clerk, TO_CHAR);
    }
    else if (clerk && !CHAR_CAN_SEE(clerk, pc))
    {
        act("$1n says 'Sorry, but I only do business with people I can see...'",
            A_SOMEONE,
            clerk,
            cActParameter(),
            cActParameter(),
            TO_ROOM);
    }
    else if (!IS_PC(pc))
    {
        send_to_char("Only trustworthy people are served...<br/>", pc);
    }
    else
    {
        if (init)
        {
            char *c = PC_BANK(pc);
            long amt = 0;
            int cur = 0;
            int i = 0;

            for (i = 0; i <= MAX_CURRENCY; ++i)
            {
                balance[i] = 0;
            }

            if (c)
            {
                while ((c = strchr(c, '~')))
                {
                    sscanf(++c, "%d %ld", &cur, &amt);
                    balance[cur] = amt;
                }
            }
        }

        return TRUE;
    }

    return FALSE;
}

static void cmd_balance(const unit_data *pc, unit_data *clerk, char *s)
{
    char buf[1024];
    bool any = FALSE;
    int i = 0;

    if (!init_bank(pc, clerk, TRUE))
    {
        return;
    }

    strcpy(buf, "You don't have an account.");

    for (i = 0; i <= MAX_CURRENCY; ++i)
    {
        if (balance[i])
        {
            if (any)
            {
                strcat(buf, ", ");
            }
            else
            {
                strcpy(buf, "You've got a balance of ");
            }

            any = TRUE;
            strcat(buf, money_string(balance[i], i, TRUE));
        }
    }

    act("$1n says '$2t'", A_SOMEONE, clerk, buf, pc, TO_VICT);
    act("$1n talks to $3n.", A_SOMEONE, pc, cActParameter(), clerk, TO_ROOM);
}

static void cmd_deposit(const unit_data *pc, unit_data *clerk, char *s)
{
    unit_data *thing = nullptr;
    amount_t amount = 0;

    if (!init_bank(pc, clerk, TRUE))
    {
        return;
    }

    if (str_is_empty(s))
    {
        act("$1n says 'Deposit what, $3n?'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
        return;
    }

    if (next_word_is_number(s))
    {
        char buf[MAX_INPUT_LENGTH];

        s = str_next_word(s, buf);
        if ((amount = atoi(buf)) < 1)
        {
            act("$1n coughs and says 'That's a silly thing to do, $3n...'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
            return;
        }
    }

    if ((thing = find_unit(pc, &s, nullptr, FIND_UNIT_INVEN)) == nullptr)
    {
        act("$1n sighs and says 'I can't deposit what you haven't got, $3n...'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
    }
    else if (!IS_MONEY(thing))
    {
        act("$1n says 'I won't store anything but money for you, $3n...'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
    }
    else
    {
        currency_t cur = MONEY_CURRENCY(thing);
        amount_t old_balance = 0;

        if (amount == 0)
        {
            amount = MONEY_AMOUNT(thing);
        }
        else if (MONEY_AMOUNT(thing) < amount)
        {
            act("$1n says 'Credit on deposits, $3n?  What a preposterous thought!'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
            return;
        }

        thing = split_money(thing, amount);

        old_balance = balance[cur];
        balance[cur] += (INTEREST * MONEY_VALUE(thing));

        if (balance[cur] < old_balance) /* overflow! */
        {
            balance[cur] = old_balance;
            act("$3n shrugs and says 'Uhm, your account is full?'", A_SOMEONE, pc, cActParameter(), clerk, TO_CHAR);
            pile_money(thing);
        }
        else
        {
            act("$1n hands over some $2t to $3n.", A_HIDEINV, pc, money_pluralis(thing), clerk, TO_NOTVICT);
            act("You hand $3n your $2t.", A_SOMEONE, pc, money_pluralis(thing), clerk, TO_CHAR);
            act("$3n whispers to you 'I'll take $2d% interest.'", A_SOMEONE, pc, &INT_INTEREST, clerk, TO_CHAR);

            changed_balance = TRUE;
            unit_from_unit(thing);
            extract_unit(thing);
        }
    }
}

static void cmd_exchange(const unit_data *pc, unit_data *clerk, char *s)
{
    unit_data *thing = nullptr;
    currency_t cur = 0;
    amount_t amount = 0;

    if (!init_bank(pc, clerk, FALSE))
    {
        return;
    }

    if (str_is_empty(s))
    {
        act("$1n says 'Exchange what, $3n?'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
        return;
    }

    if (next_word_is_number(s))
    {
        char buf[MAX_INPUT_LENGTH];

        s = str_next_word(s, buf);
        if ((amount = atoi(buf)) < 1)
        {
            act("$1n coughs and says 'That's a silly thing to do, $3n...'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
            return;
        }
    }

    if ((thing = find_unit(pc, &s, nullptr, FIND_UNIT_INVEN)) == nullptr)
    {
        act("$1n sighs and says 'I can't exchange what you haven't got, $3n...'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
        return;
    }
    else if (!IS_MONEY(thing))
    {
        act("$1n says 'I only deal in money, $3n!'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
        return;
    }

    if (amount == 0)
    {
        amount = MONEY_AMOUNT(thing);
    }
    else if (MONEY_AMOUNT(thing) < amount)
    {
        act("$1n says 'No credit, $3n!  What cheek!'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
        return;
    }

    thing = split_money(thing, amount);

    cur = MONEY_CURRENCY(thing);
    amount = (INTEREST * MONEY_VALUE(thing));

    unit_from_unit(thing);

    if (str_is_empty(s))
    {
        act("$1n exchanges some $2t with $3n.", A_HIDEINV, pc, money_pluralis(thing), clerk, TO_NOTVICT);
        act("You exchange your $2t with $3n.", A_SOMEONE, pc, money_pluralis(thing), clerk, TO_CHAR);
        act("$3n whispers to you 'I'll take $2d% interest.'", A_SOMEONE, pc, &INT_INTEREST, clerk, TO_CHAR);
        act("You get $2t back.", A_SOMEONE, pc, money_string(amount, cur, TRUE), cActParameter(), TO_CHAR);

        extract_unit(thing);
        money_to_unit((unit_data *)pc, amount, cur);
    }
    else if ((s = str_ccmp_next_word(s, "to")))
    {
        int tmp = 0;
        int remainder = 0;
        int i = 0;

        for (i = 0; i <= MAX_MONEY; ++i)
        {
            if (is_name(s, g_money_types[i].strings))
            {
                cur = g_money_types[i].currency;
                break;
            }
        }

        if (i > MAX_MONEY)
        {
            act("$1n shrugs and says 'Never heard of that one before, $3n.'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
            unit_to_unit(thing, (unit_data *)pc);
            return;
        }

        tmp = amount / g_money_types[i].relative_value;
        remainder = amount - tmp * g_money_types[i].relative_value;

        act("$1n exchanges some $2t with $3n.", A_HIDEINV, pc, money_pluralis(thing), clerk, TO_NOTVICT);
        act("You exchange your $2t with $3n.", A_SOMEONE, pc, money_pluralis(thing), clerk, TO_CHAR);
        act("$3n whispers to you 'I'll take $2d% interest.'", A_SOMEONE, pc, &INT_INTEREST, clerk, TO_CHAR);

        extract_unit(thing);

        if (tmp > 0)
        {
            coins_to_unit((unit_data *)pc, tmp, i);
            act("You get $2d $3t back.", A_SOMEONE, pc, &tmp, tmp == 1 ? money_singularis_type(i) : money_pluralis_type(i), TO_CHAR);
        }
        if (remainder > 0)
        {
            money_to_unit((unit_data *)pc, remainder, cur);
            act(tmp > 0 ? "...and $2t in change." : "You get $2t back.",
                A_SOMEONE,
                pc,
                money_string(remainder, cur, TRUE),
                cActParameter(),
                TO_CHAR);
        }
    }
    else
    {
        act("$1n says 'Exchange to what, $3n?'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
        unit_to_unit(thing, (unit_data *)pc);
        return;
    }
}

static void cmd_withdraw(const unit_data *pc, unit_data *clerk, char *s)
{
    currency_t cur = 0;
    amount_t amount = 0;
    int i = 0;

    if (!init_bank(pc, clerk, TRUE))
    {
        return;
    }

    if (str_is_empty(s))
    {
        act("$1n says 'Withdraw what, $3n?'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
        return;
    }

    if (next_word_is_number(s))
    {
        char buf[MAX_INPUT_LENGTH];

        s = str_next_word(s, buf);
        if ((amount = atoi(buf)) < 1)
        {
            act("$1n coughs and says 'That's a silly thing to do, $3n...'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
            return;
        }
    }
    else
    {
        act("$1n says 'You have to specify how much to withdraw, $3n.'", A_SOMEONE, clerk, s, pc, TO_VICT);
        return;
    }

    for (i = 0; i <= MAX_MONEY; ++i)
    {
        if (0 < balance[g_money_types[i].currency] && is_name(s, g_money_types[i].strings))
        {
            cur = g_money_types[i].currency;
            break;
        }
    }

    if (i > MAX_MONEY)
    {
        act("$1n shrugs and says 'I'm storing nothing of the sort for you, $3n.'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
    }
    else if ((balance[cur] < amount * g_money_types[i].relative_value) || (amount * g_money_types[i].relative_value < 0))
    {
        act("$1n shakes $1s head and says 'No loans, $3n.'", A_SOMEONE, clerk, cActParameter(), pc, TO_VICT);
    }
    else
    {
        balance[cur] -= amount * g_money_types[i].relative_value;
        changed_balance = TRUE;

        coins_to_unit((unit_data *)pc, amount, i);

        act("$3n hands $1n some $2t.", A_SOMEONE, pc, money_pluralis_type(i), clerk, TO_ROOM);
        act("$3n hands you your $2t.", A_SOMEONE, pc, money_pluralis_type(i), clerk, TO_CHAR);
    }
}

int bank(spec_arg *sarg)
{
    if (sarg->activator == nullptr)
    {
        return SFR_SHARE;
    }

    changed_balance = FALSE;

    if (is_command(sarg->cmd, "balance"))
    {
        cmd_balance(sarg->activator, sarg->owner, (char *)sarg->arg);
    }
    else if (is_command(sarg->cmd, "deposit"))
    {
        cmd_deposit(sarg->activator, sarg->owner, (char *)sarg->arg);
    }
    else if (is_command(sarg->cmd, "exchange"))
    {
        cmd_exchange(sarg->activator, sarg->owner, (char *)sarg->arg);
    }
    else if (is_command(sarg->cmd, "withdraw"))
    {
        cmd_withdraw(sarg->activator, sarg->owner, (char *)sarg->arg);
    }
    else
    {
        return SFR_SHARE;
    }

    if (changed_balance)
    {
        char buf[MAX_STRING_LENGTH];
        char *b = nullptr;
        int i = 0;

        if (PC_BANK(sarg->activator))
            FREE(PC_BANK(sarg->activator));

        for (i = 0, *buf = '\0', b = buf; i <= MAX_CURRENCY; ++i)
        {
            sprintf(b, "~%d %ld", i, (long)balance[i]);
            TAIL(b);
        }

        PC_BANK(sarg->activator) = str_dup(buf);
    }

    return SFR_BLOCK;
}

static bool move_money_up(unit_data *ch, unit_data *u)
{
    unit_data *tmp = nullptr;
    unit_data *next = nullptr;
    bool found = FALSE;

    for (tmp = UNIT_CONTAINS(u); tmp; tmp = next)
    {
        next = tmp->next;

        if (IS_MONEY(tmp))
        {
            while (UNIT_IN(tmp) != ch)
            {
                found = TRUE;
                unit_up(tmp);
            }
        }
        else if ((IS_OBJ(tmp) && OBJ_TYPE(tmp) == ITEM_CONTAINER) || IS_ROOM(tmp) || IS_CHAR(tmp))
        {
            found = found || move_money_up(ch, tmp);
        }
    }

    return found;
}

void tax_player(unit_data *ch)
{
    amount_t limit = 50 * PLATINUM_MULT;
    descriptor_data *d = CHAR_DESCRIPTOR(ch);

    amount_t holds = 0;
    amount_t holds_sum = 0;

    char buf[MAX_STRING_LENGTH];
    char *b = nullptr;
    bool tmp_bool = FALSE;
    int i = 0;

    *(b = buf) = '\0';

    CHAR_DESCRIPTOR(ch) = nullptr; /* To avoid getting text output to the player */

    init_bank(ch, nullptr, TRUE);

    if (PC_BANK(ch))
    {
        FREE(PC_BANK(ch)); /* clear the bank... */
        PC_BANK(ch) = nullptr;
    }

    for (i = 0; i <= MAX_CURRENCY; ++i)
    {
        if (balance[i])
        {
            tmp_bool = TRUE;
            money_to_unit(ch, balance[i], i);
        }
    }

    if (tmp_bool)
    {
        strcpy(b,
               "The holdings of your bank account was moved into your"
               " purse.<br/>");
        TAIL(b);
    }

    if (move_money_up(ch, ch))
    {
        strcpy(b, "All money hidden in bags etc was moved into your purse.<br/>");
        TAIL(b);
    }

    /* Now all ch's money is in his inventory */

    tmp_bool = FALSE;

    /* Loop because the holds check may overflow... */
    for (i = 0; i <= MAX_CURRENCY; ++i)
    {
        holds_sum = 0;

        while ((holds = char_holds_amount(ch, i)) > limit)
        {
            holds_sum += holds;

            tmp_bool = TRUE;

            money_from_unit(ch, holds - limit, i);
            sprintf(b, "You were taxed the equivalent of %s.<br/>", money_string(holds - limit, DEF_CURRENCY, TRUE));
            TAIL(b);
        }

        limit -= (holds_sum + holds);
    }

    CHAR_DESCRIPTOR(ch) = d;

    if (b != buf)
    {
        if (tmp_bool)
        {
            act("Due to the new money limits imposed on the game, you have been"
                " subjected to a taxing, as reported below.  You will have to "
                "believe that, what is good for the game, is indeed good for you."
                "<br/>$2t",
                A_ALWAYS,
                ch,
                buf,
                cActParameter(),
                TO_CHAR);
        }
        else
        {
            act("Your money has has moved around a bit, but you weren't taxed."
                "  Congratulations!<br/>$2t",
                A_ALWAYS,
                ch,
                buf,
                cActParameter(),
                TO_CHAR);
        }
    }
}

void stat_bank(const unit_data *ch, unit_data *u)
{
    bool none = TRUE;
    int i = 0;

    if (!IS_PC(u))
    {
        send_to_char("You can only get bank info for PC's<br/>", ch);
        return;
    }

    init_bank(u, nullptr, TRUE);

    act("$2n has a bank-deposit of:", A_ALWAYS, ch, u, cActParameter(), TO_CHAR);

    for (i = 0; i <= MAX_CURRENCY; ++i)
    {
        if (balance[i])
        {
            none = FALSE;

            act("  $2t", A_ALWAYS, ch, money_string(balance[i], i, FALSE), cActParameter(), TO_CHAR);
        }
    }

    if (none)
    {
        send_to_char("  Nothing!<br/>", ch);
    }
}
