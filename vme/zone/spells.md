# spells: review questions

## Shared zone review

Line numbers are those of the reviewed file (after the fixes below).

Fixed in place:

DIL bugs:
- every spell template that plays an effect (condemn, mire, focus, raise_hpp, raise_div .. mind_shield,
  energy_drain, slow, haste, quicksand, spl_invis, spl_fear, spl_confusion, spl_repel_1/2,
  spl_sanctuary, spl_sustain and the rest, about 55 calls): `effect(tgt, medi, hm)` passed target and
  medium swapped. The server (magic.cpp `dil_effect`) and every effect template in the zones take
  `(medi, targ, hm)`, as cure_light, cure_serious, aid_undead and refresh already called it. Now
  `effect(medi, tgt, ...)` everywhere (quicksand 12041: `effect(medi, u, hm)`).
- spl_repel_2 14562: the effect was played on `tgt` instead of the loop's `u` (the undead feared).
- remove_poison 6608: `tgt.type & UNIT_ST_PC|UNIT_ST_NPC` parsed as `(tgt.type & UNIT_ST_PC) | UNIT_ST_NPC`
  (always true); now `tgt.type & (UNIT_ST_PC|UNIT_ST_NPC)`.
- leaving 10357: `cast leaving` with no direction sent the caster north (`"" in "north"` is true); it
  now gets the template's own "You must specify a direction to leave in."
- slipemup 12481, prickly 12694: the tick branch did `goto loop`, a label inside the `while (d)` body,
  so `d` was never tested: the mud puddle never dried up and the bramble bush never withered by
  themselves (Help Mudpuddle, Help Brambles say they do). Now `continue`.
- capture_room 13108: the "try it once more" tick counter fell into `:relink:` either way; added the
  missing `goto start`.
- room_inside 14000, 14031 (wizard eye): `isset(self.flags, ID_DETECT_LIFE)` tested an affect id against
  the unit flags; now `isset(self.charflags, CHAR_DETECT_LIFE)`, so detect life shows hidden life forms.
- room_inside 14094: every player line got a second `<br/>` (blank line after each player).
- spl_unlock 14680, spl_lock 14906: "u" matched "south" (substring test, north/south tested first), so
  `cast unlock u <door>` worked on the south exit; "up" is now tested first.

act() and messages:
- condemn 110: the caster's "You call on the dark forces..." went to the target; now to the caster.
- undead_timer 3477: broken act code `$1 master`; now `$1s master`.
- gate_dil 2369: "grant group" on a gate already set to group said "allowed others"; now "allowed
  only your followers".
- refit_aff 5686: the onlookers' "falls off $1n" line went TO_CHAR (the wearer got both lines, the room
  nothing); now TO_REST.
- calm 9770: the room was told the caster "seems less paranoid"; now the target ($3n).
- earth_quake 12573: the bystanders' "knocked off $3s feet" line had a null character with TO_NOTVICT,
  so nobody saw it; now `self`.
- tsu_eff 11950: the caster, washed away too, also got "<own name> summons a powerful tidal wave that
  washes you away!"; that line now skips the caster.
- melting (ice thorn) 15170-15200: with the dagger on the ground or in a bag, the room was told
  "<room title>'s ice dagger begins to melt."; the messages now go out only while a character holds it.
- identify_str 6082, 6087: missing `<br/>` after the "please report to an admin" lines.

Text:
- condemn 93, focus 1586, raise_hpp 3143, raise_div .. mind_shield and energy_drain (16 lines from
  8601 to 9304): "The magic dissipates as it is cast on $2n" had no full stop.
- focus 1599 "comming"; mana_boost 1881 "Magical" mid-sentence; portal 2463 "prevents the you".
- raise_horde 2679, raise_champ 2772, raise_skel 2844, animate 10756: "dust...something" (no space).
- raise_champ 2760/2762, raise_skel 2832/2834, animate 10744/10746: "nothing happens" without full stop.
- blind_flash 4322, slow_rot 4791, refit 5636/5638, madness 6252, spl_madness 6298, mending/repair/
  reconstruct 6791/6848/6907, summon_earth 7083, summon_air 7119, hold_monster 9608-9631,
  spl_command 10098-10112, dust_devil 10205, spl_invis 14230: missing full stops (hold_monster 9612
  also had misplaced commas, spl_command 10110/10112 a double space).
