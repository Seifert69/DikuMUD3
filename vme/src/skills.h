/*
 $Author: All $
 $RCSfile: skills.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.4 $
 */
#pragma once

#include "essential.h"
#include "structs.h"
#include "values.h"

#include <csignal>
#include <string>
#include <utility>

struct skill_interval
{
    int skill;
    const char *descr;
};

class profession_cost;
class tree_type;
class skill_collection
{
public:
    const char **gettext();
    skill_collection(int nSize);

    profession_cost *prof_table;
    const char **text;
    tree_type *tree;
    sbit8 *racial[PC_RACE_MAX];
};

struct profession_cost
{
    ubit16 sanity; /* Used for sanity check */
    ubit8 min_level;
    ubit8 min_abil[ABIL_TREE_MAX];
    sbit8 profession_cost[PROFESSION_MAX]; // 0 is middle, +1 easier to learn, -1 more dificult, etc.
};

/* ---------------- COMBAT MESSAGE SYSTEM -------------------- */

#define COM_MAX_MSGS 60

#define COM_MSG_DEAD -1
#define COM_MSG_MISS -2
#define COM_MSG_NODAM -3
#define COM_MSG_EBODY -4

/* ---------------- RACES -------------------- */

#define RACE_IS_HUMANOID(race) ((race) <= RACE_OTHER_HUMANOID)

#define CHAR_IS_MAMMAL(ch) RACE_IS_MAMMAL(CHAR_RACE(ch))

#define RACE_IS_MAMMAL(race) (((race) > RACE_OTHER_HUMANOID) && ((race) <= RACE_OTHER_MAMMAL))

#define RACE_IS_UNDEAD(race) (((race) > RACE_OTHER_CREATURE) && ((race) <= RACE_OTHER_UNDEAD))

#define CHAR_IS_HUMANOID(ch) RACE_IS_HUMANOID(CHAR_RACE(ch))

#define CHAR_IS_UNDEAD(ch) RACE_IS_UNDEAD(CHAR_RACE(ch))

struct dice_type
{
    ubit16 reps;
    ubit16 size;
};

struct base_race_info_type
{
    ubit16 height;
    dice_type height_dice;

    ubit16 weight;
    dice_type weight_dice;

    ubit16 lifespan;
    dice_type lifespan_dice;
};

struct race_info_type
{
    base_race_info_type male;
    base_race_info_type female;

    ubit16 age;
    dice_type age_dice;
};

struct damage_chart_element_type
{
    int offset;  /* When does damage start         */
    int basedam; /* The damage given at 'offset'   */
    int alpha;   /* Step size of damage as 1/alpha */
};

struct damage_chart_type
{
    int fumble; /* from 01 - fuble => fumble      */

    damage_chart_element_type element[5];
};

#define TREE_PARENT(tree, node) (tree[node].parent)
#define TREE_GRANDPARENT(tree, node) (tree[tree[node].parent].parent)
#define TREE_ISROOT(tree, node) ((node) == (tree[node].parent))
#define TREE_ISLEAF(tree, node) (tree[node].isleaf)

struct tree_type
{
    int parent;
    ubit8 isleaf;
    ubit1 bAutoTrain;
    ubit1 bAutoTeacherNoAdd;
};

/* Tree has a pointer to parent for each node. 0 pointer from root */
struct wpn_info_type
{
    int hands;        /* 0=N/A, 1 = 1, 2 = 1.5, 3 = 2          */
    int speed;        /* Speed modification by weapon 0..      */
    int type;         /* Is the weapon slashing/piercing...    */
    ubit8 ability[3]; // Primary abilities, e.g. [0]=ABIL_STR, [1]=ABIL_STR, [2]=ABIL_DEX
    ubit8 shield;     /* Shield method SHIELD_M_XXX            */
};

#define DEMIGOD_LEVEL_XP (40000000)

void roll_description(unit_data *att, const char *text, int roll);
int open_ended_roll(int size, int end);
inline int open100()
{
    // MS2020 reducing open roll chance to 2% on high and 1% low.
    return open_ended_roll(100, 2);
}

int skillchecksa(unit_data *u, int skillidx, int abiidx, int difficulty);
int resistance_skill_check(int att_skill1, int def_skill1, int att_skill2, int def_skill2);

int weapon_fumble(unit_data *weapon, int roll);
int chart_damage(int roll, damage_chart_element_type *element);
int chart_size_damage(int roll, damage_chart_element_type *element, int lbs);
int weapon_damage(int roll, int weapon_type, int armour_type);
int natural_damage(int roll, int weapon_type, int armour_type, int lbs);

int weapon_defense_skill(unit_data *ch, int skill);
int weapon_attack_skill(unit_data *ch, int skill);
int weapon_attack_ability(unit_data *ch, int skill);
int hit_location(unit_data *att, unit_data *def);
int effective_dex(unit_data *ch);

bool pairISCompare(const std::pair<int, std::string> &firstElem, const std::pair<int, std::string> &secondElem);
void boot_ability();
void boot_profession();
void boot_race();
void boot_skill();
void boot_weapon();

int get_racial_ability(int nRace, int nAbility);
int get_racial_weapon(int nRace, int nWeapon);
int get_racial_skill(int nRace, int nSkill);
int get_racial_spells(int nRace, int nSpell);

extern diltemplate *g_nanny_dil_tmpl;
extern diltemplate *g_playerinit_tmpl;
extern wpn_info_type g_wpn_info[];
extern const char *g_professions[PROFESSION_MAX + 1];
extern int g_hit_location_table[];
extern race_info_type g_race_info[PC_RACE_MAX];
extern const char *g_pc_races[PC_RACE_MAX + 1];
extern const char *g_pc_race_adverbs[PC_RACE_MAX + 1];

extern damage_chart_type g_weapon_chart[WPN_TREE_MAX];
extern damage_chart_type g_spell_chart[SPL_TREE_MAX];
