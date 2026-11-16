# assassin: review questions

## Step 1

1. **back_room** (line 192): the description is a placeholder ("Well, I suppose you want
   something to actually be described here, huh?") and the room has no exits. Remove it, or write it?
2. **captain_dagger** (line 538): nothing loads the dagger and nothing sets the quest flag
   "Assassin Quest Ongoing" that it needs (searched public and private zones). Dead content: remove
   it, or is the Amir quest planned?
3. **dark_katana** / **dark_wakiz** (lines 727, 749): `$improved identify` says Dex +2, katana or
   wakizashi and hide +3%, craftsmanship 5 and magic 5, but both are
   `WEAPON_DEF(WPN_SCIMITAR, BONUS_AVERAGE, BONUS_AVERAGE)` with no transfers. Text or stats?
4. **master_trainer** (line 270): the line "The Master Assassin pulls some items ... gives them to
   you." is sent `TO_CHAR` with the master as `$1`, so the player never sees it (DIL, step 2).
   Fixed in step 2.
5. **death_bringer** (line 670): the `death` DIL checks `IsInGuild@guilds(self, ...)` where `self`
   is the sword, so the life-drain message never fires (DIL, step 2). Fixed in step 2.

## Step 2

Fixed in place:
- **guilditem** (line 270): the "pulls some items ... gives them to you" line was sent `TO_CHAR` to the
  master; now to the player (step 1, question 4).
- **prg3** (line 507): `secure(pc, lostpc)` came before `pc := activator`, so it secured nothing; the
  two lines are swapped.
- **death** (line 670): `IsInGuild@guilds(self, ...)` tested the sword; now the wielder,
  `self.outside` (step 1, question 5). When one of the two outer tests failed the program ran off its
  end, which restarts it at the top (only `heartbeat` again); the `goto start` added there has no
  effect in play; kept for clarity. The room message wrote `self.outside.fighting.name`, so an NPC
  victim showed by its first name ("from goblin"); now `$1n`/`$3n`.

1. **death** (lines 666-697): the program only prints the glow messages. `storenum` (a part of the
   victim's max hp) is computed and never used, so no health is drained or given, though the
   `$identify` says it "will drain the health of your opponent and transfer it to you". Add the drain
   (a balance question), or change the identify text?
2. **prg2 / prg3** (shadow assassins, lines 453-472 and 506-523): they wake at every command a guild
   member types near them, send the offer ("Just nod if you wish to go.") each time, and then test
   `command("nod")` on that same command. So the offer repeats at every command, and a member who
   nods is moved before being asked. Should the offer come once (when the member arrives) and then
   wait for that member's `nod`? Fixed: the offer comes once per member while he stays in the room (at his first command there); a member's nod moves him as before, without the offer repeated.
3. **guilditem** (line 271): every `worship` by a member loads a new Death Bringer and Black Lotus
   Talisman, with nothing to record that they were given. Intended, or a flag as in the thieves' guild?

### Precedence

Since #603 (merged 2026-11-15) `and`/`or` bind as in C (looser than the comparisons), so once the
zones are rebuilt these conditions do what was written. What that changes in play is on Papi's
review list (#596's table).

None found.
