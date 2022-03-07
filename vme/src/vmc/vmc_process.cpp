/*
 $Author: All $
 $RCSfile: vmc_process.cpp,v $
 $Date: 2003/12/29 11:25:10 $
 $Revision: 2.4 $
 */

#include "common.h"
#include "money.h"
#include "skills.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "utility.h"
#include "vmc.h"

#include <cctype>
#include <cstdarg> /* va_args in dmc_error() */
#include <cstdio>
#include <cstdlib>

/* PS Algorithm 3                                                      */
/* This algorithm returns the total amount of hitpoints possessed when */
/* 'hpp' hit-point-points is current.                                  */
/* Example: hitpoint_total(4) returns total amount of hitpoints when   */
/*          hpp == 4                                                   */
int hitpoint_total(int hpp)
{
    return 10 + 3 * hpp;
}

const char *g_error_zone_name = "";

/* if fatal is 2, it will NEVER be fatal */
void dmc_error(int fatal, const char *fmt, ...)
{
    char buf[MAX_STRING_LENGTH];
    char filename[128];
    va_list args;
    FILE *f = nullptr;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf) - 10, fmt, args);
    buf[sizeof(buf) - 10] = 0;
    va_end(args);

    if (!g_quiet_compile || fatal)
    {
        fprintf(stderr, "%s: %s\n", fatal && fatal != 2 ? "FATAL" : "WARNING", buf);
    }
    sprintf(filename, "%s.err", g_error_zone_name);

    if ((f = fopen(filename, "a")))
    {
        fprintf(f, "%s\n", buf);
        fclose(f);
    }

    if (fatal != 2 && (fatal || g_fatal_warnings))
    {
        g_errcon = 1;
    }
}

/*  MONEY
 */
unsigned long legal_amount(unit_data *u)
{
    unsigned long res = IRON_MULT * CHAR_LEVEL(u);

    if (CHAR_LEVEL(u) < 21)
    {
        res *= 5;
    }
    else if (CHAR_LEVEL(u) < 101)
    {
        res *= 10;
    }
    else if (CHAR_LEVEL(u) < 151)
    {
        res *= 15;
    }
    else if (CHAR_LEVEL(u) < 201)
    {
        res *= 20;
    }
    else
    {
        res *= 25;
    }

    return res;
}

const char *convert_back_money(unsigned long val)
{
    return ""; /* Not programmed yet! */
}

amount_t convert_money(char *str)
{
    long int val1 = 0;
    long int val2 = 0;
    amount_t res = 0;
    char *c = nullptr;

    if (str == nullptr)
    {
        return 0;
    }

    while ((c = strchr(str, '~')))
    {
        sscanf(str, "%ld %ld", &val1, &val2);
        if (val1 > 0)
        {
            if (val2 == -1)
            {
                res += val1;
            }
            else if (is_in(val2, DEF_CURRENCY, MAX_MONEY))
            {
                res += val1 * val2;
            }
            else
            {
                dmc_error(TRUE, "Weird money error");
                return 1;
            }
        }
        str = c + 1;
    }

    return res;
}

/* WEAPONS                               */
/* Input:  Value[0] is weapon category   */
/*         Value[1] is material bonus    */
/*         Value[2] is magic bonus       */
/*         Value[3] is slaying race      */
/*                                       */
/* Output: [3] as input                  */
void dmc_set_weapon(unit_data *weapon)
{
    int category = 0;
    int craftsmanship = 0;
    int magic_bonus = 0;
    int slay = 0;

    category = OBJ_VALUE(weapon, 0);
    craftsmanship = OBJ_VALUE(weapon, 1);
    magic_bonus = OBJ_VALUE(weapon, 2);
    slay = OBJ_VALUE(weapon, 3);

    if (!is_in(category, WPN_GROUP_MAX, WPN_TREE_MAX - 1))
    {
        dmc_error(TRUE, "Illegal weapon category in '%s'.", UNIT_IDENT(weapon));
        OBJ_TYPE(weapon) = ITEM_TRASH;
        return;
    }

    if (!is_in(craftsmanship, BONUS_JUNK, BONUS_ARTIFACT))
    {
        dmc_error(TRUE, "Illegal bonus (-7..+7 allowed) in '%s'.", UNIT_IDENT(weapon));
        OBJ_TYPE(weapon) = ITEM_TRASH;
        return;
    }

    if (!is_in(magic_bonus, BONUS_JUNK, BONUS_ARTIFACT))
    {
        dmc_error(TRUE, "Illegal bonus (-7..7 allowed) in '%s'.", UNIT_IDENT(weapon));
        OBJ_TYPE(weapon) = ITEM_TRASH;
        return;
    }

    // MS2020 we seem to be missing test for slay.
    if (!is_in(slay, 0, 50000))
    {
        dmc_error(TRUE, "Slay race appears uset (1..50000 allowed) in '%s'.", UNIT_IDENT(weapon));
        OBJ_TYPE(weapon) = ITEM_TRASH;
        return;
    }

    set_weapon(weapon);
}

