# holiday22: review questions

## Step 1

1. **zone header** (line 24): the zone's help says "Players should never be in here.", but Santa's
   study and workshop and Jolnir's Yule hall are rooms players are sent to on purpose (the pole,
   the Yule Wheel). Reword the help (e.g. "Santa's rooms; players reach them through the pole in
   Udgaard's square."), or keep it as an admin-only note? Fixed (text follows the world): help now: Santa's rooms; players reach them through the pole in Udgaard's square..
2. **boom_box3, DIL prg3** (line 1803; for step 2): when the opener is lost, the trap gift says
   "A possible 500000 experience disappears forever!", although this box is the one that explodes
   and never gives experience; after `destroy(self)` the code also falls through into `:trap:`.
   Use the "The gift falls apart, and whatever was in it is lost." line of boom_box2/boom_box4,
   and `quit` after the destroy? Fixed in step 2.
3. **santa, DIL santa_shout** (line 583): "Is that slay bells I hear already!" plays on
   "sleigh bells"; keep the pun as it is, or make it "Are those slay bells I hear already?"
4. **choco_heart** (line 2292): its names had "staff" (copied from some other unit), now
   removed, and the text says "a large chocolate reindeer-shaped heart". A heart shaped like a
   reindeer? Perhaps "a large chocolate heart with a reindeer on it" was meant. Fixed (text follows the world): now "a large chocolate heart with a reindeer on it" in descr and extra.

## Step 2

Fixed in place:
- **rocket** (line 196): a note with no extra jumped to `:default:` before `holstr2` was set, so the
  world saw an empty line instead of the burst of sparkles; it now leaves the loop and reaches
  `:default:` the normal way, after the texts are set.
- **touch_pole** (lines 507, 529): the "come back when you have gained some experience" and "you
  need N more experience" branches went back to waiting with the player still secured;
  `unsecure(pc)` added. Line 475: the log line lacked a space ("Bobhas not played").
- **look_santa** (lines 638, 646): the same missing `unsecure(pc)` on the "don't be greedy" and
  "my assistant will take care of you" branches.
- **christmas_2022** (lines 882, 913): `"ankle please" in sl` matches "ankle", but the gift was
  then loaded as `yule_ankle please_...`, which doesn't exist: the player got nothing and was
  marked as having had the gift. The answer is now cut down to the list entry it matched.
- **prg3, prg4, prg5** (lines 1799, 1884, 2011): "open box" while the gift lay on the floor left the
  player secured, so walking away later destroyed the gift ("falls apart") although nobody had
  opened it; `unsecure(pc)` added before going back to waiting.
- **prg3** (line 1810): the lost-opener message said "A possible 500000 experience disappears
  forever!" on the box that only explodes; now the "gift falls apart" line of boom_box2/4 (step
  1, question 2). prg3 and prg5 also `quit` after `destroy(self)` now instead of running into
  `:trap:` (destroy(self) already stopped them, so this changes nothing in play).
- **clean_graf** (lines 5759-5766): the remover can was destroyed through a variable cleared by
  the pauses before it, so every cleaning left a can in the janitor's pack; it is looked up again
  now. The program also ran off its end after each cleaning, which restarts it at the top (only
  `heartbeat` again); the `goto start` added there has no effect in play; kept for clarity.

Questions:
1. **xmas_magic / x_charge** (lines 787 and 827): they take the twinkle off and put it back with
   `self.extra.[0]`, meaning the item's description, but `addextra` puts the `$XMAS_Charges` extra in
   front of the list, so from the first use `extra.[0]` is that extra: the twinkle never leaves the
   description, and x_charge writes it into the charges extra (then overwritten with "charged").
   Only the look of the item is wrong (the charges themselves work). Suggested fix: find the
   unnamed description extra (`ex := self.extra; while ((ex != null) and (length(ex.names) > 0))
   ex := ex.next;`) and change `ex.descr`. Fixed: as suggested, in xmas_magic, x_charge and Mrs. Claus's upgrade too (its `item.extra` was the `$ply_restrict` extra put first by ply_restrict, so the twinkle never showed); now it shows, goes when the charge is used and comes back with the recharge.
2. **prg3, prg4, prg5** (lines 1789, 1875, 2002): the wait takes any "open box/gift/basket" by
   anyone around, blocks it and says "You must pick me up before opening me", so a gift lying on
   the floor stops players opening their own box or basket there, and one "open gift" opens every
   gift the player carries. Check `findunit(activator, argument, FIND_UNIT_INVEN, null) == self`
   (as yule_spoke does) before blocking? Fixed: each program goes on only when `findunit(activator, argument, FIND_UNIT_HERE, null)` is that very gift (inventory, held or on the floor, so the floor case still gets "You must pick me up").

## vmc warnings

- **prg5** (line 2052): the card's text was sent to `activator` three pauses after the box was
  opened, when `activator` belonged to a tick (null), so the opener never saw it; now `pc`.
  Changes play: the opener now sees the greeting card's text when the box opens.
