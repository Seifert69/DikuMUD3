# boards: review questions

## Shared zone review

Fixed in place:
- **rem_res** (line 74): "other peoples posts" -> "other people's posts".
- **do_reply** (line 129): with no board in hand or in the room, `u` was null and
  `u.objecttype != ITEM_BOARD` was false (a failed value), so reply went on without a board
  (loading "<nothing>.idx"). Now `if (u == null)`, as do_write does.
- **do_reply, board, do_write** (8 lines: 177, 240, 266, 380, 958, 968, 987, 1013): "This board doesn't
  work report to an administrator." -> "doesn't work; report it to an administrator."
- **do_reply** (line 199): "You can't reply to that the board is full." -> "to that; the board is full."
- **do_reply** (line 211): "finished posts please wait" -> "finished posts; please wait".
- **board, rem_post** (line 499): `remove 0` passed the check (`msg_num<0`), said "You remove message 0"
  and rewrote the index; now `msg_num<1`, so remove 0 is left to the ordinary remove command.
- **board, rem_post** (line 551): "You remove message N from $3n" had no full stop.
- **do_read** (line 637): "No competition found with a value of +5" (stray "+") sent to `activator`;
  now "...a value of 5." to `self`, the reader.
- **do_read** (line 649): a competition that hasn't started yet said "Started:" like a past one; now
  "Starts:" (as "Ended:"/"Ends:" below).
- **do_read** (line 696): the gods' "Only these Quests allowed" line printed the excluded quests
  (`names.[3]`) instead of the allowed ones (`names.[4]`).
- **do_read** (line 726): `$BOARD_MAX` was looked up on the reader (`self.extra`) instead of the board,
  so the range check failed as a whole, and `read 99` on a short board said the board was broken.
  Now `u.extra`, and `read 0` is refused the same way.
- **do_read** (line 747): "This board is not working report to an Administrator" -> "not working;
  report it to an administrator."
- **do_write** (lines 868, 884, 1000): full stops on "You begin to jot down a note on $2n",
  "Done editing", "You begin to write a message on the board".
- **do_write** (line 939): "Headline to long truncating." -> "Headline too long; truncating."
- **bull_board** (line 1093): "Its a big ugly Oak Bulletin board." -> "It's a big ugly oak bulletin board."

Questions:
1. **board, rem_post** (line 526): a player may remove a post when
   `activator.name == left(tname, length(activator.name))`, a prefix test, so "Bob" may remove
   Bobby's posts. Compare the whole name: `(activator.name + ")") == left(tname, length(activator.name)+1)`?
   Fixed: as suggested.
2. **board** (lines 360-370): the `boards` list tests `pc.type` (the board_room, always a room)
   where it means the board's surroundings (`self.outside.type`), so a board carried by someone or
   inside an object is listed by that unit's title anyway. Probably harmless; fix to
   `self.outside.type`? Returned to Papi: with that fix a board inside an object or carried by an
   NPC would be listed by its `outside_descr` ("board in A canoe has been left here."), worse than
   the title it gets now; which text to show is a choice.
