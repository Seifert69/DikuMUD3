# paladin_guild: review questions

## Step 1

1. **pal_cross** (line 1509): the `$identify` text says the amulet raises Constitution and the
   `$improved identify` says "+1 Con, Brain 6 and Divine 6 required", but the amulet only has
   `PROTECTION_FROM_EVIL` and the guild restriction. Should the text change, or should the item get
   those stats and restrictions? (knight's `night_cross` copies the same Constitution text.)
2. **zone header** (line 57): the `notes` give entry rules (level 39, the Wight Warren quest,
   alignment +700) that the code no longer uses (it checks alignment 350, protected status in
   Midgaard, and Joan's Frozen Shade quest), plus two dead e-mail addresses. Should the notes be updated?
3. **reignier** (lines 789 and 924): `paladin_quest1` picks the item it asks for by `pc.vlevel`
   (< 15, < 25, < 35, < 51), but `paladin_quest2` accepts the item by `pc.level`. A player whose
   level and vlevel differ may be asked for one item and have another accepted. This is DIL, so it
   could wait for step 2.
