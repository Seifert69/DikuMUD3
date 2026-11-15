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
   WEAR_WAIST. Should it be `MANIPULATE_WEAR_WAIST`?
9. **moot_beco** (line 8727): Besri's coat is "a brown coat" (title, descr and material "A heavy brown
   wool"), but its description says "A black overtunic". Brown or black?
