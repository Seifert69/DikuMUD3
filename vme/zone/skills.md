# skills: review questions

## Shared zone review

Fixed in place:
- steal 575: "non-equiped" -> "non-equipped"; 625 "toward you..You" -> "toward you... You"; 643 the witness line ends with a period.
- pickpocket 724: the victim was told "$1n just tried to steal" when the coins had been taken: "just stole some of your hard-earned money"; 778 the witness line ends with a period.
- filch 864: "much to difficult" -> "much too difficult"; 912 "you..You" -> "you... You".
- peek 961: "Who's inventory" -> "Whose"; 1001 "blatently" -> "blatantly"; 1009 "discrete" -> "discreet".
- do_use 1075, 1209: "?.", "!." and a missing comma.
- do_use 1085: it replaced the staff or wand you named with whatever you held, so "use wand" while holding a torch or another staff used the held item (a torch as a wand). Now the named item must be the one you hold, as the help says.
- flee 1296: "vines!." -> "vines!"; 1474: a rider fleeing out of a container was shown as "charges out of <his horse> on his horse" ($2n for $3n).
- picklock 1555: missing period; 1633 "$1n Tries" -> "$1n tries".
- ski_glance 1728: "What are you crazy?" -> "What, are you crazy?"; 1742 "amazing ehh?" -> "amazing, eh?".
- ski_bash 1987-1988: "send them reeling" -> "send $3m reeling".
- disarm 2068 "?" for a question; 2108 "$3n anticipates" -> "$3e"; 2119 "too shook up" -> "too shaken up"; 2173 "!." -> "!"; 2191 "without waking them" -> "$3m".
- rearm 2239, 2254, 2257: the item name was glued in without an article or period ("manages to retrieve a long sword", "his long sword" with no stop); now act() with the weapon as $2. 2269 "resigns to using" -> "resigns $1mself to using".
- res_obj 2558-2667: the failure messages said "You try to enlarges the cloak" and "$1n tries to shrinks but ..." (no item); now "enlarge"/"shrink" and the item. Missing periods; "realy" -> "really"; "$1n Begins" -> "begins"; "exausted" -> "exhausted".
- climb 3121: the header comment said UP == 5 (it is 4); 3181, 3186, 3194 "$1n failed to climb ... and takes" -> "fails".
- do_rescue 3227: `targ.type & UNIT_ST_PC|UNIT_ST_NPC` is `(type & PC) | NPC`, always true, so objects passed as rescue targets; parenthesised.
- trip 3785: the "face down in the muck" message used $3n before a target was found; 3863 "much to heavy" -> "much too heavy"; 3879 "$3n anticipates" -> "$3e"; 3918-3928 "!." and "sweeep"; 3938 "manuever" -> "maneuver".
- skin 4918 "Sorry, that"; 4969 double space; 4986 "skin $3n ." -> "skin $3n.".
- race_name 5421-5553 (skin and butcher titles): "a mummie", "a baracuda", "a octopus", "a ostrich", "a weasle", "a odd plant".
- butcher 5669, 5681: missing commas ("that, stupid!", "first, silly!").
- butcher 5727: it looked for "beheaded" in the corpse's description, which nothing writes; death.zon marks a beheaded corpse "headless" and with a $headless extra. Now both are checked, as death.zon does.
- butcher 5802, 5849, 6032: destroy() on the secured corpse jumped to lost_targ and skipped the rest; unsecure first.
- throw 6107: "$1e would not like that." spoke of the thrower; the carried creature is $2. 6299, 6313, 6328, 6345: "$1n throw" -> "throws".
- lay_trap 6471 "kidding, right?!"; 6599 "first!." -> "first!".
- trap_laid 6705: "narrowily" -> "narrowly"; 6722-6726: a trap with its own $spring_self/$spring_other text printed an empty line (the text was never used); 6744: a trapped monster read "A a man trap has trapped goblin here!" -> "A man trap has trapped a goblin here!".
- hunting 7609: "first.." -> "first."
- hunting 7616: `room` was set once before the loop and is cleared by every pause, so from the second step on "You can't seem to get any further" never fired and a stuck hunter kept trying every two seconds; now set at each step.
- hunting 7827: after a "harder to follow" pause `targ` was null, so the hunt ended silently after printing the direction; it now takes the unit found again, as track does. 7863: "Though you can't see $3n" had no $3.
- hunting 7819, track 8152: missing semicolon in "than you expected; you start". track 7967 "his tracking skills" -> "$1s".
- weapon_curare 8563: "and they flinch" -> "$1e flinches"; 8568: the wielder got "X flinches" on top of "Your weapon cuts X" (TO_REST -> TO_NOTVICT).
- shoot 8682-8688: "You are not skilled enough with a Bow" -> lowercase; crossbows "knock" -> "cock". 8748: `targ.type & UNIT_ST_NPC|UNIT_ST_PC` always true, so you could shoot objects; parenthesised.
- fashion_weapon 9229: `link(unit1, self.inside)` put the new weapon inside the first item in your inventory (a bag, or a sword); now your inventory. 9276: "You can only fashion" at full skill -> "You can fashion".
- soothe 9341 "Soothe" -> "soothe"; 9403-9405 "provokes it into attack" -> "attacking".
- shelter_rot 9605 "begings" -> "begins"; 9632-9634: the shack said "The grass rots away." and the hut "The wood rots."; swapped back.
- ambush 9714, ski_charge 12544, ski_cleave 12835, ski_rend 12921: "Sheesh some people" -> "Sheesh, some people".
- ski_scroll_use 10670 "scroll's" -> "scrolls"; ski_first_aid 11372 "Real bad.." -> "Real bad.".
- slip 11441, plant 11575, ski_stalk 11884: `not tgt.type & (...)` is `(not tgt.type) & (...)`, always false, so you could slip an item into a container or stalk an object; parenthesised. slip 11461, plant 11619 "groud" -> "ground".
- slip 11500: the receiver got the "X slips Y to Z" witness line as well; skipped like the slipper.
- palm 11741 "Palm what ?"; 11804 the palmer could get "$1n palms ..." about themself; skipped.
- ski_taunt 11987 "yourself !"; 12022-12024 "themself" -> "$1mself"; 12076 "$1n pitifully attempt" -> "attempts"; 12078 "fails their effort" -> "fails in $1s effort".
- defending 12238-12239 "their attack" -> "$3s attack".
- ski_berserk 12276, 12293 "attemps ... their" -> "attempts ... $1s"; 12305, 12315 "their" -> "$1s".
- ski_charge 12578 "you !" -> "you!".
- ski_leap 12631-12703: "You leap to the up." -> "You leap up." (the direction text now carries "to the").
- ski_whirl 12760-12761: "your long sword and a short sword" -> $3N; "their" -> "$1s"; 12776 "Your get tripped up" -> "You get"; 12777 "their" -> "$1s".
- ski_cleave 12876-12877, ski_rend 12961-12962, ski_slam 13089: "their $2N" -> "$1s $2N"; ski_rend 12956 "a heavy bladed to do that" -> "heavy bladed weapon"; rend_bleed 13003, 13011 "their" -> "$1s".
- d_fish 13462 "filet" -> "fillet"; d_slug 13501 "it's" -> "its"; wooden_shack 13741, 13743 "the hut" -> "the shack"; igloo 13764 "apexs" -> "apexes"; ice 13811 "blocks have" -> "has"; wood 13821 "are all that remain" -> "is all that remains", 13825 the remains of "an igloo" -> "a wooden shack".

