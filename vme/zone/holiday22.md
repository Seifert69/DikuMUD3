# holiday22: review questions

## Step 1

1. **zone header** (line 24): the zone's help says "Players should never be in here.", but Santa's
   study and workshop and Jolnir's Yule hall are rooms players are sent to on purpose (the pole,
   the Yule Wheel). Reword the help (e.g. "Santa's rooms; players reach them through the pole in
   Udgaard's square."), or keep it as an admin-only note?
2. **boom_box3, DIL prg3** (line 1803; for step 2): when the opener is lost, the trap gift says
   "A possible 500000 experience disappears forever!", although this box is the one that explodes
   and never gives experience; after `destroy(self)` the code also falls through into `:trap:`.
   Use the "The gift falls apart, and whatever was in it is lost." line of boom_box2/boom_box4,
   and `quit` after the destroy?
3. **santa, DIL santa_shout** (line 583): "Is that slay bells I hear already!" plays on
   "sleigh bells"; keep the pun as it is, or make it "Are those slay bells I hear already?"
4. **choco_heart** (line 2292): its names had "staff" (copied from some other unit), now
   removed, and the text says "a large chocolate reindeer-shaped heart". A heart shaped like a
   reindeer? Perhaps "a large chocolate heart with a reindeer on it" was meant.
