---
name: zone
description: Writing or changing zone files (vme/zone/*.zon) - the %zone header, rooms and exits, doors and keys, objects (weapons, armour, containers, drinks, money...), monsters (%mobiles) and the %reset section that loads and equips them, plus the preprocessor and the macro headers in vme/include. Use with the vmc skill (compiling) and the dil skill (scripts).
---

# Writing zone files

`example.zon` next to this file is a complete zone that compiles with `vmc -l` and was
played through in the game. Start from it. It has:
- three rooms, a locked door with its key, a weapon, armour, a container, a lamp;
- a steward with a whole quest from `quests.zon` (`qstSimpleQuest` and `qstItemsWanted`,
  string lists, a reward item that isn't saved), its names in `#define`s;
- a guard loaded and equipped by resets, carrying the key and random treasure, who blocks
  the vault until the quest is done and answers questions: two of the zone's own templates
  from `%dil`, with `fnpri`, `block`, `on_activation` and `secure`;
- a sword with `guild_restrict@function` (a string list of guild macros) and a hidden
  `$identify` extra.

Details per part, read when you need them:
- `rooms.md`: room fields, exits, doors, light, nested rooms.
- `objects.md`: object fields and the macro for each kind of object.
- `mobiles.md`: monster fields and templates, and the `%reset` commands.
- `quests.md`: the quest helpers, quest state, gates and chains, hidden extras.

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

## DIL on units

- `dilcopy tmpl@zone(args);` in a room, object or monster runs that template on it; a
  template of the same zone needs no `@zone`. The `;` is required.
- Arguments: numbers, strings, string lists `{"a", "b"}` (macros from the headers work
  inside: `{GUILD_UDG_FIGHTER, GUILD_PALADIN}`), and `null` for an empty list.
- vmc checks neither the template's name nor its arguments: a wrong one shows up only at
  boot or when it runs. Read the template's `dilbegin` line (and the comment above it) in
  its zone before calling it; `function.zon` holds the common ones (restrictions, guards,
  aggression, death rooms).
- A `dilbegin ... dilend` written inside a unit runs on that unit, without arguments. Its
  name is also a template of the zone, so it must be unique there.
- `wstat <sym>@<zone> func` in the game lists the DIL programs a unit runs.
- Writing the template itself, and what survives a `wait`: the dil skill.

## Strings and the preprocessor

- A string is `"..."` and may span lines. **It can't contain `"`**, not even as `\"`; use
  `'`. An unclosed `"` swallows the rest of the file and the error appears at the end.
- **Line breaks in a `descr` or `extra` are shown as written**; the game only wraps a line
  that's too long. Break the text yourself, each line under about 72 characters (a room's
  first line gets 3 spaces in front), as the real zones do; a longer line wraps and leaves a
  stray word on a line of its own.
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
- **Object flags under `flags`**: `flags {OBJ_NO_DUAL}` sets UNIT_FL_BURIED (same bit), so
  the item loads buried. Object flags go under `romflags`; `flags` takes `UNIT_FL_*` only.
- **Macros overwrite earlier fields**: `value[]`, `type`, `title`, `weight` and `romflags`
  keep the last value, so a field set before `WEAPON_DEF(...)` or `MONEY(...)` is lost. Put
  the type macro first. `manipulate`, `flags` and `open` add up instead.
- **`CONTAINER_DEF` doesn't let anything in**: add `MANIPULATE_ENTER`. **`ARMOUR_*` can't be
  worn** until you add a `MANIPULATE_WEAR_*` flag.
- **Doors are two exits**: give both sides their flags, keyword and key, and add a `door`
  reset per side, or a door opened once stays open forever.
- **A monster defaults to `max 1` in the whole game**: two `load guard into ...` lines load
  only one guard. Use `local 1` (per room) or `zonemax`.
- **`weight` over 2000, `alignment` over 1000 and too much money on a monster** aren't
  caught (weight is checked before it's set). `value[5]` and `exit[10]` crash vmc.

## Checking a zone

1. Compile with `-l` (vmc skill): warnings such as a cut name or a bad title become errors.
2. Add the zone to `vme/etc/zonelist` if it's new, restart `vme`, and read `vme.log` for the
   zone's name: dropped exits, missing templates, slime from a reset.
3. Log in and look: `goto <room>@<zone>`, `wstat zone <zone>`, `wstat <sym>@<zone>`, walk
   every exit, open every door, `look` at the monsters for their equipment.
