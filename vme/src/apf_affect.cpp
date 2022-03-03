/*
 $Author: All $
 $RCSfile: apf_affect.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.4 $
 */

#include "affect.h"
#include "handler.h"
#include "modify.h"
#include "skills.h"
#include "slog.h"
#include "structs.h"
#include "unit_affected_type.h"
#include "utility.h"
#include "utils.h"
#include "vmelimits.h"

ubit1 raw_destruct_affect(unit_affected_type *af)
{
    unlink_affect(af); /* registers and frees later */
    return FALSE;      /* CANCEL */
}

ubit1 skill_overflow(int skill, int change, ubit1 set)
{
    if (set)
    {
        if (skill < 1)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*                                                      */
/* Data[0] Must contain bits to set in CHAR_FLAGS()     */
/*                                                      */
ubit1 apf_mod_char_flags(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    unit_affected_type *taf = nullptr;

    assert(IS_CHAR(unit));

    if (set)
    {
        SET_BIT(CHAR_FLAGS(unit), (ubit32)af->getDataAtIndex(0));
    }
    else
    {
        REMOVE_BIT(CHAR_FLAGS(unit), (ubit32)af->getDataAtIndex(0));

        /* After bit has been removed, call all apply functions  */
        /* which will set bits [and one of these may be the bit  */
        /* which this function just removed. Notice that this    */
        /* implies that a character can not permanently have     */
        /* these bits set, since a call of this function will    */
        /* remove them                                           */
        for (taf = UNIT_AFFECTED(af->cgetOwner()); taf; taf = taf->getNext())
        {
            if ((taf != af) && (taf->getApplyFI() == APF_MOD_CHAR_FLAGS))
            {
                SET_BIT(CHAR_FLAGS(unit), (ubit32)taf->getDataAtIndex(0));
            }
        }
    }
    return TRUE;
}

/*                                                      */
/* Data[0] Must contain bits to set in OBJ_FLAGS() */
/*                                                      */
ubit1 apf_mod_obj_flags(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    unit_affected_type *taf = nullptr;

    assert(IS_OBJ(unit));

    if (set)
    {
        SET_BIT(OBJ_FLAGS(unit), (ubit32)af->getDataAtIndex(0));
    }
    else
    {
        REMOVE_BIT(OBJ_FLAGS(unit), (ubit32)af->getDataAtIndex(0));

        /* After bit has been removed, call all apply functions  */
        /* which will set bits [and one of these may be the bit  */
        /* which this function just removed. Notice that this    */
        /* implies that a object can not permanently have these  */
        /* bits set, since a call of this function will remove   */
        /* them                                                  */
        for (taf = UNIT_AFFECTED(af->cgetOwner()); taf; taf = taf->getNext())
        {
            if ((taf != af) && (taf->getApplyFI() == APF_MOD_OBJ_FLAGS))
            {
                SET_BIT(OBJ_FLAGS(unit), (ubit32)taf->getDataAtIndex(0));
            }
        }
    }
    return TRUE;
}

/*                                                      */
/* Data[0] Must contain bits to set in UNIT_FLAGS()     */
/*                                                      */
ubit1 apf_mod_unit_flags(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    unit_affected_type *taf = nullptr;

    if (set)
    {
        SET_BIT(UNIT_FLAGS(unit), (ubit16)af->getDataAtIndex(0));
    }
    else
    {
        REMOVE_BIT(UNIT_FLAGS(unit), (ubit16)af->getDataAtIndex(0));

        /* After bit has been removed, call all apply functions  */
        /* which will set bits [and one of these may be the bit  */
        /* which this function just removed. Notice that this    */
        /* implies that a character can not permanently have     */
        /* these bits set, since a call of this function will    */
        /* remove them                                           */
        for (taf = UNIT_AFFECTED(af->cgetOwner()); taf; taf = taf->getNext())
        {
            if ((taf != af) && (taf->getApplyFI() == APF_MOD_UNIT_FLAGS))
            {
                SET_BIT(UNIT_FLAGS(unit), (ubit16)taf->getDataAtIndex(0));
            }
        }
    }
    return TRUE;
}

ubit1 apf_weapon_adj(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    int modify = 0;

    if (!IS_CHAR(unit))
    {
        slog(LOG_ALL, 0, "ERROR: Affect weapon groups on room/obj %s@%s", UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit));
        return TRUE;
    }

    modify = af->getDataAtIndex(0);
    if (IS_NPC(unit))
    {
        while (modify > WPN_GROUP_MAX)
        {
            modify = TREE_PARENT(g_WpnColl.tree, modify);
        }
    }

    if (set)
    {
        if (IS_PC(unit))
        {
            if (skill_overflow(PC_WPN_SKILL(unit, modify), af->getDataAtIndex(1), set))
            {
                return raw_destruct_affect(af);
            }
            PC_WPN_SKILL(unit, modify) += af->getDataAtIndex(1);
        }
        else
        {
            NPC_WPN_SKILL(unit, modify) += af->getDataAtIndex(1);
        }
    }
    else
    {
        if (IS_PC(unit))
        {
            PC_WPN_SKILL(unit, modify) -= af->getDataAtIndex(1);
        }
        else
        {
            NPC_WPN_SKILL(unit, modify) -= af->getDataAtIndex(1);
        }
    }

    return TRUE;
}

/* NPC's are ignored, they don't have skills. */
ubit1 apf_skill_adj(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    if (!IS_CHAR(unit))
    {
        slog(LOG_ALL, 0, "ERROR: Affect skill groups on room/obj %s@%s", UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit));
        return TRUE;
    }

    if (set)
    {
        if (IS_PC(unit))
        {
            if (skill_overflow(PC_SKI_SKILL(unit, af->getDataAtIndex(0)), af->getDataAtIndex(1), set))
            {
                return raw_destruct_affect(af);
            }
            PC_SKI_SKILL(unit, af->getDataAtIndex(0)) += af->getDataAtIndex(1);
        }
    }
    else
    {
        if (IS_PC(unit))
        {
            PC_SKI_SKILL(unit, af->getDataAtIndex(0)) -= af->getDataAtIndex(1);
        }
    }

    return TRUE;
}

