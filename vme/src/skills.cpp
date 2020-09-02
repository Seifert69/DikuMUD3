/*
 $Author: All $
 $RCSfile: skills.cpp,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.3 $
 */

#include <stdlib.h>
#include <stdio.h>

#include "cmdload.h"
#include "structs.h"
#include "skills.h"
#include "common.h"
#include "utils.h"
#include "utility.h"
#include "comm.h"
#include "files.h"
#include "db.h"
#include "textutil.h"
#include "interpreter.h"
#include "trie.h"
#include "spelldef.h"

int hit_location_table[] = /* Determine by using 2d8 */
{
    WEAR_HEAD, /* 2..3   =>  4.68% for Head  */
    WEAR_HEAD,
    WEAR_HANDS, /* 4      =>  4.69% for Hands */
    WEAR_ARMS,  /* 5..7   => 23.43% for Arms  */
    WEAR_ARMS,
    WEAR_ARMS,
    WEAR_BODY, /* 8..10  => 34.38% for Body  */
    WEAR_BODY,
    WEAR_BODY,
    WEAR_LEGS, /* 11..14 => 28.12% for legs  */
    WEAR_LEGS,
    WEAR_LEGS,
    WEAR_LEGS,
    WEAR_FEET, /* 15..16 => 4.68% for feet   */
    WEAR_FEET
};


int wear_location_prop[WEAR_MAX] = {0,0,0,0,0,0,
22, // Body  [6]
3,  // Head  [7]
18, // Legs  [8]
3,  // Feet  [9]
3,  // Hands [10]
15, // Arms  [11]
0,0,0,0,0,0,0,0,0,0,0,0};

// HEAD (7), HANDS(10), ARMS(11), BODY(6), LEGS(8), FEET(9)

int hit_probability_table[] = /* Determine by using 2d8 */
{
    1, /* 2..3   =>  4.68% for Head  */
    2,
    3, /* 4      =>  4.69% for Hands */
    4,  /* 5..7   => 23.43% for Arms  */
    5,
    6,
    7, /* 8..10  => 34.38% for Body  */
    8,
    7,
    6, /* 11..14 => 28.12% for legs  */
    5,
    4,
    3,
    2, /* 15..16 => 4.68% for feet   */
    1
};  // 64 total


const char *professions[PROFESSION_MAX+1] = {PROFESSION_STRINGS, NULL};

struct profession_cost ability_prof_table[ABIL_TREE_MAX + 1];

struct profession_cost weapon_prof_table[WPN_TREE_MAX + 1];

struct profession_cost skill_prof_table[SKI_TREE_MAX + 1];

struct profession_cost spell_prof_table[SPL_TREE_MAX + 1];

const char *spl_text[SPL_TREE_MAX + 1];
struct tree_type spl_tree[SPL_TREE_MAX + 1];
sbit8 racial_spells[PC_RACE_MAX][SPL_TREE_MAX];
struct damage_chart_type spell_chart[SPL_TREE_MAX];

const char *pc_races[PC_RACE_MAX + 1];
const char *pc_race_adverbs[PC_RACE_MAX + 1];
struct race_info_type race_info[PC_RACE_MAX];

const char *ski_text[SKI_TREE_MAX + 1];
struct tree_type ski_tree[SKI_TREE_MAX + 1];
sbit8 racial_skills[PC_RACE_MAX][SKI_TREE_MAX];

const char *abil_text[ABIL_TREE_MAX + 1];
struct tree_type abil_tree[ABIL_TREE_MAX + 1];
sbit8 racial_ability[PC_RACE_MAX][ABIL_TREE_MAX];

const char *wpn_text[WPN_TREE_MAX + 1];
struct tree_type wpn_tree[WPN_TREE_MAX + 1];
sbit8 racial_weapons[PC_RACE_MAX][WPN_TREE_MAX];
struct damage_chart_type weapon_chart[WPN_TREE_MAX];
struct wpn_info_type wpn_info[WPN_TREE_MAX];

/* ===================================================================== */
int get_racial_ability(int nRace, int nAbility)
{
    if ((nRace < 0) or (nRace >= PC_RACE_MAX))
        return 0;

    if ((nAbility < 0) or (nAbility >= ABIL_TREE_MAX))
        return 0;
    
    return racial_ability[nRace][nAbility];
}

int get_racial_weapon(int nRace, int nWeapon)
{
    if ((nRace < 0) or (nRace >= PC_RACE_MAX))
        return 0;

    if ((nWeapon < 0) or (nWeapon >= WPN_TREE_MAX))
        return 0;
    
    return racial_weapons[nRace][nWeapon];
}

int get_racial_skill(int nRace, int nSkill)
{
    if ((nRace < 0) or (nRace >= PC_RACE_MAX))
        return 0;

    if ((nSkill < 0) or (nSkill >= SKI_TREE_MAX))
        return 0;
    
    return racial_skills[nRace][nSkill];
}

