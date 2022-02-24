/*
 $Author: tperry $
 $RCSfile: magic.cpp,v $
 $Date: 2003/01/02 23:39:20 $
 $Revision: 2.4 $
 */

#include "magic.h"

#include "comm.h"
#include "common.h"
#include "dbfind.h"
#include "dil.h"
#include "dilrun.h"
#include "experience.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "modify.h"
#include "skills.h"
#include "slog.h"
#include "spell_parser.h"
#include "spells.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"

/* Returns TRUE when effect is shown by DIL */

int dil_effect(char *pStr, spell_args *sa)
{
    if (str_is_empty(pStr))
    {
        return FALSE;
    }

    diltemplate *tmpl = nullptr;

    tmpl = find_dil_template(pStr);

    if (tmpl == nullptr)
    {
        return FALSE;
    }

    if (tmpl->argc != 3)
    {
        slog(LOG_ALL, 0, "Spell DIL effect wrong arg count.");
        return FALSE;
    }

    if (tmpl->argt[0] != DILV_UP)
    {
        slog(LOG_ALL, 0, "Spell DIL effect arg 1 mismatch.");
        return FALSE;
    }

    if (tmpl->argt[1] != DILV_UP)
    {
        slog(LOG_ALL, 0, "Spell DIL effect arg 2 mismatch.");
        return FALSE;
    }

    if (tmpl->argt[2] != DILV_INT)
    {
        slog(LOG_ALL, 0, "Spell DIL effect arg 3 mismatch.");
        return FALSE;
    }

    dilprg *prg = nullptr;
    unit_fptr *fptr = nullptr;

    prg = dil_copy_template(tmpl, sa->caster, &fptr);
    if (prg)
    {
        prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file

        prg->fp->vars[0].val.unitptr = sa->medium;
        prg->fp->vars[1].val.unitptr = sa->target;
        prg->fp->vars[2].val.integer = sa->hm;

        dil_add_secure(prg, sa->medium, prg->fp->tmpl->core);
        dil_add_secure(prg, sa->target, prg->fp->tmpl->core);

        assert(fptr);

        spec_arg sarg;

        sarg.owner = prg->owner;
        sarg.activator = sa->caster;
        sarg.medium = nullptr;
        sarg.target = nullptr;
        sarg.pInt = nullptr;
        sarg.fptr = fptr;
        sarg.cmd = &g_cmd_auto_tick;
        sarg.arg = nullptr;
        sarg.mflags = SFB_TICK | SFB_AWARE;

        run_dil(&sarg);
    }

    return TRUE;
}

