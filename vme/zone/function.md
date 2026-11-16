# function: review questions

## Shared zone review

Fixed in place:
- mercenary_hire, lines 689 and 752: `victim.type & UNIT_ST_PC|UNIT_ST_NPC` read as
  `(victim.type & UNIT_ST_PC) | UNIT_ST_NPC` (`&` binds like `*`, `|` like `+`), so the "not a
  character" test was never true and `contract` took an object as its victim. Parenthesised. (At
  752 `offer` already looks for characters only, so nothing changes there.)
- mercenary_hire, line 698: "Sorry <name> you should" gets its comma; line 741: "says 'Get me"
  gets "says, '".
- evaluate, lines 1043-1072: "craftmanship" (three times) is "craftsmanship"; "neither a sword,
  shield nor armor" says "weapon", since any weapon passes the test.
- tuborg, line 1282: "$1n tastes $2n, enjoying every drop." (comma).
- no_kill, line 1419: "Pk" is "PK".
- quest_restrict, nquest_restrict, quests_restrict, lines 3742, 3811, 3878: "the right Quest" is
  "the right quest".
- ali_restrict, lines 3952 and 3955: "You are too good/evil to use $2n" end with a period.
- shopkeepernew and the shopkeeper wrapper, lines 5434-5440 and 6175-6181: the default shop lines
  end with a period, and "i don't have enough cash" is "I". (The wrapper fills the entries a zone
  leaves out from its own copy of the list.)
- shopkeepernew, line 5569: "I better get back" is "I'd better"; line 5599: "people i can see"
  is "I"; line 5798: "Ewww, it's alive!"; lines 5815 and 5839 end with ".'".
- shopkeepernew, lines 5887, 5963, 5985: "Those/That cost you $2t" had no closing ".'" (the say
  never closed its quote).
- shopkeepernew, lines 5889-5893: buying several: "$1n gives you 3 a long sword." and
  "gives Bob some a long swords." used `$2n` (the title, with its article); now `$2Ns`, as the
  shop's "that many $2Ns" line does, and the count is `nitem` rather than the first word of
  `argument`. The stock item, destroyed when the last of it is sold, was destroyed before the two
  acts that name it; the destroy now comes after them.
- shopkeepernew, line 5908: "You can't afford 3 of those." went TO_ALL (the whole shop, the
  shopkeeper included); now TO_VICT, the buyer.
- wander_zones, line 7515: `self.exit_to[x]` on the NPC (always fails) is
  `self.outside.exit_to[x]`. No change in play: the zone test after it already skipped the
  missing exits.
- channel, line 8362: "Channel x Already Active!" is "is already active!"; line 8444: "is active"
  ends with a period.
- channel, line 8395: `comm delete x` for a channel you aren't on said "Unknown channel command
  or incorrect options." because the outer test already required the channel to be on, so the
  "Channel x is not on!" branch inside it could never run. The outer test no longer requires it.
- channel, lines 8467-8481 (`comm help`): `<channel name>`, `<on|off|depart|join>` and
  `<action>` were raw HTML, so the client hid them as unknown tags ("comm add " and nothing
  after); now `&lt;...&gt;`. A string with a newline inside it is two strings; "comm echo" gets
  its blank line; "gchat only members will hear you" is "gchat: only members of your guild will
  hear you."
- channel, line 8488: `comm inform` with no word tested `words.[1] == ""`, past the end of the
  list (fails, so false), and printed nothing; now `length(words) < 2`, and the text says
  'comm help' (the command), not 'chan help'.
- aggressive, lines 9088-9101: the comment above the template says a "" victim text shows the
  room's text to the victim too, but with two entries the victim got `act("")`, nothing: 111 of
  the 374 aggressive@function copies (both zone directories) give `{"...", ""}`, so their victims
  saw nothing as they were attacked. With no victim text, the room's text now goes TO_ALL, with the
  victim as `$3n` (it was null in the one-entry case, so a `$3n` in it printed nothing).
- moneychanger, line 9830: "noone" is "no one".

Questions:
1. **teamwork** (line 8567): `if (u.zoneidx == self.zoneidx) continue;` skips the NPCs of the
   teamworker's own zone, where rescue (line 8649) has `!=` and the 2020 note at the top of the
   zone says both should help only their own zone's NPCs. Should it be `!=`?
