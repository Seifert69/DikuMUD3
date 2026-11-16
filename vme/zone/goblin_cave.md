# goblin_cave: review questions

## Step 1

1. **bed_chamber** (line 104): the description ends "The only way from this room is to the east",
   but the room also has `north to guard_room` (no door, not hidden), and guard_room (line 70) says
   its only exit is south. Should the north exit be hidden (a secret guard room), or should the text
   mention the way north? Fixed (text follows the world): bed_chamber now says ways lead east and north.
2. **boo** (line 975): `M_GOBLIN_WARRIOR_SWORD(3, SEX_MALE)`, but the extra says "She is wearing
   normal yellow leather-armour", and the DIL emote says "points a finger to his ear". Is Boo male
   or female? Fixed (text follows the world): Boo changed to SEX_FEMALE (no DIL tests sex); emote "his ear" changed to "her ear".
3. **bee** (line 1131): Bee otherwise speaks plain English (typos there were fixed: "though",
   "The do?", "the lived", "Thats"), but says "graverty". Goblin speech on purpose, or "gravity"?
4. **goblin_leggins** (line 526): "It's fine craftsmanship. Made of goblins and decorated by
   hobgoblins." A grim joke, or "Made by goblins"?

## Step 2

Fixed in place:
- **mguulprg** (line 752): the PC secured for the burn spell was never unsecured before `goto start`
  (burp and ghopla do it), so a player who had left long ago could later restart his routine.
- **cook** (line 1278): the poison test added the bread's poison twice
  (`u1.value[3] + u1.value[3]`); the comment says bread and mushroom, so `u2` for the mushroom.
  No effect today: both have poison 0.
- **cook** (line 1307): after each "give stew to ..." it destroyed the next stew in its inventory,
  meant for a stew the lord didn't take. As the give had worked, that was the next lord's stew: burp and
  ghopla got one, mguul never. Now it gives all three and then throws away what is left.

Questions:
1. **prisonerprg** (lines 1528-1529): after `kill burp` the prisoner waits for burp's death
   (`wait(SFB_DEAD, ...)`), which he only notices in the same room. If burp is already dead (not
   reset yet), or the prisoner never reaches throne2, he waits there for good and never goes back to
   the jail. Suggested fix: after `kill burp`, `if (findunit(self, "burp", FIND_UNIT_SURRO, null) ==
   null) goto back;` with `:back:` before the walk to the jail. Fixed: when no NPC burp is in the room after `kill burp`, the prisoner walks back to the jail (without the "King Burp is DEAD" shout) and starts over.
2. **baaprg** (line 950): `exec("agree Baa", self)`: Baa agrees with himself. A joke, or
   `agree Boo` (the one he is talking to)?
3. **ghoplaprg, mguulprg** (lines 846, 750): the 2008 note says the king's attack on the player was
   removed because "4 at once on a newbie is a bit much", but ghopla still `kill`s and mguul still
   casts burn on the first player in the throne room. Keep, or remove those two as well?
4. **goblin2prg** (line 1628): it kills `argument` two pauses after goblin1's message, when
   `argument` belongs to a later tick and is empty, so goblin2 never attacks. The fix is to keep
   the name right after the wait (`s := argument;`, then `exec("kill "+s, self)`). Not made: goblin2
   joining goblin1 makes the cave entrance harder than it has been. Fix it? Fixed (vmc warnings).

## vmc warnings

- **goblin2prg** (line 1629): `exec("kill "+argument, self)` ran three pauses after goblin1's
  message, when `argument` belonged to a tick and was empty. The name is now kept in `s` right
  after the wait and used for the kill.
  Changes play: goblin2 now joins goblin1 in attacking the player goblin1 picked.
