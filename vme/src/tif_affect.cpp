/*
 $Author: All $
 $RCSfile: tif_affect.cpp,v $
 $Date: 2004/12/12 18:22:49 $
 $Revision: 2.5 $
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "structs.h"
#include "utils.h"
#include "handler.h"
#include "comm.h"
#include "spells.h"
#include "skills.h"
#include "affect.h"
#include "db.h"
#include "utility.h"
#include "interpreter.h"
#include "magic.h"
#include "money.h"
#include "vmelimits.h"
#include "fight.h"


void
tif_confusion_tick (class unit_affected_type *af, class unit_data * unit)
{
    class unit_fptr *fptr;

    if ((fptr = find_fptr (unit, SFUN_HUNTING)))
    {
        send_to_char ("You feel confused.<br/>", unit);
        destroy_fptr (unit, fptr);
    }
}

void
tif_confusion_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel confused.<br/>", unit);
    act ("$1n seems confused.", A_HIDEINV, unit, 0, 0, TO_ROOM);
    tif_confusion_tick (af, unit);
}

void
tif_confusion_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You less confused.<br/>", unit);
    act ("$1n seem less confused.", A_HIDEINV, unit, 0, 0, TO_ROOM);
}

void
tif_invisibility_on (class unit_affected_type *af, class unit_data * unit)
{
    if (!IS_SET (UNIT_FLAGS (unit), UNIT_FL_INVISIBLE))
    {
        send_to_char ("Your body appears ghostly.<br/>", unit);
        act ("$1n vanish into thin air.", A_HIDEINV, unit, 0, 0, TO_ROOM);
    }
}

void
tif_invisibility_off (class unit_affected_type *af, class unit_data * unit)
{
    if (!IS_SET (UNIT_FLAGS (unit), UNIT_FL_INVISIBLE))
    {
        send_to_char ("Your body is once again visible.<br/>", unit);
        act ("$1n appears from thin air.", A_HIDEINV, unit, 0, 0, TO_ROOM);
    }
}



void
tif_fear_check (class unit_affected_type *af, class unit_data * unit)
{
    class unit_data *ch;

    if (CHAR_FIGHTING (unit))
    {
        switch (number (0, 2))
        {
        case 0:
            act ("You are about to die!!!", A_SOMEONE, unit, 0, 0, TO_CHAR);
            command_interpreter(unit, "flee");
            /* He could be dead now! */
            return;

        case 1:
            act ("That really did HURT!", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("You wish your wounds would stop BLEEDING that much!",
                 A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        }
        return;
    }
    else
    {
        /* Find someone else */
        for (ch = UNIT_CONTAINS (UNIT_IN (unit)); ch; ch = ch->next)
            if (ch != unit && IS_CHAR (ch))
                break;

        if (ch)
            switch (number (0, 1))
            {
            case 0:
                act ("$3n prepares to kill you.", A_SOMEONE, unit, 0, ch,
                     TO_CHAR);
                act ("$1n looks at you in a strange way.", A_SOMEONE, unit, 0, ch,
                     TO_VICT);
                act ("$1n seems paranoid, looking at $3n.", A_SOMEONE, unit, 0,
                     ch, TO_NOTVICT);
                break;
            case 1:
                act ("$3n wounds you fatally with $3s sword.",
                     A_SOMEONE, unit, 0, ch, TO_CHAR);
                act ("$1n screams in agony.", A_SOMEONE, unit, 0, ch, TO_ROOM);
                break;
            }
        else
            switch (number (0, 2))
            {
            case 0:
                act ("Someone tries to steal your weapon!",
                     A_ALWAYS, unit, 0, 0, TO_CHAR);
                break;
            case 1:
                act ("Someone grins evilly.", A_ALWAYS, unit, 0, 0, TO_CHAR);
                break;
            case 2:
                act ("The huge green dragon appears in a puff of smoke!",
                     A_ALWAYS, unit, 0, 0, TO_CHAR);
            }
    }
}

void
tif_blind_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("A cloak of darkness falls around you.<br/>", unit);
}

void
tif_blind_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("The veil of darkness disappears.<br/>", unit);
}

/* sneak */
void
tif_sneak_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("Ok, you'll try to move silently for a while.<br/>", unit);
}

void
tif_sneak_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You stop sneaking about.<br/>", unit);
}

