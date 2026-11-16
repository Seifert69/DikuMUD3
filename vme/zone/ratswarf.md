# ratswarf: review questions

## Step 1

1. **la_room1 / kingroad3** (lines 48-50, 137-138): the two sides of the cottage door name different
   keys. The la_room1 side uses `monkey_key` (this zone's monkey key, line 1959); the kingroad3 side
   uses `monkey_key@houses`, and no zone, public or private, has a `monkey_key`. Nothing loads
   `monkey_key@ratswarf` either, and nothing in the zone sets the door on either side. Is this
   player home still in use? If it is, should both sides use `monkey_key@ratswarf`, and who gets the
   key?
2. **dragon_heart** (line 664): no exit leads into this room. It has `north to tunnel3` and
   `down to cth_sanctum@midgaard`, but tunnel3 has no south exit, though tunnel3's text says "On the
   south wall, to the dragon's left, a warmth can be felt, followed by a feeling of pure evil". It
   also has two `flags` lines (`{UNIT_FL_SACRED}`, then the indoors/no-weather/private set). Is it a
   god's private room that is meant to have no way in? If players should reach it, it needs a south
   exit from tunnel3 (a hidden door, say). Also, should the two flags lines become one, so that
   UNIT_FL_SACRED is surely kept?
3. **zone header** (line 15): the zone's title is "Ratswarf@Mainland", but all the room and NPC text
   spells the town "Ratswharf" (I changed the three "Ratswarf"/"Rastwarf" in player text to
   "Ratswharf"). Should the title be "Ratswharf" too?
4. **slaver** (line 1433): the drunk slaver says "Thems wore the days -hic-." Is "wore" a slip for
   "were" ("Them were the days"), or meant as slurred speech? I left it.
5. **turning_point** (line 100): "All roads lead to Midgaard, they say, well, until the Great Dragon
   toasted it. Now, most roads lead to Midgaard II". Midgaard is just to the west and isn't toasted
   anywhere else; is this old lore that should go?

## Step 2

Fixed in place:
- **captainprg** (line 1663): when the Elven Commander wasn't in the room (at load, or after he died
  or left, through `:nobuddy:` and `goto init`), the captain went to `wait(SFB_MSG, activator==buddy)`
  with `buddy` null and never woke again, even when a new commander was loaded. Now it pauses and
  looks again.

1. **slaverprg** (lines 1398-1437): `walk_room@function` sets the caller's heartbeat to its walking
   speed (5 seconds) and leaves it there, so after his first walk the slaver's pauses are 5 seconds
   instead of the 20 he sets at the start, and he chatters four times as fast. Set
   `heartbeat := PULSE_SEC*20;` again after each `walk_room` call (or fix `walk_room`, which is
   shared)? Fixed: `heartbeat:=PULSE_SEC*20;` after each of the four `walk_room` calls (the shared `walk_room` left alone).

### Precedence

Since #603 (merged 2026-11-15) `and`/`or` bind as in C (looser than the comparisons), so once the
zones are rebuilt these conditions do what was written. What that changes in play is on Papi's
review list (#596's table).

None found.
