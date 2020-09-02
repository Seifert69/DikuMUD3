/*
 $Author: tperry $
 $RCSfile: combat.cpp,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */

#include "structs.h"
#include "utils.h"
#include "fight.h"
#include "comm.h"
#include "textutil.h"
#include "interpreter.h"
#include "utility.h"
#include "fight.h"
#include "unitfind.h"

// The Global Combat List...

class cCombatList CombatList;

cCombatList::cCombatList()
{
   nMaxTop = 10;

   CREATE(pElems, class cCombat *, nMaxTop);

   nTop = 0;
   nIdx = -1;
}

cCombatList::~cCombatList()
{
   FREE(pElems);

   nMaxTop = 0;
   nTop = 0;
}

void cCombatList::add(class cCombat *Combat)
{
   if (nTop >= nMaxTop)
   {
      nMaxTop += 10;
      RECREATE(pElems, class cCombat *, nMaxTop);
   }

   pElems[nTop++] = Combat;
}

void cCombatList::sub(class cCombat *pc)
{
   for (int i = 0; i < nTop; i++)
      if (pElems[i] == pc)
      {
         if (nTop - i > 1)
            memmove(&pElems[i], &pElems[i + 1],
                    sizeof(class cCombat *) * (nTop - i - 1));

         pElems[nTop - 1] = NULL;

         nTop--;

         if (nIdx != -1) /* Uh oh, we are running through the list... */
         {
            if (i < nIdx)
               nIdx--;
         }
      }
}

static int combat_compare(const void *v1, const void *v2)
{
   class cCombat *e1 = *((class cCombat **)v1);
   class cCombat *e2 = *((class cCombat **)v2);

   if (e1->When() > e2->When())
      return +1;
   else if (e1->When() < e2->When())
      return -1;
   else
      return 0;
}

void cCombatList::Sort(void)
{
   if (nTop > 0)
      qsort(pElems, nTop, sizeof(class cCombat *), combat_compare);
}

void cCombatList::PerformViolence(void)
{
   int bAnyaction = FALSE;

   if (nTop < 1)
      return;

   Sort();

   // Happens just ONCE per turn, give everybody 12 actions...
   for (nIdx = 0; nIdx < nTop; nIdx++)
      if (pElems[nIdx]->nWhen > 0)
         pElems[nIdx]->nWhen = MAX(0, pElems[nIdx]->nWhen - SPEED_DEFAULT);

   do
   {
      for (nIdx = 0; nIdx < nTop; nIdx++)
      {
         bAnyaction = FALSE;

         if (pElems[nIdx]->nWhen >= SPEED_DEFAULT)
            break; // The rest are larger...

         class cCombat *tmp = pElems[nIdx];

         if (pElems[nIdx]->cmd[0]) // Execute a combat command...
         {
            char *c = str_dup(pElems[nIdx]->cmd);

            pElems[nIdx]->cmd[0] = 0;
            command_interpreter(pElems[nIdx]->pOwner, c);
            bAnyaction = TRUE;

            FREE(c);
         }
         else
         {
            if (CHAR_FIGHTING(pElems[nIdx]->pOwner))
            {
               if (char_dual_wield(pElems[nIdx]->pOwner))
               {
                  bAnyaction = TRUE;
                  melee_violence(pElems[nIdx]->pOwner,
                                 tmp->nWhen <= (SPEED_DEFAULT + 1) / 2);
                  if ((nIdx != -1) && (nIdx < nTop) && (tmp == pElems[nIdx]))
                     tmp->nWhen +=
                         MAX(2, (1 + CHAR_SPEED(tmp->pOwner)) / 2);
               }
               else
               {
                  bAnyaction = TRUE;
                  melee_violence(pElems[nIdx]->pOwner, TRUE);
                  if ((nIdx != -1) && (nIdx < nTop) && (tmp == pElems[nIdx]))
                     tmp->nWhen += MAX(4, CHAR_SPEED(tmp->pOwner));
               }
            }
         }
      }
      Sort();
   } while (bAnyaction && nTop > 0 && pElems[0]->nWhen < SPEED_DEFAULT);

   nIdx = -1;
}

void cCombatList::status(const class unit_data *ch)
{
   char buf[MAX_STRING_LENGTH];

   sprintf(buf, "The Global Combat List contains [%d] entries.<br/>", nTop);
   send_to_char(buf, ch);
}