int get_racial_spells(int nRace, int nSpell)
{
    if ((nRace < 0) or (nRace >= PC_RACE_MAX))
        return 0;

    if ((nSpell < 0) or (nSpell >= SPL_TREE_MAX))
        return 0;
    
    return racial_spells[nRace][nSpell];
}

/* ===================================================================== */

void roll_description(class unit_data *att, const char *text, int roll)
{
    if (roll >= 200)
    {
        switch (roll / 100)
        {
        case 2:
            act("Great $2t!", A_ALWAYS, att, text, cActParameter(), TO_CHAR);
            act("$1n makes a great $2t!", A_ALWAYS, att, text, cActParameter(), TO_ROOM);
            break;

        case 3:
            act("Superb $2t!", A_ALWAYS, att, text, cActParameter(), TO_CHAR);
            act("$1n makes a superb $2t!", A_ALWAYS, att, text, cActParameter(), TO_ROOM);
            break;

        default:
            act("Divine $2t!", A_ALWAYS, att, text, cActParameter(), TO_CHAR);
            act("$1n makes a divinely inspired $2t!", A_ALWAYS, att, text, cActParameter(), TO_ROOM);
            break;
        }
    }
    else if (roll <= -100)
    {
        act("You fumble!", A_ALWAYS, att, text, cActParameter(), TO_CHAR);
        act("$1n fumbles!", A_ALWAYS, att, text, cActParameter(), TO_ROOM);
    }
}

int open_ended_roll(int size, int end)
{
    int i, s;

    s = i = number(1, size);

    if (i >= size - end + 1)  //1d100,5 means 96..100 (5% chance) xsuccess
    {
        do
        {
            s += (i = number(1, size));
        } while (i >= size - end + 1);
    }
    else if (i < end)        //1d100,5 means 1..4 (4% chance) of fumble
    {
        do
        {
            s -= (i = number(1, size));
        } while (i >= size - end + 1);
    }

    return s;
}

/* Copy of DIL function in skills.zon                            */
/* skillchecksa() checks 'self' versus the difficulty            */
/*    SkillIdx is the skill in question.                         */ 
/*    AbiIdx is the primary ability related                      */
/* Result > 0 is success                                         */
/* If skill and difficulty are equal => 50% chance success       */
int skillchecksa(class unit_data *u, int skillidx, int abiidx, int difficulty)
{
    int roll; 
    int skl;
    int hm;

    roll = open100();

    if (UNIT_TYPE(u) == UNIT_ST_PC)
        skl = (PC_SKI_SKILL(u, skillidx) * 2 + CHAR_ABILITY(u, abiidx)) / 3; /* 2/3rd skill, 1/3rd ability */
    else if (UNIT_TYPE(u) == UNIT_ST_NPC)
        skl = CHAR_ABILITY(u, abiidx);
    else
        skl = 0;

    hm = roll + skl - difficulty - 50;

    /* if (difficulty < 0)
        slog(LOG_ALL, 0, "skillchecksa() < 0  difficulty  = %d", difficulty);*/
    //slog(LOG_ALL, 0, "skillchecksa() 1d100OE = %d + skill = %d - difficulty %d - 50 = %d", roll, skl, difficulty, hm);

    return hm;
}


/* "Defender" must resist against attack from attacker.    */
/* I.e. attacker tries to steal from defender, or backstab */
/* or first aid, etc.                                      */
/* A result >= 0 means "resisted successfully by n.        */
/* A result < 0 means "failed resistance by n".            */
int resistance_skill_check(int att_skill1, int def_skill1,
                           int att_skill2, int def_skill2)
{
    return open100() + att_skill1 + att_skill2 - def_skill1 - def_skill2 - 50;
}

int weight_size(int lbs)
{
    if (lbs <= 5)
        return SIZ_TINY;
    else if (lbs <= 40)
        return SIZ_SMALL;
    else if (lbs <= 160)
        return SIZ_MEDIUM;
    else if (lbs <= 500)
        return SIZ_LARGE;
    else
        return SIZ_HUGE;
}

int weapon_fumble(class unit_data *weapon, int roll)
{
    assert(IS_OBJ(weapon) && (OBJ_TYPE(weapon) == ITEM_WEAPON));

    return roll <= weapon_chart[OBJ_VALUE(weapon, 0)].fumble;
}

int chart_damage(int roll, struct damage_chart_element_type *element)
{
    if (element->alpha == 0)
    {
        slog(LOG_ALL, 0, "Error: Damage chart alpha is 0!");
        return element->basedam;
    }

    if (roll < element->offset)
        return 0;
    else
        return element->basedam + ((roll - element->offset) / element->alpha);
}

/* Size is for natural attacks to limit max damage for such */
int chart_size_damage(int roll, struct damage_chart_element_type *element,
                      int lbs)
{
    if (element->alpha == 0)
    {
        slog(LOG_ALL, 0, "Error: Damage chart alpha is 0!");
        return element->basedam;
    }

    if (roll < element->offset)
        return 0;
    else
    {
        switch (weight_size(lbs))
        {
        case SIZ_TINY:
            roll = MIN(100, roll);
            break;

        case SIZ_SMALL:
            roll = MIN(110, roll);
            break;

        case SIZ_MEDIUM:
            roll = MIN(130, roll);
            break;

        case SIZ_LARGE:
            roll = MIN(150, roll);
            break;
        }

        return element->basedam + ((roll - element->offset) / element->alpha);
    }
}

