# comm: review questions

## Shared zone review

Fixed in place:
- **do_ignore** (line 208): "So you don't think to highly of yourself?" -> "too highly".
- **do_ignore** (line 220): "You are alreadying ignoring" -> "already ignoring".
- **do_tell, tell group** (lines 388-389): the group tell stored the teller's name for `respond`
  as `$reply` in the listener's `extra`, while `reply` (and everything else) reads it from
  `quests`; so `respond` after a group tell answered the previous teller instead. Now `quests`.
- **do_tell** (lines 431-442): the sleeping test and the PC_NOTELL test are two ifs now, the
  pcflags test only for players. No change in play: `or` counts the failed pcflags read of an NPC
  as false, so the old combined test already stopped tells to sleeping NPCs.
- **do_tell** (line 447): "$3e can't hear you ." -> no space before the full stop.
- **do_tell** (lines 504-505): the tell went into the listener's tell history with its own
  `<br/>`, and `get_history` adds another, so `tell` alone showed a blank line after each tell
  (but not after responses). The history entry no longer carries the `<br/>`.
- **do_whisper** (line 1340): `not tgt.type & (UNIT_ST_PC|UNIT_ST_NPC)` is `(not tgt.type) & ...`,
  always 0, so you could whisper to an object (and the room saw it). Now `not (tgt.type & ...)`.
- **do_whisper** (line 1368): the echo `You whisper to $3n '$2t<div class='whisper'>'</div>`
  opened a second div that was never closed; now `'$2t'</div>`.

Questions:
1. **whostring** (line 43): the Discord `who` list tags `(Immortal)` only for `level > IMMORTAL_LEVEL`,
   so a level 200 god gets no tag (Creator is `>=`). Make it `>=`? Fixed: `>=`.
2. **do_ask** (lines 903-904) against Help Ask: the help says "Only the character you're asking
   will hear the question", but the room sees `$1n asks $3n '...'` with the whole question.
   Either the help is wrong, or the room should see only that $1n asks $3n something (as with
   whisper). Left: Help Ask already says everyone in the room hears the question and to whisper to keep it private, as the code does.
3. **reply** (respond) (lines 621-649): unlike tell, it doesn't check whether the receiver is
   asleep, has turned tells off (PC_NOTELL), or is in hobgob's nightmare room, and it doesn't
   slur drunk speech. Intended (a respond always gets through) or should it follow tell?
4. **swear_check** (lines 542-551): the bad-word list includes "que", "qui" (common French and
   Spanish words), "homo" and "caesure"; a player shouting "que" loses shout for an hour. Remove
   those from the list?
5. **do_tell** (line 329): `left(arg,5) == "group"` also takes "tell groupie hi" (a player whose name
   starts with "group") as a group tell. Compare the first word instead (`getword`)? Fixed: a group
   tell is now "group" alone or "group " followed by the message.
