/*
 $Author: tperry $
 $RCSfile: experience.cpp,v $
 $Date: 2002/05/22 01:35:59 $
 $Revision: 2.2 $
 */
#include "experience.h"

#include "affect.h"
#include "comm.h"
#include "common.h"
#include "fight.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "magic.h"
#include "modify.h"
#include "skills.h"
#include "slog.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"

#include <cstdio>

// MS2020 Trying to decipher the purpose & get rid of this kludge for a more dynamic point system
//   level 0 - 20 : Changes nothing
//   up to lvl 50 : add 5% bonus to points for each level above 20, for points up to 100 max.
//                  will yield bonus from 0..150
//   above lvl 50 :
//                  will yield bonus from 150..150+5/level
//

int kludge_bonus(int level, int points)
{
    if (level <= 20)
    {
        return 0;
    }

    if (level <= 50)
    {
        return ((5 * (level - 20)) * MIN(100, points)) / 100;
    }

    int b = 0;
    int expected = 0;

    expected = 100 + (level - 50);

    b = 150;
    b += (5 * (level - 50) * MIN(expected, points)) / expected;

    return b;
}

/* Let 'def' attempt to use his shield against 'att'. If the shield */
/* is magic, then it is easier to use. Material & magic does also   */
/* offer half the additional protection in case it is used          */
/* successfully).                                                   */
/* Returns 0 or less if unsuccessful, or 1..100 for the blocking    */
/* chance                                                           */

int shield_bonus(unit_data *att, unit_data *def, unit_data **pDef_shield)
{
    unit_data *def_shield = nullptr;
    int def_shield_bonus = 0;

    int att_dex = 0;
    int def_dex = 0;
    int hm = 0;

    att_dex = effective_dex(att);
    def_dex = effective_dex(def);

    /* If attacker can't see the defender, then the defender has a   */
    /* much better effective dexterity (since attacker can't see him */
    if (!CHAR_CAN_SEE(att, def))
    {
        att_dex -= 12;
    }

    if ((def_shield = equipment_type(def, WEAR_SHIELD, ITEM_SHIELD)))
    {
        int shield_bonus = 0;

        if (!is_in(OBJ_VALUE(def_shield, 0), SHIELD_SMALL, SHIELD_LARGE))
        {
            slog(LOG_ALL, 0, "Illegal shield type.");
            OBJ_TYPE(def_shield) = ITEM_TRASH;
        }
        else if (!is_in(OBJ_VALUE(def_shield, 1), -25, 25) || !is_in(OBJ_VALUE(def_shield, 2), -25, 25))
        {
            slog(LOG_ALL, 0, "Illegal shield bonus");
            OBJ_TYPE(def_shield) = ITEM_TRASH;
        }
        else
        {
            shield_bonus = OBJ_VALUE(def_shield, 1) + OBJ_VALUE(def_shield, 2);

            /* Let's make a shield check - CAN_SEE does affect this too */
            hm = resistance_skill_check(def_dex + shield_bonus,
                                        att_dex,
                                        IS_PC(def) ? PC_SKI_SKILL(def, SKI_SHIELD) : def_dex,
                                        IS_PC(att) ? PC_SKI_SKILL(att, SKI_SHIELD) : att_dex);

            if (hm >= 0)
            { /* Successful Shield use */
                def_shield_bonus = g_shi_info[OBJ_VALUE(def_shield, 0)].melee + shield_bonus / 2;
            }
        }
    } /* End of Shield */

    if (pDef_shield)
    {
        *pDef_shield = def_shield;
    }

    return def_shield_bonus;
}

