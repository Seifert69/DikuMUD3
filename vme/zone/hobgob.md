# hobgob: review questions

## Step 1

1. **weeve** (line 4581): Weeve runs "Weeve's Weapons" and sells `short_sword` and `long_sword`,
   but the item types his shopkeeper DIL trades in are `ITEM_ARMOR`+`ITEM_WORN` (the same line as
   Able the armorer), so he buys armour and won't buy weapons. Should it be `ITEM_WEAPON`?
2. **cloak_skin1, dragon_scale1, gem, tri_plate** (lines 5151, 5135, 6171, 6386): the text promises
   bonuses the object doesn't give. The cloak's `$identify` says it lets you move about undetected
   and gives extra strength and dex, but it has no transfers. The scale's extra says holding it makes
   you more dexterous; no DEX transfer. The gem's `$identify` says "Makes you a bit stronger"; no STR
   transfer. The tri breastplate's `$improved identify` says "Turn undead bonus 6", but its
   transfer is `BONUS_AVERAGE_PLUS` (+1). Add the bonuses, or change the text?
3. **mines and maze** (from line 2063): several mine exits are one-way: mine_6 west to mine_11,
   mine_8 west to mine_13, mine_9 west to mine_2, mine_10 east to mine_6, mine_12 east to mine_8,
   mine_13 east to mine_9. None of them has an exit back. In the maze above the town (from line 671),
   mtrail_4 north to mtrail_6 and mtrail_7 east to mtrail_1 are one-way, and mtrail_6 south leads back
   to mtrail_6. The maze is described as confusing, so that is probably on purpose; are the mines
   meant to be a maze too?
4. **cant** (line 1854): the room says the tables would seat about 150 people; the tables extra
   says the 5 tables could seat about 50. Which number is meant? Fixed (text follows the world): tables extra now says about 150 people, like the room.
5. **DIL, for step 2** (lines 5488, 5684, 3577): `wings_2` makes the wearer say "lost him" when it
   loses track of the player (`exec("say lost him", pc)`). `wing_scroll` makes the player emote
   "You are not yet meant to have this knowledge" instead of telling them. In `mageq_2` (the
   cheater branch) "You feel less experienced." is an act() with no character, so nobody sees it. Fixed in step 2.

## Step 2

Fixed in place:
- `smelldil` (line 77): the immortal check compared `activator.type` with `IMMORTAL_LEVEL`, which
  is never true; now `activator.level`.
- `pet_container` (line 254): `link(pet, self.inside)` linked the new pet into whatever was first in
  the cage (null when empty); now `link(pet, self)`.
- `mageq_2` (line 3577): "You feel less experienced." had no character; now sent to the player
  (step-1 item 5).
- `i_fool` (line 3707): after the story the program ran off its end, which restarts it at the top
  (only `heartbeat` again); the `goto start;` added there has no effect in play; kept for clarity.
- `lor_2` (line 3826) and `eagle_2` (line 4062): `secure(item, ...)` and the test
  `item #= self.inside` used `item` without setting it; set to `self.inside` first, as `mageq_2`
  and `getit_dil` do.
- `getit_dil` (lines 4921, 4958): after a wrong item or the first of the two items, the program
  went back to waiting with the player still secured, so the old cuss said "Wuss! Can't even
  handle one little assignment." whenever that player later left; `unsecure(pc)` added.
- `wings_2` (line 5465): `if` instead of `else if` after the assassin branch, so assassins were
  flown home and then also told "Tell an administrator your guild wasn't found."
- `wings_2` (line 5495): removed the debug `exec("say lost him", pc)` that made the player say
  "lost him" (step-1 item 5).
- `wing_scroll` (line 5691): a player without the quest emoted "You are not yet meant to have this
  knowledge"; now it is told to them (step-1 item 5).
- `wing_scroll` (line 5695): the `:lost:` branch ran off the end, which restarts the program at the
  top (only `heartbeat` again); the `goto start` added there has no effect in play; kept for clarity.

Questions:
0. **smelldil** (line 76): the skunk's spray copies `stink` onto `self.outside`, the room, instead
   of the player it sprays (the stink texts say "emits from your clothing"). The fix is
   `enemy := pc`. Not made: `stink` is a recall DIL, so players would then carry a skunk smell that
   is announced to their room every tick and survives a save, which they never did before.

1. **lover_6** (Reckus, line 3241): `secure(pc, losthim)` with `pc` never set (only `pname` is), so
   the `losthim` branch can't run. Setting `pc` would make the quest break when the player walks
   off during the speech: Jesealla keeps her `$gnitiaw` extra and never starts the ending again.
   Remove the `secure` (and the dead `losthim`), or set `pc` and, on `losthim`, tell Jesealla to
   drop `$gnitiaw`? Fixed (vmc warnings): the `secure` removed.
2. **lover_2** (Jesealla, line 3038): she ends the quest for any player whose command she sees
   while Reckus is in her room, also one without "Lost Love Search Ongoing"; `lover_3` then gives
   that player the heart tear and "Lost Love Search Complete". Add
   `(LOVE_ONGOING in activator.quests)` to the wait?
3. **echo_pet** (the macaw, line 368) and **pet_func** (the kittens, line 394): neither has a
   `goto start`, but both restart at the top when they run off their end, so the macaw echoes every
   say in its room and a kitten follows and purrs at every tickle. The macaw also echoes other
   NPCs' says, so two macaws in one room set each other off (each echo has a 3 in 4 chance of being
   answered). Should it ignore other pets' says? Fixed: the macaw ignores says by units running echo_pet (other macaws and itself), so two macaws no longer set each other off; it still echoes players and other NPCs. The kittens are fine as they are.
4. **teddy_hea** (line 5096): the teddy heals its holder when anyone in the room says "I love you
   teddy", and blocks that person's say; the description says you should say it. Add
   `(activator == self.outside)` to the wait? Fixed: added; only the holder's say works (and is blocked).

### Precedence

Since #603 (merged 2026-11-15) `and`/`or` bind as in C (looser than the comparisons), so once the
zones are rebuilt these conditions do what was written. What that changes in play is on Papi's
review list (#596's table).


- `getit_dil` line 4930: `if (command("give") and activator.type == UNIT_ST_PC)` is read as
  `(command("give") and activator.type) == UNIT_ST_PC`, always false, so a second `give` typed right
  after the first is missed. Meant: `(command("give")) and (activator.type == UNIT_ST_PC)`.

## vmc warnings

- **lover_6** (line 3240): `secure(pc, losthim)` secured a `pc` that is never set. Removed: setting
  `pc` would make the quest stick when the player walks off during the speech (question 1), and
  nothing else uses `pc`. `:losthim:` is unreachable now, as it was before. No change in play.
- **mageq_2** (lines 3578-3579): after `:cheater:`'s pauses the program was woken by a tick, so
  `if ((command("give")) and (activator.type == UNIT_ST_PC))` was always false and it always went
  to `start`. Now plainly `goto start`, with `unsecure(pc)` first, as the main path does: the
  cheater stayed secured, so walking off later made the NPC say "They won't get far!" out of
  nowhere. Changes play: only that stray line is gone.
