# gremlin: review questions

## Step 1

1. **fn_01** (line 990): the carrot field is a 3x2 grid (fn_03 fn_04 fn_05 to the north, fn_01 fn_02
   fn_06 to the south), and every other exit fits it, but fn_01's east exit leads to fn_04 (the
   north-middle field) instead of fn_02. fn_02's west exit leads to fn_01, so walking west then east
   puts you somewhere else, and fn_04's west exit goes to fn_03. Should fn_01 have `east to fn_02`?
2. **fn_03** (line 1023): its description is a copy of fn_02's ("A strong fence opens to a wide gate
   here leading to the south. North, east, and west, carrots dot the ground."), but fn_03 is the
   north-west corner: exits only south and east, and the gate to the farm is at fn_02. Rewrite it
   along the lines of fn_04/fn_05 (fence to the north and west, carrots to the south and east)? Fixed (text follows the world): fn_03 description now fence north and west, carrots south and east.
3. **pendant** (Gwydion's Amulet, line 2290): nothing in the game loads it. It has
   `dilcopy charger@sorcerer()` and `mana_drain@sorcerer()`, which only act while the item is
   wielded (`WEAR_WIELD`) and whose messages name "Vorous" (the sorcerer dagger); on a neck amulet
   they never fire. Is the amulet meant to be used (as a reward somewhere), and if so, should those
   two DILs go?
4. **gremlin_large, grumpkin_large, gaflaw, gguard, sventer**: vmc warns that they carry too much
   money for their level (1 silver on a level 9 gremlin, 1 gold on sventer). This is a newbie zone,
   so the extra may be on purpose; keep it?

## Step 2

Fixed in place:
- boldin_request (`:water:`): the hint "Simply type 'nod'" was an act() after `qstWaitHero`,
  which only returns once the player has nodded; it is now part of Boldin's last whisper, as in the
  first part of the quest.
- soul_anchor_qst (`:accept_explore:`): the shaman waited for any player's command and took that
  player as `pc`, so a bystander's nod or shake answered for the one he was talking to. It now waits
  for `pc`.
- soul_anchor_qst: `exec("wave $1n", self)`: exec() doesn't expand `$1n`, so the shaman waved at
  "$1n"; now `"wave " + pc.name`.
- barnham_give: `destroy(self.inside)` and, after a pause, `link(self.inside, pc)` acted on the
  first unit in Barnham's inventory rather than the item given; now `destroy(item)`, and a wrong
  item is handed back before the pause (`item` is cleared by it).

Questions:
1. **barnham_request** (line 725): the weapon Barnham hands out is chosen by profession, and
   `PROFESSION_MYSTIC` is in none of the three lists, so a mystic gets nothing: `item` is null, the
   act says "Barnham hands you a ." and the quest still goes on. Which weapon should a mystic get
   (`barnham_mace`, like priests)? `barnham_axe` exists but nothing loads it; was it meant for
   barbarians?
2. **soul_anchor_qst** (lines 477-490): the first wait takes any player's command, but the
   second one (`smile`) takes any player's smile, without checking that player's quests. A player
   who has finished the quest (or has it ongoing) can smile, nod, and be given "Soul Anchor Request"
   again, then earn the 1000 experience once more. Check `SOUL_ANCHOR_COMPLETE` /
   `SOUL_ANCHOR_ONGOING` on the smiler (or wait for the smile first and then test the quests)?
3. **soul_anchor_qst** (`:soul_anchor_ongoing:`, line 581): once a player with the quest ongoing
   does anything near the shaman, he waits for *any* give, from anyone: a player without the quest
   who gives him Gaflaw's heart completes it and gets the experience, and no new player can start
   the quest (smile) until someone gives him something. Check `SOUL_ANCHOR_ONGOING in pc.quests`
   on the giver, and wait for the give and the smile in one place? Fixed: the give is handled at `:start:` only when the giver has the quest ongoing; any other action by such a player goes back to waiting, so others can start the quest. (The smiler's quests, question 2, are untouched.)
4. **soul_anchor_qst** (line 569): `qstAssign` executes each string of its list as the shaman's
   command; `"This string doesn't seem to be used?"` makes him try the command "This". Replace it
   with a real line (e.g. `"nod"`), or leave the list empty? Fixed: the line is `"nod"`.
5. **charger, mana_drain** (lines 90, 102): copies of the sorcerer zone's templates; nothing uses
   the gremlin copies (the pendant uses `charger@sorcerer` and `mana_drain@sorcerer`, see step 1
   question 3). Remove them?