/* ======================================================================= */
/*                                                                         */
/*                         The Combat Element                              */
/*                                                                         */
/* ======================================================================= */

cCombat::cCombat(class unit_data *owner, int bMelee)
{
   assert(owner);

   pOwner = owner;
   pMelee = NULL;

   nWhen = SPEED_DEFAULT;
   cmd[0] = 0;

   pOpponents = NULL;
   nNoOpponents = 0;

   CombatList.add(this);
}

cCombat::~cCombat(void)
{
   while (nNoOpponents > 0)
      subOpponent(pOpponents[nNoOpponents - 1]); // Faster sub at tail

   if (pOpponents)
   {
      FREE(pOpponents);
      pOpponents = NULL;
   }

   CHAR_COMBAT(pOwner) = NULL;

   CombatList.sub(this);

   CHAR_POS(pOwner) = POSITION_STANDING;
   update_pos(pOwner);

   pOwner = NULL;
}

void cCombat::setCommand(const char *arg)
{
   strncpy(cmd, arg, MAX_INPUT_LENGTH);
   cmd[MAX_INPUT_LENGTH] = 0;
}

void cCombat::changeSpeed(int delta)
{
   nWhen += delta;
}

int cCombat::findOpponentIdx(class unit_data *target)
{
   for (int i = 0; i < nNoOpponents; i++)
      if (pOpponents[i] == target)
         return i;

   return -1;
}

class unit_data *cCombat::FindOpponent(class unit_data *victim)
{
   int i = findOpponentIdx(victim);

   if (i == -1)
      return NULL;
   else
      return pOpponents[i];
}

void cCombat::add(class unit_data *victim)
{
   assert(victim);

   nNoOpponents++;

   if (nNoOpponents == 1)
   {
      CREATE(pOpponents, class unit_data *, 1);
   }
   else
   {
      RECREATE(pOpponents, class unit_data *, nNoOpponents);
   }
   pOpponents[nNoOpponents - 1] = victim;
}

void cCombat::sub(int idx)
{
   int bWas = FALSE;

   if (idx == -1)
      return;

   assert(nNoOpponents > 0);
   assert(idx >= 0 && idx < nNoOpponents);

   // Never mind about realloc, it will be free'd soon anyhow... how long
   // can a combat take anyway?

   if (pOpponents[idx] == pMelee)
   {
      pMelee = NULL;
      bWas = TRUE;
   }

   if (nNoOpponents - idx > 1)
      memmove(&pOpponents[idx], &pOpponents[idx + 1],
              sizeof(class unit_data *) * (nNoOpponents - idx - 1));

   pOpponents[nNoOpponents - 1] = NULL;
   nNoOpponents--;

   if (nNoOpponents < 1)
   {
      FREE(pOpponents);
      pOpponents = NULL;
      delete this; // We are done...
   }
   else if (bWas)
   {
      setMelee(Opponent(number(0, nNoOpponents - 1)));
   }
}

void cCombat::setMelee(class unit_data *victim)
{
   pMelee = victim;
}

// Add another opponent. A very important feature is, that opponents
// always exists as pairs and if one is removed so is the other.
//
void cCombat::addOpponent(class unit_data *victim, int bMelee = FALSE)
{
   // This if is needed since we call recursively for the victim

   if (!FindOpponent(victim))
   {
      add(victim);

      if (!CHAR_COMBAT(victim))
         CHAR_COMBAT(victim) = new cCombat(victim, bMelee);

      CHAR_COMBAT(victim)->add(pOwner);
   }

   if (bMelee && pMelee == NULL)
      setMelee(victim);
}

void cCombat::subOpponent(class unit_data *victim)
{
   if (nNoOpponents < 1)
      return;

   int i = findOpponentIdx(victim);

   if (i == -1) // Not found
      return;

   CHAR_COMBAT(victim)->sub(CHAR_COMBAT(victim)->findOpponentIdx(pOwner));
   sub(i);
}

class unit_data *cCombat::Opponent(int i)
{
   if (i >= nNoOpponents)
      return NULL;
   else
      return pOpponents[i];
}


