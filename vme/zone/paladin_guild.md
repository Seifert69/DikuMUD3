# paladin_guild: review questions

## Step 1

1. **pal_cross** (line 1509): the `$identify` text says the amulet raises Constitution and the
   `$improved identify` says "+1 Con, Brain 6 and Divine 6 required", but the amulet only has
   `PROTECTION_FROM_EVIL` and the guild restriction. Should the text change, or should the item get
   those stats and restrictions? (knight's `night_cross` copies the same Constitution text.) Fixed (text follows the world): removed the Constitution promise from $identify; $improved identify says no bonus, guild members only; same Constitution sentence removed from knight night_cross.
2. **zone header** (line 57): the `notes` give entry rules (level 39, the Wight Warren quest,
   alignment +700) that the code no longer uses (it checks alignment 350, protected status in
   Midgaard, and Joan's Frozen Shade quest), plus two dead e-mail addresses. Should the notes be updated? Fixed (text follows the world): notes rewritten to the real entry rules (alignment 350, protected in Midgaard, not an outlaw, Frozen Shade quest, not fallen); dead e-mail addresses removed.
3. **reignier** (lines 789 and 924): `paladin_quest1` picks the item it asks for by `pc.vlevel`
   (< 15, < 25, < 35, < 51), but `paladin_quest2` accepts the item by `pc.level`. A player whose
   level and vlevel differ may be asked for one item and have another accepted. This is DIL, so it
   could wait for step 2. Carried into step 2, question 2.

## Step 2

Fixed in place:
- BlockEvilTrain: a trainee below -350 alignment got "You are a disgrace to our order!!" and then,
  falling into the next `if`, "You have strayed from the path..." as well; added `goto start`.
- block_evil_join (Reignier): "You are not pure enough to join our order" fell through to the
  protected-citizen test, so a player could get two refusals at once; added `goto start`.
- paladin_quest2: `gift` was destroyed at the end, after pauses that clear it, so the weapon a
  paladin handed in stayed in Reignier's inventory; it is now destroyed when it is accepted.
- flamechat (Flame Tongue): ten TO_VICT messages had `null` as the victim, so the enemy never saw
  them; now `enemy`.
- flamespell: anyone in the room saying "Faith!" set off the wielder's Flame Tongue (and used up
  its charges), with "You raise your fist to the heavens" sent to the wielder; it now reacts only
  to the wielder.
- restrict_guild: "$1n's $2N burns $1m and $1s drops it" read "...and his drops it"; now `$1e`.
  (Nothing uses restrict_guild.)

Questions:
1. **paladin_quest1** (line 876): the banishment branch (paladin with alignment below -900) can't
   be reached: the branch before it (`alignment < 349`) catches those paladins first and only
   tells them they have strayed. Put the -900 test first? That banishes players, so it is your call.
   Also, a paladin with exactly 349 matches neither test and is told "Beggar, be off!".
2. **paladin_quest1 / paladin_quest2** (step 1 question 3): quest1 picks the item to ask for by
   `pc.vlevel`, quest2 accepts it by `pc.level`. Use `vlevel` in quest2 as well?
3. **paladin_quest2** (line 957): any item given to Reignier that isn't the wanted weapon (or is
   given by a non-paladin) is destroyed without a word. Hand it back with a remark instead?
4. **antievil** (line 107): `exec("emote casts a spell that drains your life energy.")` is seen by
   everyone in the room as "... drains your life energy", and the player's hp is set to 0 without
   `position_update`. Should it be an act() to the player and another to the room, followed by
   `position_update(pc)`? Fixed: two act()s, "drains your life energy" to the player and "drains <name>'s life energy" to the room. `position_update(pc)` not added and returned to Papi: with 0 hp it would leave the player stunned on the street, which makes it harder.

## vmc warnings

- **flamespell** (lines 1478, 1480): the two acts passed `enemy`, never set, as `$3`, which their
  texts don't use; now `null`, and the unused `enemy` variable removed. No change in play.
