/*
 $Author: All $
 $RCSfile: utils.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.7 $
 */
#ifndef _MUD_UTILS_H
#define _MUD_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "structs.h"
#include "essential.h"

extern const sbit8 time_light[];

/* ..................................................................... */

#define PK_RELAXED            0
#define PK_STRICT             1

#define IS_BEGINNER(ch)      (CHAR_LEVEL(ch) <= START_LEVEL)
#define IS_MORTAL(ch)        (CHAR_LEVEL(ch) <  IMMORTAL_LEVEL)
#define IS_IMMORTAL(ch)      (CHAR_LEVEL(ch) >= IMMORTAL_LEVEL)
#define IS_GOD(ch)           (CHAR_LEVEL(ch) >= GOD_LEVEL)
#define IS_CREATOR(ch)       (CHAR_LEVEL(ch) >= CREATOR_LEVEL)
#define IS_OVERSEER(ch)      (CHAR_LEVEL(ch) >= OVERSEER_LEVEL)
#define IS_ADMINISTRATOR(ch) (CHAR_LEVEL(ch) >= ADMINISTRATOR_LEVEL)
#define IS_ULTIMATE(ch)      (CHAR_LEVEL(ch) == ULTIMATE_LEVEL)

/* ..................................................................... */

/*  Do NOT use these macros unless you know PRECISELY what you are doing!!!
 *  and only if you have to assign directly to them (as in db_utils.c)
 */
#define U_CHAR(u)   ((u)->data.ch)
#define U_NPC(u)    (U_CHAR(u)->specific.npc)
#define U_PC(u)     (U_CHAR(u)->specific.pc)
#define U_OBJ(u)    ((u)->data.obj)
#define U_ROOM(u)   ((u)->data.room)

#ifdef MUD_DEBUG
#define DEBUG_HISTORY
#define UNIT_TYPE_DEBUG
#endif

#ifdef UNIT_TYPE_DEBUG
#define UCHAR(u)   (assert(u && IS_CHAR(u)),	U_CHAR(u))
#define UNPC(u)    (assert(u && IS_NPC(u)),	U_NPC(u))
#define UPC(u)     (assert(u && IS_PC(u)),	U_PC(u))
#define UOBJ(u)    (assert(u && IS_OBJ(u)),	U_OBJ(u))
#define UROOM(u)   (assert(u && IS_ROOM(u)), U_ROOM(u))
#else
#define UCHAR(u)   U_CHAR(u)
#define UNPC(u)    U_NPC(u)
#define UPC(u)     U_PC(u)
#define UOBJ(u)    U_OBJ(u)
#define UROOM(u)   U_ROOM(u)
#endif

/* ..................................................................... */



#define UNIT_FUNC(unit) \
   ((unit)->func)

#define UNIT_FILE_INDEX(unit)   \
   ((unit)->fi)

#define UNIT_MANIPULATE(unit)  \
  ((unit)->manipulate)

#define UNIT_FLAGS(unit)       \
  ((unit)->flags)

#define UNIT_WEIGHT(unit)    \
   ((unit)->weight)

#define UNIT_BASE_WEIGHT(unit)    \
   ((unit)->base_weight)

#define UNIT_SIZE(unit)   \
   ((unit)->size)

#define UNIT_CAPACITY(unit)  \
  ((unit)->capacity)

#define UNIT_BRIGHT(unit)   \
  ((unit)->bright)

#define UNIT_LIGHTS(unit)   \
  ((unit)->light)

#define UNIT_ILLUM(unit)    \
  ((unit)->illum)

#define UNIT_CHARS(unit)   \
  ((unit)->chars)

#define UNIT_CONTAINS(unit)   \
  ((unit)->inside)

#define UNIT_IN(unit)  \
  ((unit)->outside)

#define UNIT_AFFECTED(unit)   \
  ((unit)->affected)

#define UNIT_NAME(unit)  \
  (UNIT_NAMES(unit).Name())

#define UNIT_KEY(unit)   \
  ((unit)->key)

#define UNIT_OPEN_FLAGS(unit)   \
  ((unit)->open_flags)

#define UNIT_OPEN_DIFF(unit)   \
  ((unit)->open_diff)

#define UNIT_TYPE(unit)   \
  ((unit)->status)

#define UNIT_NAMES(unit)    \
  ((unit)->names)

#define UNIT_ALIGNMENT(unit)  \
   ((unit)->alignment)

#define UNIT_HIT(unit)     \
   ((unit)->hp)

#define UNIT_MAX_HIT(unit) \
   ((unit)->max_hp)

#define UNIT_MINV(u)  ((u)->minv)



