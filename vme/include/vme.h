/*
 $Author: tperry $
 $RCSfile: vme.h,v $
 $Date: 2003/01/03 00:59:14 $
 $Revision: 2.7 $
 */
#ifndef _MUD_VME_H
#define _MUD_VME_H

/*
	*  Nothing in this file should be changed all Admin definable aspects of
	* 	vme are in the values.h file.
	*
*/

/*Values returned by Loadstr, Savestr*/
#define FILE_LOADED   1
#define FILE_NOT_FOUND   0
#define FILE_TO_LARGE   -1
#define FILE_OUT_OF_MEMORY 2
#define FILE_SAVED  1
#define FILE_NOT_CREATED   0
#define FILE_ILEGAL_OPP   -1
#define FILE_NOT_SAVED   -2

/*end file returns*/

#define START_LEVEL           1
#define MORTAL_MAX_LEVEL     50	/* Maximum level for mortal players */
#define IMMORTAL_LEVEL      200	/* At 200 you can't really die      */
#define GOD_LEVEL           220	/* Here are the *real* gods         */
#define CREATOR_LEVEL       230
#define OVERSEER_LEVEL      253
#define ADMINISTRATOR_LEVEL 254
#define ULTIMATE_LEVEL      255
#define FALSE 0
#define TRUE  1

#define A_HIDEINV  10
#define A_SOMEONE  11
#define A_ALWAYS   12

#define TO_ROOM    0
#define TO_VICT    1
#define TO_NOTVICT 2
#define TO_CHAR    3
#define TO_ALL     4
#define TO_REST    5

#define FIND_UNIT_EQUIP  (0x0001)
#define FIND_UNIT_INVEN  (0x0002)
#define FIND_UNIT_SURRO  (0x0004)
#define FIND_UNIT_ZONE   (0x0008)
#define FIND_UNIT_WORLD  (0x0010)
#define FIND_UNIT_LOCATION_MASK (FIND_UNIT_EQUIP | FIND_UNIT_INVEN | \
                                 FIND_UNIT_SURRO | FIND_UNIT_ZONE  | \
                                 FIND_UNIT_WORLD)

#define FIND_UNIT_PAY    (0x0020)	/* Only find units in pay zones */
#define FIND_UNIT_NOPAY  (0x0040)	/* Only find units in nopay zones */

#define FIND_UNIT_IN_ME  (FIND_UNIT_EQUIP + FIND_UNIT_INVEN)
#define FIND_UNIT_HERE   (FIND_UNIT_IN_ME + FIND_UNIT_SURRO)
#define FIND_UNIT_GLOBAL (FIND_UNIT_HERE  + FIND_UNIT_ZONE + FIND_UNIT_WORLD)

#define FIND_ROOM    (UNIT_ST_ROOM)
#define FIND_OBJ     (UNIT_ST_OBJ)
#define FIND_PC      (UNIT_ST_PC)
#define FIND_NPC     (UNIT_ST_NPC)

#define FIND_CHAR    (FIND_PC  + FIND_NPC)
#define FIND_NONROOM (FIND_OBJ + FIND_CHAR)
#define FIND_UNIT    (FIND_NONROOM + FIND_ROOM)

#define PULSE_SEC                   4	/* Each pulse is 1/4 of a second  */
#define WAIT_SEC                    4
#define PULSE_ZONE      (60*PULSE_SEC)
#define PULSE_MOBILE    (10*PULSE_SEC)
#define PULSE_VIOLENCE   (3*PULSE_SEC)
#define PULSE_POINTS    (30*PULSE_SEC)	/* Pointupdate every 30 secs (2020 - doubled it) */
#define PULSE_ROUND      (1*PULSE_SEC)

#define SECS_PER_REAL_MIN  60
#define SECS_PER_REAL_HOUR (60*SECS_PER_REAL_MIN)
#define SECS_PER_REAL_DAY  (24*SECS_PER_REAL_HOUR)
#define SECS_PER_REAL_YEAR (365*SECS_PER_REAL_DAY)

#define MUD_DAY            24	/* Twentyfour hours per day    */
#define MUD_WEEK            7	/* Seven mud days per mud week */
#define MUD_MONTH          14	/* 21 mud days per mud month   */
#define MUD_YEAR            9	/* 9 mud months per mud year   */

#define SECS_PER_MUD_HOUR  (5*SECS_PER_REAL_MIN)
#define SECS_PER_MUD_DAY   (24*SECS_PER_MUD_HOUR)
#define SECS_PER_MUD_MONTH (MUD_MONTH*SECS_PER_MUD_DAY)
#define SECS_PER_MUD_YEAR  (MUD_YEAR*SECS_PER_MUD_MONTH)


/* These number must be unique AND at the same time it represents */
/* how much each crime is weighted, thus it is four times as bad  */
/* to murder as it is to steal                                    */
#define CRIME_EXTRA     2	/* Extra witness to same crime */
#define CRIME_STEALING  3
#define CRIME_MURDER    8
#define CRIME_PK       64	/* Murder on player */


/* How much light is in the land ? */

#define SUN_DARK  0
#define SUN_RISE  1
#define SUN_LIGHT 2
#define SUN_SET   3

/* And how is the sky ? */

#define SKY_CLOUDLESS  0
#define SKY_CLOUDY     1
#define SKY_RAINING    2
#define SKY_LIGHTNING  3

/* Directions for use in 'pathto' */

#define DIR_NORTH        NORTH
#define DIR_EAST         EAST
#define DIR_SOUTH        SOUTH
#define DIR_WEST         WEST
#define DIR_UP           UP
#define DIR_DOWN         DOWN
#define DIR_NORTHEAST    NORTHEAST
#define DIR_NORTHWEST    NORTHWEST
#define DIR_SOUTHEAST    SOUTHEAST
#define DIR_SOUTHWEST    SOUTHWEST
#define DIR_ENTER        10
#define DIR_EXIT         11
#define DIR_IMPOSSIBLE   12
#define DIR_HERE         13
#define DIR_TRYAGAIN		 14

#define DIR_SL_SHORT     {"n", "e", "s", "w", "u", "d", "ne", "nw", "se", "sw", \
                          "enter", "exit", "impossible", "here", "try again"}
#define DIR_SL_LONG      {"north", "east", "south", "west", "up", "down", \
                          "northeast", "northwest", "southeast", "southwest", \
                          "enter", "exit", "impossible", "here", "try again"}

