# basemove: review questions

## Shared zone review

Fixed in place:
- **fall_dil** (line 213): the null test of `room_next` is now its own `if`, before the test that
  reads `room_next.idx`. No change in play: `or` counts a failed side as false, so the old
  combined test already caught a missing exit; this only makes the order plain.
- **do_swim** (line 635): "You cant swim here!" -> "You can't swim here!".
- **open_dil / close_dil** (lines 1792, 1794, 1983, 1985): the "is opened/closed from the
  inside/outside by ..." messages had no full stop. close_dil line 1988: "$1n closed $2n." ->
  "$1n closes $2n." like every other room message.
- **open_dil / close_dil** (lines 1807, 1999): for doors they cut "2.door" down to "door" before
  calling `checkdoor()`, which reads the "2." itself (`checkexit`), so "open 2.door" always opened
  the first door. They now pass the argument whole, as lock_dil and unlock_dil already did.
- **lock_dil / unlock_dil** (lines 2124, 2209, 2325, 2407): "That is impossible" got its full stop;
  "Perhaps you should close it first...." had four dots.
- **direc_dil** (lines 2520, 2522): each exit line opened a `<div class='exit'>` that was never
  closed; now closed after its `<br/>` (telnet output unchanged).
- **drag_dil** (line 2669): it walked with `exec(arg, self)`, the word typed. When the way was
  named by its exit ("drag corpse door") or by a piece of a direction that `checkexit` accepts
  ("or", "th"), that isn't a command, so the drag failed; it now execs the direction `checkexit`
  found (`dirstring@function(dir)`), as enter_dil does.

Questions:
1. **climb_dil** (line 424): the arrival message is `"$1n climbs in on the $2t."` with `arg`, the
   word typed in the old room ("climbs in on the north", "climbs in on the rope"), and nobody in
   the room left behind sees the climber go. Suggest a TO_ROOM "$1n climbs <dir>." before the
   `link()` and "$1n climbs in from <reverse direction>." after it (fall_dil already builds such
   names with `rev_dir@function`). Fixed: the old room sees "$1n climbs <dir>." and the new one
   "$1n climbs in from <the reverse direction / above / below>.", built as fall_dil does.
2. **checkexit, do_swim, do_ride, do_sail, search_dil** (lines 910, 643, 726, 781, 2882): the
   direction is found with `arg1 in "north"`, a substring test: "o", "or", "th", "h" mean north,
   "a" or "st" east, "ou" south, "wn" down. An exit named "out" is taken for south. do_swim, do_ride
   and do_sail then `exec(arg1, self)` (lines 680, 684, 743, 798), the typed word, which for
   "th" or "o" is no direction command at all. Suggest a prefix test
   (`left("north", length(arg1)) == arg1`, as `is_dir@baselook` does) and exec'ing the full
   direction name.
3. **exit_dil / leave_dil** (lines 1461, 1600): `if (can_carry(item, self, 1))` is meant to stop
   you when there is no room outside, but `can_carry`'s first argument must be a character: for a
   boat, cart or container it fails, so the check never fires; and `item` is the unit being left,
   not the one entered. Drop it, or check `item.outside`'s capacity as enter_dil does?
4. **enter_dil / leave_dil** (lines 1278, 1637 and the "in $1s $3N" lines after them): "$1n has
   entered/left the $2N on $1s $3N." go out with `A_ALWAYS`, so they reach sleeping characters and
   show invisible riders by name, while the walking variants use `A_HIDEINV`. Make them
   `A_HIDEINV`? Fixed: the four messages use `A_HIDEINV`.
5. **fall_dil** (line 270): at `splat_now` it builds `rev_name` again but never uses it; when the
   fall starts with no fall exit, `from_dir` is still 0. Dead code, harmless; remove?
6. **secret_wipe** (line 3016): if its zone isn't found the `while` runs `zon` off the list and the
   error log after it never prints (the null comparison fails). Only matters for a broken boot.
   Fixed: the test after the loop is `zon == null`, so the error is logged.
7. **search_dil** (lines 2813-2818) declares `search_item` and `skillresist@function`, and
   secret_wipe `dirstring@spells`, that they never call. Cosmetic.
