/*
 $Author: All $
 $RCSfile: utils.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.7 $
 */
#pragma once

#include "constants.h"
#include "descriptor_data.h"
#include "essential.h"
#include "file_index_type.h"
#include "obj_data.h"
#include "room_data.h"
#include "structs.h"
#include "weather.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

/* ..................................................................... */

#define PK_RELAXED 0
#define PK_STRICT 1

#define IS_BEGINNER(ch) (CHAR_LEVEL(ch) <= START_LEVEL)
#define IS_MORTAL(ch) (CHAR_LEVEL(ch) < IMMORTAL_LEVEL)
#define IS_IMMORTAL(ch) (CHAR_LEVEL(ch) >= IMMORTAL_LEVEL)
#define IS_GOD(ch) (CHAR_LEVEL(ch) >= GOD_LEVEL)
#define IS_CREATOR(ch) (CHAR_LEVEL(ch) >= CREATOR_LEVEL)
#define IS_OVERSEER(ch) (CHAR_LEVEL(ch) >= OVERSEER_LEVEL)
#define IS_ADMINISTRATOR(ch) (CHAR_LEVEL(ch) >= ADMINISTRATOR_LEVEL)
#define IS_ULTIMATE(ch) (CHAR_LEVEL(ch) == ULTIMATE_LEVEL)

/* ..................................................................... */

/*  Do NOT use these macros unless you know PRECISELY what you are doing!!!
 *  and only if you have to assign directly to them (as in db_utils.c)
 */
#define U_CHAR(u) ((char_data *)(dynamic_cast<const char_data *>(u)))
#define U_NPC(u) ((npc_data *)(dynamic_cast<const npc_data *>(u)))
#define U_PC(u) ((pc_data *)(dynamic_cast<const pc_data *>(u)))
#define U_OBJ(u) ((obj_data *)(dynamic_cast<const obj_data *>(u)))
#define U_ROOM(u) ((room_data *)(dynamic_cast<const room_data *>(u)))

#ifdef MUD_DEBUG
    #define DEBUG_HISTORY
    #define UNIT_TYPE_DEBUG
#endif

#ifdef UNIT_TYPE_DEBUG
    #define UCHAR(u) (assert(u &&IS_CHAR(u)), U_CHAR(u))
    #define UNPC(u) (assert(u &&IS_NPC(u)), U_NPC(u))
    #define UPC(u) (assert(u &&IS_PC(u)), U_PC(u))
    #define UOBJ(u) (assert(u &&IS_OBJ(u)), U_OBJ(u))
    #define UROOM(u) (assert(u &&IS_ROOM(u)), U_ROOM(u))
#else
    #define UCHAR(u) U_CHAR(u)
    #define UNPC(u) U_NPC(u)
    #define UPC(u) U_PC(u)
    #define UOBJ(u) U_OBJ(u)
    #define UROOM(u) U_ROOM(u)
#endif

/* ..................................................................... */

// clang-format off
inline unit_fptr *UNIT_FUNC(unit_data *unit) { return unit->getFunctionPointer(); }

inline file_index_type *UNIT_FILE_INDEX(unit_data *unit) { return unit->getFileIndex(); }
inline const file_index_type *UNIT_FILE_INDEX(const unit_data *unit) { return unit->getFileIndex(); }

inline ubit32 UNIT_MANIPULATE(unit_data *unit) { return unit->getManipulate(); }

inline ubit16 UNIT_FLAGS(unit_data *unit) { return unit->getUnitFlags(); }
inline ubit16 UNIT_FLAGS(const unit_data *unit) { return unit->getUnitFlags(); }

inline sbit32 UNIT_WEIGHT(unit_data *unit) { return unit->getWeight(); }

inline sbit32 UNIT_BASE_WEIGHT(unit_data *unit) {return unit->getBaseWeight(); }

inline ubit16 UNIT_SIZE(unit_data *unit) { return unit->getSize(); }

inline sbit16 UNIT_CAPACITY(unit_data *unit) { return unit->getCapacity(); }