/* ---------------------- UNIT DEFINITIONS ------------------------- */
/* These are only flags for the purpose of quick testing, a unit     */
/* must NEVER assume more than one of these flags.                   */

#define UNIT_ST_NPC         (0x01)	/* Is this a NPC?                  */
#define UNIT_ST_PC          (0x02)	/* Is this a PC?                   */
#define UNIT_ST_ROOM        (0x04)	/* Is this a ROOM?                 */
#define UNIT_ST_OBJ         (0x08)	/* Is this a OBJ?                  */



/* ------------------ PC DEFINITIONS ----------------------- */
#define PC_RACE_MAX   100
/* Predifined conditions for PC's only */
#define DRUNK        0
#define FULL         1
#define THIRST       2

/* For PC flags */
#define PC_BRIEF        (0x0001)	/* Brief descriptions?                     */
#define PC_PK_RELAXED   (0x0002)	/* Relaxed PK mode?                        */
#define PC_INFORM       (0x0004)	/* Get `Nnn has entered the world.'?       */
#define PC_COMPACT      (0x0008)	/* Compact Display Bit                     */
#define PC_NOWIZ        (0x0010)	/* Can wiz? Can hear wiz?                  */
#define PC_SPIRIT       (0x0020)	/* When player is dead                     */
#define PC_EXPERT       (0x0040)	/* When set, get details about point system */
#define PC_PROMPT       (0x0080)	/* Used with prompt on/off                 */
#define PC_ECHO         (0x0200)	/* Echo communication commands to pc?      */
#define PC_EXITS        (0x0400)	/* Display exits                           */


#define PC_NOSHOUTING   (0x1000)	/* Can shout? (usually wiz punishment)     */
#define PC_NOTELLING    (0x2000)	/* Can tell? (usually wiz punishment)      */
#define PC_NOSHOUT      (0x4000)	/* Can hear shouts?                        */
#define PC_NOTELL       (0x8000)	/* Can hear tells?                         */


/* ------------------------- Zone Reset modes ------------------------------
 RESET_NOT
   Zone is never reset, except at boot time.

 RESET_IFEMPTY
   A zone is only reset if no players are present in the zone.

 RESET_ANYHOW
   A zone is reset regardless of wheter it contains players or not.
*/
#define RESET_NOT 0
#define RESET_IFEMPTY 1
#define RESET_ANYHOW 2



/* ------------------------- Directions ------------------------------
 NORTH, EAST, SOUTH, WEST, UP and DOWN
   The macros to use in exits.
   For example:
      exit[NORTH] to temple;
   We recommend that you use the shorthand direction macros. For example:
      north to temple;
*/
#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3
#define UP    4
#define DOWN  5
#define NORTHEAST 6
#define NORTHWEST 7
#define SOUTHEAST 8
#define SOUTHWEST 9
#define MAX_EXIT 9
/* ------------------------- Sector Types ------------------------------
 Common for all sector types are that when you specify one for your
 location, it does not mean that your location is that type, but
 rather that moving inside your location is like moving in an area
 similar to the one you specify. For example:

   movement SECT_CITY

 Naturally this should be used in a city, but it might be used in a
 maze if you judge that moving about in your maze is similar to
 walking in a city.


 SECT_INSIDE
   This is used when moving is like moving around inside (ie. no
   weather etc.)

 SECT_CITY
   This is used when moving is like moving in a city.

 SECT_FIELD
   This is used when moving is like moving in a field.

 SECT_FOREST
   This is used when moving is like moving in a forest (ie. soft ground
   and obstacles (trees)).

 SECT_HILLS
   This is used when moving is like moving in hills (ie. up and down hill
   movement).

 SECT_MOUNTAIN
   This is used when moving is like moving in mountains (ie. steep up and
   downhills, dangerous passages, loose rocks etc)

 SECT_DESERT
   This is used when moving is like moving in a desert (ie. very heavy
   to move feet in sand, like walking on a beach with loose sand)

 SECT_SWAMP
   This is used when moving is like moving in a swamp (ie. your feet
   are stuck each time you take a step)

 SECT_WATER_SWIM
   This is used when moving is like swimming in water. When the movement
   type is water, things dropped will sink to the bottom. Diving is
   only possible in movement types of type water to types of underwater.

 SECT_WATER_SAIL
   This is used to specify that the room is water, but you can only enter
   if in a boat or similar device. Actually this is meant to be used when
   you want to make a ocean "room" or similar rooms. Swimming is possible
   but probably fatal - an ocean should thus be at least two rooms wide.

 SECT_UNDER_WATER
   This is used when underwater (i.e. breathing isn't possible).

*/

#define SECT_INSIDE          0
#define SECT_CITY            1
#define SECT_FIELD           2
#define SECT_FOREST          3
#define SECT_HILLS           4
#define SECT_MOUNTAIN        5
#define SECT_DESERT          6
#define SECT_SWAMP           7
#define SECT_WATER_SWIM      8
#define SECT_WATER_SAIL      9
#define SECT_UNDER_WATER    10
#define SECT_SNOW           11
#define SECT_SLUSH          12	/* Wet snow */
#define SECT_ICE            13

/* ------------------------- NPC Flags ------------------------------
 INFINITE_CAPACITY
   Set capacity to this if you want it to be infinitely big
*/

#define INFINITE_CAPACITY   -1

/* ------------------------- NPC Flags ------------------------------

 NPC_NICE_THIEF
   The mobile will treat characters that attempt to steal from it non-
   violently.

*/
#define NPC_NICE_THIEF  0x0008