int weapon_damage(int roll, int weapon_type, int armour_type)
{
    if (!is_in(weapon_type, WPN_GROUP_MAX, WPN_TREE_MAX))
    {
        slog(LOG_ALL, 0, "Illegal weapon type.");
        return 0;
    }

    if (!is_in(armour_type, ARM_CLOTHES, ARM_PLATE))
    {
        slog(LOG_ALL, 0, "Illegal armour type.");
        armour_type = ARM_CLOTHES;
    }

    return chart_damage(roll,
                        &(weapon_chart[weapon_type].element[armour_type]));
}

int natural_damage(int roll, int weapon_type, int armour_type, int lbs)
{
    if (!is_in(weapon_type, WPN_GROUP_MAX, WPN_TREE_MAX))
    {
        slog(LOG_ALL, 0, "Illegal weapon type.");
        return 0;
    }

    if (!is_in(armour_type, ARM_CLOTHES, ARM_PLATE))
    {
        slog(LOG_ALL, 0, "Illegal armour type.");
        armour_type = ARM_CLOTHES;
    }

    return chart_size_damage(roll,
                             &(weapon_chart[weapon_type].element[armour_type]),
                             lbs);
}

/* Return [0..200] for skill when defending with a weapon */
int weapon_defense_skill(class unit_data *ch, int skill)
{
    int max;

    if (IS_PC(ch))
    {
        if (TREE_ISLEAF(wpn_tree, skill))
            max = PC_WPN_SKILL(ch, skill) / 2;
        else
            max = PC_WPN_SKILL(ch, skill);

        while (!TREE_ISROOT(wpn_tree, skill))
        {
            skill = TREE_PARENT(wpn_tree, skill);

            if (PC_WPN_SKILL(ch, skill) > max)
                max = PC_WPN_SKILL(ch, skill);
        }

        return max;
    }
    else /* a NPC */
    {
        if (TREE_ISLEAF(wpn_tree, skill))
            skill = TREE_PARENT(wpn_tree, skill);

        if (TREE_ISROOT(wpn_tree, skill))
            max = NPC_WPN_SKILL(ch, skill);
        else
            max = NPC_WPN_SKILL(ch, skill) / 2;

        while (!TREE_ISROOT(wpn_tree, skill))
        {
            skill = TREE_PARENT(wpn_tree, skill);

            if (NPC_WPN_SKILL(ch, skill) > max)
                max = NPC_WPN_SKILL(ch, skill);
        }

        return max;
    }
}

/* Return [0..200] for skill when attacking with a weapon */
int weapon_attack_skill(class unit_data *ch, int skill)
{
    if (IS_PC(ch))
    {
        int n;

        n = PC_WPN_SKILL(ch, skill);

        if (TREE_ISLEAF(wpn_tree, skill))
            n = MAX(TREE_PARENT(wpn_tree, skill), n);

        if (n == 0)
            n = -25;

        return n;
    }
    else
    {
        if (TREE_ISLEAF(wpn_tree, skill))
            skill = TREE_PARENT(wpn_tree, skill);

        return NPC_WPN_SKILL(ch, skill);
    }
}


int weapon_attack_ability(class unit_data *ch, int skill)
{
    int i;

    i = CHAR_ABILITY(ch, wpn_info[skill].ability[0]) + 
        CHAR_ABILITY(ch, wpn_info[skill].ability[1]) + 
        CHAR_ABILITY(ch, wpn_info[skill].ability[2]);

    i = i / 3;

    return i;
}


/* Return the armour position of where one person hits another */
int hit_location(class unit_data *att, class unit_data *def)
{
    /* Maybe do height reductions later */

    return hit_location_table[dice(2, 8) - 2];
}

