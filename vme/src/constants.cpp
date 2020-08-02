/*
 $Author: tperry $
 $RCSfile: constants.cpp,v $
 $Date: 2003/01/30 04:08:57 $
 $Revision: 2.3 $
 */

#include <stdlib.h>

#include "utils.h"
#include "structs.h"
#include "vmelimits.h"
#include "skills.h"
#include "protocol.h"
#include "constants.h"
#include "values.h"


/* ---------------------------------------------------------------------- */
/*                         M O V E M E N T                                */
/* ---------------------------------------------------------------------- */

const char *char_pos[] = {
    "dead R.I.P.",
    "mortally-wounded",
    "incapacitated",
    "stunned",
    "sleeping",
    "resting",
    "sitting",
    "fighting",
    "standing",
    NULL
};


/* Moved to common.c: const char *dirs[] = */

const char *enter_dirs[] = {
    "the north",
    "the east",
    "the south",
    "the west",
    "above",
    "below",
    "the northeast",
    "the northwest",
    "the southeast",
    "the southwest",
    NULL
};

const int rev_dir[] = {
    2,
    3,
    0,
    1,
    5,
    4,
    9,
    8,
    7,
    6
};


const char *room_landscape[] = {
    "inside",
    "city",
    "field",
    "forest",
    "hills",
    "mountain",
    "desert",
    "swamp",

    "water-swim",
    "water-sail",
    "under-water",
    "snow",
    "slush",
    "ice",
    NULL
};


const int movement_loss[] = {
    1,				/* Inside     */
    1,				/* City       */
    2,				/* Field      */
    3,				/* Forest     */
    4,				/* Hills      */
    6,				/* Mountains  */
    8,				/* Desert     */
    8,				/* Swamp      */
    20,				/* low-water  */
    20,				/* deep-water */
    20,				/* underwater */
    8,				/* snow */
    8,				/* slush */
    10				/* ice   */
};


/* ---------------------------------------------------------------------- */
/*                               T I M E                                  */
/* ---------------------------------------------------------------------- */

const char *weekdays[MUD_WEEK] = {
    "the Day of the Moon",
    "the Day of the Bull",
    "the Day of the Deception",
    "the Day of Thunder",
    "the Day of Freedom",
    "the Day of the Great Gods",
    "the Day of the Sun"
};

const char *month_name[MUD_YEAR] = {
    "Month of Winter",		/* Winter 0 */
    "Month of the Winter Wolf",	/* Winter 1 */
    "Month of the Frost Giant",	/* Winter 2 */

    "Month of Spring",	/* Spring 3 */
    "Month of Futility",		/* Spring 4 */

    "Month of the Sun",		/* Summer 5 */
    "Month of the Heat",		/* Summer 6 */

    "Month of the Long Shadows",	/* Fall   7 */
    "Month of the Ancient Darkness"	/* Fall   8 */
};

const sbit8 time_light[SUN_SET + 1] = {
    -1,				/* SUN_DARK  */
        0,				/* SUN_RISE  */
        1,				/* SUN_LIGHT */
        0				/* SUN_SET   */
    };

/* ---------------------------------------------------------------------- */
/*                          C O N S U M A B L E                           */
/* ---------------------------------------------------------------------- */

/* char *drinks moved to dil functions! */
const char *fullness[] = {
    "less than half ",
    "about half ",
    "more than half ",
    "",				/* nothing makes "full" appear */
    NULL
};


/* ---------------------------------------------------------------------- */
/*                         I N F O R M A T I V E                          */
/* ---------------------------------------------------------------------- */

const char *arm_text[ARM_PLATE + 1 + 1] = {
    "clothes base",
    "leather base",
    "hardened leather base",
    "chain base",
    "plate base",
    NULL
};

const char *shi_text[SHIELD_LARGE + 2] = {
    "small shield",
    "medium shield",
    "large shield",
    NULL
};

const char *bodyparts[] = {
    "arms",
    "left arm",
    "right arm",
    "legs",
    "left leg",
    "right leg",
    "head",
    "feet",
    "right foot",
    "left foot",
    "toes",
    "hands",
    "right hand",
    "left hand",
    "face",
    "hair",
    "neck",
    "butt",
    "eyes",
    "right eye",
    "left eye",
    "mouth",
    "ears",
    "right ear",
    "left ear",
    "teeth",
    "tounge",
    "nose",
    "nostrils",
    NULL
};

