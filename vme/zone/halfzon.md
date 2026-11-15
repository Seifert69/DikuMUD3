# halfzon: review questions

## Step 1

1. **zone header** (lines 71-77): the zone `help` that players read still says to contact Bakka or
   Whistler by mud mail or e-mail, with two 1995 addresses at blinksoft.com. Remove that paragraph
   from `help` (the notes can keep it), or point to something current? Fixed (text follows the world): contact paragraph and the e-mail addresses removed from the zone help (notes untouched).
2. **ne_yard** (line 2617): the room has two `south` exits, first `south to mayor_walk` and then
   `south to e_walk`. No unit `mayor_walk` exists; the second exit is the one that works and matches
   the text. Delete the first one?
3. **kaasak** (line 3802): Kaasak the barkeeper sells ale and dark ale (drink containers), but the
   item types his shopkeeper DIL trades in are only `ITEM_FOOD`, so he won't buy drinks. Should it be
   `ITEM_DRINKCON` (perhaps with `ITEM_FOOD`)?
4. **sailor_burrow** (line 1907): the room says "The only exit is to the east", but the burrow has no
   exits; it is entered with `enter` from vin_5 and left with `exit`. Change the text to say so? Fixed (text follows the world): sailor_burrow now says you can leave with exit.
5. **oldman** (line 4023): his extra says "Elven in race", but he is built with
   `M_HALFLING_MAGE_FROST`. Which race is meant? Fixed (text follows the world): oldman extra now says Halfling in race, as built.
6. **pitchfork, ship1** (lines 4841, 4921): the pitch fork's extra says it has four tines, its
   material says a three pronged head. The ship's extra says it was carved from a large cypress
   tree, its material says poplar wood. Which is right? Fixed (text follows the world): pitchfork extra says three tines; ship extra says poplar tree (as the materials).

## Step 2

Fixed in place:
- **firepit (room)** (line 393): the list of rooms `exit` can put a player in had `w_pit` twice and
  no `s_pit`, one of the eight rooms around the fire; the second `w_pit` is now `s_pit`.
- **firepit_inside** (line 238): "Your ... falls off" was shown for burnt-out items that were *not*
  worn (`item.equip==0`); now for the worn ones, which are the ones taken off.
- **spink_give** (line 3729): after handing out a replacement staff the player stayed secured, so
  when he later left, the mayor said "Now I will never get to go on my vacation". Unsecured now.
- **spink_qst_mboat** (line 4609): a wrong item given to Sarabin was destroyed after a `pause`,
  when `item` had been cleared, so he kept it; destroyed before the pause now.
- **oldstaff** (line 6216): `make food` / `make drink` by anyone in the room set off a staff held
  by someone else, with the typist as `pc`: without the mayor's quest he was drained (mana 0, half
  his hit points). The wait now also needs `activator == self.outside`.

Questions:
1. **war_hammer** (line 5056): it has no `goto start`, but it restarts at the top when it runs off
   its end, so it acts in every combat round while wielded: 1 in 20 a flash, and when the enemy has
   under 10 hp it "slams $2n into the dust", setting him to -10 hp without `position_update`, and
   drops itself on the floor. So it finishes off every enemy below 10 hp and throws itself out of
   the wielder's hand each time. Meant (balance), or rarer, with `position_update`?
2. **oldstaff** (line 6239): `make drink` takes the 10 mana twice (once for every make, once more in
   the drink branch), and only 10 are checked. Meant (drinks dearer than food), or remove the second?
3. **wine_press** (line 5571): the press can be entered. When the handle is pulled with the wrong
   ingredients, a player inside is set to -50 hp, so `position_update` kills him; after the
   `pause` the loop `destroy`s what is inside: his corpse with all his equipment (and if he didn't
   die, a god for instance, the player himself, which `destroy` takes out of the game and
   disconnects). The `pause` in that loop also lets a `put`/`get` interrupt jump away from it
   halfway. Keep the trap but let the corpse fall out (`link` it to the room), or no killing at all?

