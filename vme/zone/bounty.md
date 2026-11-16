# bounty: review questions

## Step 1

1. **guide, historian, blacksmith, jeweler, summoner, necromancer and their `2` copies** (line 6465 and
   on): the titles are "The Guide", "The Historian" and so on, and vmc warns "Title CASE seems to be
   wrong" for all twelve. The zone uses them as proper names everywhere ("Go and speak to The
   Historian"), so I left them. Keep them, or make them "the guide" etc. (which would read oddly in
   the DIL speech that names them)?
2. **qnecro2** (line 3442): when you hand over a rainbow crystal, the Necromancer tucks it "into the
   pocket of his sash" if `"skin"` is in the quest names, otherwise he pats his chest looking for a
   pocket. The sash is the `"sash"` entry; this looks like it should test `"sash"`. (DIL logic, so
   not changed.) Fixed in step 2.
3. **qnecro2** (line 3421): it waits for `SUMMONER_ONGOING` rather than `NECRO_ONGOING`, so the
   Necromancer only takes items while the Summoner's task is open, and it reads
   `pc.quests.[NECRO_ONGOING]` without checking that it exists. Intended? (For step 2.) Carried
   into step 2, question 9.
4. **qsummoner2** (lines 2700, 2714 and on): the Summoner says "sets the "+item.title+" aside" /
   "tucks the "+item.title+" into her robes" after `destroy(item)`, so the item title may come out
   empty, and "the" before a title that already has an article reads "the a ...". Same pattern in
   qnecro2 ("tucks "+item.title). For step 2. Fixed in step 2.
5. **tome** (line 6938): at boot the world-run log shows
   `load_string(): File does not exist: ../lib/clan/strings/bounty_names.data`. Expected until the
   first hero signs the tome, or should the file be created empty?
