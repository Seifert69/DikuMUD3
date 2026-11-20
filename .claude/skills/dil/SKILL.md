---
name: dil
description: Writing or changing DIL, the MUD's script language, in vme/zone/*.zon - NPC and object scripts, quests, player commands written in DIL (commands.def "func ="), helper functions called as name@zone. Covers the reference to use, the traps that the manual gets wrong or leaves out, and how to try a change. Use the vmc skill for compiling.
---

# Writing DIL

DIL lives in the `%dil` section of a zone (`vme/zone/<zone>.zon`) or inside a unit's
definition. `example.zon` next to this file is a small zone whose DIL was played through in
the game: a gatekeeper who blocks a way until a token is given (`fnpri`, `block`, a
`SFB_DONE` give handler with `secure`, a hidden quest flag), a herald who hears of it by
message (`sendto`, `interrupt`, `priority`, `heartbeat`) and announces it, and an `answer`
template both use (`on_activation`, `secure`). Start from it. A template:

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

- The DIL manual is on the wiki: <https://wiki.dikumud.com/wiki/Manual:DIL_Manual>, with a
  page per function, field and keyword, checked against the compiler and the interpreter
  (November 2026; every example compiles). Read a page with
  `python3 deploy/wiki.py get "Manual:DIL Manual/<name>"`, e.g. `addaff()`, `exit to`,
  `integer`; the chapters (Operators, DIL Constructs, Messages...) and the Full Listing are
  there too.
- When the manual and the code disagree, the code is right: keywords in
  `vme/src/vmc/diltok.h`, the grammar in `vme/src/vmc/dilpar.y`, functions in
  `vme/src/dilexp.cpp`, fields in `vme/src/dilfld.cpp`, statements in `vme/src/dilinst.cpp`.
- Working examples: grep `vme/zone/*.zon` for the function you need. `quests.zon` and
  `randomt.zon` hold helper libraries.

## Traps

- **Reserved words.** Every function, field and built-in variable in `diltok.h` is reserved,
  so `name`, `names`, `key`, `npcs`, `unset`, `target`, `length`, `title`, `level`, `race`,
  `extra`, `help`, `full`, `fname`, `offensive`... can't be variable or parameter names (a parameter fails at the `dilbegin`
  line). vmc says `718: syntax error` / `Token: 'name'` at the `var` line, or only further
  down, at some statement well after the variables (`Token: 'if'`). Use
  `pname`, `namex`, `kw`, `u`.
- **Template names are 15 characters at most** (`Name of dilprogram ... was too long`).
- **No field on a call's result**: `findroom(s).title` is a syntax error; put the unit in a
  variable first. Likewise `getword(s);` alone is one: write `w := getword(s);` (it also takes
  the word, and the spaces after it, off the variable `s`).
- **No `\"` in a string**: vmc stops at it. Use single quotes inside the text.
- **Strings**: `replace(old, new, text)` replaces every `old`; `left`, `right` and `mid` cut;
  `split(text, sep)` trims the spaces around each piece, so it can't put text back together.
- **`addextra(u.extra, {"$x"}, s)` adds nothing when `s` is a string that was never set**
  (null, e.g. `sl.[5]` past the end of a list): the text must exist. A literal `""` is fine.
- **`"words" in a stringlist`** is the position plus one (0 when absent) of the first entry
  that is the string or its first words, ignoring case: `"arrived"` doesn't match
  `"arrived_port"`, but `"ship arrived"` matches `"ship"`.
- **A unit saved with `store()` keeps its DIL programs, but their variables only while the
  template's variables stay the same**: after an edit that changes them (a variable added or
  removed), `restore()` starts every variable at 0 / "" / empty, the template's arguments too
  (`bread_dil` in db_file.cpp, "Var CRC mismatch"). Keep what a stored unit's program needs (a
  limit, an owner) in an extra on the unit, never in an argument (`house_chest@housing`).
