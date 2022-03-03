/*
 $Author: tperry $
 $RCSfile: act_skills.cpp,v $
 $Date: 2002/05/22 01:35:58 $
 $Revision: 2.4 $
 */

#include "affect.h"
#include "comm.h"
#include "experience.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "magic.h"
#include "skills.h"
#include "structs.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "unitfind.h"
#include "utils.h"

/*
 * coded: Mon Jun 22 00:22:44 MET DST 1992 [HH]
 * tested: No
 */
void do_backstab(unit_data *ch, char *arg, const command_info *cmd)
{
    unit_affected_type af;
    unit_affected_type *paf = nullptr;
    unit_data *vict = nullptr;
    unit_data *stabber = nullptr;
    int skilla = 0;
    int skillb = 0;
    int hm = 0;
    char *oarg = arg;

    if (str_is_empty(arg))
    {
        send_to_char("Backstab who?<br/>", ch);
        return;
    }

    skilla = IS_PC(ch) ? PC_SKI_SKILL(ch, SKI_BACKSTAB) : CHAR_BRA(ch);

    if (skilla == 0)
    {
        send_to_char("You must practice first.<br/>", ch);
        return;
    }

    if ((vict = find_unit(ch, &arg, nullptr, FIND_UNIT_SURRO)) == nullptr)
    {
        send_to_char("Noone here by that name.<br/>", ch);
        return;
    }

    if (!IS_CHAR(vict))
    {
        send_to_char("But it's dead already.<br/>", ch);
        return;
    }

    if (vict == ch)
    {
        send_to_char("How can you sneak up on yourself?<br/>", ch);
        return;
    }

    if (pk_test(ch, vict, TRUE))
    {
        return;
    }

    if ((stabber = equipment_type(ch, WEAR_WIELD, ITEM_WEAPON)) == nullptr)
    {
        send_to_char("You need to wield a weapon, to make it a succes.<br/>", ch);
        return;
    }

    if (OBJ_VALUE(stabber, 0) != WPN_DAGGER && OBJ_VALUE(stabber, 0) != WPN_SHORT_SWORD && OBJ_VALUE(stabber, 0) != WPN_STILETTO)
    {
        send_to_char("Only small piercing weapons can be used "
                     "for backstabbing.<br/>",
                     ch);
        return;
    }

    if (CHAR_FIGHTING(vict))
    {
        send_to_char("You can't backstab a fighting person, too alert!<br/>", ch);
        return;
    }

    if (CHAR_AWAKE(vict))
    {
        skillb = IS_PC(vict) ? PC_SKI_SKILL(vict, SKI_BACKSTAB) : CHAR_BRA(vict);

        /* For each recent backstab, victim gets a +50 bonus to resist. */

        if ((paf = affected_by_spell(vict, ID_BACKSTABBED)))
        {
            skillb += 50 * paf->getDataAtIndex(0);
        }

        hm = resistance_skill_check(effective_dex(ch), CHAR_DEX(vict), skilla, skillb);
    }
    else
    {
        hm = resistance_skill_check(effective_dex(ch), 0, skilla, 0);
    }

    if (paf == nullptr)
    {
        af.setID(ID_BACKSTABBED);
        af.setDuration(15);
        af.setBeat(WAIT_SEC * 60);
        af.setFirstFI(TIF_NONE);
        af.setTickFI(TIF_NONE);
        af.setLastFI(TIF_NONE);
        af.setApplyFI(APF_NONE);
        af.setDataAtIndex(0, 1);
        create_affect(vict, &af);
    }
    else
    {
        paf->setDuration(15);
        paf->setBeat(WAIT_SEC * 60);
        paf->incrementDataAtIndex(0);
    }

    if (hm < 0)
    {
        damage(ch, vict, stabber, 0, MSG_TYPE_SKILL, SKI_BACKSTAB, COM_MSG_MISS);
    }
    else
    {
        int dam = 0;
        int att_bonus = 0;

        att_bonus = melee_bonus(ch, vict, hit_location(ch, vict), nullptr, nullptr, nullptr, nullptr);

        att_bonus += open100() + hm;

        dam = weapon_damage(att_bonus, OBJ_VALUE(stabber, 0), ARM_CLOTHES);

        send_done(ch, stabber, vict, dam, cmd, oarg);

        damage(ch, vict, stabber, dam, MSG_TYPE_SKILL, SKI_BACKSTAB, COM_MSG_EBODY);
    }
}