/* This procedure uses mana from a medium */
/* returns TRUE if ok, and FALSE if there was not enough mana.  */
/* wands and staffs uses one charge, no matter what 'mana' is. --HHS */
ubit1 use_mana(unit_data *medium, int mana)
{
    if (IS_CHAR(medium))
    {
        if (CHAR_MANA(medium) >= mana)
        {
            CHAR_MANA(medium) -= mana;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else if (IS_OBJ(medium))
    {
        switch (OBJ_TYPE(medium))
        {
            case ITEM_STAFF:
            case ITEM_WAND:
                if (OBJ_VALUE(medium, 1))
                {
                    --OBJ_VALUE(medium, 1);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
                break;
            default:
                return FALSE; /* no mana in other objects */
        }
    }
    return FALSE; /* no mana/charge in this type of unit */
}

/* Determines if healing combat mana should be cast?? */
ubit1 cast_magic_now(unit_data *ch, int mana)
{
    int hleft = 0;
    int sleft = 0;

    if (CHAR_MANA(ch) > mana)
    {
        if (UNIT_MAX_HIT(ch) <= 0)
        {
            hleft = 0;
        }
        else
        {
            hleft = (100 * UNIT_HIT(ch)) / UNIT_MAX_HIT(ch);
        }

        sleft = mana ? CHAR_MANA(ch) / mana : 20;

        if (sleft >= 5)
        {
            return TRUE;
        }

        if (hleft >= 95)
        {
            return FALSE;
        }
        else if (hleft > 80)
        { /* Small chance, allow heal to be possible */
            return (number(1, MAX(1, 16 - 2 * sleft)) == 1);
        }
        else if (hleft > 50)
        {
            return (number(1, MAX(1, 6 - sleft)) == 1);
        }
        else if (hleft > 40)
        {
            return (number(1, MAX(1, 4 - sleft)) == 1);
        }
        else
        {
            return TRUE;
        }
    }
    return FALSE;
}

/* ===================================================================== */

/* down and up is how much percentage that num will be adjusted randomly */
/* up or down                                                            */
int variation(int num, int d, int u)
{
    return number(num - (num * d) / 100, num + (num * u) / 100);
}

/* See if unit is allowed to be transferred away from its surroundings */
/* i.e. if a player is allowed to transfer out of jail, etc.           */
ubit1 may_teleport_away(unit_data *unit)
{
    if (IS_SET(UNIT_FLAGS(unit), UNIT_FL_NO_TELEPORT))
    {
        return FALSE;
    }

    while ((unit = UNIT_IN(unit)))
    {
        if (IS_SET(UNIT_FLAGS(unit), UNIT_FL_NO_TELEPORT))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/* See if unit is allowed to be transferred to 'dest' */
ubit1 may_teleport_to(unit_data *unit, unit_data *dest)
{
    if (unit == dest || IS_SET(UNIT_FLAGS(dest), UNIT_FL_NO_TELEPORT) || unit_recursive(unit, dest) ||
        UNIT_WEIGHT(unit) + UNIT_WEIGHT(dest) > UNIT_CAPACITY(dest))
    {
        return FALSE;
    }

    do
    {
        if (IS_SET(UNIT_FLAGS(dest), UNIT_FL_NO_TELEPORT))
        {
            return FALSE;
        }
    } while ((dest = UNIT_IN(dest)));

    return TRUE;
}

/* See if unit is allowed to be transferred to 'dest' */
ubit1 may_teleport(unit_data *unit, unit_data *dest)
{
    return may_teleport_away(unit) && may_teleport_to(unit, dest);
}

/* ===================================================================== */

int object_power(unit_data *unit)
{
    if (IS_OBJ(unit))
    {
        if (OBJ_TYPE(unit) == ITEM_POTION || OBJ_TYPE(unit) == ITEM_SCROLL || OBJ_TYPE(unit) == ITEM_WAND || OBJ_TYPE(unit) == ITEM_STAFF)
        {
            return OBJ_VALUE(unit, 0);
        }
        else
        {
            return OBJ_RESISTANCE(unit);
        }
    }
    else
    {
        return 0;
    }
}

int room_power(unit_data *unit)
{
    if (IS_ROOM(unit))
    {
        return ROOM_RESISTANCE(unit);
    }
    else
    {
        return 0;
    }
}

/* Return how well a unit defends itself against a spell, by using */
/* its skill (not its power) - That is the group (or attack).      */
/*                                                                 */
int spell_defense_skill(unit_data *unit, int spell)
{
    int max = 0;

    if (IS_PC(unit))
    {
        if (TREE_ISLEAF(g_SplColl.tree, spell))
        {
            max = PC_SPL_SKILL(unit, spell) / 2;
        }
        else
        {
            max = PC_SPL_SKILL(unit, spell);
        }

        while (!TREE_ISROOT(g_SplColl.tree, spell))
        {
            spell = TREE_PARENT(g_SplColl.tree, spell);

            if (PC_SPL_SKILL(unit, spell) > max)
            {
                max = PC_SPL_SKILL(unit, spell);
            }
        }

        return max;
    }

    if (IS_OBJ(unit))
    {
        return object_power(unit); //  Philosophical... / 2 ?
    }

    if (IS_NPC(unit))
    {
        if (TREE_ISLEAF(g_SplColl.tree, spell))
        {
            spell = TREE_PARENT(g_SplColl.tree, spell);
        }

        if (TREE_ISROOT(g_SplColl.tree, spell))
        {
            max = NPC_SPL_SKILL(unit, spell);
        }
        else
        {
            max = NPC_SPL_SKILL(unit, spell) / 2;
        }

        while (!TREE_ISROOT(g_SplColl.tree, spell))
        {
            spell = TREE_PARENT(g_SplColl.tree, spell);

            if (NPC_SPL_SKILL(unit, spell) > max)
            {
                max = NPC_SPL_SKILL(unit, spell);
            }
        }

        return max;
    }
    else
    {
        return room_power(unit);
    }
}

/* Return how well a unit attacks with a spell, by using its skill */
/* (not its power).                                                */
/*                                                                 */
int spell_attack_skill(unit_data *unit, int spell)
{
    if (IS_PC(unit))
    {
        return PC_SPL_SKILL(unit, spell);
    }

    if (IS_OBJ(unit))
    {
        return object_power(unit);
    }

    if (IS_NPC(unit))
    {
        if (TREE_ISLEAF(g_SplColl.tree, spell))
        {
            spell = TREE_PARENT(g_SplColl.tree, spell);
        }

        return NPC_SPL_SKILL(unit, spell);
    }
    else
    {
        return room_power(unit);
    }
}

/* Return the power in a unit for a given spell type     */
/* For CHAR's determine if Divine or Magic power is used */
int spell_attack_ability(unit_data *medium, int spell)
{
    if (IS_CHAR(medium))
    {
        /* Figure out if char will use Divine or Magic powers */
        assert(g_spell_info[spell].realm == ABIL_MAG || g_spell_info[spell].realm == ABIL_DIV);

        return CHAR_ABILITY(medium, g_spell_info[spell].realm);
    }

    return spell_attack_skill(medium, spell);
}

int spell_ability(unit_data *u, int ability, int spell)
{
    if (IS_CHAR(u))
    {
        return CHAR_ABILITY(u, ability);
    }
    else
    {
        return spell_defense_skill(u, spell);
    }
}

/* ===================================================================== */

/* Use this function when a spell caster 'competes' against something  */
/* else. These are typically aggressive spells like 'sleep' etc. where */
/* the defender is entiteled a saving throw.                           */
/*                                                                     */
int spell_resistance(unit_data *att, unit_data *def, int spell)
{
    if ((att == nullptr) || (def == nullptr))
    {
        slog(LOG_ALL, 0, "Attacker or defender NULL in spell_resistance for spell %d.", spell);
        return -1;
    }

    if (IS_CHAR(att) && IS_CHAR(def))
    {
        return resistance_skill_check(spell_attack_ability(att, spell),
                                      spell_ability(def, ABIL_BRA, spell),
                                      spell_attack_skill(att, spell),
                                      spell_defense_skill(def, spell));
    }
    else
    {
        return resistance_skill_check(spell_attack_ability(att, spell),
                                      spell_ability(def, ABIL_BRA, spell),
                                      spell_attack_skill(att, spell),
                                      spell_defense_skill(def, spell));
    }
}

/* Use this function when a spell caster competes against his own */
/* skill/ability when casting a spell. For example healing spells */
/* create food etc.                                               */
/* Returns how well it went "100" is perfect > better.            */
int spell_cast_check(unit_data *att, int spell)
{
    return resistance_skill_check(spell_attack_ability(att, spell), 0, spell_attack_skill(att, spell), 0);
}

/* ===================================================================== */

/* Use this function from attack spells, it will do all the  */
/* nessecary work for you                                    */
/* Qty is 1 for small, 2 for medium and 3 for large versions */
/* of each spell                                             */
int spell_offensive(spell_args *sa, int spell_number, int bonus)
{
    int def_shield_bonus = 0;
    int armour_type = 0;
    int hit_loc = 0;
    int roll = 0;
    int bEffect = 0;
    unit_data *def_shield = nullptr;

    /* Does the spell perhaps only hit head / body? All?? Right now I
        do it randomly */
    hit_loc = hit_location(sa->caster, sa->target);

    bonus += spell_bonus(sa->caster, sa->medium, sa->target, hit_loc, spell_number, &armour_type, nullptr, nullptr);

    roll = open100();
    roll_description(sa->caster, "spell", roll);
    bonus += roll_boost(roll, CHAR_LEVEL(sa->caster));

    sa->hm = chart_damage(bonus, &(g_spell_chart[spell_number].element[armour_type]));

    def_shield_bonus = shield_bonus(sa->caster, sa->target, &def_shield);

    if (def_shield && g_spell_info[spell_number].shield != SHIELD_M_USELESS)
    {
        if ((g_spell_info[spell_number].shield == SHIELD_M_BLOCK) && (number(1, 100) <= def_shield_bonus))
        {
            damage_object(sa->target, def_shield, sa->hm);
            damage(sa->caster, sa->target, def_shield, 0, MSG_TYPE_SPELL, spell_number, WEAR_SHIELD);
            return 0;
        }
        if ((g_spell_info[spell_number].shield == SHIELD_M_REDUCE) && (number(1, 100) <= def_shield_bonus))
        {
            sa->hm -= (sa->hm * def_shield_bonus) / 100;
        }
    }

    bEffect = dil_effect(sa->pEffect, sa);

    if (sa->hm > 0)
    {
        damage(sa->caster, sa->target, nullptr, sa->hm, MSG_TYPE_SPELL, spell_number, COM_MSG_EBODY, !bEffect);
    }
    else
    {
        damage(sa->caster, sa->target, nullptr, 0, MSG_TYPE_SPELL, spell_number, COM_MSG_MISS, !bEffect);
    }

    return sa->hm;
}

/*last spells*/

void spell_clear_skies(spell_args *sa)
{
    unit_data *room = unit_room(sa->caster);

    if ((sa->hm / 20 <= 0) || (IS_SET(UNIT_FLAGS(room), UNIT_FL_NO_WEATHER | UNIT_FL_INDOORS)))
    {
        act("Nothing happens.", A_ALWAYS, sa->caster, cActParameter(), cActParameter(), TO_CHAR);
        return;
    }

    unit_zone(sa->caster)->getWeather().incrementChangeBy(sa->hm / 20);
    unit_zone(sa->caster)->getWeather().setChange(MIN(unit_zone(sa->caster)->getWeather().getChange(), 12));

    act("You feel a warm breeze.", A_ALWAYS, sa->caster, cActParameter(), cActParameter(), TO_ALL);
}

void spell_storm_call(spell_args *sa)
{
    unit_data *room = unit_room(sa->caster);

    if ((sa->hm / 20 <= 0) || (IS_SET(UNIT_FLAGS(room), UNIT_FL_NO_WEATHER | UNIT_FL_INDOORS)))
    {
        act("Nothing happens.", A_ALWAYS, sa->caster, cActParameter(), cActParameter(), TO_CHAR);
        return;
    }

    unit_zone(sa->caster)->getWeather().decrementChangeBy(sa->hm / 20);
    unit_zone(sa->caster)->getWeather().setChange(MAX(unit_zone(sa->caster)->getWeather().getChange(), -12));

    act("A cold wind chills you to the bone.", A_ALWAYS, sa->caster, cActParameter(), cActParameter(), TO_ALL);
}
