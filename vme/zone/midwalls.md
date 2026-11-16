# midwalls: review questions

## Step 1

1. **rn_027** (line 832): the room text says the upper chamber of the western gatehouse "is clean
   and free of all grime", then "The floor here is quite dirty, and some dust clouds fill the air",
   and the floor/dust extra says it "has been left uncleaned for ages". One of the two halves has to
   go; which one? Fixed (text follows the world): dropped "clean and free of all grime" (floor, dust and extra agree it is dirty).
2. **tower_3** (line 2538): "a beautiful view is unidirectional" doesn't make sense (the text then
   describes views in every direction). Was "omnidirectional" or "in every direction" meant?
3. **sign** (lines 4327, 4884): the sign loaded into rn_015 says the research lab entrance "has been
   moved south and east of the southern gate of Midgaard" because a wooden door out of the tower
   would be silly. Its room description is "A sign from god." Is this notice still needed, or can
   the sign (and its reset) go? Left: removing the sign needs its %reset line removed, which is not a text change; sign text left.
4. **ARMOR / ARMOR2 macros** (lines 4741, 4752): every guard gets `equip bracers position
   WEAR_HANDS`, the same position as the gauntlets; bracers are made for `MANIPULATE_WEAR_ARMS`.
   Should it be `WEAR_ARMS`? (A reset change, so not fixed.) Fixed: `WEAR_ARMS` in both macros; the guards wear their bracers on their arms.
5. **gate_e2** (Fenris, line 3014): his patrol list is `{"rn_014@midwalls"}` (the ground floor of
   the southern gate tower), while the other eastern gate guards (gate_e1, gate_e3, gate_e4) stand
   at `rn_012@midgaard`, the gate passage itself. Slip for `rn_012@midgaard`?
6. **rocks, DIL rock** (lines 4424-4485, for step 2): the room messages use
   `act("$1n picks up a rock.", A_SOMEONE, self.outside, ...)` and `"$1n hurls a rock ..."` with
   `self.outside` (the room) as `$1`, so the other players see the room's name instead of the
   thrower. Probably `pc` was meant. The `wait()` condition also mixes `and`/`or` so the PC check
   only applies to "get rocks". First half fixed in step 2; the second is step 2's question 1.
7. **food** (line 4504): the cook's "hunk o beef" has no `descr`, so it is invisible when dropped
   in a room. Add one (e.g. "A hunk of beef lies here.")? Fixed: `descr "A hunk of beef lies here."`.
8. **gate_e3** (Sven, line 3859): "You get the idea that he has been to visit a fanny recently." In
   British English this reads as crude slang; is it intended, or was another word meant?
9. **guard1 to guard24**: their `names` include "lieutenant", but their descriptions call them
   ordinary wall guards ("one of the wall guards"); only lieu1 to lieu8 are lieutenants. Drop
   "lieutenant" from the guards' names, or keep it so that "lieutenant" finds any of them?

## Step 2

Fixed in place:
- **rock** (lines 4452-4479): the five room messages had the room (`self.outside`) as `$1`; now the
  player (step 1, question 6, first half).
- **guardschedule** (line 82): the patrol emote read "Speaks with the Guards."; now "speaks with the
  guards.".

1. **rock** (line 4436): `get` and `"rock"` `or` (`get` and `"rocks"` and a PC): `"rock"` already
   matches "rocks", so the PC test never matters and an NPC's `get rock` would be caught too (none
   does today). Meant `command("get") and ("rock" in argument) and (activator.type == UNIT_ST_PC)`?
   (Step 1, question 6, second half.) Fixed: the wait is now as suggested.
2. **rock** (line 4432): `on_activation` tests `self.position`, but `self` is the pile of rocks, which
   has no position, so it never skips. Was it meant to ignore sleeping or fighting players
   (`activator.position`)? Returned to Papi: either the line goes (deleting code) or it starts refusing fighting players; which was meant is a choice.
3. **guardstep** (line 37): `if (not transfermoney(u, null, 1 * COPPER_MULT))` takes a copper from
   the guard whenever he has one, and gives him two only when he has none. So a guard with money loses
   a copper at every meal besides paying for it. Meant as "if he has no money, give him some"? A fix
   that keeps his purse: `if (transfermoney(u, null, 1 * COPPER_MULT)) j := transfermoney(null, u, 1 *
   COPPER_MULT); else j := transfermoney(null, u, 2 * COPPER_MULT);`.

### Precedence

Since #603 (merged 2026-11-15) `and`/`or` bind as in C (looser than the comparisons), so once the
zones are rebuilt these conditions do what was written. What that changes in play is on Papi's
review list (#596's table).

None found.