/* ------------------------- UNIT Flags ------------------------------
 UNIT_FL_PRIVATE
   Set this flag if the unit is a wizard unit. For rooms this implies
   private conversations, ei. max two characters can goto a particular
   room.

 UNIT_FL_INVISIBLE
   Set this flag if the unit is invisible.

 UNIT_FL_NO_BURY
   Set this flag if units can NOT be buried inside this unit.
   I.e. set this flag in rooms like a canoe, or a house.

 UNIT_FL_BURIED
   Set this flag if the unit is buried inside whatever it is loaded
   into.

 UNIT_FL_NO_TELEPORT
   Set this flag if no teleport to/from/with this unit is allowed.

    UNIT_FL_NO_TEL 0X8000

 * if on room or object can't teleport out.
 * if on npc or pc blocks teleport

 UNIT_FL_NO_SUM

 * if on room and object can't summon in them
 * if on npc or char can't summon

  UNIT_FL_NO_SUM_FROM

  * if on room can't summon from that room.
  * if on npc or char can't summon them from anywhere



 UNIT_FL_NO_MOB
   Set this flag in a unit, if mobiles are not allowed to enter it.

 UNIT_FL_NO_WEATHER
   Set this flag in a unit if no weather information shall be passed
   to the players inside the unit. When set it also indicates that no
   natural light, lightning, rain etc. is available in the unit.

 UNIT_FL_INDOORS
   Set this bit if the unit is "indoors" ie. it has walls/ceiling
   (living room/cave/dungeon etc). This bit does not affect the
   weather flag, but might have an impact on spells such as
   earthquake :)

 UNIT_FL_TRANS
   Set this flag, if the unit is to be transperant, as f.inst.
   a riding horse, boat, or other object, where you can
   communicate with/see characters outside.

 UNIT_FL_NOSAVE
   If this flag is set the unit is not allowed to be saved as
   inventory. This is useful for the safe key etc.
   If a player is (recursively) inside a room with this flag,
   he won't be loaded into this location when reentering the game.
   This is useful for treasure rooms, dangerous rooms, etc.

 UNIT_FL_SACRED
    When set, all characters in the room will gain hit-points, mana-
   points and endurance points at double rate.


 UNIT_FL_MAGIC
   When this flag is set the unit is radiating an aura of magic when
   it is examined.
*/

#define UNIT_FL_PRIVATE        0x0001
#define UNIT_FL_INVISIBLE      0x0002
#define UNIT_FL_NO_BURY        0x0004
#define UNIT_FL_BURIED         0x0008
#define UNIT_FL_NO_SUM         0x0010
#define UNIT_FL_SACRED         0x0020
#define UNIT_FL_NO_MOB         0x0040
#define UNIT_FL_NO_WEATHER     0x0080
#define UNIT_FL_INDOORS        0x0100
#define UNIT_FL_NO_SUM_FROM    0x0200
#define UNIT_FL_TRANS          0x0400
#define UNIT_FL_XXXUNUSED2     0x0800
#define UNIT_FL_NOSAVE         0x1000
#define UNIT_FL_MAGIC          0x2000
#define UNIT_FL_NO_TEL         0X4000
#define UNIT_FL_NO_TEL_TO      0x8000


#define UNIT_FL_NO_TELEPORT ( UNIT_FL_NO_TEL+UNIT_FL_NO_TEL_TO+UNIT_FL_NO_SUM+UNIT_FL_NO_SUM_FROM)


/* ------------------------ Positions ------------------------------

 POSITION_DEAD
   Do not use this position. Make a corpse instead.

 POSITION_MORTALLYW

 POSITION_INCAP

 POSITION_STUNNED
   A stunned monster will quickly recover to position standing, no need
   to use this position.

 POSITION_SLEEPING

 POSITION_RESTING

 POSITION_SITTING

 POSITION_FIGHTING
   Do not use this position.

 POSITION_STANDING
*/
#define POSITION_DEAD       0
#define POSITION_MORTALLYW  1
#define POSITION_INCAP      2
#define POSITION_STUNNED    3
#define POSITION_SLEEPING   4
#define POSITION_RESTING    5
#define POSITION_SITTING    6
#define POSITION_FIGHTING   7
#define POSITION_STANDING   8



/* ------------------------- MANIPULATE Flags ------------------------------
 MANIPULATE_TAKE
   Set this flag if the unit can be taken (picked up/moved about).

 MANIPULATE_ENTER
   Set this flag if it is possible to enter a unit, ie set it in a
   coffin if you want players to be able to enter the coffin.

 These flags are set to indicate on what body positions a particular
 object can be worn:

 MANIPULATE_WEAR_FINGER
 MANIPULATE_WEAR_NECK
 MANIPULATE_WEAR_BODY
 MANIPULATE_WEAR_HEAD
 MANIPULATE_WEAR_LEGS
 MANIPULATE_WEAR_FEET
 MANIPULATE_WEAR_HANDS
 MANIPULATE_WEAR_ARMS
 MANIPULATE_WEAR_SHIELD
 MANIPULATE_WEAR_ABOUT
   Cloak, robes etc.
 MANIPULATE_WEAR_WAIST
 MANIPULATE_WEAR_WRIST
 MANIPULATE_WIELD
 MANIPULATE_HOLD
   Torches etc.

 MANIPULATE_WEAR_EAR
   Earrings...

 MANIPULATE_WEAR_BACK
   Backpacks, quivers, shields...

 MANIPULATE_WEAR_CHEST
   Across chest, Baldric

 MANIPULATE_WEAR_ANKLE
   Anklets
*/
#define MANIPULATE_TAKE           0x00001
#define MANIPULATE_WEAR_FINGER    0x00002
#define MANIPULATE_WEAR_NECK      0x00004
#define MANIPULATE_WEAR_BODY      0x00008
#define MANIPULATE_WEAR_HEAD      0x00010
#define MANIPULATE_WEAR_LEGS      0x00020
#define MANIPULATE_WEAR_FEET      0x00040
#define MANIPULATE_WEAR_HANDS     0x00080
#define MANIPULATE_WEAR_ARMS      0x00100
#define MANIPULATE_WEAR_SHIELD    0x00200
#define MANIPULATE_WEAR_ABOUT     0x00400
#define MANIPULATE_WEAR_WAIST     0x00800
#define MANIPULATE_WEAR_WRIST     0x01000
#define MANIPULATE_WIELD          0x02000
#define MANIPULATE_HOLD           0x04000
#define MANIPULATE_ENTER          0x08000
#define MANIPULATE_WEAR_EAR       0x10000
#define MANIPULATE_WEAR_BACK      0x20000
#define MANIPULATE_WEAR_CHEST     0x40000
#define MANIPULATE_WEAR_ANKLE     0x80000



