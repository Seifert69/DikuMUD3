---
name: vmc
description: Compiling zones with vmc, the zone compiler - vme/zone/*.zon (rooms, objects, NPCs, resets and their DIL) into .data/.reset files the server loads at boot. Use after changing a zone, when vmc reports an error, or when a change doesn't show up in the game. Writing zones is the zone skill.
---

# Compiling zones with vmc

```bash
cd vme/zone
../bin/vmc -m -d ../etc/ -I../include/ guilds.zon     # one zone
../bin/vmc -m -d ../etc/ -I../include/ *.zon          # every changed zone
```

`make zone_files` from the repository root does the same through cmake. `vmc` is built
into `vme/bin/` by `make all`.

## Options

| | |
|---|---|
| `-m` | compile only zones whose `.zon` is newer than their `.data` and `.reset` |
| `-s` | check only, write no files |
| `-l` | warnings are fatal |
| `-q` / `-v` | quiet / verbose |
| `-p` | print the preprocessed zone to stdout, to see what a macro became |
| `-d ../etc/` | where the money file and other definitions are |
| `-I../include/` | where `#include <macros.h>` and the other headers are |

## Traps

- **`-m` only looks at the `.zon` file's own time.** A change to a header in `vme/include/`
  or to an included file doesn't make the zone recompile: leave out `-m`, or `touch` the zone.
- **vmc stops at the first zone with errors.** With `*.zon`, the zones after it are not
  compiled. Fix it, or compile the others by name.
- **A failed compile keeps the old `.data` and `.reset`**, and the server boots them without
  a word. Check that vmc exited with status 0 (or that the `.data` is newer than the `.zon`)
  before testing; otherwise you test the previous version.
- **The server reads zones only at boot**: restart `vme` after compiling.
- **A clean compile doesn't mean the zone works.** vmc checks no references (exits, keys,
  `dilcopy`, resets) and lets a `%zone` name that differs from the file name through; the
  server finds those at boot (zone skill, "Checking a zone").

## Errors

Errors go to stderr, with line numbers of the `.zon` file, and vmc exits with status 1.
Errors found after parsing (bad values, sums, names) also go to `<zone>.err`; a syntax
error doesn't, so read stderr. `-s` still writes the `.err` (and a `.dh` for zones with DIL). A DIL error looks like:

```
718: syntax error
    Token: 'name'
guilds.zon: 829: DIL not compiled properly.
```

The first number is the line with the error; the second is where vmc gave up on that DIL.
`Token: '<word>'` on a variable is usually a reserved word (dil skill).

## DIL warnings

vmc also warns about DIL that compiles but doesn't do what it says (vmc/dilcheck.h), one line
each after the template, `WARNING: <file>: <line>: DIL <template>: ...`:
- a unitptr used after a `wait`/`pause` that set it before and doesn't `secure` it, or an
  extraptr used after any wait (the server clears them when the program wakes up);
  `secure(x)` when `x` was cleared that way, or before `x` is set at all;
- `activator`, `medium`, `target` or `argument` after a `pause` (they belong to the tick);
- an unsecured unitptr or an extraptr in a `wait()`/`interrupt()`/`on_activation()` condition
  (checked at the next wait, and dropped if the variable is secured anywhere in the template);
- `act()` with a null character, or a null victim for TO_VICT/TO_NOTVICT: nobody sees it;
- `exec("kill"+name, ...)`: no space after the command.

The checks follow the text: each branch of an `if` starts from the state before it and the ways
are joined after it, leaning towards no warning; code right after a jump isn't checked; labels
and loops forget what was known. So a warning is almost always real; code reached by a jump
isn't checked, nor are DIL calls and `waitnoop` that pause. `-l` turns them into errors.

## Zone files

How to write them (header, rooms, objects, monsters, resets, macros) is in the zone skill.
Leftovers of a compile (`.err`, `.dh`, `.dot`) and the output (`.data`, `.reset`) are not
committed.
