# newbie: review questions

## Step 1

1. **newbie_part1, newbie_sign** (lines 924, 1021, 3246): the guide's tour invites players to Discord
   with `https://discord.gg/vmha2rGRA2`, the sign in the Udgaard temple with
   `https://discord.gg/cN7B3ZnY9y`. Which invite is current? Both should probably be the same.
2. **newbie_part5c/5f/5m/5t, newbie_brain** (lines 1691, 1889, 2083, 2282, 2756, 2792; for step 2):
   `act("Suddenly "+pc.name+" appears before your eyes.", A_ALWAYS, self.outside, null, null,
   TO_REST)` passes the room as `$1`; the newbie_brain teleports at lines 2673-2892 use
   `$2n` with the same `self.outside` and look fine. Do the onlookers in the guild room ever see
   the message from the first form? If not, use the `$2n` form there too. Fixed in step 2: neither form reached anyone (see below).
3. **newbie_temp** (lines 1549-1560; for step 2): when the player types something else instead of
   `info` or `practice`, the guide says "Please just try the info command." and then jumps to
   `try_again`, which is the earlier "Please just STAND up" loop, so the player is told to stand
   until they type `stand`, and the info/practice steps are skipped. Fixed in step 2.
4. **newbie_part3** (lines 1184-1185): the guide loads one loaf of bread but runs
   `give bread to <pc>` twice; the second give fails (or gives away other bread the guide carries).
   Load two loaves, or drop the second give?
5. **ferret** (line 3154): vmc warns "Too much money on ferret. (a copper piece vs 5 iron
   pieces)". Keep the copper piece (a reward for the tour's first kill), or remove it?
6. **builder_plate** (line 3601): its names are `{"builder plate", "breastplate", "blue", "glowing",
   "guide", "plate"}` and its room description is the same as guide_bplate3 ("A glowing blue
   breastplate"). The "guide" name looks copied from the guide plates; drop it? A different colour
   to tell the two apart?
7. **hall** (line 2953 onwards): the room's exits to the four offices all use the door keyword
   "door", so `open door` in the hall only ever finds the first one. Give each door its own keyword
   (dragon, griffon, owlbear, raven)? Returned to Papi: it changes the exits' door keywords, and which names to give them is a choice.

## Step 2

Fixed in place:
- newbie_part5c, 5f, 5m, 5t, newbie_help, newbie_brain (lines 1693, 1891, 2085, 2284, 2675,
  2758, 2794, 2833, 2859, 2894): step 1 question 2. "Suddenly ... appears before your eyes"
  had the room (`self.outside`) as $1, and a room as $1 reaches nobody, in both the `pc.name`
  and the `$2n` form. Now `act("Suddenly $1n appears ...", A_ALWAYS, pc, null, null, TO_REST)`,
  like the "disappears" lines.
- stop_test (line 677): `wait(SFB_DONE, TRUE)` woke on every command of anyone in the room:
  each one repeated "Just 'nod' if you want to continue..." to the player, and another
  player's nod or shake decided for them. Now `activator == pc`.
- newbie_part4 (line 1360): `if (pc.master.name != self.name)` is false when the player follows
  no one (a field of null fails), so a player resuming at part 4 without following was never
  asked to follow and was left behind by the walk. Now `not(... == ...)` as in the other parts.
- newbie_temp (lines 1545-1562): step 1 question 3; the info and practice prompts now loop on
  their own labels instead of `try_again` (the stand loop).

Questions:
1. **newbie_part1** (line 800): `i1 := interrupt(... argument == "go on", hurry)` stands after
   `goto stop` in the unreachable `:no_quest:` block, so it never runs: the guide tells players
   "you can skip this part by saying 'go on'" (line 831) but saying it does nothing, and the
   `:hurry:` short tour (line 1004) is dead. Fix: move the interrupt to just before
   `goto stage1` (line 790), and `clear(i1)` at `:stage6:`. It changes what the tour does, so
   your call. `:no_quest:` itself is dead too; drop it? Returned to Papi: it changes what the tour does, and said after the silver ring is given (the interrupt stays until `:stage6:`) `:hurry:` hands the player a second ring; dropping `:no_quest:` is deleting code.
2. (Withdrawn: the program restarts at the top when it runs off its end.)
3. **newbie_help** (lines 2642-2651): after `walk_room` to the temple the code returns whatever
   `tf` is; the `if (tf == FALSE)` block with the teleport is reached only through
   `goto to_long2` from other zones. A walk that fails leaves the player where they were. Meant:
   `if (tf == FALSE) goto to_long2;` right after the walk? Fixed: added; when the walk fails the guide teleports to the temple and summons the player, as for far zones.
4. **quest_parse** (lines 2454-2457): `wait(SFB_DONE|SFB_TICK, command("say") and ...)` turns
   every tick down, so `if (command(CMD_AUTO_TICK) ...) goto tick;` can never be true. Harmless;
   drop SFB_TICK and the test, or was something meant to happen on a tick?
5. **getem** (line 539): `bring` works for guides and for `activator.level > IMMORTAL_LEVEL`,
   while the n-commands (naffect, nbless...) let gods in from `IMMORTAL_LEVEL` itself (they
   refuse `self.level < IMMORTAL_LEVEL`). `>=` meant? Fixed: `>=`.
6. Step 1 question 4 (the second `give bread`, line 1185) is still open.