int dikuii_spell_bonus(unit_data *att,
                       unit_data *medium,
                       unit_data *def,
                       int hit_loc,
                       int spell_number,
                       int *pDef_armour_type,
                       unit_data **pDef_armour)
{
    int att_spl_knowledge = 0;
    int def_spl_knowledge = 0;
    int att_bonus = 0;
    int def_bonus = 0;
    int def_armour_type = 0;
    unit_data *def_armour = nullptr;
    int hm = 0;

    att_bonus = CHAR_OFFENSIVE(att);
    def_bonus = CHAR_DEFENSIVE(def);

    /* If attacker can't see the defender, then the defender has a   */
    /* much better effective dexterity (since attacker can't see him */
    if (!CHAR_CAN_SEE(att, def))
    {
        att_bonus -= 12;
    }

    if ((UNIT_IS_GOOD(att) && affected_by_spell(def, ID_PROT_GOOD)) || (UNIT_IS_EVIL(att) && affected_by_spell(def, ID_PROT_EVIL)))
    {
        def_bonus += 10;
    }

    if ((def_armour = equipment_type(def, hit_loc, ITEM_ARMOR)))
    {
        def_armour_type = OBJ_VALUE(def_armour, 0);
        if (is_in(OBJ_VALUE(def_armour, 1), -25, 25))
        {
            def_bonus += OBJ_VALUE(def_armour, 1) + OBJ_VALUE(def_armour, 2);
        }
        else
        {
            slog(LOG_ALL, 0, "Illegal armour bonus.");
        }
    }
    else
    {
        def_armour_type = CHAR_NATURAL_ARMOUR(def);
    }

    if (pDef_armour_type)
    {
        *pDef_armour_type = def_armour_type;
    }

    if (pDef_armour)
    {
        *pDef_armour = def_armour;
    }

    att_spl_knowledge = spell_attack_skill(medium, spell_number);
    def_spl_knowledge = spell_defense_skill(def, spell_number);

    att_spl_knowledge += kludge_bonus(CHAR_LEVEL(att), att_spl_knowledge);
    def_spl_knowledge += kludge_bonus(CHAR_LEVEL(def), def_spl_knowledge) / 2;

    if (CHAR_AWAKE(def))
    {
        hm = (5 * (spell_attack_ability(medium, spell_number) - spell_ability(def, ABIL_BRA, spell_number))) / 2 +
             2 * (att_spl_knowledge - def_spl_knowledge) - def_bonus;
    }
    else
    {
        hm = (5 * spell_attack_ability(medium, spell_number)) / 2 + 2 * spell_attack_skill(medium, spell_number) - def_bonus;
    }

    return MAX(-50, hm);
}

/* If 'att' hits 'def' on 'hit_loc' then what is his basic attack */
/* modification? This value should then be added to an open roll  */
/* and looked up upon the damage_charts. If armour type points    */
/* to anything, then it will be set to the defenders armour_type  */
/* which should be used upon lookup                               */