- **`store(u, f, FALSE)` saves only what is in `u`, never `u` itself, and with nothing in it
  removes `f`** (`basic_save_contents` in reception.cpp). To keep a single unit (a bed, a chest),
  `store(u, f, TRUE)`; move out an NPC that is in it first, or it is saved inside.
- **`delstr(f)` only deletes what `savestr` wrote** (lib/dilfiles/strings); a unit saved with
  `store()` lies in lib/dilfiles/units and goes with `delunit(f)`. Both return an integer that
  must be assigned, like `transfermoney`: a bare call is a syntax error.
- **`dilcopy("tmpl@zone(...)", u)` from DIL takes only string and integer arguments**, cut at
  every comma: no lists, and no commas inside a string. A template with list arguments can
  only be copied in a zone file: `dilcopy tmpl@zone({"a", "b"}, {1, 2});`, where an empty
  `{}` fits either kind of list.
- **`x := {};` in code doesn't compile** (`Assigning incompatible types (C)`): an empty `{}`
  only works as a zone-file argument. To empty a list, `sl := null;`.
- **Unit and extra variables are cleared at every activation**: after each `pause` or `wait`,
  every `unitptr` and `extraptr` variable that isn't `secure`d is null (`dil_clear_non_secured`
  in dilrun.cpp), wherever the unit is. Find the unit again after the pause
  (`findroom`, `findsymbolic`), or `secure` it.
- **`and` / `or` evaluate both sides**, no short-circuit: `(u != null) and (u.level > 5)`
  still reads `u.level`. That read fails, and `and`, `or` and `not` count a failed value as false
  (`dilfe_land`/`dilfe_lor`/`dilfe_not` use `dil_getbool`), so `(u == null) or (u.level > 5)` is
  true when `u` is null: such guards work. Only the bitwise `&` and `|` fail as a whole when a side
  fails (`dilfe_and`/`dilfe_or`).
- **A field of null fails, and a failed value is false in every comparison**: `u.level > 5`
  and `u.level <= 5` are both false when `u` is null. Test `if (u == null)` on its own first.
- **String comparisons ignore case**, `<` and `>` too; `strcmp()` is exact.
- **Locals start empty**: null, 0 or "". No need to set them first.
- **Running off the end of the code starts it again at the top**, not ends it: vmc appends
  `goto 0` to every template (the `program` rule in dilpar.y). Variables keep their values;
  unsecured unitptrs are null as after any wait. Only `quit` (or destroying the unit) ends a
  program. So a missing `goto start` re-runs the code above `:start:` (heartbeat, variable
  setup), it doesn't silence the program. Running `interrupt()` again doesn't add a second one:
  `dil_intr_insert` replaces the interrupt with the same label.
- **A failed value assigned**: an integer variable keeps its old value, a string variable
  becomes `""` (`dilfi_ass`). Test the source (`if (u == null)`) rather than the result.
- **`return (null);` doesn't compile** in a `unitptr` function (`return expression not of
  correct type`): return a `unitptr` variable that is null.
- **`and` / `or` bind as in C** since #603 (looser than the comparisons, `or` looser than
  `and`), but `&` and `|` still bind tighter than the comparisons. Zones compiled with an older
  vmc kept the old meaning (`a == 1 and b == 2` was `(a == (1 and b)) == 2`).
- **vmc warns** about pointers used after a wait without `secure`, `activator` & co. after a
  `pause`, unsecured pointers in wait/interrupt conditions, `act()` that reaches nobody and
  `exec("kill"+name)` (vmc skill, "DIL warnings"): fix them, they are almost always real.
- **`MEDIA_*` and `TAR_*` from `spelldef.h` can't be used in DIL** (they're written with `<<`,
  a syntax error); `spelldef.h` isn't in `macros.h`, so `SPLCST_*` need
  `#include <spelldef.h>`.
- **`{...}` takes only constants**: `{"a", MACRO}` works, `{flag}` with a variable is a
  syntax error. Build such a list with `addstring(sl, flag)`.
