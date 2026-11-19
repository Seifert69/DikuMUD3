/* Player houses: what the house library (housing.zon) and the house lanes (uhouses.zon) share. */
#ifndef _MUD_HOUSING_H
#define _MUD_HOUSING_H

/* A house's front door, on both sides: a real door, shut and pick proof, which house_door
   (housing.zon) locks while the house is for sale or its owner has locked it */
#define HOUSE_DOOR_FLAGS open {EX_OPEN_CLOSE, EX_CLOSED, EX_PICKPROOF}
/* A Midgaard house's front door, from the street (mhouses.zon, and Penny Lane in midgaard.zon) */
#define MHOUSE_DOOR HOUSE_DOOR_FLAGS keyword {"oak door", "door"} \
   descr "A polished oak door leads into a house."

#define HOUSE_HP       18000            /* a house's condition when new: its front room's hit points */

#define HOUSE_WHERE    "$house where"   /* on a front room: where its door is, for the architect */
#define HOUSE_BACK     "$house back"    /* on a back room, open in a house and a townhouse */
#define HOUSE_UPPER    "$house upper"   /* on an upper floor, open only in a townhouse */
#define HOUSE_STREET   "$house street"  /* on a front room in a town of several streets: the street's word */
#define HOUSE_OWNER    "$house owner"
#define HOUSE_TYPE     "$house type"
#define HOUSE_FRIEND   "$house friends"  /* who has a key: the owner invited them ("$Name") */
#define HOUSE_LOCK     "$house lock"     /* the door lock, a paid add-on: "locked" or "open" */
#define HOUSE_CHESTLOCK "$house chestlock" /* on a chest with a lock: its owner, who alone opens it */
#define HOUSE_FURNISH  "$house furnished"
#define HOUSE_FILE     "$house file"
#define HOUSE_HOLDS    "$house holds"   /* on a chest: how many things it holds */
#define HOUSE_AT       "$house at"      /* on furniture dragged out of the front room: the room it stands in (plot5b) */

/* The building materials a player has handed in, counters on the player: what the architect
   takes off the price (felled logs to John the Lumberjack, iron ore to Sam the blacksmith) */
#define HOUSE_PAID     "$house paid"     /* on a player: what their house and lock cost them */
#define HOUSE_LOGS     "$house logs"
#define HOUSE_ORE      "$house ore"
#define ORE_VEIN       "$ore vein"       /* on a room: iron ore can be mined here */

#endif
