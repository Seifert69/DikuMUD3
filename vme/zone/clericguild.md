# clericguild: review questions

## Step 1

1. **garden2** (line 322): says the monastery's golden doors are to the east and that a path leads
   north and south, but garden2's exits are south (garden1) and west (garden3); the golden doors are
   in the foyer, reached from garden1. **garden3** (line 340): says the path continues "west and
   south", but its exits are east and west. Rewrite the directions to match the exits? Fixed (text follows the world): garden2 now says the path leads south and west (golden doors dropped); garden3 says east and west.
2. **heal_torc** (line 1114): `$identify` promises more Divine Contact, Divine sphere skill and
   strength ("strength of faith is reflected in your strength of arm"); the torc only transfers
   first aid +2. Text or stats?
3. **tuck** (line 558): titled "Father Tuck" but his description calls him "Brother Tuck" twice.
   Which one? Fixed (text follows the world): "Brother Tuck" changed to "Father Tuck" in his description.
4. **morenae** (lines 915-918, 924): during the torc ritual three `sendtext(..., self.outside)`
   messages go to the room unit, which most likely nobody sees. DIL, for step 2: should they be
   `act(..., TO_ALL)`? Fixed in step 2.

## Step 2

Fixed in place:
- torc_give (Morenae, line 915): the three ritual messages went by `sendtext` to the room unit
  and reached nobody; they are now act() to everyone in the room (step 1 question 4).
- torc_give: the wait takes any give by a player in the room, but only `if (target == self)` led
  back to `start`; a give between two other players ran off the end of the code, which restarts the
  program at the top (only `heartbeat` again). The `goto start` added there has no effect in play;
  kept for clarity.
- torc_repl: when an immortal looked at Morenae, the code ran off the end of the program in the
  same way (the whole body is inside `if (pc.level < IMMORTAL_LEVEL)`) and restarted at the top
  (`heartbeat` and `on_activation` again, which change nothing). Added `unsecure(pc); goto start;`
  there (the goto has no effect in play; kept for clarity), and `unsecure(pc)` on the two other ways
  back to `start`.
- healer_give: a non-member who worshipped Father Tuck got "Please, not here" and then fell into
  the lecture for players who lost their torc ("You were given a powerful protection ... go see
  Sister Morenae"); added `unsecure(pc); goto start;`.