inline sbit16 UNIT_BRIGHT(unit_data *unit) { return unit->getLightOutput(); }

inline sbit16 UNIT_LIGHTS(unit_data *unit) { return unit->getNumberOfActiveLightSources(); }

inline sbit16 UNIT_ILLUM(unit_data *unit) { return unit->getTransparentLightOutput(); }

inline sbit8 UNIT_CHARS(const unit_data *unit) { return unit->getNumberOfCharactersInsideUnit(); }

inline unit_data *UNIT_CONTAINS(unit_data *unit) { return unit->getContainedUnits(); }
inline const unit_data *UNIT_CONTAINS(const unit_data *unit) { return unit->getContainedUnits(); }

inline unit_data *UNIT_IN(unit_data *unit) { return unit->getMyContainer(); }
inline unit_data *UNIT_IN(const unit_data *unit) { return const_cast<unit_data*>(unit)->getMyContainer(); }

inline unit_affected_type *UNIT_AFFECTED(unit_data *unit) { return unit->getUnitAffectedType(); }

inline const char *UNIT_NAME(const unit_data *unit) { return unit->getNames().Name(); }

inline const char *UNIT_KEY(unit_data *unit) { return unit->getKey(); }

inline ubit8 UNIT_OPEN_FLAGS(const unit_data *unit) { return unit->getOpenFlags(); }

inline ubit8 UNIT_OPEN_DIFF(unit_data *unit) { return unit->getOpenDifficulty(); }

inline ubit8 UNIT_TYPE(unit_data *unit) { return unit->getUnitType(); }
inline ubit8 UNIT_TYPE(const unit_data *unit) { return unit->getUnitType(); }

inline cNamelist &UNIT_NAMES(unit_data *unit) { return unit->getNames(); }

inline sbit16 UNIT_ALIGNMENT(unit_data *unit) { return unit->getAlignment(); }

inline sbit32 UNIT_HIT(unit_data *unit) { return unit->getCurrentHitpoints(); }

inline sbit32 UNIT_MAX_HIT(unit_data *unit) { return unit->getMaximumHitpoints(); }

inline ubit8 UNIT_MINV(unit_data *unit) { return unit->getLevelOfWizardInvisibility(); }
inline ubit8 UNIT_MINV(const unit_data *unit) { return unit->getLevelOfWizardInvisibility(); }

inline const std::string &UNIT_TITLE(const unit_data *unit) { return unit->getTitle(); }

inline const std::string &UNIT_OUT_DESCR(const unit_data *unit) { return unit->getDescriptionOfOutside(); }

inline const std::string &UNIT_IN_DESCR(const unit_data *unit) { return unit->getDescriptionOfInside(); }

inline extra_list &UNIT_EXTRA(unit_data *unit) { return unit->getExtraList(); }
inline const extra_list &UNIT_EXTRA(const unit_data *unit) { return unit->getExtraList(); }
// clang-format on
/* ..................................................................... */

#define IS_ROOM(unit) (UNIT_TYPE(unit) == UNIT_ST_ROOM)

#define IS_OBJ(unit) (UNIT_TYPE(unit) == UNIT_ST_OBJ)

#define IS_NPC(unit) (UNIT_TYPE(unit) == UNIT_ST_NPC)

#define IS_PC(unit) (UNIT_TYPE(unit) == UNIT_ST_PC)

#define IS_CHAR(unit) (IS_SET(UNIT_TYPE(unit), UNIT_ST_PC | UNIT_ST_NPC))

/* ............................FILE INDEX STUFF..................... */

#define FI_ZONENAME(fi) ((fi)->getZone()->getName())

#define FI_NAME(fi) ((fi)->getName())

/* ............................UNIT SUPERSTRUCTURES..................... */

#define UNIT_IS_TRANSPARENT(u)                                                                                                             \
    (!IS_SET(UNIT_FLAGS(u), UNIT_FL_BURIED) && IS_SET(UNIT_FLAGS(u), UNIT_FL_TRANS) && !IS_SET(UNIT_OPEN_FLAGS(u), EX_CLOSED))