void cCombat::status(const class unit_data *god)
{
   char buf[MAX_STRING_LENGTH];
   int i;
   string str;

   sprintf(buf,
           "Combat Status of '%s':<br/>"
           "Combat Speed [%d]  Turn [%d]<br/>"
           "Melee Opponent '%s'<br/>"
           "Total of %d Opponents:<br/><br/>",
           STR(UNIT_NAME(pOwner)),
           CHAR_SPEED(pOwner), nWhen,
           CHAR_FIGHTING(pOwner) ? STR(UNIT_NAME(CHAR_FIGHTING(pOwner))) : "NONE", nNoOpponents);

   str.append(buf);

   for (i = 0; i < nNoOpponents; i++)
   {
      sprintf(buf, "   %s<br/>", STR(UNIT_NAME(pOpponents[i])));
      str.append(buf);
   }

   send_to_char(buf, god);
}

/* ======================================================================= */
/*                                                                         */
/*                         Utility Routines                                */
/*                                                                         */
/* ======================================================================= */

/* start one char fighting another (yes, it is horrible, I know... )  */
void set_fighting(class unit_data *ch, class unit_data *vict, int bMelee)
{
   if (ch == vict)
      return;

   /* No check for awake! If you die, FIGHTING() is set to point to murderer */

   if (ch->is_destructed() || vict->is_destructed())
      return;

   if (CHAR_COMBAT(ch) == NULL)
      CHAR_COMBAT(ch) = new cCombat(ch, bMelee);

   CHAR_COMBAT(ch)->addOpponent(vict, bMelee);

   CHAR_POS(ch) = POSITION_FIGHTING;
}

/* start one char fighting another (yes, it is horrible, I know... )  */
void add_fighting(class unit_data *ch, class unit_data *vict, int bMelee)
{
   if (ch == vict)
      return;

   /* No check for awake! If you die, FIGHTING() is set to point to murderer */

   if (ch->is_destructed() || vict->is_destructed())
      return;

   if (CHAR_COMBAT(ch) == NULL)
      CHAR_COMBAT(ch) = new cCombat(ch, bMelee);

   CHAR_COMBAT(ch)->addOpponent(vict, bMelee);
}

/* remove a char from the list of fighting chars */
void stop_fighting(class unit_data *ch, class unit_data *victim)
{
   if (victim == NULL) // Stop all combat...
   {
      while (CHAR_COMBAT(ch))
         CHAR_COMBAT(ch)->subOpponent(CHAR_COMBAT(ch)->Opponent());
   }
   else
      CHAR_COMBAT(ch)->subOpponent(victim);

   if (CHAR_COMBAT(ch) == NULL)
   {
      REMOVE_BIT(CHAR_FLAGS(ch), CHAR_SELF_DEFENCE);
      CHAR_POS(ch) = POSITION_STANDING;
      update_pos(ch);
   }
}

/* ======================================================================= */
/*                                                                         */
/*                         Status Routines                                 */
/*                                                                         */
/* ======================================================================= */

void stat_combat(class unit_data *god, class unit_data *u, const char *pStr)
{   
   class unit_data *u2;

   if (!IS_CHAR(u))
   {
      act("$2n is not a pc / npc.", A_ALWAYS, god, u, cActParameter(), TO_CHAR);
      return;
   }

   u2 = NULL;

   if (!str_is_empty(pStr))
   {
      u2 = find_unit(god, (char **) &pStr, 0, FIND_UNIT_SURRO);
   }

   if (u2 == NULL)
      u2 = god;

   if (CHAR_FIGHTING(u))
      u2 = CHAR_FIGHTING(u);

   if (!IS_CHAR(u2))
   {
      act("$2n is not a pc / npc.", A_ALWAYS, god, u2, cActParameter(), TO_CHAR);
      return;
   }

   CombatList.status(god);

   if (!CHAR_COMBAT(u))
      act("No combat structure on '$2n'", A_ALWAYS, god, u, cActParameter(), TO_CHAR);
   else
      CHAR_COMBAT(u)->status(god);


   string str;
   melee_bonus(u, u2, WEAR_BODY, NULL, NULL, NULL, NULL, TRUE, &str);
   send_to_char(str.c_str(), god);
   melee_bonus(u2, u, WEAR_BODY, NULL, NULL, NULL, NULL, TRUE, &str);
   send_to_char(str.c_str(), god);
}