#define UNIT_TITLE(unit)  \
((unit)->title)

#define UNIT_OUT_DESCR(unit)  \
((unit)->out_descr)

#define UNIT_IN_DESCR(unit)  \
((unit)->in_descr)

#define UNIT_EXTRA(unit)  \
   ((unit)->extra)

/* ..................................................................... */

#define IS_ROOM(unit)  \
   (UNIT_TYPE(unit) == UNIT_ST_ROOM)

#define IS_OBJ(unit)   \
   (UNIT_TYPE(unit) == UNIT_ST_OBJ)

#define IS_NPC(unit)   \
   (UNIT_TYPE(unit) == UNIT_ST_NPC)

#define IS_PC(unit)    \
   (UNIT_TYPE(unit) == UNIT_ST_PC)

#define IS_CHAR(unit)  \
   (IS_SET(UNIT_TYPE(unit), UNIT_ST_PC | UNIT_ST_NPC))



/* ............................FILE INDEX STUFF..................... */

#define FI_ZONENAME(fi)   \
   ((fi) ? (fi)->zone->name : "NO-ZONE")

#define FI_NAME(fi)   \
   ((fi) ? (fi)->name : "NO-NAME")

/* ............................UNIT SUPERSTRUCTURES..................... */

#define UNIT_IS_TRANSPARENT(u)  \
  (!IS_SET(UNIT_FLAGS(u),UNIT_FL_BURIED) && \
   IS_SET(UNIT_FLAGS(u),UNIT_FL_TRANS) && \
   !IS_SET(UNIT_OPEN_FLAGS(u),EX_CLOSED))

#define UNIT_FI_ZONE(unit)   \
   (UNIT_FILE_INDEX(unit) ? (unit)->fi->zone : (class zone_type *) NULL)

#define UNIT_FI_ZONENAME(unit)   \
   (FI_ZONENAME(UNIT_FILE_INDEX(unit)))

#define UNIT_FI_NAME(unit)   \
   (FI_NAME(UNIT_FILE_INDEX(unit)))

#define UNIT_WEAR(unit, part)    \
   (IS_SET(UNIT_MANIPULATE(unit), part))

#define UNIT_IS_OUTSIDE(unit)  \
   (!IS_SET(UNIT_FLAGS(UNIT_IN(unit)), UNIT_FL_INDOORS))

extern int sunlight;

#define UNIT_OUTSIDE_LIGHT(unit)                 \
  (!IS_SET(UNIT_FLAGS(unit),UNIT_FL_INDOORS) ?  \
   time_light[sunlight] : 0)

#define UNIT_IS_DARK(unit)  \
   (UNIT_LIGHTS(unit)+      \
    UNIT_OUTSIDE_LIGHT(unit)+\
    (UNIT_IN(unit)?UNIT_LIGHTS(UNIT_IN(unit)):0) < 0)

#define UNIT_IS_LIGHT(unit) \
  (UNIT_LIGHTS(unit)+       \
   UNIT_OUTSIDE_LIGHT(unit)+\
   (UNIT_IN(unit)?UNIT_LIGHTS(UNIT_IN(unit)):0) >= 0)

#define UNIT_SEX(unit) \
   (IS_CHAR(unit) ? CHAR_SEX(unit) : SEX_NEUTRAL)

#define UNIT_IS_GOOD(ch)    (UNIT_ALIGNMENT(ch) >= 350)
#define UNIT_IS_EVIL(ch)    (UNIT_ALIGNMENT(ch) <= -350)
#define UNIT_IS_NEUTRAL(ch) (!UNIT_IS_GOOD(ch) && !UNIT_IS_EVIL(ch))

#define UNIT_CONTAINING_W(u) \
   (UNIT_WEIGHT(u) - UNIT_BASE_WEIGHT(u))


/* ..................................................................... */


#define ROOM_RESISTANCE(room)  \
  (UROOM(room)->resistance)

#define ROOM_LANDSCAPE(unit)   \
  (UROOM(unit)->movement_type)

#define ROOM_FLAGS(unit)       \
  (UROOM(unit)->flags)

#define ROOM_EXIT(unit, exit)  \
  (UROOM(unit)->dir_option[exit])

#define ROOM_NUM(unit)  \
  (UROOM(unit)->num)

#define ROOM_SC(unit)  \
  (UROOM(unit)->sc)

#define ROOM_WAITD(unit)  \
  (UROOM(unit)->waiting_dijkstra)

#define ROOM_PATH(unit)  \
  (UROOM(unit)->path)

#define ROOM_DISTANCE(unit)  \
  (UROOM(unit)->distance)
/* ..................................................................... */