#define UNIT_FI_ZONE(unit) ((unit)->getFileIndex()->getZone())

#define UNIT_FI_ZONENAME(unit) (UNIT_FILE_INDEX(unit) ? FI_ZONENAME(UNIT_FILE_INDEX(unit)) : "NO-ZONE")

#define UNIT_FI_NAME(unit) (UNIT_FILE_INDEX(unit) ? FI_NAME(UNIT_FILE_INDEX(unit)) : "NO-NAME")

#define UNIT_WEAR(unit, part) (IS_SET(UNIT_MANIPULATE(unit), part))

#define UNIT_IS_OUTSIDE(unit) (!IS_SET(UNIT_FLAGS(UNIT_IN(unit)), UNIT_FL_INDOORS))

#define UNIT_OUTSIDE_LIGHT(unit) (!IS_SET(UNIT_FLAGS(unit), UNIT_FL_INDOORS) ? g_time_light[g_sunlight] : 0)

#define UNIT_IS_DARK(unit) (UNIT_LIGHTS(unit) + UNIT_OUTSIDE_LIGHT(unit) + (UNIT_IN(unit) ? UNIT_LIGHTS(UNIT_IN(unit)) : 0) < 0)

#define UNIT_IS_LIGHT(unit) (UNIT_LIGHTS(unit) + UNIT_OUTSIDE_LIGHT(unit) + (UNIT_IN(unit) ? UNIT_LIGHTS(UNIT_IN(unit)) : 0) >= 0)

#define UNIT_SEX(unit) (IS_CHAR(unit) ? CHAR_SEX(unit) : SEX_NEUTRAL)

#define UNIT_IS_GOOD(ch) (UNIT_ALIGNMENT(ch) >= 350)
#define UNIT_IS_EVIL(ch) (UNIT_ALIGNMENT(ch) <= -350)
#define UNIT_IS_NEUTRAL(ch) (!UNIT_IS_GOOD(ch) && !UNIT_IS_EVIL(ch))

#define UNIT_CONTAINING_W(u) (UNIT_WEIGHT(u) - UNIT_BASE_WEIGHT(u))

/* ..................................................................... */
// clang-format off

inline ubit8 ROOM_RESISTANCE(unit_data *room) { return UROOM(room)->getRoomMagicalResistance(); }

inline ubit8 ROOM_LANDSCAPE(const unit_data *unit) { return UROOM(unit)->getLandscapeTerrain(); }

inline ubit8 ROOM_FLAGS(const unit_data *unit) { return UROOM(unit)->getRoomFlags(); }

inline room_direction_data *ROOM_EXIT(unit_data *unit, size_t exit) { return UROOM(unit)->getRoomDirectionDataForExit(exit); }

inline int ROOM_NUM(const unit_data *unit) { return UROOM(unit)->getRoomNumber(); }

inline int ROOM_SC(const unit_data *unit) { return UROOM(unit)->getStrongComponent(); }

#ifndef MPLEX_COMPILE
    inline std::vector<room_data::vertex_descriptor> &ROOM_PATH(unit_data *unit) { return UROOM(unit)->getPath(); }
    inline std::vector<room_data::vertex_descriptor> &ROOM_DISTANCE(unit_data *unit) { return UROOM(unit)->getDistance();}
#endif

// clang-format on
/* ..................................................................... */
// clang-format off

inline ubit8 OBJ_RESISTANCE(unit_data *obj) { return UOBJ(obj)->getMagicResistance(); }

inline sbit32 OBJ_VALUE(unit_data *unit, size_t index) { return UOBJ(unit)->getValueAtIndex(index); }

inline ubit32 OBJ_PRICE(unit_data *unit) { return UOBJ(unit)->getPriceInGP(); }

inline ubit32 OBJ_PRICE_DAY(unit_data *unit) { return UOBJ(unit)->getPricePerDay(); }

inline ubit8 OBJ_TYPE(unit_data *unit) { return UOBJ(unit)->getObjectItemType(); }