/* ------------------------- CHAR Flags ------------------------------
 CHAR_PROTECTED
   Set this flag if the character is protected by the law-system.

 CHAR_LEGAL_TARGET
   Do not use.

 CHAR_OUTLAW
   Do not use.

 CHAR_GROUP
   Do not use.

 CHAR_BLIND
   Set flag if character is blinded.

 CHAR_HIDE
   Set flag if character is hidden.

 CHAR_MUTE
   Set flag if character is mute.

 CHAR_SNEAK
   Set flag if character is in sneaking mode.

 CHAR_DETECT_ALIGN
   No actual effect on NPC's.

 CHAR_DETECT_INVISIBLE
   Set flag if character can see invisible units.

 CHAR_DETECT_MAGIC
   No actual effect on NPC's.

 CHAR_DETECT_POISON
   No actual effect on NPC's.

 CHAR_DETECT_UNDEAD
   No actual effect on NPC's.

 CHAR_DETECT_CURSE
   No actual effect on NPC's.

 CHAR_DETECT_LIFE
   No actual effect on NPC's.

 CHAR_WIMPY
   Set flag if character if wimpy. Wimpy characters flee when they are
   low on hitpoints, and they gain less experience when killing others.
   If a character is both wimpy and aggressive (NPC_AGGRESSIVE) it will
   only attack sleeping players.

 CHAR_SELF_DEFENCE
   Do not use - Internal.

*/
#define CHAR_PROTECTED         0x00001
#define CHAR_LEGAL_TARGET      0x00002
#define CHAR_OUTLAW            0x00004
#define CHAR_GROUP             0x00008
#define CHAR_BLIND             0x00010
#define CHAR_HIDE              0x00020
#define CHAR_MUTE              0x00040
#define CHAR_SNEAK             0x00080
#define CHAR_DETECT_ALIGN      0x00100
#define CHAR_DETECT_INVISIBLE  0x00200
#define CHAR_DETECT_MAGIC      0x00400
#define CHAR_DETECT_POISON     0x00800
#define CHAR_DETECT_UNDEAD     0x01000
#define CHAR_DETECT_CURSE      0x02000
#define CHAR_DETECT_LIFE       0x04000
#define CHAR_WIMPY             0x08000
#define CHAR_SELF_DEFENCE      0x20000
#define CHAR_PEACEFUL          0x40000	/* Auto-attack stuff? */
#define CHAR_KILL_SELF     (0x80000)	/*For death if NPC/PC kills self or no exploss */


/* ------------------------- ITEM Types ------------------------------
 ITEM_LIGHT
   Items of this type can be lighted and extinguished.
   value[0] How many hours of burning left, 0 if
	    burnt out, -1 if eternal.
   value[1] Number of light sources when burning
	    (normally >= 1, never > 10. 1 = torch)

 ITEM_SCROLL
 ITEM_WAND
 ITEM_STAFF
 ITEM_WEAPON
 ITEM_FIREWEAPON
 ITEM_MISSILE
 ITEM_TREASURE
 ITEM_ARMOR
 ITEM_POTION
 ITEM_WORN
 ITEM_OTHER
 ITEM_TRASH
 ITEM_TRAP
 ITEM_CONTAINER
 ITEM_NOTE
 ITEM_DRINKCON
   Value[0] Max-contains
   Value[1] Contains
   Value[2] Liquid Type (LIQ_XXX)
   Value[3] Poison Factor
   Value[4] Unused

   Warning. Setting the value [1] without adjusting the weight of the
   unit will cause a series of weight errors in the environment.

 ITEM_KEY
 ITEM_FOOD
 ITEM_MONEY
 ITEM_PEN
 ITEM_BOAT
 ITEM_SPELL
   A page in a spell book
 ITEM_BOOK
   Spell book
 ITEM_SHIELD
*/
#define ITEM_LIGHT      1
#define ITEM_SCROLL     2
#define ITEM_WAND       3
#define ITEM_STAFF      4
#define ITEM_WEAPON     5
#define ITEM_FIREWEAPON 6
#define ITEM_MISSILE    7
#define ITEM_TREASURE   8
#define ITEM_ARMOR      9
#define ITEM_POTION    10
#define ITEM_WORN      11
#define ITEM_OTHER     12
#define ITEM_TRASH     13
#define ITEM_TRAP      14
#define ITEM_CONTAINER 15
#define ITEM_NOTE      16
#define ITEM_DRINKCON  17
#define ITEM_KEY       18
#define ITEM_FOOD      19
#define ITEM_MONEY     20
#define ITEM_PEN       21
#define ITEM_BOAT      22
#define ITEM_SPELL     23
#define ITEM_BOOK      24
#define ITEM_SHIELD    25
#define ITEM_SKIN      26
#define ITEM_BOARD     27


/* ------------------------- ITEM Types ------------------------------
 WEAR_UNUSED
   Do not use.
 WEAR_FINGER_R
 WEAR_FINGER_L
 WEAR_NECK_1
 WEAR_NECK_2
 WEAR_BODY
 WEAR_HEAD
 WEAR_LEGS
 WEAR_FEET
 WEAR_HANDS
 WEAR_ARMS
 WEAR_SHIELD
 WEAR_ABOUT
 WEAR_WAIST
 WEAR_WRIST_R
 WEAR_WRIST_L
 WEAR_WIELD
 WEAR_HOLD
 WEAR_MAX
   Do not use - internal value.
*/
#define WEAR_UNUSED     1
#define WEAR_FINGER_R   2
#define WEAR_FINGER_L   3
#define WEAR_NECK_1     4
#define WEAR_NECK_2     5
#define WEAR_BODY       6
#define WEAR_HEAD       7
#define WEAR_LEGS       8
#define WEAR_FEET       9
#define WEAR_HANDS     10
#define WEAR_ARMS      11
#define WEAR_SHIELD    12
#define WEAR_ABOUT     13
#define WEAR_WAIST     14
#define WEAR_WRIST_R   15
#define WEAR_WRIST_L   16
#define WEAR_WIELD     17
#define WEAR_HOLD      18

#define WEAR_CHEST     19
#define WEAR_BACK      20
#define WEAR_EAR_L     21
#define WEAR_EAR_R     22
#define WEAR_ANKLE_L   23
#define WEAR_ANKLE_R   24

#define WEAR_MAX       24	/* Top of "table" */



/* ------------------------- Sex Types ------------------------------
 Obivious, aren't they?
 SEX_NEUTRAL
 SEX_MALE
 SEX_FEMALE
*/
#define SEX_NEUTRAL   0
#define SEX_MALE      1
#define SEX_FEMALE    2


