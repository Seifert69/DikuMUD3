# udgaard: review questions

## Step 1

1. **handbooks** (chapter 3 of all ten handbooks, first at line 5584): "From the village square simply
   go east three times to get to the Cypress forest, or west three times to get to the Haon-Dor
   forest." On the map it is five steps east (square, village_rd3, village_rd4, village_rd5,
   watch_tower, then cypress/hill) and four west (village_rd2, village_rd1, watch_tower2, then
   haon_dor/forest_edge). Change the counts to five and four, or describe it some other way?
2. **map** (line 10047): the map that Harold sells is an old layout: guilds in Udgaard (Cleric's,
   Warrior's, Thieves', Mage's), rows E and F with gates, the temple at A07 and Haon-Dor at C01. The
   map in the zone help (line 53) is newer and matches the village. Replace the object's map with the
   help map?
3. **moot hall** (rooms moot_ent to moot_prac, lines 3715-5268): the rooms are compiled, but nothing
   leads to them. village_rd9's north exit is commented out (EXTENDEDMUD), and the moot NPCs, items
   and doors only reset under `#ifdef EXTENDEDMUD`. Other zones still use the rooms: competition.zon
   loads comp_board into moot_sou, hallows23 (and the private hallows10/holiday08) load jack-o'-lanterns
   into moot_ent. The library books (moot_bbook, line 9131) also give directions that no longer fit
   (guilds "from the obelisk", which now stands in Midgaard's market square). Reconnect the hall, move
   it to its own zone as the header comment suggests, or drop it along with those loads?
4. **eliza** resets (lines 14315-14331): Bonzino is a level-10 shopkeeper, and each of the three copies
   (haon_dor/rn_6014, jail@udgaard, jail@midgaard) wields sirius, a level-40 great sword (+2 STR,
   +1 DEX, rent 214). The copy in jail@udgaard also has wander_zones but can't leave the locked jail;
   world-run.log: "udgaard/Deseri got stuck in a room with no exits jail@udgaard". Is the sword
   intended, and should she load in the Udgaard jail at all?
5. **sirius** (line 7462): `$identify` and `$improved identify` say Disarm +10%, but the transfer is
   `SKILL_TRANSFER(SKI_DISARM, BONUS_GOOD_PLUS)`, which is +5. Which one is right?
6. **crown** (line 7536): `SPELL_TRANSFER(SPL_CURE_WOUNDS_1, BONUS_GOOD)` appears four times next to a
   single `SPL_CAUSE_WOUNDS_1`. The identify text says +5 to harm and heal. Copy and paste, or were the
   other three meant for different spells?
7. **mage_robe** (line 7286): the Robe of Ixtlan is `ARMOUR_PLATE` with `MANIPULATE_WEAR_ARMS`, but the
   (EXTENDEDMUD) reset puts it on WEAR_BODY. Should it be a body item, and cloth rather than plate?
8. **moot_debe** (line 9072): Dellinger's belt has `MANIPULATE_WEAR_FEET`, and the reset equips it on
   WEAR_WAIST. Should it be `MANIPULATE_WEAR_WAIST`? Fixed: `MANIPULATE_WEAR_WAIST`.
9. **moot_beco** (line 8727): Besri's coat is "a brown coat" (title, descr and material "A heavy brown
   wool"), but its description says "A black overtunic". Brown or black? Fixed (text follows the world): Besri's coat description now says brown overtunic.

## Step 2

Fixed in place:
- `fix` (line 240): when `load()` of the item's symbol fails (the unit no longer exists), the
  fixer went on, destroyed the player's item and gave nothing back. Now he returns the old item
  ("I'm afraid I can't fix this") and logs it.
- `statue_load` (line 1265): no null check after `load()`; a missing object gave run-time errors.
  It now logs "statue_load@udgaard: no such object ..." and returns (see question 1).
- `clone_attr` (line 1340): the NPC branch copied weapon skills up to `SPL_GROUP_MAX` (12) instead
  of `WPN_GROUP_MAX` (7), past the end of the weapon groups. (The template is not used anywhere.)
- `cigar_dil` (line 1923): the burnt-out stump was `addequip`ped to WEAR_HOLD while the cigar still
  held that slot, so it always landed in the inventory; the cigar is now unequipped first.
- `new_storage` (line 5501): `sendtext` without `<br/>` (see question 3).
- `Gold_bucket` (line 9417): the donation total `t` was never reset, so every "lookin" added the
  donations again and the bucket filled up by itself; `t := 0` before the sum.
- `poly_names` (line 11725): Bonzino's new title was the name plus a stray `</div>` (no opening
  tag), which broke every act() that names her.
- `auction` (line 12771): removed two debug `log()` lines ("Fucking length: ...") written to
  vme.log on every `list`.

Questions:

1. **odinstatue** (lines 7825-7872): the statue gives starting equipment by profession, but ten of
   the objects don't exist, so those players get nothing for that slot (before this step the DIL
   also failed at run time; now it logs each one): `udgaard/assassin_item`, `udgaard/ranger_item`,
   `udgaard/sorcerer_book`, `udgaard/sorcerer_item`, `udgaard/cloth` (paladin; `newbie_cloth`
   exists), `udgaard/paladin_item`, `udgaard/necro_item`, `udgaard/druid_book`,
   `midgaard/silver_dagger` (midgaard has `silver_dag`) and `necro_guild/necro_hand` (necro_guild
   has `necro_hand1`). Assassins, rangers, sorcerers, paladins, necromancers and druids have no
   `*_item` (the others have `conjure_item`, `healer_item`, `thief_item`, `fighter_item`,
   `druid_item`, `barbarian_item`), sorcerers and druids no book. Which objects should they get,
   or should the missing ones be created?
2. **statue_load** (line 1270): the third parameter `i` is overwritten by
   `i := dildestroy("randomizeItem@randomt", u)` before it is read, so the 1/0 the callers pass
   does nothing; `blowaway` is added only to items that had randomizeItem. Was `i` meant to choose
   whether the item blows away? (All live calls pass 1.)
3. **new_storage** (line 5501): the message goes to `self`, the storage container, so nobody sees
   it. Should it go to `self.outside` (whoever carries the old container)? Fixed: it goes to `self.outside`.
4. **crook1** (Jens, line 11488) and **crook2** (Jesper, line 11548): neither program loops. Jens
   greets the first player who types a command, pickpockets them three times and then the program
   ends (the five pauses at the end and the `nopriority` after `:start:` suggest a `goto start;`
   was meant); Jesper says his lines once after each load. Add `goto start;` at the end of both?
   For Jens that means he pickpockets every player who comes by, not only the first.
5. **pipe_dil / cigar_dil** (lines 1765, 1899): the burn timer keeps running when the pipe or cigar
   is put out by `remove` (pipe_dil2/cigar_dil2 set it out, but pipe_dil/cigar_dil still wait their
   two ticks), so "Your pipe goes out" comes later anyway and a cigar that was removed still burns
   out and turns into a stump. `extinguish` and `light` restart the timer, so a cigar lasts forever
   if relit. Should the timers stop when the item is put out? Returned to Papi: how long a pipe or cigar burns (and whether a relit cigar ever burns out) is a timing players feel and a design choice.
6. **auction** (line 12496): after the player nods, the item is looked up again with no null check;
   if they dropped it meanwhile, Roland still says he tagged it (nothing is lost). Low priority:
   add `if (item == null) goto lost;`? Fixed: Roland says "You don't have it anymore, <name>." and stops.

No precedence cases (`and`/`or` next to a comparison) in udgaard's DIL.
