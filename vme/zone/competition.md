# competition: review questions

## Step 1

1. **comp_board** (line 1443): the sign says "read [number] - shows individual competition
   information", but the board's DIL only handles `look` and `list`; `read` falls through to the
   ordinary board command. Is there a `read` handler somewhere else, or should the line go?
   (The "top 200 leaders" on the same sign was changed to "top 20", since the board lists 20.)
2. **xp_board** (line 1422): the object is the "Legends of Valhalla board" (now with "the" in
   front), but the table it shows is headed "Heroes of Valhalla". Should one name be used for both?
3. **death_brd** (line 70): for a player who isn't on the death list, the DIL checks
   `"$TOTAL_DEATHS" in rm.extra` (the board object) but reads `activator.extra`, so it always
   shows 0 deaths. Probably meant `activator.extra` (DIL logic, for step 2).
