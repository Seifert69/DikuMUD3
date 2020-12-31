/*
 $Author: tperry $
 $RCSfile: combat.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */

#ifndef _MUD_COMBAT_H
#define _MUD_COMBAT_H

#ifdef __cplusplus

class cCombatList
{
public:
    cCombatList ();
    ~cCombatList ();
    void PerformViolence ();
    void add (class cCombat * pc);
    void sub (class cCombat * pc);
    void status (const class unit_data * ch);

private:
    void Sort ();

    class cCombat **pElems;
    int nMaxTop;			// No of allocated elements
    int nTop;			// Current Max
    int nIdx;			// Updated when in Perform() and doing Sub()
};

class cCombat
{
    friend class cCombatList;

public:
    cCombat (class unit_data * owner, int bMelee = FALSE);
    ~cCombat ();

    class unit_data *Opponent (int i = 0);
    class unit_data *FindOpponent (class unit_data * tmp);

    inline class unit_data *Owner (void)
    {
        return pOwner;
    }
    inline class unit_data *Melee (void)
    {
        return pMelee;
    }
    inline int When ()
    {
        return nWhen;
    }
    inline int NoOpponents (void)
    {
        return nNoOpponents;
    }

    void changeSpeed (int delta);
    void setMelee (class unit_data * victim);
    void setCommand (const char *arg);

    void addOpponent (class unit_data * victim, int bMelee);
    void subOpponent (class unit_data * victim);
    void status (const class unit_data * ch);

private:
    void add (class unit_data * victim);
    void sub (int idx);
    int findOpponentIdx (class unit_data * tmp);

    int nWhen;			// What tick to attack / command at
    class unit_data *pOwner;	// The owning unit
    class unit_data *pMelee;	// The melee or kill pointer
    class unit_data **pOpponents;	// Array of opponents (given damage)
    int nNoOpponents;		// Number of opponents
    char cmd[MAX_INPUT_LENGTH + 1];	// A combat command
};

extern class cCombatList CombatList;

void set_fighting (class unit_data * ch,
                   class unit_data * vict, int bMelee = FALSE);
void add_fighting (class unit_data * ch,
                   class unit_data * vict, int bMelee = FALSE);
void stop_fighting (class unit_data * ch, class unit_data * victim = NULL);

void stat_combat(class unit_data * ch, class unit_data * u, const char *pStr);
void stat_spell(class unit_data *god, class unit_data *u, const char *pStr);

#endif

#endif
