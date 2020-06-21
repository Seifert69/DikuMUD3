/* *********************************************************************** *
 * File   : spelldef.h                                Part of Valhalla MUD *
 * Version: 1.00                                                           *
 * Author : Unknown.                                                       *
 *                                                                         *
 * Purpose: Unknown.                                                       *
 * Bugs   : Unknown.                                                       *
 * Status : Published.                                                     *
 *                                                                         *
 * Copyright (C) 1994 - 1996 by Valhalla (This work is published).         *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This work is copyrighted. No part of this work may be copied,           *
 * reproduced, translated or reduced to any medium without the prior       *
 * written consent of Valhalla.                                            *
 * *********************************************************************** */
#ifndef _MUD_SPELLDEF_H
#define _MUD_SPELLDEF_H

/* Different kind of weapon types */
#define WPNT_NONE     -1
#define WPNT_SLASH    0
#define WPNT_BLUDGEON 1
#define WPNT_PIERCE   2


#define SPLCST_CHECK  1
#define SPLCST_RESIST 2
#define SPLCST_IGNORE 3

/* Use the find_unit bits, and furthermore these below are needed */
#define TAR_IGNORE      (FIND_UNIT_WORLD<<1)
#define TAR_CHAR        (FIND_UNIT_WORLD<<2)
#define TAR_OBJ         (FIND_UNIT_WORLD<<3)
#define TAR_ROOM        (FIND_UNIT_WORLD<<4)
#define TAR_FIGHT_SELF  (FIND_UNIT_WORLD<<5)
#define TAR_FIGHT_VICT  (FIND_UNIT_WORLD<<6)
#define TAR_SELF_ONLY   (FIND_UNIT_WORLD<<7)
#define TAR_SELF_NONO   (FIND_UNIT_WORLD<<8)
#define TAR_AUTO_SELF   (FIND_UNIT_WORLD<<9)

/* used both to signify what media is casting, and what media is possible */
#define MEDIA_SPELL     (1<<0)
#define MEDIA_POTION    (1<<1)
#define MEDIA_WAND      (1<<2)
#define MEDIA_STAFF     (1<<3)
#define MEDIA_SCROLL    (1<<4)
#define MEDIA_SELF_NONO (1<<5)  /* Used for staffs to exclude self */


#endif
