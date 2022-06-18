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
#include "utility.h"
#include "utils.h"
#include "vmelimits.h"

#include <ctime>

void do_save(unit_data *ch, char *arg, const command_info *cmd)
{
    if (!ch->isPC())
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

/**
 * @note  This function is also called from DIL  from the dil function reset_race()
 */
void race_adjust(unit_data *ch)
{
    base_race_info_type *sex_race = nullptr;
    race_info_type *my_race = nullptr;

    assert(ch->isPC());
    assert(is_in(CHAR_RACE(ch), 0, PC_RACE_MAX - 1));

    my_race = &g_race_info[CHAR_RACE(ch)];

    if (ch->getSex() == SEX_MALE)
    {
        sex_race = &my_race->male;
    }
    else
    {
        sex_race = &my_race->female;
    }

    auto weight = sex_race->weight + dice(sex_race->weight_dice.reps, sex_race->weight_dice.size);
    ch->setWeight(weight);
    ch->setBaseWeight(weight);

    ch->setSize(sex_race->height + dice(sex_race->height_dice.reps, sex_race->height_dice.size));

    UPC(ch)->setLifespan(sex_race->lifespan + dice(sex_race->lifespan_dice.reps, sex_race->lifespan_dice.size));

    const int years = my_race->age + dice(my_race->age_dice.reps, my_race->age_dice.size);

    const auto value = PC_TIME(ch).getPlayerCharacterCreationTime() - years * SECS_PER_MUD_YEAR;
    UPC(ch)->getPCTimeInformation().setPlayerBirthday(value);
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

    UPC(ch)->setVirtualPlayerLevel(START_LEVEL);
    UCHAR(ch)->setLevel(START_LEVEL);

    UPC(ch)->setAbilityPoints(0);
    UPC(ch)->setSkillPoints(0);

    for (i = 0; i < ABIL_TREE_MAX; i++)
    {
        UCHAR(ch)->setAbilityAtIndexTo(i, 0);
        UPC(ch)->setAbilityLevelAtIndexTo(i, 0);
    }

    for (i = 0; i < SKI_TREE_MAX; i++)
    {
        UPC(ch)->setSkillAtIndexTo(i, 0);
        UPC(ch)->setSkillLevelAtIndexTo(i, 0);
    }

    for (i = 0; i < SPL_TREE_MAX; i++)
    {
        UPC(ch)->setSpellLevelAtIndexTo(i, 0);
        if (i < SPL_GROUP_MAX)
        {
            UPC(ch)->setSpellSKillAtIndexTo(i, 1); // So resistance spells work!
        }
        else
        {
            UPC(ch)->setSpellSKillAtIndexTo(i, 0);
        }
    }

    for (i = 0; i < WPN_TREE_MAX; i++)
    {
        UPC(ch)->setWeaponSkillLevelAtIndexTo(i, 0);
        if (i < WPN_GROUP_MAX)
        {
            UPC(ch)->setWeaponSkillAtIndexTo(i, 1); // So resistance spells work!
        }
        else
        {
            UPC(ch)->setWeaponSkillAtIndexTo(i, 0);
        }
    }
}

/**
 * Can be called once when a new player is created from nanny().
 */
void start_player(unit_data *ch)
{
    assert(CHAR_LEVEL(ch) == 0);
    assert(ch->getUnitContains() == nullptr);
    assert(ch->isPC());
    assert(CHAR_DESCRIPTOR(ch)); // Needed to copy pwd

    race_adjust(ch);
    // Obsolete race_cost(ch);
    points_reset(ch);

    UCHAR(ch)->setPlayerExperience(required_xp(PC_VIRTUAL_LEVEL(ch)));

    set_title(ch);

    UCHAR(ch)->setAttackType(WPN_FIST);
    UCHAR(ch)->setNaturalArmor(ARM_CLOTHES);

    UPC(ch)->setConditionAtIndexTo(THIRST, 24);
    UPC(ch)->setConditionAtIndexTo(FULL, 24);
    UPC(ch)->setConditionAtIndexTo(DRUNK, 0);

    PC_TIME(ch).setTotalTimePlayedInSeconds(0);

    UPC(ch)->setPCFlag(PC_ECHO);
    UPC(ch)->setPCFlag(PC_PROMPT);

    if (!ch->isEvil())
    {
        UCHAR(ch)->setCharacterFlag(CHAR_PEACEFUL);
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

    ch->setMaximumHitpoints(hit_limit(ch));
    ch->setCurrentHitpoints(hit_limit(ch));
    UCHAR(ch)->setMana(mana_limit(ch));
    UCHAR(ch)->setEndurance(move_limit(ch));
}
