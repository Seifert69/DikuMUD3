# midwalls: review questions

## Step 1

1. **rn_027** (line 832): the room text says the upper chamber of the western gatehouse "is clean
   and free of all grime", then "The floor here is quite dirty, and some dust clouds fill the air",
   and the floor/dust extra says it "has been left uncleaned for ages". One of the two halves has to
   go; which one?
2. **tower_3** (line 2538): "a beautiful view is unidirectional" doesn't make sense (the text then
   describes views in every direction). Was "omnidirectional" or "in every direction" meant?
3. **sign** (lines 4327, 4884): the sign loaded into rn_015 says the research lab entrance "has been
   moved south and east of the southern gate of Midgaard" because a wooden door out of the tower
   would be silly. Its room description is "A sign from god." Is this notice still needed, or can
   the sign (and its reset) go?
4. **ARMOR / ARMOR2 macros** (lines 4741, 4752): every guard gets `equip bracers position
   WEAR_HANDS`, the same position as the gauntlets; bracers are made for `MANIPULATE_WEAR_ARMS`.
   Should it be `WEAR_ARMS`? (A reset change, so not fixed.)
5. **gate_e2** (Fenris, line 3014): his patrol list is `{"rn_014@midwalls"}` (the ground floor of
   the southern gate tower), while the other eastern gate guards (gate_e1, gate_e3, gate_e4) stand
   at `rn_012@midgaard`, the gate passage itself. Slip for `rn_012@midgaard`?
6. **rocks, DIL rock** (lines 4424-4485, for step 2): the room messages use
   `act("$1n picks up a rock.", A_SOMEONE, self.outside, ...)` and `"$1n hurls a rock ..."` with
   `self.outside` (the room) as `$1`, so the other players see the room's name instead of the
   thrower. Probably `pc` was meant. The `wait()` condition also mixes `and`/`or` so the PC check
   only applies to "get rocks".
7. **food** (line 4504): the cook's "hunk o beef" has no `descr`, so it is invisible when dropped
   in a room. Add one (e.g. "A hunk of beef lies here.")?
8. **gate_e3** (Sven, line 3859): "You get the idea that he has been to visit a fanny recently." In
   British English this reads as crude slang; is it intended, or was another word meant?
9. **guard1 to guard24**: their `names` include "lieutenant", but their descriptions call them
   ordinary wall guards ("one of the wall guards"); only lieu1 to lieu8 are lieutenants. Drop
   "lieutenant" from the guards' names, or keep it so that "lieutenant" finds any of them?