6. **zone header `notes`**: the developer notes (not shown to players) still hold the v1.0 to-do
   list ("500,000 lbs of steel ... Need to write ability to count", "Final reward will be a hero
   plate"). Keep as history, or trim to what is still open?

## Step 2

Fixed in place:
- **corpse_move** (line 463): the loop over the room took `item.next` after `link(item, ...)` had
  moved the corpse, so it went on through the sanctum and skipped the rest of the battle room; and it
  called `cleanup()` (which destroys every corpse left in the battle rooms) right after the first
  player corpse. With two players dead in one room, the second corpse (and everything on it) was
  destroyed. Now the next unit is kept before the move, and qhistorian3 is stopped and `cleanup()`
  run once, after every player corpse has been moved.
- **report** (line 1342): the Necromancer's book/sash/scythe/skin list read `NECRO_ONGOING` even
  when the player had none, so after the task he said "I still require..." for all four. The list
  is now only given while the task is open.
- **reprint** (line 1756) and **make_bounty** (line 4727): the contract text went to the whole
  room (`TO_ALL`) right after "$2n quietly slips you a contract" / "quietly slips $1n something";
  now `TO_CHAR`.
- **status** (line 1789): `sd`, `sm`, `sb` kept the previous player's contracts, so a player without
  a bonus (daily, monthly) contract was handed a copy of the last player's; they are now emptied for
  each player and tested with `!= ""`.
- **qhistorian2** (line 2015): after "I need just a little more time" the heartbeat stayed at 100
  seconds, so the next talk took 100 seconds a line; the 4-second heartbeat is now set at `:start:`.
- **qblacksmith2** (line 2557): the earth element was destroyed while still secured, which jumped
  to `:donezo:` and skipped `smith_done`, so giving the earth element last didn't finish the task
  until the next give. Added the `unsecure(item)` the other branches have.
- **qsummoner2** (lines 2715-2785): the "sets ... aside" / "tucks ... into her robes" emotes ran after
  `destroy(item)` and printed an empty title, and "the "+title made "the a white jewel"; the emotes
  now come before the destroy, without "the" (step 1, question 4).
- **qjeweler2** (line 3278): tested `"$packages"` but added `"packages"`, so the name was added again
  at every give; now tests `"packages"`.
- **qnecro2** (line 3457): the crystal emote ran after `destroy(item)` (empty title), and it tested
  `"skin"` for the pocket of his sash; now before the destroy, and `"sash"` (step 1, questions 2, 4).
- **expel** (line 3758): "the world fades" and "You feel a searing pain!" went to the Guide
  (`TO_CHAR` with the Guide as `$1`) and "$2n begins to fade" to the attacker too; now `TO_VICT` to
  the attacker, `TO_NOTVICT` to the others, and the pain to the attacker.
- **spree_m** (line 3976): the "has just led a successful spree" announcement walked the player list
  from the leader, not from `ghead()`, so players before the leader in the list never heard it, and
  the line had no `<br/>`.
- **new_spree** (line 4055): `sl` kept the list from the last run, so a player without a
  `$DAILY_SPREE` could be told "Perhaps you'll do a bit better this time"; emptied per player.
- **make_bounty** (lines 4604, 4741): the "daily, monthly or bonus?" and "abandon this one?" waits
  took the answer from any player in the room, and the abandon wait then set `pc := activator`: a
  bystander's nod cost the bystander 25,000 xp, 5 bounty points and their bonus contract. Both now
  wait for the player who asked.
- **claim_bounty** (line 4826): the mulligan's "say 'daily' or 'monthly'" wait took any player's
  answer (and ended the giver's contract); now only the giver's. Line 5042: the head was destroyed
  twice (`destroy(self.inside)` again after the commented-out leaderboard), destroying whatever else
  the Hunter held; the second destroy is gone.
- **shop** (line 5365): `if(argument) arg := argument;` kept the previous buyer's number, so `buy`
  without a number bought that item again; `arg := argument;` always.

Questions:
1. **craft_s** (line 771) and **repair2** (line 592) both run on the Blacksmith's give. When a
   player gives a piece to re-forge, repair2 sees an armour with `nquest_restrict` ("This is a
   special item... We'd better not melt this one down") or a non-armour ("I can't melt this down")
   and gives it straight back; craft_s then, on the nod, loads the upgrade and runs
   `destroy(self.inside)` (line 864) on whatever the Blacksmith holds. So the player keeps the old
   piece and gets the new one, and something else of the Blacksmith's may be destroyed. Fix: do as
   repair does (line 500): `dildestroy("repair2@bounty", self)` while crafting and copy it back at
   `:donezo:`, and `secure` the given item and destroy that instead of `self.inside`. Not changed:
   it takes away the extra piece players get today. Returned to Papi: the fix takes away the extra piece players get today (harder).
2. **guard_agg** (line 2375): the `goto start` sits inside the `foreach`, after the first unit only,
   so a guard attacks only when the first unit it looks at is not an ally (often it is itself or
   another guard). Was it meant to attack the first non-ally (move `goto start` into the `if`)? That
   makes the final battle harder.
3. **qhistorian3** (line 2236): the guards are given `str_arms@midgaard`, which doesn't exist
   (`load` fails, so they have nothing on their arms). Meant `lth_sleeves@midgaard`, the sleeves
   that go with the leather jerkin and pants loaded with it (clan guards elsewhere wear that set)?
   `str_arms@bounty` exists too but is the guild's plate.
4. **pray** (line 3643): reads `$PK_TIMER` from `self` (the shrine), which never has one, so "recent
   battles prevent your focus" never happens. `$PK_TIMER` is on players (basis.zon); meant
   `pc.extra`? That would block praying for 10 minutes after a player-killing fight.
5. **new_spree** (line 4214): a follower is enrolled unless their `$DAILY_SPREE` today is `CLOSED`
   or `ongoing`. A follower with an unclaimed `SUCCESS` today is enrolled again, and that overwrites
   the `SUCCESS` before they said "end spree", losing the reward. Should `SUCCESS` (and `FAIL`?)
   also keep them out? Returned to Papi: a check that starts keeping followers out of a spree, and whether `FAIL` counts too is a choice.
6. **delivery** (line 1430) and **qjeweler2** (line 3308) both complete the Jeweler's task. When the
   100-delivery streak is the last thing done, delivery completes it with no experience, no bounty
   points and leaves the twelve `$jeweler_*` counters on the player; qjeweler2 gives 10,000 xp and 200
   bounty points and removes them. Should delivery do the same (or leave completion to the next
   give)?
7. **delivery** (line 1466) and **make_bounty** (line 4695): the `findrndunit` loops look for a random
   NPC in the level range with no `pause`; when few NPCs fit (e.g. a level 48-49 player, or a world
   with few such NPCs loaded) they can run long enough for the server to stop the program. A
   `pause` every so many tries, or a give-up after N tries? Fixed: delivery pauses every 1000 tries and after 10000 says it has no package right now (the server used to stop it after about 2500); make_bounty left, it has no level range and finds a target in a few tries.
8. **crafting** (line 709): the spot is `argument` matched with `in`, which accepts the first word:
   "neck please" matches "neck", then `load("neck please_STR@bounty")` fails, no piece is made, but
   `JEWELER_CRAFTING` is still set, so the one free crafting is gone. Use `getword(argument)`, or
   check `gift` before marking the crafting done? Fixed: the spot and the ability are the first word said, so "neck please" makes the neck piece.
9. **qnecro2** (step 1, question 3): it waits for `SUMMONER_ONGOING`, but `NECRO_ONGOING` is only
   given with `SUMMONER_ONGOING`, and the Summoner's task can only end after the Necromancer's
   activators, so the two are open together and it works. A player with only `SUMMONER_ONGOING`
   gets the item back ("Why did you bring me..."). Change the wait to `NECRO_ONGOING` for clarity?
   (Then such a player's item would stay with the Necromancer.)
10. **repair** (line 524): after "Give me the item you would like to have repaired now" it waits
    for that player's give with no time limit, and repair2 is removed meanwhile; a player who walks
    off without leaving the room keeps the Blacksmith from repairing or taking donations from anyone
    until they leave. Add a timeout? Fixed: after about two minutes without a give he says "Come back when you know what you want repaired" and serves others again.
11. **spree_m** (line 3908 on): the success log and announcement print the time from `minutes` /
    `seconds`, which were last computed at the last kill message (or never, for a spree with no
    kills), not at the end. Compute them in `:donezo:`? Returned to Papi: `:donezo:` is reached only when the spree's time is up, so computed there it would always print the full duration; today it prints the time of the last kill, except for the no-minimum spree with no kills (then "dur-1:60"). Which time the line should show is a choice.

### Precedence

Since #603 (merged 2026-11-15) `and`/`or` bind as in C (looser than the comparisons), so once the
zones are rebuilt these conditions do what was written. What that changes in play is on Papi's
review list (#596's table).

- **claim_bounty** line 5085: `if(i == 14 or i == 30 or i == 60 or i == 90 or i == 180 or i == 365)`
  is `((i == (14 or i)) == (30 or i)) ...`, true only for a streak of 1, so the daily milestone
  bonus (double xp and points, "for making 1 daily contracts in a row") is paid on the first daily
  contract and never at 14, 30, ... Meant `(i == 14) or (i == 30) or ...`.
- **claim_bounty** line 5125: the same for the monthly milestones `i == 6 or i == 12 or i == 18 or
  i == 24`: true for a streak of 1. Meant `(i == 6) or (i == 12) or ...`.

## vmc warnings

- **qnecro2** (line 3474): the warning is wrong; left as it is. `item` is secured at line 3443
  and still is at the sash's `destroy(item)`: the checker carries the `unsecure(item)` of the
  crystal branch (line 3461) into the exclusive `else if` after it.
