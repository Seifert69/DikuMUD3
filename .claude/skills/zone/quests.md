# Quests

`vme/zone/quests.zon` is the quest library; `haon_dor.zon` (John, Mary) and `example.zon`
(the steward) use it. Read the comment above each `dilbegin` there before using a helper.

## A quest in two dilcopies

The steward in `example.zon` is a whole quest: she offers it, the player accepts, brings the
lamp, and gets the reward. Tried in the game.

```
#define QUEST_LAMP          "Steward's Lamp"
#define QUEST_LAMP_COMPLETE "Steward's Lamp Complete"

dilcopy qstSimpleQuest@quests(QUEST_LAMP,
         {"say Oh, it's so dark in here.", "say Would you fetch me the oil lamp?"}, // offer
         {"say Thank you, $1n."},                                                   // accepted
         "The steward of the Example Keep wants the oil lamp.",  // shown by `quests`
         {"say Take this badge."},                                                  // done
         100, 2 * COPPER_MULT, {"keep_badge@example"}, "");  // xp, money, items, prereq
dilcopy qstItemsWanted@quests(QUEST_LAMP, {"lamp"}, {""},
         {"say Ah, the lamp. Thank you, $1n."},           // one line per wanted item
         "say That's kind, $1n, but it's the oil lamp I need.");
```

- The NPC offers the quest when a player in the room types any command, if the NPC is
  awake, not fighting and can see them. `nod` or `say yes` accepts.
- **In the qst* string lists `$1n` is the player and `$3n` the NPC** (some comments in
  quests.zon say otherwise).
- `qstItemsWanted` takes what's given (SFB_DONE give) and destroys it. A wanted entry matches
  the item's symbol or one of its names as a whole word, ignoring case: `"lamp"` matches
  `{"oil lamp", "lamp"}` through its second name. `itemsl`, `itemfunc` and `thanksl` line up,
  one entry per item.
- Anything else given is handed back: with the `nothanks` line while the player is on the
  quest, otherwise "won't accept your item". With `nothanks ""` it's kept, without a word.
- Money is in iron pieces: write `n * COPPER_MULT`. Reward items are `"sym@zone"`, loaded,
  resized to the player if wearable, and given.
- A completed quest can't be taken again. Two `qstSimpleQuest` on one NPC both offer on the
  same command: one quest per NPC, or write the offer yourself (below).

## Quest state

- The helpers add `"<name> Ongoing"` to `pc.quests`, then replace it with
  `"<name> Complete"`. Test with `if (QUEST_LAMP_COMPLETE in activator.quests)`: on a list of
  extras, `in` is an exact name, ignoring case.
- Keep the names in `#define`s so the dilcopies, the gates and every check agree.
- The `quests` command lists each entry with its description, except names starting with
  `$`: use `"$..."` names for hidden flags.

## Gates and chains

- A way blocked until a quest is done: `vault_gate` in `example.zon`
  (`fnpri(FN_PRI_BLOCK)`, `wait(SFB_CMD, command(dir))`, `block` when the quest is missing).
- A teacher who teaches only after a quest:
  `dilcopy teach_q_block@function(QUEST_POT_COMPLETE, "say Not before you've helped Mary.");`
  next to the teacher's `special SFUN_TEACH_INIT ...` (teachers still use that special).
- The last argument of `qstSimpleQuest` is a condition to be offered the quest, as
  `"func@zone <int> <string>"`: `"hasQuest@quests 0 " + QUEST_X_COMPLETE` (a chain),
  `"NotInGuild@quests 0 " + GUILD_UDG_FIGHTER`. `+` and arithmetic work in dilcopy arguments.

## Writing the steps yourself

For a quest that branches or has several steps, use the pieces (signatures in quests.zon):
`qstWaitHero(qname, pc, sl, acptfunc, prereqfunc)` (offer; null if declined),
`qstAssign(qname, pc, sl, qdescr)` (mark it Ongoing), `qstWaitDone(qname, pc, sl, xp, gold,
itemsl)` and `qstOnComplete(qname, sl, xp, gold, itemsl)` (dilcopy it to reward as soon as
`quest$done` arrives). Mary in `haon_dor.zon` uses them.

- Helpers run inside the calling program, so they change its `heartbeat`.
- A `secure(pc, ...)` around the steps ends the quest talk when the player walks out.

## Talking

- `Explainer@quests(pc, sl)` says one line every 3 seconds; `"::text to the player::text
  to the room"` sends two different messages.
- `DailyRoutine@quests(sl, ...)` runs a list of actions in a loop; `"::walkto::room@zone"`
  walks there, `"::descr::$1n is ..."` changes how the NPC looks in the room.

## Hidden extras

Extras whose first name starts with `$` aren't shown by `look`; scripts and the game read
them. `extra {"$identify"} "..."` is what identify shows (the sword in `example.zon`);
`extra {"$reward", ...}` on a monster is its bounty for the justice system (see the orc chief
in `haon_dor.zon`). Read one with `exd := "$x" in u.extra;` then `exd.descr`.
