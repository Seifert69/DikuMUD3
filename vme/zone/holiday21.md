# holiday21: review questions

## Step 1

1. **norma** (line 3297): Mrs. Claus has `sex SEX_MALE` and `height 30` (Howard the elf is 107,
   Santa 180). `SEX_FEMALE`, and is the height meant? Same question as holiday20. Fixed (text follows the world): norma changed to SEX_FEMALE; height 30 left (a stat).
2. **choco_heart** (line 1460): the names end in "staff", which looks copied from another unit;
   remove it (and the name "a chocolate heart" with its article)? Same as holiday20.

## Step 2

Fixed in place:
- touch_pole: the log line lacked a space ("Bobhas not played ...").
- touch_pole: after moving the player to the North Pole it paused twice before `exec("look", pc)`;
  the player had left the pole's room, so the secure jumped to `lostpc` and the look never ran. It
  now looks right after the move.
- touch_pole, look_santa: three `goto start` (not enough experience yet, "Don't be greedy", "My
  assistant will take care of you") left the player secured; a later player's talk could be cut off
  when the earlier one walked away. Added `unsecure(pc)`.
- christmas_2021 (Howard): an answer that only starts with a choice ("ring please", "STR now") is
  accepted by `in`, but the whole sentence went into `load(spot+"_"+stat+"@holiday21")`, which loaded
  nothing: no gift, yet the quest "Holiday Gift 2021 Received" was set. The spot and stat are now
  taken from the list entry that matched.
- christmas_2021: same as touch_pole, the `pause` after sending the player to Udgaard's square
  made the secure fire and the `look` never ran.
- prg3, prg4, prg5 (the gift boxes): "You must pick me up before opening me" left the player
  secured; added `unsecure(pc)`.
- prg4, prg5: the Christmas card's text was sent to `activator` after two pauses (a tick has no
  activator), so the player never saw what the card says; now sent to `pc`.
- clean_graf (the janitor): `u` (the graffiti remover) was cleared by the pauses, so
  `destroy(u)` did nothing and the can stayed in his inventory. It is looked up again before
  being destroyed.
- clean_graf: the code had no `goto start` at the end; running off the end restarts it at the
  top (only `heartbeat` again), so the `goto start;` added, as in holiday22, has no effect in play;
  kept for clarity.

Questions:
1. (Withdrawn: the program restarts at the top when it runs off its end.)
2. **xmas_magic / x_charge** (Mrs. Claus's upgrade): `addextra` puts a new extra first, so once
   `$XMAS_Charges` exists, `self.extra.[0]` is that extra and not the item's description. When the
   charge is used, the "magical twinkle" sentence Mrs. Claus added to the description is never
   removed (the replace works on "empty"), and x_charge's re-append lands in `$XMAS_Charges` and is
   overwritten by "charged". The charges themselves work. Keep the sentence in the description and
   find that extra by its names instead of `.[0]`, or drop the twinkle text? Fixed: Mrs. Claus's upgrade, xmas_magic and x_charge all use the unnamed extra (the description). The upgrade's `item.extra` was itself the `$ply_restrict` extra (put first by ply_restrict), so the twinkle never showed at all; now it shows, goes when the charge is used and comes back with the recharge.
3. **rocket**: `block` comes before the check that the rocket the player named is this one, so
   with two rockets in a room "light rocket" can be swallowed by the wrong rocket's program and
   nothing happens. Move `block` after the `item != self` check? Fixed: `block` and the "can not light that from here" check come after the `item != self` check.