/* ARMOURS                                */
/* INPUT:  Value[0] = Category            */
/*         Value[1] = Bonus               */
/*         Value[2] = ...                 */
void dmc_set_armour(unit_data *armour)
{
    int category = 0;
    int craftsmanship = 0;
    int magic_bonus = 0;

    category = OBJ_VALUE(armour, 0);
    craftsmanship = OBJ_VALUE(armour, 1);
    magic_bonus = OBJ_VALUE(armour, 2);

    if (!is_in(category, ARM_CLOTHES, ARM_PLATE))
    {
        dmc_error(TRUE, "Illegal armour category in '%s'.", UNIT_IDENT(armour));
        OBJ_TYPE(armour) = ITEM_TRASH;
        return;
    }

    if (!is_in(craftsmanship, BONUS_JUNK, BONUS_ARTIFACT))
    {
        dmc_error(TRUE, "Illegal bonus (-7..+7 allowed) in '%s'.", UNIT_IDENT(armour));
        OBJ_TYPE(armour) = ITEM_TRASH;
        return;
    }

    if (!is_in(magic_bonus, BONUS_JUNK, BONUS_ARTIFACT))
    {
        dmc_error(TRUE, "Illegal bonus (-7..+7 allowed) in '%s'.", UNIT_IDENT(armour));
        OBJ_TYPE(armour) = ITEM_TRASH;
        return;
    }

    set_armour(armour);
}

/* SHIELDS                                  */
/* INPUT:  Value[0] = Shield Category       */
/*         Value[1] = Shield Bonus          */
/*         Value[2] = ...                   */
/*                                          */

void dmc_set_shield(unit_data *shield)
{
    int category = 0;
    int craftsmanship = 0;
    int magic_bonus = 0;

    category = OBJ_VALUE(shield, 0);
    craftsmanship = OBJ_VALUE(shield, 1);
    magic_bonus = OBJ_VALUE(shield, 2);

    if (!is_in(category, SHIELD_SMALL, SHIELD_LARGE))
    {
        dmc_error(TRUE, "Illegal shield category in '%s'.", UNIT_IDENT(shield));
        OBJ_TYPE(shield) = ITEM_TRASH;
        return;
    }

    if (!is_in(craftsmanship, BONUS_JUNK, BONUS_ARTIFACT))
    {
        dmc_error(TRUE, "Illegal bonus (-7..+7 allowed) in '%s'.", UNIT_IDENT(shield));
        OBJ_TYPE(shield) = ITEM_TRASH;
        return;
    }

    if (!is_in(magic_bonus, BONUS_JUNK, BONUS_ARTIFACT))
    {
        dmc_error(TRUE, "Illegal bonus (-7..+7 allowed) in '%s'.", UNIT_IDENT(shield));
        OBJ_TYPE(shield) = ITEM_TRASH;
        return;
    }

    set_shield(shield);
}

void set_points(unit_data *u)
{
    int i = 0;
    int sum = 0;
    int max = 0;
    int spoints = 0;
    int apoints = 0;

    if (!is_in(CHAR_ATTACK_TYPE(u), WPN_FIST, WPN_CRUSH))
    {
        dmc_error(TRUE, "Illegal hand attack type %d.", CHAR_ATTACK_TYPE(u));
        CHAR_ATTACK_TYPE(u) = WPN_FIST;
    }

    if (!is_in(CHAR_LEVEL(u), 0, 199))
    {
        dmc_error(TRUE, "Illegal level in  '%s'.", UNIT_IDENT(u));
        CHAR_LEVEL(u) = 0;
    }

    for (i = sum = 0; i < ABIL_TREE_MAX; i++)
    {
        sum += CHAR_ABILITY(u, i);
    }

    if (sum != 100)
    {
        dmc_error(TRUE, "Abilities in '%s' sums up to %d,and not 100.", UNIT_IDENT(u), sum);
    }

    for (i = sum = 0; i < WPN_GROUP_MAX; i++)
    {
        sum += NPC_WPN_SKILL(u, i);
    }

    for (i = 0; i < SPL_GROUP_MAX; i++)
    {
        sum += NPC_SPL_SKILL(u, i);
    }

    if (sum != 100)
    {
        dmc_error(TRUE, "Spells&weapons in '%s' sums up to %d, and not 100.", UNIT_IDENT(u), sum);
    }

    /* It's "*2" because each training session gives the player 2 ability
       points */

    apoints = 2 * ability_point_total(u);
    apoints = (12 * apoints) / 10;

    // apoints = ((100+20-CHAR_LEVEL(u)/10)*apoints) / 100; /* 120% - 100% */

    /* It's "*5" because each training session gives the player 5 skill
       points.
       Remember that monsters do not have nearly as many weapons / spells */
    spoints = (5 * ability_point_total(u)) / 2;
    spoints = (12 * spoints) / 10;

    // spoints = ((100+20-CHAR_LEVEL(u)/10)*spoints) / 100; /* 120% - 100% */

    if ((i = distribute_points(&CHAR_ABILITY(u, 0), ABIL_TREE_MAX, apoints, CHAR_LEVEL(u))))
    {
        dmc_error(FALSE, "%s - An ability is %d points.", UNIT_IDENT(u), i);
    }

    if ((i = distribute_points(&NPC_WPN_SKILL(u, 0), WPN_GROUP_MAX, spoints, CHAR_LEVEL(u))))
    {
        dmc_error(FALSE, "%s - A weapon skill exceeds %d points.", UNIT_IDENT(u), i);
    }

    for (max = i = 0; i < WPN_GROUP_MAX; i++)
    {
        if (NPC_WPN_SKILL(u, i) > max)
        {
            max = NPC_WPN_SKILL(u, i);
        }
    }

    NPC_WPN_SKILL(u, WPN_ROOT) = max / 4;

    if ((i = distribute_points(&NPC_SPL_SKILL(u, 0), SPL_GROUP_MAX, spoints, CHAR_LEVEL(u))))
    {
        dmc_error(FALSE, "%s - A spell skill exceeds %d points.", UNIT_IDENT(u), i);
    }

    for (max = i = 0; i < SPL_GROUP_MAX; i++)
    {
        if (NPC_SPL_SKILL(u, i) > max)
        {
            max = NPC_SPL_SKILL(u, i);
        }
    }

    NPC_SPL_SKILL(u, SPL_ALL) = max / 4;

    if (!is_in(CHAR_NATURAL_ARMOUR(u), ARM_CLOTHES, ARM_PLATE))
    {
        dmc_error(TRUE, "%s, Illegal natural armour type.", UNIT_IDENT(u));
        CHAR_NATURAL_ARMOUR(u) = ARM_PLATE;
    }

    if (!is_in(CHAR_ATTACK_TYPE(u), WPN_GROUP_MAX, WPN_TREE_MAX - 1))
    {
        dmc_error(TRUE, "Illegal attack category in '%s'.", UNIT_IDENT(u));
        CHAR_ATTACK_TYPE(u) = WPN_FIST;
    }

    UNIT_HIT(u) = UNIT_MAX_HIT(u) = hitpoint_total(CHAR_HPP(u));
}

