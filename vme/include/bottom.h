
#ifndef _MUD_BOTTOM_H
#define _MUD_BOTTOM_H




#define SETHEIGHT(MLVL,minh,maxh) \
height (((((maxh-minh)+1)*((MLVL*100)/199))/100)+minh)

#define SETWEIGHT(MLVL,minw,maxw) \
weight (((((maxw-minw)+1)*((MLVL*100)/199))/100)+minw)
                                           

/* Abilities: Str, Dex, Con, Hpp, Bra, Cha, Mag, Div */
/* Weapons: Axe_ham, Sword, Club_mace, Polearm, Unarmed, Special */
/* Spells: Div, Pro, Det, Sum, Cre, Min, Hea, Col, Cel, Int, Ext */
/* base */


#endif