int dikuii_melee_bonus(unit_data *att,
                       unit_data *def,
                       int hit_loc,
                       int *pAtt_weapon_type,
                       unit_data **pAtt_weapon,
                       int *pDef_armour_type,
                       unit_data **pDef_armour,
                       int primary)
{
    int att_bonus = 0;
    int att_wpn_knowledge = 0;
    int def_dex = 0;
    int def_bonus = 0;
    int def_wpn_knowledge = 0;

    unit_data *att_wpn = nullptr;
    int att_wpn_type = 0;

    int def_armour_type = 0;
    unit_data *def_armour = nullptr;

    int hm = 0;

    def_dex = effective_dex(def);

    att_bonus = CHAR_OFFENSIVE(att);
    def_bonus = CHAR_DEFENSIVE(def);

    if (pAtt_weapon_type && is_in(*pAtt_weapon_type, WPN_GROUP_MAX, WPN_TREE_MAX - 1))
    {
        att_wpn = nullptr;
        att_wpn_type = *pAtt_weapon_type;
        att_wpn_knowledge = weapon_attack_skill(att, att_wpn_type);
    }
    else
    {
        if (primary)
        {
            att_wpn = equipment_type(att, WEAR_WIELD, ITEM_WEAPON);
        }
        else
        {
            att_wpn = equipment_type(att, WEAR_HOLD, ITEM_WEAPON);
        }

        if (att_wpn)
        {
            att_wpn_type = OBJ_VALUE(att_wpn, 0); /* [0] is category */
            att_wpn_knowledge = weapon_attack_skill(att, att_wpn_type);
            if (is_in(OBJ_VALUE(att_wpn, 1), -25, 25))
            {
                att_bonus += OBJ_VALUE(att_wpn, 1) + OBJ_VALUE(att_wpn, 2);
            }
        }
        else
        {
            att_wpn_type = CHAR_ATTACK_TYPE(att);
            att_wpn_knowledge = weapon_attack_skill(att, att_wpn_type);
        }
    }

    if (char_dual_wield(att))
    {
        int dual_skill = 0;

        if (IS_PC(att))
        {
            dual_skill = PC_SKI_SKILL(att, SKI_DUAL_WIELD);
        }
        else
        {
            dual_skill = CHAR_DEX(att);
        }

        if (primary)
        {
            att_bonus -= MAX(0, 25 - (dual_skill / 4));
        }
        else
        {
            att_bonus -= MAX(0, 50 - (dual_skill / 4));
        }
    }

    def_wpn_knowledge = weapon_defense_skill(def, att_wpn_type);

    if (CHAR_FIGHTING(def) != att)
    {
        def_bonus -= 25;
    }

    /* If attacker can't see the defender, then the defender has a   */
    /* much better effective dexterity (since attacker can't see him */
    if (!CHAR_CAN_SEE(att, def))
    {
        def_bonus += 25;
    }

    if (!CHAR_CAN_SEE(def, att))
    {
        att_bonus += 25;
    }

    /* Slaying Weapons */
    if (is_obj_slaying(att_wpn, CHAR_RACE(def)))
    {
        att_bonus += 25;
    }

    if ((UNIT_IS_GOOD(att) && affected_by_spell(def, ID_PROT_GOOD)) || (UNIT_IS_EVIL(att) && affected_by_spell(def, ID_PROT_EVIL)))
    {
        def_bonus += 20;
    }

    if ((def_armour = equipment_type(def, hit_loc, ITEM_ARMOR)))
    {
        def_armour_type = OBJ_VALUE(def_armour, 0);
        if (is_in(OBJ_VALUE(def_armour, 1), -25, 25))
        {
            def_bonus += OBJ_VALUE(def_armour, 1) + OBJ_VALUE(def_armour, 2);
        }
        else
        {
            slog(LOG_ALL, 0, "Illegal armour bonus.");
        }
    }
    else
    {
        def_armour_type = CHAR_NATURAL_ARMOUR(def);
    }

    if (pAtt_weapon_type)
    {
        *pAtt_weapon_type = att_wpn_type;
    }

    if (pAtt_weapon)
    {
        *pAtt_weapon = att_wpn;
    }

    if (pDef_armour_type)
    {
        *pDef_armour_type = def_armour_type;
    }

    if (pDef_armour)
    {
        *pDef_armour = def_armour;
    }

    att_wpn_knowledge += kludge_bonus(CHAR_LEVEL(att), att_wpn_knowledge);
    def_wpn_knowledge += kludge_bonus(CHAR_LEVEL(def), def_wpn_knowledge) / 2;

    int att_abil = weapon_attack_ability(att, att_wpn_type);

    if (CHAR_AWAKE(def))
    {
        hm = (5 * (att_abil - def_dex)) / 2 + (2 * (att_wpn_knowledge - def_wpn_knowledge)) + att_bonus - def_bonus;
    }
    else
    {
        hm = (5 * att_abil) / 2 + att_bonus + 2 * att_wpn_knowledge + 50;
    }

    // This results in a 5% hm increase per "level"

    return MAX(-50, hm);
}

