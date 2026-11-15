# newbie: review questions

## Step 1

1. **newbie_part1, newbie_sign** (lines 924, 1021, 3246): the guide's tour invites players to Discord
   with `https://discord.gg/vmha2rGRA2`, the sign in the Udgaard temple with
   `https://discord.gg/cN7B3ZnY9y`. Which invite is current? Both should probably be the same.
2. **newbie_part5c/5f/5m/5t, newbie_brain** (lines 1691, 1889, 2083, 2282, 2756, 2792; for step 2):
   `act("Suddenly "+pc.name+" appears before your eyes.", A_ALWAYS, self.outside, null, null,
   TO_REST)` passes the room as `$1`; the newbie_brain teleports at lines 2673-2892 use
   `$2n` with the same `self.outside` and look fine. Do the onlookers in the guild room ever see
   the message from the first form? If not, use the `$2n` form there too.
3. **newbie_temp** (lines 1549-1560; for step 2): when the player types something else instead of
   `info` or `practice`, the guide says "Please just try the info command." and then jumps to
   `try_again`, which is the earlier "Please just STAND up" loop, so the player is told to stand
   until they type `stand`, and the info/practice steps are skipped.
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
   (dragon, griffon, owlbear, raven)?
