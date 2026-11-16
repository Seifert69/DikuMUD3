# guilds: review questions

## Shared zone review

Fixed in place:
- **Specialize** (line 193): "two or fewer professions at be at least level 25" -> "and be".
- **Transform** (lines 259-260): "Your overwhelmed with agony" -> "You're overwhelmed"; "drinks $2n then"
  -> "drinks $2n, then".
- **TrainMemberChk** (line 576): `(self.position > POSITION_SLEEPING) or (self.position != POSITION_FIGHTING)`
  is always true, so a sleeping teacher still spoke; now `and` (the practice is blocked either way).
- **do_guilds** (line 782): "adminsitrator" -> "administrator".

Questions:
1. **MurderBanish** (lines 482-491), used by some 70 guild masters and teachers: on the master's
   death it walks `u := self.outside; ... u := u.next`, which starts at the room itself and never
   looks at the characters in it, so nobody has ever been banished. Help Guilds says "you can never
   rejoin a guild that has banished you". The fix would be `u := self.outside.inside` (and checking
   that `u.fighting` is still set when SFB_DEAD arrives), but it would start banishing players who
   kill guild masters: do you want that?
2. **Specialize** (line 193) against `ReadySpecialize@professions`: the master says "at least level 25
   in your main guild", but the code checks the character's level (`pc.level >= 25`), and Help
   Professions says "At level 25". Drop "in your main guild" from the line? Fixed (text follows the world): dropped "in your main guild" from the master's line.
3. **AlertMembers** (line 501) is marked NOT YET IMPLEMENTED: it builds a message and sends it
   nowhere. Is anything using it? (Nothing in vme/zone; leave or remove.)
