/*
 $Author: All $
 $RCSfile: tif_affect.cpp,v $
 $Date: 2004/12/12 18:22:49 $
 $Revision: 2.5 $
 */

#include "affect.h"
#include "comm.h"
#include "dilinst.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "magic.h"
#include "modify.h"
#include "pcsave.h"
#include "skills.h"
#include "slog.h"
#include "unit_affected_type.h"
#include "utils.h"
#include "vmelimits.h"

#include <cstdlib>
#include <cstring>

void tif_confusion_tick(unit_affected_type *af, unit_data *unit)
{
    unit_fptr *fptr = nullptr;

    if ((fptr = find_fptr(unit, SFUN_HUNTING)))
    {
        send_to_char("You feel confused.<br/>", unit);
        destroy_fptr(unit, fptr);
    }
}

void tif_confusion_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel confused.<br/>", unit);
    act("$1n seems confused.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
    tif_confusion_tick(af, unit);
}

void tif_confusion_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You less confused.<br/>", unit);
    act("$1n seem less confused.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
}

void tif_invisibility_on(unit_affected_type *af, unit_data *unit)
{
    if (!IS_SET(unit->getUnitFlags(), UNIT_FL_INVISIBLE))
    {
        send_to_char("Your body appears ghostly.<br/>", unit);
        act("$1n vanish into thin air.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
    }
}

void tif_invisibility_off(unit_affected_type *af, unit_data *unit)
{
    if (!IS_SET(unit->getUnitFlags(), UNIT_FL_INVISIBLE))
    {
        send_to_char("Your body is once again visible.<br/>", unit);
        act("$1n appears from thin air.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
    }
}

void tif_fear_check(unit_affected_type *af, unit_data *unit)
{
    unit_data *ch = nullptr;

    if (CHAR_FIGHTING(unit))
    {
        switch (number(0, 2))
        {
            case 0:
                act("You are about to die!!!", eA_SOMEONE, unit, cActParameter(), cActParameter(), eTO_CHAR);
                command_interpreter(unit, "flee");
                /* He could be dead now! */
                return;

            case 1:
                act("That really did HURT!", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("You wish your wounds would stop BLEEDING that much!", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        }
        return;
    }
    else
    {
        /* Find someone else */
        for (ch = unit->getUnitIn()->getUnitContains(); ch; ch = ch->getNext())
        {
            if (ch != unit && ch->isChar())
            {
                break;
            }
        }

        if (ch)
        {
            switch (number(0, 1))
            {
                case 0:
                    act("$3n prepares to kill you.", eA_SOMEONE, unit, cActParameter(), ch, eTO_CHAR);
                    act("$1n looks at you in a strange way.", eA_SOMEONE, unit, cActParameter(), ch, eTO_VICT);
                    act("$1n seems paranoid, looking at $3n.", eA_SOMEONE, unit, cActParameter(), ch, eTO_NOTVICT);
                    break;
                case 1:
                    act("$3n wounds you fatally with $3s sword.", eA_SOMEONE, unit, cActParameter(), ch, eTO_CHAR);
                    act("$1n screams in agony.", eA_SOMEONE, unit, cActParameter(), ch, eTO_ROOM);
                    break;
            }
        }
        else
        {
            switch (number(0, 2))
            {
                case 0:
                    act("Someone tries to steal your weapon!", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                    break;
                case 1:
                    act("Someone grins evilly.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                    break;
                case 2:
                    act("The huge green dragon appears in a puff of smoke!", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
            }
        }
    }
}

void tif_blind_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("A cloak of darkness falls around you.<br/>", unit);
}

void tif_blind_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("The veil of darkness disappears.<br/>", unit);
}

/* sneak */
void tif_sneak_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("Ok, you'll try to move silently for a while.<br/>", unit);
}

void tif_sneak_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You stop sneaking about.<br/>", unit);
}

void tif_sneak_tick(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You sneak about unnoticed.<br/>", unit);
}

/* hide */
void tif_hide_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You try to hide yourself.<br/>", unit);
}

void tif_hide_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You stop hiding.<br/>", unit);
    if (af->getTickFI() == TIF_HIDE_TICK)
    {
        act("You suddenly notice that $1n is standing here.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
    }
}

void tif_hide_tick(unit_affected_type *af, unit_data *unit)
{
    if (!(af->getDataAtIndex(1)))
    {
        return;
    }

    auto value = af->getDataAtIndex(1);
    af->setDataAtIndex(1, value - 1);
    switch (value)
    {
        case 1:
            send_to_char("Someone is bound to look here sometime.<br/>", unit);
            break;
        case 2:
            send_to_char("You master the ways of concealing yourself.<br/>", unit);
            break;
        case 3:
            send_to_char("You skillfully use the shadows.<br/>", unit);
            break;
        case 4:
            send_to_char("Surely noone can see you here.<br/>", unit);
            break;
        case 5:
            send_to_char("You cleverly hide away.<br/>", unit);
    }
}

void tif_nohide_tick(unit_affected_type *af, unit_data *unit)
{
    if (!(af->getDataAtIndex(1)))
    {
        return;
    }

    auto value = af->getDataAtIndex(1);
    af->setDataAtIndex(1, value - 1);
    switch (value)
    {
        case 1:
            send_to_char("Someone is probably looking at you.<br/>", unit);
            break;
        case 2:
            send_to_char("Maybe you should practice this hide-thing.<br/>", unit);
            break;
        case 3:
            send_to_char("You skillfully avoid the shadows.<br/>", unit);
            break;
        case 4:
            send_to_char("Surely you can see noone here?<br/>", unit);
            break;
        case 5:
            send_to_char("You cleverly hide away.<br/>", unit);
    }
}

/* bless */
void tif_bless_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel divine forces helping you.<br/>", unit);
}

void tif_bless_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel divine forces abandoning you.<br/>", unit);
}

