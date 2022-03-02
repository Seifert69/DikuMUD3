/*
 $Author: All $
 $RCSfile: act_other.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.6 $
 */

#include "account.h"
#include "comm.h"
#include "common.h"
#include "descriptor_data.h"
#include "dilrun.h"
#include "handler.h"
#include "interpreter.h"
#include "pcsave.h"
#include "skills.h"
#include "structs.h"
#include "utility.h"
#include "utils.h"
#include "vmelimits.h"

#include <ctime>

void do_save(unit_data *ch, char *arg, const command_info *cmd)
{
    if (!IS_PC(ch))
    {
        return;
    }

    if (!PC_IS_UNSAVED(ch) && CHAR_DESCRIPTOR(ch))
    {
        if (CHAR_LEVEL(ch) < 200 && difftime(time(nullptr), CHAR_DESCRIPTOR(ch)->getLastLogonTime()) < 60)
        {
            send_to_char("You must wait a minute between saves.<br/>", ch);
            return;
        }
    }
    else
    {
        send_to_char("You are no longer a guest on this game.<br/>", ch);
        g_possible_saves--;
    }
    act("Saving $1n.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);

    if (account_is_closed(ch))
    {
        extract_unit(ch); /* Saves */
        return;
    }

    save_player(ch);
    save_player_contents(ch, TRUE);
}

/*
 This function is also called from DIL
 from the dil function reset_race()
*/
void race_adjust(unit_data *ch)
{
    base_race_info_type *sex_race = nullptr;
    race_info_type *my_race = nullptr;

    assert(IS_PC(ch));
    assert(is_in(CHAR_RACE(ch), 0, PC_RACE_MAX - 1));

    my_race = &g_race_info[CHAR_RACE(ch)];

    if (CHAR_SEX(ch) == SEX_MALE)
    {
        sex_race = &my_race->male;
    }
    else
    {
        sex_race = &my_race->female;
    }

    UNIT_WEIGHT(ch) = UNIT_BASE_WEIGHT(ch) = sex_race->weight + dice(sex_race->weight_dice.reps, sex_race->weight_dice.size);

    UNIT_SIZE(ch) = sex_race->height + dice(sex_race->height_dice.reps, sex_race->height_dice.size);

    PC_LIFESPAN(ch) = sex_race->lifespan + dice(sex_race->lifespan_dice.reps, sex_race->lifespan_dice.size);

    PC_TIME(ch).birth = PC_TIME(ch).creation;

    int years = 0;

    years = my_race->age + dice(my_race->age_dice.reps, my_race->age_dice.size);

    PC_TIME(ch).birth -= years * SECS_PER_MUD_YEAR;
}

/* OBSOLETE. Should only be called when initializing a new player (or rerolling)
void race_cost(class unit_data *ch)
{
    int i;

    for (i = 0; i < ABIL_TREE_MAX; i++)
        PC_ABI_COST(ch, i) = racial_ability[i][CHAR_RACE(ch)];

    for (i = 0; i < WPN_TREE_MAX; i++)
        PC_WPN_COST(ch, i) = racial_weapons[i][CHAR_RACE(ch)];

    for (i = 0; i < SKI_TREE_MAX; i++)
        PC_SKI_COST(ch, i) = racial_skills[i][CHAR_RACE(ch)];

    for (i = 0; i < SPL_TREE_MAX; i++)
        PC_SPL_COST(ch, i) = racial_spells[i][CHAR_RACE(ch)];
}*/

void points_reset(unit_data *ch)
{
    int i = 0;

    PC_VIRTUAL_LEVEL(ch) = CHAR_LEVEL(ch) = START_LEVEL;

    PC_ABILITY_POINTS(ch) = 0;
    PC_SKILL_POINTS(ch) = 0;

    for (i = 0; i < ABIL_TREE_MAX; i++)
    {
        CHAR_ABILITY(ch, i) = 0;
        PC_ABI_LVL(ch, i) = 0;
    }

    for (i = 0; i < SKI_TREE_MAX; i++)
    {
        PC_SKI_SKILL(ch, i) = 0;
        PC_SKI_LVL(ch, i) = 0;
    }

    for (i = 0; i < SPL_TREE_MAX; i++)
    {
        PC_SPL_LVL(ch, i) = 0;
        if (i < SPL_GROUP_MAX)
        {
            PC_SPL_SKILL(ch, i) = 1; /* So resistance spells work! */
        }
        else
        {
            PC_SPL_SKILL(ch, i) = 0;
        }
    }

    for (i = 0; i < WPN_TREE_MAX; i++)
    {
        PC_WPN_LVL(ch, i) = 0;
        if (i < WPN_GROUP_MAX)
        {
            PC_WPN_SKILL(ch, i) = 1; /* So resistance spells work! */
        }
        else
        {
            PC_WPN_SKILL(ch, i) = 0;
        }
    }
}

/* Can be called once when a new player is created from nanny().  */

void start_player(unit_data *ch)
{
    assert(CHAR_LEVEL(ch) == 0);
    assert(UNIT_CONTAINS(ch) == nullptr);
    assert(IS_PC(ch));
    assert(CHAR_DESCRIPTOR(ch)); // Needed to copy pwd

    race_adjust(ch);
    // Obsolete race_cost(ch);
    points_reset(ch);

    CHAR_EXP(ch) = required_xp(PC_VIRTUAL_LEVEL(ch));

    set_title(ch);

    CHAR_ATTACK_TYPE(ch) = WPN_FIST;
    CHAR_NATURAL_ARMOUR(ch) = ARM_CLOTHES;

    PC_COND(ch, THIRST) = 24;
    PC_COND(ch, FULL) = 24;
    PC_COND(ch, DRUNK) = 0;

    PC_TIME(ch).played = 0;

    SET_BIT(PC_FLAGS(ch), PC_ECHO);
    SET_BIT(PC_FLAGS(ch), PC_PROMPT);

    if (!UNIT_IS_EVIL(ch))
    {
        SET_BIT(CHAR_FLAGS(ch), CHAR_PEACEFUL);
    }

    if (g_playerinit_tmpl)
    {
        /* Call DIL to see if we should init the player in any other way. */
        dilprg *prg = dil_copy_template(g_playerinit_tmpl, ch, nullptr);

        if (prg)
        {
            prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file
            dil_activate(prg);
        }
    }

    UNIT_MAX_HIT(ch) = UNIT_HIT(ch) = hit_limit(ch);
    CHAR_MANA(ch) = mana_limit(ch);
    CHAR_ENDURANCE(ch) = move_limit(ch);
}
