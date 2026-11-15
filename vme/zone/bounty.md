# bounty: review questions

## Step 1

1. **guide, historian, blacksmith, jeweler, summoner, necromancer and their `2` copies** (line 6465 and
   on): the titles are "The Guide", "The Historian" and so on, and vmc warns "Title CASE seems to be
   wrong" for all twelve. The zone uses them as proper names everywhere ("Go and speak to The
   Historian"), so I left them. Keep them, or make them "the guide" etc. (which would read oddly in
   the DIL speech that names them)?
2. **qnecro2** (line 3442): when you hand over a rainbow crystal, the Necromancer tucks it "into the
   pocket of his sash" if `"skin"` is in the quest names, otherwise he pats his chest looking for a
   pocket. The sash is the `"sash"` entry; this looks like it should test `"sash"`. (DIL logic, so
   not changed.)
3. **qnecro2** (line 3421): it waits for `SUMMONER_ONGOING` rather than `NECRO_ONGOING`, so the
   Necromancer only takes items while the Summoner's task is open, and it reads
   `pc.quests.[NECRO_ONGOING]` without checking that it exists. Intended? (For step 2.)
4. **qsummoner2** (lines 2700, 2714 and on): the Summoner says "sets the "+item.title+" aside" /
   "tucks the "+item.title+" into her robes" after `destroy(item)`, so the item title may come out
   empty, and "the" before a title that already has an article reads "the a ...". Same pattern in
   qnecro2 ("tucks "+item.title). For step 2.
5. **tome** (line 6938): at boot the world-run log shows
   `load_string(): File does not exist: ../lib/clan/strings/bounty_names.data`. Expected until the
   first hero signs the tome, or should the file be created empty?
6. **zone header `notes`**: the developer notes (not shown to players) still hold the v1.0 to-do
   list ("500,000 lbs of steel ... Need to write ability to count", "Final reward will be a hero
   plate"). Keep as history, or trim to what is still open?