int spell_bonus(unit_data *att,
                unit_data *medium,
                unit_data *def,
                int hit_loc,
                int spell_number,
                int *pDef_armour_type,
                unit_data **pDef_armour,
                std::string *pStat)
{
    int att_spl_knowledge = 0;
    int def_spl_knowledge = 0;
    int att_bonus = 0;
    int def_bonus = 0;
    int def_armour_type = 0;
    unit_data *def_armour = nullptr;
    int hm = 0;

    if (pStat)
    {
        *pStat =
            diku::format_to_str("<u>%s spelling %s with %s:</u><br/><pre>", UNIT_NAME(att), UNIT_NAME(def), g_SplColl.text[spell_number]);
        pStat->append("                        ATT     DEF<br/>");
    }

    att_bonus = CHAR_OFFENSIVE(att);
    def_bonus = CHAR_DEFENSIVE(def);
    if (pStat)
    {
        pStat->append(diku::format_to_str("Off / Def bonus     :  %4d    %4d<br/>", att_bonus, def_bonus));
    }

    /* If attacker can't see the defender, then the defender has a   */
    /* much better effective attack (since attacker can't see him    */
    if (!CHAR_CAN_SEE(att, def))
    {
        att_bonus -= 25;
        if (pStat)
        {
            pStat->append(diku::format_to_str("Attacker cant see    :         %4d<br/>", -25));
        }
    }

    // Check for free hands
    // What if there is something on _HANDS, e.g. Gloves - how should that affect bonus?
    // What if the CHAR is holding something?
    unit_data *wield = equipment(att, WEAR_WIELD);
    unit_data *shield = equipment(att, WEAR_SHIELD);
    if (shield == nullptr)
    {
        if (wield == nullptr)
        {
            att_bonus += 50;
            if (pStat)
            {
                pStat->append(diku::format_to_str("Free wield+shield    :         %4d<br/>", +50));
            }
        }
    }

    if ((UNIT_IS_GOOD(att) && affected_by_spell(def, ID_PROT_GOOD)) || (UNIT_IS_EVIL(att) && affected_by_spell(def, ID_PROT_EVIL)))
    {
        if (pStat)
        {
            pStat->append(diku::format_to_str("Evil good protect     :       %4d  <br/>", +25));
        }
        def_bonus += 25;
    }

    if ((def_armour = equipment_type(def, hit_loc, ITEM_ARMOR)))
    {
        def_armour_type = OBJ_VALUE(def_armour, 0);
        if (is_in(OBJ_VALUE(def_armour, 1), -25, 25))
        {
            int tmp = OBJ_VALUE(def_armour, 1) + OBJ_VALUE(def_armour, 2);
            def_bonus += tmp;

            if (pStat)
            {
                pStat->append(diku::format_to_str("Armor bonus         :       %4d  <br/>", tmp));
            }
        }
        else
        {
            slog(LOG_ALL, 0, "Illegal armour bonus.");
        }
    }
    else
    {
        def_armour_type = CHAR_NATURAL_ARMOUR(def);
    }

    if (pDef_armour_type)
    {
        *pDef_armour_type = def_armour_type;
    }

    if (pDef_armour)
    {
        *pDef_armour = def_armour;
    }

    att_spl_knowledge = spell_attack_skill(medium, spell_number);
    def_spl_knowledge = spell_defense_skill(def, spell_number);

    int att_spl_ability = spell_attack_ability(medium, spell_number);
    int def_spl_ability = spell_ability(def, ABIL_BRA, spell_number);

    if (pStat)
    {
        pStat->append(" --- SUMMARY ---<br/>");
        pStat->append(diku::format_to_str("Spl knowledge       :  %4d    %4d<br/>", att_spl_knowledge, def_spl_knowledge));
        pStat->append(diku::format_to_str("Spl ability         :  %4d    %4d<br/>", att_spl_ability, def_spl_ability));
        pStat->append(diku::format_to_str("Att bonus vs Def    :  %4d    %4d<br/>", att_bonus, def_bonus));
    }

    // MS2020. Maybe the CHAR_LEVEL should not be a part of the final formula.
    if (CHAR_AWAKE(def))
    {
        hm = 2 * (att_spl_ability - def_spl_ability) + 2 * (att_spl_knowledge - def_spl_knowledge) +
             (att_bonus - def_bonus); // + CHAR_LEVEL(att) - CHAR_LEVEL(def);

        if (pStat)
        {
            pStat->append(diku::format_to_str("Result              :  %4d<br/>", hm));
        }
    }
    else
    {
        hm = 2 * att_spl_ability + 2 * att_spl_knowledge + (att_bonus - def_bonus); // + CHAR_LEVEL(att) - CHAR_LEVEL(def);

        if (pStat)
        {
            pStat->append(diku::format_to_str("Result (sleep def)  :  %4d<br/>", hm));
        }
    }

    if (pStat)
    {
        pStat->append("Result = 2*(att_abil-def_abil) + 2*(att_spl - def_spl) + (att_bonus - def_bonus)) + roll<br/>");
        pStat->append("<br/>");

        int dam5 = chart_damage(hm + roll_boost(5, CHAR_LEVEL(att)), &(g_spell_chart[spell_number].element[def_armour_type]));
        int dam50 = chart_damage(hm + roll_boost(50, CHAR_LEVEL(att)), &(g_spell_chart[spell_number].element[def_armour_type]));
        int dam95 = chart_damage(hm + roll_boost(95, CHAR_LEVEL(att)), &(g_spell_chart[spell_number].element[def_armour_type]));

        pStat->append(diku::format_to_str("Spell  dmg (5/50/95) : %4d %4d %4d<br/>", dam5, dam50, dam95));

        pStat->append(diku::format_to_str("Rounds to kill def = %d<br/>", UNIT_MAX_HIT(def) / MAX(1, (dam5 + dam50 + dam95) / 3)));
        pStat->append("Defensive Shield bonus not part of stat<br/>");

        pStat->append("</pre>");
    }

    return MAX(-50, hm);
}

