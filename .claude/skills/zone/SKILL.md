---
name: zone
description: Writing or changing zone files (vme/zone/*.zon) - the %zone header, rooms and exits, doors and keys, objects (weapons, armour, containers, drinks, money...), monsters (%mobiles) and the %reset section that loads and equips them, plus the preprocessor and the macro headers in vme/include. Use with the vmc skill (compiling) and the dil skill (scripts).
---

# Writing zone files

`example.zon` next to this file is a complete zone (two rooms with a locked door, a key, a
weapon, armour, a container, a guard who is loaded, equipped and given the key) that
compiles with `vmc -l` and was tried in the game. Start from it.

Details per part, read when you need them:
- `rooms.md`: room fields, exits, doors, light, nested rooms.
- `objects.md`: object fields and the macro for each kind of object.
- `mobiles.md`: monster fields and templates, and the `%reset` commands.

The old manuals are `vme/doc/BasicZoneManual*` (sgml, pdf, html), `vme/doc/vmc.txt`,
`rooms.txt`, `objects.txt` and `monsters.txt`. They are useful for what each field means,
but out of date in many details (removed `SFUN_*` specials, `composed.h`, bonus ranges of
±25, money syntax, default reset mode). Where they disagree with this skill or with vmc, vmc
is right: the grammar is `vme/src/vmc/vmcpar.y`, the keywords `vme/src/vmc/vmctok.h`, the
checks `vme/src/vmc/vmc_process.cpp`. Real zones in `vme/zone/` show what works.

## The file

```
#include <macros.h>          // always: constants, wmacros.h, liquid.h, the monster templates

%zone example                // must be the file's name without .zon
title "Example Keep@Mainland"
lifespan 20                  // minutes between resets (default 60; 0: never, not even at boot)
reset RESET_ANYHOW           // default is RESET_NOT
creators {"papi"}
notes "Shown by wstat zone."

%rooms   ...  %objects  ...  %mobiles  ...  %dil  ...  %reset  ...

%end
```

- `%zone` comes first and `%end` last; the other sections are optional, in any order. An
  empty `%reset` is a syntax error. Keywords are lowercase and case-sensitive.
- `reset`: `RESET_NOT` (0, once at boot), `RESET_IFEMPTY` (when no player is in the zone),
  `RESET_ANYHOW` (every `lifespan` minutes), `RESET_NEVER` (not even at boot).
- `weather` is the base air pressure, 960..1040 (default 1000); lower is stormier.
- `title "Name@Continent"`: the `areas` command lists zones whose title has one `@`.
- A zone is loaded only if it has a line in `vme/etc/zonelist`. **A blank line ends that
  file**: zones listed after it are never loaded, without a word in the log.

Each unit is a symbol, its fields in any order, then `end`:

```
short_sword
names {"short sword", "sword"}
title "a short sword"
descr "A short sword lies here."
...
end
```

## Names and references

- Symbols: letters, digits and `_`, at most 15 characters (longer is cut, with only a
  warning). Rooms, objects and monsters share one namespace, without regard to case.
- A field keyword can't be a symbol (`door`, `key`, `level`, `time`...); a direction can't
  either, because `north`, `up`... are macros. Keywords are in `vmctok.h`.
- Refer to a unit as `sym` in its own zone, or `sym@zone` / `zone/sym`.
- **vmc doesn't check references** (exits, keys, `in`, `dilcopy`, resets). A wrong one shows
  up only at boot in `vme.log`: "Unknown room direction" (the exit is dropped), "Template ..
  not found", or a reset loading a "blob of green slime" in its place.
- `names` lists: no name may start with an earlier one (case-insensitive), so put the longer
  first: `{"keep guard", "guard"}`, never `{"guard", "keep guard"}`. No `'s` at the end, no
  name starting with `self`. The same goes for `extra` names and door keywords.

## Strings and the preprocessor

- A string is `"..."` and may span lines. **It can't contain `"`**, not even as `\"`; use
  `'`. An unclosed `"` swallows the rest of the file and the error appears at the end.
- The preprocessor is cpp: `#include`, `#define` with arguments, `\` continuation, `#if`.
  No variadic macros. `vmc -p` shows what a macro became.
- Commas inside `{}` split macro arguments: `NAMED({"a","b"})` breaks. Define the list as a
  macro of its own and pass that.
- Lowercase macros expand everywhere as a word: `north`...`southwest`, `up`, `down`,
  `pause`, `dilend`, `on_activation`.
- DIL in a zone needs `macros.h` (it defines `dilend`), and a `^` anywhere in DIL, even in a
  string, breaks the compile.

## Traps that compile without a word

- **The `%zone` name differs from the file name**, or a stray word in the header (it becomes
  the zone name): vmc accepts it, the server stops at boot.
- **The wrong kind of flags**: `flags` takes `UNIT_FL_*` only; an object's `OBJ_*` and a
  monster's `CHAR_*` go under `romflags`. vmc sees only numbers, so `flags {OBJ_NO_DUAL}` sets
  UNIT_FL_BURIED (same bit) and the item loads buried, and `flags {CHAR_SNEAK}` sets
  UNIT_FL_NO_WEATHER. `vme/bin/zoneflags.py` finds these (all zones, or the files you name).
- **Macros overwrite earlier fields**: `value[]`, `type`, `title` and `weight` keep the last
  value, so a field set before `WEAPON_DEF(...)` or `MONEY(...)` is lost. Put the type macro
  first. `manipulate`, `flags`, `romflags` and `open` add up instead.
- **`CONTAINER_DEF` doesn't let anything in**: add `MANIPULATE_ENTER`. **`ARMOUR_*` can't be
  worn** until you add a `MANIPULATE_WEAR_*` flag.
- **Doors are two exits**: give both sides their flags, keyword and key, and add a `door`
  reset per side, or a door opened once stays open forever.
- **A monster defaults to `max 1` in the whole game**: two `load guard into ...` lines load
  only one guard. Use `local 1` (per room) or `zonemax`.
- **What vmc only warns about**: alignment outside -1000..1000 (kept within, and fatal with
  `-l`), a negative weight on a monster, and more money on a monster than its level allows
  (never fatal; shown unless `-q`).

## Checking a zone

1. Compile with `-l` (vmc skill): warnings such as a cut name or a bad title become errors.
2. Add the zone to `vme/etc/zonelist` if it's new, restart `vme`, and read `vme.log` for the
   zone's name: dropped exits, missing templates, slime from a reset.
3. Log in and look: `goto <room>@<zone>`, `wstat zone <zone>`, `wstat <sym>@<zone>`, walk
   every exit, open every door, `look` at the monsters for their equipment.