void set_room_data(unit_data *u)
{
    SET_BIT(UNIT_MANIPULATE(u), MANIPULATE_ENTER);
}

bool affect_vector_string(unit_data *obj, std::string &s)
{
    int bonusvector[11];

    s.clear();

    memset(bonusvector, 0, sizeof(bonusvector));

    for (unit_affected_type *af = UNIT_AFFECTED(obj); af; af = af->getNext())
    {
        switch (af->getID())
        {
            case ID_TRANSFER_STR:
                bonusvector[0] += af->getDataAtIndex(1);
                break;
            case ID_TRANSFER_DEX:
                bonusvector[1] += af->getDataAtIndex(1);
                break;
            case ID_TRANSFER_CON:
                bonusvector[2] += af->getDataAtIndex(1);
                break;
            case ID_TRANSFER_CHA:
                bonusvector[3] += af->getDataAtIndex(1);
                break;
            case ID_TRANSFER_BRA:
                bonusvector[4] += af->getDataAtIndex(1);
                break;
            case ID_TRANSFER_MAG:
                bonusvector[5] += af->getDataAtIndex(1);
                break;
            case ID_TRANSFER_DIV:
                bonusvector[6] += af->getDataAtIndex(1);
                break;
            case ID_TRANSFER_HPP:
                bonusvector[7] += af->getDataAtIndex(1);
                break;
            case ID_PROT_GOOD_TRANSFER:
                bonusvector[8] += af->getDataAtIndex(1);
                break;
            case ID_PROT_EVIL_TRANSFER:
                bonusvector[9] += af->getDataAtIndex(1);
                break;
            case ID_TRANSFER_CHARFLAGS:
                bonusvector[10] += af->getDataAtIndex(1);
                break;
        }
    }

    bool bHasData = false;

    for (int i = 0; i < 11; i++)
    {
        s += itoa(bonusvector[i]);

        if (bonusvector[i] != 0)
            bHasData = true;

        if (i < 10)
            s += ",";
    }

    return bHasData;
}

bool affect_vector_list(unit_data *obj, std::string &s)
{
    s.clear();

    for (unit_affected_type *af = UNIT_AFFECTED(obj); af; af = af->getNext())
    {
        switch (af->getID())
        {
            case ID_SPELL_TRANSFER:
                s += "Spl,";
                s += itoa(af->getDataAtIndex(0));
                s += ",";
                s += itoa(af->getDataAtIndex(1));
                s += ",";
                break;
            case ID_SKILL_TRANSFER:
                s += "Ski,";
                s += itoa(af->getDataAtIndex(0));
                s += ",";
                s += itoa(af->getDataAtIndex(1));
                s += ",";
                break;
            case ID_WEAPON_TRANSFER:
                s += "Wpn,";
                s += itoa(af->getDataAtIndex(0));
                s += ",";
                s += itoa(af->getDataAtIndex(1));
                s += ",";
                break;
        }
    }

    return !s.empty();
}

