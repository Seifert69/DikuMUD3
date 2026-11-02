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
| **230** `CREATOR_LEVEL` | **Game master**: stage the world, look after players' health | `wizinv`, `goto`, `at`, `echo`, `message`, `restore`, `makemoney`, `load` (NPCs and objects), `purge` (NPCs and objects), `eat` (any NPC or object), `set` on the world (NPCs, objects, rooms; `set room dir-to <direction> <room@zone>` makes an exit, `none` removes it), `force` (an NPC), `switch` (into an NPC; `switch` alone returns), `gmmem` (the game master's memory of an NPC, object or room), `gmtrack` (follow a player), `gmroom` (rooms made while the game runs), `reset`, `wpath`, `boards` |
| **240** `MODERATOR_LEVEL` | **Disruptive players** | `nocomm`, `notell`, `petrify`, `freeze`, `ban` (a site), `broadcast` |
| **245** `PLAYER_ADMIN_LEVEL` | **Players**: change a player or what they carry | `set` on another player or anything they carry or wear, `purge` and `eat` of a player, `load <player>` (brings an offline player into the game), `force` of a player or `force all`, `transfer`, `snoop`, `setskill`, `zap` (removes a DIL from a unit), `copy` (adds a DIL to a unit), `cdelete` (deletes a clan) |
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
- **`force`** (`force`, `vme/zone/commands.zon`): an NPC needs `CREATOR_LEVEL`; a player, or
  `force all`, needs `PLAYER_ADMIN_LEVEL`. `switch` only ever goes into NPCs.
- **`load`** (`do_load`, `vme/src/act_wizard.cpp`): loading a player needs
  `PLAYER_ADMIN_LEVEL`. Loading from a zone also needs that zone's load level (the 5th column
  of `etc/zonelist`), unless you're one of its creators.

## Helper agents

The in-game agents (`agents/`) are ordinary characters. An agent that runs events as a game
master is meant to sit at 230: it can watch, go anywhere unseen, narrate, heal, load and dress
NPCs, objects and rooms, and clean up, but it can't change a player beyond their health, and
it has none of the tools for disruptive players. It can follow a player with `gmtrack`, which
changes nothing about them. Exits made with `set … dir-to` don't enter
the path finder's map (`wpath`, auto paths) until the next reboot.

## The game master's memory

`gmmem` (in `vme/zone/commands.zon`) keeps what a game master needs to remember about the NPCs,
objects and rooms of its events, in extras on the unit:

- `$gm_brief`: who or what it is, what it knows and may reveal (`gmmem <unit> brief <text>`,
  `brief+` to add to it);
- `$gm_comms`: what players said and did with an NPC: says, asks, emotes, tells, gifts, fights,
  the last 40 lines. `gmmem <npc> watch` copies `gm_comms@commands` onto the NPC to record
  them; it also sends each line at once to every game master online (a god with `$gm` in its
  info), as `[gm <npc>] <time> <what happened>`, and reports the NPC's death. On an object,
  `watch` (`gm_objwatch@commands`) records who gets, drops, puts, steals, gives or sells it;
  give and sell are told as the player types them, since the object hears nothing more once
  it's in someone else's inventory;
- `$gm_told`: what it has revealed, and to whom (`gmmem <unit> told <text>`);
- `$gm_event`: the event it belongs to (`gmmem <unit> event <id>`);
- `$gm_greet`: a tripwire on an NPC or room (`gmmem <unit> greet [line]`, which copies
  `gm_greet@commands` onto it). When a player walks in (a direction, `enter`, `leave`, `exit`;
  not goto, teleport, recall or summon), the game masters hear `[gm <unit>] <time> <player>
  walks in`, and the line plays once for each player: an NPC does it as a command (`say …`,
  `emote …`), a room shows it to everyone there; `$1n` is the player. With no line, it only
  tells. It reacts at once, without waiting for the game master;
- `$gm_for`: the players a tripwire is for (`gmmem <unit> for <name> …`, nothing to clear it);
  others walk in unnoticed.

`gmmem <unit>` shows them all; `gmmem <unit> clear [brief|comms|told|event|greet|for]` removes
them (`clear` alone, all of them; clearing `greet` also stops the tripwire). A unit is an NPC or
object by name, `room` for the room you're in, or `sym@zone`. Players are never a game master's
to remember.

## Following a player

`gmtrack <player> <zone …|any> [hours]` copies `gm_track@commands` onto a player, which tells the
game masters, as `[gm <player>] <time> …`:

- when the player enters or leaves one of the zones (`any`: every change of zone; a room made
  inside another counts as the outer room's zone);
- when they die, or fall below a third of their hit points (once, until they're above half);
- when someone starts following them;
- when they've been idle 10 minutes, and when they're back;
- when they leave the game.

Nothing about the player changes. A move by magic (goto, teleport, recall, summon) is noticed
at the player's next command or within the minute. The tracker removes itself after `hours`
(2 by default, at most 6), when the player leaves the game (`who_leave@basis`), or with
`gmtrack <player> off`; saved with a player all the same, it finds itself expired at the next
login. `gmtrack` alone lists who is tracked.

## Rooms made while the game runs

`gmroom` (C++, `act_wizard.cpp`) makes rooms in the virtual zone `_gm`, which, like `_players`,
has no files and exists from boot:

- `gmroom new <keyword> [in <room@zone>|here]` makes `gm1@_gm`, `gm2@_gm`, ... (a name isn't used
  twice before the reboot). With `in`, the room stands inside that room, like the barrel to the
  thieves' guild: players see "A <keyword> is here." and `enter <keyword>` and `leave` it, and a
  room may stand inside another made one. Without `in`, give it exits with `set room dir-to`.
  It's an indoor room, lit, with room for players to enter (capacity 30000, as vmc gives rooms).
- Only add exits in directions a room doesn't use yet: `set room dir-to` replaces an existing
  exit (the temple's stairs down to the crypt, say), and freeing the made room then removes it,
  so the old way is gone until the next reboot.
- `gmroom title|descr|outside <gm1|here> <text>` sets, in one line, its title, what players see
  inside it, and how it shows in the room around it ("A striped tent stands here."). It only
  changes made rooms. (`set` has no field for a room's inside description: `set … descr` edits
  the outside one, in the line editor, ended with `@`.)
- `gmroom free <gm1|gm1@_gm|here>` moves everyone and everything inside (players, their corpses
  and belongings, NPCs) to the room it stands in, else to where its first exit leads, else to
  the crypt in Udgaard, removes every exit that leads to it, and deletes it. A room with a made
  room inside is freed after that one. It moves an event's own NPCs and objects out too:
  purge those first.
- `gmroom list` shows them, with where each stands and how many players are inside.
- At most 100 at a time. They're gone at the next reboot: a player saved in one comes back in
  their hometown, and a stored player corpse whose room is gone comes back in the crypt in
  Udgaard (`reload_corpse@death`).
- NPCs don't find their way into them: rooms made after boot aren't in the path finder's map,
  and `wpath` or a DIL `pathto` to one says impossible. Players walk in and out as anywhere.

## Changing a level

Edit `minlevel` in `vme/etc/commands.def`, and `loglevel` with it: when it isn't 0, every use
of the command goes to the log as `CMDLOG`, shown to gods of that level or the user's, if
higher; `make all` rebuilds `commands.dat`, which is committed with it. Keep
this file and the in-game help in step.
