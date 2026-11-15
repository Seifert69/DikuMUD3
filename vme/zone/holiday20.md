# holiday20: review questions

## Step 1

1. **norma** (line 3314): Mrs. Claus has `sex SEX_MALE`, so the game calls her "he"/"him". She is also
   `height 30`, against 107 for Howard the elf and 180 for Santa. Should she be `SEX_FEMALE`, and
   is the height meant (a tiny lady) or a slip (e.g. 130 or 160)? holiday21 has the same unit.
2. **choco_heart** (line 1477): its names are `{"a chocolate heart", ..., "heart", "staff"}`.
   "staff" looks copied from another unit (nothing about the heart is a staff), and the name with
   the article "a" is unusual. I added "chocolate heart" and "chocolate"; should "staff" (and "a
   chocolate heart") be removed? Same in holiday21.

## Step 2

Fixed in place:
- touch_pole: the log line lacked a space ("Bobhas not played ...").
- touch_pole: after moving the player to the North Pole it paused twice before `exec("look", pc)`;
  the player had left the pole's room, so the secure jumped to `lostpc` and the look never ran. It
  now looks right after the move.
- touch_pole, look_santa: three `goto start` (not enough experience yet, "Don't be greedy", "My
  assistant will take care of you") left the player secured; a later player's talk could be cut off
  when the earlier one walked away. Added `unsecure(pc)`.
- christmas_2020 (Howard): an answer that only starts with a choice ("ring please", "STR now") is
  accepted by `in`, but the whole sentence went into `load(spot+"_"+stat+"@holiday20")`, which loaded
  nothing: no gift, yet the quest "Holiday Gift 2020 Received" was set. The spot and stat are now
  taken from the list entry that matched.
- christmas_2020: same as touch_pole, the `pause` after sending the player to Udgaard's square
  made the secure fire and the `look` never ran.
- prg3, prg4, prg5 (the gift boxes): "You must pick me up before opening me" left the player
  secured; added `unsecure(pc)`.
- prg4, prg5: the Christmas card's text was sent to `activator` after two pauses (a tick has no
  activator), so the player never saw what the card says; now sent to `pc`.
- clean_graf (the janitor): `u` (the graffiti remover) was cleared by the pauses, so
  `destroy(u)` did nothing and the can stayed in his inventory. It is looked up again before
  being destroyed.
- clean_graf: the code had no `goto start` at the end, so the program ended after the first
  graffiti he cleaned; `goto start;` added, as in holiday22 (the janitor's description says he
  keeps the streets clean).

Questions:
1. **clean_graf**: fixed (see above), as in holiday22.
2. **xmas_magic / x_charge** (Mrs. Claus's upgrade): `addextra` puts a new extra first, so once
   `$XMAS_Charges` exists, `self.extra.[0]` is that extra and not the item's description. When the
   charge is used, the "magical twinkle" sentence Mrs. Claus added to the description is never
   removed (the replace works on "empty"), and x_charge's re-append lands in `$XMAS_Charges` and is
   overwritten by "charged". The charges themselves work. Keep the sentence in the description and
   find that extra by its names instead of `.[0]`, or drop the twinkle text?
3. **rocket**: `block` comes before the check that the rocket the player named is this one, so
   with two rockets in a room "light rocket" can be swallowed by the wrong rocket's program and
   nothing happens. Move `block` after the `item != self` check?