/* Data[0] must contain the SPL_XXX to change             */
/* Data[1] must contain the amount to change              */
/* Data[1] is added when set, and subtracted when not set */
/* Unit can be CHAR                                       */
ubit1 apf_spell_adj(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    int modify = 0;

    if (!IS_CHAR(unit))
    {
        slog(LOG_ALL, 0, "ERROR: Affect spell groups on room/obj %s@%s", UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit));
        return TRUE;
    }

    modify = af->getDataAtIndex(0);
    if (IS_NPC(unit))
    {
        while (modify > SPL_GROUP_MAX)
        {
            modify = TREE_PARENT(g_SplColl.tree, modify);
        }
    }

    if (set)
    {
        if (IS_PC(unit))
        {
            if (skill_overflow(PC_SPL_SKILL(unit, modify), af->getDataAtIndex(1), set))
            {
                return raw_destruct_affect(af);
            }
            PC_SPL_SKILL(unit, modify) += af->getDataAtIndex(1);
        }
        else
        {
            NPC_SPL_SKILL(unit, modify) += af->getDataAtIndex(1);
        }
    }
    else
    {
        if (IS_PC(unit))
        {
            PC_SPL_SKILL(unit, modify) -= af->getDataAtIndex(1);
        }
        else
        {
            NPC_SPL_SKILL(unit, modify) -= af->getDataAtIndex(1);
        }
    }

    return TRUE;
}

