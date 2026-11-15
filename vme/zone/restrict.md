# restrict: review questions

## Shared zone review

Fixed in place (all in code that nothing calls, see question 1):
- **is_restricted** (line 82): the weapon restriction took its numbers from the ability restriction
  (`ABILITY_R`); now `WEAPON_R`.
- **is_restricted** (line 125): the anti-guild restriction read the guild list of `GUILD_R`; now
  `ANTI_GUILD_R`.
- **is_restricted** (lines 148-151): alignment used `activator` (the function's caller has none) and
  `$2n` where the item is `$3n`; now `self` and `$3n`, with full stops.
- **hammer** (lines 326, 329): "an old  iron hammer" (two spaces), "realy", "rustted", "it is" ->
  "really", "rusted", "It is".

Questions:
1. **The whole zone**: nothing in vme/zone, ~/diku/Zones or the server calls `is_restricted`,
   `glds`, `anti_glds` or `ply`; item restrictions are done by the `*_restrict@function` templates.
   `is_restricted` also never sets `damage` or `percent` (so it would never hurt), passes the
   room (`self.outside`) to `IsInGuilds`, and shows negative numbers in "$2t skill needed". Remove
   the zone, or keep it as an example?
