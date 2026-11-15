# hallows23: review questions

## Step 1

1. **tot_bag2 / tot_bag4 / stuff_name** (lines 169, 472, 655): the bags and the item list load units
   that don't exist in the zone: `lbat@hallows23` (tot_bag2 `load_7` and `stuff_name`) and
   `ceye@hallows23` (tot_bag4 `load_4`). `load()` then gives nothing, so one roll in eight of those
   bags hands the player nothing (and the bag is destroyed). The zone has `licbat` ("a licorice bat")
   and `cball` ("an eyeball" of white chocolate), which no bag loads. Rename the loads to `licbat`
   and `cball`? (DIL change, so not made.)
2. **cball** (line 1050): `type ITEM_POTION` but it uses `SCROLL_DEF(60,SPL_BLESS,...)`, and its
   identify says "when you quaff it". Should it be `POTION_DEF`?
3. **dq_shirt** (line 908): the runes use `<div class='cp'>`, a class the client doesn't define, so
   they show in the default colour. A colour meant there (e.g. `cpm`, purple)?
