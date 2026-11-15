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
