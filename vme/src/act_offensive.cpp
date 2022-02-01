/*
 $Author: tperry $
 $RCSfile: act_offensive.cpp,v $
 $Date: 2003/01/02 23:39:20 $
 $Revision: 2.3 $
 */

#include "affect.h"
#include "comm.h"
#include "db.h"
#include "dilrun.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"

#include <cstring>

/* extern variables */

void do_hit(unit_data *ch, char *argument, const command_info *cmd)
{
    unit_data *victim = nullptr;

    if (str_is_empty(argument))
    {
        act("Who do you want to hit?", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
        return;
    }

    victim = find_unit(ch, &argument, nullptr, FIND_UNIT_SURRO);

    if (!victim || !IS_CHAR(victim))
    {
        act("There is nobody here called $2t which you can hit.", A_ALWAYS, ch, argument, cActParameter(), TO_CHAR);
        return;
    }

    if (victim == ch)
    {
        send_to_char("You hit yourself... OUCH!.<br/>", ch);
        act("$1n hits $1mself, and says OUCH!", A_SOMEONE, ch, cActParameter(), victim, TO_ROOM);
    }
    else
    {
        if (pk_test(ch, victim, TRUE))
        {
            return;
        }

        if (!CHAR_FIGHTING(ch))
        {
            simple_one_hit(ch, victim);
        }
        else
        {
            send_to_char("You do the best you can!<br/>", ch);
        }
    }
}

void do_kill(unit_data *ch, char *argument, const command_info *cmd)
{
    unit_data *victim = nullptr;

    if (str_is_empty(argument))
    {
        act("Who do you want to kill?", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
        return;
    }

    if ((CHAR_LEVEL(ch) < ULTIMATE_LEVEL) || IS_NPC(ch))
    {
        do_hit(ch, argument, &g_cmd_auto_unknown);
        return;
    }

    victim = find_unit(ch, &argument, nullptr, FIND_UNIT_SURRO);

    if (!victim || !IS_CHAR(victim))
    {
        act("There is nobody here called $2t which you can kill.", A_ALWAYS, ch, argument, cActParameter(), TO_CHAR);
        return;
    }

    if (ch == victim)
    {
        send_to_char("Your mother would be so sad.. :(<br/>", ch);
    }
    else
    {
        act("You chop $3m to pieces! Ah! The blood!", A_SOMEONE, ch, cActParameter(), victim, TO_CHAR);
        act("$3n chops you to pieces!", A_SOMEONE, victim, cActParameter(), ch, TO_CHAR);
        act("$1n brutally slays $3n.", A_SOMEONE, ch, cActParameter(), victim, TO_NOTVICT);
        set_fighting(ch, victim, TRUE); /* Point to the killer! */
        SET_BIT(CHAR_FLAGS(ch), CHAR_KILL_SELF);

        diltemplate *death = nullptr;
        death = find_dil_template("death@death");
        if (death)
        {
            send_death(ch);
            dilprg *prg = dil_copy_template(death, victim, nullptr);

            if (prg)
            {
                prg->waitcmd = WAITCMD_MAXINST - 1;
                dil_activate(prg);
            }
            return;
        }
        die(victim);
    }
}
