---
name: dil
description: Writing or changing DIL, the MUD's script language, in vme/zone/*.zon - NPC and object scripts, quests, player commands written in DIL (commands.def "func ="), helper functions called as name@zone. Covers the reference to use, the traps that the manual gets wrong or leaves out, and how to try a change. Use the vmc skill for compiling.
---

# Writing DIL

DIL lives in the `%dil` section of a zone (`vme/zone/<zone>.zon`) or inside a unit's
definition. A template:

```
dilbegin integer do_something(arg : string);   // type only for functions
external
   string ProfessionName@professions(prof_xxx : integer);   // other zones' functions
var
   pc : unitptr;
   pname : string;
code
{
   ...
   quit;            // procedures end with quit; functions with return(value);
}
dilend
```

A unit runs a template with `dilcopy name@zone(args);` in its definition.

## Reference

- `vme/doc/dil/AGENTS.md` is the overview; each function, field and keyword has its own
  page in `vme/doc/dil/*.wiki`, also at <https://wiki.dikumud.com/wiki/DikuMUD>.
- The manual has mistakes; some of its own examples break the rules below (variables named
  `name` or `target`, a two-argument `findunit`). When it and the code disagree, the code is
  right: keywords in `vme/src/vmc/diltok.h`, functions in `vme/src/dilexp.cpp`, fields in
  `vme/src/dilfld.cpp`, statements in `vme/src/dilinst.cpp`.
- Working examples: grep `vme/zone/*.zon` for the function you need. `quests.zon` and
  `randomt.zon` hold helper libraries.

## Traps

- **Reserved words.** Every function, field and built-in variable in `diltok.h` is reserved,
  so `name`, `target`, `length`, `title`, `level`... can't be variable names. vmc says
  `718: syntax error` / `Token: 'name'` at the `var` line. Use `pname`, `namex`, `u`.
- **`and` / `or` evaluate both sides.** No short-circuit: `(u != null) and (u.level > 5)`
  still reads `u.level`.
- **A field of null fails, and a failed value is false in every comparison**: `u.level > 5`
  and `u.level <= 5` are both false when `u` is null. Test `if (u == null)` on its own first.
- **String `==` and `!=` ignore case**; `<` and `>` don't.
- **Locals start empty**: null, 0 or "". No need to set them first.
- **Units and extras in variables don't survive a wait**: see "Waking up and secure" below.
- **`"word" in s`** is a substring test that ignores case ("key" is in "Do you have a KEY").
- **`findunit(self, arg, FIND_UNIT_SURRO|FIND_UNIT_WORLD, null, UNIT_ST_PC)`** takes the word
  it matched out of `arg` (copy `arg` first if you need it afterwards), matches partial names
  ("bra" finds Brawla), and only finds what the searcher can see. To find a player by exact
  name, walk the player list instead (next point).
- **The global list is sorted**: all players first, then NPCs, then objects, then rooms. A
  walk for one type stops at the first unit of another type; the list holds tens of
  thousands of units, so never walk it all to find players:

  ```
  pc := ghead();
  while ((pc.type == UNIT_ST_PC) and (pc.name != pname))
     pc := pc.gnext;
  if (pc.type != UNIT_ST_PC)
     ... // not in the game
  ```

  `phead()` is the first player (null when none is on), `nhead()` the first NPC, `ohead()`
  the first object and `rhead()` the first room, each with `.gnext` to go on.
- **Moving an NPC through an exit**: `exec("<dir>", self)`, then check `self.outside`, as
  CLAUDE.md says; `link()` only where no exit exists.
- Text to a player is `sendtext("...<br/>", u)` (lines end with `<br/>`) or `act(...)` with
  its `$1n`-style fields. `log("...")` writes to `vme.log`.

## Waking up and secure

A program sleeps in `wait(...)`, `pause` and between heartbeats. **Every event of the kind it
waits for wakes it, even one the wait's condition then turns down**: with
`wait(SFB_CMD, ...)` that's every command typed around it. At every wake
(`vme/src/dilrun.cpp`, `run_dil`):

- **These built-ins describe the event that woke it**: `activator`, `medium`, `target`,
  `power`, `argument`, `cmdstr`, `excmdstr`, `excmdstr_case` and `command(...)`. After a
  `pause` or the next `wait` they belong to another event (a tick has no activator), so take
  what you need right after the wait:

  ```
  wait(SFB_CMD, command("say") and (activator.type == UNIT_ST_PC));
  pc := activator;
  secure(pc, lost);       // keep pc while it stays around
  said := argument;       // strings keep their value anyway
  pause;
  ```

