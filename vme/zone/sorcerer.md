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
   ends? Fixed (text follows the world): inside_tower2/3 now say the only way out leads back to the entrance.
5. **neutral_mage**: vmc warns she carries too much money for her level (2 gold). She is a test
   opponent for sorcerers; keep it?
6. **vorous2** (line 1480): vmc warns that the title "the dagger 'Vorous'" ends with a non-letter.
   The quotes look intended; keep, or change to "Vorous, the sorcerer's dagger" or similar?

## Step 2

Fixed in place:
- daggerchat (blkm_stiletto): two TO_VICT messages ("pierce cuts deep into your stomach", "places $1s stiletto in
  your side") had `null` as the victim, so the victim never saw them; now `enemy`.
- blkm_stiletto (line 1796): `mvlev_restrict@function(84,0,0,"You are too high a level to use this
  dagger.")`: the last argument is called as a DIL function, and a sentence names none, so no
  message was shown at all. Now `""`, which gives mvlev_restrict's own message ("You are above the
  maximum level allowed to use ...").

Questions:
0. **headbump** (the cave rooms, line 144): it hurts and makes scream `self.inside`, the room's
   first unit (often another player or an item), instead of the non-sorcerer who typed the
   command. The fix is `activator` in place of `self.inside` (and `$1n` in the room message). Not
   made: the stalactites would then always hit the player who types, 1-10 hp on one command in
   five, with no `position_update` and no exception for gods; that makes the caves harder.
1. **cloak_invi** (invi_cloak1, line 2225): `command("raise hood")` never matches: "raise" is taken as the
   abbreviation of the social `raisebrow`, and `command()` looks up the whole string "raise hood",
   which is no command. So the cloak's invisibility (which its extra text promises: "The command to
   become invisible is simply 'raise hood'") has never worked. Fix with
   `command("raisebrow") and ("hood" in argument)`? That gives players the invisibility, so it is
   your call. Returned to Papi: making it work hands players the invisibility.
2. **cutlass_chat** (mith_cut, line 2044): when the enemy has fewer than 10 hp it says "The lifeless corpse
   keels over at your feet" and "$1n's slash cuts your throat!", but the enemy isn't hurt or killed
   (daggerchat sets `enemy.hp := -8` in the same case). Reword, or should it kill like the dagger? Returned to Papi: a choice between rewording and a kill (which would add damage).
3. **parlor_home** (line 1261): every time a sorcerer types `worship` there, the NPC loads and gives
   a new pendant and a new Vorous dagger, with no check for one already given. Intended, or should
   it give them once (a quest flag)?
4. **mana_drain** (line 201): the guild test is made on `activator`, the character whose combat
   round woke the program, which need not be the wielder (the enemy's rounds may wake it too). When
   it isn't a sorcerer the program skips the drain but still goes to `wait_time` (costs the wielder
   1 mana and a charge: "Your Vorous glows a little as it draws your energy"). Was the test meant for
   `self.outside`? (Changing it changes how often the drain fires.) Returned to Papi: it changes how often the drain fires and the charges it costs (numbers).
