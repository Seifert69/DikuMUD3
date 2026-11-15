# mageguild: review questions

## Step 1

1. **wiz_staff** (line 1424): the carved wizard staff has no `manipulate` line, so it can't be
   taken or wielded by players, though Caval and the gate guards wield it through the reset and its
   `$wear_s` text describes a player wielding it. Every other weapon in the zone has
   `manipulate {MANIPULATE_TAKE,MANIPULATE_WIELD}`. Add it?
2. **zone header help** (line 59): "Bar and relaxation - ... in the College Gardens (north and east
   or south and east from the Main Hall)". North and south of the Main Hall are the two towers; the
   gardens are reached from the Congregation Hall (east, east, then north and east, or south and
   east). Reword to "east and east, then north and east or south and east from the Main Hall"?
3. **rayasa** (line 992): "Rayasa the elf found a good and quiet job ... (if you don't count the
   constant buggering by the lecherous students of magery)". "Buggering" reads as crude; was
   "bugging" (pestering) meant?
4. **conj_charm** (line 1149): Rafizir calls it the "Spellweaver Charm" in all his speech (and the
   zone notes say so too), but the item's title and names say "Spellweaving Charm". Pick one? (The
   names would need "spellweaver charm" if the speech wins.) The charm's `chargercharm` DIL also
   waits for the charm to be wielded, but it is a neck item, so it never recharges that way; that
   is for the DIL review.
5. **backspindle** (line 954): the gardener is described as "a gnome" but is built with
   `M_HUMAN_WARRIOR_SWORD`. Make him a gnome, or leave him human?
