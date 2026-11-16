# youth: review questions

## Step 1

1. **wide_pass** (line 404): the room has the same `ceiling/tafoni/honeycomb` and `glyphs/glyph/wall`
   extras as chamber, word for word, but its description has the sky overhead and mentions neither a
   ceiling nor glyphs (the glyphs are on chamber's south wall). Remove the two extras from wide_pass, or
   give it its own? Fixed (text follows the world): removed the ceiling and glyph extras from wide_pass.

## Step 2

Fixed in place:
- **isAgeDead** (lines 71-77): tested `self` instead of its `pc` argument. It worked only because
  `login_modify@basis` runs on the player itself.
- **save_spring** (lines 336, 343): "You command $1n into/out of existence." was sent `TO_ALL` with the
  spring as `$1`, i.e. to the spring's room, not to the god who typed `togglespring`. Now to the god
  (`TO_CHAR`, spring as `$2`).
- **save_spring** (line 350): it stored itself as `spring_saveobj.youth`, but `load_save` restores
  `spring_saveobj`, so `togglespring`/`savespring` never survived a reboot. Now stores
  `spring_saveobj`.

1. **change_age** (lines 156-161): the `years` argument is ignored: line 158 computes the new birth
   from it, and line 159 overwrites it with "lifespan minus 50 years". It also acts only on players who
   are already age-dead. Nothing calls it (public or private zones). Use `years` (drop line 159), keep it
   as it is, or remove the template?
2. **restore_youth** (line 220): the drink is caught only when the argument is one of the spring's
   names (`drink spring`, `drink pool`); `drink from spring` falls through to the ordinary drink command,
   which says the spring is not a drink. Accept a leading "from " as well? Returned to Papi: it makes the restoring drink easier to get (hands players something more easily).

Added after review: **save_spring** (line 324): (Withdrawn: running `interrupt()` again replaces the
interrupt with the same label; it doesn't add another.)

### Precedence

Since #603 (merged 2026-11-15) `and`/`or` bind as in C (looser than the comparisons), so once the
zones are rebuilt these conditions do what was written. What that changes in play is on Papi's
review list (#596's table).

None found.
