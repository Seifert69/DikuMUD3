/*
 $Author: tperry $
 $RCSfile: combat.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */
#pragma once

#include "essential.h"

class cCombat;
class unit_data;

class cCombatList
{
public:
    cCombatList();
    ~cCombatList();
    void PerformViolence();
    void add(cCombat *pc);
    void sub(cCombat *pc);
    void status(const unit_data *ch);

private:
    void Sort();

    cCombat **pElems;
    int nMaxTop; // No of allocated elements
    int nTop;    // Current Max
    int nIdx;    // Updated when in Perform() and doing Sub()
};

class cCombat
{
    friend cCombatList;

public:
    cCombat(unit_data *owner, int bMelee = FALSE);
    ~cCombat();

    unit_data *Opponent(int i = 0);
    unit_data *FindOpponent(unit_data *tmp);

    inline unit_data *Owner() { return pOwner; }
    inline unit_data *Melee() { return pMelee; }
    inline int When() { return nWhen; }
    inline int NoOpponents() { return nNoOpponents; }

    void changeSpeed(int delta);
    void setMelee(unit_data *victim);
    void setCommand(const char *arg);

    void addOpponent(unit_data *victim, int bMelee);
    void subOpponent(unit_data *victim);
    void status(const unit_data *ch);

private:
    void add(unit_data *victim);
    void sub(int idx);
    int findOpponentIdx(unit_data *tmp);

    int nWhen;                      // What tick to attack / command at
    unit_data *pOwner;              // The owning unit
    unit_data *pMelee;              // The melee or kill pointer
    unit_data **pOpponents;         // Array of opponents (given damage)
    int nNoOpponents;               // Number of opponents
    char cmd[MAX_INPUT_LENGTH + 1]; // A combat command
};

void set_fighting(unit_data *ch, unit_data *vict, int bMelee = FALSE);
void add_fighting(unit_data *ch, unit_data *vict, int bMelee = FALSE);
void stop_fighting(unit_data *ch, unit_data *victim = nullptr);
void stat_combat(unit_data *ch, unit_data *u, const char *pStr);
void stat_spell(unit_data *god, unit_data *u, const char *pStr);

extern cCombatList g_CombatList;
