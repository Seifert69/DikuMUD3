# ranger: review questions

## Step 1

1. **for_gath1** (lines 80, 96): the description says the forest seems lighter to the east, and
   the exit north (to trail_6@talen) has the description "...it looks like the forest gets lighter to
   the east". There is no east exit. Is the north exit the way meant (then "east" should read
   "north"), or is an east exit missing? Fixed (text follows the world): for_gath1 description now says lighter to the north (the real exit).
2. **ranger_bow** (line 623): names say "Milvamer", the title says "Milivamar". Which spelling?
   (I added "Milivamar" to the names so the title word works.)
3. **seeker** / **guardian** (lines 522, 548): both Maliki and Sholdin include the `_SPELL`
   teacher; there's no second skill or ability teacher. Intended?
4. **master** (line 377): the lowest title is "Master of the Hunt", above "Scout"/"Guide". Is the
   first title in the wrong place?

## Step 2

Nothing fixed in place.

Questions:
1. **nokill** (line 29): unlike the paladin and mage guilds' nokill, it has no
   `if (pc.type == UNIT_ST_NPC) goto start;`, so an NPC fighting a ranger guild NPC (a pet, a
   summoned creature, a wandering monster) is beaten unconscious and thrown out to for_gath1 like a
   player. Skip NPCs as the other guilds do? Returned to Papi: the staff would then fight players' pets and summoned creatures to the end and could be killed by them (barbarian's nokill got the same change under the vmc warnings, noted there as "Changes play").
2. **ranger_dil** (line 423): every time a guild member bows to (or worships) the master, a new
   ranger bow is loaded and given, with no check for one given before. Intended, or once only (a
   quest flag)?
