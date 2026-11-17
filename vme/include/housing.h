/* Player houses: what the house library (housing.zon) and the house lanes (uhouses.zon) share. */
#ifndef _MUD_HOUSING_H
#define _MUD_HOUSING_H

#define HOUSE_HP       18000            /* a house's condition when new: its front room's hit points */

#define HOUSE_WHERE    "$house where"   /* on a front room: where its door is, for the architect */
#define HOUSE_BACK     "$house back"    /* on a back room, open only in a two-room house */
#define HOUSE_OWNER    "$house owner"
#define HOUSE_TYPE     "$house type"
#define HOUSE_FRIEND   "$house friends"  /* who has a key: the owner invited them ("$Name") */
#define HOUSE_LOCK     "$house lock"     /* the door lock, a paid add-on: "locked" or "open" */
#define HOUSE_CHESTLOCK "$house chestlock" /* on a chest with a lock: its owner, who alone opens it */
#define HOUSE_FURNISH  "$house furnished"
#define HOUSE_FILE     "$house file"
#define HOUSE_HOLDS    "$house holds"   /* on a chest: how many things it holds */

#endif