void
tif_sneak_tick (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You sneak about unnoticed.<br/>", unit);
}

/* hide */
void
tif_hide_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You try to hide yourself.<br/>", unit);
}

void
tif_hide_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You stop hiding.<br/>", unit);
    if (af->tickf_i == TIF_HIDE_TICK)
        act ("You suddenly notice that $1n is standing here.",
             A_HIDEINV, unit, 0, 0, TO_ROOM);
}

void
tif_hide_tick (class unit_affected_type *af, class unit_data * unit)
{
    if (!(af->data[1]))
        return;

    switch (af->data[1]--)
    {
    case 1:
        send_to_char ("Someone is bound to look here sometime.<br/>", unit);
        break;
    case 2:
        send_to_char ("You master the ways of concealing yourself.<br/>", unit);
        break;
    case 3:
        send_to_char ("You skillfully use the shadows.<br/>", unit);
        break;
    case 4:
        send_to_char ("Surely noone can see you here.<br/>", unit);
        break;
    case 5:
        send_to_char ("You cleverly hide away.<br/>", unit);
    }
}



void
tif_nohide_tick (class unit_affected_type *af, class unit_data * unit)
{
    if (!(af->data[1]))
        return;

    switch (af->data[1]--)
    {
    case 1:
        send_to_char ("Someone is probably looking at you.<br/>", unit);
        break;
    case 2:
        send_to_char ("Maybe you should practice this hide-thing.<br/>", unit);
        break;
    case 3:
        send_to_char ("You skillfully avoid the shadows.<br/>", unit);
        break;
    case 4:
        send_to_char ("Surely you can see noone here?<br/>", unit);
        break;
    case 5:
        send_to_char ("You cleverly hide away.<br/>", unit);
    }
}

/* bless */
void
tif_bless_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel divine forces helping you.<br/>", unit);
}

void
tif_bless_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel divine forces abandoning you.<br/>", unit);
}

void
tif_bless_tick (class unit_affected_type *af, class unit_data * unit)
{
    if (af->duration == 1)
        send_to_char ("You sense the divine forces about to move away.<br/>",
                      unit);
}

/* curse */
void
tif_curse_on (class unit_affected_type *af, class unit_data * unit)
{
    if (IS_CHAR (unit))
        send_to_char ("You feel that the gods are against you.<br/>", unit);
    act ("A shadow falls upon $1n.", A_HIDEINV, unit, 0, 0, TO_ROOM);
    if (UNIT_IN (unit) && IS_CHAR (UNIT_IN (unit)))
        act ("A shadow falls upon $3n.", A_HIDEINV, UNIT_IN (unit), 0, unit,
             TO_CHAR);
}

void
tif_curse_off (class unit_affected_type *af, class unit_data * unit)
{
    if (IS_CHAR (unit))
        send_to_char ("You no longer feel that the gods are against you.<br/>",
                      unit);

    act ("A shadow lifts from $1n.", A_HIDEINV, unit, 0, 0, TO_ROOM);
    if (UNIT_IN (unit) && IS_CHAR (UNIT_IN (unit)))
        act ("A shadow lifts from $3n.", A_HIDEINV, UNIT_IN (unit), 0, unit,
             TO_CHAR);
}

/* sanctuary */
void
tif_sanctuary_on (class unit_affected_type *af, class unit_data * unit)
{
    act ("$1n momentarily glows in a bright white light.",
         A_HIDEINV, unit, 0, 0, TO_ROOM);
    send_to_char ("You momentarily glow in a bright white light.<br/>", unit);
}

void
tif_sanctuary_off (class unit_affected_type *af, class unit_data * unit)
{
    act ("$1n glows in a bright white light. Then it fades away.",
         A_HIDEINV, unit, 0, 0, TO_ROOM);
    send_to_char ("You glow in a bright white light. "
                  "Then it fades away.<br/>", unit);
}

void
tif_sanctuary_tick (class unit_affected_type *af, class unit_data * unit)
{
    if (af->duration == 1)
    {
        act ("$1n glows in a bright white light... The light flickers.",
             A_HIDEINV, unit, 0, 0, TO_ROOM);
        send_to_char ("You glow in a bright white light... "
                      "The light flickers.<br/>", unit);
    }
}