/* If 'att' hits 'def' on 'hit_loc' then what is his basic attack */
/* modification? This value should then be added to an open roll  */
/* and looked up upon the damage_charts. If armour type points    */
/* to anything, then it will be set to the defenders armour_type  */
/* which should be used upon lookup                               */

int melee_bonus(unit_data *att,
                unit_data *def,
                int hit_loc,
                int *pAtt_weapon_type,
                unit_data **pAtt_weapon,
                int *pDef_armour_type,
                unit_data **pDef_armour,
                int primary,
                std::string *pStat)
{
    int att_bonus = 0;
    int att_wpn_knowledge = 0;
    int def_bonus = 0;
    int def_wpn_knowledge = 0;

    unit_data *att_wpn = nullptr;
    int att_wpn_type = 0;

    int def_armour_type = 0;
    int tmp = 0;
    unit_data *def_armour = nullptr;
    int hm = 0;

    if (pStat)
    {
        *pStat = diku::format_to_str("<u>%s attacking %s:</u><br/><pre>", UNIT_NAME(att), UNIT_NAME(def));
        pStat->append("                        ATT     DEF<br/>");
    }

    att_bonus = CHAR_OFFENSIVE(att);
    def_bonus = CHAR_DEFENSIVE(def);
    if (pStat)
    {
        pStat->append(diku::format_to_str("Off / Def bonus     :  %4d   %4d<br/>", att_bonus, def_bonus));
    }

    if (pAtt_weapon_type && is_in(*pAtt_weapon_type, WPN_GROUP_MAX, WPN_TREE_MAX - 1))
    {
        att_wpn = nullptr;
        att_wpn_type = *pAtt_weapon_type;
        att_wpn_knowledge = weapon_attack_skill(att, att_wpn_type);
    }
    else
    {
        if (primary)
        {
            att_wpn = equipment_type(att, WEAR_WIELD, ITEM_WEAPON);
        }
        else
        {
            att_wpn = equipment_type(att, WEAR_HOLD, ITEM_WEAPON);
        }

        if (att_wpn)
        {
            att_wpn_type = OBJ_VALUE(att_wpn, 0); /* [0] is category */
            att_wpn_knowledge = weapon_attack_skill(att, att_wpn_type);
            if (pStat)
            {
                pStat->append(diku::format_to_str("Att Weapon          :  %s       <br/>", UNIT_NAME(att_wpn)));
            }
            if (is_in(OBJ_VALUE(att_wpn, 1), -25, 25))
            {
                tmp = OBJ_VALUE(att_wpn, 1) + OBJ_VALUE(att_wpn, 2);
                att_bonus += tmp;
                if (pStat)
                {
                    pStat->append(diku::format_to_str("Att Weapon mat+mag  :  %4d       <br/>", tmp));
                }
            }
        }
        else
        {
            att_wpn_type = CHAR_ATTACK_TYPE(att);
            att_wpn_knowledge = weapon_attack_skill(att, att_wpn_type);

            if (pStat)
            {
                pStat->append("Att Weapon          :  None       <br/>");
            }
        }
    }

    if (char_dual_wield(att))
    {
        int dual_skill = 0;

        if (IS_PC(att))
        {
            dual_skill = PC_SKI_SKILL(att, SKI_DUAL_WIELD);
        }
        else
        {
            dual_skill = CHAR_DEX(att);
        }

        if (primary)
        {
            tmp = -MAX(0, 25 - (dual_skill / 4));
        }
        else
        {
            tmp = -MAX(0, 50 - (dual_skill / 4));
        }

        att_bonus += tmp;

        if (pStat)
        {
            pStat->append(diku::format_to_str("Dual wield          :  %4d       <br/>", tmp));
        }
    }

    def_wpn_knowledge = weapon_defense_skill(def, att_wpn_type);
    if (pStat)
    {
        pStat->append(diku::format_to_str("Weapon skill        :  %4d   %4d<br/>", att_wpn_knowledge, def_wpn_knowledge));
    }

    if (CHAR_FIGHTING(def) != att)
    {
        def_bonus -= 25;

        if (pStat)
        {
            pStat->append(diku::format_to_str("Def not fighting att:          %4d<br/>", -25));
        }
    }

    /* If attacker can't see the defender, then the defender has a   */
    /* much better effective dexterity (since attacker can't see him */
    if (!CHAR_CAN_SEE(att, def))
    {
        def_bonus += 25;
        if (pStat)
        {
            pStat->append(diku::format_to_str("Attacker cant see    :       %4d<br/>", 25));
        }
    }

    if (!CHAR_CAN_SEE(def, att))
    {
        att_bonus += 25;
        if (pStat)
        {
            pStat->append(diku::format_to_str("Defender cant see    :  %4d<br/>", 25));
        }
    }

    /* Slaying Weapons */
    if (is_obj_slaying(att_wpn, CHAR_RACE(def)))
    {
        att_bonus += 50;

        if (pStat)
        {
            pStat->append(diku::format_to_str("Slaying              :  %4d  <br/>", 50));
        }
    }

    if ((UNIT_IS_GOOD(att) && affected_by_spell(def, ID_PROT_GOOD)) || (UNIT_IS_EVIL(att) && affected_by_spell(def, ID_PROT_EVIL)))
    {
        def_bonus += 20;

        if (pStat)
        {
            pStat->append(diku::format_to_str("Evil good protect     :       %4d  <br/>", 20));
        }
    }

    if ((def_armour = equipment_type(def, hit_loc, ITEM_ARMOR)))
    {
        def_armour_type = OBJ_VALUE(def_armour, 0);
        if (is_in(OBJ_VALUE(def_armour, 1), -25, 25))
        {
            tmp = OBJ_VALUE(def_armour, 1) + OBJ_VALUE(def_armour, 2);
            def_bonus += tmp;

            if (pStat)
            {
                pStat->append(diku::format_to_str("Armor bonus         :       %4d  <br/>", tmp));
            }
        }
        else
        {
            slog(LOG_ALL, 0, "Illegal armour bonus.");
        }
    }
    else
    {
        def_armour_type = CHAR_NATURAL_ARMOUR(def);
    }

    if (pAtt_weapon_type)
    {
        *pAtt_weapon_type = att_wpn_type;
    }

    if (pAtt_weapon)
    {
        *pAtt_weapon = att_wpn;
    }

    if (pDef_armour_type)
    {
        *pDef_armour_type = def_armour_type;
    }

    if (pDef_armour)
    {
        *pDef_armour = def_armour;
    }

    int def_dex = effective_dex(def);
    int att_abil = weapon_attack_ability(att, att_wpn_type);

    if (pStat)
    {
        pStat->append(" --- SUMMARY ---<br/>");
        pStat->append(diku::format_to_str("Abil wpn vs def DB  :  %4d    %4d<br/>", att_abil, def_dex));
        pStat->append(diku::format_to_str("Wpn knowledge       :  %4d    %4d<br/>", att_wpn_knowledge, def_wpn_knowledge));
        pStat->append(diku::format_to_str("Att bonus vs Def    :  %4d    %4d<br/>", att_bonus, def_bonus));
        // snprintf(buf, sizeof(buf), "Level vs level      :  %4d    %4d<br/>", CHAR_LEVEL(att), CHAR_LEVEL(def));
        // pStat->append(buf);
    }

    if (CHAR_AWAKE(def))
    {
        hm = 2 * (att_abil - def_dex) + 2 * (att_wpn_knowledge - def_wpn_knowledge) + (att_bonus - def_bonus);

        if (pStat)
        {
            pStat->append(diku::format_to_str("Result              :  %4d<br/>", hm));
        }
    }
    else
    {
        hm = 2 * att_abil + 2 * att_wpn_knowledge + att_bonus;
        if (pStat)
        {
            pStat->append(diku::format_to_str("Result (sleep def)  :  %4d<br/>", hm));
        }
    }

    if (pStat)
    {
        pStat->append("Result = 2*(att_abil-def_dex) + 2*(att_wpn - def_wpn) + (att_bonus - def_bonus)) + roll<br/>");
        pStat->append("<br/>");

        int dam5 = weapon_damage(roll_boost(5, CHAR_LEVEL(att)) + hm, att_wpn_type, def_armour_type);
        int dam50 = weapon_damage(roll_boost(50, CHAR_LEVEL(att)) + hm, att_wpn_type, def_armour_type);
        int dam95 = weapon_damage(roll_boost(95, CHAR_LEVEL(att)) + hm, att_wpn_type, def_armour_type);

        pStat->append(diku::format_to_str("Weapon dmg (5/50/95) : %4d %4d %4d<br/>", dam5, dam50, dam95));

        pStat->append(diku::format_to_str("Rounds to kill def = %d<br/>", UNIT_MAX_HIT(def) / MAX(1, (dam5 + dam50 + dam95) / 3)));

        pStat->append("</pre>");
    }

    return MAX(-50, hm);
}

