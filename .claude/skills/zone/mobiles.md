# Monsters (`%mobiles`) and resets (`%reset`)

## Monster fields

Start from a template (below), then set what differs:

| field | |
|---|---|
| `names`, `title`, `descr`, `extra` | `descr` is shown when it stands in a room |
| `level n` | 0..199 |
| `race RACE_*`, `sex SEX_MALE|SEX_FEMALE|SEX_NEUTRAL` | |
| `alignment n` | -1000..1000 |
| `money 3 COPPER_PIECE, 2 IRON_PIECE` | count, then coin |
| `exp n` | XP share, -500..499, default 100; 0 gives none |
| `height`, `weight` | cm and pounds (defaults 180, 120) |
| `speed n` | 4..200, default 12, lower is faster |
| `attack WPN_*`, `armour ARM_*` | natural attack and armour; see `NATURAL_DEF` |
| `offensive n`, `defensive n` | -1000..1000; see `ATTACK_DEFENSE` |
| `ability[ABIL_x] n`, `weapon[WPN_x] n`, `spell[SPL_x] n` | see the sums below |
| `position`, `romflags {CHAR_*}`, `npcflags {NPC_NICE_THIEF}` | |
| `dilcopy f@zone(args);` | behaviour: aggression, combat magic, shops... |

`hit` and `mana` are ignored: hit points come from the HP ability.

**The sums vmc checks**: the 8 abilities add up to 100; the weapon groups and spell groups
together add up to 100. The defaults already do (abilities STR 40 DEX 30 CON 10 CHA 2 BRA 3
HP 15; weapon UNARMED 100), so one `weapon[WPN_SWORD] 50` on its own makes 150 and fails.
Change whole sets: `MSET_ABILITY(str, dex, con, hpp, bra, cha, mag, div)`,
`MSET_WEAPON(axe_ham, sword, club_mace, pole, unarmed, special)`,
`MSET_SPELL(div, pro, det, sum, cre, min, hea, col, cel, int, ext)`. After a template, a new
set must keep the template's split: `M_HUMAN_WARRIOR_SWORD` puts 15 in spells, so an
`MSET_WEAPON` after it adds up to 85.

## Templates

`#include <macros.h>` brings them all in:

- People (`mhumanoid.h`): `M_AVG_<RACE>(level, sex)`,
  `M_<RACE>_WARRIOR_<AXE|SWORD|CLUB|POLE|NATURAL|SPECIAL>(level, sex)`, the same with
  `THIEF`, `M_<RACE>_MAGE_<FIRE|FROST|ACID|ELECTRICITY|MIND|GAS>(level, sex, "spell")`,
  `M_<RACE>_CLERIC_<HAMMER|CLUB|STAFF|SWORD>(level, sex, "attack", "defence")`,
  `M_SHOP_KEEPER(level, sex, race)`. Children: the same with `C_` (`chumanoid.h`).
- Creatures (`monster.h`, `fantasy.h`, `plant.h`, `aquatic.h`):
  `M_<CREATURE>_<TINY|SMALL|MEDIUM|LARGE|XLARGE|GODLY>(level, sex)`; `_MAD` adds aggression
  and poison.
- Grep the headers for the exact name before using one. A few dozen templates no one uses
  don't compile (e.g. `M_KOBOLD_THIEF_*`, `M_IVY_*`, `M_SPRITE_MEDIUM`, some `C_` giants).
- Helpers: `NATURAL_DEF(attack, armour)`, `ATTACK_DEFENSE(off, def)`,
  `SETWEIGHT(level, min, max)`, `SETHEIGHT(level, min, max)`.
- Old specials (`special SFUN_AGGRESSIVE`, `SFUN_COMBAT_MAGIC`...) are gone; use the
  `dilcopy` functions in `function.zon` that real zones use.

## Resets

```
load guard into hall local 1
{
   equip short_sword position WEAR_WIELD
   equip leather_cap position WEAR_HEAD
   load vault_key                       // into the guard's inventory
}
load sack into vault local 1
door hall NORTH {EX_OPEN_CLOSE, EX_CLOSED, EX_LOCKED}
```

| command | |
|---|---|
| `load X [into ROOM] [max n] [zonemax n] [local n] [complete]` | a monster or object |
| `equip OBJ position WEAR_x [max n] [complete]` | only inside a monster's block |
| `follow NPC [max n] [zonemax n] [local n] [complete]` | loads it next to the block's monster, following it |
| `door ROOM DIR {EX_*}` | sets one side of a door |
| `purge ROOM` | removes everything but players from the room |
| `remove X in ROOM` | removes every copy of X in the room |
| `random PCT`, `nop` | for blocks: run PCT% of the time, or always |

- **A `{ }` block runs only if its command loaded something**, and works on that unit:
  `load` inside a monster's block goes into its inventory, inside a container's into the
  container. If the guard is still alive from the last reset, his block doesn't run, so a
  looted guard stays empty until he dies.
- **Limits**: `max` counts copies in the whole game, `zonemax` in this zone, `local` in the
  target room (or container). With none given, **a monster is `max 1` world-wide** and an
  object `local 1`. Equip skips a slot that's already filled.
- **`complete`**: if a command directly in its block fails, the unit is removed again. `door`,
  `purge` and `remove` always count as failing, and so does a nested `load` stopped by its
  limit: keep those out of a `complete` block.
- `into` takes only a room. Inside `random`/`nop` blocks always give `into`.
- `local` on `follow` counts the leader's inventory, so it never limits anything.
- vmc doesn't check reset references; at boot a missing unit is loaded as a blob of slime and
  logged.

## Traps

- `money 5*GOLD_PIECE` and `money 25600` are syntax errors.
- `attack WPN_CRUSH_LARGE` / `_HUGE` are out of range (fatal).
- `CHAR_HIDDEN` doesn't exist (it's `CHAR_HIDE`); `NPC_AGGRESSIVE` doesn't exist (use the
  aggressive DIL).
- `default` position doesn't decide whether `descr` is shown: `look` shows `descr` only for a
  standing monster, otherwise its title and position.
- Too much money for the level and `alignment` over 1000 aren't caught.
