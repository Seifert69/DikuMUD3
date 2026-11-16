# helpzon: review questions

## Shared zone review

Fixed in place:
- **do_help, help area** (line 297): the zone search was two nested loops; when a zone's name
  started with the current zone's name but was longer (as "deathlink" does "death"), the inner
  loop stopped and the outer one never moved on: an endless loop. Now one loop, as `wizhelp notes`.
- **do_help** (line 317): "You're on your own" had no full stop.

Questions:
1. **do_help** (lines 279, 289): any start of "topics" or "area" is taken as those words, so
   `help a`, `help ar` and `help t` never reach the help pages that begin with those letters.
   Require at least two or three letters? Fixed: do_help needs at least 3 letters ("are",
   "top") for area and topics, so `help a`/`help ar`/`help t`/`help to` reach the help pages.