int base_melee(unit_data *att, unit_data *def, int hit_loc)
{
    int ocp = 0;
    int bonus = 0;
    unit_data *ocf = nullptr;

    assert(CHAR_COMBAT(def));

    ocp = CHAR_POS(def);
    ocf = CHAR_FIGHTING(def);
    CHAR_COMBAT(def)->setMelee(att);
    CHAR_POS(def) = POSITION_FIGHTING;

    bonus = melee_bonus(att, def, hit_loc, nullptr, nullptr, nullptr, nullptr);

    CHAR_POS(def) = ocp;
    CHAR_COMBAT(def)->setMelee(ocf);

    return bonus;
}

/* Disregards how likely it is to be hit and only concentrates on the       */
/* danger involved in combat (i.e. how fast would you die in worst case)    */
/* Returns number of rounds it takes att to kill def                        */

int base_consider(unit_data *att, unit_data *def, std::string *pStr)
{
    int ocp = 0;
    int bonus = 0;
    int att_wpn_type = 0;
    int def_arm_type = 0;
    int dam = 0;

    ocp = CHAR_POS(def);
    CHAR_FIGHTING(def);
    CHAR_POS(def) = POSITION_FIGHTING;

    att_wpn_type = WPN_ROOT;

    bonus = melee_bonus(att, def, WEAR_BODY, &att_wpn_type, nullptr, &def_arm_type, nullptr, 1, pStr);
    bonus -= 25; // Compensate for the -25 for def not fighting attacker

    CHAR_POS(def) = ocp;

    dam = weapon_damage(5 + bonus, att_wpn_type, def_arm_type);
    dam += weapon_damage(50 + bonus, att_wpn_type, def_arm_type);
    dam += weapon_damage(95 + bonus, att_wpn_type, def_arm_type);
    dam /= 3;

    if (dam <= 0)
    {
        return 100;
    }
    else
    {
        return UNIT_MAX_HIT(def) / dam; /* Rounds to die.... */
    }
}