void
tif_eyes_tingle (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("Your eyes begin to tingle.<br/>", unit);
}


void
tif_torch_tick (class unit_affected_type *af, class unit_data * unit)
{
    OBJ_VALUE (unit, 0)--;

    if (af->duration <= 4)
    {
        if (IS_CHAR (UNIT_IN (unit)))
            act ("Your $2N is getting dim.", A_HIDEINV,
                 UNIT_IN (unit), unit, 0, TO_CHAR);
    }
}



void
tif_light_add (class unit_affected_type *af, class unit_data * unit)
{
    if (!UNIT_IN (unit))
        return;

    /* If the thing is carried by a character */
    if (IS_CHAR (UNIT_IN (unit)))
    {
        act ("Your $3N starts to glow.", A_HIDEINV, UNIT_IN (unit), 0, unit,
             TO_CHAR);
        act ("$1n's $3n starts to glow.", A_HIDEINV, UNIT_IN (unit), 0, unit,
             TO_ROOM);
    }
    else if (UNIT_CONTAINS (UNIT_IN (unit)))
        act ("The $3N starts to glow.", A_HIDEINV,
             UNIT_CONTAINS (UNIT_IN (unit)), 0, unit, TO_ALL);
}



void
tif_light_sub (class unit_affected_type *af, class unit_data * unit)
{
    if (!UNIT_IN (unit))
        return;

    /* If the thing is carried by a character */
    if (IS_CHAR (UNIT_IN (unit)))
    {
        act ("Your $3N gets dimmer.", A_HIDEINV, UNIT_IN (unit), 0, unit,
             TO_CHAR);
        act ("$1n's $3N gets dimmer.", A_HIDEINV, UNIT_IN (unit), 0, unit,
             TO_ROOM);
    }
    else if (UNIT_CONTAINS (UNIT_IN (unit)))
        act ("The $3N gets dimmer.", A_HIDEINV,
             UNIT_CONTAINS (UNIT_IN (unit)), 0, unit, TO_ALL);
}


