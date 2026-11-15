# sorcerer: review questions

## Step 1

1. **inside_cave1** (line 603): the six cave rooms form a 3x2 grid (inside_cave4, 5, 6 to the north;
   inside_cave3, 1, 2 to the south), and inside_cave2 leads west to inside_cave1, inside_cave3 leads
   east to inside_cave1. But inside_cave1 has `east to inside_cave3` and `west to inside_cave2`, the
   wrong way round, so going east and then west does not bring you back. Swap them, or is the cave
   meant to confuse (the headbump DIL suggests a nasty cave, but nothing else in it is a maze)?
2. **lasheetah** (line 1374): `M_DARK_ELF_MAGE_GAS(100,SEX_FEMALE,"sinking cloud")`. Every other
   gas mage in the game casts "stinking cloud"; is "sinking cloud" a slip that keeps her from casting
   her spell? Fixing it changes how she fights, so I left it.
3. **libram** (line 1566): the line
   `dilcopy abi_restrict@function (ABIL_BRA,50,0,25,"bra_rest@skaven");` is there twice. Drop one?
4. **inside_tower2, inside_tower3** (lines 500, 523): both say "There is an exit leading to the
   entrance and the other directions lead onward through the first floor of the tower", copied from
   inside_tower1, but each has a single exit back east/west to inside_tower1. Reword them as dead
   ends?
5. **neutral_mage**: vmc warns she carries too much money for her level (2 gold). She is a test
   opponent for sorcerers; keep it?
6. **vorous2** (line 1480): vmc warns that the title "the dagger 'Vorous'" ends with a non-letter.
   The quotes look intended; keep, or change to "Vorous, the sorcerer's dagger" or similar?
