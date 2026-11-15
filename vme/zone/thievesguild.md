# thievesguild: review questions

## Step 1

1. **oliver** (line 368): the first female title is "Pilferers", the male one "Pilferer". A typo for
   "Pilferess" (like "Filcheress", "Pincheress"), or "Pilferer"?
2. **thief_dirk** (line 702): `$improved identify` says craftsmanship 4 and magic bonus 3, but the
   dirk is `BONUS_EXCELLENT` (7,7) and also has Cha -1, which the text doesn't mention. Text or stats?
3. **wcellar3** (line 175): the parchment says 'The Thieving Guiltae'. Deliberate (thieves'
   spelling) or a typo for "Guild"?

## Step 2

Fixed in place:
- **thieves_give** (lines 424, 447): the two member branches went back to waiting with the player
  still secured; a later departure of that player made the master jump to `:lost:` and shrug in the
  middle of another conversation. Added `unsecure(pc)`.
- **dirk_replace** (line 485): `secure(pc,lost)` was written twice; the second was meant for the
  gift. Without it the gift was null after the first `pause`, so the amethyst and the "bribes" were
  never destroyed (they piled up in Gryma's inventory) and "examines ... and hides it away" failed.
  Now `secure(gift,lost)`, with `unsecure(gift)` before each `destroy(gift)` (a destroy of a secured
  unit jumps to the label at once) and both released when a non-member's gift is ignored.
- **dirkpoison** (line 756): the dirk caught `press button` from anyone in the room (and blocked their
  command) as long as its owner wielded it. Now only the wielder's own command.

No new questions.

### Precedence

Since #603 (merged 2026-11-15) `and`/`or` bind as in C (looser than the comparisons), so once the
zones are rebuilt these conditions do what was written. What that changes in play is on Papi's
review list (#596's table).

None found.
