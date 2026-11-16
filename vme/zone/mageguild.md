# mageguild: review questions

## Step 1

1. **wiz_staff** (line 1424): the carved wizard staff has no `manipulate` line, so it can't be
   taken or wielded by players, though Caval and the gate guards wield it through the reset and its
   `$wear_s` text describes a player wielding it. Every other weapon in the zone has
   `manipulate {MANIPULATE_TAKE,MANIPULATE_WIELD}`. Add it?
2. **zone header help** (line 59): "Bar and relaxation - ... in the College Gardens (north and east
   or south and east from the Main Hall)". North and south of the Main Hall are the two towers; the
   gardens are reached from the Congregation Hall (east, east, then north and east, or south and
   east). Reword to "east and east, then north and east or south and east from the Main Hall"? Fixed (text follows the world): zone help now says east and east, then north and east or south and east from the Main Hall.
3. **rayasa** (line 992): "Rayasa the elf found a good and quiet job ... (if you don't count the
   constant buggering by the lecherous students of magery)". "Buggering" reads as crude; was
   "bugging" (pestering) meant?
4. **conj_charm** (line 1149): Rafizir calls it the "Spellweaver Charm" in all his speech (and the
   zone notes say so too), but the item's title and names say "Spellweaving Charm". Pick one? (The
   names would need "spellweaver charm" if the speech wins.) The charm's `chargercharm` DIL also
   waits for the charm to be wielded, but it is a neck item, so it never recharges that way; that
   is for the DIL review. Carried into step 2, question 1. Fixed (text follows the world): item title and $identify now say Spellweaver Charm; names left (Spellweaving Charm, charm) because names are not changed.
5. **backspindle** (line 954): the gardener is described as "a gnome" but is built with
   `M_HUMAN_WARRIOR_SWORD`. Make him a gnome, or leave him human? Fixed (text follows the world): description now says Backspindle is a man (he is built human); "gnome" keyword left in names.

## Step 2

Fixed in place:
- nokill (the guild's NPCs): `if (pc.type == UNIT_ST_NPC)` came before `pc := self.fighting`,
  so it tested a cleared variable and NPC attackers (pets, summoned creatures) were also knocked out
  and thrown into Midgaard; the assignment now comes first, as in paladin_guild's nokill. Also
  "Several of them charge into the room, $3n topples" had no `$3`; now `$1n`.
- conj_give (Rafizir): "hands you a small charm" was `sendtext(self.name + ...)`, which printed the
  keyword "rafizir"; now an act() with `$3n`.
- conj_quest2: for a wrong item, the emote "locks ... away in the cabinet" and `destroy(gift)`
  came after a pause that clears `gift`, so the emote had no item name and the item stayed in
  Rafizir's inventory; both now come before the pause.
- mob_gating (the Spellweaving Charm): the wait tested `WEAR_NECK_1` twice, so the charm did
  nothing when worn in the second neck slot; now `WEAR_NECK_1` or `WEAR_NECK_2`. It also reacted to
  anyone in the room typing "grip" (blocking their command); it now reacts only to the wearer.

Questions:
1. **chargercharm** (line 1187, step 1 question 4): it waits for the charm to be wielded, but the
   charm is worn on the neck, so it never recharges. Meanwhile mob_gating's `:fail:` (no charges
   left) falls into `:charge:` and adds a charge, so an empty charm recharges by being gripped
   once while empty. Should chargercharm wait for `WEAR_NECK_1`/`WEAR_NECK_2`, and `:fail:` end
   with `goto start`? (That changes how often the charm can be used.) If chargercharm is fixed,
   `:charge:` also needs `goto init` when the charm is already full, or it falls into
   `:no_person:` and the wearer sees "You grip your charm, but it senses no need ...".
2. **conj_quest2** (line 836): an item given by someone who isn't a guild member with the charm
   quest is kept without a word (the program just waits again). Hand it back? Fixed: he hands it back ("$1n hands $2n back to you"); money given stays with him.