/* ------------------------- Lock Flags ------------------------------
 Use these values in door reset commands, or use with "open" flags in
 units.

 EX_OPEN_CLOSE
   Set flag if unit (container)/door can be open and closed.

 EX_CLOSED
   Set flag if the unit(container)/door is closed.

 EX_LOCKED
   Set flag if the unit(container)/door is locked.

 EX_PICKPROOF
   Set flag if the unit(container)/door is pick proof.

 EX_INSIDE_OPEN
   Set if the unit(container only) can be opened and locked from the inside.
   For example a coffin could be opened from both sides, whereas a closet
   perhaps only can be opened from the outside.

 EX_HIDDEN
   When bit is set the exit is only revealed by a command to 'search' in
   a specific direction.

 EX_CLIMB
   When bit set, you must climb to go this way. 
*/
#define EX_OPEN_CLOSE    0x01
#define EX_CLOSED        0x02
#define EX_LOCKED        0x04
#define EX_PICKPROOF     0x08
#define EX_INSIDE_OPEN   0x10
#define EX_HIDDEN        0x20
#define EX_CLIMB         0x40
#define EX_FALL_THIS_WAY 0x80

/* ------------------------- Room Flags ------------------------------
 Use the room flags with "romflags" when defining a room.

*/


/* ------------------------- Object Flags ------------------------------
 Use the object flags with "romflags" when defining an object.

 OBJ_NO_UNEQUIP
   When set, the object can not be un-equipped.

 OBJ_TWO_HANDS
   Used to indicate if an object requires two hands to use. E.g. if set
   on a long sword, then it can not be used in any dual-wield combination,
   or if set on a wand, then no weapon can be wielded simultaneously.

 OBJ_NOCOVER
   Used to indicate that as equipment, this object does not cover
   the bodypart.

 OBJ_NO_DUAL
   Used on weapons to indicate that it can not be used as a secondary
   weapon in a dual-wield.
*/
#define OBJ_NO_UNEQUIP      0x0001
#define OBJ_TWO_HANDS       0x0002
#define OBJ_NOCOVER         0x0004
#define OBJ_NO_DUAL         0x0008



/* ------------------------- Money Values -------------------------------
*/

#define ANY_CURRENCY	-1	/* For internal use only. */
#define DEF_CURRENCY     0

#define MAX_CURRENCY     0


#define     IRON_PIECE   "ip"	/* 0 */
#define   COPPER_PIECE   "cp"	/* 1 */
#define   SILVER_PIECE   "sp"	/* 2 */
#define     GOLD_PIECE   "gp"	/* 3 */
#define PLATINUM_PIECE   "pp"	/* 4 */

#define MAX_MONEY        4	/* This is the number of strings above - 1 */

/* Remember to change these if you change in the money file!! */
#define     IRON_MULT    10
#define   COPPER_MULT     (8 * IRON_MULT)
#define   SILVER_MULT     (8 * COPPER_MULT)
#define     GOLD_MULT     (8 * SILVER_MULT)
#define PLATINUM_MULT     (8 * GOLD_MULT)
/* ------------------------- Light Values  ------------------------------
 INFINITE_BURNTIME
   Set this if the light source never burns out.

*/
#define INFINITE_BURNTIME -1




/* ------------------------- Speed  ------------------------------
   The combat speed is 12 by default and is lower when faster.
   4 is the fastest speed and means 3 attacks per combat round.
   200 is slowest and means 1 attack per 200/12 combat rounds.

   SPEED_DEFAULT is also the same as the duration of one combat round.
*/

#define SPEED_DEFAULT   12
#define SPEED_MIN        4
#define SPEED_MAX      200



/* ------------------------- Damage Groups  ------------------------------
    These are the possible damage groups... For the weapon group, the
    attack number is one of WPN_XXX, for the spell group attack number
    is one of SPL_XXX, for the skill group one of SKI_XXX and finally
    for the _OTHER group, one of MSG_OTHER_XXX
*/

#define MSG_TYPE_WEAPON       0
#define MSG_TYPE_SPELL        1
#define MSG_TYPE_SKILL        2
#define MSG_TYPE_OTHER        3

/* ------------------------- Damage Groups  ------------------------------
    These are the damage identifiers fro the MSG_TYPE_OTHER damage group,
    as shown above.
*/

#define MSG_OTHER_BLEEDING    1	/* Messages when bleeding on ground */
#define MSG_OTHER_POISON      2	/* Messages when poison suffering   */
#define MSG_OTHER_STARVATION  3	/* Messages when starving/thirsting */


/* ------------------------- Armour Types  ------------------------------
   ARM_XXX is the armour type. Use the definied macros instead of these
	   values when defining armours.
*/
#define ARM_CLOTHES  0		/* Naked / clothes is only protection    */
#define ARM_LEATHER  1		/* A soft flexible leather base armour   */
#define ARM_HLEATHER 2		/* A hard unflexible leather base armour */
#define ARM_CHAIN    3		/* A flexible armour composed of interlocking rings */
#define ARM_PLATE    4		/* An unflexible plate armour. */
#define MAX_ARMOUR_TYPES  5

/* ------------------------- Monster Size  ------------------------------
   SIZ_XXX is the size of the monster. This has effect on the maximum
	   damage it can give with NATURAL attacks (hand, bite, etc).
           The size is dependant on WEIGHT only.
*/

#define SIZ_TINY    0		/*   0 -   5 lbs */
#define SIZ_SMALL   1		/*  11 -  40 lbs */
#define SIZ_MEDIUM  2		/*  41 - 160 lbs */
#define SIZ_LARGE   3		/* 161 - 500 lbs */
#define SIZ_HUGE    4		/* 500+ lbs      */



/* ------------------------- Skill Values  ------------------------------
   ABIL_XXX refers to the abilitity. Use 'ability[ABIL_XXX] = yyy' in
	    the zone, or use predefined macros.
*/
#define ABIL_MAG               0
#define ABIL_DIV               1
#define ABIL_STR               2
#define ABIL_DEX               3
#define ABIL_CON               4
#define ABIL_CHA               5
#define ABIL_BRA               6
#define ABIL_HP                7
#define ABIL_TREE_MAX          8	/* Do not use */

/* All the nodes in the weapon tree

   WPN_XXX refers to the weapon skills, attack types, and weapon types.
	   For the skill, you should only use the 'group' weapons (0..6),
	   or use the predefined macros
*/