void tif_bless_tick(unit_affected_type *af, unit_data *unit)
{
    if (af->getDuration() == 1)
    {
        send_to_char("You sense the divine forces about to move away.<br/>", unit);
    }
}

/* berserk */
void tif_berserk_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You suddenly go berserk!<br/>", unit);
}

void tif_berserk_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You no longer feel berserk.<br/>", unit);
}

void tif_berserk_tick(unit_affected_type *af, unit_data *unit)
{
    if (af->getDuration() == 1)
    {
        send_to_char("Your sense of being berserk is ebbing.<br/>", unit);
    }
}

/* rage */
void tif_rage_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel enraged!<br/>", unit);
}

void tif_rage_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel your rage subsiding.<br/>", unit);
}

void tif_rage_tick(unit_affected_type *af, unit_data *unit)
{
    if (af->getDuration() == 1)
    {
        send_to_char("You sense your rage starting to ebb.<br/>", unit);
    }
}

/* curse */
void tif_curse_on(unit_affected_type *af, unit_data *unit)
{
    if (unit->isChar())
    {
        send_to_char("You feel that the gods are against you.<br/>", unit);
    }
    act("A shadow falls upon $1n.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
    if (unit->getUnitIn() && unit->getUnitIn()->isChar())
    {
        act("A shadow falls upon $3n.", eA_HIDEINV, unit->getUnitIn(), cActParameter(), unit, eTO_CHAR);
    }
}

void tif_curse_off(unit_affected_type *af, unit_data *unit)
{
    if (unit->isChar())
    {
        send_to_char("You no longer feel that the gods are against you.<br/>", unit);
    }

    act("A shadow lifts from $1n.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
    if (unit->getUnitIn() && unit->getUnitIn()->isChar())
    {
        act("A shadow lifts from $3n.", eA_HIDEINV, unit->getUnitIn(), cActParameter(), unit, eTO_CHAR);
    }
}

/* sanctuary */
void tif_sanctuary_on(unit_affected_type *af, unit_data *unit)
{
    act("$1n momentarily glows in a bright white light.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
    send_to_char("You momentarily glow in a bright white light.<br/>", unit);
}

void tif_sanctuary_off(unit_affected_type *af, unit_data *unit)
{
    act("$1n glows in a bright white light. Then it fades away.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
    send_to_char("You glow in a bright white light. "
                 "Then it fades away.<br/>",
                 unit);
}

void tif_sanctuary_tick(unit_affected_type *af, unit_data *unit)
{
    if (af->getDuration() == 1)
    {
        act("$1n glows in a bright white light... The light flickers.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
        send_to_char("You glow in a bright white light... "
                     "The light flickers.<br/>",
                     unit);
    }
}

void tif_eyes_tingle(unit_affected_type *af, unit_data *unit)
{
    send_to_char("Your eyes begin to tingle.<br/>", unit);
}

void tif_torch_tick(unit_affected_type *af, unit_data *unit)
{
    if (OBJ_VALUE(unit, 0) > 0)
    {
        UOBJ(unit)->setValueAtIndexTo(0, OBJ_VALUE(unit, 0) - 1); // Decreash how many hours of burning are left.
    }

    if (af->getDuration() <= 4)
    {
        if (unit->getUnitIn()->isChar())
        {
            if (af->getDuration() <= 1) // Last tick.
            {
                act("Your $2N goes out.", eA_HIDEINV, unit->getUnitIn(), unit, cActParameter(), eTO_CHAR);
            }
            else
            {
                act("Your $2N is getting dim.", eA_HIDEINV, unit->getUnitIn(), unit, cActParameter(), eTO_CHAR);
            }
        }
    }
}

void tif_light_add(unit_affected_type *af, unit_data *unit)
{
    if (!unit->getUnitIn())
    {
        return;
    }

    /* If the thing is carried by a character */
    if (unit->getUnitIn()->isChar())
    {
        act("Your $3N starts to glow.", eA_HIDEINV, unit->getUnitIn(), cActParameter(), unit, eTO_CHAR);
        act("$1n's $3n starts to glow.", eA_HIDEINV, unit->getUnitIn(), cActParameter(), unit, eTO_ROOM);
    }
    else if (unit->getUnitIn()->getUnitContains())
    {
        act("The $3N starts to glow.", eA_HIDEINV, unit->getUnitIn()->getUnitContains(), cActParameter(), unit, eTO_ALL);
    }
}

void tif_light_sub(unit_affected_type *af, unit_data *unit)
{
    if (!unit->getUnitIn())
    {
        return;
    }

    /* If the thing is carried by a character */
    if (unit->getUnitIn()->isChar())
    {
        act("Your $3N gets dimmer.", eA_HIDEINV, unit->getUnitIn(), cActParameter(), unit, eTO_CHAR);
        act("$1n's $3N gets dimmer.", eA_HIDEINV, unit->getUnitIn(), cActParameter(), unit, eTO_ROOM);
    }
    else if (unit->getUnitIn()->getUnitContains())
    {
        act("The $3N gets dimmer.", eA_HIDEINV, unit->getUnitIn()->getUnitContains(), cActParameter(), unit, eTO_ALL);
    }
}

void tif_sleep_on(unit_affected_type *af, unit_data *unit)
{
    act("You feel very tired.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    if (CHAR_POS(unit) > POSITION_SLEEPING)
    {
        if (CHAR_FIGHTING(unit))
        {
            stop_fighting(unit);
        }

        act("You fall asleep.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
        act("$1n falls asleep.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_ROOM);
        UCHAR(unit)->setPosition(POSITION_SLEEPING);
    }
}

void tif_sleep_check(unit_affected_type *af, unit_data *unit)
{
    int hm = 0;

    if (CHAR_POS(unit) > POSITION_SLEEPING)
    {
        hm = resistance_skill_check(af->getDataAtIndex(0),
                                    spell_ability(unit, ABIL_BRA, SPL_SLEEP),
                                    af->getDataAtIndex(1),
                                    spell_defense_skill(unit, SPL_SLEEP));
        if (hm < 0)
        {
            if (CHAR_FIGHTING(unit))
            {
                stop_fighting(unit);
            }
            act("You fall asleep.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
            act("$1n falls asleep.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
            UCHAR(unit)->setPosition(POSITION_SLEEPING);
        }
    }
}

void tif_sleep_off(unit_affected_type *af, unit_data *unit)
{
    act("You feel less sleepy.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    /* no no not a 'wake' here, remember he's still affacted */
}

void tif_protect_on(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel protected.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
    else
    {
        act("You feel less protected.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
}

void tif_protect_off(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel less protected.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
    else
    {
        act("You feel protected.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
}

void tif_hit_on(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel more healthy.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
    else
    {
        act("You feel less healthy.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
}

void tif_hit_off(unit_affected_type *af, unit_data *unit)
{
    af->setDataAtIndex(1, -af->getDataAtIndex(1));
    tif_hit_on(af, unit);
    af->setDataAtIndex(1, -af->getDataAtIndex(1));
}

void tif_mag_on(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel more powerful.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
    else
    {
        act("You feel less powerful.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
}

void tif_mag_off(unit_affected_type *af, unit_data *unit)
{
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
    tif_mag_on(af, unit);
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
}

void tif_div_on(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel closer to your god.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
    else
    {
        act("You feel parted from your god.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    }
}

void tif_div_off(unit_affected_type *af, unit_data *unit)
{
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
    tif_div_on(af, unit);
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
}

void tif_str_on(unit_affected_type *af, unit_data *unit)
{
    /*old stuff
       if (af->data[1] > 0)
         act ("You feel stronger.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
       else
         act ("You feel weaker.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);

     */

    /*Darg new str*/
    if (af->getDataAtIndex(1) > 0)
    {
        switch (1 + (int)(5.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You feel stronger.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("You feel more fit.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("You feel more buff.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("You feel able to take on the world.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("The power of Odin courses through your body!", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }
    else
    {
        switch (1 + (int)(2.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You feel weaker.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("Your arms start to feel heavier.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("The power of odin leaves you.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("You feel like you were just run over with a donkey cart!", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("You feel a unhealthy.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }

    /*end new str*/
}

void tif_str_off(unit_affected_type *af, unit_data *unit)
{
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
    tif_str_on(af, unit);
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
}

void tif_dex_on(unit_affected_type *af, unit_data *unit)
{
    /* old dex stuff
       if (af->data[1] > 0)
         act ("You feel dexterous.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
       else
         act ("You feel a little clumsy.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
     */
    /*Darg new dex*/
    if (af->getDataAtIndex(1) > 0)
    {
        switch (1 + (int)(5.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You feel dexterous.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("You feel nimble.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("You feel a bit more agile.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("You feel a bit more coordinated.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("You feel a bit slick.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }
    else
    {
        switch (1 + (int)(5.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You feel a little clumsy.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("You feel awkward.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("You feel a tad uncoordinated.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("You feel a bit more dim-witted.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("You feel a bumble footed.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }

    /*end new dex*/
}

void tif_dex_off(unit_affected_type *af, unit_data *unit)
{
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
    tif_dex_on(af, unit);
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
}

void tif_con_on(unit_affected_type *af, unit_data *unit)
{
    /* old con stuff
       if (af->data[1] > 0)
         act ("You feel more robust.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
       else
         act ("You feel less robust.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
     */
    /*Darg new con*/
    if (af->getDataAtIndex(1) > 0)
    {
        switch (1 + (int)(5.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You feel more robust.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("You feel a shift for the better in your disposition.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("You sense an increase in your persona .", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("You gain confidence in your stride.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("You feel concentrated on the task at hand.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }
    else
    {
        switch (1 + (int)(5.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You feel less robust.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("Your attitude slides downward.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("You feel winded.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("You lose confidence in yourself.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("You lose focus on the task at hand.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }

    /*end new con*/
}

void tif_con_off(unit_affected_type *af, unit_data *unit)
{
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
    tif_con_on(af, unit);
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
}

void tif_cha_on(unit_affected_type *af, unit_data *unit)
{
    /* old cha stuff
       if (af->data[1] > 0)
         act ("You feel more authoritative.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
       else
         act ("You feel less authoritative.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
     */
    /*Darg new cha*/
    if (af->getDataAtIndex(1) > 0)
    {
        switch (1 + (int)(5.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You feel more appealing.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("You feel your animal magnetism kick in.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("You feel more seductive.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("You feel more authoritative.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("Your conjuration feels more in tune with nature.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }
    else
    {
        switch (1 + (int)(5.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You suddenly feel hateful.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("You feel repulsed.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("You feel disenchanted.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("A bored, hollow feeling fills you.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("You become disinterested in your well being.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }

    /*end new cha*/
}

void tif_cha_off(unit_affected_type *af, unit_data *unit)
{
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
    tif_cha_on(af, unit);
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
}

void tif_bra_on(unit_affected_type *af, unit_data *unit)
{
    /* old bra stuff
       if (af->data[1] > 0)
         act ("You feel smarter.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
       else
         act ("You feel a little dumb.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
     */
    /*Darg new bra*/
    if (af->getDataAtIndex(1) > 0)
    {
        switch (1 + (int)(5.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You feel more intellectual.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("You feel smarter.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("You feel aggressive.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("You feel a rush of intellect wash over you.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("You feel witty.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }
    else
    {
        switch (1 + (int)(5.0 * rand() / (RAND_MAX + 1.0)))
        {
            case 1:
                act("You feel dumb.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 2:
                act("You feel idiotic.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 3:
                act("You feel more imbecillic.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 4:
                act("A lesser medulla oblongata fills your grey matter.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
            case 5:
                act("You suddenly realize you're a moron.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
                break;
        };
    }

    /*end new cha*/
}

void tif_bra_off(unit_affected_type *af, unit_data *unit)
{
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
    tif_bra_on(af, unit);
    af->setDataAtIndex(1, af->getDataAtIndex(1) * -1);
}

void tif_poison_on(unit_affected_type *af, unit_data *unit)
{
    act("You feel very ill.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    act("$1n seems very ill.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_ROOM);
}

/* Data[0] The amount of hitpoints to loose (>=0)        */
/* Data[1] The amount of Mana points to loose (>=0)      */
/* Data[2] The amount of Endurance points to loose (>=0) */
void tif_poison_suffer(unit_affected_type *af, unit_data *unit)
{
    UCHAR(unit)->decreaseManaBy(af->getDataAtIndex(1));
    UCHAR(unit)->decreaseEnduranceBy(af->getDataAtIndex(2));
    damage(unit, unit, nullptr, af->getDataAtIndex(0), MSG_TYPE_OTHER, MSG_OTHER_POISON, COM_MSG_EBODY);
    /* unit can be destructed now, but no problemo */
}

void tif_poison_off(unit_affected_type *af, unit_data *unit)
{
    act("You feel better.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
}

/* plague */
void tif_plague_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You suddenly feel very sick.<br/>", unit);
}

void tif_plague_tick(unit_affected_type *af, unit_data *unit)
{
}

void tif_plague_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel much better.<br/>", unit);
}

/* insanity */
void tif_insanity_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel a battle with your own mind begins.<br/>", unit);
    act("A mad look appears on $1n's face", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
}

void tif_insanity_tick(unit_affected_type *af, unit_data *unit)
{
}

void tif_insanity_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel the battle with your own mind is over.<br/>", unit);
    act("The mad look disappears from $1n's face.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
}

void tif_prot_evil_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel protected from the forces of evil.<br/>", unit);
}

void tif_prot_good_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel protected from the forces of good.<br/>", unit);
}

void tif_prot_evil_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel less protected from the forces of evil.<br/>", unit);
}

void tif_prot_good_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel less protected from the forces of good.<br/>", unit);
}

void tif_sustain_on(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You feel sustained.<br/>", unit);
}

void tif_sustain_tick(unit_affected_type *af, unit_data *unit)
{
    if (!unit->isPC())
    {
        return;
    }
    UPC(unit)->setConditionAtIndexTo(DRUNK, af->getDataAtIndex(0));
    UPC(unit)->setConditionAtIndexTo(FULL, af->getDataAtIndex(1));
    UPC(unit)->setConditionAtIndexTo(THIRST, af->getDataAtIndex(2));
}

void tif_sustain_off(unit_affected_type *af, unit_data *unit)
{
    send_to_char("You no longer feel sustained.<br/>", unit);
}

void tif_decay_corpse(unit_affected_type *af, unit_data *unit)
{
    /* Make routine to change the description of a corpse instead */
    if (ODD(af->getDuration()) && !IS_SET(unit->getUnitFlags(), UNIT_FL_BURIED))
    {
        act("The rotten stench of $1n is here.", eA_SOMEONE, unit, cActParameter(), cActParameter(), eTO_ROOM);
    }
}

void tif_destroy_corpse(unit_affected_type *af, unit_data *unit)
{
    if (!IS_SET(unit->getUnitFlags(), UNIT_FL_BURIED))
    {
        act("A quivering horde of maggots consume $1n.", eA_SOMEONE, unit, cActParameter(), cActParameter(), eTO_ROOM);
    }
    extract_unit(unit);
}

void tif_buried_destruct(unit_affected_type *af, unit_data *unit)
{
    if (IS_SET(unit->getUnitFlags(), UNIT_FL_BURIED))
    {
        /* Empty the container and set buried status of contents */

        while (unit->getUnitContains())
        {
            unit_affected_type naf;

            unit->getUnitContains()->setUnitFlag(UNIT_FL_BURIED);

            naf.setID(ID_BURIED);
            naf.setDuration(0);
            naf.setBeat(WAIT_SEC * SECS_PER_REAL_HOUR);
            naf.setFirstFI(TIF_NONE);
            naf.setTickFI(TIF_NONE);
            naf.setLastFI(TIF_BURIED_DESTRUCT);
            naf.setApplyFI(APF_NONE);

            create_affect(unit->getUnitContains(), &naf);

            if (UNIT_IS_EQUIPPED(unit->getUnitContains()))
            {
                unequip_object(unit->getUnitContains());
            }

            unit_up(unit->getUnitContains());
        }

        extract_unit(unit);
    }
}

void tif_valhalla_ret(unit_affected_type *af, unit_data *unit)
{
    if (!unit->isPC())
    {
        return;
    }

    UCHAR(unit)->setPosition(POSITION_STANDING);
    UPC(unit)->removePCFlag(PC_SPIRIT);
    UCHAR(unit)->removeCharacterFlag(CHAR_KILL_SELF);

    act("You have a strange feeling...", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
    act("$1n materializes and vanish.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);

    unit_from_unit(unit);
    unit_to_unit(unit, hometown_unit(UPC(unit)->getHometown()));

    act("The spirit of $1n materializes.", eA_HIDEINV, unit, cActParameter(), cActParameter(), eTO_ROOM);
    act("You feel dizzy.", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);

    UPC(unit)->setConditionAtIndexTo(FULL, 24);
    UPC(unit)->setConditionAtIndexTo(THIRST, 24);
    UPC(unit)->setConditionAtIndexTo(DRUNK, 0);

    UCHAR(unit)->setMana(mana_limit(unit));
    UCHAR(unit)->setEndurance(move_limit(unit));
    unit->setCurrentHitpoints(unit->getMaximumHitpoints());

    if (!unit->is_destructed())
    {
        save_player(unit);
        save_player_contents(unit, TRUE);
    }
}

void tif_jail_wait(unit_affected_type *af, unit_data *unit)
{
}

/* Get thrown out of jail */
void tif_jail_release(unit_affected_type *af, unit_data *unit)
{
}

void tif_spl_on(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel more skilled at $2t.", eA_ALWAYS, unit, g_SplColl.text[af->getDataAtIndex(0)], cActParameter(), eTO_CHAR);
    }
    else
    {
        act("You feel less skilled at $2t.", eA_ALWAYS, unit, g_SplColl.text[af->getDataAtIndex(0)], cActParameter(), eTO_CHAR);
    }
}

void tif_spl_off(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel less skilled at $2t.", eA_ALWAYS, unit, g_SplColl.text[af->getDataAtIndex(0)], cActParameter(), eTO_CHAR);
    }
    else
    {
        act("You feel more skilled at $2t.", eA_ALWAYS, unit, g_SplColl.text[af->getDataAtIndex(0)], cActParameter(), eTO_CHAR);
    }
}

void tif_ski_on(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel more skilled in $2t.", eA_ALWAYS, unit, g_SkiColl.text[af->getDataAtIndex(0)], cActParameter(), eTO_CHAR);
    }
    else
    {
        act("You feel less skilled in $2t.", eA_ALWAYS, unit, g_SkiColl.text[af->getDataAtIndex(0)], cActParameter(), eTO_CHAR);
    }
}

void tif_ski_off(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel less skilled in $2t.", eA_ALWAYS, unit, g_SkiColl.text[af->getDataAtIndex(0)], cActParameter(), eTO_CHAR);
    }
    else
    {
        act("You feel more skilled in $2t.", eA_ALWAYS, unit, g_SkiColl.text[af->getDataAtIndex(0)], cActParameter(), eTO_CHAR);
    }
}

void tif_wpn_on(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel more skilled at the $2t fighting style.",
            eA_ALWAYS,
            unit,
            g_WpnColl.text[af->getDataAtIndex(0)],
            cActParameter(),
            eTO_CHAR);
    }
    else
    {
        act("You feel less skilled at the $2t fighting style.",
            eA_ALWAYS,
            unit,
            g_WpnColl.text[af->getDataAtIndex(0)],
            cActParameter(),
            eTO_CHAR);
    }
}

void tif_wpn_off(unit_affected_type *af, unit_data *unit)
{
    if (af->getDataAtIndex(1) > 0)
    {
        act("You feel less skilled at the $2t fighting style.",
            eA_ALWAYS,
            unit,
            g_WpnColl.text[af->getDataAtIndex(0)],
            cActParameter(),
            eTO_CHAR);
    }
    else
    {
        act("You feel more skilled at the $2t fighting style.",
            eA_ALWAYS,
            unit,
            g_WpnColl.text[af->getDataAtIndex(0)],
            cActParameter(),
            eTO_CHAR);
    }
}

void tif_armour_on(unit_affected_type *af, unit_data *unit)
{
    const char *c = "back to normal skin.";

    switch (CHAR_NATURAL_ARMOUR(unit))
    {
        case ARM_LEATHER:
            c = "into a dark leather like hide.";
            break;

        case ARM_HLEATHER:
            c = "into a dark bark like hide.";
            break;

        case ARM_CHAIN:
            c = "into an ebony coloured bone like shell.";
            break;

        case ARM_PLATE:
            c = "into a grey stone like shell.";
            break;

        default:
            af->setDataAtIndex(0, ARM_CLOTHES);
            break;
    }

    act("Your skin transforms $2t", eA_ALWAYS, unit, c, cActParameter(), eTO_CHAR);

    act("$1n's skin transforms $2t", eA_ALWAYS, unit, c, cActParameter(), eTO_ROOM);
}

void tif_speed_on(unit_affected_type *af, unit_data *unit)
{
    act("You feel faster...", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
}

void tif_speed_off(unit_affected_type *af, unit_data *unit)
{
    act("You feel slower...", eA_ALWAYS, unit, cActParameter(), cActParameter(), eTO_CHAR);
}

/* --------------------------------------------------------------------- */

void tif_naught(unit_affected_type *af, unit_data *unit)
{
    slog(LOG_ALL, 0, "Obsoleted affect called with ID %d on %s.", af->getApplyFI(), unit->getFileIndexSymName());
}

tick_function_type g_tif[] = {{"Decay Corpse", tif_decay_corpse}, // 0
                              {"Destroy Corpse", tif_destroy_corpse},
                              {"Valhalla Return", tif_valhalla_ret},
                              {"Hitpoints On", tif_hit_on},
                              {"Hitpoints Off", tif_hit_off},
                              {"Magic Mod On", tif_mag_on},
                              {"Magic Mod Off", tif_mag_off},
                              {"Divine Mod On", tif_div_on},
                              {"Divine Mod Off", tif_div_off},
                              {"Strength Mod On", tif_str_on}, // 9
                              {"Strength Mod Off", tif_str_off},
                              {"Dexterity Mod On", tif_dex_on},
                              {"Dexterity Mod Off", tif_dex_off},
                              {"Constitution Mod On", tif_con_on},
                              {"Constitution Mod Off", tif_con_off},
                              {"Charisma Mod On", tif_cha_on}, // 15
                              {"Charisma Mod Off", tif_cha_off},
                              {"Brain Mod On", tif_bra_on},
                              {"Brain Mod Off", tif_bra_off},
                              {"XXX", tif_armour_on},
                              {"XXX", tif_armour_on}, // 20
                              {"Spell-skill Mod On", tif_protect_on},
                              {"Spell-skill Mod Off", tif_protect_off},
                              {"Poison On", tif_poison_on},
                              {"Poison Off", tif_poison_off},
                              {"Poison Suffer", tif_poison_suffer},        // 25
                              {"Light/Dark Add", tif_light_add},           // 26
                              {"Light/Dark Sub", tif_light_sub},           // 27
                              {"Torch burning countdown", tif_torch_tick}, // 28
                              {"Eyes tingles", tif_eyes_tingle},
                              {"Blind On", tif_blind_on},
                              {"Blind Off", tif_blind_off},
                              {"Fear Tick Check", tif_fear_check},
                              {"Invisibility On", tif_invisibility_on},
                              {"Invisibility Off", tif_invisibility_off},
                              {"Sneak on", tif_sneak_on},
                              {"Sneak off", tif_sneak_off},
                              {"Sneak tick", tif_sneak_tick},
                              {"Hide on", tif_hide_on},
                              {"Hide off", tif_hide_off},
                              {"Nide tick", tif_hide_tick},
                              {"Nohide tick", tif_nohide_tick},
                              {"sleep on", tif_sleep_on},
                              {"sleep check", tif_sleep_check},
                              {"sleep off", tif_sleep_off},
                              {"bless on", tif_bless_on},
                              {"bless tick", tif_bless_tick},
                              {"bless off", tif_bless_off},
                              {"curse on", tif_curse_on},
                              {"curse off", tif_curse_off},
                              {"sancruary on", tif_sanctuary_on},
                              {"sanctuary tick", tif_sanctuary_tick},
                              {"sanctuary off", tif_sanctuary_off},
                              {"insanity on", tif_insanity_on},
                              {"insanity tick", tif_insanity_tick},
                              {"insanity off", tif_insanity_off},
                              {"protect from evil on", tif_prot_evil_on},
                              {"protect from evil off", tif_prot_evil_off},
                              {"sustain on", tif_sustain_on},
                              {"sustain tick", tif_sustain_tick},
                              {"sustain off", tif_sustain_off},
                              {"jail waiting", tif_jail_wait},
                              {"jail release", tif_jail_release},
                              {"plague on", tif_plague_on},
                              {"plague tick", tif_plague_tick},
                              {"plague off", tif_plague_off},
                              {"obsoleted reward on", tif_naught},
                              {"obsoleted reward off", tif_naught},
                              {"spell on", tif_spl_on},
                              {"spell off", tif_spl_off},
                              {"skill on", tif_ski_on},
                              {"skill off", tif_ski_off},
                              {"weapon on", tif_wpn_on},
                              {"weapon off", tif_wpn_off},
                              {"confusion on", tif_confusion_on},
                              {"confusion off", tif_confusion_off},
                              {"confusion tick", tif_confusion_tick},
                              {"buried destruct", tif_buried_destruct},
                              {"protection from good on", tif_prot_good_on},
                              {"protection from good off", tif_prot_good_off},
                              {"natural armour on", tif_armour_on},
                              {"natural armour off", tif_armour_on},
                              {"speed better", tif_speed_on},
                              {"speed worse", tif_speed_off}};
