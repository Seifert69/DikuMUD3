# assassin: review questions

## Step 1

1. **back_room** (line 192): the description is a placeholder ("Well, I suppose you want
   something to actually be described here, huh?") and the room has no exits. Remove it, or write it?
2. **captain_dagger** (line 538): nothing loads the dagger and nothing sets the quest flag
   "Assassin Quest Ongoing" that it needs (searched public and private zones). Dead content: remove
   it, or is the Amir quest planned?
3. **dark_katana** / **dark_wakiz** (lines 727, 749): `$improved identify` says Dex +2, katana or
   wakizashi and hide +3%, craftsmanship 5 and magic 5, but both are
   `WEAPON_DEF(WPN_SCIMITAR, BONUS_AVERAGE, BONUS_AVERAGE)` with no transfers. Text or stats?
4. **master_trainer** (line 270): the line "The Master Assassin pulls some items ... gives them to
   you." is sent `TO_CHAR` with the master as `$1`, so the player never sees it (DIL, step 2).
5. **death_bringer** (line 670): the `death` DIL checks `IsInGuild@guilds(self, ...)` where `self`
   is the sword, so the life-drain message never fires (DIL, step 2).
