# commands: review questions

## Shared zone review

Fixed in place:
- dlrmanage, 180: "revet" changed to "revert".
- knock, 265: "knock on what?" now starts with a capital.
- knock, 311: "You knock on $2m." printed "You knock on it." ($2m is him/her/it). Now $2n.
- boot_area, 666: removed a second, identical addextra of $donlist.
- do_areas, 730: "(*)denotes" changed to "(*) denotes".
- do_areas, 791: `areas <continent>`: the heading had no `<br/>`, so the first area ran onto the heading line.
- do_prompt, 1085: "is :" changed to "is:".
- do_wiz, 1313/1315: the `<div class='wiz'>` of every wiz line was never closed (the line ended with `<br/>`). It now ends with `</div>`.
- ditch, 2211/2217: "by that name!." changed to "!". 2239: added the missing period.
- do_group, 2375: the "must follow you" sendtext had no `<br/>`.
- do_restore, 2477: "You have restore" changed to "restored".
- do_score, 2676: removed a stray "." after the hungry `</div>`.
- do_advance:
  - 2800: "how high." changed to "how high?".
  - 2865: "darkness"+"comes" ran together; added the space.
  - 2866: "which begin" changed to "begins".
  - 2900: added the missing ";".
  - 2910/2921: "Characters Experience" changed to "Character's experience".
  - 2925: the run-on sentence now has a ";" and ends with a period.
