# midgaard: review questions

## Step 1

1. **rainbow_3** (line 5231): the description says "a crystal bridge spans to the east", but the
   room has no east exit (the comment says the HF clan beyond it was removed). Drop the sentence,
   or is a bridge coming back?
2. **jandump** (line 1914): the description offers "a small room in the back, where those in need
   can get a free meal", but that back room (jandump2) is commented out and there is no exit to
   it. Drop the sentence or bring the room back?
3. **wall_rd3** (line 2131): the door extra says the door "appears to be securely locked", but the
   exit to sorcerer/ent_tower is only closed, not locked. Change the text, or should the exit be
   locked?
4. **fishmonger** (line 6333): "Molly ... is here, brushing fish scales off her pantry." "Pantry"
   can't be right; "apron"? "pants"?
5. **fishmonger** (line 6358): her shopkeepernew message list has an eighth entry,
   `"$1n says, '."`, where every other shop has seven. Leftover to delete?
6. **market_sq** (line 3062): the south exit text says oak trees line Royal Avenue; every Royal
   Avenue room says chestnut trees. Which one?
7. **deengrave / engrave** (lines 800 and 914): `exec("drop"+medium.name, self)` has no space
   after "drop", so the engraver never drops an item given by someone he can't see. Fix in step 2?
8. **item_charge / identifyhelp / resize_func** (lines 575, 729, 1080): the busy@function dilcopy
   strings have no closing parenthesis ("busy@function(Please wait I am busy with other work right
   now."), unlike deengrave/engrave which close it. Worth checking in step 2.
9. **to_the_rescue** (line 252): "$1n is trodding along." Is "trodding" meant (dialect), or
   "trudging"/"plodding"?
10. **wall_rd1** (line 2086): "running north-south along the east side of the city wall" while the
    room is on the western wall. Meant as "the inner side"? Other western Wall Road rooms say
    "along the western city wall".
11. **veejay2, reejay** (lines 6271, 6287): `weight -199`, which vmc warns about. Intended (so they
    can carry anything?) or a slip?
12. **banker, mercenary** (lines 6883, 7824): vmc warns "Too much money" on both (Smith carries a
    gold piece, the assassin 2 silver pieces). Keep or lower?
13. **mayor citizenDone** (line 8183): the hidden `$protected midgaard` extra says "Paid 1 PP to the
    Mayor", while the quest takes 2 platinum pieces. Update the note to 2?
