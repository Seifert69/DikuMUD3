# knight: review questions

## Step 1

1. **nokill** (line 65): ejects the player to `findroom("sth1@sewers")`, but there is no zone
   `sewers`; room_1's exit goes to `sth1@midsewers`. So `link()` gets null and the player is not
   moved. Change it to `sth1@midsewers` (DIL, step 2)? Fixed in step 2.
2. **room_3** (line 123): "The hall continues in all directions except to south where a large
   tapestry hangs", but there is an exit south to room_5 (the Master's Chambers). Is the door
   behind the tapestry (then the text could say so), or should the text change? Fixed (text follows the world): room_3 now says a large tapestry hangs over the way south to the Master's Chambers.
3. **hilary** (line 396): titled "Maid Hilary", but names and descriptions say "Lady Hilary".
   Which one? (If "Maid", her names need "maid".) Fixed (text follows the world): title changed to "Lady Hilary".
4. **hilary** (line 405): `M_HUMAN_MAGE_FIRE(130, SEX_FEMALE, "cast lava flow")`; the other
   teachers give a plain spell name ("magma", "hellfire"). Does "cast lava flow" work?
5. **avenger** (line 559): `$improved identify` says strength +1; the sword gives Str +2, Div +2 and
   Cha -1. Text or stats?
6. **night_cross** (line 786): the identify says the pendant increases Constitution; it only has
   `PROTECTION_FROM_GOOD` (same as paladin_guild's `pal_cross`).
7. **room_1** (line 93): `movement SECT_FIELD` for an indoor room; room_2 has no movement at all.
   Should both be `SECT_INSIDE`? Fixed: room_1 is `SECT_INSIDE`; room_2 already is (no `movement` means sector 0, `SECT_INSIDE`). The step between room_1 and the sewers costs 4 movement points instead of 5.

## Step 2

Fixed in place:
- **nokill** (line 65): `findroom("sth1@sewers")` named no zone, so the dragged-out fighter was
  never moved; now `sth1@midsewers`, the sewer room_1's exit leads to (step 1, question 1).
- **knight_dil** (lines 293, 302): the "already given" and "work harder on your demeanor" branches
  went back to waiting with the player still secured, so that player walking off later made
  Gawaine jump to `:lost:` and say "Just wait until ..." out of nowhere; `unsecure(pc)` added.
- **flamechat** (lines 614-692, ten acts): the TO_VICT messages had `null` as the victim, so the
  enemy never saw them; now `enemy`. Line 639: `$2N` made `$2n` like the other lines.
- **flamespell** (line 732): anyone in the room saying "Avenge!" set off the wielder's sword (and
  had their say blocked); the wait now also checks `activator == self.outside`, as its messages
  assume. Line 745: `$1N's` made `$1n's`.

Questions:
1. **knight_dil** (line 291): `AVENGER_GIVEN` is tested but never added to the player, so a Dark
   Knight can bow again and again for a new Avenger and pendant each time. Add
   `addextra(pc.quests, {AVENGER_GIVEN}, "")` after the gifts (or a quests.zon quest)? That makes
   the sword one per player, a change for players, so it's your call.
2. **knight_dil** (lines 359-362): `:lost:` runs `destroy(item)`, but `item` is never secured, so
   it is always null there (harmless); a player who leaves mid-speech gets the sword without the
   pendant, and Gawaine says "Just wait until they decide to talk to me and I now ignore them!",
   which reads oddly. Reword (e.g. "Hmph, gone before I finished.")? Fixed: he says "Hmph, gone before I finished."
3. **flamechat** (line 603): the instant kill ("engulfs $3n in darkness", hp set to -7) fires on
   enemies under 10 hp with `alignment < 0`, i.e. on evil ones, while the sword belongs to the Dark
   Knights who "vanquish good". Was `alignment > 0` meant? (Balance: owner's call.)

## vmc warnings

- **flamespell** (lines 743, 745): the two acts passed `enemy`, never set, as `$3`, which their
  texts don't use; now `null`, and the unused `enemy` variable removed. No change in play.
