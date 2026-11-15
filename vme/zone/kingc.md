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
   anywhere else). Was the chest meant to be locked with `small_key`?
4. **butler_quarters** (line 564): the description has "a large bed and a drawer", but there is only
   an extra for the bed; no drawer object or extra. Add a "drawer" extra (or a drawer container)?
5. **by_treasury** (line 720): the room text says "a great iron door"; the exit description and
   keyword say "large steel door". Which metal?
6. **royal_herald** (line 1995): `exec("emote gives you the chalice.", self)` is seen by everyone in
   the throne room as "The Royal Herald gives you the chalice." Should it be an act() to the player
   ("$1n gives you $2n.") and one to the room ("$1n gives $2n to $3n.")? (DIL change, so not made.)