#define WPN_ROOT      0		/* Root of weapon tree        */
#define WPN_AXE_HAM   1		/* Axe/hammer category        */
#define WPN_SWORD     2		/* Sword Category             */
#define WPN_CLUB_MACE 3		/* Club/Mace/Flail Category   */
#define WPN_POLEARM   4		/* Polearm category           */
#define WPN_UNARMED   5		/* Natural attacks            */
#define WPN_SPECIAL   6		/* Special weapons            */
#define WPN_GROUP_MAX 7		/* #[0..6] of groups          */
#define WPN_TREE_MAX     200	/* Top of weapon list  #[0..48] = 49 */

/* For use with armours and shields, in the 'value[X]' */

#define SHIELD_SMALL   0	/* Small Shield */
#define SHIELD_MEDIUM  1	/* Medium Shield */
#define SHIELD_LARGE   2	/* Large Shield */

/* Shield methods set for each weapon type & spell type */

#define SHIELD_M_BLOCK   0	/* Can completely block the attack              */
#define SHIELD_M_REDUCE  1	/* Can partially reduce the damage of an attack */
#define SHIELD_M_USELESS 2	/* Can not help at all against an attack        */
#define RACE_DO_NOT_USE    15000

/* ..................................................................... */
/*                              S P E L L S                              */
/* ..................................................................... */

/* Spell numbers. Only use the groups for 'spell[X]'... */

#define SPL_NONE              -1	/* For wand, scrolls, etc. */
#define SPL_ALL                0
#define SPL_DIVINE             1
#define SPL_PROTECTION         2
#define SPL_DETECTION          3
#define SPL_SUMMONING          4
#define SPL_CREATION           5
#define SPL_MIND               6
#define SPL_HEAT               7	/* Fire          */
#define SPL_COLD               8	/* Frost         */
#define SPL_CELL               9	/* Electricity   */
#define SPL_INTERNAL          10	/* Poison        */
#define SPL_EXTERNAL          11	/* Acid          */
#define SPL_GROUP_MAX         12	/* #[0..11] = 12 */

/*
	Max number of spells
*/
#define SPL_TREE_MAX         500	/* #[0..167] = 168 */
/*
	Max number of skills
*/
#define SKI_TREE_MAX         300



/* ------------------------- Special Functions ------------------------------
When SFR_BLOCK is returned, all other scannings for further special routines
are canceled, as well as blocking any command issued.
   SFR_BLOCK

The function may share with others/continue command sequence
   SFR_SHARE
*/

#define SFR_BLOCK              5
#define SFR_SHARE              6


/* ------------------------- Special Functions ------------------------------
  Used in conjunction with 'special ... bits SFB_XXX' keyword.

  SFB_PRIORITY
    When this bit is set, no routines 'below' the specified routine will
    be allowed to be called on any basis. This is only performed locally
    relative to the unit in question ('below' is considered ->next).

  SFB_RANTIME
    (for compatibility also: SFB_TIME1, SFB_TIME2, SFB_TIME3)
    When this bit is set, the next event 'time' will be randomly requested
    at (T=time):   [T-T/2..T+T/2]
*/

#define GS_PLAY 0
#define GS_QUIT 1
#define GS_MENU 2
#define GS_LINK_DEAD 3

#define SFB_PRIORITY (0x0001)	/* Dont allow SFUNS below when this is set   */
#define SFB_RANTIME  (0x0002)	/* Time variation                            */
#define SFB_CMD      (0x0004)	/* normal command                            */
#define SFB_TICK     (0x0008)	/* tick messages                             */
#define SFB_DEAD     (0x0010)	/* death message                             */
#define SFB_COM      (0x0020)	/* combat event                              */
#define SFB_MSG      (0x0040)	/* message event                             */
#define SFB_SAVE     (0x0080)	/* save event                                */
#define SFB_AWARE    (0x0100)	/* Activate self? (self awareness)           */
#define SFB_ACTIVATE (0x0200)	/* DIL only! .. on actiavtion                */
#define SFB_PRE      (0x0400)	/* Used to pre ack (block) messages          */
#define SFB_DONE     (0x0800)	/* Used to tell when something has been done */
#define SFB_EDIT     (0x1000)	/* Edit */
#define SFB_DILDESTROY     (0x2000)	/* Edit */
#define SFB_PROMPT     (0x4000)	/* PROMPT */
#define SFB_UNIQUE     (0x10000)	/* PROMPT */
#define SFB_ALL   (SFB_CMD|SFB_TICK|SFB_DEAD|SFB_COM|SFB_MSG|SFB_SAVE|SFB_PRE)

/* ------------------------- Special Functions ------------------------------
SFUN_ACCUSE

SFUN_PROTECT_LAWFUL

SFUN_BANK

SFUN_DUMP

SFUN_WARD_GUARD

SFUN_GUARD_RETURN
   INTERNAL USE ONLY.

SFUN_GUARD_ARRESTING

SFUN_CUFFS

SFUN_CUFFS_LINK
   INTERNAL USE ONLY.

SFUN_CAPTAIN

SFUN_TEACH_ABL

SFUN_TEACH_WPN

SFUN_TEACH_SPL

SFUN_TEACH_SKI

SFUN_ARREST_CHECK


*/

#define FN_PRI_RES                 0  /* don't use                       */
#define FN_PRI_SYS                 1  /* functions dealing with yr death */
#define FN_PRI_DEATH               2  /* functions dealing with yr death */
#define FN_PRI_BODY                5  /* gaining hits, etc.              */
#define FN_PRI_COMBAT             10  /* in-Combat moves                 */
#define FN_PRI_RESCUE             20  /* E.g. rescue someone rather than passing by */
#define FN_PRI_BLOCK              25  /* Block stuff like commands       */
#define FN_PRI_MISSION            30  /* Sent on important mission       */
#define FN_PRI_CHORES             40  /* Picking up garbage, eating rbts */



