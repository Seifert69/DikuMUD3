# hallows23: review questions

## Step 1

1. **tot_bag2 / tot_bag4 / stuff_name** (lines 169, 472, 655): the bags and the item list load units
   that don't exist in the zone: `lbat@hallows23` (tot_bag2 `load_7` and `stuff_name`) and
   `ceye@hallows23` (tot_bag4 `load_4`). `load()` then gives nothing, so one roll in eight of those
   bags hands the player nothing (and the bag is destroyed). The zone has `licbat` ("a licorice bat")
   and `cball` ("an eyeball" of white chocolate), which no bag loads. Rename the loads to `licbat`
   and `cball`? (DIL change, so not made.) Still open, see step 2, question 1.
2. **cball** (line 1050): `type ITEM_POTION` but it uses `SCROLL_DEF(60,SPL_BLESS,...)`, and its
   identify says "when you quaff it". Should it be `POTION_DEF`? Fixed: `POTION_DEF`, so the eyeball is quaffed (the macro's later `type ITEM_SCROLL` made it a scroll); same spell and power.
3. **dq_shirt** (line 908): the runes use `<div class='cp'>`, a class the client doesn't define, so
   they show in the default colour. A colour meant there (e.g. `cpm`, purple)?

## Step 2

Fixed in place:
- **jack_fight** (lines 323-326): "gets all tangled up" and "gets totally tangled" were both
  `TO_ALL`, so everyone saw two messages for one event; now `TO_VICT` and `TO_NOTVICT`, as in
  every other branch.
- **hall23** (line 2201): when an immortal said "trick or treat", the program skipped the
  mortal-only block and ran off the end of its code, which restarts the program at the top
  (`heartbeat` and `on_activation` again, which change nothing). It now unsecures the immortal and
  goes back to `start` (the goto has no effect in play; kept for clarity). The "already done" and
  "working on it" answers (lines 2116, 2125) also unsecure the player before waiting again, as the
  other branches do.
- **hall23_give** (line 2556): a give between two other people in the room (the `target == self`
  test failing) ran off the end of the program, which restarts it at the top (only `heartbeat`
  again). The `goto start` added there has no effect in play; kept for clarity.
- **hall23_give** (line 2249): the heart was destroyed after a `pause`, when `item` had been
  cleared, so the hearts stayed in the Baron's inventory. Destroyed before the pause now.
- **hall23_give** (line 2522): the "all 13 pieces?" check read `activator.quests`, which after the
  heart's `pause` belongs to another event, so giving the heart as the last piece didn't finish
  the quest (the next piece given did). Now `pc.quests`.

Questions:
1. Step 1, question 1 (`lbat`, `ceye`) stays open. Note that none of this runs today: `load_gift`
   is commented out on the study and `load_stuff` is dilcopied nowhere, so the three
   trick-or-treat bags (`prg3`, `prg4`, `prg5`) and the stuff list never reach the game. Revive the
   bags, or remove the loaders and the bags?
2. **prg3 / prg4 / prg5** (lines 411, 520, 612): after "You must pick me up before opening me"
   the player stays secured; when he later leaves, the bag (still on the floor) jumps to `:lost:`
   and destroys itself with "A possible rare item disappears forever!". Meant, or should it
   `unsecure(pc)` before `goto start`? (Dormant, see question 1.) Fixed: `unsecure(pc)` before `goto start` in all three.
3. **jack_fight** (line 327): `self.abilities[ABIL_DEX] := (self.abilities[ABIL_STR])-rnd(1,2);`
   sets Jack's Dex from his Str. Meant `ABIL_DEX` on the right as well (each tangle lowering Dex,
   which nothing restores)? Not changed: balance.
4. **jack_fight** (lines 267-341): the special attacks go on every 7 seconds for as long as the first
   victim stays in the room, whether or not Jack still fights him (he only stops when the victim
   leaves), and keep hitting that victim when Jack fights someone else. Add
   `if (self.fighting != vict) goto damnleft;` after the `pause`? Not made: it changes the fight.
5. **poistrap** (line 772): the gas fires before the `open` runs, so it also strikes a player whose
   open fails (the chest is locked with `dq_key`) or who opens another chest in the room. Meant to
   go off only when this chest really opens (`SFB_DONE`, `self == target`)?
6. **hall23_give** (line 2537): `dilcopy("quest_restrict@function(" + HALL23_COM + ",0,5,)", item)`
   ends in an empty argument, which `dil_copy` doesn't count (strtok), so the template gets 3
   arguments for 4 and isn't copied: the candy wrapper belt has no quest restriction (only
   `ply_restrict`, which binds it to the finder). DIL can't pass an empty string there, and `" "`
   would make `quest_restrict` call a function named " ". Remove the line, or give it an action?
