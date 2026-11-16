# necro_guild: review questions

## Step 1

1. **dalmar** (line 1173): nothing loads Dalmar the Archnecromancer, and he is the only way into the
   golem quest (get_golem, got_stuff and golem_load all run on him), so `golem` and `golem_manual`
   never appear in the game. Is the golem quest retired, or should Dalmar be loaded somewhere (the
   underground laboratory fits his "preparing golem bodies")?
2. **murdock reset** (line 2389): Murdock is equipped with `black_breast` at WEAR_BODY and then
   `dk_blue_robe` at WEAR_BODY too, but the robe can only be worn about the body (WEAR_ABOUT), as
   meclor's reset does. Should the robe be `position WEAR_ABOUT`? Fixed: `position WEAR_ABOUT`.
3. **kadiya reset** (line 2422): `black_robe` is equipped at WEAR_BODY, but the robe is
   MANIPULATE_WEAR_ABOUT (krain wears the same robe at WEAR_ABOUT). Should it be WEAR_ABOUT? Fixed: `position WEAR_ABOUT`.
4. **dark_corridor** (line 301): the iron door to the west read "Murdoch - Enter at your own risk"
   while the weapon master behind it is Murdock everywhere else; I changed the door to "Murdock".
   Say if "Murdoch" was meant.
5. **golem_brain / golem_load** (lines 1036, 1189): the golem's text still speaks of a familiar here
   and there (I fixed "your familiar is ..." in the status report and the manual), and Dalmar's
   `:hagsecure:` line says "Fool probably could command it anyhow!". Should that be "couldn't"?
   (The golem quest is only live if question 1 brings Dalmar back.) Still open; the golem DIL is
   reviewed in step 2 below (fixes and questions 5 to 7). Fixed (text follows the world): Dalmar now says "Fool probably couldn't command it anyhow!"; no "familiar" text left in the golem DIL strings.

## Step 2

Fixed in place:
- **prg2** (line 484) and **prg3** (line 539): `secure(pc, lostpc)` came before `pc := activator`,
  so it secured nothing; swapped.
- **bonescythe** (lines 1664-1713): the victim's messages ("You scream as faces...", "Thousands of
  hands...", "Your blood begins to boil...", "Behind $1n, the ghastly face of Cthulhu...") were
  `TO_VICT` with the scythe as `$3`, so the victim never saw them; and the "$2n's eyes bulge" style
  `TO_NOTVICT` lines had the scythe as `$3`, so the victim saw their own name in them. The victim is
  now `$3` in those acts.
- **golem_load** `:hagsecure:` (line 1311) and **get_golem** `:lostpc:` (line 1423): when the player
  left during the talk, `priority` stayed on, so Dalmar's later programs (get_golem, got_stuff)
  stopped getting events; added `nopriority`. (Dalmar isn't loaded today, step 1 question 1.)

Questions:
1. **prg2** (line 465) and **prg3** (line 520): every command a guild member types near the Zombie
   Lord gets the "would you like to journey to our guild... Just nod" tell (the Servant says the lift
   offer aloud), and `if (command("nod"))` tests the command that woke the program, so the teleport
   happens when the member nods, whether or not they heard the offer. Meant: offer once when a member
   arrives (or says something), then wait for the nod? Fixed: the offer comes once per member while he stays in the room (at his first command there); a member's nod moves him as before, without the offer repeated.
2. **necro_give** (line 710): every `worship` of Wicca by a necromancer with alignment below -349
   loads a new severed hand (line 748), and at level 20 and up a new Scythe of Bone, with no check
   that they got one before. Intended, or once per player (a quest flag)?
3. **nokill** (line 55): `if (command("flee")) block;` runs on a combat event (`SFB_COM`), where the
   command is never "flee", so it never blocks anything. The player's hit points are set to 0 a line
   later anyway; remove the two lines?
4. **bonescythe** (lines 1652-1715): the effects are `if (flux < 25) {fear}`, `if (flux < 50)
   {confusion}`, `if (flux < 50) {curse} else {disease}`: below 25 the enemy gets fear, confusion and
   curse (three spells, three pauses), 25-49 confusion and curse, 50 and up disease. It reads as an
   `else if` chain meant to give one spell each, the second `< 50` perhaps `< 75`. Which?
5. **golem_brain** `:lostloop:` (line 1154): `walk_room(u.nameidx+"@"+u.zoneidx)` walks to the
   owner's own symbolic name, not to a room, and `u` (from `findunit` on the owner's name) can be
   null. Meant `u.outside`, with a null check? (Dead while Dalmar isn't loaded.)
6. **golem_load** (line 1211): the wait tests `"familiar" in activator.inside.name` (only the first
   thing the player carries; the golem's names are "flesh golem", "golem"). Meant "the player has no
   golem following"? (Dead while Dalmar isn't loaded.)
7. **got_stuff** (lines 1441-1525): the ingredient is found as `self.inside` after the next event,
   and `exdp` (an extraptr) is read after that wait, so it is always null there; the counting works
   only because the branch that sets `exdp` has no wait before the final check. Worth rewriting with
   `wait(SFB_DONE, command("give") and (target == self))` and `medium`, as bounty.zon's givers do,
   if the golem quest comes back.