#define SFUN_DILCOPY_INTERNAL      1	/* Dont use.                       */
#define SFUN_HUNTING               4	/* INTERNAL, DO NOT USE!           */
#define SFUN_ACCUSE                8	/* Accuse function for law system  */
#define SFUN_PROTECT_LAWFUL        9	/* Protection of protected chars   */
#define SFUN_NPC_VISIT_ROOM       12	/* Protection of protected chars   */
#define SFUN_BANK                 15	/* Banker routine                  */
#define SFUN_TEACH_INIT           25	/* Midgaard teacher function       */
#define SFUN_TEACHING             26	/* INTERNAL DO NOT USE!            */
#define SFUN_LOG_OBJECT           31	/* Wiz tool.                       */
#define SFUN_DICTIONARY           51	/* Func for Alias object           */
#define SFUN_GUILD_GUARD_WAY      67
#define SFUN_MEMBERS_ONLY         68
#define SFUN_WHISTLE              69
#define SFUN_GUILD_MASTER         70
#define SFUN_GUILD_BASIS          71
#define SFUN_GUILD_TITLES         75
#define SFUN_REWARD_BOARD         76
#define SFUN_REWARD_GIVE          77
#define SFUN_ERROR_ERASE          79	/* Do not use */
#define SFUN_INFO_ERASE           80	/* Do not use */
#define SFUN_DIL_INTERNAL         82
#define SFUN_ORACLE               83
#define SFUN_ADMINISTRATOR        84
#define SFUN_CRYSTAL_BALL         86
#define SFUN_SLIME_ADM            87
#define SFUN_GUILD_INTERNAL       90	/* Internal use only */
#define SFUN_DIL_COPY            101
#define SFUN_TOP_IDX             101	/* SANITY CHECKING, ADJUST */

/* ------------------------- Affect Types ------------------------------
 Dont ask, and don't use. Use the predefined macros in wmacros.h
*/
#define ID_FILE_STORED        0	/* For empty affects in player-file */
#define ID_NONE               1
#define ID_CORPSE             2	/* Makes corpses disappear */
#define ID_VALHALLA           3
#define ID_LIFE_PROTECTION    4
#define ID_RAISE_MAG          5
#define ID_RAISE_DIV          6
#define ID_RAISE_STR          7
#define ID_RAISE_DEX          8
#define ID_RAISE_CON          9
#define ID_RAISE_CHA         10
#define ID_RAISE_BRA         11
#define ID_RAISE_HPP         12
#define ID_RAISE_DIVINE      13
#define ID_RAISE_SUMMONING   16
#define ID_RAISE_MIND        18
#define ID_RAISE_HEAT        19
#define ID_RAISE_COLD        20
#define ID_RAISE_CELL        21
#define ID_RAISE_INTERNAL    22
#define ID_RAISE_EXTERNAL    23
#define ID_POISON            24
#define ID_MAGIC_LIGHT       25	/* For magic spells              */
#define ID_LIGHT_EXTINGUISH  26	/* For light/extinguish commands */
#define ID_DETECT_ALIGN      27
#define ID_DETECT_INVISIBLE  28
#define ID_DETECT_MAGIC      29
#define ID_DETECT_POISON     30
#define ID_DETECT_UNDEAD     31
#define ID_DETECT_CURSE      32
#define ID_DETECT_LIFE       33
#define ID_BLIND_CHAR        34
#define ID_FEAR              35
#define ID_INVISIBILITY      36
#define ID_SNEAK             37	/* for sneak skill   */
#define ID_HIDE              38	/* for hide skill    */
#define ID_WITNESS           39	/* Used by justice.c */
#define ID_SLEEP             40
#define ID_BLESS             41
#define ID_CURSE             42
#define ID_SANCTUARY         43
#define ID_INSANITY          44
#define ID_PROT_EVIL         45
#define ID_SUSTAIN           46
#define ID_PRISON            47
#define ID_PLAGUE            48
#define ID_ENCHANT_ARMOUR    49
#define ID_ENCHANT_WEAPON    50
#define ID_HOLD              51
#define ID_CHARM             52
#define ID_CLONE             53
#define ID_ABSORBTION        54	/* for indicating absorbtion spell */
#define ID_ENERGY_DRAIN      55
#define ID_SPOTTED_SECRET    56
#define ID_HIDDEN_DIFFICULTY 57	/* Use macro: SECRET_DOOR_DIFFICULTY */
#define ID_REWARD            58
#define ID_CONFUSION         59
#define ID_BURIED            60
#define ID_BACKSTABBED       61	/* When backstabbed, this improves defense */
#define ID_MAX_ATTACKER      62	/* Record of highest level attacker        */
#define ID_PROT_GOOD         63
#define ID_NATURAL_ARMOUR    64
#define ID_SPEED             65
#define ID_CHARFLAGS         66	/* For the transfer functions */
#define ID_SPELL             67	/* ditto */
#define ID_SKILL             68	/* ditto */
#define ID_WEAPON            69	/* ditto */
#define ID_MAGIC_DARK        70	/* For magic spells              */

#define ID_SPL_RAISE_MAG         71
#define ID_SPL_RAISE_DIV         72
#define ID_SPL_RAISE_STR         73
#define ID_SPL_RAISE_DEX         74
#define ID_SPL_RAISE_CON         75
#define ID_SPL_RAISE_CHA         76
#define ID_SPL_RAISE_BRA         77
#define ID_SPL_RAISE_HPP         78
#define ID_SPL_RAISE_DIVINE      79
#define ID_SPL_RAISE_SUMMONING   80
#define ID_SPL_RAISE_MIND        81
#define ID_SPL_RAISE_HEAT        82
#define ID_SPL_RAISE_COLD        83
#define ID_SPL_RAISE_CELL        84
#define ID_SPL_RAISE_INTERNAL    85
#define ID_SPL_RAISE_EXTERNAL    86

#define ID_WEAPON_DEFENSE        87
#define ID_SPELL_DEFENSE         88
#define ID_UNHOLY_BLESSING		 89

#define ID_TOP_IDX              4096

#define ID_TRANSFER_MAG        (-ID_RAISE_MAG)
#define ID_TRANSFER_DIV        (-ID_RAISE_DIV)
#define ID_TRANSFER_STR        (-ID_RAISE_STR)
#define ID_TRANSFER_DEX        (-ID_RAISE_DEX)
#define ID_TRANSFER_CON        (-ID_RAISE_CON)
#define ID_TRANSFER_CHA        (-ID_RAISE_CHA)
#define ID_TRANSFER_BRA        (-ID_RAISE_BRA)
#define ID_TRANSFER_HPP        (-ID_RAISE_HPP)
#define ID_TRANSFER_SPEED      (-ID_SPEED)
#define ID_PROT_GOOD_TRANSFER  (-ID_PROT_GOOD)
#define ID_PROT_EVIL_TRANSFER  (-ID_PROT_EVIL)

