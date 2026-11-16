# haon_dor: review questions

## Step 1

1. **rn_6004, rn_6006, rn_6103** (lines 495, 551, 1333): exits the room description doesn't mention.
   rn_6004 says the trail leads east and that the huge trees to the west make it "impossible to
   enter the forest from here", yet it has a west exit to `path_8@fields` ("The narrow dusty trail
   leads west"). rn_6006 says "The path continues north and east" but also goes west to
   `clan_guard@clan_bus`, with the bare exit description "Hall of Heroes.". rn_6103 has a north
   exit to `haon_link@ogre1` that its description doesn't mention. Should the descriptions name
   these exits (and the Hall of Heroes exit get a real sentence)? Fixed (text follows the world): rn_6004 trail leads east and west and the trees extra no longer says you cannot enter from here; rn_6006 mentions the way west and the exit text is a sentence; rn_6103 mentions east, south and north.
2. **rn_6121, rn_6123, rn_6124** (lines 1820, 1878, 1913): these say the dark river "flows from east
   to west", but every river room (`darkriver1`, `darkriver2`, `lightriver`) carries you east and
   rn_6120/rn_6125 say it flows eastwards. Change these three to "flows eastwards"? Also, rn_6121
   says "The only exit appears to be east." while it has a south exit down to the river (its exit
   text says you could "just barely get down"); is "appears" deliberate, or should it mention the
   way down? Fixed (text follows the world): rn_6121/6123/6124 now say flows eastwards; rn_6121 mentions the way down to the river.
3. **iron_crown** (line 3790): `$identify` says it "protects as a chain coif", but it is defined
   with `ARMOUR_HRD_LEATHER` (and the comment says iron). Should the armour type be chain, or the
   identify text say hard leather?
4. **lr_mob1, lr_mob2** (lines 2360, 2374, only under `EXTENDED_MUD`): Darion's description is cut
   off mid-word ("Darion is a 6'4 man. He lo"), and Hesra is described as a woman ("She looks like
   the typical grandmother") but is `SEX_MALE`. Both carry the comment "He shall use a polearm"
   while wielding a sword. These player-house units are only compiled with `EXTENDED_MUD`; fix
   them, or are they dead and could be removed? Fixed (text follows the world): Darion's cut-off extra trimmed to "Darion is a 6'4 man."; Hesra changed to SEX_FEMALE (no sex tests); the "He shall use a polearm" comments left.

## Step 2

Fixed in place:
- dragon_attack (green_dragon): `priority` was set before the vlevel test and never released, and
  a player below vlevel 71 left a secure behind. The test now comes first, and `:start:` releases
  the priority (the dragon has no other program today, so nothing changes in play).

Questions:
1. **web_movement** (room under_web, line 2133) and **spider** (line 43): web_movement has no
   `goto`, but it restarts at the top when it runs off its end, so every command typed under the
   web (by anyone visible below level 200) sends "webaction", and at the first one every spider on
   the web drops. The spiders then `exec("u", self)` to go back, but under_web has no up exit (only
   east), so they stay under the web for good, and as they are no longer `on_web` they never drop
   again (the reset's `max 4` loads no new ones while they live). Put the spider back with
   `link(self, findroom("on_web@haon_dor"))` plus messages, since no exit leads there (perhaps with
   a pause or a per-player limit)? That makes the ambush happen every time, so it is your call.
2. **orc_return** (line 137, on the chief's three followers): the first thing it does is
   `walk_room@function("throne@dwarves", 4)`, so the orcs try to walk to that room in another zone
   as soon as they load, and only come back to orc_cave_chief when woken. Is that meant (a raid?), or a leftover from a copy (`orc_cave_chief@haon_dor`)? orc_return also puts an
   orc back to sleep as soon as it is awake, while orc_follower wakes them for the stew raid;
   the two fight each other.
3. **compl_pot** (line 2643): Mary's list includes "hold pot", but qstWaitDone has already
   deleted the pot she was given and the new one is loaded only after it, so she holds nothing.
   Load the pot before qstWaitDone's lines, or drop "hold pot"? Fixed: "hold pot" moved out of the list to right after the new pot is loaded, so Mary holds it again.