Questions:
1. climb 3151-3163: the room program reads the climber's skill from `self` (the room), so `skilla` is always 0 and SKI_CLIMB is never used; only the climber's dexterity counts. It also wakes for any "climb" command, whatever the direction. No zone copies climb@skills any more (the one in darkhollow is commented out). Fix to `activator.skills[SKI_CLIMB]` / `activator.abilities[ABIL_DEX]`, or remove the template.
2. ski_sneak 11199-11201: for a player `skilld` is the sneaker's own sneak skill, the same as `skillc`, so the skill cancels out and practising sneak does nothing (the help says "Your skill decides it"). The commented old code used 50; suggest `skilld := 50`.
3. steal 588, filch 878: `can_carry` returns 2 when the item is too heavy; only 1 (too many items) is checked, so a thief can steal more weight than they can carry. Refuse on 2 as well ("too heavy")?
4. ski_turn 10750: no Book of Blood check. A failed turn on a player (or a turn on a non-undead player) calls provoked_attack, which makes that player fight the turner. The other combat skills refuse unless both have signed. Add the same check?
5. ski_turn 10767: without the skill you get "You failed." instead of "You must practice first." Intended?
6. punch 3735: the bonus is the fist skill without `+ self.level`; kick, elbow and knee add the level. Intended?
7. storm_gate 2290: unused; spells.zon has its own storm_gate that is used. Delete this copy?
8. res_clothes 2676, res_lth 2819, res_metal 2958: dead since resize calls res_obj for all three; they still carry the old typos ("to much", "realy", "exausted", "and he destroys it"). Delete them?
9. resize 2470: the person to fit is any unit found in the room, also an object ("resize ring sword" fits the ring to the sword's height), and for a monster the text reads "until it shrinks enough to fit guard." Limit tgt_pc to characters and use the title?
10. fashion_weapon 9113: the $fashioning extra is removed only when the program finishes; if the player quits, dies or is purged during its two pauses, the extra stays and every later "fashion" says "You are busy making a weapon". The commented-out `dilfind("fashion_weapon@skills", self)` check wouldn't leave anything behind. Switch to that? Fixed: not with dilfind (it finds the running program itself, so it would always say busy); $fashioning now holds the start time and one older than 60 seconds no longer blocks.
11. shelter_block 9450, 9459: `log("Block check")` and `log("Block")` write to vme.log on every command a sheltering player types. Remove them?
12. shelter 9581: the endurance check uses j (20 to 100, by skill) but the cost is always 40, and j is not used otherwise. Should the cost be j?
13. lay_trap 6591: the endurance is spent before the "someone has already planted some kind of trap here" check, which then quits. Move the cost after it? Fixed: endurance is spent after the already-trapped check.
14. lay_trap 6626: a trap's $spring_self/$spring_other text is passed through dilcopy() arguments, which are cut at commas; no trap has its own text today (man_trap says "default"), but one with a comma would break the template's arguments. Returned to Papi: nothing is affected today, and the fix is a choice of design between escaping the text or having trap_laid read the trap's own extras (changing what lay_trap passes).
15. trap_laid 6674: the trap wakes on every completed command by any monster in the room (`activator.type == UNIT_ST_NPC` with no command test), so a monster that says or emotes something springs it, while a player only by moving or looking. Intended?
16. flee 1447, 1478: `position_update(pc)` where pc is the opponent (null when not fighting); probably meant `self`. And fleeing out of a container while mounted (1467) execs "exit" for the rider, not the mount, while the text says the mount charges out.
