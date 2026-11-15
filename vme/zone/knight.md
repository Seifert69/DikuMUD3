# knight: review questions

## Step 1

1. **nokill** (line 65): ejects the player to `findroom("sth1@sewers")`, but there is no zone
   `sewers`; room_1's exit goes to `sth1@midsewers`. So `link()` gets null and the player is not
   moved. Change it to `sth1@midsewers` (DIL, step 2)?
2. **room_3** (line 123): "The hall continues in all directions except to south where a large
   tapestry hangs", but there is an exit south to room_5 (the Master's Chambers). Is the door
   behind the tapestry (then the text could say so), or should the text change?
3. **hilary** (line 396): titled "Maid Hilary", but names and descriptions say "Lady Hilary".
   Which one? (If "Maid", her names need "maid".)
4. **hilary** (line 405): `M_HUMAN_MAGE_FIRE(130, SEX_FEMALE, "cast lava flow")`; the other
   teachers give a plain spell name ("magma", "hellfire"). Does "cast lava flow" work?
5. **avenger** (line 559): `$improved identify` says strength +1; the sword gives Str +2, Div +2 and
   Cha -1. Text or stats?
6. **night_cross** (line 786): the identify says the pendant increases Constitution; it only has
   `PROTECTION_FROM_GOOD` (same as paladin_guild's `pal_cross`).
7. **room_1** (line 93): `movement SECT_FIELD` for an indoor room; room_2 has no movement at all.
   Should both be `SECT_INSIDE`?
