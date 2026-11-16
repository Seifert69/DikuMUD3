# justice: review questions

## Shared zone review

Fixed in place:
- activate_accuse, line 908: the witness's report had no quotes ("$1n says, I would like to report a crime!"); now `'...'` like every other say.
- set_witness, line 974: "You just witnessed a crime committed by $1n." now "You have just witnessed ...", the line the accuse help page quotes.
- wanted_poster, line 1060: the "Wanted Alive" heading closed `</b></div></b>` (an extra `</b>`); now `</div></b>`, as the "Wanted Dead" heading.
- accuse, line 1172: the clickable crimes listed by a bare `accuse` opened `<a cmd='accuse #'>` and never closed it; `</a>` added.
- accuse, lines 1230 and 1236: "lets see" -> "let's see".
- accuse, line 1241: "Are you accusing of murder or stealing?" -> "Are you accusing them of murder or stealing?".
- accuse, line 1220: "What do you wish to accuse $3t of. Murder or stealing?" -> "of? Murder or stealing?" (unreachable today: crime is never "").

Questions:
1. die, line 554: `subextra(jd.extra, self.name)` is meant to "remove all entries from the justice database" for a dead player, but the database extras are named "$reward <symname> <idx> [juris]", never the player's name, so it removes nothing. raw_kill@death calls `rem_db_entry@justice(self, "$reward", "all")` right after, which does the job, so the line looks dead. Suggest deleting it (or confirming rem_db_entry is the intended path).
2. rem_db_entry, line 702 on: the "all" branch removes the criminal's database entries but never sends "save" to the database (the single-jurisdiction branch at line 732 does), so a reboot before the next crime brings the entries back. Suggest `sendto("save", jd);` before the `return` in the "all" branch. Fixed: as suggested.
3. accuse, line 1276: the first $witness memory of that criminal for that crime that belongs to another jurisdiction answers "out of my jurisdiction" at once, even when the same witness also has a memory of the same criminal and crime in this captain's jurisdiction further down the list. Suggest `goto skipit2` instead of `goto notmyproblem` there, and saying "out of my jurisdiction" only when no match in this jurisdiction was found. Fixed: another jurisdiction's memory is skipped and remembered; "out of my jurisdiction" comes only when none in this one matched.
4. accuse, line 1128: a sleeping captain is woken only by NPC accusers (line 1125); a player accusing a sleeping captain gets the full conversation from him in his sleep. Should a sleeping captain be "busy" (or woken) for players too?
5. wanted_poster, line 1042: a counter below 0 (the -1 that marks "in jail", see add_reward line 439) lists the prisoner under "Wanted Alive". Intended, or should prisoners be left off the poster?
6. activate_accuse, line 890: a crime_type other than murder or stealing only logs; crime_name stays "" and the NPC still walks to the captain and types "accuse <name> " (the captain then asks "Are you accusing them of murder or stealing?"). log_crime only passes CRIME_MURDER and CRIME_STEALING today, so it can't happen yet; suggest `goto cleanup` after the log in case CRIME_PK comes back. Fixed: `goto cleanup` after the log.
