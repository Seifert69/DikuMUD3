# necro_guild: review questions

## Step 1

1. **dalmar** (line 1173): nothing loads Dalmar the Archnecromancer, and he is the only way into the
   golem quest (get_golem, got_stuff and golem_load all run on him), so `golem` and `golem_manual`
   never appear in the game. Is the golem quest retired, or should Dalmar be loaded somewhere (the
   underground laboratory fits his "preparing golem bodies")?
2. **murdock reset** (line 2389): Murdock is equipped with `black_breast` at WEAR_BODY and then
   `dk_blue_robe` at WEAR_BODY too, but the robe can only be worn about the body (WEAR_ABOUT), as
   meclor's reset does. Should the robe be `position WEAR_ABOUT`?
3. **kadiya reset** (line 2422): `black_robe` is equipped at WEAR_BODY, but the robe is
   MANIPULATE_WEAR_ABOUT (krain wears the same robe at WEAR_ABOUT). Should it be WEAR_ABOUT?
4. **dark_corridor** (line 301): the iron door to the west read "Murdoch - Enter at your own risk"
   while the weapon master behind it is Murdock everywhere else; I changed the door to "Murdock".
   Say if "Murdoch" was meant.
5. **golem_brain / golem_load** (lines 1036, 1189): the golem's text still speaks of a familiar here
   and there (I fixed "your familiar is ..." in the status report and the manual), and Dalmar's
   `:hagsecure:` line says "Fool probably could command it anyhow!". Should that be "couldn't"?
   (The golem quest is only live if question 1 brings Dalmar back.)
