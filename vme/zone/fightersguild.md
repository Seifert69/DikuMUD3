# fightersguild: review questions

## Step 1

1. **fight_belt** (line 972): `$improved identify` (and the comment above the item) say +1 Str and
   +2% sword skills; the belt gives Str +3 and sword +5. Text or stats?
2. **arthur** (line 684): `sendtext("Sir Arthur hands you a belt.<br/>", self)` sends the line to
   Sir Arthur, not to the player. Should it be `pc` (DIL, step 2)? Fixed in step 2.
3. **bastardsword** (line 1087): the identify texts say Str +1, sword +3, craftsmanship 5 and magic
   bonus 3; the item is `WEAPON_DEF(... BONUS_AVERAGE, BONUS_AVERAGE)` with sword +1 only. Nothing
   loads it (its reset is commented out). Fix the text, the stats, or remove the item?
4. **tower** (line 463): the description doesn't mention the exit east to the bar. Add it? Fixed (text follows the world): tower description now mentions the opening east to the bar.

## Step 2

Fixed in place:
- **metallurgist** (lines 210-218): the craftsmanship words checked `value[1]` against the lower bound
  but `value[2]` (the magic bonus) against the upper one, so an item's craft was rated by its magic
  ("craft 0-2" etc. in the comments). All five now check `value[1]`.
- **fighter_give** (line 684): "Sir Arthur hands you a belt." went to Sir Arthur (`self`); now to the
  player.

1. **fighter_give** (line 681): every `worship` by a member loads a new fight_belt; nothing records
   that the belt was given (thieves_give keeps a `$Thieves Dirk given` flag and asks for an amethyst
   for a replacement). Intended, or add a flag like the thieves' guild?
2. **beltchat** (line 1005): the "sword" test is `value[0] > 10 and < 21`, which includes the dagger
   (20) and leaves out the claymore (53). Only the combat messages depend on it. Keep it, or test the
   sword types (11, 12, 14, 16-19, 53)?

### Precedence

Since #603 (merged 2026-11-15) `and`/`or` bind as in C (looser than the comparisons), so once the
zones are rebuilt these conditions do what was written. What that changes in play is on Papi's
review list (#596's table).

None found.