#define OBJ_RESISTANCE(obj)  \
  (UOBJ(obj)->resistance)

#define OBJ_VALUE(unit, index)   \
  (UOBJ(unit)->value[index])

#define OBJ_PRICE(unit)  \
  (UOBJ(unit)->cost)

#define OBJ_PRICE_DAY(unit)  \
  (UOBJ(unit)->cost_per_day)

#define OBJ_TYPE(unit)  \
  (UOBJ(unit)->type)

#define OBJ_EQP_POS(unit) \
  (UOBJ(unit)->equip_pos)

#define OBJ_FLAGS(obj)  \
   (UOBJ(obj)->flags)

/* ...........................OBJECT SUPERSTRUCTURES..................... */

#define OBJ_HAS_EXTRA(obj,stat)  \
   (IS_SET(OBJ_EXTRA(obj), stat))

#define UNIT_IS_EQUIPPED(obj) \
   (IS_OBJ(obj) && OBJ_EQP_POS(obj))

/* ..................................................................... */

/*#define CHAR_DEX_RED(ch)     \  (UCHAR(ch)->points.dex_reduction)*/

#define CHAR_DESCRIPTOR(ch)  \
  (UCHAR(ch)->descriptor)

#define CHAR_OFFENSIVE(unit) \
   (UCHAR(unit)->points.offensive)

#define CHAR_DEFENSIVE(unit) \
   (UCHAR(unit)->points.defensive)

#define CHAR_FLAGS(unit)  \
  (UCHAR(unit)->points.flags)

#define CHAR_SEX(ch)         \
  (UCHAR(ch)->points.sex)

#define CHAR_LAST_ROOM(unit) \
   (UCHAR(unit)->last_room)

#define CHAR_LAST_ATTACKER(unit) \
   (UCHAR(unit)->last_attacker)

#define CHAR_LAST_ATTACKER_TYPE(unit) \
   (UCHAR(unit)->last_attacker_type)

#define CHAR_POS(ch)      \
  (UCHAR(ch)->points.position)

#define CHAR_LEVEL(ch)    \
   (UCHAR(ch)->points.level)

#define CHAR_RACE(ch) \
   (UCHAR(ch)->points.race)

#define CHAR_ABILITY(ch, index)  \
   (UCHAR(ch)->points.abilities[index])

#define CHAR_STR(ch)      \
   (CHAR_ABILITY(ch, ABIL_STR))

#define CHAR_DEX(ch)      \
   (CHAR_ABILITY(ch, ABIL_DEX))

#define CHAR_CON(ch)      \
   (CHAR_ABILITY(ch, ABIL_CON))

#define CHAR_CHA(ch)      \
   (CHAR_ABILITY(ch, ABIL_CHA))

#define CHAR_BRA(ch)      \
   (CHAR_ABILITY(ch, ABIL_BRA))

#define CHAR_MAG(ch)      \
   (CHAR_ABILITY(ch, ABIL_MAG))

#define CHAR_DIV(ch)      \
   (CHAR_ABILITY(ch, ABIL_DIV))

#define CHAR_HPP(ch)      \
   (CHAR_ABILITY(ch, ABIL_HP))

#define CHAR_ENDURANCE(ch)    \
   (UCHAR(ch)->points.endurance)

#define CHAR_MANA(ch)     \
   (UCHAR(ch)->points.mana)

/* NOT to be used unless by db.... */
#define CHAR_MONEY(ch) \
  (UCHAR(ch)->money)

#define CHAR_EXP(ch)      \
   (UCHAR(ch)->points.exp)

#define CHAR_ATTACK_TYPE(ch)      \
  (UCHAR(ch)->points.attack_type)

#define CHAR_NATURAL_ARMOUR(ch)   \
  (UCHAR(ch)->points.natural_armour)

#define CHAR_SPEED(ch)   \
  (UCHAR(ch)->points.speed)

#define CHAR_COMBAT(ch)  \
  (UCHAR(ch)->Combat)

#define CHAR_FIGHTING(ch)  \
  (UCHAR(ch)->Combat ? CHAR_COMBAT(ch)->Melee() : (class unit_data *) NULL)

#define CHAR_MASTER(ch)  \
  (UCHAR(ch)->master)

#define CHAR_FOLLOWERS(ch)  \
  (UCHAR(ch)->followers)

/* ...........................CHAR SUPERSTRUCTURES....................... */

#define CHAR_IS_READY(ch)    \
   (CHAR_AWAKE(ch) && (CHAR_POS(ch) != POSITION_FIGHTING))