/* Return the effective dex of a person in armour ...             */
/* Later we will redo this function - as of now it doesn't matter */
/* what armour you wear                                           */
int effective_dex(class unit_data *ch)
{
    class unit_data *u;
    static const int arm_dex_penalty[] = {0, 10, 20, 40, 100};

    int at, b, p , psum = 0;

    for (u = UNIT_CONTAINS(ch); u; u = u->next)
    {
        if (IS_OBJ(u) && (OBJ_EQP_POS(u) != 0) && (OBJ_TYPE(u) == ITEM_ARMOR))
        {
            if (OBJ_EQP_POS(u) > WEAR_MAX)
                continue; // slog ?

            at = OBJ_VALUE(u, 0);
            if (!is_in(at, ARM_CLOTHES, ARM_PLATE))
                continue; // slog ?

            switch (at)
            {
                case ARM_LEATHER:
                    b = (CHAR_ABILITY(ch, ABIL_STR) + 4*CHAR_ABILITY(ch, ABIL_DEX))/5;
                    if (IS_PC(ch))
                        b = (b + PC_SKI_SKILL(ch, SKI_ARM_LEATHER)) / 2;
                    break;

                case ARM_HLEATHER:
                    b = (CHAR_ABILITY(ch, ABIL_STR) + 2*CHAR_ABILITY(ch, ABIL_DEX))/3;
                    if (IS_PC(ch))
                        b = (b + PC_SKI_SKILL(ch, SKI_ARM_HLEATHER))/2;
                    break;
                case ARM_CHAIN:
                    b = (2*CHAR_ABILITY(ch, ABIL_STR) + CHAR_ABILITY(ch, ABIL_DEX))/3;
                    if (IS_PC(ch))
                        b = (b + PC_SKI_SKILL(ch, SKI_ARM_CHAIN))/2;
                    break;
                case ARM_PLATE:
                    b = CHAR_ABILITY(ch, ABIL_STR);
                    if (IS_PC(ch))
                        b = (b + PC_SKI_SKILL(ch, SKI_ARM_PLATE))/2;
                    break;
                default:
                    continue;
            } // switch

            b = MIN(100, b);
            p = arm_dex_penalty[at] - (b * arm_dex_penalty[at]) / 200;
            psum +=  p * wear_location_prop[OBJ_EQP_POS(u)];
        }
    } // for

    return MAX(0,CHAR_DEX(ch) - psum/64);
}

/* ========================================================================= */


void profession_init(void)
{
    int i;
    int ln;

    ln = sizeof(professions) / sizeof(professions[0]);

    for (i = 0; i < ln; i++)
    {
        professions[i] = NULL;
    }
}


static void profession_read(void)
{
    int idx = -1;
    char pTmp[256];
    char *pCh;
    FILE *fl;

    touch_file(str_cc(g_cServerConfig.m_etcdir, PROFESSION_DEFS));
    if (!(fl = fopen(str_cc(g_cServerConfig.m_etcdir, PROFESSION_DEFS), "rb")))
    {
        slog(LOG_ALL, 0, "unable to read etc/" PROFESSION_DEFS);
        exit(0);
    }

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);
        if (mstmp == NULL)
            continue;

        str_remspc(pTmp);

        if ((pCh = strchr(pTmp, '=')))
        {
            *pCh = 0;
            pCh = skip_blanks(pCh + 1);
            strip_trailing_blanks(pCh);
            str_lower(pTmp);
        }

        if (pCh == NULL || str_is_empty(pCh))
            continue;

        if (strncmp(pTmp, "index", 5) == 0)
        {
            idx = atoi(pCh);
            if (!str_is_number(pCh) || !is_in(idx, 0, PROFESSION_MAX - 1))
            {
                slog(LOG_ALL, 0, "Profession boot error: %s", pCh);
                idx = -1;
            }
            continue;
        }

        if (idx == -1)
            continue;

        if (strncmp(pTmp, "name", 4) == 0)
        {
            if (professions[idx])
               free((char *) professions[idx]);
            professions[idx] = str_dup(pCh);
        }
        else
            slog(LOG_ALL, 0, "Profession boot unknown string: %s", pTmp);
    }

    fclose(fl);
}

void boot_profession(void)
{
    profession_init();
    profession_read();
}

/* ========================================================================= */

static void race_read(void)
{
    int idx = -1;
    char pTmp[256];
    char *pCh;
    FILE *fl;
    char tmp[256];

    touch_file(str_cc(g_cServerConfig.m_etcdir, RACE_DEFS));
    if (!(fl = fopen(str_cc(g_cServerConfig.m_etcdir, RACE_DEFS), "rb")))
    {
        slog(LOG_ALL, 0, "unable to create etc/" RACE_DEFS);
        exit(0);
    }

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);
        if (mstmp == NULL)
            continue;

        str_remspc(pTmp);

        if ((pCh = strchr(pTmp, '=')))
        {
            *pCh = 0;
            pCh = skip_blanks(pCh + 1);
            strip_trailing_blanks(pCh);
            str_lower(pTmp);
        }

        if (pCh == NULL || str_is_empty(pCh))
            continue;

        if (strncmp(pTmp, "index", 5) == 0)
        {
            idx = atoi(pCh);
            if (!str_is_number(pCh) || !is_in(idx, 0, PC_RACE_MAX - 1))
            {
                slog(LOG_ALL, 0, "Race boot error: %s", pCh);
                idx = -1;
            }
            continue;
        }

        if (idx == -1)
            continue;

        if (strncmp(pTmp, "name", 4) == 0)
        {
            if (pc_races[idx])
            {
                free((char *)pc_races[idx]);
                pc_races[idx] = NULL;
            }
            pc_races[idx] = str_dup(pCh);
        }
        else if (strncmp(pTmp, "adverb", 6) == 0)
        {
            if (pc_race_adverbs[idx])
            {
                free((char *)pc_race_adverbs[idx]);
                pc_race_adverbs[idx] = NULL;
            }
            pc_race_adverbs[idx] = str_dup(pCh);
        }
        else if (strncmp(pTmp, "height male", 11) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.height = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.height_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.height_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "height female", 13) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.height = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.height_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.height_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "weight male", 11) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.weight = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.weight_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.weight_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "weight female", 13) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.weight = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.weight_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.weight_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "lifespan male", 13) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.lifespan = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.lifespan_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.lifespan_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "lifespan female", 15) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.lifespan = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.lifespan_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.lifespan_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "age", 3) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].age = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].age_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].age_dice.size = atoi(tmp);
        }
        else
            slog(LOG_ALL, 0, "Race boot unknown string: %s", pTmp);
    }

    fclose(fl);
}