void
tif_sleep_on (class unit_affected_type *af, class unit_data * unit)
{
    act ("You feel very tired.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    if (CHAR_POS (unit) > POSITION_SLEEPING)
    {
        if (CHAR_FIGHTING (unit))
            stop_fighting (unit);

        act ("You fall asleep.", A_ALWAYS, unit, 0, 0, TO_CHAR);
        act ("$1n falls asleep.", A_ALWAYS, unit, 0, 0, TO_ROOM);
        CHAR_POS (unit) = POSITION_SLEEPING;
    }
}


void
tif_sleep_check (class unit_affected_type *af, class unit_data * unit)
{
    int hm;

    if (CHAR_POS (unit) > POSITION_SLEEPING)
    {
        hm = resistance_skill_check (af->data[0],
                                     spell_ability (unit, ABIL_BRA, SPL_SLEEP),
                                     af->data[1],
                                     spell_defense_skill (unit, SPL_SLEEP));
        if (hm < 0)
        {
            if (CHAR_FIGHTING (unit))
                stop_fighting (unit);
            act ("You fall asleep.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            act ("$1n falls asleep.", A_HIDEINV, unit, 0, 0, TO_ROOM);
            CHAR_POS (unit) = POSITION_SLEEPING;
        }
    }
}


void
tif_sleep_off (class unit_affected_type *af, class unit_data * unit)
{
    act ("You feel less sleepy.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    /* no no not a 'wake' here, remember he's still affacted */
}

void
tif_protect_on (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel protected.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    else
        act ("You feel less protected.", A_ALWAYS, unit, 0, 0, TO_CHAR);
}

void
tif_protect_off (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel less protected.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    else
        act ("You feel protected.", A_ALWAYS, unit, 0, 0, TO_CHAR);
}


void
tif_hit_on (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel more healthy.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    else
        act ("You feel less healthy.", A_ALWAYS, unit, 0, 0, TO_CHAR);
}


void
tif_hit_off (class unit_affected_type *af, class unit_data * unit)
{
    af->data[1] = -af->data[1];
    tif_hit_on (af, unit);
    af->data[1] = -af->data[1];
}


void
tif_mag_on (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel more powerful.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    else
        act ("You feel less powerful.", A_ALWAYS, unit, 0, 0, TO_CHAR);
}


void
tif_mag_off (class unit_affected_type *af, class unit_data * unit)
{
    af->data[1] = -af->data[1];
    tif_mag_on (af, unit);
    af->data[1] = -af->data[1];
}


void
tif_div_on (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel closer to your god.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    else
        act ("You feel parted from your god.", A_ALWAYS, unit, 0, 0, TO_CHAR);
}


void
tif_div_off (class unit_affected_type *af, class unit_data * unit)
{
    af->data[1] = -af->data[1];
    tif_div_on (af, unit);
    af->data[1] = -af->data[1];
}

void
tif_str_on (class unit_affected_type *af, class unit_data * unit)
{

    /*old stuff
      if (af->data[1] > 0)
        act ("You feel stronger.", A_ALWAYS, unit, 0, 0, TO_CHAR);
      else
        act ("You feel weaker.", A_ALWAYS, unit, 0, 0, TO_CHAR);

    */

    /*Darg new str*/
    if (af->data[1] > 0)
    {
        switch (1+(int) (5.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You feel stronger.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("You feel more fit.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("You feel more buff.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("You feel able to take on the world.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("The power of Odin courses through your body!", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };
    }
    else
    {
        switch (1+(int) (2.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You feel weaker.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("Your arms start to feel heavier.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("The power of odin leaves you.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("You feel like you were just run over with a donkey cart!", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("You feel a unhealthy.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };
    }

    /*end new str*/

}


void
tif_str_off (class unit_affected_type *af, class unit_data * unit)
{
    af->data[1] = -af->data[1];
    tif_str_on (af, unit);
    af->data[1] = -af->data[1];
}


void
tif_dex_on (class unit_affected_type *af, class unit_data * unit)
{
    /* old dex stuff
      if (af->data[1] > 0)
        act ("You feel dexterous.", A_ALWAYS, unit, 0, 0, TO_CHAR);
      else
        act ("You feel a little clumsy.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    */
    /*Darg new dex*/
    if (af->data[1] > 0)
    {
        switch (1+(int) (5.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You feel dexterous.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("You feel nimble.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("You feel a bit more agile.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("You feel a bit more coordinated.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("You feel a bit slick.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };

    }
    else
    {
        switch (1+(int) (5.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You feel a little clumsy.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("You feel awkward.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("You feel a tad uncoordinated.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("You feel a bit more dim-witted.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("You feel a bumble footed.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };
    }

    /*end new dex*/
}


void
tif_dex_off (class unit_affected_type *af, class unit_data * unit)
{
    af->data[1] = -af->data[1];
    tif_dex_on (af, unit);
    af->data[1] = -af->data[1];
}

void
tif_con_on (class unit_affected_type *af, class unit_data * unit)
{
    /* old con stuff
      if (af->data[1] > 0)
        act ("You feel more robust.", A_ALWAYS, unit, 0, 0, TO_CHAR);
      else
        act ("You feel less robust.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    */
    /*Darg new con*/
    if (af->data[1] > 0)
    {
        switch (1+(int) (5.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You feel more robust.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("You feel a shift for the better in your disposition.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("You sense an increase in your persona .", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("You gain confidence in your stride.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("You feel concentrated on the task at hand.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };
    }
    else
    {
        switch (1+(int) (5.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You feel less robust.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("Your attitude slides downward.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("You feel winded.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("You lose confidence in yourself.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("You lose focus on the task at hand.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };
    }

    /*end new con*/
}



void
tif_con_off (class unit_affected_type *af, class unit_data * unit)
{
    af->data[1] = -af->data[1];
    tif_con_on (af, unit);
    af->data[1] = -af->data[1];
}


void
tif_cha_on (class unit_affected_type *af, class unit_data * unit)
{
    /* old cha stuff
      if (af->data[1] > 0)
        act ("You feel more authoritative.", A_ALWAYS, unit, 0, 0, TO_CHAR);
      else
        act ("You feel less authoritative.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    */
    /*Darg new cha*/
    if (af->data[1] > 0)
    {
        switch (1+(int) (5.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You feel more appealing.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("You feel your animal magnetism kick in.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("You feel more seductive.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("You feel more authoritative.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("Your conjuration feels more in tune with nature.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };
    }
    else
    {
        switch (1+(int) (5.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You suddenly feel hateful.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("You feel repulsed.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("You feel disenchanted.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("A bored, hollow feeling fills you.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("You become disinterested in your well being.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };
    }

    /*end new cha*/
}


void
tif_cha_off (class unit_affected_type *af, class unit_data * unit)
{
    af->data[1] = -af->data[1];
    tif_cha_on (af, unit);
    af->data[1] = -af->data[1];
}


void
tif_bra_on (class unit_affected_type *af, class unit_data * unit)
{
    /* old bra stuff
      if (af->data[1] > 0)
        act ("You feel smarter.", A_ALWAYS, unit, 0, 0, TO_CHAR);
      else
        act ("You feel a little dumb.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    */
    /*Darg new bra*/
    if (af->data[1] > 0)
    {
        switch (1+(int) (5.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You feel more intellectual.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("You feel smarter.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("You feel aggressive.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("You feel a rush of intellect wash over you.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("You feel witty.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };
    }
    else
    {
        switch (1+(int) (5.0*rand()/(RAND_MAX+1.0))) {
        case 1:
            act ("You feel dumb.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 2:
            act ("You feel idiotic.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 3:
            act ("You feel like a imbecile.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 4:
            act ("A lesser medulla oblongata fills your grey matter.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        case 5:
            act ("You suddenly realize your a moron.", A_ALWAYS, unit, 0, 0, TO_CHAR);
            break;
        };
    }

    /*end new cha*/

}


void
tif_bra_off (class unit_affected_type *af, class unit_data * unit)
{
    af->data[1] = -af->data[1];
    tif_bra_on (af, unit);
    af->data[1] = -af->data[1];
}


void
tif_poison_on (class unit_affected_type *af, class unit_data * unit)
{
    act ("You feel very ill.", A_ALWAYS, unit, 0, 0, TO_CHAR);
    act ("$1n seems very ill.", A_ALWAYS, unit, 0, 0, TO_ROOM);
}


/* Data[0] The amount of hitpoints to loose (>=0)        */
/* Data[1] The amount of Mana points to loose (>=0)      */
/* Data[2] The amount of Endurance points to loose (>=0) */
void
tif_poison_suffer (class unit_affected_type *af, class unit_data * unit)
{
    CHAR_MANA (unit) -= af->data[1];
    CHAR_ENDURANCE (unit) -= af->data[2];
    damage (unit, unit, 0, af->data[0], MSG_TYPE_OTHER,
            MSG_OTHER_POISON, COM_MSG_EBODY);
    /* unit can be destructed now, but no problemo */
}

void
tif_poison_off (class unit_affected_type *af, class unit_data * unit)
{
    act ("You feel better.", A_ALWAYS, unit, 0, 0, TO_CHAR);
}

/* plague */
void
tif_plague_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You suddenly feel very sick.<br/>", unit);
}

void
tif_plague_tick (class unit_affected_type *af, class unit_data * unit)
{
}

void
tif_plague_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel much better.<br/>", unit);
}

/* insanity */
void
tif_insanity_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel a battle with your own mind begins.<br/>", unit);
    act ("A mad look appears on $1n's face", A_HIDEINV, unit, 0, 0, TO_ROOM);
}

void
tif_insanity_tick (class unit_affected_type *af, class unit_data * unit)
{
}

void
tif_insanity_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel the battle with your own mind is over.<br/>", unit);
    act ("The mad look disappears from $1n's face.", A_HIDEINV, unit, 0, 0,
         TO_ROOM);
}

void
tif_prot_evil_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel protected from the forces of evil.<br/>", unit);
}

void
tif_prot_good_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel protected from the forces of good.<br/>", unit);
}

void
tif_prot_evil_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel less protected from the forces of evil.<br/>", unit);
}

void
tif_prot_good_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel less protected from the forces of good.<br/>", unit);
}

void
tif_sustain_on (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You feel sustained.<br/>", unit);
}

void
tif_sustain_tick (class unit_affected_type *af, class unit_data * unit)
{
    if (!IS_PC (unit))
        return;
    PC_COND (unit, 0) = af->data[0];
    PC_COND (unit, 1) = af->data[1];
    PC_COND (unit, 2) = af->data[2];
}

void
tif_sustain_off (class unit_affected_type *af, class unit_data * unit)
{
    send_to_char ("You no longer feel sustained.<br/>", unit);
}

void
tif_decay_corpse (class unit_affected_type *af, class unit_data * unit)
{
    /* Make routine to change the description of a corpse instead */
    if (ODD (af->duration) && !IS_SET (UNIT_FLAGS (unit), UNIT_FL_BURIED))
        act ("The rotten stench of $1n is here.", A_SOMEONE, unit, 0, 0, TO_ROOM);
}

void
tif_destroy_corpse (class unit_affected_type *af, class unit_data * unit)
{
    if (!IS_SET (UNIT_FLAGS (unit), UNIT_FL_BURIED))
        act ("A quivering horde of maggots consume $1n.",
             A_SOMEONE, unit, 0, 0, TO_ROOM);
    extract_unit (unit);
}

void
tif_buried_destruct (class unit_affected_type *af, class unit_data * unit)
{
    if (IS_SET (UNIT_FLAGS (unit), UNIT_FL_BURIED))
    {
        /* Empty the container and set buried status of contents */

        while (UNIT_CONTAINS (unit))
        {
            class unit_affected_type naf;

            SET_BIT (UNIT_FLAGS (UNIT_CONTAINS (unit)), UNIT_FL_BURIED);

            naf.id = ID_BURIED;
            naf.duration = 0;
            naf.beat = WAIT_SEC * SECS_PER_REAL_HOUR;
            naf.firstf_i = TIF_NONE;
            naf.tickf_i = TIF_NONE;
            naf.lastf_i = TIF_BURIED_DESTRUCT;
            naf.applyf_i = APF_NONE;

            create_affect (UNIT_CONTAINS (unit), &naf);

            if (UNIT_IS_EQUIPPED (UNIT_CONTAINS (unit)))
                unequip_object (UNIT_CONTAINS (unit));

            unit_up (UNIT_CONTAINS (unit));
        }

        extract_unit (unit);
    }
}

void
tif_valhalla_ret (class unit_affected_type *af, class unit_data * unit)
{
    if (!IS_PC (unit))
        return;

    CHAR_POS (unit) = POSITION_STANDING;
    REMOVE_BIT (PC_FLAGS (unit), PC_SPIRIT);
    REMOVE_BIT (CHAR_FLAGS (unit), CHAR_KILL_SELF);

    act ("You have a strange feeling...", A_ALWAYS, unit, 0, 0,
         TO_CHAR);
    act ("$1n materializes and vanish.", A_HIDEINV, unit, 0, 0, TO_ROOM);

    unit_from_unit (unit);
    unit_to_unit (unit, hometown_unit (PC_HOME (unit)));

    act ("The spirit of $1n materializes.", A_HIDEINV, unit, 0, 0, TO_ROOM);
    act ("You feel dizzy.", A_ALWAYS, unit, 0, 0, TO_CHAR);

    PC_COND (unit, FULL) = 24;
    PC_COND (unit, THIRST) = 24;
    PC_COND (unit, DRUNK) = 0;

    CHAR_MANA (unit) = mana_limit (unit);
    CHAR_ENDURANCE (unit) = move_limit (unit);
    UNIT_HIT (unit) = UNIT_MAX_HIT (unit);

    if (!unit->is_destructed())
    {
        save_player (unit);
        save_player_contents (unit, TRUE);
    }
}


void
tif_jail_wait (class unit_affected_type *af, class unit_data * unit)
{
}

/* Get thrown out of jail */
void
tif_jail_release (class unit_affected_type *af, class unit_data * unit)
{
}


void
tif_spl_on (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel more skilled at $2t.",
             A_ALWAYS, unit, spl_text[af->data[0]], 0, TO_CHAR);
    else
        act ("You feel less skilled at $2t.",
             A_ALWAYS, unit, spl_text[af->data[0]], 0, TO_CHAR);
}

void
tif_spl_off (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel less skilled at $2t.",
             A_ALWAYS, unit, spl_text[af->data[0]], 0, TO_CHAR);
    else
        act ("You feel more skilled at $2t.",
             A_ALWAYS, unit, spl_text[af->data[0]], 0, TO_CHAR);
}

void
tif_ski_on (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel more skilled in $2t.",
             A_ALWAYS, unit, ski_text[af->data[0]], 0, TO_CHAR);
    else
        act ("You feel less skilled in $2t.",
             A_ALWAYS, unit, ski_text[af->data[0]], 0, TO_CHAR);
}

void
tif_ski_off (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel less skilled in $2t.",
             A_ALWAYS, unit, ski_text[af->data[0]], 0, TO_CHAR);
    else
        act ("You feel more skilled in $2t.",
             A_ALWAYS, unit, ski_text[af->data[0]], 0, TO_CHAR);
}

void
tif_wpn_on (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel more skilled at the $2t fighting style.",
             A_ALWAYS, unit, wpn_text[af->data[0]], 0, TO_CHAR);
    else
        act ("You feel less skilled at the $2t fighting style.",
             A_ALWAYS, unit, wpn_text[af->data[0]], 0, TO_CHAR);
}

void
tif_wpn_off (class unit_affected_type *af, class unit_data * unit)
{
    if (af->data[1] > 0)
        act ("You feel less skilled at the $2t fighting style.",
             A_ALWAYS, unit, wpn_text[af->data[0]], 0, TO_CHAR);
    else
        act ("You feel more skilled at the $2t fighting style.",
             A_ALWAYS, unit, wpn_text[af->data[0]], 0, TO_CHAR);
}

void
tif_armour_on (class unit_affected_type *af, class unit_data * unit)
{
    const   char *c = "pale skin like";

    switch (CHAR_NATURAL_ARMOUR (unit))
    {
    case ARM_LEATHER:
        c = "a dark leather like";
        break;

    case ARM_HLEATHER:
        c = "a dark bark like";
        break;

    case ARM_CHAIN:
        c = "an ebony coloured bone like";
        break;

    case ARM_PLATE:
        c = "a grey stone like";
        break;

    default:
        af->data[0] = ARM_CLOTHES;
        break;
    }

    act ("Your skin transforms into $2t substance.",
         A_ALWAYS, unit, c, 0, TO_CHAR);

    act ("$1n's skin transforms into $2t substance.",
         A_ALWAYS, unit, c, 0, TO_ROOM);
}

void
tif_speed_on (class unit_affected_type *af, class unit_data * unit)
{
    act ("You feel faster...", A_ALWAYS, unit, 0, 0, TO_CHAR);
}

void
tif_speed_off (class unit_affected_type *af, class unit_data * unit)
{
    act ("You feel slower...", A_ALWAYS, unit, 0, 0, TO_CHAR);
}

/* --------------------------------------------------------------------- */
void tif_reward_on (class unit_affected_type *af, class unit_data * unit);
void tif_reward_off (class unit_affected_type *af, class unit_data * unit);

struct tick_function_type tif[] = {
    {"Decay Corpse", tif_decay_corpse},
    {"Destroy Corpse", tif_destroy_corpse},
    {"Valhalla Return", tif_valhalla_ret},
    {"Hitpoints On", tif_hit_on},
    {"Hitpoints Off", tif_hit_off},
    {"Magic Mod On", tif_mag_on},
    {"Magic Mod Off", tif_mag_off},
    {"Divine Mod On", tif_div_on},
    {"Divine Mod Off", tif_div_off},
    {"Strength Mod On", tif_str_on},
    {"Strength Mod Off", tif_str_off},
    {"Dexterity Mod On", tif_dex_on},
    {"Dexterity Mod Off", tif_dex_off},
    {"Constitution Mod On", tif_con_on},
    {"Constitution Mod Off", tif_con_off},
    {"Charisma Mod On", tif_cha_on},
    {"Charisma Mod Off", tif_cha_off},
    {"Brain Mod On", tif_bra_on},
    {"Brain Mod Off", tif_bra_off},
    {"XXX", tif_armour_on},
    {"XXX", tif_armour_on},
    {"Spell-skill Mod On", tif_protect_on},
    {"Spell-skill Mod Off", tif_protect_off},
    {"Poison On", tif_poison_on},
    {"Poison Off", tif_poison_off},
    {"Poison Suffer", tif_poison_suffer},
    {"Light/Dark Add", tif_light_add},
    {"Light/Dark Sub", tif_light_sub},
    {"Torch burning countdown", tif_torch_tick},
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
    {"reward on", tif_reward_on},
    {"reward off", tif_reward_off},
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
    {"speed worse", tif_speed_off}
};