const ubit8 bodyparts_cover[31][5] = {
    {WEAR_ARMS, WEAR_UNUSED},	/* arms */
    {WEAR_ARMS, WEAR_UNUSED},	/* left arm */
    {WEAR_ARMS, WEAR_UNUSED},	/* right arm */
    {WEAR_LEGS, WEAR_UNUSED},	/* legs */
    {WEAR_LEGS, WEAR_UNUSED},	/* left leg */
    {WEAR_LEGS, WEAR_UNUSED},	/* right leg */
    {WEAR_HEAD, WEAR_UNUSED},	/* head */
    {WEAR_FEET, WEAR_UNUSED},	/* feet */
    {WEAR_FEET, WEAR_UNUSED},	/* right foot */
    {WEAR_FEET, WEAR_UNUSED},	/* left foot */
    {WEAR_FEET, WEAR_UNUSED},	/* toes */
    {WEAR_HANDS, WEAR_UNUSED},	/* hands */
    {WEAR_HANDS, WEAR_UNUSED},	/* right hand */
    {WEAR_HANDS, WEAR_UNUSED},	/* left hand */
    {WEAR_HEAD, WEAR_UNUSED},	/* face */
    {WEAR_HEAD, WEAR_UNUSED},	/* hair */
    {WEAR_NECK_1, WEAR_NECK_2, WEAR_UNUSED},	/* neck */
    {WEAR_ABOUT, WEAR_BODY, WEAR_LEGS, WEAR_WAIST, WEAR_UNUSED},	/* butt */
    {WEAR_HEAD, WEAR_UNUSED},	/* eyes */
    {WEAR_HEAD, WEAR_UNUSED},	/* right eye */
    {WEAR_HEAD, WEAR_UNUSED},	/* left eye */
    {WEAR_HEAD, WEAR_UNUSED},	/* mouth */
    {WEAR_HEAD, WEAR_UNUSED},	/* ears */
    {WEAR_HEAD, WEAR_UNUSED},	/* right ear */
    {WEAR_HEAD, WEAR_UNUSED},	/* left ear */
    {WEAR_HEAD, WEAR_UNUSED},	/* teeth */
    {WEAR_HEAD, WEAR_UNUSED},	/* tounge */
    {WEAR_HEAD, WEAR_UNUSED},	/* nose */
    {WEAR_HEAD, WEAR_UNUSED}	/* nostrils */
};



const char *where[] = {
    "&lt;ILLEGAL POSITION&gt;",	/* Position 0 is not a position */
    "&lt;ILLEGAL POSITION&gt;",	/* Position light is not used   */
    "&lt;worn on finger&gt;",
    "&lt;worn on finger&gt;",
    "&lt;worn around neck&gt;",
    "&lt;worn around neck&gt;",
    "&lt;worn on body&gt;",
    "&lt;worn on head&gt;",
    "&lt;worn on legs&gt;",
    "&lt;worn on feet&gt;",
    "&lt;worn on hands&gt;",
    "&lt;worn on arms&gt;",
    "&lt;worn as shield&gt;",
    "&lt;worn about body&gt;",
    "&lt;worn about waist&gt;",
    "&lt;worn around wrist&gt;",
    "&lt;worn around wrist&gt;",
    "&lt;wielded&gt;",
    "&lt;held&gt;",
    "&lt;worn across chest&gt;",
    "&lt;worn on back&gt;",
    "&lt;worn on left ear&gt;",
    "&lt;worn on right ear&gt;",
    "&lt;worn on ankle&gt;",
    "&lt;worn on ankle&gt;",
    NULL
};

/* ---------------------------------------------------------------------- */
/*                      P O I N T   S Y S T E M                           */
/* ---------------------------------------------------------------------- */

const char *npc_class_types[] = {
    "Normal",
    "Undead",
    NULL
};

const struct skill_interval how_good[] = {
    {-20, "horrible"},
    {-15, "very bad"},
    {-10, "bad"},
    {-5, "worse than average"},
    {0, "average"},
    {5, "a little better than average"},
    {10, "better than average"},
    {15, "good"},
    {20, "very good"},
    {25, "supreme"},
    {-1, NULL}
};

const struct skill_interval weapon_skills[] = {
    {0, "utterly hopeless"},
    {10, "impossible"},
    {25, "poor"},
    {40, "good"},
    {50, "skilled"},
    {70, "specialized"},
    {90, "supreme"},
    {100, "expert"},
    {150, "godly"},
    {-1, NULL}
};

const struct skill_interval skill_skills[] = {
    {0, "utterly hopeless"},
    {10, "have heard about"},
    {25, "have tried a little"},
    {40, "have practiced"},
    {50, "are good at"},
    {70, "are an expert at"},
    {90, "master"},
    {100, "are expert at"},
    {150, "are godly at"},
    {-1, NULL}
};

const struct skill_interval spell_skills[] = {
    {10, "might remember"},
    {25, "can almost remember"},
    {40, "can remember"},
    {50, "have practiced"},
    {70, "have learned"},
    {90, "master"},
    {100, "are expert with"},
    {150, "are godly with"},
    {-1, NULL}
};

/* ---------------------------------------------------------------------- */
/*                         B I T - V E C T O R S                          */
/* ---------------------------------------------------------------------- */

const char *sfb_flags[] = {
    "Priority",
    "Time_Variation",
    "Normal_Cmd",
    "Tick",
    "Death",
    "Combat",
    "Message",
    "Save",
    "Aware",
    "Activate",
    "PRE",
    "Done",
    "Edit",
    "DILDestroy",
    "Prompt",
    "Unique",
    NULL
};