struct diltemplate *playerinit_tmpl;
struct diltemplate *nanny_dil_tmpl;

static void race_init(void)
{
    int i;

    for (i = 0; i < PC_RACE_MAX; i++)
    {
        memset(&race_info[i], 0, sizeof(struct race_info_type));

        pc_races[i] = NULL;
        pc_race_adverbs[i] = NULL;
    }

    pc_races[PC_RACE_MAX] = NULL;
    pc_race_adverbs[PC_RACE_MAX] = NULL;

    playerinit_tmpl = find_dil_template("playerinit@basis");
    if (playerinit_tmpl == NULL)
        slog(LOG_ALL, 0, "No 'playerinit@basis' DIL template.");
    else
    {
        if (playerinit_tmpl->argc != 0)
        {
            slog(LOG_ALL, 0, "playerinit@basis(); not defined correctly.");
            playerinit_tmpl = NULL;
        }
    }

    nanny_dil_tmpl = find_dil_template("nanny@basis");
    if (nanny_dil_tmpl == NULL)
        slog(LOG_ALL, 0, "No 'nanny@basis' DIL template.");
    else
    {
        if ((nanny_dil_tmpl->argc != 1) || (nanny_dil_tmpl->argt[0] != DILV_SP))
        {
            slog(LOG_ALL, 0, "nanny@basis(string); not defined correctly.");
            nanny_dil_tmpl = NULL;
        }
    }
}

void boot_race(void)
{
    race_init();
    race_read();
}

/* ========================================================================= */

static void ability_read(void)
{
    int dummy, idx = -1;
    char pTmp[256];
    char *pCh;
    FILE *fl;

    touch_file(str_cc(g_cServerConfig.m_etcdir, ABILITY_DEFS));
    if (!(fl = fopen(str_cc(g_cServerConfig.m_etcdir, ABILITY_DEFS), "rb")))
    {
        slog(LOG_ALL, 0, "unable to create etc/" ABILITY_DEFS);
        exit(0);
    }

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);

        if (mstmp == NULL)
            continue;

        str_remspc(pTmp);

        if ((pCh = strchr(pTmp, '=')))
        {
            *pCh = 0;
            pCh = skip_blanks(pCh + 1);
            strip_trailing_blanks(pCh);
        }

        str_lower(pTmp);
        strip_trailing_blanks(pTmp);

        if (pCh == NULL || str_is_empty(pCh))
            continue;

        if (strncmp(pTmp, "index", 5) == 0)
        {
            idx = atoi(pCh);
            if (!str_is_number(pCh) || !is_in(idx, 0, ABIL_TREE_MAX - 1))
            {
                slog(LOG_ALL, 0, "Ability boot error: %s", pCh);
                idx = -1;
            }
            continue;
        }

        if (idx == -1)
            continue;

        if (strncmp(pTmp, "name", 4) == 0)
        {
            if (abil_text[idx])
            {
                free((char *)abil_text[idx]);
                abil_text[idx] = NULL;
            }
            abil_text[idx] = str_dup(pCh);
        }
        else if (strncmp(pTmp, "race ", 5) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -3, +3))
            {
                slog(LOG_ALL, 0, "Abilities: race modifier %d for %s not in [-3..+3]", dummy, pTmp);
                continue;
            }

            int ridx = search_block(pTmp + 5, pc_races, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Abilities: Illegal race in: %s", pTmp);
            else
                racial_ability[ridx][idx] = dummy;
        }
        else if (strncmp(pTmp, "profession ", 11) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -9, +5))
            {
                slog(LOG_ALL, 0, "Abilities: profession modifier %d for %s not in [-9..+5]", dummy, pTmp);
                continue;
            }

            int ridx = search_block(pTmp + 11, professions, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Abilities: Illegal profession %s", pTmp);
            else
                ability_prof_table[idx].profession_cost[ridx] = dummy;
        }
        else
            slog(LOG_ALL, 0, "Ability boot unknown string: %s", pTmp);
    }

    fclose(fl);
}

