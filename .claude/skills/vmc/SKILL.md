---
name: vmc
description: Compiling zones with vmc, the zone compiler - vme/zone/*.zon (rooms, objects, NPCs, resets and their DIL) into .data/.reset files the server loads at boot. Use after changing a zone, when vmc reports an error, or when a change doesn't show up in the game.
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

## Errors

Errors go to stderr, with line numbers of the `.zon` file, and vmc exits with status 1.
Errors found after parsing (unknown units, bad values) also go to `<zone>.err`; a syntax
error doesn't, so read stderr. A DIL error looks like:

```
718: syntax error
    Token: 'name'
guilds.zon: 829: DIL not compiled properly.
```

The first number is the line with the error; the second is where vmc gave up on that DIL.
`Token: '<word>'` on a variable is usually a reserved word (dil skill).

## Zone files

A zone is `#include <macros.h>`, then the sections `%zone`, `%dil`, `%rooms`, `%objects`,
`%mobiles`, `%reset` and `%end`. Units are named `symbol@zone` from other zones. Leftovers
of a compile (`.err`, `.dh`, `.dot`) and the output (`.data`, `.reset`) are not committed.