void show_obj_info(unit_data *obj)
{
    static int first = FALSE;
    bool doprint = false;

    if (!first)
    {
        fprintf(stderr,
                "OBJECT\ntype,name,ntype,zone,val0,val1,val2,val3,//,STR,DEX,CON,CHA,BRA,MAG,DIV,HPP,Good,Evil,Flags,//,Spell/Weapon/Skill "
                "Transfers\n\n");
        first = TRUE;
    }

    std::string sAffectVector;
    bool bHasAffect1 = affect_vector_string(obj, sAffectVector);

    std::string sAffectList;
    bool bHasAffect2 = affect_vector_list(obj, sAffectList);

    std::string s;

    switch (OBJ_TYPE(obj))
    {
        case ITEM_WEAPON:
            s += "weapon,";
            s += itoa(OBJ_TYPE(obj));
            s += ",";
            s += UNIT_IDENT(obj);
            s += ",";
            s += g_error_zone_name;
            s += ",";
            s += itoa(OBJ_VALUE(obj, 0)); // Sword type
            s += ",";
            s += itoa(OBJ_VALUE(obj, 1)); // Material bonus
            s += ",";
            s += itoa(OBJ_VALUE(obj, 2)); // Magic bonus
            s += ",";
            s += itoa(OBJ_VALUE(obj, 3)); // Slaying race
            s += ",";
            doprint = true;
            break;

        case ITEM_ARMOR:
            s += "armor,";
            s += itoa(OBJ_TYPE(obj));
            s += ",";
            s += UNIT_IDENT(obj);
            s += ",";
            s += g_error_zone_name;
            s += ",";
            s += itoa(OBJ_VALUE(obj, 0)); // Armor category
            s += ",";
            s += itoa(OBJ_VALUE(obj, 1)); // Material bonus
            s += ",";
            s += itoa(OBJ_VALUE(obj, 2)); // Magic bonus
            s += ",-,";
            doprint = true;
            break;

        case ITEM_SHIELD:
            s += "shield,";
            s += itoa(OBJ_TYPE(obj));
            s += ",";
            s += UNIT_IDENT(obj);
            s += ",";
            s += g_error_zone_name;
            s += ",";
            s += itoa(OBJ_VALUE(obj, 0)); // Armor category
            s += ",";
            s += itoa(OBJ_VALUE(obj, 1)); // Material bonus
            s += ",";
            s += itoa(OBJ_VALUE(obj, 2)); // Magic bonus
            s += ",-,";
            doprint = true;
            break;

        case ITEM_WORN:
        case ITEM_SCROLL:
        case ITEM_WAND:
        case ITEM_STAFF:
        case ITEM_FIREWEAPON:
        case ITEM_MISSILE:
        case ITEM_POTION:
        case ITEM_SPELL:
            s += "other,";
            s += itoa(OBJ_TYPE(obj));
            s += ",";
            s += UNIT_IDENT(obj);
            s += ",";
            s += g_error_zone_name;
            s += ",";
            s += "0"; // itoa(OBJ_VALUE(obj, 0)); // Armor category
            s += ",";
            s += "0"; // itoa(OBJ_VALUE(obj, 1)); // Material bonus
            s += ",";
            s += "0"; // itoa(OBJ_VALUE(obj, 2)); // Magic bonus
            s += ",-,";
            doprint = true;
            break;
    }

    if (doprint || bHasAffect1 || bHasAffect2)
    {
        s += "//," + sAffectVector + ",//,";
        std::string title = UNIT_TITLE(obj);
        std::replace(title.begin(), title.end(), ',', ' ');
        s += title;
        s += ",";
        s += sAffectList;
        fprintf(stderr, "%s\n", s.c_str());
    }
}

void show_npc_info(unit_data *npc)
{
    static int first = FALSE;

    if (!first)
    {
        fprintf(stderr, "                      STR  DEX  CON  HIT  BRA  CHA  MAG  DIV\n\n");
        first = TRUE;
    }

    fprintf(stderr,
            "%-20s %4d %4d %4d %4d %4d %4d %4d %4d\n",
            UNIT_IDENT(npc),
            CHAR_STR(npc),
            CHAR_DEX(npc),
            CHAR_CON(npc),
            UNIT_MAX_HIT(npc),
            CHAR_BRA(npc),
            CHAR_CHA(npc),
            CHAR_MAG(npc),
            CHAR_DIV(npc));
}