/* UNIT_MANIPULATE */
const char *unit_manipulate[] = {
    "take",
    "finger",
    "neck",
    "body",
    "head",
    "legs",
    "feet",
    "hands",
    "arms",
    "shield",
    "about-body",
    "waist",
    "wrist",
    "wield",
    "hold",
    "enter",
    "ear",
    "back",
    "chest",
    "ankle",
    NULL
};


const char *unit_open_flags[] = {
    "open-close",
    "closed",
    "locked",
    "pickproof",
    "inside-open-close-lock-unlock",
    "hidden",
    "climb",
    NULL
};


/* UNIT_FLAGS */
const char *unit_flags[] = {
    "private",
    "invisible",
    "can-be-buried",
    "is-buried",
    "no-teleport",
    "sacred",
    "no-mobile",
    "no-weather",
    "indoors",
    "invisible-mortals",
    "transparant",
    "is-edited",
    "no-save",
    "magic",
    NULL
};


/* OBJ_FLAGS */
const char *obj_flags[] = {
    "unequip",
    "two-hands",
    "nocover",
    "nodual",
    NULL
};


/* ROOM_FLAGS */
const char *room_flags[] = {
    "save",
    "unused",
    "no-save",

    NULL
};


/* CHAR_FLAGS but ought to be CHAR_FLAGS */
const char *char_flags[] = {
    "law-protected",
    "legal-target",
    "outlaw",
    "group",
    "blind",
    "hide",
    "mute",
    "sneak",
    "detect-alignments",
    "detect-invisible",
    "detect-magic",
    "detect-poison",
    "detect-undead",
    "detect-curse",
    "detect-life",
    "wimpy",
    "not-used",
    "self-defence",
    "peaceful",
    NULL
};


/* PC_FLAGS */
const char *pc_flags[] = {
    "brief",
    "PK relaxed",
    "information",
    "compact",
    "nowiz",
    "spirit",
    "expert",
    "prompt",
    "GA Telnet",
    "echo-say",
    "ansi-terminal",
    "dmserver-terminal",
    "noshouting",
    "notelling",
    "noshout",
    "notell",
    NULL
};


/* NPC FLAGS */
const char *npc_flags[] = {
    "ILLEGAL-BIT",
    "sentinel",
    "scavenger",
    "nice-thief",
    "agressive",
    "stay-zone",
    NULL
};


/* ---------------------------------------------------------------------- */
/*                             O T H E R (stat)                           */
/* ---------------------------------------------------------------------- */

const char *unit_status[] = {
    "NPC",
    "PC",
    "ROOM",
    "OBJECT",
    NULL
};


const char *char_sex[] = {
    "neutral",
    "male",
    "female",
    NULL
};


const char *obj_types[] = {
    "ILLEGAL OBJECT TYPE",
    "light",
    "scroll",
    "wand",
    "staff",
    "weapon",
    "fire-weapon",
    "missile",
    "treasure",
    "armor",
    "potion",
    "worn",
    "other",
    "trash",
    "trap",
    "container",
    "note",
    "liquid-container",
    "key",
    "food",
    "money",
    "pen",
    "boat",
    "spell-page",
    "spell-book",
    "shield",
    "fur",
    "board",
    NULL
};


const char *equip_pos[] = {
    "In inventory/Not equipped",
    "ILLEGAL EQUIP POSITION - REPORT!",
    "Worn on right finger",
    "Worn on left finger",
    "First worn around Neck",
    "Second worn around Neck",
    "Worn on body",
    "Worn on head",
    "Worn on legs",
    "Worn on feet",
    "Worn on hands",
    "Worn on arms",
    "Worn as shield",
    "Worn about body",
    "Worn around waist",
    "Worn around right wrist",
    "Worn around left wrist",
    "Wielded",
    "Held",
    NULL
};

const char *obj_pos[] = {
    "inventory",
    "ILLEGAL EQUIP POSITION - REPORT!",
    "rfinger",
    "lfinger",
    "1neck",
    "2neck",
    "body",
    "head",
    "legs",
    "feet",
    "hands",
    "arms",
    "shield",
    "body",
    "waist",
    "rwrist",
    "lwrist",
    "wielded",
    "held",
    "chest",
    "back",
    "ear1",
    "ear2",
    "ankle1",
    "ankle2",
    NULL
};

const char *connected_types[] = {
    "Playing",
    "Get Name",
    "Confirm Name",
    "Read Pwd",
    "New Pwd",
    "New Pwd Chk",
    "Message Of Today",
    "Menu Select",
    "Extra Description",
    "Redefine Pwd",
    "Redefine Pwd Chk",
    "Waiting to cut connection.",
    "Thrown off",
    "New select Sex",
    "New select Race",
    "New select Guild",
    "Confirm player deletion",
    NULL
};