inline ubit8 OBJ_EQP_POS(unit_data *unit) { return UOBJ(unit)->getEquipmentPosition(); }

inline ubit8 OBJ_FLAGS(unit_data *obj) { return UOBJ(obj)->getObjectFlags(); }

// clang-format on
/* ...........................OBJECT SUPERSTRUCTURES..................... */

#define OBJ_HAS_EXTRA(obj, stat) (IS_SET(OBJ_EXTRA(obj), stat))

#define UNIT_IS_EQUIPPED(obj) (IS_OBJ(obj) && OBJ_EQP_POS(obj))

/* ..................................................................... */

/*#define CHAR_DEX_RED(ch)     \  (UCHAR(ch)->points.dex_reduction)*/
// clang-format off
inline const descriptor_data *CHAR_DESCRIPTOR(const unit_data *ch) { return UCHAR(ch)->getDescriptor(); }

inline descriptor_data *CHAR_DESCRIPTOR(unit_data *ch) { return UCHAR(ch)->getDescriptor(); }

inline sbit16 CHAR_OFFENSIVE(const unit_data *unit) { return UCHAR(unit)->getOffensiveBonus(); }

inline sbit16 CHAR_DEFENSIVE(const unit_data *unit) { return UCHAR(unit)->getDefensiveBonus(); }

inline ubit32 CHAR_FLAGS(const unit_data *unit) { return UCHAR(unit)->getCharacterFlags(); }

inline ubit8 CHAR_SEX(const unit_data *ch) { return UCHAR(ch)->getSex(); }

inline unit_data *CHAR_LAST_ROOM(unit_data *unit) { return UCHAR(unit)->getLastLocation(); }

inline const char *CHAR_LAST_ATTACKER(const unit_data *unit) { return UCHAR(unit)->getLastAttacker(); }

inline ubit8 CHAR_LAST_ATTACKER_TYPE(const unit_data *unit) { return UCHAR(unit)->getLastAttackerType(); }

inline ubit8 CHAR_POS(const unit_data *ch) { return UCHAR(ch)->getPosition(); }

inline ubit8 CHAR_LEVEL(const unit_data *ch) { return UCHAR(ch)->getLevel(); }

inline ubit16 CHAR_RACE(const unit_data *ch) { return UCHAR(ch)->getRace(); }

inline sbit16 CHAR_ABILITY(const unit_data *ch, size_t index) { return UCHAR(ch)->getAbilityAtIndex(index); }

inline sbit16 CHAR_STR(const unit_data *ch) { return UCHAR(ch)->getSTR(); }

inline sbit16 CHAR_DEX(const unit_data *ch) { return UCHAR(ch)->getDEX(); }

inline sbit16 CHAR_CON(const unit_data *ch) { return UCHAR(ch)->getCON(); }

inline sbit16 CHAR_CHA(const unit_data *ch) { return UCHAR(ch)->getCHA(); }

inline sbit16 CHAR_BRA(const unit_data *ch) { return UCHAR(ch)->getBRA(); }

inline sbit16 CHAR_MAG(const unit_data *ch) { return UCHAR(ch)->getMAG(); }

inline sbit16 CHAR_DIV(const unit_data *ch) { return UCHAR(ch)->getDIV(); }

inline sbit16 CHAR_HPP(const unit_data *ch) { return UCHAR(ch)->getHPP(); }

inline sbit16 CHAR_ENDURANCE(const unit_data *ch) { return UCHAR(ch)->getEndurance(); }

inline sbit16 CHAR_MANA(const unit_data *ch) { return UCHAR(ch)->getMana(); }

/* NOT to be used unless by db.... */
inline const char *CHAR_MONEY(const unit_data *ch) { return UCHAR(ch)->getMoney(); }

inline char *CHAR_MONEY(unit_data *ch) { return UCHAR(ch)->getMoney(); }

inline sbit32 CHAR_EXP(const unit_data *ch) { return UCHAR(ch)->getPlayerExperience(); }

