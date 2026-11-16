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
   enemy.name, with enemy null. For step 2. Fixed in step 2.

## Step 2

Fixed in place:
- **bone_aff, flame_aff, flame_aff2, dagger_nheal** (lines 232, 271, 277, 294, 334): spell-effect
  messages for the victim were sent TO_VICT with `null` as the victim, so the victim never saw
  them; now `targ`. flame_aff line 280 used the built-in `target` instead of its `targ` argument.
- **shitthrow** (line 436): anyone in the room typing "throw dust" set off the dust held by someone
  else; the wait now checks `activator == self.outside`. Line 446: with no target found it printed
  "I see no " + enemy.name with `enemy` null, so nothing at all (step 1, question 6); now "You see
  no one here to throw $2n at!". `secure(enemy, lostenemy)` ran before `enemy` was set, so it
  secured nothing; it now comes after the `findrndunit`. (`findrndunit` can still pick the thrower
  himself when he is the only player in the room; a question, since it changes who is hit.)
- **nighteff** (line 492): a spell effect must take `(medi, targ, hm)`; without them the server
  refused it ("Spell DIL effect wrong arg count") and nightdrain's cause wounds showed the plain
  spell messages. It now has the arguments and uses them instead of `medium`/`target`.
- **exit_arena** (line 708): `exec("look", self.outside)` made the room look; now the player looks
  at the arena entrance.
- **whip** (lines 3275, 3286): TO_VICT with `null` as the victim; now `enemy`.
- **nine_switch** (line 4537): when the weapon was neither flail nor whip the program ran off its
  end, which restarts it at the top (`heartbeat` and `on_activation` again, which change nothing);
  the `goto start` added there has no effect in play; kept for clarity.

Questions:
1. **arena_enter** (line 767): it sends "remove kill prottection" (misspelt, also in basis.zon),
   but no_kill@function listens for "remove kill protection", so entering the arena never ends a
   player's post-death PK protection. Correct the spelling? It changes who can be attacked after
   leaving the arena, so it's your call.
2. **restore_pc** (line 1114): nothing copies it, and it copies `expcatch@arena`, which doesn't
   exist. Delete the template?
3. **goddaggerdil** (line 2846): casts lightning 3 on the wielder (`self.outside`), with
   dagger_nheal as effect, whose messages describe the dagger digging into the enemy's flesh;
   `enemy` is set and never used. Was `enemy` the target? (Balance.)
4. **lio** (line 2703): `enemy.hp := enemy.hp + i` heals the enemy, with the message "Your $2N shoots
   forth a burst of pure evil". Meant `-`? (Balance.) Returned to Papi: `-` would make the weapon deal damage, a change of numbers.
5. **gel** (lines 3330-3342): the three first branches take 2-4 times `i` hit points, silently, only
   from enemies *above* IMMORTAL_LEVEL; mortal enemies only get the `i < 30` drain. Was `<` meant
   there, or is it a deliberate anti-god trap? (Balance.)
6. **flames** (lines 389-410): touching the flames takes 75 or 30 hit points without
   `position_update`, so a player can stand on below 0 hp until something else updates them; `if
   (1 < 30)` (line 402) is always true (meant `i < 30`; the same except when `i` is 30), and
   `secure(pc, lost)` secures a `pc` that is never set. Add `position_update(activator)` after the
   damage? The `secure(pc, lost)` part: Fixed (vmc warnings); the rest is still open.

## vmc warnings

- **flames** (line 382): `secure(pc, lost)` secured a `pc` that was never set. `pc := activator;`
  added before it, and `unsecure(pc);` at `:start:` so each touch lets the previous player go.
  No change in play.
