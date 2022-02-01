/*
 $Author: tperry $
 $RCSfile: spells.h,v $
 $Date: 2003/01/30 04:08:58 $
 $Revision: 2.2 $
 */
#pragma once

#include "dil.h"
#include "spelldef.h"

int spell_perform(int spell_no,
                  int spell_type,
                  unit_data *caster,
                  unit_data *medium,
                  unit_data *target,
                  const char *argument,
                  char *pEffect = nullptr,
                  int bonus = 0);

ubit1 spell_legal_type(int spl, int type);
ubit1 spell_legal_target(int spl, unit_data *c, unit_data *t);

struct spell_args
{
    unit_data *caster, *medium, *target;
    const char *arg;
    int hm;
    char *pEffect;
};

struct spell_info_type
{
    ubit8 acttype;
    char *tochar;
    char *tovict;
    char *torest;
    char *toself;
    char *toselfroom;

    void (*spell_pointer)(spell_args *sa);
    ubit8 minimum_position; /* Position for caster               */
    sbit16 usesmana;        /* Amount of mana used by a spell    */
    sbit8 beats;            /* Heartbeats until ready for next   */
    ubit16 targets;         /* See below for use with TAR_XXX    */
    ubit8 media;            /* found in spells/potions/...       */
    ubit8 cast_type;        /* Resist, self check, etc?          */
    ubit16 demi_power;      /* Power used by demis (0..5)        */
    ubit8 offensive;        /* Is this an offensive spell?       */
    ubit8 realm;            /* Essence, Channeling, Mentalism?   */
    ubit8 shield;           /* Shield method SHIELD_M_XXX        */

    diltemplate *tmpl; /* Perhaps a DIL template...         */
};

#define SPL_MAG_REQ(spell) (spl_requirement[spell].abilities[ABIL_MAG])

#define SPL_DIV_REQ(spell) (spl_requirement[spell].abilities[ABIL_DIV])

#define SPL_POW_REQ(spell) MIN(SPL_MAG_REQ(spell), SPL_DIV_REQ(spell))