void do_consider(unit_data *ch, char *arg, const command_info *cmd)
{
    unit_data *vict = nullptr;
    int rtd = 0;
    char *oarg = arg;
    std::string str;

    if (IS_PC(ch) && PC_SKI_SKILL(ch, SKI_CONSIDER) == 0)
    {
        send_to_char("You must practice first.<br/>", ch);
        return;
    }

    if (str_is_empty(arg))
    {
        send_to_char("You consider Life, the Universe and everything.<br/>", ch);
        return;
    }

    if ((vict = find_unit(ch, &arg, nullptr, FIND_UNIT_SURRO)) == nullptr)
    {
        send_to_char("No such person around.<br/>", ch);
        return;
    }

    if (!IS_CHAR(vict))
    {
        send_to_char("It must be dead already?<br/>", ch);
        return;
    }

    if (vict == ch)
    {
        send_to_char("Easy! Very easy indeed!<br/>", ch);
        return;
    }

    if (IS_SET(CHAR_FLAGS(vict), CHAR_PROTECTED))
    {
        send_to_char("Why not just walk to the jail yourself?<br/>", ch);
        return;
    }

    if (CHAR_LEVEL(vict) - 10 >= CHAR_LEVEL(ch))
    {
        act("$3e is probably out of your league.", A_SOMEONE, ch, cActParameter(), vict, TO_CHAR);
    }

    rtd = base_consider(vict, ch, &str);

    if (IS_IMMORTAL(ch))
    {
        act("Rounds to be killed by $3n = $2d.", A_SOMEONE, ch, &rtd, vict, TO_CHAR);
        send_to_char(str.c_str(), ch);

        str = "";
        rtd = base_consider(ch, vict, &str);
        act("Rounds to kill $3n = $2d.", A_SOMEONE, ch, &rtd, vict, TO_CHAR);
        send_to_char(str.c_str(), ch);
        return;
    }

    if (rtd <= 1)
    {
        send_to_char("RUN AWAY!<br/>", ch);
    }
    else if (rtd <= 2)
    {
        send_to_char("You ARE mad!<br/>", ch);
    }
    else if (rtd <= 3)
    {
        send_to_char("Very risky indeed!<br/>", ch);
    }
    else if (rtd <= 4)
    {
        send_to_char("Quite a risk.<br/>", ch);
    }
    else if (rtd <= 5)
    {
        send_to_char("Perhaps you would have time to flee.<br/>", ch);
    }
    else if (rtd <= 7)
    {
        send_to_char("You got a fair chance of fleeing.<br/>", ch);
    }
    else if (rtd <= 10)
    {
        send_to_char("You got a very good chance of fleeing.<br/>", ch);
    }
    else
    {
        send_to_char("Plenty of time to flee.<br/>", ch);
    }

    send_done(ch, nullptr, vict, rtd, cmd, oarg);
}

/* Return the quality modifier for the given monster. Modifier depends on */
/* the hand or weapon quality and armour or toughness quality.            */
int experience_modification(unit_data *att, unit_data *def)
{
    return base_melee(def, att, WEAR_BODY) - base_melee(att, def, WEAR_BODY);
}
