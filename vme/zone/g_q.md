# g_q: review questions

## Step 1

1. **grail** (line 2815): its description tells the player "To use this, type: Pour grail <player>.
   It will reduce their age by 20%.", but `dilcopy grail();` is commented out and there is no grail
   DIL in the zone, so the item does nothing. Remove the instruction from the text, or bring the DIL
   back?
2. **banksy** (line 1856): the graffiti seller stocks fifteen cans, among them `graf_w` (white), but
   not `graf_bw` (bright white, line 2550), which nothing else loads either. Should banksy sell it
   too, or is it left out on purpose?
3. **magef_manual** (line 2231): Hexica's owner's manual is a changelog from 1995 ("MageFamiliar
   Version 5.2", "Coming in revision 5.2 (scheduled for Jan 1995)", "KNOWN BUGS: lost master
   routine"), while the familiar's tags say "Version 6.0" and the code comment says much has changed
   since. Some of what it promises may no longer hold (summoning a familiar makes it attack you; the
   familiar knows every spell). Should the manual be rewritten as an in-game guide to what familiars
   do today?

## Step 2

Fixed in place:
- **do_graffiti, graf_r** (lines 81, 228): every graffiti can and remover in the room answered
  `use graffiti` / `use remover`, whoever held it: a can on the floor or in someone else's hands
  could block the player's own can ("You must hold that before you can use it"), or paint (and
  empty) another player's can while the messages went to the one who typed. The wait now also
  needs `activator == self.outside`, the holder the code works with.
- **familiar_main** (line 1718): any `give` in Hexica's room went to the ingredient code, so a
  player on the familiar quest who gave something to another player had it destroyed by Hexica,
  and others were asked "Trying a bribe?". Now only gives to Hexica (`target == self`).
- **familiar_main** (line 1723): the "You have been banned" answer used `pc.name` before `pc` was
  set; now `activator.name`.

Questions:
1. **fam_personality** (line 1071): the wait asks for `(activator == self)` and `(activator != self)`
   at once, so it never wakes and the familiar jealousy (98% to destroy another familiar of the same
   master, 2% grumpy) has never run. Meant `activator != self` with the same master. Not changed:
   it would start destroying players' familiars. Keep it off and remove it (with the unused
   `oldfam_persona`, line 1132), or turn it on?
2. **prg1** (magic cricket, line 1491): the program has no loop: after the cricket's first escape
   from a player and its walk home, it ends, and from then on (until it is reloaded) it can be carried
   as long as anyone likes. The comment says it "routinely tries to go back". Add `goto start;` at
   the end? Not made: it makes the quest harder than it is now.
3. **dil_zap** (Phoenix, line 1438): also no loop: it answers the first "fix me" after a boot and
   then ends. It resets a `$stone code` extra that only a zone outside this repository uses, and
   says "Your code was successfully fixed" even to players without it. Loop it, or remove it?
4. **create_fam** (line 442): when the player leaves during the ritual, the familiar already
   loaded into Hexica's inventory stays there (its `fam_unbonded` waits while an NPC holds it), one
   more for each broken ritual, and Hexica says "lost pointer". `unsecure(familiar); destroy(familiar);`
   at `:hagsecure:` (and a line in character instead of "lost pointer")?
5. **no_steal** (banksy, line 1887): blocks every `steal` in the room, by anyone from anyone, while
   banksy wanders Udgaard. Only steals from him meant (e.g. `findunit(activator, argument,
   FIND_UNIT_SURRO, null) == self` on the victim word)?
6. **rocket** (line 1963): the player who lit it stays secured to `start` through the countdown;
   if he leaves after "leaves the ground with blinding speed" (`self.minv := 200`), the rocket goes
   back to waiting, invisible to mortals and never launched. `unsecure(pc);` right after
   `pname := pc.name;` (nothing later uses `pc`)? Also the loop "The rocket you are riding in
   explodes" never reaches anyone, since `ast_launch` has already thrown the riders out.

