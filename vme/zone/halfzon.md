# halfzon: review questions

## Step 1

1. **zone header** (lines 71-77): the zone `help` that players read still says to contact Bakka or
   Whistler by mud mail or e-mail, with two 1995 addresses at blinksoft.com. Remove that paragraph
   from `help` (the notes can keep it), or point to something current?
2. **ne_yard** (line 2617): the room has two `south` exits, first `south to mayor_walk` and then
   `south to e_walk`. No unit `mayor_walk` exists; the second exit is the one that works and matches
   the text. Delete the first one?
3. **kaasak** (line 3802): Kaasak the barkeeper sells ale and dark ale (drink containers), but the
   item types his shopkeeper DIL trades in are only `ITEM_FOOD`, so he won't buy drinks. Should it be
   `ITEM_DRINKCON` (perhaps with `ITEM_FOOD`)?
4. **sailor_burrow** (line 1907): the room says "The only exit is to the east", but the burrow has no
   exits; it is entered with `enter` from vin_5 and left with `exit`. Change the text to say so?
5. **oldman** (line 4023): his extra says "Elven in race", but he is built with
   `M_HALFLING_MAGE_FROST`. Which race is meant?
6. **pitchfork, ship1** (lines 4841, 4921): the pitch fork's extra says it has four tines, its
   material says a three pronged head. The ship's extra says it was carved from a large cypress
   tree, its material says poplar wood. Which is right?
