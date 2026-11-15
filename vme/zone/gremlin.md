# gremlin: review questions

## Step 1

1. **fn_01** (line 990): the carrot field is a 3x2 grid (fn_03 fn_04 fn_05 to the north, fn_01 fn_02
   fn_06 to the south), and every other exit fits it, but fn_01's east exit leads to fn_04 (the
   north-middle field) instead of fn_02. fn_02's west exit leads to fn_01, so walking west then east
   puts you somewhere else, and fn_04's west exit goes to fn_03. Should fn_01 have `east to fn_02`?
2. **fn_03** (line 1023): its description is a copy of fn_02's ("A strong fence opens to a wide gate
   here leading to the south. North, east, and west, carrots dot the ground."), but fn_03 is the
   north-west corner: exits only south and east, and the gate to the farm is at fn_02. Rewrite it
   along the lines of fn_04/fn_05 (fence to the north and west, carrots to the south and east)?
3. **pendant** (Gwydion's Amulet, line 2290): nothing in the game loads it. It has
   `dilcopy charger@sorcerer()` and `mana_drain@sorcerer()`, which only act while the item is
   wielded (`WEAR_WIELD`) and whose messages name "Vorous" (the sorcerer dagger); on a neck amulet
   they never fire. Is the amulet meant to be used (as a reward somewhere), and if so, should those
   two DILs go?
4. **gremlin_large, grumpkin_large, gaflaw, gguard, sventer**: vmc warns that they carry too much
   money for their level (1 silver on a level 9 gremlin, 1 gold on sventer). This is a newbie zone,
   so the extra may be on purpose; keep it?
