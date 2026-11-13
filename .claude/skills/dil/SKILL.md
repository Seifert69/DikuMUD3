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
  so `name`, `names`, `key`, `npcs`, `unset`, `target`, `length`, `title`, `level`... can't
  be variable names. vmc says `718: syntax error` / `Token: 'name'` at the `var` line. Use
  `pname`, `namex`, `kw`, `u`.
- **Template names are 15 characters at most** (`Name of dilprogram ... was too long`).
- **No field on a call's result**: `findroom(s).title` is a syntax error; put the unit in a
  variable first. Likewise `getword(s);` alone is one: write `w := getword(s);` (it also takes
  the word, and the spaces after it, off the variable `s`).
- **No `\"` in a string**: vmc stops at it. Use single quotes inside the text.
- **Strings**: `replace(old, new, text)` replaces every `old`; `left`, `right` and `mid` cut;
  `split(text, sep)` trims the spaces around each piece, so it can't put text back together.
- **`addextra(u.extra, {"$x"}, "")` adds nothing**: an extra made in DIL needs some text.
- **`"words" in a stringlist`** is the position plus one (0 when absent) of the first entry
  that is the string or its first words, ignoring case: `"arrived"` doesn't match
  `"arrived_port"`, but `"ship arrived"` matches `"ship"`.
- **`dilcopy("tmpl@zone(...)", u)` from DIL takes only string and integer arguments**, cut at
  every comma: no lists, and no commas inside a string. A template with list arguments can
  only be copied in a zone file: `dilcopy tmpl@zone({"a", "b"}, {1, 2});`, where an empty
  `{}` fits either kind of list.
- **`x := {};` in code doesn't compile** (`Assigning incompatible types (C)`): an empty `{}`
  only works as a zone-file argument. To start a list over, build a fresh one with
  `addstring`, or keep a plain string and `split` it.
- **Unit and extra variables are cleared at every activation**: after each `pause` or `wait`,
  every `unitptr` and `extraptr` variable that isn't `secure`d is null (`dil_clear_non_secured`
  in dilrun.cpp), wherever the unit is. Find the unit again after the pause
  (`findroom`, `findsymbolic`), or `secure` it.
- **`and` / `or` evaluate both sides.** No short-circuit: `(u != null) and (u.level > 5)`
  still reads `u.level`.
- **A field of null fails, and a failed value is false in every comparison**: `u.level > 5`
  and `u.level <= 5` are both false when `u` is null. Test `if (u == null)` on its own first.
- **String `==` and `!=` ignore case**; `<` and `>` don't.
- **Locals start empty**: null, 0 or "". No need to set them first.
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