inline ubit8 CHAR_ATTACK_TYPE(const unit_data *ch) { return UCHAR(ch)->getAttackType(); }

inline ubit8 CHAR_NATURAL_ARMOUR(const unit_data *ch) { return UCHAR(ch)->getNaturalArmor(); }

inline sbit16 CHAR_SPEED(const unit_data *ch) { return UCHAR(ch)->getSpeed(); }

inline cCombat *CHAR_COMBAT(unit_data *ch) { return UCHAR(ch)->getCombat(); }

inline unit_data *CHAR_FIGHTING(unit_data *ch) { return UCHAR(ch)->getCombat() ? CHAR_COMBAT(ch)->Melee() : nullptr; }

inline unit_data *CHAR_MASTER(unit_data *ch) { return UCHAR(ch)->getMaster(); }

inline char_follow_type *CHAR_FOLLOWERS(const unit_data *ch) { return UCHAR(ch)->getFollowers(); }
// clang-format on
/* ...........................CHAR SUPERSTRUCTURES....................... */

#define CHAR_IS_READY(ch) (CHAR_AWAKE(ch) && (CHAR_POS(ch) != POSITION_FIGHTING))

#define CHAR_IS_SNOOPING(ch) (CHAR_DESCRIPTOR(ch) ? (CHAR_DESCRIPTOR(ch)->getSnoopData().getSnooping()) : (unit_data *)nullptr)

#define CHAR_IS_SNOOPED(ch) (CHAR_DESCRIPTOR(ch) ? (CHAR_DESCRIPTOR(ch)->getSnoopData().getSnoopBy()) : (unit_data *)nullptr)

#define CHAR_IS_SWITCHED(ch) (CHAR_DESCRIPTOR(ch) ? (CHAR_DESCRIPTOR(ch)->cgetOriginalCharacter()) : (unit_data *)nullptr)

#define CHAR_SNOOPING(ch) (CHAR_IS_SNOOPING(ch) ? (CHAR_DESCRIPTOR(ch)->getSnoopData().getSnooping()) : (ch))

#define CHAR_SNOOPED(ch) (CHAR_IS_SNOOPED(ch) ? (CHAR_DESCRIPTOR(ch)->getSnoopData().getSnoopBy()) : (ch))

inline unit_data *CHAR_ORIGINAL(unit_data *ch)
{
    return CHAR_IS_SWITCHED(ch) ? CHAR_DESCRIPTOR(ch)->getOriginalCharacter() : ch;
}

#define CHAR_AWAKE(ch) (CHAR_POS(ch) > POSITION_SLEEPING)

#define CHAR_HAS_FLAG(ch, flags) (IS_SET(CHAR_FLAGS(ch), (flags)))

#define CHAR_ROOM_EXIT(ch, door) (IS_ROOM(UNIT_IN(ch)) ? ROOM_EXIT(UNIT_IN(ch), (door)) : (unit_data *)nullptr)

#define CHAR_VISION(ch) (!CHAR_HAS_FLAG(ch, CHAR_BLIND))

// Made the decision that you can always see what you are inside, so you can e.g. knock a coffin you've been buried in
#define CHAR_CAN_SEE(ch, unit)                                                                                                             \
    (!IS_CHAR(ch) || (CHAR_VISION(ch) && (!IS_SET(UNIT_FLAGS(unit), UNIT_FL_BURIED) || (UNIT_IN(ch) == unit)) && (CHAR_LEVEL(ch) >= UNIT_MINV(unit)) &&               \
                      (CHAR_LEVEL(ch) >= CREATOR_LEVEL || (UNIT_IS_LIGHT(UNIT_IN(ch)) && (!IS_SET(UNIT_FLAGS(unit), UNIT_FL_INVISIBLE) ||  \
                                                                                          CHAR_HAS_FLAG(ch, CHAR_DETECT_INVISIBLE))))))

#define CHAR_CAN_GO(ch, door)                                                                                                              \
    (ROOM_EXIT(UNIT_IN(ch), door) && (ROOM_EXIT(UNIT_IN(ch), door)->to_room) && !IS_SET(ROOM_EXIT(UNIT_IN(ch), door)->exit_info, EX_CLOSED))

