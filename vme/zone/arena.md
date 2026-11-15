# arena: review questions

## Step 1

1. **zone header `help`** (line 73) against `BLUE_LEVEL`/`GREEN_LEVEL`/`YELLOW_LEVEL` (line 12):
   the help says Blue 1-12, Green 13-24, Yellow 25-39, Red 40-255, but `arena_enter` uses
   `level < 12` for blue, `< 24` for green and `< 39` for yellow, so a level 12 player starts in
   green, 24 in yellow and 39 in red. Which is right: the help text or the defines?
2. **arena_7 / arena_28 / arena_34 / arena_55 doors** (and arena_13 line 1450, arena_29 line 1825,
   arena_33 line 1923, arena_49 line 2304): each coloured arena has a door into the Ring of Honor,
   but the Ring of Honor rooms have no exit back, so the doors are one-way. The door signs say "You
   are now leaving the blue arena", which suggests this is deliberate (you can't go back into a
   level-restricted area). Confirm it is intended. I fixed the wall extras in arena_28/29/33/34/55
   that put the door in the wrong wall (they all said "south", copied from arena_7).
3. **entrance / infirm signs** (lines 1183, 2631): "Welcome to the New Arena ... send mail to
   whistler@blinksoft.com or palio@blinksoft.com". These addresses are surely dead. Remove the mail
   line, or point players at a god / the arena board instead?
4. **ravencla1**: its title is "Raven Claw" (no article, capitalised) while its twin phoenixcla1 is
   "a bloody phoenix claw"; both have the same descr "A bloody bird's claw is lying here." Left as a
   proper name because the DIL says "your $2n" / "The $2n". Rename to e.g. "the raven claw"?
5. **Ring Of Honor titles**: 30 rooms are titled "Ring Of Honor" and arena_33 "Ring of Honor".
   Left as they are; make them all "Ring of Honor"?
6. **shitthrow (magicdus1)** (line 446): when no target is found it prints "I see no " +
   enemy.name, with enemy null. For step 2.