- do_light, 3001: "wont" changed to "won't", as the help page quotes it.
- wear, 3326-3344: "ringfinger" changed to "ring finger".
- wear, 3730: the shield's size message was copied from the arms one ("$3n are $2t to fit your arms"). Now "$3n is $2t to fit your arm".
- remove_equip, 4108: `isset(obj.flags, OBJ_NO_UNEQUIP)` tested an object flag in the unit flags, where it is UNIT_FL_PRIVATE. Now `obj.objectflags`. Every OBJ_NO_UNEQUIP in the game comes with ID_CURSE, which the same line already tests, so play is unchanged.
- unit_msg, 4237-4244: act() used `self` instead of its `ch` parameter. Every caller passes `self` today, so nothing visible changes.
- attack_switch, 4805: removed a stray `</div>` from the "not available" message.
- do_quit, 5331: "friend.." changed to "friend.".
- do_password, 5386/5395: "to short" and "to long" changed to "too". 5410: "do not match try again" changed to "do not match. Try again."
- do_delete, 5476: added the missing period.
- bury, 5613: "No Way! Your fighting" changed to "No way! You're fighting". 5665/5711: added the missing periods.
- bury, 5707: in the inventory loop, the count of `bury 2.<item>` went down for every item skipped, matching or not. It now goes down only for items that match.
- bury, 5714: in the inventory loop, `temp` (the next item) was set only when an item was buried. `bury all.<item>` with an item that can't be buried went on from a stale `temp`: the same item again, in an endless loop until the DIL was killed. `temp` is now set before the test.
- do_stand, 5899/5911: "clambers on $1s feet" changed to "clambers to $1s feet".
- do_sit/do_rest/do_sleep: "? are you MAD?" changed to "? Are you MAD?".
- do_sleep, 6062: "and lie down to sleep" changed to "and lies down to sleep" (the room's message).
- do_wake, 6098: removed a stray `<br>` from the act() text (act() adds the line break).
- getstring, 6162-6172 (make_names, used by piset):
  - Each word replaced the text before it (`temp := " "+word`) instead of being added to it, so piset gave power items names like " anklet" instead of the phrases of the new title. Now `temp := temp+" "+word`.
  - `i+ln>=size` left out the last phrase. Now `>`.
- piset:
  - 6267: "piset what?" now starts with a capital.
  - 6306: `<help piset>` was swallowed as an HTML tag. Now 'help piset'.
  - The three Y/N prompts had "(N)o" and an unclosed "(". Now "[N]o)".
  - "objects title/short description/description" changed to "object's".
  - 6475: the editor prompt is now a sentence.
- agestring, 6950: "You are a 18 year old" (and 8, 11, 80-89) now reads "an".
- playingstring, 7008: with less than a day of play, score said "You have been playing for, and 5 hours." Now "for 5 hours".
- cmd_stat, 7298: `status spells`: the divine spells' `<div class='twocol'>` was never closed.
- gtitle, 7666: "tot he" changed to "to the".
- describe, 7719: `describe` on its own said "No such location to describe". The code meant it to show the usage, but its `arg == ""` test could never be reached. With no words it now shows the usage, as the help page says ("describe help").
- describe, 7799/7801: "your Current description" changed to "Your current description".
- do_drink/do_sip/do_taste: "can't contain anymore" changed to "can't contain any more".
- do_taste:
  - 8392/8411: tasting a drink said "tried to sip" and "You sip the". Both now say "taste".
  - 8429: the last taste of a drink emptied the container by hand and left its $drink_color/$drink_thirst/... extras behind. It now calls sub_drink_info(), as drink and sip do.
- do_pour, 8645: "error do to new container code replace" changed to "error due to the new container code; replace".
- do_split, 8767: "pieces"+"(or all" ran together; added the space. The split.log lines had the same problem ("the"+"split", "split"+"command").
- seize, 9243; reply, 9315/9322/9330: a line break inside a message string (sent as a raw newline). Each is now one line.
- who_dil, 9440: the heading "Valhalla Friend:" changed to "Valhalla Friends:".
- who_dil, 9480-9750: the divs of the who lines didn't balance.
  - [ INV ] opened a `<div class='who'>` that only Builder/Friend/Guide closed. When the player wasn't invisible, those tags closed a div nobody had opened. One invisible player could pull the following lines into an open div.
  - Now [ INV ] closes its own div, and each tag is its own `<div class='who'>` (the class the clients colour).
  - The clan list's line had an extra `</div>` after the title.
- affect, 9859: Frenzy was listed twice. Now once.
- transfer, 10160/10204: added the missing periods. 10224: removed a stray `</div>` ("You must type transfer all now</div>.").
- assist:
  - 10270: "!." changed to "!".
  - 10273: "Your in pretty bad shape if your the only one" changed to "You're ... you're", and the string had a line break inside it.
  - 10276: removed a line break inside the string.
- evaluate, 10434: added the missing period. 10470-10478: removed the comma in "Refitting the size of it, requires"; "leather working" changed to "leatherworking".
- guild_wipe, 10514: "supply a the name of a player" changed to "supply the name of a player.". 10528: added the missing period.
- ctnstore, 10684: "We do not accept to store containers anymore" changed to "We do not store containers any more".
- ctnstorage, 10863: "contains : " changed to "contains:".
- donate:
  - 10961: "junk!" changed to "junk?".
  - 10981: "error report to Admin" changed to "error; report to an Admin".
  - 11008: "can't donate your clan Chest is full" changed to "can't donate; your clan chest is full".
  - 11050: "A Glowing" changed to "A glowing".
- guild_obj, 12782: "Simple type 'gwipe <playername>'" changed to "Simply type", and `<playername>`, which was swallowed as a tag, is now `&lt;playername&gt;`.

Questions:
1. do_extinguish, 2960: `torch.value[0] := 0` throws away the hours left, so the light can never be lit again. Help Extinguish and Help Light say extinguishing saves what is left for later. This is already known: 7f9707ce reverted a fix, because relighting before the hourly tick would let a light burn forever. Either change the two help pages, or keep the hours less one (an hour off each time it is put out). Left: the fix is in the help pages Help Extinguish / Help Light (vme/etc/help/strings/extinguish.hlp, lantern.hlp), which are outside the zones of this batch; the edit was blocked. They still claim the hours left are saved.
2. do_split, 8795: when the splitter follows a leader, `kids` counts every grouped PC and NPC in the room, visible or not. The coins go only to visible PCs (SPLIT_CHARS), so with a grouped pet or an invisible member each share is smaller and the splitter keeps the rest. The leader's branch counts visible PCs only. Suggest counting the same way in both branches (foreach SPLIT_CHARS, visible). Fixed: the follower's branch of do_split now counts visible grouped PCs only (foreach SPLIT_CHARS plus visible).
3. do_areas, 860-890: the newbie-area table can't be reached (it comes after `goto end`). Remove it, or give it an option (`areas newbie`)?
4. Templates that nothing uses: depre_donate (11068, the old donate, still with &[clan] codes), reply (respond runs reply@comm), whostatus, wizhelp, do_reroll ("Deprecated"), and add_spaces (commented out). Remove them?
5. hel (mobile, 12802): `dilcopy test1();` names a template that exists in no zone, and nothing loads hel@commands. Remove the mobile, or at least the dilcopy?

## Play-test follow-ups

- Drink containers: drink and taste subtract the amount from the container's weight, which
  LIQ_DEF() sets to empty weight + contents. 39 containers (11 public: udgaard 10, midwalls 1)
  write `weight N` after LIQ_DEF() and so leave the contents out; tasting such a bottle empty took
  it to weight -4. The code now stops at 0. Question: remove those `weight` lines (the bottles
  get heavier by their contents, as LIQ_DEF() means), or keep them?