void process_affects(unit_data *pUnit)
{
    unit_affected_type *pAf = nullptr;
    int firstf = 0;
    int tickf = 0;
    int lastf = 0;
    int applyf = 0;

    for (pAf = UNIT_AFFECTED(pUnit); pAf; pAf = pAf->getNext())
    {
        firstf = (pAf->getFirstFI() == TIF_NONE);
        tickf = (pAf->getTickFI() == TIF_NONE);
        lastf = (pAf->getLastFI() == TIF_NONE);
        applyf = (pAf->getApplyFI() == APF_NONE);

        switch (pAf->getID())
        {
            case ID_TRANSFER_MAG:
                firstf = (pAf->getFirstFI() == TIF_MAG_INC);
                lastf = (pAf->getLastFI() == TIF_MAG_DEC);
                applyf = (pAf->getApplyFI() == APF_ABILITY);
                if (pAf->getDataAtIndex(0) != ABIL_MAG)
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_TRANSFER_DIV:
                firstf = (pAf->getFirstFI() == TIF_DIV_INC);
                lastf = (pAf->getLastFI() == TIF_DIV_DEC);
                applyf = (pAf->getApplyFI() == APF_ABILITY);
                if (pAf->getDataAtIndex(0) != ABIL_DIV)
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_TRANSFER_STR:
                firstf = (pAf->getFirstFI() == TIF_STR_INC);
                lastf = (pAf->getLastFI() == TIF_STR_DEC);
                applyf = (pAf->getApplyFI() == APF_ABILITY);
                if (pAf->getDataAtIndex(0) != ABIL_STR)
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_TRANSFER_DEX:
                firstf = (pAf->getFirstFI() == TIF_DEX_INC);
                lastf = (pAf->getLastFI() == TIF_DEX_DEC);
                applyf = (pAf->getApplyFI() == APF_ABILITY);
                if (pAf->getDataAtIndex(0) != ABIL_DEX)
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_TRANSFER_CON:
                firstf = (pAf->getFirstFI() == TIF_CON_INC);
                lastf = (pAf->getLastFI() == TIF_CON_DEC);
                applyf = (pAf->getApplyFI() == APF_ABILITY);
                if (pAf->getDataAtIndex(0) != ABIL_CON)
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_TRANSFER_CHA:
                firstf = (pAf->getFirstFI() == TIF_CHA_INC);
                lastf = (pAf->getLastFI() == TIF_CHA_DEC);
                applyf = (pAf->getApplyFI() == APF_ABILITY);
                if (pAf->getDataAtIndex(0) != ABIL_CHA)
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_TRANSFER_BRA:
                firstf = (pAf->getFirstFI() == TIF_BRA_INC);
                lastf = (pAf->getLastFI() == TIF_BRA_DEC);
                applyf = (pAf->getApplyFI() == APF_ABILITY);
                if (pAf->getDataAtIndex(0) != ABIL_BRA)
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_TRANSFER_HPP:
                firstf = (pAf->getFirstFI() == TIF_HIT_INC);
                lastf = (pAf->getLastFI() == TIF_HIT_DEC);
                applyf = (pAf->getApplyFI() == APF_ABILITY);
                if (pAf->getDataAtIndex(0) != ABIL_HP)
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_HIDDEN_DIFFICULTY:
                if (!is_in(pAf->getDataAtIndex(0), 0, 5))
                {
                    dmc_error(TRUE, "%s: Illegal direction %d!", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                    pAf->setDataAtIndex(0, 0);
                }
                if (!is_in(pAf->getDataAtIndex(1), 0, SKILL_MAX))
                {
                    dmc_error(TRUE, "%s: Illegal skill %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                    pAf->setDataAtIndex(0, 0);
                }
                break;

            case ID_CURSE:
                applyf = (pAf->getApplyFI() == APF_MOD_OBJ_FLAGS);
                break;

            case ID_TRANSFER_CHARFLAGS:
                firstf = (pAf->getFirstFI() == TIF_EYES_TINGLE);
                lastf = (pAf->getLastFI() == TIF_EYES_TINGLE);
                applyf = (pAf->getApplyFI() == APF_MOD_CHAR_FLAGS);
                break;

            case ID_SPELL_TRANSFER:
                firstf = (pAf->getFirstFI() == TIF_SPL_INC);
                lastf = (pAf->getLastFI() == TIF_SPL_DEC);
                applyf = (pAf->getApplyFI() == APF_SPELL_ADJ);
                if (!is_in(pAf->getDataAtIndex(0), 0, SPL_TREE_MAX))
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_SKILL_TRANSFER:
                firstf = (pAf->getFirstFI() == TIF_SKI_INC);
                lastf = (pAf->getLastFI() == TIF_SKI_DEC);
                applyf = (pAf->getApplyFI() == APF_SKILL_ADJ);
                if (!is_in(pAf->getDataAtIndex(0), 0, SKI_TREE_MAX))
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_WEAPON_TRANSFER:
                firstf = (pAf->getFirstFI() == TIF_WPN_INC);
                lastf = (pAf->getLastFI() == TIF_WPN_DEC);
                applyf = (pAf->getApplyFI() == APF_WEAPON_ADJ);
                if (!is_in(pAf->getDataAtIndex(0), 0, WPN_TREE_MAX))
                {
                    dmc_error(TRUE, "%s: Illegal data[0] = %d in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(0), pAf->getID());
                }
                if (!is_in(pAf->getDataAtIndex(1), BONUS_JUNK, BONUS_ARTIFACT))
                {
                    dmc_error(TRUE, "%s: Adjustment %d outside -7..+7 in ID %d.", UNIT_IDENT(pUnit), pAf->getDataAtIndex(1), pAf->getID());
                }
                break;

            case ID_PROT_EVIL_TRANSFER:
                firstf = (pAf->getFirstFI() == TIF_PROT_EVIL_ON);
                lastf = (pAf->getLastFI() == TIF_PROT_EVIL_OFF);
                tickf = (pAf->getTickFI() == TIF_NONE);
                applyf = (pAf->getApplyFI() == APF_NONE);
                break;

            case ID_PROT_GOOD_TRANSFER:
                firstf = (pAf->getFirstFI() == TIF_PROT_GOOD_ON);
                lastf = (pAf->getLastFI() == TIF_PROT_GOOD_OFF);
                tickf = (pAf->getTickFI() == TIF_NONE);
                applyf = (pAf->getApplyFI() == APF_NONE);
                break;

            case ID_TRANSFER_SPEED:
                firstf = ((pAf->getFirstFI() == TIF_SPEED_BETTER) || (pAf->getFirstFI() == TIF_SPEED_WORSE));
                lastf = ((pAf->getLastFI() == TIF_SPEED_BETTER) || (pAf->getLastFI() == TIF_SPEED_WORSE));
                tickf = (pAf->getTickFI() == TIF_NONE);
                applyf = (pAf->getApplyFI() == APF_SPEED);
                break;

            default:
                dmc_error(TRUE, "%s: Illegal affect ID (%d).", UNIT_IDENT(pUnit), pAf->getID());
                break;
        }

        if (firstf == FALSE)
        {
            dmc_error(TRUE, "%s: Illegal firstf %d in id %d.", UNIT_IDENT(pUnit), pAf->getFirstFI(), pAf->getID());
        }
        if (tickf == FALSE)
        {
            dmc_error(TRUE, "%s: Illegal tickf %d in id %d.", UNIT_IDENT(pUnit), pAf->getTickFI(), pAf->getID());
        }
        if (lastf == FALSE)
        {
            dmc_error(TRUE, "%s: Illegal lastf %d in id %d.", UNIT_IDENT(pUnit), pAf->getLastFI(), pAf->getID());
        }
        if (applyf == FALSE)
        {
            dmc_error(TRUE, "%s: Illegal applyf %d in id %d.", UNIT_IDENT(pUnit), pAf->getApplyFI(), pAf->getID());
        }
    }
}

void process_funcs(unit_data *u)
{
    unit_fptr *fptr = nullptr;

    for (fptr = UNIT_FUNC(u); fptr; fptr = fptr->getNext())
    {
        if (!is_in(fptr->getFunctionPointerIndex(), 0, SFUN_TOP_IDX))
        {
            dmc_error(TRUE,
                      "%s: Function index not in legal SFUN_XX range.",
                      UNIT_IDENT(u),
                      (float)fptr->getHeartBeat() / (float)PULSE_SEC);
            fptr->setFunctionPointerIndex(0);
        }

        if (fptr->getHeartBeat() != 0)
        {
            if (fptr->getHeartBeat() < WAIT_SEC * 3)
            {
                dmc_error(TRUE,
                          "%s: Heartbeat in function is only "
                          "%.1f seconds! You'll use too much CPU "
                          "(use minimum 3 seconds).",
                          UNIT_IDENT(u),
                          (float)fptr->getHeartBeat() / (float)PULSE_SEC);
                fptr->setHeartBeat(PULSE_SEC * 10);
            }
            else if (fptr->getHeartBeat() > 60000)
            {
                dmc_error(TRUE,
                          "%s: Heartbeat in function is "
                          "%.1f seconds! That's probably an error?",
                          UNIT_IDENT(u),
                          (float)fptr->getHeartBeat() / (float)PULSE_SEC);
                fptr->setHeartBeat(PULSE_SEC * 60 * 60);
            }
        }
    }
}

void check_namelist(unit_data *unit, cNamelist *nl)
{
    char buf[128];

    if (nl)
    {
        ubit32 i = 0;
        int len = 0;

        for (i = 0; i < nl->Length(); i++)
        {
            if (strlen(nl->Name(i)) > 60)
            {
                dmc_error(TRUE, "Name '%s' too long (>60) in unit %s@", nl->Name(i), UNIT_IDENT(unit));
                exit(1);
            }
            strcpy(buf, skip_spaces(nl->Name(i)));
            str_remspc(buf);
            strip_trailing_spaces(buf);
            nl->Substitute(i, buf);
        }

        for (i = 0; i < nl->Length(); i++)
        {
            if ((str_nccmp("self ", nl->Name(i), 5) == 0) || (str_ccmp("self", nl->Name(i)) == 0))
            {
                dmc_error(TRUE,
                          "Reserved word 'self' used in start of "
                          "name in %s@",
                          UNIT_IDENT(unit));
            }

            len = strlen(nl->Name(i));

            if (nl->Name(i)[len - 1] == 's' && nl->Name(i)[len - 2] == '\'')
            {
                dmc_error(TRUE, "%s@: name [%s] not allowed to end in 's", UNIT_IDENT(unit), nl->Name(i));
            }
        }

        cNamelist tmp;

        if (nl->Length() > 1)
        {
            tmp.AppendName(nl->Name(0));

            for (i = 1; i < nl->Length(); i++)
            {
                if (tmp.IsNameRaw(nl->Name(i)) && tmp.Name(0)[0] != '$')
                {
                    // If first char is $ then it's programmatic.
                    //
                    dmc_error(TRUE,
                              "Name order error (or matching names) "
                              "for '%s' in %s@",
                              nl->Name(i),
                              UNIT_IDENT(unit));
                }
                tmp.AppendName(nl->Name(i));
            }
        }
    }
}

void process_unit(unit_data *u)
{
    int i = 0;
    extra_descr_data *exd = nullptr;

    process_affects(u);
    process_funcs(u);

    check_namelist(u, &UNIT_NAMES(u));

    for (exd = UNIT_EXTRA(u).m_pList; exd; exd = exd->next)
    {
        check_namelist(u, &exd->names);
    }

    if (IS_ROOM(u))
    {
        for (i = 0; i <= MAX_EXIT; i++)
        {
            if (ROOM_EXIT(u, i))
            {
                check_namelist(u, &ROOM_EXIT(u, i)->open_name);
            }
        }
    }

    if (!IS_ROOM(u) && !UNIT_TITLE(u).empty())
    {
        if (isupper(*UNIT_TITLE(u).c_str()))
        {
            char buf[MAX_STRING_LENGTH];
            str_next_word_copy(UNIT_TITLE(u).c_str(), buf);
            if (fill_word(buf))
            {
                dmc_error(FALSE, "%s: Title CASE seems to be wrong for '%s'", UNIT_IDENT(u), UNIT_TITLE(u).c_str());
            }
        }

        i = strlen(UNIT_TITLE(u).c_str());

        // Title should end with character or HTML end code >
        if ((i > 0) && !isalpha(UNIT_TITLE(u).c_str()[i - 1]) && (UNIT_TITLE(u).c_str()[i - 1] != '>'))
        {
            dmc_error(FALSE, "%s: Title ends with non-alphabet character '%s'", UNIT_IDENT(u), UNIT_TITLE(u).c_str());
        }
    }

    if (!is_in(UNIT_WEIGHT(u), 0, 2000))
    {
        dmc_error(TRUE, "%s: Illegal weight %d (expected 0..2000).", UNIT_IDENT(u), UNIT_WEIGHT(u));
        UNIT_WEIGHT(u) = 0;
    }
    UNIT_WEIGHT(u) = UNIT_BASE_WEIGHT(u);

    if (!is_in(UNIT_ALIGNMENT(u), -1000, +1000))
    {
        dmc_error(TRUE, "%s: Illegal alignment %d (expected -1000..+1000).", UNIT_IDENT(u), UNIT_ALIGNMENT(u));
        UNIT_ALIGNMENT(u) = 0;
    }

    if (!is_in(UNIT_LIGHTS(u), -6, 6))
    {
        dmc_error(TRUE, "%s: Illegal light %d (expected -6..+6).", UNIT_IDENT(u), UNIT_LIGHTS(u));
        UNIT_LIGHTS(u) = 0;
    }

    if (!is_in(UNIT_BRIGHT(u), -6, 6))
    {
        dmc_error(TRUE, "%s: Illegal bright %d (expected -6..+6).", UNIT_IDENT(u), UNIT_BRIGHT(u));
        UNIT_BRIGHT(u) = 0;
    }

    switch (UNIT_TYPE(u))
    {
        case UNIT_ST_ROOM:
            set_room_data(u);
            break;

        case UNIT_ST_OBJ:
            switch (OBJ_TYPE(u))
            {
                case ITEM_MONEY:
                    OBJ_PRICE(u) = OBJ_VALUE(u, 0);
                    OBJ_VALUE(u, 0) = 0;

                    if (!UNIT_TITLE(u).empty())
                    {
                        int i = 0;

                        for (i = 0; i <= MAX_MONEY; i++)
                        {
                            if (!strcmp(UNIT_TITLE(u).c_str(), g_money_types[i].abbrev))
                            {
                                break;
                            }
                        }

                        if (i > MAX_MONEY)
                        {
                            dmc_error(TRUE, "Not a legal money denominator (%s) on %s", UNIT_TITLE(u).c_str(), UNIT_IDENT(u));
                        }
                        else
                        {
                            OBJ_VALUE(u, 0) = i;
                        }
                    }
                    else
                    {
                        dmc_error(TRUE, "No money denominator on %s", UNIT_IDENT(u));
                    }
                    break;

                case ITEM_WEAPON:
                    dmc_set_weapon(u);
                    break;
                case ITEM_ARMOR:
                    dmc_set_armour(u);
                    break;
                case ITEM_SHIELD:
                    dmc_set_shield(u);
                    break;
            }
            if (g_verbose)
            {
                show_obj_info(u);
            }
            break;

        case UNIT_ST_NPC:
            set_points(u);
            CHAR_MANA(u) = 100;
            CHAR_ENDURANCE(u) = 100;

            if (!is_in(CHAR_SPEED(u), SPEED_MIN, SPEED_MAX))
            {
                dmc_error(TRUE, "%s: Illegal Combat Speed %d - expected [%d..%d].", UNIT_IDENT(u), CHAR_SPEED(u), SPEED_MIN, SPEED_MAX);
            }

            if (CHAR_EXP(u) >= 500)
            {
                dmc_error(TRUE,
                          "%s: Illegal experience bonus %d: substantially above "
                          " the 100 default!",
                          UNIT_IDENT(u),
                          CHAR_EXP(u));
                CHAR_EXP(u) = 100;
            }
            else if (CHAR_EXP(u) < -500)
            {
                dmc_error(TRUE,
                          "%s: Illegal experience penalty %d is less than "
                          "-500 XP.",
                          UNIT_IDENT(u),
                          CHAR_EXP(u));
                CHAR_EXP(u) = -500;
            }

            if (!is_in(CHAR_OFFENSIVE(u), -1000, 1000))
            {
                dmc_error(TRUE,
                          "%s: Illegal offensive bonus %d%%"
                          " (expected -1000 .. +1000).",
                          UNIT_IDENT(u),
                          CHAR_OFFENSIVE(u));
            }

            if (!is_in(CHAR_DEFENSIVE(u), -1000, 1000))
            {
                dmc_error(TRUE,
                          "%s: Illegal defensive bonus %d%%"
                          " (expected -1000 .. +1000).",
                          UNIT_IDENT(u),
                          CHAR_DEFENSIVE(u));
            }

            if (legal_amount(u) < (unsigned long)convert_money(CHAR_MONEY(u)))
            {
                char buf[512];
                sprintf(buf, "%s", money_string(legal_amount(u), local_currency(u), TRUE));

                dmc_error(FALSE,
                          "Too much money on %s.  (%s vs %s)",
                          UNIT_IDENT(u),
                          money_string(convert_money(CHAR_MONEY(u)), local_currency(u), TRUE),
                          buf);

                /* Eventually (?):
                free(CHAR_MONEY(u));
                CHAR_MONEY(u) = str_dup(convert_back_money(legal_amount(u)));
                */
            }

            if (g_verbose)
            {
                show_npc_info(u);
            }
            break;
    }
}

void init_unit(unit_data *u)
{
    int i = 0;

    u->next = nullptr;

    UNIT_KEY(u) = nullptr;
    UNIT_MANIPULATE(u) = 0;
    UNIT_FLAGS(u) = 0;
    UNIT_BASE_WEIGHT(u) = 1;
    UNIT_CAPACITY(u) = 0;
    UNIT_HIT(u) = UNIT_MAX_HIT(u) = 100;
    UNIT_ALIGNMENT(u) = 0;
    UNIT_OPEN_FLAGS(u) = 0;
    UNIT_LIGHTS(u) = 0;
    UNIT_BRIGHT(u) = 0;
    UNIT_CHARS(u) = 0;
    UNIT_AFFECTED(u) = nullptr;
    UNIT_SIZE(u) = 180; /* 180cm default */

    switch (UNIT_TYPE(u))
    {
        case UNIT_ST_NPC:
            UNIT_BASE_WEIGHT(u) = UNIT_WEIGHT(u) = 120; /* lbs default */
            CHAR_MONEY(u) = nullptr;
            CHAR_EXP(u) = 100; /* 100 XP per default at your own level */
            CHAR_FLAGS(u) = 0;
            CHAR_ATTACK_TYPE(u) = WPN_FIST;
            CHAR_NATURAL_ARMOUR(u) = ARM_HLEATHER;
            CHAR_SPEED(u) = 12;
            CHAR_RACE(u) = RACE_HUMAN;
            CHAR_SEX(u) = SEX_NEUTRAL;
            CHAR_LEVEL(u) = 1;
            CHAR_POS(u) = POSITION_STANDING;
            NPC_DEFAULT(u) = POSITION_STANDING;
            for (i = 0; i < ABIL_TREE_MAX; i++)
            {
                CHAR_ABILITY(u, i) = 0;
            }
            CHAR_STR(u) = 40; /* % */
            CHAR_DEX(u) = 30;
            CHAR_CON(u) = 10;
            CHAR_CHA(u) = 2;
            CHAR_BRA(u) = 3;
            CHAR_HPP(u) = 15;

            for (i = 0; i < WPN_GROUP_MAX; i++)
            {
                NPC_WPN_SKILL(u, i) = 0;
            }
            NPC_WPN_SKILL(u, WPN_UNARMED) = 100; /* % */

            for (i = 0; i < SPL_GROUP_MAX; i++)
            {
                NPC_SPL_SKILL(u, i) = 0;
            }
            NPC_FLAGS(u) = 0;
            CHAR_OFFENSIVE(u) = 0;
            CHAR_DEFENSIVE(u) = 0;

            break;

        case UNIT_ST_OBJ:
            for (i = 0; i <= 3; i++)
            {
                OBJ_VALUE(u, i) = 0;
            }
            OBJ_FLAGS(u) = 0;
            OBJ_PRICE(u) = 0;
            OBJ_PRICE_DAY(u) = 0;
            OBJ_TYPE(u) = ITEM_OTHER;
            OBJ_RESISTANCE(u) = 0;
            break;

        case UNIT_ST_ROOM:
            UNIT_CAPACITY(u) = 30000;
            UNIT_BASE_WEIGHT(u) = 10;
            UNIT_WEIGHT(u) = 10;
            UNIT_IN(u) = nullptr;
            for (i = 0; i <= MAX_EXIT; i++)
            {
                ROOM_EXIT(u, i) = nullptr;
            }
            ROOM_FLAGS(u) = 0;
            ROOM_LANDSCAPE(u) = SECT_CITY;
            ROOM_RESISTANCE(u) = 0;
            break;
    }
}
