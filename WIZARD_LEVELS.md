# The god command ladder

Immortals (level 200 and up) get commands by level. Each rung adds to the ones below it, so a
level tells you what someone can do: watch the game, stage events, deal with disruptive
players, change players, run the game.

The levels are the `minlevel` of each command in `vme/etc/commands.def`; the names are in
`vme/include/vme.h`. A few commands also check the target (see "Guards").

| level | rung | adds |
|---|---|---|
| **200** `IMMORTAL_LEVEL` | **Watch**: see the game, change nothing | `where`, `users`, `wstat`, `jstat`, `cstat`, `corpses`, `wedit` (view a unit in the web client), `wiz` (the immortals' channel), `wizhelp`, `wizsocials`, `flick`, `mash`, `rock` |
| **210** | **Cosmetics** | `title` (another player's title) |
| **220** `GOD_LEVEL` | **Sport and competition** | `competition` |
| **230** `CREATOR_LEVEL` | **Game master**: stage the world, look after players' health | `wizinv`, `goto`, `at`, `echo`, `message`, `restore`, `makemoney`, `load` (NPCs and objects), `purge` (NPCs and objects), `eat` (any NPC or object), `set` on the world (NPCs, objects, rooms), `reset`, `wpath`, `boards` |
| **240** `MODERATOR_LEVEL` | **Disruptive players** | `nocomm`, `notell`, `petrify`, `freeze`, `ban` (a site), `broadcast` |
| **245** `PLAYER_ADMIN_LEVEL` | **Players**: change a player or what they carry | `set` on another player or anything they carry or wear, `purge` and `eat` of a player, `load <player>` (brings an offline player into the game), `transfer`, `force`, `switch`, `snoop`, `setskill`, `zap` (removes a DIL from a unit), `copy` (adds a DIL to a unit), `cdelete` (deletes a clan) |
| **250** | **Running the game** | `reboot`, `wizlock`, `advance` |
| **253–255** | **The server** | `nset` (the DIL `set`), `execute` (runs a shell command), `plog`, `crash`, `wtimewarp`, `shutdown`, and the `set` fields `add-info`, `del-info`, `access`, `profession`, `lifespan`, `lights`, `exp` |

## Guards

Some commands do world things at one level and player things at a higher one:

- **`set`** (`vme/src/modify.cpp`): another player, or a unit anywhere inside one (their
  inventory, equipment, containers), needs `PLAYER_ADMIN_LEVEL`, or the field's own higher
  level from the table in `modify.cpp`. Setting things on yourself and on the world follows
  the table's other columns, as before.
- **`purge`** (`do_purge`, `vme/zone/commands.zon`): a player needs `PLAYER_ADMIN_LEVEL`, and
  a level above theirs.
- **`eat`** (`do_eat`, `vme/zone/commands.zon`): a god eating something destroys it. Food:
  any god. Any other object or an NPC: `CREATOR_LEVEL`, like `purge`. A player:
  `PLAYER_ADMIN_LEVEL` and a level above theirs. Below that: "Your stomach refuses to eat
  that!?!".
- **`load`** (`do_load`, `vme/src/act_wizard.cpp`): loading a player needs
  `PLAYER_ADMIN_LEVEL`. Loading from a zone also needs that zone's load level (the 5th column
  of `etc/zonelist`), unless you're one of its creators.

## Helper agents

The in-game agents (`agents/`) are ordinary characters. An agent that runs events as a game
master is meant to sit at 230: it can watch, go anywhere unseen, narrate, heal, load and dress
NPCs, objects and rooms, and clean up, but it can't change a player beyond their health, and
it has none of the tools for disruptive players.

## Changing a level

Edit `minlevel` in `vme/etc/commands.def`, and `loglevel` with it: when it isn't 0, every use
of the command goes to the log as `CMDLOG`, shown to gods of that level or the user's, if
higher; `make all` rebuilds `commands.dat`, which is committed with it. Keep
this file and the in-game help in step.
