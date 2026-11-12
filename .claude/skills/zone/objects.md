# Objects (`%objects`)

## Fields

| field | |
|---|---|
| `names`, `title`, `descr`, `extra` | as for rooms; `descr` is what lies on the floor |
| `inside_descr "..."` | what you see inside it |
| `type ITEM_*` | usually set by a macro below |
| `manipulate {MANIPULATE_*}` | `TAKE`, `HOLD`, `WIELD`, `ENTER`, a `WEAR_*` for the slot |
| `flags {UNIT_FL_*}` | unit flags only |
| `romflags {OBJ_*}` | object flags: `OBJ_NO_UNEQUIP`, `OBJ_TWO_HANDS`, `OBJ_NOCOVER`, `OBJ_NO_DUAL` |
| `weight n`, `capacity n`, `height n` | pounds, what it holds, size in cm (default 180) |
| `cost`, `rent` | money: `cost 2 SILVER_PIECE, 4 COPPER_PIECE` (count, then coin; never a bare number) |
| `value[0..4] n` | type-specific; usually set by a macro |
| `open {EX_*}`, `key <obj>` | containers that close and lock |
| `light`, `bright` | -6..6 |
| `spell n` | magic resistance |
| `dilcopy f@zone(args);` | e.g. a restriction DIL |

Defaults: weight 1, capacity 0, type `ITEM_OTHER`.

`manipulate`, `flags` and `open` add up; every other field keeps the last value. So write the
type macro first and change things after it.

Numbers can be `+ - * / ( )`, and `{A, B}` ORs flags (`|` is a syntax error).

## The macros (`vme/include/wmacros.h`, `liquid.h`)

Bonuses (craftsmanship, magic) are -7..+7, as `BONUS_JUNK`..`BONUS_ARTIFACT`; the header
comments that say ±25 are old.

- **Weapon**: `WEAPON_DEF(WPN_x, craft, magic)` (sets TAKE and WIELD). `WPN_x` is a weapon, not
  a group. `WEAPONSZ_DEF(..., height)`. Add-ons after it: `WEAPON_SLAYER(race)`,
  `WEAPON_SLAYER_DRAGONS`.
- **Armour**: `ARMOUR_CLOTHES/LEATHER/HRD_LEATHER/CHAIN/PLATE(craft, magic)` or
  `ARMOUR_DEF(ARM_x, craft, magic)`, plus `ARMOURSZ_*(..., height)`. They set only TAKE: add
  `MANIPULATE_WEAR_<slot>`.
- **Shield**: `SHIELD_DEF(SHIELD_SMALL|MEDIUM|LARGE, craft, magic)`, `SHIELDSZ_DEF`.
  `SHIELD_TOWER` is rejected.
- **Container**: `CONTAINER_DEF(capacity)` sets only type and capacity: add
  `MANIPULATE_ENTER`, and `open {EX_OPEN_CLOSE, EX_CLOSED}` for a lid.
- **Drink**: `LIQ_WATER/BEER/WINE/ALE/MILK/TEA/...(weight, capacity, inside, poison)` or
  `LIQ_DEF(...)`; capacity -1 is endless.
- **Food**: `FOOD_DEF(amount, poison)`. **Light**: `LIGHT_DEF(hours, brightness)`.
- **Money**: `MONEY(GOLD_PIECE, coins)`; it sets the title, so give no `title`.
- **Magic**: `POTION_DEF` / `SCROLL_DEF(power, spell, spell, spell)`, `WAND_DEF` /
  `STAFF_DEF(power, charges, spell, spell)`; `SPL_NONE` for an unused spell (0 is a real
  spell).
- **Ranged**: `MISSILE_DEF(...)`, `RANGED_DEF(...)`. **Trap**: `TRAP_DEF(craft, magic)`.
- **Bonuses while worn**: `STR_TRANSFER(n)` .. `DIV_TRANSFER`, `HIT_TRANSFER`,
  `SKILL_TRANSFER(SKI_x, n)`, `WEAPON_TRANSFER`, `SPELL_TRANSFER` (-7..+7),
  `CHAR_FLAG_TRANSFER`, `SPEED_TRANSFER`, `PROTECTION_FROM_GOOD/EVIL`, `CURSED_OBJECT`.
- **Key**: no macro: `type ITEM_KEY`, `manipulate {MANIPULATE_TAKE, MANIPULATE_HOLD}`.
- **Board**: `type ITEM_BOARD`, no TAKE, `dilcopy board@boards(...)` (see `boards.zon`).
- **Restrictions** (`function.zon`): `level_restrict`, `guild_restrict`, and the ability,
  skill, weapon, race and quest variants.

## How weapons and armour work

`value[0]` is the weapon or armour kind, `value[1]` craftsmanship, `value[2]` the magic
bonus. Craftsmanship sets the item's hit points (about 755 to 2400); a `hit` field is
ignored. An item a monster is equipped with by a reset is resized to fit the monster.

## Traps

- `flags {OBJ_...}` sets the wrong bits with no error (OBJ_NO_DUAL loads the item buried,
  OBJ_TWO_HANDS makes it invisible). Object flags go under `romflags`.
- `WEAPON_SLAYER` before `WEAPON_DEF` is overwritten.
- A bonus outside -7..+7 is fatal and turns the item into trash.
- `value[0..4]`, `capacity` up to 32767 (-1 is endless) and `weight` 0..32767; vmc refuses
  others. A drink container weighs its own weight plus what's inside.
- `cost 25600`, `cost 5*GOLD_PIECE` and `money` on an object are syntax errors.
- Old names that don't exist: `ARMOR_*`, `UNIT_FL_NO_SAVE` (it's `UNIT_FL_NOSAVE`),
  `MANIPULATE_WEAR_WIELD` (it's `MANIPULATE_WIELD`), `container_def` in lowercase.
- A title starting with a capitalised filler word ("The rock") or ending in punctuation gives a
  warning.
