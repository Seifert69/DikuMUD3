# clan_koh: review questions

## Step 1

1. **htown** (line 100): "To reach the chest you must walk through the Veil of Truth", but the
   `veil` object (with `portal_clan`) is never loaded, here or elsewhere. Load it somewhere, or drop
   the sentence?
2. **guard**, **guard2**, **guard3** (lines 421, 453, 488): `rescue@function("cloaked figure")` and
   `teamwork@function("cloaked figure")`, but these guards are "shimmering figure"s. Do the guards
   still find each other (copied from a zone with cloaked guards)? Answered in step 2, question 1.

## Step 2

Fixed in place: nothing. The zone's own DIL (portal_midgaard, portal_clan) has no bug with a
plain fix; the helpers it copies (clans.zon, function.zon, boards.zon) take the arguments given.

Questions:
1. **guard, guard2, guard3** (lines 421-422, 453-454, 488-489, step 1 question 2): they don't find
   each other. `rescue@function` and `teamwork@function` match `"cloaked figure" in u.names`, and
   the guards' names are "shimmering figure", "shimmering guard", "figure", "guard", so no guard is
   ever rescued or assisted by another. (teamwork also only helps NPCs of other zones, function.zon
   line 8562, so it would not help these guards either way.) Change the argument to
   `"shimmering figure"` or `"figure"`? That makes the guards rescue each other, a harder clan hall.
2. **portal_midgaard** (line 255): when a clan member types `enter` without "gate" (e.g. `enter
   door`), the program falls from `:yes:` into `:lpc:` and `goto init`, leaving `pc` secured; harmless,
   but a `goto start` after the `if` would be what was meant. Also only `htown@clan_koh` members get
   the gate: everyone else's `enter gate` goes to the normal command. Intended? Fixed: `unsecure(pc); goto start;` after the `if` (nothing players see). The members-only gate is left as it is: who may use it is a design question.
