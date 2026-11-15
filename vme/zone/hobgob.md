# hobgob: review questions

## Step 1

1. **weeve** (line 4581): Weeve runs "Weeve's Weapons" and sells `short_sword` and `long_sword`,
   but the item types his shopkeeper DIL trades in are `ITEM_ARMOR`+`ITEM_WORN` (the same line as
   Able the armorer), so he buys armour and won't buy weapons. Should it be `ITEM_WEAPON`?
2. **cloak_skin1, dragon_scale1, gem, tri_plate** (lines 5151, 5135, 6171, 6386): the text promises
   bonuses the object doesn't give. The cloak's `$identify` says it lets you move about undetected
   and gives extra strength and dex, but it has no transfers. The scale's extra says holding it makes
   you more dexterous; no DEX transfer. The gem's `$identify` says "Makes you a bit stronger"; no STR
   transfer. The tri breastplate's `$improved identify` says "Turn undead bonus 6", but its
   transfer is `BONUS_AVERAGE_PLUS` (+1). Add the bonuses, or change the text?
3. **mines and maze** (from line 2063): several mine exits are one-way: mine_6 west to mine_11,
   mine_8 west to mine_13, mine_9 west to mine_2, mine_10 east to mine_6, mine_12 east to mine_8,
   mine_13 east to mine_9. None of them has an exit back. In the maze above the town (from line 671),
   mtrail_4 north to mtrail_6 and mtrail_7 east to mtrail_1 are one-way, and mtrail_6 south leads back
   to mtrail_6. The maze is described as confusing, so that is probably on purpose; are the mines
   meant to be a maze too?
4. **cant** (line 1854): the room says the tables would seat about 150 people; the tables extra
   says the 5 tables could seat about 50. Which number is meant?
5. **DIL, for step 2** (lines 5488, 5684, 3577): `wings_2` makes the wearer say "lost him" when it
   loses track of the player (`exec("say lost him", pc)`). `wing_scroll` makes the player emote
   "You are not yet meant to have this knowledge" instead of telling them. In `mageq_2` (the
   cheater branch) "You feel less experienced." is an act() with no character, so nobody sees it.
