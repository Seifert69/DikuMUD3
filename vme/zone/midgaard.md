# midgaard: review questions

## Step 1

1. **rainbow_3** (line 5231): the description says "a crystal bridge spans to the east", but the
   room has no east exit (the comment says the HF clan beyond it was removed). Drop the sentence,
   or is a bridge coming back? Fixed (text follows the world): dropped the crystal bridge sentence.
2. **jandump** (line 1914): the description offers "a small room in the back, where those in need
   can get a free meal", but that back room (jandump2) is commented out and there is no exit to
   it. Drop the sentence or bring the room back? Fixed (text follows the world): dropped the sentence about the back room.
3. **wall_rd3** (line 2131): the door extra says the door "appears to be securely locked", but the
   exit to sorcerer/ent_tower is only closed, not locked. Change the text, or should the exit be
   locked? Fixed (text follows the world): door extra now says it looks solid and heavy.
4. **fishmonger** (line 6333): "Molly ... is here, brushing fish scales off her pantry." "Pantry"
   can't be right; "apron"? "pants"?
5. **fishmonger** (line 6358): her shopkeepernew message list has an eighth entry,
   `"$1n says, '."`, where every other shop has seven. Leftover to delete? Fixed: deleted. The eighth entry replaces shopkeepernew's "$3n, I have no use for $2n" (no-sell items, closed hours), so Molly said "Molly says, '." there; she now uses the default.
6. **market_sq** (line 3062): the south exit text says oak trees line Royal Avenue; every Royal
   Avenue room says chestnut trees. Which one? Fixed (text follows the world): exit text now says chestnut trees.
7. **deengrave / engrave** (lines 800 and 914): `exec("drop"+medium.name, self)` has no space
   after "drop", so the engraver never drops an item given by someone he can't see. Fix in step 2? Fixed in step 2.
8. **item_charge / identifyhelp / resize_func** (lines 575, 729, 1080): the busy@function dilcopy
   strings have no closing parenthesis ("busy@function(Please wait I am busy with other work right
   now."), unlike deengrave/engrave which close it. Worth checking in step 2. Harmless (dil_copy reads to the end when the `)` is missing); closed in step 2 anyway. Left: all three already close the parenthesis.
9. **to_the_rescue** (line 252): "$1n is trodding along." Is "trodding" meant (dialect), or
   "trudging"/"plodding"?
10. **wall_rd1** (line 2086): "running north-south along the east side of the city wall" while the
    room is on the western wall. Meant as "the inner side"? Other western Wall Road rooms say
    "along the western city wall". Fixed (text follows the world): wall_rd1 now runs along the western city wall.
11. **veejay2, reejay** (lines 6271, 6287): `weight -199`, which vmc warns about. Intended (so they
    can carry anything?) or a slip?
12. **banker, mercenary** (lines 6883, 7824): vmc warns "Too much money" on both (Smith carries a
    gold piece, the assassin 2 silver pieces). Keep or lower?
13. **mayor citizenDone** (line 8183): the hidden `$protected midgaard` extra says "Paid 1 PP to the
    Mayor", while the quest takes 2 platinum pieces. Update the note to 2? Fixed in step 2.

## Step 2

Fixed in place:
- **item_charge, identifyhelp, resize_func** (lines 575, 729, 1080): closing `)` added to the
  busy@function dilcopy strings (step 1, question 8; no change in play).
- **item_charge** (line 653): `exec(", touches ...")`: the `,` emote shortcut only works for a
  player typing it, so the mage showed nothing; now `emote touches ...`, as identifyhelp has.
- **deengrave, engrave** (lines 800, 914): `"drop"+medium.name` lacked the space, so an item given
  by someone the engraver can't see stayed in his pack (step 1, question 7).
- **engrave** (line 990): saying "no" to "An engraving saying: '...', yes?" tested
  `(" " + argument) in "no"`, which is never true, so "no" was taken as yes and the player was
  charged for the engraving; now `argument == "no"` (shake still works as before).
- **deputy_join** (line 7213): the program ran off its end after each deputy it swore in, which
  restarts it at the top, where `:loop:` is; the `goto loop` added there has no effect in play;
  kept for clarity.
- **citizenDone** (line 8184): the hidden note said "Paid 1 PP to the Mayor"; the quest takes 2
  (step 1, question 13).
- **bob** (line 9850): a line break inside a string literal put a newline in the middle of "that's
  what signing this book means"; joined.

Questions:
1. **item_charge** (lines 654-659): `:repair_it:` casts charge wand/staff again until `value[1]`
   reaches `value[4]`, with no pause and no limit. If the spell ever fails to add a charge, the
   loop runs until the server stops the program, with the player's money paid and the wand still in
   the mage's hands, and the mage stops serving until the next boot. The labels `:broken_thing:`
   and `:not_possible:` are never jumped to. Add a counter (give up after, say, 20 casts and go to
   `:broken_thing:`)? Fixed: casts that add no charge are counted; after 20 of them he goes to `:broken_thing:` ("Oh my, it's broken...") and hands the item back. Casts that do add charges don't count, so big staffs still fill up.
2. **fido** (line 1779): nothing uses this template (the fido dogs use fido@function). It has no
   `goto loop` after the `foreach`, but it restarts at the top when it runs off its end, so it would
   work. Delete it?

## vmc warnings (after #604)

- janitors (line 403): a player's corpse got "$owner notified" after two pauses, when `trash` was
  already cleared, so the flag never landed and the owner was told again on every pass. The flag
  is now set before the pauses.
