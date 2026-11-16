# basis: review questions

## Shared zone review

Fixed in place:
- command_block, line 113: waited for `command("backstab")` and ignored its parameter `s`, which the comment says is the command to block; now `command(s)` (no zone uses the template yet).
- odinshand, line 644: "looking bleak for $1n $1e suddenly vanishes": comma added after $1n.
- color_arg (cmes), line 1087: a name nobody in the game has left `pc` null; the null fell past the `pc.type != UNIT_ST_PC` test into the "Done." branch and the act() to nobody. Now says "Not a PC.".
- color_title (coltitle), line 1227: a name found nowhere left `victim` null and went on to `victim.title :=` on null; now goes to "No such person in world.".
- cheat_del, line 875: "in error contacted Whistler" -> "contact".
- nanny_alignment, line 1583, and nanny_sex, line 2446: "logon help is not set" message had no `<br/>`, so the prompt ran on after it.
- nanny_maiden, nanny_email, lines 2540, 2593: "anwser" -> "answer".
- nanny_newbie, lines 2627, 2691: "There is a few things" -> "There are a few things"; "menu choices they are" -> "menu choices; they are".
- dead_only, line 2799: "no need to dress you are dead" -> "dress, you are dead".
- ocean_void, line 3042: "You drifting" -> "You are drifting".
- timeout, line 3099: "All players which are idle, are stored" -> "who are idle are stored".
- throne_room2, line 3150: "a name on it Kuros, ..." -> "a name on it: Kuros, ...".
- heaven_entry, line 3169: stray comma in "sitting in it, view".
- hell_entry, line 3185: a stray `</div>` in the room description (no div was open); "long not that you should worry....You ARE dead!" and "this place nothing good" punctuated.
- deadbug, line 3205: description had no final full stop.
- dice, lines 3309, 3383: "two 100 sided dices", "rolling dices" -> "dice"; "The last rool of" -> "roll".
- color_object, lines 3403, 3440: "gawdy" -> "gaudy"; the `cb` example closed with `.,&ltdiv&gt` instead of `&lt/div&gt`.
- death_obj, line 3861: "astral planes." ran into "You slowly regain" without a space.
- scythe, line 3888: "Each rune ... glow" -> "glows".
- rod_error, rod_info, lines 3976, 3988: "this objects is" -> "this object is".
- energyworms, lines 4108, 4142: "attacks $1n $2e is dissolved" (comma); "$1n consumes $2n" -> "consume" (the title is "the energy worms").
- accuse_detect (odin), line 4397: comma splice "crime, if you wish" -> "crime. If you wish".
- pig, line 4500: "You can only cut in Saerimner the pig." -> "cut Saerimner".
- destroy_money (hel), line 4579: `priority;` was never followed by `nopriority;`, so after the first item given to Hel her later programs (appocalypse) got no more events; `nopriority;` added at `:losthim:`, where every path ends.

Questions:
1. death_obj, line 3800: sends "remove kill prottection", but no_kill@function waits for "remove kill protection". The message is meant to end the no_kill of an earlier death before the new copy is made; misspelt, the old copy keeps its timer and the new copy quits at once (PC_PK_RELAXED is already cleared), so dying again within the 10 minutes does not renew the protection. Fixing the spelling changes when kill protection ends (and the player sees "Your protection from Pk has just worn off." at the second death, then gets 10 new minutes). Suggest: fix the spelling.
2. color_title, line 1245: `victim.level >= self.level` compares with the colour object's level; objects have none, so the test always fails and a god can retitle anyone, a higher god too. Probably meant `pc.level`. Also line 1235: "You must be level 245 or higher to color title players." is behind `activator.level<240`, which never holds because the wait already requires 240. Which level is meant for titling players, 240 or 245?
3. throne_room, line 3129: the description says "An exit leads up to the throne hall and down to a portion of hell", but the up and down exits are commented out; only west exists. Restore the exits or drop the sentence? Fixed (text follows the world): dropped the sentence about exits up and down.
4. playerinit, lines 2196 and 2204: assassins and barbarians get kd/kb/kf/kr/ks aliases to "backstab", but barbarians get no backstab skill (frenzy and rage). Suggest "kill" for barbarians. (Only the four nanny professions can start today, so nobody gets these yet.) Fixed: barbarians' kd/kb/kf/kr/ks aliases now use "kill" (playerinit).
