# baseget: review questions

## Shared zone review

Fixed in place:
- get_item, lines 469, 483, 489: "$2n</div>: You cannot take that." (and "...carry that much weight.", "...carry that many items.") had a stray `</div>` with no opening tag, which closes the client's own div; removed. get_money's same message (line 824) never had it.
- do_get, line 217: "You can only get stuff from your own players remains." -> "You can only get things from your own remains." (the help says "from your own corpse"; the check covers statues too).
- do_put, line 1166: "put 0 coins in bag" answered with the rest of the line instead of the number ("How can you put coins in bag of something into something?"): it used `temp` where do_give uses `buff`. Now `buff`, and the unused unit argument is null.
- put_item, lines 1347 and 1407: "cannot contain anymore" -> "cannot contain any more".

Questions:
1. put_item, line 1345: money is refused when `toitem.weight + toitem.baseweight >= toitem.capacity`; everywhere else (line 1355, line 1405) the contents are `toitem.weight - toitem.baseweight`. With `+` a heavy container refuses coins while it's far from full. Suggest `-`, as on line 1355. (Changes what put does, so not fixed.) Fixed: put_item tests `toitem.weight - toitem.baseweight`, the contents, as the lines after it do.
2. do_put, line 1227: "put all in <container>" puts the familiar (familiar2@g_q) in like any item (put_item already ran on line 1223) and only then says it "kicks and screams"; "put familiar in <container>" (line 1282) refuses it. Suggest skipping the familiar before put_item in the "all" loop, as the single put does. Left: familiar2@g_q is an NPC, so `item.objecttype != ITEM_MONEY` fails and the "all" loop never calls put_item on it; it only says it kicks and screams and stays.
3. do_get, line 322: without "from", `get <item>` looks with FIND_UNIT_SURRO, which includes your own inventory, so "get sword" with a sword in your inventory and none on the floor says "You get a sword." and moves nothing. Suggest FIND_UNIT_HERE minus inventory (or a "You already have that." answer). Left: FIND_UNIT_SURRO doesn't include your own inventory (find_unit_general only looks inside transparent units, and only at characters there), so "get sword" with the sword only in your inventory says "No such thing here."
4. drop_item, line 1125, and put_item: money dropped in a pile merges with the coins already there, so "drop 1 gold" on a pile of 5 says "You drop 1 gold pieces."; give and makemoney have the same plural from the merged pile. Cosmetic; say if it's worth a fix.