/* ..................................................................... */

#define PC_ACCESS_LEVEL(pc) (UPC(pc)->nAccessLevel)

inline ubit16 PC_CRACK_ATTEMPTS(unit_data *pc)
{
    return UPC(pc)->getNumberOfCrackAttempts();
}

inline terminal_setup_type &PC_SETUP(unit_data *pc)
{
    return UPC(pc)->getTerminalSetupType();
}

inline ubit16 PC_LIFESPAN(unit_data *pc)
{
    return UPC(pc)->getLifespan();
}

inline pc_account_data &PC_ACCOUNT(unit_data *pc)
{
    return UPC(pc)->getPCAccountData();
}

inline pc_account_data &PC_ACCOUNT(const unit_data *pc)
{
    return UPC(pc)->getPCAccountData();
}

inline ubit16 PC_VIRTUAL_LEVEL(unit_data *pc)
{
    return UPC(pc)->getVirtualPlayerLevel();
}

inline extra_list &PC_INFO(unit_data *pc)
{
    return UPC(pc)->getAdministrationInformation();
}

inline ubit8 PC_SETUP_ECHO(unit_data *pc)
{
    return UPC(pc)->getTerminalSetupType().echo;
}

inline ubit8 PC_SETUP_REDRAW(unit_data *pc)
{
    return UPC(pc)->getTerminalSetupType().redraw;
}

inline ubit8 PC_SETUP_WIDTH(unit_data *pc)
{
    return UPC(pc)->getTerminalSetupType().width;
}

inline ubit8 PC_SETUP_HEIGHT(unit_data *pc)
{
    return UPC(pc)->getTerminalSetupType().height;
}

inline ubit8 PC_SETUP_TELNET(unit_data *pc)
{
    return UPC(pc)->getTerminalSetupType().telnet;
}

inline ubit8 PC_SETUP_EMULATION(unit_data *pc)
{
    return UPC(pc)->getTerminalSetupType().emulation;
}

inline ubit8 PC_SETUP_COLOUR(unit_data *pc)
{
    return UPC(pc)->getTerminalSetupType().colour_convert;
}

inline const char *PC_GUILD(unit_data *pc)
{
    return UPC(pc)->getGuild();
}

inline const char *PC_PROMPTSTR(unit_data *pc)
{
    return UPC(pc)->getPromptString();
}

inline sbit8 PC_PROFESSION(unit_data *pc)
{
    return UPC(pc)->getProfession();
}

inline extra_list &PC_QUEST(unit_data *pc)
{
    return UPC(pc)->getQuestInformation();
}

#define PC_COND(ch, i) (UPC(ch)->conditions[i])

inline sbit32 PC_ABILITY_POINTS(unit_data *ch)
{
    return UPC(ch)->getAbilityPoints();
}

inline sbit32 PC_SKILL_POINTS(unit_data *ch)
{
    return UPC(ch)->getSkillPoints();
}

inline ubit8 PC_ABI_LVL(unit_data *ch, size_t index)
{
    return UPC(ch)->getAbilityLevelAtIndex(index);
}

inline sbit16 PC_SKI_SKILL(unit_data *ch, size_t index)
{
    return UPC(ch)->getSkillAtIndex(index);
}

inline ubit8 PC_SKI_LVL(unit_data *ch, size_t index)
{
    return UPC(ch)->getSpellSkillAtIndex(index);
}

inline ubit8 PC_WPN_LVL(unit_data *ch, size_t index)
{
    return UPC(ch)->getWeaponSkillLevelAtIndex(index);
}

inline sbit16 PC_WPN_SKILL(unit_data *ch, size_t index)
{
    return UPC(ch)->getWeaponSkillAtIndex(index);
}

inline ubit8 PC_SPL_LVL(unit_data *ch, size_t index)
{
    return UPC(ch)->getSpellLevelAtIndex(index);
}

