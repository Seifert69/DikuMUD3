# g_q: review questions

## Step 1

1. **grail** (line 2815): its description tells the player "To use this, type: Pour grail <player>.
   It will reduce their age by 20%.", but `dilcopy grail();` is commented out and there is no grail
   DIL in the zone, so the item does nothing. Remove the instruction from the text, or bring the DIL
   back?
2. **banksy** (line 1856): the graffiti seller stocks fifteen cans, among them `graf_w` (white), but
   not `graf_bw` (bright white, line 2550), which nothing else loads either. Should banksy sell it
   too, or is it left out on purpose?
3. **magef_manual** (line 2231): Hexica's owner's manual is a changelog from 1995 ("MageFamiliar
   Version 5.2", "Coming in revision 5.2 (scheduled for Jan 1995)", "KNOWN BUGS: lost master
   routine"), while the familiar's tags say "Version 6.0" and the code comment says much has changed
   since. Some of what it promises may no longer hold (summoning a familiar makes it attack you; the
   familiar knows every spell). Should the manual be rewritten as an in-game guide to what familiars
   do today?
