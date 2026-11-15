# update: review questions

## Shared zone review

Fixed in place:
- **regen_pc** (line 212): `isset(self.flags, PC_SPIRIT)` tested the unit flag with the same bit
  (UNIT_FL_SACRED) instead of the player flag; now `self.pcflags`, so a dead player (spirit) is
  skipped, as the C++ regeneration does (vmelimits.cpp).
- **gain_cond** (line 297): "$1n is very thirsty.</div> " had a stray `</div>` and space.
- **gain_cond** (line 304): the "dying of thirst" step tested `pc.full` instead of `pc.thirst`.
- **gain_cond** (line 307): trailing space in "$1n is dying of thirst. ".
- **gain_cond** (lines 343-349): "You sober up!" was tested before the drunk value was clamped at 0,
  so when the drop went below 0 (sleeping, or in a sacred room) the message never came. Now after.
- **gain_cond** (line 353): "light headed" -> "light-headed".
- **gain_cond** (line 355): "You are getting tipsy.</br>" -> `<br/>`.

Questions:
1. **send_prompt** (lines 673-674, 696-697): `%oppdiag%`/`%tankdiag%` divide by `max_hp/4`, which is 0
   for a creature with fewer than 4 max hit points (a division by zero in DIL). Use
   `if (x < 1) x := 1;`?
2. **disarm_pc / trip_pc** (lines 23-98): `block` after an SFB_COM wait does nothing, and the
   heartbeat is set but no tick is waited for. They work (an exec per combat round); is anything
   still using them? Nothing in vme/zone does.
