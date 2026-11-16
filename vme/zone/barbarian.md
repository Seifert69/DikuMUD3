# barbarian: review questions

## Step 1

1. **zone header** (line 34): the `help` text held a commented-out block copied from the conjurer
   guild (Archcancellor, Tower of Mundane...) inside the string, so players could read it. I removed
   it. The help is now just the introduction. Should it list where the trainers are, like the other
   guilds' help?
2. **bloody_gauntlet** (line 1090): `$improved identify` says "+2 Str and +3 Hit", but the item
   gives Str +2, Hit +2 and Brain -1. Which is right?
3. **faeld** (line 922): alignment 1000, while every other barbarian has 0. Intended?
4. **hall_n** (line 493): the description ends in a sentence without a verb ("An assortment of
   objects from across the land, representing the far-ranging escapades ... necessary for living.").
   What was meant? Perhaps "An assortment of objects ... is on display". Fixed (text follows the world): added "is on display" verb to the last sentence.
5. **gate_guard** (line 655): "The barbarian guards appear to exist without physical match." Meant
   "without equal"/"without match"?
6. **nokill** (line 150): checks `pc.type` before `pc` is set, so the NPC check never works. DIL,
   for step 2. Carried into step 2, question 1. Left: already fixed, `pc := self.fighting;` now comes before the NPC check (vmc warnings).

## Step 2

Fixed in place:
- **barb_quest2** (line 847): the item given to Hilka (`gift`) wasn't secured, so after the first
  `pause` it was null: the blood bowl was never destroyed (it piled up in Hilka's inventory), and a
  wrong item was "tossed away onto the table" without its name and kept. Now secured, and
  unsecured before each `destroy`.
- **assignBrawl** (line 1043): after `qstWaitDone` had rewarded a player (with pauses), the program
  went on to `qstWaitHero` with `activator`, by then another event's unit or null. Now it goes back
  to waiting when `qstWaitDone` returned the player, as the other quest givers do.
- **blood** (lines 256-259): the victim's "You cringe as the blood ... enters your body!" was
  `TO_VICT` with the victim as `$2` (`TO_VICT` goes to `$3`), so nobody got it, and the room line was
  `TO_REST`, which the victim got too. Now the victim is `$3`, with `TO_VICT` and `TO_NOTVICT`.

Questions:
1. **nokill** (line 150): `if (pc.type == UNIT_ST_NPC) goto start;` comes before
   `pc := self.fighting;`, and `pc` is null there (unit variables are cleared at every wake), so
   NPCs that fight the guild staff (a player's charmed pet, say) are thrown out at 0 hp like players
   and the staff healed. Swap the two lines, as fightersguild's nokill has it? Not made: the staff
   would then fight such NPCs to the end and could be killed by them. (Smaller: `command("flee")`
   never matches under `SFB_COM`, and `:gone:` / `unsecure(pc)` have no `secure`; harmless.) Fixed (vmc warnings).
2. **brawl_det** (line 123): once the fight is fair, any death the loin cloth hears completes the
   Barbarian entry quest: the player's own death against the wolf as well, or any other creature
   dying in the room. Suggested: on `CMD_AUTO_DEATH`, `if (activator == self.outside) goto init;`
   (the player lost) and `if (activator.race != RACE_WOLF) goto loop;`. Not made: it makes the quest
   harder than it is today.
3. **blood** (line 241): `skilla := self.abilities[ABIL_CON];` reads the abilities of the weapon
   (`self`), which has none, so the attacker's side of `skillresist` is always 0 and the blood
   rarely enters the victim. Meant `self.outside.abilities[ABIL_CON]` (the wielder's Con)? Not made:
   it would make the gauntlets' poison land much more often (balance).
4. **gauntlet_blood** (lines 1188-1217): `grip` spends a charge (the rune number drops, "Blood gushes
   out...") before checking mana and endurance; with less than 5 of either the player loses the
   charge and gets "You have not enough power". Move the two checks before line 1188? Not made:
   it saves players charges. Returned to Papi: it saves players charges (hands them something).
5. **blood_poison** (lines 294-295): mana and endurance go below 0, while the poison spell it was
   copied from (spells.zon, `poison`) stops them at 0. Add the same two `if (... < 0) ... := 0;`?
   (`$blood_str`, set on the weapon by `blood`, is read by nothing either.) Returned to Papi: stopping the drain at 0 changes how hard the poison hits (numbers).

## vmc warnings

- **nokill** (line 150): `if (pc.type == UNIT_ST_NPC) goto start;` read `pc` before
  `pc := self.fighting;`; the two lines swapped, as fightersguild's nokill has them.
  Changes play: an NPC fighting the guild staff (a charmed pet, say) is no longer thrown out at
  0 hp with the staff healed; the staff fight it out normally.