static void ability_init(void)
{
    int i;

    for (i = 0; i < ABIL_TREE_MAX; i++)
    {
        abil_tree[i].parent = i;
        abil_tree[i].isleaf = TRUE;
        abil_text[i] = NULL;

        for (int j = 0; j < PC_RACE_MAX; j++)
            racial_ability[j][i] = 0;

        ability_prof_table[i].sanity = i;
        ability_prof_table[i].min_level = 0;

        for (int j=0; j < ABIL_TREE_MAX; j++)
            ability_prof_table[i].min_abil[j] = 0;

        for (int j=0; j < PROFESSION_MAX; j++)
            ability_prof_table[i].profession_cost[j] = -7;
    }

    abil_tree[ABIL_TREE_MAX].parent = -1;
    abil_tree[ABIL_TREE_MAX].isleaf = FALSE;
    abil_text[ABIL_TREE_MAX] = NULL;
}

bool pairISCompare(const std::pair<int, string>& firstElem, const std::pair<int, string>& secondElem)
{
    return firstElem.first > secondElem.first;
}


void ability_dump(void)
{
    string str;
    char buf[MAX_STRING_LENGTH];

    for (int j = 0; j < PROFESSION_MAX; j++)
    {
        vector< pair <int,string> > vect; 

        for (int i = 0; i < ABIL_TREE_MAX; i++)
        {
            str = "";

            sprintf(buf, "%s,%s", abil_text[i], spc(20-strlen(abil_text[i])));
            str.append(buf);

            sprintf(buf, ".profession %s%s = %s%d\n", professions[j], spc(12-strlen(professions[j])),
                (ability_prof_table[i].profession_cost[j] >= 0) ? "+" : "", ability_prof_table[i].profession_cost[j]);
            str.append(buf);

            vect.push_back(std::make_pair(ability_prof_table[i].profession_cost[j], str));

            if (ability_prof_table[i].min_level > 0 )
            {
                sprintf(buf, "restrict level          = %d\n", ability_prof_table[i].min_level);
                str.append(buf);
            }

            for (int j=0; j < ABIL_TREE_MAX; j++)
                if (ability_prof_table[i].min_abil[j] > 0)
                {
                    sprintf(buf, "restrict %s%s    = %s%d\n", abil_text[j], spc(12-strlen(professions[j])),
                       (ability_prof_table[i].min_abil[j] >= 0) ? "+" : "", ability_prof_table[i].min_abil[j]);
                    str.append(buf);

                }
        }
        std::sort(vect.begin(), vect.end(), pairISCompare);
        for (auto it = vect.begin(); it != vect.end(); ++it)
            printf("%s", it->second.c_str());
    }
    exit(0);
}

void boot_ability(void)
{
    ability_init();
    ability_read();
    //ability_dump(); // Saved in case someone needs to dump it out to excel or something
}

/* ========================================================================= */