#define ID_TRANSFER_CHARFLAGS  (-ID_CHARFLAGS)
#define ID_SPELL_TRANSFER      (-ID_SPELL)
#define ID_SKILL_TRANSFER      (-ID_SKILL)
#define ID_WEAPON_TRANSFER     (-ID_WEAPON)

#define APF_NONE           -1
#define APF_ABILITY         0
#define APF_SPELL_ADJ       1
#define APF_LIGHT_DARK      2
#define APF_MOD_CHAR_FLAGS  3
#define APF_MOD_UNIT_FLAGS  4
#define APF_MOD_OBJ_FLAGS   5
#define APF_SKILL_ADJ       6
#define APF_WEAPON_ADJ      7
#define APF_NATURAL_ARMOUR  8
#define APF_SPEED           9

#define APF_MAX             9

#define TIF_NONE               -1
#define TIF_CORPSE_DECAY        0
#define TIF_CORPSE_ZAP          1
#define TIF_VALHALLA_RET        2
#define TIF_HIT_INC             3
#define TIF_HIT_DEC             4
#define TIF_MAG_INC             5
#define TIF_MAG_DEC             6
#define TIF_DIV_INC             7
#define TIF_DIV_DEC             8
#define TIF_STR_INC             9
#define TIF_STR_DEC            10
#define TIF_DEX_INC            11
#define TIF_DEX_DEC            12
#define TIF_CON_INC            13
#define TIF_CON_DEC            14
#define TIF_CHA_INC            15
#define TIF_CHA_DEC            16
#define TIF_BRA_INC            17
#define TIF_BRA_DEC            18
#define TIF_TGH_INC            19
#define TIF_TGH_DEC            20
#define TIF_PROTECT_INC        21
#define TIF_PROTECT_DEC        22
#define TIF_POISON_ON          23
#define TIF_POISON_OFF         24
#define TIF_POISON_SUFFER      25
#define TIF_CHANGE_LIGHT_ADD   26
#define TIF_CHANGE_LIGHT_SUB   27
#define TIF_TORCH_TICK         28
#define TIF_EYES_TINGLE        29
#define TIF_BLIND_ON           30
#define TIF_BLIND_OFF          31
#define TIF_FEAR_CHECK         32
#define TIF_INVISIBILITY_ON    33
#define TIF_INVISIBILITY_OFF   34
#define TIF_SNEAK_ON           35
#define TIF_SNEAK_OFF          36
#define TIF_SNEAK_TICK         37
#define TIF_HIDE_ON            38
#define TIF_HIDE_OFF           39
#define TIF_HIDE_TICK          40
#define TIF_NOHIDE_TICK        41
#define TIF_SLEEP_ON           42
#define TIF_SLEEP_CHECK        43
#define TIF_SLEEP_OFF          44
#define TIF_BLESS_ON           45
#define TIF_BLESS_TICK         46
#define TIF_BLESS_OFF          47
#define TIF_CURSE_ON           48
#define TIF_CURSE_OFF          49
#define TIF_SANCTUARY_ON       50
#define TIF_SANCTUARY_TICK     51
#define TIF_SANCTUARY_OFF      52
#define TIF_INSANITY_ON        53
#define TIF_INSANITY_TICK      54
#define TIF_INSANITY_OFF       55
#define TIF_PROT_EVIL_ON       56
#define TIF_PROT_EVIL_OFF      57
#define TIF_SUSTAIN_ON         58
#define TIF_SUSTAIN_TICK       59
#define TIF_SUSTAIN_OFF        60
#define TIF_JAIL_WAIT          61
#define TIF_JAIL_RELEASE       62
#define TIF_PLAGUE_ON          63
#define TIF_PLAGUE_TICK        64
#define TIF_PLAGUE_OFF         65
#define TIF_REWARD_ON          66
#define TIF_REWARD_OFF         67
#define TIF_SPL_INC            68
#define TIF_SPL_DEC            69
#define TIF_SKI_INC            70
#define TIF_SKI_DEC            71
#define TIF_WPN_INC            72
#define TIF_WPN_DEC            73
#define TIF_CONFUSION_ON       74
#define TIF_CONFUSION_OFF      75
#define TIF_CONFUSION_TICK     76
#define TIF_BURIED_DESTRUCT    77
#define TIF_PROT_GOOD_ON       78
#define TIF_PROT_GOOD_OFF      79
#define TIF_ARMOUR_ON          80
#define TIF_ARMOUR_OFF         81
#define TIF_SPEED_BETTER       82
#define TIF_SPEED_WORSE        83
#define TIF_BERSERK_ON         84
#define TIF_BERSERK_TICK       85
#define TIF_BERSERK_OFF        86
#define TIF_RAGE_ON            87
#define TIF_RAGE_TICK          88
#define TIF_RAGE_OFF           89

#define TIF_MAX                89

#define CMD_AUTO_EDIT    -13	/* Used when any pc leaves the game            */
#define CMD_AUTO_LEAVE   -12	/* Used when any pc leaves the game            */
#define CMD_AUTO_PLAY    -11	/* Used when any pc enters the game playing    */
#define CMD_AUTO_DAMAGE  -10	/* Used when any damage is given               */
#define CMD_AUTO_ENTER    -9	/* Used when room is entered (walk, sail, etc) */
#define CMD_AUTO_SAVE     -8	/* Used when player is saved                   */
#define CMD_AUTO_MSG      -7	/* Used to pass messages                       */
#define CMD_AUTO_EXTRACT  -6	/* Used when extract_unit is called            */
#define CMD_NON_USABLE    -5	/* Used when there is nothing apropriate       */
#define CMD_AUTO_UNKNOWN  -4	/* Used when cmd_interpreter doesn't recognize */
			      /* command typed by ch. cmd->cmd_str is used   */
#define CMD_AUTO_DEATH    -3	/* Used when someone is killed (combat)        */
#define CMD_AUTO_COMBAT   -2	/* Used when combat round                      */
#define CMD_AUTO_TICK     -1	/* MUST BE -1 ,Used when units are "ticking"   */

#endif /* _MUD_VME_H */