#define CHAR_IS_SNOOPING(ch)  \
   (CHAR_DESCRIPTOR(ch) ? (CHAR_DESCRIPTOR(ch)->snoop.snooping) : \
                          (class unit_data *) NULL)

#define CHAR_IS_SNOOPED(ch) \
   (CHAR_DESCRIPTOR(ch) ? (CHAR_DESCRIPTOR(ch)->snoop.snoop_by) : \
                          (class unit_data *)  NULL)

#define CHAR_IS_SWITCHED(ch) \
   (CHAR_DESCRIPTOR(ch) ? (CHAR_DESCRIPTOR(ch)->original) : \
                          (class unit_data *) NULL)

#define CHAR_SNOOPING(ch) \
  (CHAR_IS_SNOOPING(ch) ? (CHAR_DESCRIPTOR(ch)->snoop.snooping) : (ch))

#define CHAR_SNOOPED(ch) \
  (CHAR_IS_SNOOPED(ch) ? (CHAR_DESCRIPTOR(ch)->snoop.snoop_by) : (ch))

#define CHAR_ORIGINAL(ch) \
   (CHAR_IS_SWITCHED(ch) ? (CHAR_DESCRIPTOR(ch)->original) : (ch))

#define CHAR_AWAKE(ch)        \
   (CHAR_POS(ch) > POSITION_SLEEPING)

#define CHAR_HAS_FLAG(ch, flags) \
   (IS_SET(CHAR_FLAGS(ch), (flags)))

#define CHAR_ROOM_EXIT(ch, door)                  \
   (IS_ROOM(UNIT_IN(ch)) ? ROOM_EXIT(UNIT_IN(ch), (door)) : \
                           (class unit_data *) NULL)

#define CHAR_VISION(ch)    \
  (!CHAR_HAS_FLAG(ch, CHAR_BLIND))

#define CHAR_CAN_SEE(ch, unit) \
  (CHAR_VISION(ch) &&					\
   !IS_SET(UNIT_FLAGS(unit), UNIT_FL_BURIED) &&		\
   (CHAR_LEVEL(ch) >= UNIT_MINV(unit)) &&		\
   (CHAR_LEVEL(ch) >= CREATOR_LEVEL ||			\
    (UNIT_IS_LIGHT(UNIT_IN(ch)) &&			\
     (!IS_SET(UNIT_FLAGS(unit), UNIT_FL_INVISIBLE) ||	\
      CHAR_HAS_FLAG(ch, CHAR_DETECT_INVISIBLE)))))

#define CHAR_CAN_GO(ch, door) \
   (ROOM_EXIT(UNIT_IN(ch),door) && (ROOM_EXIT(UNIT_IN(ch),door)->to_room) \
    && !IS_SET(ROOM_EXIT(UNIT_IN(ch), door)->exit_info, EX_CLOSED))


/* ..................................................................... */

#define PC_ACCESS_LEVEL(pc) \
  (UPC(pc)->nAccessLevel)

#define PC_CRACK_ATTEMPTS(pc) \
  (UPC(pc)->crack_attempts)

#define PC_SETUP(pc)   \
  (UPC(pc)->setup)


#define PC_LIFESPAN(pc)   \
  (UPC(pc)->lifespan)

#define PC_ACCOUNT(pc) \
  (UPC(pc)->account)

#define PC_VIRTUAL_LEVEL(pc) \
  (UPC(pc)->vlvl)

#define PC_INFO(pc) \
  (UPC(pc)->info)

#define PC_SETUP_ECHO(pc)   \
  (UPC(pc)->setup.echo)

#define PC_SETUP_REDRAW(pc)   \
  (UPC(pc)->setup.redraw)

#define PC_SETUP_WIDTH(pc)   \
  (UPC(pc)->setup.width)

#define PC_SETUP_HEIGHT(pc)   \
  (UPC(pc)->setup.height)

#define PC_SETUP_TELNET(pc)   \
  (UPC(pc)->setup.telnet)

#define PC_SETUP_EMULATION(pc)   \
  (UPC(pc)->setup.emulation)

#define PC_SETUP_COLOUR(pc)   \
  (UPC(pc)->setup.colour_convert)

#define PC_GUILD(pc)  \
  (UPC(pc)->guild)

#define PC_PROMPTSTR(pc)  \
  (UPC(pc)->promptstr)

#define PC_PROFESSION(pc)  \
  (UPC(pc)->profession)

#define PC_GUILD_TIME(pc)  \
  (UPC(pc)->guild_time)

#define PC_QUEST(pc)   \
  (UPC(pc)->quest)

#define PC_COND(ch, i)  \
  (UPC(ch)->conditions[i])

