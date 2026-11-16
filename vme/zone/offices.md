# offices: review questions

## Step 1

1. **newspaper2** (line 1002): in the Clan News article, the Advocates of Honor and Bushido
   sections both say "No reports of anything worthwhile or interesting happening in Unseen Chaos",
   copied from the Unseen Chaos section. Fix the clan names, or is the paper an archive to leave
   alone? Fixed (text follows the world): Advocates of Honor and Bushido sections now name their own clan.
2. **newspaper1/newspaper2**: the paper is the June 2011 issue (Facebook/MySpace/Twitter links,
   2011 competitions). Keep it as it is, or retire or replace it? I fixed only plain typos in it.
3. **hallway** (line 50): the description and the signs mention doorways north, east and west,
   but not the open exit south to `dil_room`. Add it to the text? Fixed (text follows the world): hallway description now mentions the open way south.
4. **hallway** (line 60): the west sign says 'Games Development', but the room west is the
   Conference Room, whose plaque says it is for the Valhalla Times staff. Which is right? Fixed (text follows the world): west sign now reads Conference Room.

## Step 2

Fixed in place:
- **shred** (line 223): the holiday card branch didn't test `target == self`, so a card put into any
  container near the shredder (the player's own bag) was shredded for its experience, and a card made
  out to someone else was pulled out of the bag. The card says "Place this greeting into the
  shredder"; now only the shredder takes it.

No new questions.

### Precedence

Since #603 (merged 2026-11-15) `and`/`or` bind as in C (looser than the comparisons), so once the
zones are rebuilt these conditions do what was written. What that changes in play is on Papi's
review list (#596's table).

None found.