- cause_insta 4526 "You is"; hammer_hand 4661 "you hands"; hold_flight 4721 "therefor"; spl_sleep
  4898/4900 "lay/lays down"; spl_plague 5070 "$1n look"; sum_check 5181 "loses his link" (it);
  find_path 5526 "illusive"; refit 5614 "The $2N appear".
- identify_str 5753 "is light source", 5758 scroll line without full stop, 5812 double space.
- madness 6250 "a battle ... begins"; spl_madness 6316 "You sudden realize", 6366 stray comma and
  double space, 6396/6400 "Khrisna" and "$1n sing", 6422 "life!.", 6508 "looks paranoidal at
  everyone", 6516 "laughs like he" (now $1e).
- charge_staff 6746, 6754: said "wand".
- demon_summon 7235 "unbonded".
- energy_drain 9293, 9324, 9328: "$1n ... chant", "$1n lay hands", "drain".
- spl_command 10091 "Thats", 10141 "? are you MAD?"; dust_devil 10194 "Dust swirl".
- resurrect 10529, 10563: run-on sentences.
- entangle 11001 "condusive", 11013 "wont".
- charm_animal 11336: "why summon another?" in the charm spell; heal_animal 11497 "does nothing more it".
- spl_dispel_good 14458 "to good"; bram_bush 15134 "brambles bush"; candle 15307 "into an oblivion";
  night_orb 15329 and demon 15512 "existance"; sum_eagle 15433 "it's"; fire_elm 15494 "suggest".
- lightn_elm, earth_elm, air_elm, water_elm, fire_elm, demon, devil: room line started "a large".

Questions:

1. gate_dil 2186: the "grant group" shock (rnd(10,50) hp, then position_update) can kill or
   incapacitate, while the "deny" shock at 2173 stops at 1 hp. Suggest the same `if (pc.hp < 1)
   pc.hp := 1;` floor.
2. gate_dil 2163, 2178, 2286, 2303, 2345: the owner test is `ownr in pc.name`, a substring test: a
   player "Alice" counts as the owner of Al's gate (passes a denied gate, can deny, grant, untie it).
   Suggest `pc.name == ownr`.
3. travel 2044, portal 2482: `targ.type & UNIT_ST_OBJ|UNIT_ST_ROOM` is always true and subtracts
   `targ.spells[SPL_NONE]` (index -1, which fails, so hm is unchanged). The line does nothing; suggest
   deleting it.