#define PC_ABILITY_POINTS(ch) \
  (UPC(ch)->ability_points)

#define PC_SKILL_POINTS(ch) \
  (UPC(ch)->skill_points)



#define PC_ABI_LVL(ch, index)  \
   (UPC(ch)->ability_lvl[index])

#define PC_SKI_SKILL(ch, index)  \
  (UPC(ch)->skills[index])


#define PC_SKI_LVL(ch, index)  \
  (UPC(ch)->skill_lvl[index])

#define PC_WPN_LVL(ch, index)  \
  (UPC(ch)->weapon_lvl[index])

#define PC_WPN_SKILL(ch, index)  \
  (UPC(ch)->weapons[index])

#define PC_SPL_LVL(ch, index)  \
  (UPC(ch)->spell_lvl[index])

#define PC_SPL_SKILL(ch, index)  \
  (UPC(ch)->spells[index])

#define PC_CRIMES(unit)  \
  (UPC(unit)->nr_of_crimes)

#define PC_PWD(unit)  \
  (UPC(unit)->pwd)

#define PC_LASTHOST(unit)  \
  (UPC(unit)->lasthosts)

#define PC_FILENAME(unit)  \
  (UPC(unit)->filename)

#define PC_ID(unit) \
   (UPC(unit)->id)

#define PC_BANK(unit) \
  (UPC(unit)->bank)

#define PC_FLAGS(unit)   \
  (UPC(unit)->flags)

#define PC_TIME(unit)  \
   (UPC(unit)->time)

#define PC_HOME(ch)     \
   (UPC(ch)->hometown)
/* .................... PC SUPER STRUCTURE ............................. */

#define PC_IMMORTAL(ch) (IS_PC(ch) && CHAR_LEVEL(ch) >= 200)

#define PC_MORTAL(ch)   (IS_PC(ch) && CHAR_LEVEL(ch) <  200)

#define PC_IS_UNSAVED(ch)  (PC_TIME(ch).played == 0)

/* ..................................................................... */

#define NPC_WPN_SKILL(ch, index)  \
  (UNPC(ch)->weapons[index])

#define NPC_SPL_SKILL(ch, index)  \
  (UNPC(ch)->spells[index])

#define NPC_DEFAULT(unit)  \
   (UNPC(unit)->default_pos)

#define NPC_FLAGS(unit)  \
   (UNPC(unit)->flags)

/* ..................................................................... */

#define UNIT_TITLE_STRING(unit) \
   (UNIT_TITLE(unit).c_str())

#define UNIT_OUT_DESCR_STRING(unit) \
   (UNIT_OUT_DESCR(unit).c_str())

#define UNIT_IN_DESCR_STRING(unit) \
   (UNIT_IN_DESCR(unit).c_str())

#define TITLENAME(unit) \
  (IS_PC(unit) ? UNIT_NAME(unit) : UNIT_TITLE_STRING(unit))

#define SOMETON(unit) \
   ((UNIT_SEX(unit) == SEX_NEUTRAL) ? "something" : "someone")


/* Title, Name or Someone/Something */
#define UNIT_SEE_TITLE(ch, unit) \
  (CHAR_CAN_SEE(ch, unit) ? TITLENAME(unit) : SOMETON(unit))

#define UNIT_SEE_NAME(ch, unit)   \
  (CHAR_CAN_SEE(ch, unit) ? UNIT_NAME(unit) : SOMETON(unit))


/* ..................................................................... */

/* Invis people aren't supposed to have sex... /gnort */

#define B_HSHR(ch) \
  ((UNIT_SEX(ch) == SEX_NEUTRAL) ? "its" : \
   ((CHAR_SEX(ch) == SEX_MALE) ? "his" : "her"))

#define HSHR(to, ch)   \
  (CHAR_CAN_SEE((to), (ch)) ? B_HSHR(ch) : "their")

#define B_HESH(ch) \
  ((UNIT_SEX(ch) == SEX_NEUTRAL) ? "it" : \
   ((CHAR_SEX(ch) == SEX_MALE) ? "he" : "she"))

#define HESH(to, ch)   \
  (CHAR_CAN_SEE((to), (ch)) ? B_HESH(ch) : "they")

#define B_HMHR(ch) \
  ((UNIT_SEX(ch) == SEX_NEUTRAL) ? "it" : \
   ((CHAR_SEX(ch) == SEX_MALE) ? "him" : "her"))

#define HMHR(to, ch)  \
  (CHAR_CAN_SEE((to), (ch)) ? B_HMHR(ch) : "them")

#define UNIT_ANA(unit) ANA(*UNIT_NAME(unit))

#endif /* _MUD_UTILS_H */
