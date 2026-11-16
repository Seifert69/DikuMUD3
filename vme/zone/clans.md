# clans: review questions

## Shared zone review

Fixed in place:
- namepillar, lines 92, 101, and tome_dil, lines 156, 165: a stray `'` before `</div>` in four messages ("...sign this pillar!'").
- tome_dil, line 147: "The tome lists the names Bounty Hunters" -> "the names of Bounty Hunters".
- clan_aggro, lines 230, 232: "harms way" -> "harm's way".
- clan_aggro, line 248: the warning " $3n, you are not a member of this clan! Leave NOW!" went TO_NOTVICT, so the intruder never saw it and the others saw a line with nobody saying it; now "$1n says, '$3n, you are not a member of this clan! Leave NOW!'" TO_ROOM.
- clan_aggro, lines 256, 258: "killing $3s instantly" used his/her where him/her is meant ($3m); commas added.
- cdelete, clan_delete, cupdate, cremove, caccept, crank, csrank, cpromote, cbalance, cwithdraw, cequipment (lines 429, 444, 917, 1169, 1255, 1260, 1364, 1396, 1972, 2218, 2264, 2422-2427, 2895, 2990, 3181): the clan colour div closed in the middle of the sentence ("You have been removed from the </div>"+name+" clan.") or was empty (`<div class='clan'></div>`+text); the whole message is in the div now. caccept's message also had a line break inside the string.
- cdelete, clist, cstart, cstat, ctitle, cremove (11 lines, e.g. 348): "There is something wrong with clans talk with an admin" -> "clans; talk".
- store_blk, line 612: double space in "the  overloaded clan chest".
- cinfo, line 683: `goto cinfo_treas` had no `;` (it compiled to the same code; added for clarity).
- cinfo, lines 689-884: "(* denotes on line)" -> "online"; "no lords yet see the master" (and treasurers, quartermaster) -> "yet; see the master".
- donate, lines 1012, 1037, 1077: "Clan code error report to Admin." -> "Clan code error; report it to an Admin."; "You can't donate your clan Chest is full." -> "You can't donate; your clan chest is full."; "A Glowing red portal" -> "A glowing".
- crank, lines 1246, 1249: "a Labeled rank" -> "a labeled rank".
- cstart, line 1556: "belong to a clan why would you" -> "a clan; why would you".
- cremove, line 2204: "is a clan master you can not do that" -> "is a clan master; you cannot do that".
- cpromote, lines 2369-2380: two act()s used $3n with a null victim ("$3n is not a member of your clan.", "$3n's rank is equal to or higher then yours."), so the name was missing; the member is passed now. "then" -> "than" there and at line 2391 ("ranks less then yours" -> "ranks lower than yours"); "only clan Masters" -> "Only"; "Striping"/"striped" -> "Stripping"/"stripped".
- cappoint, line 2574, and cdepose, line 2702: `class='clam'` -> `class='clan'`.
- cdepose, lines 2721, 2742: double space in "$3n's  access".
- cdeposit, lines 2789-2792: the usage told players to type 'deposit ...'; the command is cdeposit (help page Cdeposit). Line 2845: full stop added.
- cwithdraw, line 2985: "Error in the transfer contact an admin." -> "transfer; contact"; line 2997: the dilend comment said cbalance.
- cunstore, lines 3064, 3112: "What do you want to Unstore" -> "unstore?"; "You can't carry that your hands are full." -> "that; your hands".
- guard_dir, line 3329: `on_activation(..., sleep)` jumps to `:sleep:` and the jump clears it, so after a guard's first nap a sleeping guard went on blocking the way; the on_activation is set again after the guard wakes up.
- clan_meet, lines 3379-3398: "Here are 24 solid stone chairs" -> "There are"; "it's many long years" -> "its"; "much of the runes" -> "many of the runes".
- pillar, lines 3413-3495: "it seems as it is used" -> "as if it is used"; "There is a few red runes" -> "There are"; "Knight of Fire" -> "Knights of Fire" (the clan's name in its zone); a tab in the clan list; Chapter Three's heading said "What A Clan Is Not Required For" where the index says "Not Intended For"; its keyword "What a Clan is not intended for" had a line break in it.

Questions:
1. store_blk, line 617: `temp` is set only in the branch that throws an item out, so `check := temp` ends the loop after the first item (temp is null): the clan chest never pushes out items beyond CLAN_MAX_DONATION. Setting `temp := check.next;` at the top of the loop would make it work, but then the items thrown out are the ones at the end of the list (the oldest, not the one just put in), and the chest is only stored at the next `get`. Fix it that way (with a `store` after), or drop the overflow?
2. cunstore, line 3102: can_carry returns 1 when the hands are full and 2 when the item is too heavy. The code tests `ti==1` with "You can't seem to lift that." and then `==1` again with "your hands are full" (never reached); a too heavy item is handed over anyway. Suggest: the first test `ti == 2`. That refuses items that are given today. Returned to Papi: the fix starts refusing too-heavy items that quartermasters get today.
3. re_equip, line 514 (used by four clan zones): only the `wait` is inside `if (self.position == POSITION_FIGHTING)`; a guard that isn't fighting when it loads (always) goes on with activator null, findunit fails and the program quits at once. When it does wait, SFB_CMD wakes before the disarm is done, and the message "The guard retrieves a spear..." is a sendtext to the guard itself, so no player sees it. Suggest a rewrite: `wait(SFB_DONE, command("disarm") and (target == self))` in a loop, the spear from spear0@midgaard, and the message as act() TO_ROOM. Returned to Papi: making it work would make disarmed clan guards re-arm themselves, which makes them harder to fight; it is a rewrite, not a plain fix.
4. addmember, line 1980: caccept gives the new member rank "1" in the extra (the help says members start at rank 1), but addmember ignores its `rank` and `pos` arguments and writes `name:0:$member/$none/$none` to the roster, with a "\n" inside the entry, so a blank line follows each new member. cinfo shows ranks from the roster, so new members show no rank. The loop that drops empty lines also skips the line after each one it removes. Suggest: write `pname+":"+rank+":"+pos2` without the "\n".
5. namepillar line 99 and tome_dil line 163: `pc.name in nlist.[i]` is a substring test, so "Al" counts as having signed when "Alice" is on the list. Suggest comparing the first word of the line with the name. Fixed: both compare the first word of each line (getword) with the name.
6. ctitle, line 1857: a god (above IMMORTAL_LEVEL) jumps to `:admin_title:` before the "No such person found." check, so a name that isn't in the game gives a title message about the first NPC in the world list ("<npc> does not appear to have an extra..."). Suggest the same player check before the jump. Fixed: the same "No such person found." check now starts `:admin_title:`.
7. god_delete, line 1478: calls submember on every line of clanlist.data (`i+1`) instead of every clan id (`i+5`), so it tries to load files named after masters, clan names and tattoos and logs "01: Error in submember" for each. Suggest `i := i+5`. Fixed: `i := i+5`.
8. cdeposit line 2828 / cwithdraw line 2983: cdeposit takes the money before checking that the balance doesn't overflow (the money is then lost), and cwithdraw saves the lower balance before transfermoney (if that fails the money is gone). Suggest: check before taking, and save after a successful transfer. Fixed: cdeposit checks the overflow before transfermoney; cwithdraw saves the balance after it.
9. pillar, line 3462: Chapter Two asks players to email Darg@valhalla.com with a 400 platinum fee; Help Clan Startup describes the application now. Rewrite the chapter to point to the help page? Fixed (text follows the world): Chapter Two now points to Help Clan Startup; email and fee removed.
10. clan_delete (line 896), clan_clear (line 461) and donate (line 953) are used nowhere (the donate command is donate@commands). clan_delete would also drop the last name of clan.delete (`while (i<(ln-1))` after `ln` was already reduced by one). Delete them?