static void weapon_read(void)
{
    int dummy, idx = -1;
    char pTmp[256];
    char *pCh;
    FILE *fl;

    touch_file(str_cc(g_cServerConfig.m_etcdir, WEAPON_DEFS));
    if (!(fl = fopen(str_cc(g_cServerConfig.m_etcdir, WEAPON_DEFS), "rb")))
    {
        slog(LOG_ALL, 0, "unable to create etc file");
        exit(0);
    }

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);
        if (mstmp == NULL)
            continue;

        str_remspc(pTmp);

        if (str_is_empty(pTmp))
            continue;
    
        pCh = strchr(pTmp, '=');
        if (pCh == NULL)
        {
            slog(LOG_ALL, 0, "Weapon boot odd line, no equal sign: %s", pTmp);
            continue;
        }

        *pCh = 0;
        pCh = skip_blanks(pCh + 1);
        strip_trailing_blanks(pCh);

        str_lower(pTmp);
        strip_trailing_blanks(pTmp);

        if (str_is_empty(pTmp) || str_is_empty(pCh))
        {
            slog(LOG_ALL, 0, "Weapon boot odd line: %s = %s", pTmp, pCh);
            continue;
        }

        if (strncmp(pTmp, "index", 5) == 0)
        {
            idx = atoi(pCh);
            if (!str_is_number(pCh) || !is_in(idx, WPN_ROOT, WPN_TREE_MAX - 1))
            {
                slog(LOG_ALL, 0, "Weapon boot error: %s", pCh);
                idx = -1;
            }
            continue;
        }

        if (idx == -1)
            continue;

        if (strncmp(pTmp, "name", 4) == 0)
        {
            if (wpn_text[idx])
            {
                free((char *)wpn_text[idx]);
                wpn_text[idx] = NULL;
            }
            wpn_text[idx] = str_dup(pCh);
        }
        else if (strncmp(pTmp, "shield", 6) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, SHIELD_M_BLOCK, SHIELD_M_USELESS))
                wpn_info[idx].shield = dummy;
        }
        else if (strncmp(pTmp, "sphere", 6) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, WPN_ROOT, WPN_GROUP_MAX - 1))
                wpn_tree[idx].parent = dummy;
        }
        else if (strncmp(pTmp, "race ", 5) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -3, +3))
                continue;

            int ridx = search_block(pTmp + 5, pc_races, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Weapons: Illegal race in: %s", pTmp);
            else
                racial_weapons[ridx][idx] = dummy;
        }
        else if (strncmp(pTmp, "fumble", 6) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, 0, 99))
                weapon_chart[idx].fumble = dummy;
        }
        else if (strncmp(pTmp, "hands", 5) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, 1, 2))
                wpn_info[idx].hands = dummy;
        }
        else if (strncmp(pTmp, "speed", 5) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, 0, 12))
                wpn_info[idx].speed = dummy;
        }
        else if (strncmp(pTmp, "type", 4) == 0)
        {
            dummy = atoi(pCh);
            switch (dummy)
            {
            case WPNT_SLASH:
            case WPNT_PIERCE:
            case WPNT_BLUDGEON:
                wpn_info[idx].type = dummy;
            }
        }
        else if (strncmp(pTmp, "profession ", 11) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -9, +5))
            {
                slog(LOG_ALL, 0, "Weapons: profession modifier %d for %s not in [-9..+5]", dummy, pTmp);
                continue;
            }

            int ridx = search_block(pTmp + 11, professions, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Weapons: Illegal profession %s", pTmp);
            else
                weapon_prof_table[idx].profession_cost[ridx] = dummy;
        }
        else if (strncmp(pTmp, "restrict ", 9) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, 0, 250))
            {
                slog(LOG_ALL, 0, "Weapons: restrict modifier %d for %s not in [0..250]", dummy, pTmp);
                continue;
            }

            if (strncmp(pTmp+9, "level", 5) == 0)
            {
                weapon_prof_table[idx].min_level = dummy;
            }
            else
            {
                int ridx = search_block(pTmp + 9, abil_text, TRUE);

                if (ridx == -1)
                    slog(LOG_ALL, 0, "Weapons: Illegal restrict %s", pTmp);
                else
                    weapon_prof_table[idx].min_abil[ridx] = dummy;
            }
        }
        else if (strncmp(pTmp, "ability", 7) == 0)
        {
            char tmp[256];
            int i1, i2, i3;

            pCh = str_next_word(pCh, tmp);
            i1 = atoi(tmp);
            if (!is_in(i1, ABIL_MAG, ABIL_HP) || !str_is_number(tmp))
            {
                slog(LOG_ALL, 0, "Weapon init %d: Illegal ability[0] %d (%s).", idx, i1, tmp);
                continue;
            }

            pCh = str_next_word(pCh, tmp);
            i2 = atoi(tmp);
            if (!is_in(i2, ABIL_MAG, ABIL_HP) || !str_is_number(tmp))
            {
                slog(LOG_ALL, 0, "Weapon init %d: Illegal ability[0] %d (%s).", idx, i2, tmp);
                continue;
            }

            pCh = str_next_word(pCh, tmp);
            i3 = atoi(tmp);
            if (!is_in(i3, ABIL_MAG, ABIL_HP) || !str_is_number(tmp))
            {
                slog(LOG_ALL, 0, "Weapon init %d: Illegal ability[0] %d (%s).", idx, i3, tmp);
                continue;
            }

            wpn_info[idx].ability[0] = i1;
            wpn_info[idx].ability[1] = i2;
            wpn_info[idx].ability[2] = i3;
        }
        else if (strncmp(pTmp, "attack ", 7) == 0)
        {
            char tmp[256];
            int i1, i2, i3;
            int idx2 = -1;

            pCh = str_next_word(pCh, tmp);
            i1 = atoi(tmp);
            pCh = str_next_word(pCh, tmp);
            i2 = atoi(tmp);
            pCh = str_next_word(pCh, tmp);
            i3 = atoi(tmp);

            if (i3 <= 0)
            {
                slog(LOG_ALL, 0,
                     "Weapon init %d: Illegal damage series %d %d %d.", idx,
                     i1, i2, i3);
                continue;
            }

            if (strncmp(pTmp + 7, "clothes", 7) == 0)
                idx2 = ARM_CLOTHES;
            else if (strncmp(pTmp + 7, "sleather", 8) == 0)
                idx2 = ARM_LEATHER;
            else if (strncmp(pTmp + 7, "hleather", 8) == 0)
                idx2 = ARM_HLEATHER;
            else if (strncmp(pTmp + 7, "chain", 5) == 0)
                idx2 = ARM_CHAIN;
            else if (strncmp(pTmp + 7, "plate", 5) == 0)
                idx2 = ARM_PLATE;
            else
            {
                slog(LOG_ALL, 0, "Weapon init %d: Illegal attack type %s.", idx, pTmp);
                continue;
            }

            if (idx2 != -1)
            {
                weapon_chart[idx].element[idx2].offset = i1;
                weapon_chart[idx].element[idx2].basedam = i2;
                weapon_chart[idx].element[idx2].alpha = i3;
            }
        }
        else
            slog(LOG_ALL, 0, "Weapon boot unknown string: [%s]", pTmp);
    }

    fclose(fl);
}