inline sbit16 PC_SPL_SKILL(unit_data *ch, size_t index)
{
    return UPC(ch)->getSpellSkillAtIndex(index);
}

inline ubit16 PC_CRIMES(unit_data *unit)
{
    return UPC(unit)->getNumberOfCrimesCommitted();
}

#define PC_PWD(unit) (UPC(unit)->pwd)

#define PC_LASTHOST(unit) (UPC(unit)->lasthosts)

#define PC_FILENAME(unit) (UPC(unit)->filename)

inline sbit32 PC_ID(unit_data *unit)
{
    return UPC(unit)->getPlayerUID();
}

inline const char *PC_BANK(const unit_data *unit)
{
    return UPC(unit)->getBank();
}

inline ubit16 PC_FLAGS(unit_data *unit)
{
    return UPC(unit)->getAllPCFlags();
}

inline pc_time_data &PC_TIME(unit_data *unit)
{
    return UPC(unit)->getPCTimeInformation();
}

inline const char *PC_HOME(unit_data *ch)
{
    return UPC(ch)->getHometown();
}
/* .................... PC SUPER STRUCTURE ............................. */

#define PC_IMMORTAL(ch) (IS_PC(ch) && CHAR_LEVEL(ch) >= 200)

#define PC_MORTAL(ch) (IS_PC(ch) && CHAR_LEVEL(ch) < 200)

inline bool PC_IS_UNSAVED(unit_data *ch)
{
    return PC_TIME(ch).getTotalTimePlayedInSeconds() == 0;
}

/* ..................................................................... */

#define NPC_WPN_SKILL(ch, index) (UNPC(ch)->weapons[index])

#define NPC_SPL_SKILL(ch, index) (UNPC(ch)->spells[index])

#define NPC_DEFAULT(unit) (UNPC(unit)->default_pos)

#define NPC_FLAGS(unit) (UNPC(unit)->flags)

/* ..................................................................... */

#define UNIT_TITLE_STRING(unit) (UNIT_TITLE(unit).c_str())

#define UNIT_OUT_DESCR_STRING(unit) (UNIT_OUT_DESCR(unit).c_str())

#define UNIT_IN_DESCR_STRING(unit) (UNIT_IN_DESCR(unit).c_str())

#define TITLENAME(unit) (IS_PC(unit) ? UNIT_NAME(unit) : UNIT_TITLE_STRING(unit))

#define SOMETON(unit) ((UNIT_SEX(unit) == SEX_NEUTRAL) ? "something" : "someone")

/* Title, Name or Someone/Something */
#define UNIT_SEE_TITLE(ch, unit) (CHAR_CAN_SEE(ch, unit) ? TITLENAME(unit) : SOMETON(unit))

#define UNIT_SEE_NAME(ch, unit) (CHAR_CAN_SEE(ch, unit) ? UNIT_NAME(unit) : SOMETON(unit))

/* ..................................................................... */

/* Invis people aren't supposed to have sex... /gnort */

#define B_HSHR(ch) ((UNIT_SEX(ch) == SEX_NEUTRAL) ? "its" : ((CHAR_SEX(ch) == SEX_MALE) ? "his" : "her"))

#define HSHR(to, ch) (CHAR_CAN_SEE((to), (ch)) ? B_HSHR(ch) : "their")

#define B_HESH(ch) ((UNIT_SEX(ch) == SEX_NEUTRAL) ? "it" : ((CHAR_SEX(ch) == SEX_MALE) ? "he" : "she"))

#define HESH(to, ch) (CHAR_CAN_SEE((to), (ch)) ? B_HESH(ch) : "they")

#define B_HMHR(ch) ((UNIT_SEX(ch) == SEX_NEUTRAL) ? "it" : ((CHAR_SEX(ch) == SEX_MALE) ? "him" : "her"))

#define HMHR(to, ch) (CHAR_CAN_SEE((to), (ch)) ? B_HMHR(ch) : "them")

#define UNIT_ANA(unit) ANA(*UNIT_NAME(unit))