2. **walk_room** (line 2245): sets `heartbeat` (4 seconds, then `spd`, then 10 seconds in
   takeabreak) and never gives the caller its own heartbeat back, so a DIL that calls it runs on
   walk_room's heartbeat afterwards (destroy_symbols saves and restores it). Save it at the top
   and restore it at every return? Returned to Papi: right in principle, but about 240 callers
   (pub and private) then pause at their own heartbeat after a walk (1 second for most DILs that
   never set one, instead of walk_room's 5+), a timing change players would notice in many NPCs.
3. **All the *_restrict templates** (guild, prof, anti_guild, quest, nquest, quests, ali, mvlev,
   vlevel, level, race, abi, ski, sp, weap, sex, ply; e.g. lines 3518-3519): `:start:` sets
   `damage := 0; percent := 25;` over the arguments, so whatever damage and percent a zone gives,
   every restricted item takes 25% of the wearer's current hit points (and `damage == -1`, the
   openroll option, can't happen). This has been so since the first public upload. Intended
   (then the two arguments and the code using them can go), or keep the arguments in other
   variables and reset from those?
4. **The same restrict templates** (e.g. line 3529): the test meant to let through NPCs not
   working for a player, `(activator.type == UNIT_ST_NPC) and not ("$pc obey" ...) and
   (activator.master.type != UNIT_ST_PC)`, is false when the NPC has no master (its last side
   reads a field of null, which counts as false), so an NPC with no master that wears the item
   is burnt like a player, though it works for no player. Intended? If not, let a null master
   through too (`... and ((activator.master == null) or (activator.master.type != UNIT_ST_PC))`).
5. **shopkeeper** wrapper (lines 6187 and 6201): it drops custom_acts entry 6 as "the closed
   message", but the BasicZoneManual (and the 10-entry lists zones give) has the closed message
   8th, entry 7: 0 no such item, 1 you haven't got it, 2 don't trade, 3 can't afford, 4 bought,
   5 sold, 6 not that many, 7 closed, 8 no use for it, 9 shop can't afford. Since `insert` at 7 and
   8 leaves 6 empty, shopkeepernew gets: 6 the default "not that many", 7 the zone's closed text
   (said when a player tries to sell a `$no-sell` item), 8 the zone's "no use for it" text (said
   when the shop is out of cash), and 9 unused. About 70 shops use this wrapper. Fix: build the
   list with `addstring`, skipping entry 7 (0-6 as they are, 8 to 7, 9 to 8)? Some zones give
   8-entry lists (6 the shop's cash, 7 closed), which would want checking too.
6. **limit_number_to** (line 463): counts the copies of the item with
   `(u.nameidx == self.nameidx) and (u.zoneidx == tu.zoneidx)`, the zone of the next unit in the
   inventory rather than the item's own (and false for the last unit), and `i` starts at 1 although
   the item itself is among those counted, so the limit is off by one. One zone uses it. Use
   `self.zoneidx` and start `i` at 0? (It changes which copies explode.)
7. **guard_unit** (lines 354-359): when the guarded unit isn't in the room, `u` is null, and an
   argument that names no object (`open door`, `cast heal`) makes `findunit` null too, so
   `null == u` blocks every open, close, lock, unlock, pick and cast in the room. Add
   `if (u == null) goto start;` after the findsymbolic? Fixed: guard_unit goes back to waiting
   when the guarded unit isn't in the room.
8. **scramble** (line 7872, `:sorry:`): the drag and ride interrupts jump here, where `pc` is null
   (it is only set at `:scramble:`), so nobody sees "Sorry you can't do that here!", and nothing
   blocks: dragging and riding out of a scrambled room work normally. Use `activator` and add
   `block` (and "Sorry, you")? That stops drag and ride in those rooms, which the message promises.
9. **guard_way** (line 5179): `activator.name in excludepc` is a substring test on a string, so
   anyone whose name is part of the allowed one passes the hedge wall too. `==`?
10. **global_wander** (line 7602): `(spd*PULSE_SEC) < 5` only raises a `spd` of 0 or 1 seconds;
    wander_zones (line 7472) has the 5-second minimum its comment gives. `spd < 5`? Returned to
    Papi: changes a timing; one NPC (demsiege, spd 3) would wander every 5 seconds instead of 3.
11. **aggressive** (line 9002): `tme == 0` jumps to `:get_target:` at load time, skipping the
    `tar` and `attack` checks and running a target search before anyone has come in. Wrap only the
    time arithmetic in `if (tme > 0)`? Fixed: aggressive wraps only the time arithmetic in
    `if (tme > 0)`; no target search at load.
12. **guard_level** (line 7785): "$2n arrives from <dir>." goes TO_CHAR to the leader, who is still
    in the old room, while nobody in the new room sees anyone arrive. Send the arrival to the new
    room after the link instead? Fixed: guard_level sends "arrives from" to the new room after each
    link, for the followers and the leader.
13. **transfercoins** (lines 9597-9603 and 9642-9648): the loops `continue` without
    `cash := cash.next`, so if they are ever reached (a unit with a coin's symbolic name that isn't
    money) the DIL loops until the server kills it. Unreachable in practice; rewrite the loops or
    drop them?
14. **reward_give** (line 320): a head wanted elsewhere is handed back with
    `exec("give head " + ...)`, the first "head" the NPC holds, not necessarily `medium`. Give back
    `medium.name`? Fixed: reward_give links `medium` itself back with give messages (`medium.name`
    is "head" too, so it wouldn't have picked the right one).
15. **isopen** (line 4761) builds a `clock` list and never uses it, and returns "closed" only at
    the exact hours listed. Nothing calls it any more, and the old shopkeeper body after the
    wrapper's `quit` (lines 6224-6929) is dead code too. Remove both?
16. Help page **contract** says "the offer command quotes a much lower price at present", but
    offer and contract use the same price (lines 727 and 766). Update the help page? Left: contract.hlp already says offer tells you the same price.