static void weapon_init(void)
{
    int i;
    int j;

    for (i = 0; i < WPN_TREE_MAX; i++)
    {
        weapon_chart[i].fumble = 0;
        for (j = 0; j < 5; j++)
        {
            weapon_chart[i].element[j].offset = 100;
            weapon_chart[i].element[j].basedam = 0;
            weapon_chart[i].element[j].alpha = 100;
        }

        wpn_info[i].hands = 1;
        wpn_info[i].type = WPNT_SLASH;
        wpn_info[i].speed = 0;
        wpn_info[i].shield = SHIELD_M_BLOCK;
        wpn_info[i].ability[0] = ABIL_STR;
        wpn_info[i].ability[1] = ABIL_STR;
        wpn_info[i].ability[2] = ABIL_DEX;

        wpn_tree[i].parent = WPN_ROOT;

        if (i < WPN_GROUP_MAX)
            wpn_tree[i].isleaf = FALSE;
        else
            wpn_tree[i].isleaf = TRUE;

        wpn_text[i] = str_dup(""); // To manage missing weapons

        /* Default to zero */
        for (j = 0; j < PC_RACE_MAX; j++)
            racial_weapons[j][i] = 0;

        /* Clear the weapon_prof table */
        weapon_prof_table[i].sanity = i;
        weapon_prof_table[i].min_level = 0;

        for (int j=0; j < ABIL_TREE_MAX; j++)
            weapon_prof_table[i].min_abil[j] = 0;

        for (int j=0; j < PROFESSION_MAX; j++)
            weapon_prof_table[i].profession_cost[j] = -7;

        if ((i <= LAST_WEAPON) && (weapon_prof_table[i].sanity != i))
        {
            slog(LOG_ALL, 0, "weapon_prof_table[%d] has wrong sanity", i);
            exit(0);
        }
   }

    wpn_tree[WPN_TREE_MAX].parent = -1;
    wpn_tree[WPN_TREE_MAX].isleaf = FALSE;
    wpn_text[WPN_TREE_MAX] = NULL;
}

void weapon_dump(void)
{
    string str;
    char buf[MAX_STRING_LENGTH];

    for (int j = 0; j < PROFESSION_MAX; j++)
    {
        vector< pair <int,string> > vect; 

        for (int i = WPN_GROUP_MAX; i < WPN_TREE_MAX; i++)
        {
            if (str_is_empty(wpn_text[i]))
                continue;

            str = "";

            sprintf(buf, "%s,%s", wpn_text[i], spc(20-strlen(wpn_text[i])));
            str.append(buf);

            sprintf(buf, ".profession %s%s = %s%d\n", professions[j], spc(12-strlen(professions[j])),
                (weapon_prof_table[i].profession_cost[j] >= 0) ? "+" : "", weapon_prof_table[i].profession_cost[j]);
            str.append(buf);

            vect.push_back(std::make_pair(weapon_prof_table[i].profession_cost[j], str));

            /*if (weapon_prof_table[i].min_level > 0)
                printf("restrict level          = %d\n", weapon_prof_table[i].min_level);

            for (int j=0; j < ABIL_TREE_MAX; j++)
                if (weapon_prof_table[i].min_abil[j] > 0)
                    printf("restrict %s%s    = %s%d\n", abil_text[j], spc(12-strlen(abil_text[j])),
                    (weapon_prof_table[i].min_abil[j] >= 0) ? "+" : "", weapon_prof_table[i].min_abil[j]);*/

        }
        std::sort(vect.begin(), vect.end(), pairISCompare);
        for (auto it = vect.begin(); it != vect.end(); ++it) {
            printf("%s", it->second.c_str());
        }        
    }
    exit(0);
}


void boot_weapon(void)
{
    weapon_init();
    weapon_read();
    //weapon_dump();
}


/* ========================================================================= */

static void skill_init(void)
{
    int i;

    for (i = 0; i < SKI_TREE_MAX; i++)
    {
        ski_tree[i].parent = i;
        ski_tree[i].isleaf = TRUE;

        ski_text[i] = NULL;

        /* Racial skills are all zero */
        for (int j = 0; j < PC_RACE_MAX; j++)
            racial_skills[j][i] = 0;

        /* Clear the skill_prof table */
        skill_prof_table[i].sanity = i;
        skill_prof_table[i].min_level = 0;

        for (int j=0; j < ABIL_TREE_MAX; j++)
            skill_prof_table[i].min_abil[j] = 0;

        for (int j=0; j < PROFESSION_MAX; j++)
            skill_prof_table[i].profession_cost[j] = -7;

        if ((i < LAST_SKILL) && (skill_prof_table[i].sanity != i))
        {
            slog(LOG_ALL, 0, "skill_prof_table[%d] has wrong sanity", i);
            exit(0);
        }
    }

    ski_tree[SKI_TREE_MAX].parent = -1;
    ski_tree[SKI_TREE_MAX].isleaf = FALSE;
    ski_text[SKI_TREE_MAX] = NULL;
}

void boot_skill(void)
{
    skill_init();
}
