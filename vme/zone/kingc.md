# kingc: review questions

## Step 1

1. **golden_crown** (line 2804): the improved identify says "The crown gives +2 to brain and +1 to
   divine when worn on the head", but the crown has no transfers at all (only
   `ARMOUR_CHAIN(BONUS_TERRIBLE_MINUS, BONUS_AVERAGE_PLUS)`). Add BRA/DIV transfers, or change the
   text?
2. **banner** (line 2731): the identify says the banner "will cause the person bearing it to become
   stronger", but it is a plain `ITEM_TREASURE` with no transfer. Add a STR transfer, or reword?
3. **chest** / **small_key** (lines 2902, 2878): the chest's extra says "It is equipped with a small
   lock, that any self-respecting thief should be able to pick", but the chest has no lock (no
   open/lock flags, no key). The cook carries `small_key`, which opens nothing in the zone (nor
   anywhere else). Was the chest meant to be locked with `small_key`? Fixed (text follows the world): chest extra now says it is a plain chest with no lock; small_key untouched.
4. **butler_quarters** (line 564): the description has "a large bed and a drawer", but there is only
   an extra for the bed; no drawer object or extra. Add a "drawer" extra (or a drawer container)? Fixed (text follows the world): dropped "and a drawer" from the description.
5. **by_treasury** (line 720): the room text says "a great iron door"; the exit description and
   keyword say "large steel door". Which metal? Fixed (text follows the world): room text now says a great steel door.
6. **royal_herald** (line 1995): `exec("emote gives you the chalice.", self)` is seen by everyone in
   the throne room as "The Royal Herald gives you the chalice." Should it be an act() to the player
   ("$1n gives you $2n.") and one to the room ("$1n gives $2n to $3n.")? (DIL change, so not made.) Fixed in step 2.

## Step 2

Fixed in place:
- herald (line 1995): `exec("emote gives you the chalice.")` was seen by the whole throne room;
  now an act() to the player ("$3n gives you $2n.") and one to the others (step 1 question 6).
- move2room (the whirlpool in ww1): after "When you wake up, you find yourself in...." it ran
  `look` before moving the player, so the player saw the room they were leaving; the look now
  comes after the move.
- jerry, michael: every time a player came by they loaded another pair of dice, so the dice piled
  up in their inventory; they now load a pair only when they carry none.

Questions:
1. **welmar** (line 1559): an item that isn't the chalice with the water of life (or an empty
   chalice) is kept by the King with a shake of the head; the player loses it. Give it back? Fixed: after the shake he hands it back (money given stays with him, as before).
2. **herald** (line 1967): a player who doesn't bow gets `set(pc.charflags, CHAR_OUTLAW |
   CHAR_PROTECTED)`: outlaw and protected at once. Was `CHAR_PROTECTED` meant to be removed
   (`unset`) rather than set?
3. **herald** (line 1976): a player below level 10 who bows gets no answer at all (`goto loop`).
   Should the herald say why (e.g. "Come back when thou art more experienced")? Fixed: he says "Come back when thou art more experienced, <name>."
4. **foodfetch** (line 2590): when Ergan is dead, Jack keeps the gruel the chef gave him and gets a
   new one on every trip, so plates of gruel pile up in his inventory. Drop or destroy it in
   `leave_cell`? Fixed: destroyed at `:leave_cell:` (dropping it would leave plates in the cell for players).