/* Data[0] must contain the ABIL_XXX to change            */
/* Data[1] must contain the amount to change              */
/* Data[1] is added when set, and subtracted when not set */
/* Unit must be a CHAR!                                   */
ubit1 apf_ability_adj(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    assert(IS_CHAR(unit));

    if (set)
    {
        if (skill_overflow(CHAR_ABILITY(unit, af->getDataAtIndex(0)), af->getDataAtIndex(1), set))
        {
            return raw_destruct_affect(af);
        }
        CHAR_ABILITY(unit, af->getDataAtIndex(0)) += af->getDataAtIndex(1);

        if (af->getDataAtIndex(0) == ABIL_HP)
        {
            UNIT_MAX_HIT(unit) = hit_limit(unit);
        }
    }
    else
    {
        CHAR_ABILITY(unit, af->getDataAtIndex(0)) -= af->getDataAtIndex(1);

        if (af->getDataAtIndex(0) == ABIL_HP)
        {
            UNIT_MAX_HIT(unit) = hit_limit(unit);
        }
    }

    return TRUE;
}

/* Data[0] = Amount of light sources */
ubit1 apf_light(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    if (!set)
    {
        af->setDataAtIndex(0, -af->getDataAtIndex(0));
    }

    if (IS_ROOM(unit))
    {
        UNIT_LIGHTS(unit) += af->getDataAtIndex(0);
    }

    modify_bright(unit, af->getDataAtIndex(0));

    if (!set)
    {
        af->setDataAtIndex(0, -af->getDataAtIndex(0));
    }

    return TRUE;
}

/* Data[0] = The new armour-type */
/* Data[1] = The original armour-type */
ubit1 apf_natural_armour(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    if (!IS_CHAR(unit))
    {
        return TRUE;
    }

    if (!is_in(af->getDataAtIndex(0), ARM_CLOTHES, ARM_PLATE) || (af->getDataAtIndex(0) < CHAR_NATURAL_ARMOUR(unit)))
    {
        af->setDataAtIndex(0, -1); /* Ineffective, other spell cancels this one */
        return TRUE;
    }

    if (set)
    {
        unit_affected_type *taf = nullptr;

        for (taf = UNIT_AFFECTED(unit); taf; taf = taf->getNext())
        {
            if ((taf->getID() == ID_NATURAL_ARMOUR) && (taf != af))
            {
                af->setDataAtIndex(1, taf->getDataAtIndex(1));
                break;
            }
        }

        CHAR_NATURAL_ARMOUR(unit) = MAX(CHAR_NATURAL_ARMOUR(unit), af->getDataAtIndex(0));
    }
    else
    {
        /* Restore value... */
        CHAR_NATURAL_ARMOUR(unit) = af->getDataAtIndex(1);
    }

    return TRUE;
}

/* Data[0] = The new speed            */
/* Data[2] = The original speed - [2] because it is not tested in the
             unequip_object affect remove match. */
ubit1 apf_speed(unit_affected_type *af, unit_data *unit, ubit1 set)
{
    if (!IS_CHAR(unit))
    {
        return TRUE;
    }

    if ((af->getDataAtIndex(0) < 4) || (af->getDataAtIndex(2) < 0))
    {
        return TRUE;
    }

    if (set)
    {
        unit_affected_type *taf = nullptr;

        af->setDataAtIndex(2, CHAR_SPEED(unit));

        for (taf = UNIT_AFFECTED(unit); taf; taf = taf->getNext())
        {
            if ((taf->getID() == ID_SPEED) && (taf != af))
            {
                af->setDataAtIndex(2, -1);
                break;
            }
        }

        if (taf == nullptr)
        {
            CHAR_SPEED(unit) = af->getDataAtIndex(0);
        }
    }
    else
    {
        CHAR_SPEED(unit) = af->getDataAtIndex(2);
    }

    return TRUE;
}

/* --------------------------------------------------------------------- */

apply_function_type g_apf[] = {{"Ability Adjustment", apf_ability_adj},
                               {"Spell skill adjustment", apf_spell_adj},
                               {"Modify bright/light", apf_light},
                               {"Modify CHAR_FLAGS()", apf_mod_char_flags},
                               {"Modify UNIT_FLAGS()", apf_mod_unit_flags},
                               {"Modify OBJ_FLAGS()", apf_mod_obj_flags},
                               {"Skill adjustment", apf_skill_adj},
                               {"Weapon adjustment", apf_weapon_adj},
                               {"Natural Armour Modification", apf_natural_armour},
                               {"Speed Adjustment", apf_speed}};
