# Rooms (`%rooms`)

## Fields

| field | |
|---|---|
| `title "..."` | the room's name |
| `names {..}` | for `goto`/teleport, and what `enter` matches for a room inside a room |
| `descr "..."` | what `look` shows; vmc puts 3 spaces in front of it |
| `extra {names} "..."` | things to look at; `extra {"$get","statue"} "msg"` answers `get statue` |
| `movement SECT_*` | terrain, a plain constant (`SECT_FIELD+1` is a syntax error) |
| `flags {UNIT_FL_*}` | e.g. `UNIT_FL_INDOORS`, `UNIT_FL_NO_WEATHER`, `UNIT_FL_NOSAVE` |
| `light n` | -6..6, or the macros below |
| `in <room>` | puts this room inside another (no `;`) |
| `outside_descr "..."` | what the outer room shows of a room inside it (`""` hides it) |
| `gmap(x,y)` | map coordinates |
| `spell n` | magic resistance |
| `dilcopy f@zone(args);` | runs a DIL template on the room (the `;` is required) |

Defaults: `SECT_CITY`, light 0. `romflags` on a room is stored but nothing reads it.

Movement cost per step: INSIDE and CITY 1, FIELD 2, FOREST 3, HILLS 4, MOUNTAIN 6, DESERT,
SWAMP, UNDER_WATER and SNOW 8, SLUSH 6, ICE 10, WATER_SWIM 4 (needs swimming), WATER_SAIL 50
(needs a boat).

## Light

The room's `light` plus daylight (+1 day, 0 dawn and dusk, -1 night), unless the room has
`UNIT_FL_INDOORS`; below 0 it's dark. `movement SECT_INSIDE` alone doesn't keep the sun out.
Macros: `ALWAYS_LIGHT` (light 1), `IN_ALWAYS_DARK` and `OUT_DARK_NON_NOON` (-1),
`OUT_ALWAYS_DARK` (-2).

## Exits

```
north to vault open {EX_OPEN_CLOSE, EX_CLOSED, EX_LOCKED}
   key vault_key keyword {"oak door", "door"}
   descr "A heavy oak door blocks the way north.";
east to temple@udgaard;
```

- Directions: `north east south west up down northeast northwest southeast southwest`
  (macros for `exit[0]`..`exit[9]`).
- Then, in any order, without commas, ending with `;`: `to <room>`, `open {EX_*}`,
  `key <obj>`, `keyword {..}` (or one string), `difficulty 0..250`, `descr "..."`.
- Flags: `EX_OPEN_CLOSE` (it's a door), `EX_CLOSED`, `EX_LOCKED`, `EX_PICKPROOF`,
  `EX_HIDDEN`, `EX_CLIMB`, `EX_FALL_THIS_WAY`.
- `difficulty` is how hard `search` finds a hidden exit, or `climb` / swimming through it.
- The first `keyword` names the door in every message; a door without one is "the
  UNDEFINED".
- `descr` is what `look north` shows. Text for the far side goes in the target room as
  `extra {"$exit_descr"} "..."` (not the `EXDESCR` macro, which writes the wrong name).

## Doors

A door is two exits, one in each room. Give both the same flags, keyword and key. Opening
or unlocking one side also changes the other side. A door stays as players leave it until a
`door` reset sets it again, one side per line:

```
%reset
door hall NORTH {EX_OPEN_CLOSE, EX_CLOSED, EX_LOCKED}
door vault SOUTH {EX_OPEN_CLOSE, EX_CLOSED, EX_LOCKED}
```

The key is an object with `type ITEM_KEY`; unlocking looks for that object, by symbol, in the
player's inventory.

## Traps

- An exit without `to` keeps only its `descr`: its door flags, key and keyword are dropped.
- A missing `;` after an exit is reported at the next word (`Token: 'end'`).
- Old forms that are syntax errors: `exit north ...`, `ne`, `keywords`, `exit_descr "..."`,
  `EX_PICK_PROOF`, `SECRET_DOOR_DIFFICULTY`, `special SFUN_DEATH_ROOM`.
- Room DILs in `function.zon`: `death_room@function(tick, hp, "msg")` (tick in quarter
  seconds, at least 12), `force_move@function(tick, "room@zone!msg", bool)`,
  `safe_room@function()`.
- More than 300 rooms in a zone gives a warning.
