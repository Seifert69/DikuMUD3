# barbarian: review questions

## Step 1

1. **zone header** (line 34): the `help` text held a commented-out block copied from the conjurer
   guild (Archcancellor, Tower of Mundane...) inside the string, so players could read it. I removed
   it. The help is now just the introduction. Should it list where the trainers are, like the other
   guilds' help?
2. **bloody_gauntlet** (line 1090): `$improved identify` says "+2 Str and +3 Hit", but the item
   gives Str +2, Hit +2 and Brain -1. Which is right?
3. **faeld** (line 922): alignment 1000, while every other barbarian has 0. Intended?
4. **hall_n** (line 493): the description ends in a sentence without a verb ("An assortment of
   objects from across the land, representing the far-ranging escapades ... necessary for living.").
   What was meant? Perhaps "An assortment of objects ... is on display".
5. **gate_guard** (line 655): "The barbarian guards appear to exist without physical match." Meant
   "without equal"/"without match"?
6. **nokill** (line 150): checks `pc.type` before `pc` is set, so the NPC check never works. DIL,
   for step 2.
