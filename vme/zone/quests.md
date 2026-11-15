# quests: review questions

## Shared zone review

Fixed in place:
- qstOnComplete, lines 1634-1636: on completion it emptied and reused `sl`, its own explanation list, to add the "<quest> Complete" flag. The program keeps running on the NPC, so the next player to complete the quest got the explanation `{"<quest> Complete"}`: the NPC typed the flag name as a command instead of its lines (Boldin's four quests in gremlin.zon). It now uses its own list `qsl`. (qstWaitDone does the same with `sl`, but there the argument is a copy for that call, so it's harmless.)
- Explain comment, lines 391-393: the examples ended in "::" (`::$3n pats you...::$3n pats $1n...::`); split() then gives a fourth, empty part and Explain refuses the line ("not the two expected strings"). Examples fixed and a note added.
- Explainer comment, line 438, and qstAssign comment, line 980: said $1 is the NPC and $3 the player; the code (sact/act with pc as $1 and self as $3) and the examples ("say $1n, do you want this quest?") are the other way round. Comments corrected.

Questions:
1. qstSetDone, line 966: `exdp` is null when the player has no "<quest> Ongoing" flag, and `addstring(exdp.names, "$done")` then fails silently and the null is returned. Callers that test the result are fine (gremlin.zon calls it); suggest an `if (exdp == null) return (exdp);` for clarity.
2. qstItemsWanted, line 1229: the quest log line reads "You've given him <item>" with no full stop, and each item adds another such line. Text only; add a "." if wanted.