- **`pause` waits for the unit's next tick** (`wait(SFB_TICK | SFB_RANTIME, TRUE)` in
  `wmacros.h`): up to one heartbeat, or 0.5 to 1.5 heartbeats when the heartbeat was just
  changed (`SFB_RANTIME` spreads the new timer). With `heartbeat := PULSE_SEC * 90` a `pause`
  can take 90 seconds, and under `priority` the unit's other programs wait all that time.
  Shorten the heartbeat around the pauses, as the herald in `example.zon` does.
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

  `pc_head()` is the first player (null when none is on), `npc_head()` the first NPC,
  `obj_head()` the first object and `room_head()` the first room, each with `.gnext` to go on.
- **Moving an NPC through an exit**: `exec("<dir>", self)`, then check `self.outside`, as
  CLAUDE.md says; `link()` only where no exit exists.
- Text to a player is `sendtext("...<br/>", u)` (lines end with `<br/>`) or `act(...)` with
  its `$1n`-style fields. `log("...")` writes to `vme.log`.
- **A loop over a room's units that moves or kills them: use `foreach`.** In a `while` over
  `u.next`, after `link(u, elsewhere)` the next unit is the other room's, and a `continue` that
  skips the step loops on the same unit until the server kills the DIL ("had endless loop").
  `foreach` works from a snapshot and skips units that have died or left; it also lists what is
  inside transparent units, so test `u.outside == self` for the room's own
  (`watercurrent@basemove`).
- **Quests: use quests.zon's functions**, not hand-made extras: `qstSetOngoing@quests` (the
  "<name> Ongoing" entry and the "New Quest" banner), `qstOngoing@quests` / `qstComplete@quests`
  for the names, and on completion the Ongoing removed and "<name> Complete" added, as
  `qstOnComplete` does (or `qstSimpleQuest` / `qstItemsWanted` for a whole quest). Players then
  see every quest the same way in `quests`.
- **A DIL function's result can't be another DIL function's argument**: `yule_q(lvl,
  yule_count(pc))` fails with "Incompatible argument types in argument 1" (whatever argument it
  is). Put the result in a variable first. Built-ins like `itoa(f(x))` are fine.
- **In a zone file with CRLF line endings, a `#define` can't continue over lines**: the `\` is
  followed by `\r`, not the newline, and vmc reports a syntax error at the `#define`. Write it on
  one line (holiday22.zon is such a file; keep its CRLF when editing).
- **A `dilbegin` inside a unit's block belongs to that unit**: vmc copies it onto the unit at
  every load, with no arguments. A template that rooms or other units `dilcopy` goes in the
  zone's `%dil` section; otherwise the boot logs "Template ... had mismatching argument count".

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
- `string`, `stringlist`, `integer` and `intlist` variables keep their values, and `self`
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
- `secure` takes a variable, never `self` ("Arg 1 of 'secure' not a user-variable"): a player's
  own command program doesn't need it, since it ends when the player leaves the game.
- Secures belong to the call that made them and end when that template returns.
- The label is where to give up gracefully: `:lost:` and go back to waiting.
- **`destroy()` on a secured unit fires its secure**: the program jumps to the label at once
  and skips the rest. `unsecure(u)` before `destroy(u)` (the gatekeeper in `example.zon`; seen
  in the game).

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
  queued); the usual one is asleep or fighting (see `answer` in `example.zon`). With a label instead of `skip`, the jump clears it; set it again after the
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

**Waiting for game time**: a god's `wtimewarp <seconds>` runs that many seconds of game ticks
without pausing between them, and the mud clock (`mudhour`) follows the ticks. Something that
waits for an hour of the day, a ship's timetable or a long `heartbeat`, can be tried in a minute
instead of hours: a mud hour is 300 seconds (`SECS_PER_MUD_HOUR`), so `wtimewarp 3600` is 12 mud
hours. Locally the god is the `immortal_name` from `vme/etc/server.cfg` (Papi).
