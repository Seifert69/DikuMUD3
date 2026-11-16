# mail: review questions

## Shared zone review

Fixed in place:
- **mail** (lines 155, 163): the postman's "That persons address has problems contact an
  administrator." and "Sorry $3n that persons mail box is full try and send it later." had no
  closing quote; now "That person's address has a problem; contact an administrator.'" and
  "Sorry $3n, that person's mailbox is full; try sending it later.'".
- **mail** (line 217): "says 'You don't have any mail.  Sorry.'" -> "says, '... mail. Sorry.'" (comma
  as in the other lines, one space).

Questions:
1. **mail** (line 180): the "sudden urge to visit the post office" notice goes to
   `findunit(self, sDest, FIND_UNIT_WORLD, null)`, which matches the start of a name and only
   what the postman can see: mail to "Bob" may notify Bobby, or nobody when Bob is invisible.
   Walk the player list for the exact name instead (as `reply@comm` does)? Fixed: mail walks
   the player list for the exact name.
2. **Help Mail** and **Help Request** are empty in vme/etc/help (write.hlp points to Help Mail).
   Write them?
