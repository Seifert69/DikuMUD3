# death: review questions

## Shared zone review

Fixed in place:
- noaction, line 869: "You are dead, don't move!" was sent with the room (`self.outside`) as the character, so nobody saw it; now `self`, the dead player whose command is blocked.
- death_seq, line 788: "...sets your spirit down inside thew Newbie Guide Respite room You slowly regain..." -> "the", and a full stop between the sentences.
- death_seq, line 815: "...drift into the astral planes.You slowly regain your senses." -> space added.
- death_cry, line 1257: the distant death cry opened `<div class='death'>` and never closed it; `</div>` added; "someones" -> "someone's".
- rot_corpse, lines 507, 526, 545: the headless corpse's rotted description took `mid(..., 4, ...)` of "The smelly corpse of ...", which starts at the space after "The": "The headless  smelly corpse ..." with two spaces. Now `mid(..., 5, ...)`.
- decapCorpse, line 256: the head's title was "The decapitated head of X" with a capital, so "You get The decapitated head of X." Now "the ..." (string comparisons ignore case, so quests that compare the title still match).
- decap, line 328: "There is no head on that corpse silly!" -> "corpse, silly!".
- bob_code, line 438: "Making a Zombie from a fellow players corpse is forbiden." -> "Making a zombie from a fellow player's corpse is forbidden."
- make_corpse, line 917: "existance" -> "existence".
- corpse object, $rotact: "A quivering horde of maggots consume $1n" -> "consumes $1n."; corpse_statue: "The broken statue of $1n standing here." -> "is standing here.", "$1n shatters into a million pieces" -> full stop added.

Questions:
1. bob_code, line 380: `if (self.value[2]!=1) quit;` runs when the corpse is loaded, before make_corpse sets value[2] to 1 for a player (load() starts a unit's DIL at once; rot_corpse and report_corpse pause twice first for exactly this reason). So on a fresh player corpse bob_code always quits: the "You can't loot a player corpse." block and the animate dead block never work until the corpse has been stored and restored at a reboot. Getting from another player's corpse is still refused by do_get@baseget, but casting animate dead on it isn't. Suggest the same `heartbeat := PULSE_SEC; pause; pause;` before the check. (Turns protection on, so not fixed.)
2. death_seq, line 791: a player with the "New Player Helper" quest is sent to the Newbie Guide Respite Room, but the room they leave is told "$1n enters the castle of Valhalla." Suggest "$1n is carried off by a Valkyrie." or similar for all three branches (the message is shown in the death room before the move, so "enters" reads oddly in each). Fixed (text follows the world): room message in all three branches is now "$1n is carried off by a Valkyrie.".
3. remove_aff, line 1041: `continue` without `i := i + 1` after `subaff(self, i)` loops forever if an affect can't be removed (subaff refusing it), until the server stops the DIL for an endless loop. Probably never happens; suggest stepping i in that case too. Fixed: remove_aff tries subaff at most 10 times per affect id, then moves on.