- **Every `unitptr` variable that isn't secured becomes null**, at every wake, not only
  after `pause`: a unit from `findunit`, `activator` or a loop is gone after the next wait.
- **Every `extraptr` variable becomes null**; they can't be secured, so look the extra up
  again after waiting.
- `string`, `stringlist`, `integer` and `integerlist` variables keep their values, and `self`
  is always the unit running the program.

**`secure(u, label)`** keeps `u` in its variables across waits while `u` stays in the
program's surroundings: the room the unit is in (through whoever carries it), with
everything in it and what the characters there carry, but not the inside of containers or
rooms within it (unless they have `UNIT_FL_TRANS` and are open).

- When `u` leaves (walks out, is put in a sack, is taken into a room within the room) or is
  destroyed,
  the program jumps to `label` at its next wake, every variable holding `u` becomes null,
  and the secure ends. That also happens when the program's own unit walks away from `u`.
- `unsecure(u)` ends it; `secure` on the same unit again only changes the label.
- Secures belong to the call that made them and end when that template returns.
- The label is where to give up gracefully: `:lost:` and go back to waiting.

## Several programs on one unit

- A unit's programs get each event in `fnpri` order, lowest first: `FN_PRI_COMBAT` 10,
  `FN_PRI_RESCUE` 20, `FN_PRI_BLOCK` 25, `FN_PRI_MISSION` 30, `FN_PRI_CHORES` 40 (the default
  for a `dilbegin` without `fnpri`). Written `dilbegin fnpri(FN_PRI_BLOCK) name(...);`.
- `block;` stops the command (with SFB_CMD it never runs) and the programs after this one.
  A program that blocks must have a lower `fnpri` than the ones it should keep the command
  from.
- `priority;` keeps every later program on the unit from getting any event until
  `nopriority;` (so a talking NPC isn't walked off by `wander_zones`). Go back to waiting
  with `nopriority;` first, or the unit's other programs stay dead.
- `on_activation(cond, skip);` at the start: while `cond` holds, events are ignored (lost, not
  queued); the usual one is asleep or fighting (see `answer` in the zone skill's
  `example.zon`). With a label instead of `skip`, the jump clears it; set it again after the
  label.
- `interrupt(SFB_MSG, cond, label)` catches an event while the program waits for another.

## Events and messages

- `wait(SFB_CMD, command("give"))` wakes before the command runs: you can still `block`
  it, and `argument` is what was typed. `wait(SFB_DONE, command("give") and (target ==
  self))` wakes after it succeeded: `medium` is the item, already in your inventory, and
  `activator` the giver. Money given arrives merged with the coins already held (`power` is
  the amount): see `qstItemNoThank@quests` before giving coins back.
- Programs talk with messages, received by `wait(SFB_MSG, argument == "...")` (`activator`
  is the sender): `sendto(msg, u)` reaches every program on `u`; `send(msg)` the room and
  what's in it, but not the sender's own programs unless they're `aware`;
  `sendtoalldil(msg, "tmpl@zone")` every running copy of that template in the game.
- `findsymbolic("sym@zone")` is the first copy in the game;
  `findsymbolic(self, "sym@zone", FIND_UNIT_INVEN | FIND_UNIT_EQUIP)` looks only in that
  unit.
- `u := load("sym@zone")` makes a unit in the program's own unit (ignoring reset limits);
  `link(u, place)` moves it, `destroy(u)` removes it.
- `rnd(1, 100)` for chance.

## A player command in DIL

1. The template takes the rest of the line: `dilbegin do_mycmd(arg : string);`. `self` is
   the one who typed it; `getword(arg)` takes off the first word.
2. Add it to `vme/etc/commands.def` (`command = mycmd`, `func = do_mycmd@zone`,
   `minpos = POSITION_...`), near related commands. Try what its short abbreviations do
   afterwards: a new command can take over one that meant something else.
3. `vme/etc/commands.dat` is built from it and is in git: run `make dat_files`, or
   `perl ../bin/defconvert.perl commands.def commands.dat` in `vme/etc` (needs a built
   `vme/bin/vmc`; without it the .dat comes out empty). Commit both.
4. A command only some characters may use can answer everyone else the way the server
   answers an unknown command:
   `act("$2t is not a known command.", A_ALWAYS, self, excmdstr, null, TO_CHAR);`
5. Player commands have help pages; change the help together with the command.

## Trying it

Compile the zone (vmc skill), restart `vme` (it reads zones, `commands.dat` and the other
`vme/etc/*.dat` only at boot), log in through an mplex and try it, including the cases that
should be refused. DIL runtime errors go to `vme/bin/vme.log`. In-game DIL test zones:
`DIL_UNIT_TESTING.md`.