4. soul_shift 2978: `if (hm <= 0) ... quit` comes first, so the rift branch `if (hm <= -5)` (3050) can
   never run (and it has `$3n` with a null victim). The no-teleport checks (caster, its room, the
   target's room) are commented out, so soul shift swaps into and out of no-teleport rooms. Restore the
   checks? Remove or reorder the rift branch?
5. holy_word 2882, unholy_word 2901, wiz_eye 3638, bob_code 1781, simulate_spell 4842, spl_identify_2
   14988, stat_report2 12884, obj1 12924, obj1a 12963: not used by spells.def or any zone (stat_report2
   looks for `stat_obj1@spells`, which doesn't exist; the stat_corpse object and stat_blow are only
   loaded by petrify's code after its `quit`). unholy_word's bonus test `alignment >= -250` is always
   true for its targets. Delete them?
6. condemn 71: casting it on yourself asks you to sign the Book of Blood as if you attacked a player.
   Harmless; mention only.
7. cause_panic 4550: `log(self.name+" cast cause panic on "...)` writes vme.log on every cast, players'
   too; a leftover debug line? Its `tgt == null` fallback after it is dead code (TAR_CHAR|TAR_FIGHT_VICT).
8. cause_coward 4426: on success only the target gets a message; caster and room see nothing about the
   spell. Suggest "$3n suddenly looks panic-stricken." to TO_CHAR and TO_NOTVICT.
9. hold_flight 4696: no success message at all, and a second cast adds a second no_flee (each later
   prints its own fade message). Suggest a message and `if (dilfind("no_flee@spells", tgt))` ->
   "Nothing happens."
10. loc_want 5375, find_want 5438: the target resists with `spells[SPL_LOCATE_ANIMAL]` (copied from
    locate animal); SPL_LOCATE_WANTED is probably meant. Changes the chances.
11. find_want 5405: `pathto()` can return DIR_TRYAGAIN, which it doesn't handle: "You should leave ."
    Suggest find_path's retry (pause while DIR_TRYAGAIN, up to 10 times), with targ secured. Fixed: retries DIR_TRYAGAIN up to 10 times as find_path does, finding targ again after each pause (it is elsewhere in the world, so secure can't hold it), then "The magic is elusive".
12. refit 5554: with no person named, pc is null, the height checks do nothing and the item gets
    `refit_aff(<hm>,)` with a failed height. Help Refit says a person must be named. Suggest "Refit it
    for whom?" and quit (or default to the caster). Fixed: "Refit it for whom?" and quit, as the help says.
13. magic_str 5712: gives "" for +1..+5 and "no" for 0, so identify says "has a  magical modifier" or
    "has a no magical modifier". Suggest a word for 1..5 ("small") and rephrasing for 0.
14. identify_str 5989-6049: affect kinds 2 (light), 3, 4, 5 (flags) and 8 (natural armour) print the raw
    affect string plus `skill_name(<value>)`, which reads as debug output (and the wrong name table).
    Suggest printing them like kind 9 (label and signed value).
15. spl_madness 6364: `self.name in self.extra` looks in the mad person's extras instead of the paper's
    (`u.extra`); the paper (found with FIND_UNIT_HERE, maybe on the floor) is then dropped as
    "drop letter". Suggest `u.extra` and `exec("drop " + u.name, self)` only when it is carried. Fixed: looks in u.extra, and drops the paper by its name only when carried.
16. summon_air 7108, summon_water 7143, summon_fire 7180: a failed summon says "The ground shakes a
    bit." (from summon earth). Suggest a line of their own element.
17. entangled 11126 / entangled_2 11187: an entangled character can still flee: `entangled` lets
    "flee" through, and entangled_2's movement and flee blocking waits for SFB_PRE, which only spells
    send, so it never runs (walking is blocked by `entangled`). Help Entangle: "they can't walk away".
    Take "flee" out of the allowed list?
18. dispel_magic 7359: its two `interrupt()`s can never fire (the template never waits). Dead code.
19. spl_command 10073 / lst_parse 10043: with no word (`cast command bob`) the empty word matches the
    first entry: "Why not use the command 'quit'?". Suggest "Command $3n to do what?" when the word is
    empty. Fixed: spl_command says "Command $3n to do what?" when no word is given.
20. summon_animal 11518: unlike charm_animal it doesn't `dildestroy` an earlier ani_fol on the animal,
    so an animal someone else controls gets two. Suggest the same dildestroy. Fixed: summon_animal dildestroys an earlier ani_fol before adding its own.
21. tsu_eff 11910: foreach also lists what is inside a boat, so passengers are moved out of their boat
    one by one (Help Tsunami: "Those inside a boat go with their boat"); a boat that can't be taken
    stays behind. The caster is found again by name with FIND_UNIT_ZONE (11927), which a partial name
    can match first. Suggest `if (u.outside != self) continue;` as watercurrent@basemove does.
22. spl_random_tele 13235 (failed cast) and spl_minor_gate 13535 (target above your level): fail
    without a word to the caster. Suggest "Nothing happens." / "$3n is too powerful ...". Fixed: "Nothing happens." / "$3n is too powerful for you to summon." (the help says own level or lower).
23. spl_invis 14218: the spell prints "You become invisible." / "$1n fades away." and the affect's
    TIF_INVISIBILITY_ON prints "Your body appears ghostly." / "$1n vanish into thin air." too (the C
    side, tif_affect.cpp, also has "vanish" for "vanishes"). Drop one pair.
24. spl_lock 14878: locking a door found by its name locks the other side without checking that it has
    a door (EX_OPEN_CLOSE), which dir_locks checks. Suggest the same check. Fixed: spl_lock locks the other side only when it has a door (EX_OPEN_CLOSE).
25. water_elm 15480: made with M_ELEMENTAL_FROST_SMALL (a frost elemental) though
    M_ELEMENTAL_WATER_SMALL exists. Returned to Papi: the water macro changes numbers and behaviour, not just the race: alignment 0 -> -550, attack frostball -> spray of acid, other spell skills, and its aggressive@function target ANY -> GOOD.
