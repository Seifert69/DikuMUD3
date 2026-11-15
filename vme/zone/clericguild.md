# clericguild: review questions

## Step 1

1. **garden2** (line 322): says the monastery's golden doors are to the east and that a path leads
   north and south, but garden2's exits are south (garden1) and west (garden3); the golden doors are
   in the foyer, reached from garden1. **garden3** (line 340): says the path continues "west and
   south", but its exits are east and west. Rewrite the directions to match the exits?
2. **heal_torc** (line 1114): `$identify` promises more Divine Contact, Divine sphere skill and
   strength ("strength of faith is reflected in your strength of arm"); the torc only transfers
   first aid +2. Text or stats?
3. **tuck** (line 558): titled "Father Tuck" but his description calls him "Brother Tuck" twice.
   Which one?
4. **morenae** (lines 915-918, 924): during the torc ritual three `sendtext(..., self.outside)`
   messages go to the room unit, which most likely nobody sees. DIL, for step 2: should they be
   `act(..., TO_ALL)`?
