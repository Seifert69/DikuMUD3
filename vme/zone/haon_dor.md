# haon_dor: review questions

## Step 1

1. **rn_6004, rn_6006, rn_6103** (lines 495, 551, 1333): exits the room description doesn't mention.
   rn_6004 says the trail leads east and that the huge trees to the west make it "impossible to
   enter the forest from here", yet it has a west exit to `path_8@fields` ("The narrow dusty trail
   leads west"). rn_6006 says "The path continues north and east" but also goes west to
   `clan_guard@clan_bus`, with the bare exit description "Hall of Heroes.". rn_6103 has a north
   exit to `haon_link@ogre1` that its description doesn't mention. Should the descriptions name
   these exits (and the Hall of Heroes exit get a real sentence)?
2. **rn_6121, rn_6123, rn_6124** (lines 1820, 1878, 1913): these say the dark river "flows from east
   to west", but every river room (`darkriver1`, `darkriver2`, `lightriver`) carries you east and
   rn_6120/rn_6125 say it flows eastwards. Change these three to "flows eastwards"? Also, rn_6121
   says "The only exit appears to be east." while it has a south exit down to the river (its exit
   text says you could "just barely get down"); is "appears" deliberate, or should it mention the
   way down?
3. **iron_crown** (line 3790): `$identify` says it "protects as a chain coif", but it is defined
   with `ARMOUR_HRD_LEATHER` (and the comment says iron). Should the armour type be chain, or the
   identify text say hard leather?
4. **lr_mob1, lr_mob2** (lines 2360, 2374, only under `EXTENDED_MUD`): Darion's description is cut
   off mid-word ("Darion is a 6'4 man. He lo"), and Hesra is described as a woman ("She looks like
   the typical grandmother") but is `SEX_MALE`. Both carry the comment "He shall use a polearm"
   while wielding a sword. These player-house units are only compiled with `EXTENDED_MUD`; fix
   them, or are they dead and could be removed?
