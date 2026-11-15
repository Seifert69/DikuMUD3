# fightersguild: review questions

## Step 1

1. **fight_belt** (line 972): `$improved identify` (and the comment above the item) say +1 Str and
   +2% sword skills; the belt gives Str +3 and sword +5. Text or stats?
2. **arthur** (line 684): `sendtext("Sir Arthur hands you a belt.<br/>", self)` sends the line to
   Sir Arthur, not to the player. Should it be `pc` (DIL, step 2)?
3. **bastardsword** (line 1087): the identify texts say Str +1, sword +3, craftsmanship 5 and magic
   bonus 3; the item is `WEAPON_DEF(... BONUS_AVERAGE, BONUS_AVERAGE)` with sword +1 only. Nothing
   loads it (its reset is commented out). Fix the text, the stats, or remove the item?
4. **tower** (line 463): the description doesn't mention the exit east to the bar. Add it?
