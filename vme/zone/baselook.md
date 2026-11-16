# baselook: review questions

## Shared zone review

Fixed in place:
- **look_blank** (line 418): "You can't see a thing, you are blind!" went to `self` instead of
  `looker`. basemove's enter_dil and leave_dil call look_blank for every passenger of a boat or
  mount, so the one steering got the blind passenger's message.
- **look_blank** (line 491): when you are inside a player, `inside_descr != null` is always false
  in DIL (a string compared with null), so the player's inside description never showed; now
  `!= ""` as in the object and NPC branches. Line 494: "which is carrying:</div>" closed a div that
  was never opened.
- **look_blank** (lines 469, 472, 481, 509, 512, 520): "... is inside X and which contains:" ->
  "..., which contains:" (also "which is carrying").
- **look_at** (line 818): "has an ungly face and mishappen hair." -> "an ugly face and misshapen
  hair." (half-orcs). Line 766: looking at your own item ("look me's sword") told the room
  "Papi looks at Papi's sword"; now "$1n looks at $1s $2N.".
- **look_dir** (line 1136): `is_dir` turns "nw", "ne", "se", "sw" into those short names, but
  look_dir knew only the long ones, so "look nw" always said "No such direction here?". The short
  names are now mapped too.
- **look_dir** (line 1148): an exit's `$exit_descr` was shown even when the exit is hidden and not
  yet found, so "look <dir>" gave secret exits away (Help/Search: "look and examine won't reveal
  them"). It now checks `not test` like the other branches.
- **room_content** (lines 1397, 1458): the detect-life line "You sense a hidden life form in the
  room." ended in a stray `</div>` (for players) or an extra `<br/>` (for NPCs), inside the
  `npc_title` div the list adds; both now plain text, so they also count together ("[x2]").
- **obj_content, carried_by** (lines 1694, 1720, 1891, 1921): "somewhere near by" -> "nearby".
  carried_by line 1880: double space before "(wizinv)".

Questions:
1. **do_examine** (line 96): examining a mortal player without a description always says
   "$2e has big green ears, and long greasy hair." (the half-ogre line) whatever the race.
   look_at (lines 804-830) has a line per race. Use the same per-race text in examine? Fixed:
   do_examine now uses look_at's line per race.
2. **obj_content** (lines 1683, 1709): inside a container, boat or vehicle, hidden players and NPCs
   are listed with "(hidden)" to everyone who can see them, mortals too; room_content shows hidden
   characters only to gods. Hide them from mortals here as well (or show "(hidden)" to gods only,
   as carried_by does)? Returned to Papi: two answers fit (leave hidden characters out for
   mortals, or only drop the "(hidden)" tag), and the first changes what hiding in a boat or
   vehicle does.
3. **display_exits** (line 2004): the exit line shown on entering a room includes hidden exits that
   are open ("if it is open, everyone can see it"), while `exits`/`doors` (direc_dil@basemove), the
   web client's exit list (exits_h) and Help/Exits leave hidden exits out until found. Which is
   meant?
4. **exp_board / list_insrt** (lines 2107-2290): nothing in the game uses exp_board. If anything
   did, list_insrt logs every insertion (every player, every minute), "rank" with no argument says
   "Rank who?" and lists the board anyway, and the `restore`/`store` it relies on are commented out.
   Remove them?
5. **look_in drink_cont** (lines 645-653) and **look_at** (line 885) use `self` where they mean
   `looker`; today they are only called with `looker == self`, so it doesn't show. Fixed: both
   use `looker` now.
6. **do_examine / look_at** (lines 115, 852): "clearly requires a <weapon> fighting style" gives
   "a adze mattock"; minor.
