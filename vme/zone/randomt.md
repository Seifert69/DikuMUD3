# randomt: review questions

## Step 1

1. **Treasure** (line 4009): `sendtext(self.title + " takes shape in your hands.<br/>", ...)`
   starts the sentence with the lower-case title ("a silver dagger takes shape..."). Use act()
   with $2n (capitalised) in step 2? Fixed in step 2.
2. **setWpnShaft** (line 3192): "You wonder if that's really $3n $1a $1n $2n, it is so warped and
   bent." puts the material before the article ("oak a quarterstaff rod"). Meant "$1a $3n $1n $2n"? Fixed in step 2 (as "$3a $3n $1n $2n": "an oak flail handle").
3. **treasure0, treasure4, treasure7** (line 4115): test chests with descriptions like
   "treasure0 is here, open it". Fine as long as only gods see the treasury; confirm they stay as
   they are.

## Step 2

Fixed in place:
- bagofholding (line 1402): the "large belching sound" act() had the bag's outside as $1; when
  the bag lies on the floor that is the room, and a room as $1 reaches nobody. Now from the bag
  itself when it is in a room.
- enc_vorpal (line 1626): identify said "Decapitates enemies i out of 1000", but the vorpal
  chance copied is 2*i out of 1000. Text now says 2*i.
- setWpnShaft (line 3196): step 1 question 2, "$3a $3n $1n $2n".
- Treasure (line 4013): step 1 question 1, act() with $2n, which capitalises the title.

Questions:
1. **vorpalweapon** (lines 1272, 1303, 1332): after the first decapitation `heartbeat` stays at
   10 minutes ("Pause for 10 minutes"), and nothing sets it back to PULSE_SEC. The
   `wait(SFB_TICK, TRUE)` between the zing and the chop then waits up to 10 minutes, so after
   its first kill a vorpal weapon almost never chops again (the fight is over). Also
   `secure(r, loop)` is never unsecured: if the wielder walks out during the 10 minute pause the
   program jumps to `loop` and skips the rest of the pause. Fix: `heartbeat := PULSE_SEC;` after
   the pause's wait, and `unsecure(r);` before `goto petTheHorse`. That makes vorpal weapons
   work far more often than today, so it's your call.
2. **constants** (line 178): `ilStaffMat` (the woods for staves, club bases, shafts and handles,
   and the wooden small/medium shields of createArm) has `MAT_BLACK_STEEL` between black oak and
   dark wood. A metal there makes "a black steel quarterstaff" with iron's weight. A wood meant
   (e.g. MAT_BLACK_OAK twice, or MAT_IPE)? The comment above `ilWhipMat` also says "staves".
3. **enc_resist** (line 1848): the identify text calls SPL_DIVINE resistance "divination"; the
   category is divine magic. "divine magic"? Fixed (text follows the world): "divination" changed to "divine magic".

