# professions: review questions

## Shared zone review

Fixed in place:
- **SetupProf, AddSpec, AddNonSpecProf** (lines 326, 366, 404): the range check tested the local
  `i`, which is never set (always 0), instead of the profession argument, so a bad profession
  number went on to index past `PROFESSION_STRINGS`. Now tests `prof` / `newProf`.

Questions:
1. **SpecializeProf** (lines 497, 510): the text added to the `$profession` record reads
   "Specializing from Fighter to 12" / "Specializing from second profession to Thief to 5"
   (a number, and "to" twice). If anyone reads that record, use `ProfessionName` and
   "from X to Y"?
