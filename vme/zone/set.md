# set: review questions

## Shared zone review

The zone is the god command `nset` (level 254); everything below is god-facing.

Fixed in place:
- **search_block** (lines 785, 797): it built `s` from the argument with `_` turned into spaces,
  then compared the raw argument, so multi-word values ("category_sword", "PK_relaxed",
  "dead_R.I.P.") could never be given. It now compares `s`. The loop also ran one past the end of
  the string (an "index of stringlist out of bounds" line in the log for every word); it now stops
  at `length(arg)`.
- **do_set** (line 1119): the "field invalid for type of unit" test for character fields was
  `not(targ.type & UNIT_ST_PC|UNIT_ST_NPC)`; `&` binds tighter than `|`, so it was always non-zero
  and char fields (exp, mana, race...) were accepted on objects and rooms. Now
  `targ.type & (UNIT_ST_PC|UNIT_ST_NPC)`.
- **do_set** (line 1350): `if ((at < 1) and (at > 13))` was never true; now `or`.
- **do_set** (lines 1140, 1155, 1172, 1190, 1218, 1245) and **get_field_at** (lines 686-698):
  "Arg:<value>", "<bitlist>"... were sent as raw tags, which the clients swallow; now `&lt;` and
  `&gt;` as the other prompts already were.
- **do_set** (line 1891): natural-armor is a value field (`AT_VAL`) but was set from `typarg`,
  never filled for it, so it always set 0; now `valarg`.
- **do_set** (line 2283): after "Oops. Bad field!" it fell into `:lost:` and also said "Target
  lost."; now quits.
- **do_set** logs: "attemped" -> "attempted" (line 1792); the lifespan log said "age from" (line
  2137).

Questions:
1. **do_set key** (field 12, line 1513): it changes nothing (the assignment is commented out), sends
   the old key without `<br/>` and says "Key needed modified to ...". Make it say the field is
   disabled, like dir-key, access and the others?
2. **do_set log at `:end_set:`** (line 2326): for string fields it logs `strarg`, which only ins-name
   sets, so it logs "... to ." for title, guild-name and the rest. Logging `base_arg` instead would
   also write passwords (pwd is a string field) to vme.log, so pwd would need its own case. Wanted?
3. **UNIT_FL_STRUCT / NPC_FL_STRUCT** (lines 178, 324): the keywords are spelt "transparant" and
   "agressive". They are what a god types (prefixes work), so fixing the spelling changes the
   input; leave or fix?
4. **get_bit, get_mwords** (lines 850, 966 and the other `arg.[i]` loops): they read one character
   past the end of the string to stop, which logs "index of stringlist out of bounds" for every
   nset with a bit list. Stop at `length(arg)` as search_block now does?
5. **show_fields** (line 734): each row reads "add-name : on</div>... All. :" ("on" and ". :" look
   like leftovers of an old column layout). Tidy to "name / unit type / arguments"? Fixed: show_fields
   rows are now name | unit type | arguments.
