# DikuMUD3 DIL (DikuMUD Interactive Language) Reference

This document provides a comprehensive AGENT.md reference for DIL - the scripting language used in DikuMUD3 for creating interactive game content, NPCs, objects and complex game systems. 

## Compiling a zone
To compile a zone: cd vme/zone/ && vme/bin/vmc -I../include/ zonename.zon

## Zones with good DIL examples
vme/zone/haon-dor.zon
vme/zone/udgaard.zon
vme/zone/midgaard.zon
vme/zone/cypress.zon

## Quest helper DIL functions
Look in vme/zone/quests.zon for the DIL library of quest helper functions broadly exemplified in vme/zone/haon-dor.zon.
Look in vme/zone/randomt.zon for generating random treasure in DIL.

## 📚 **Core Documentation**
**🔗 Quick Access**: This condensed list covers all 315+ DIL functions. Each entry is reference their respective .wiki files in the DikuMUD3 DIL Manual documentation system. [string] for example, if explained in vme/src/mcp/string.wiki

## 🔧 **Language Overview**

### Data Types
- **[integer]** - Numeric data type: `health := 100; damage := 25; remaining := health - damage;`
- **[string]** - Text data type: `name := "Player"; message := name + " has " + itoa(health) + " HP";`
- **[unitptr]** - Unit pointer: `target := activator; npc := findunit("guard", UNIT_ST_NPC);`
- **[integerlist]** - Integer collections: `skills := {5, 3, 6, 9}; level := skills.[2]; // == 6`
- **[stringlist]** - String collections: `names := {"Alice","Bob","Charlie"}; first := names.[0];`
- **[null]** - Null value: `if (self.fighting == null) { /* not fighting anyone */ }`

### Type Conversion
- **[atoi]** - String to integer: `num := atoi("42"); // num == 42`
- **[itoa]** - Integer to string: `str := itoa(100); // str == "100"`

### Built-in Variables
- **[self]** - Current executing unit: `hp_pct := (self.hp * 100) / self.max_hp;`
- **[activator]** - Unit that triggered program: `if (activator.level >= IMMORTAL_LEVEL) { /* admin access */ }`
- **[argument]** - Command arguments: `wait(SFB_CMD, command("hello")); if (argument != "") { send("Hello " + argument + "!"); }`
- **[command]** - Command name: `if (command("north")) { act("You go north.", A_ALWAYS, self, null, null, TO_CHAR); }`
- **[cmdstr]** - Full command string: `full_cmd := cmdstr + " " + argument; act("Full: " + full_cmd, A_ALWAYS, self, null, null, TO_CHAR);`
- **[cmdptr]** - Command pointer: `cmd := getcommand("say"); if (cmd) { send("Command level: " + itoa(cmd.level), self); }`
- **[excmdstr]** - Command string (lowercase): `if (excmdstr != "quit") { sendtext("You must type 'quit' to quit.", self); }`
- **[excmdstr_case]** - Command string (case-sensitive): `if (length(excmdstr_case) < 5) { sendtext("Password too short.", self); }`
- **[heartbeat]** - Program timing: `heartbeat := PULSE_SEC * 10; // Tick every 10 seconds`

### Control Structures
- **[if]** - Conditional statement: `if (self.hp > 10) { exec("say Hehe!", self); } else { exec("say ouch!", self); }`
- **[while]** - Loop construct: `while (self.inside) { if (self.position & POSITION_SLEEPING) break; pause; }`
- **[foreach]** - Loop construct: `foreach (UNIT_ST_PC|UNIT_ST_NPC, u) { if (u.hp < u.max_hp) { u.hp := u.hp + 6; } pause; }`
- **[goto]** - Unconditional jump: `:main_loop: pause; goto main_loop; // Create infinite loop`
- **[on_goto]** - Goto construct: `on direction goto (north, south, east, west); // 0=north, 1=south, etc.`
- **[break]** - Exit loop: `foreach (UNIT_ST_PC, target) { if (target.hp < target.max_hp / 2) { break; } }`
- **[continue]** - Continue iteration: `foreach (UNIT_ST_OBJ, item) { if (not isset(item.manipulate, MANIPULATE_TAKE)) continue; }`
- **[return]** - Return from function: `return(damage); // Functions: return value; Procedures: return;`
- **[quit]** - Terminate program: `if (input == "exit") { sendtext("Goodbye!", self); quit; }`
- **[block]** - Block commands: `wait(SFB_CMD, command("forbidden")); block; send("Command blocked!", self);`
**⚠️ IMPORTANT:** DIL does NOT support C-style `for` loops. Use `while` loops instead.

### Operators
- **[and]** - Logical operators: `if (self.level >= 10 and self.hp > 50) { /* ready for combat */ }`
- **[or]** - Logical operators: `if (self.level < 10 or self.hp <= 0) { /* not ready */ }`
- **[not]** - Logical operators: `if (not (self.fighting)) { /* not in combat */ }`
- **[set]** - Bit operations: `set(self.pcflags, PC_WIZARD); // Set wizard mode flag`
- **[unset]** - Bit operations: `unset(self.charflags, CHAR_SELF_DEFENCE); // Clear defence flag`
- **[isset]** - Bit operations: `if (isset(self.flags, UNIT_FL_BURIED)) { /* item is buried */ }`

### Unit Fields
- **[inside]** - Unit positioning: `item := chest.inside; if (item != null) { sendtext("Found: " + item.name, self); }`
- **[outside]** - Unit positioning: `container := item.outside; if (container != null) { act("$1n is in $2n", A_ALWAYS, self, item, container, TO_CHAR); }`
- **[bright]** - Lighting: `torch_brightness := bright(self); sendtext("Torch brightness: " + itoa(torch_brightness), self);`
- **[light]** - Lighting: `light_count := self.light; sendtext("This unit contains " + itoa(light_count) + " light sources.", self);`
- **[illum]** - Contained light: `if (container.illum > 0) { sendtext("Container glows from within.", self); }`
- **[name]** - Object identification: `item_name := item.name; sendtext("This is called: " + item_name, self);`
- **[title]** - Object identification: `obj.title := "A shiny sword"; sendtext("Item title updated to: " + obj.title, self);`
- **[descr]** - Object identification: `extra.descr := "This item glows with magical energy."; // Set extra description`
- **[inside_descr]** - Internal description: `room.inside_descr := "This room has been modified."; // Change room description`
- **[weight]** - Weight: `total_weight := pc.weight; capacity := pc.capacity; if (total_weight > capacity) { /* overburdened */ }`
- **[baseweight]** - Weight: `empty_weight := baseweight(chest); // Get chest weight without contents`
- **[capacity]** - Carrying capacity: `max_cap := capacity(self); remaining := max_cap - weight(self); send("Capacity: " + itoa(remaining) + " remaining", self);`
- **[flags]** - Unit flags: `if (isset(self.flags, UNIT_FL_INVISIBLE)) { unset(self.flags, UNIT_FL_INVISIBLE); act("You become visible!", A_ALWAYS, self, null, null, TO_CHAR); }`
- **[idx]** - Unique ID: `guard_id := guard.idx; // Get unique identifier for tracking`
- **[key]** - Lock identifier: `if (item.key != "") { sendtext("Key required: " + item.key, self); }`

### Object Fields
- **[value]** - Economics: `weapon_damage := weapon.value[1]; // Get damage from weapon value array`
- **[cost]** - Economics: `obj.cost := 1000; obj.rent := 10;`
- **[rent]** - Economics: `obj.cost := 1000; obj.rent := 10;`
- **[objectflags]** - Classification: `if (isset(item.objectflags, OBJ_MAGIC)) { act("$1n glows with magic!", A_ALWAYS, item, null, null, TO_ROOM); }`
- **[objecttype]** - Classification: `if (obj.flags & OBJ_FL_MAGIC) { /* magic item */ }`

### Room Fields
- **[roomflags]** - Room properties: `if (isset(my_room.roomflags, ROOM_FL_DARK)) { sendtext("This room is dark.", self); }`
- **[exit_to]** - Room exits: `if (room.exit_to[NORTH] != null) { sendtext("There is an exit to the north.", self); }`
- **[exit_names]** - Room exits: `names := room.exit_names[NORTH]; if (length(names) > 0) { act("Exit: " + names.[0], A_ALWAYS, self, null, null, TO_CHAR); }`
- **[mapx]** - Coordinates: `if (room.mapx != -1 and room.mapy != -1) { act("Room coordinates: X=$2d, Y=$3d", A_ALWAYS, self, room.mapx, room.mapy, TO_CHAR); }`
- **[mapy]** - Coordinates: `distance := (room2.mapx - room1.mapx) + (room2.mapy - room1.mapy);`
- **[exit_diff]** - Exit difficulty: `difficulty := room.exit_diff[NORTH]; if (skillcheck(SKI_CLIMB, ABIL_DEX, difficulty) > 0) { sendtext("You climb successfully!", self); }`
- **[exit_info]** - Exit flags: `if (isset(room.exit_info[NORTH], EX_CLOSED)) { unset(room.exit_info[NORTH], EX_CLOSED); act("Door opens!", A_ALWAYS, self, null, null, TO_ALL); }`
- **[exit_key]** - Exit key: `key_name := room.exit_key[NORTH]; if (key_name != "" and findsymbolic(key_name, FIND_UNIT_IN_ME)) { sendtext("You have the key!", self); }`

### Character Fields
- **[level]** - Character levels: `if (self.level >= 10) { sendtext("You are experienced!", self); }`
- **[vlevel]** - Character levels: `lvl := self.level; exp := self.exp;`
- **[exp]** - Experience points: `xp := self.exp; sendtext("You have " + itoa(xp) + " experience points.", self); }`
- **[experience]** - Modify experience: `experience(1000, player); // Give 1000 XP for quest completion`
- **[exptol]** - Experience to level: `if (self.exptol <= 0) { sendtext("You can level up now!", self); }`
- **[hp]** - Health: `hp_pct := (self.hp * 100) / self.max_hp; if (self.hp < 10) { /* low health */ }`
- **[max_hp]** - Health: `hp_pct := (self.hp * 100) / self.max_hp; target.hp := target.max_hp; // Full heal`
- **[position]** - States: `if (self.position == POSITION_FIGHTING) { /* in combat */ } else if (self.position == POSITION_SLEEPING) { /* asleep */ }`
- **[defaultpos]** - States: `if (self.position == POSITION_FIGHTING) { /* in combat */ }`
- **[fighting]** - Combat: `opp := self.fighting; if (opp != null) { sendtext("Fighting " + opp.name, self); }`
- **[opponent]** - Combat: `opp := self.fighting;`
- **[master]** - Social relationships: `leader := self.master; if (leader != null) { act("You are following $1n.", A_ALWAYS, self, leader, null, TO_CHAR); }`
- **[follower]** - Social relationships: `first := self.follower; if (first != null) { act("$1n is your first follower.", A_ALWAYS, self, null, first, TO_CHAR); }`
- **[followercount]** - Social relationships: `count := self.followercount; if (count > 0) { sendtext("You have " + itoa(count) + " followers.", self); }`
- **[follow]** - Following control: `follow(prisoner, guard); // Make prisoner follow guard`
- **[abilities]** - Character attributes: `str := self.abilities[ABIL_STR]; dex := self.abilities[ABIL_DEX];`
- **[ability_points]** - Available points: `points := self.ability_points; if (points >= 5) { /* can train */ }`
- **[ability_costs]** - Training costs: `cost := self.ability_costs[ABIL_STR]; if (money >= cost) { /* can afford training */ }`
- **[ability_levels]** - Training requirements: `required := self.ability_levels[ABIL_STR]; if (level >= required) { /* can train */ }`
- **[skills]** - Skill system: `skill := self.skills.[SKI_BASH];`
- **[spells]** - Magic system: `spell := self.spells.[SPL_FIREBALL];`
- **[alignment]** - Moral standing: `align := alignment(self); if (align < -500) { send("You are evil!", self); }`
- **[birth]** - Character creation: `age_days := (realtime - birth(self)) / 86400; send("You are " + itoa(age_days) + " days old.", self); }`
- **[drunk]** - Character state: `if (self.drunk > 10) { sendtext("You are drunk!", self); }`
- **[full]** - Hunger level: `if (self.full > 20) { sendtext("You are very hungry.", self); } else if (self.full < 5) { sendtext("You feel quite full.", self); }`
- **[endurance]** - Physical stamina: `if (self.endurance < 2) { sendtext("You are exhausted!", self); } else { self.endurance := self.endurance - 1; }`
- **[charflags]** - Character flags: `if (isset(activator.charflags, CHAR_OUTLAW)) { send("You are an outlaw!", activator); }`
- **[crimes]** - Criminal record: `if (activator.crimes > 0) { send("You have " + itoa(activator.crimes) + " crimes on record.", activator); }`
- **[height]** - Physical size: `height_diff := pc.height - armor.height; // Check size compatibility`
- **[hometown]** - Player origin: `link(player, findroom(player.hometown)); // Send player home`
- **[lifespan]** - Maximum age: `if (age > self.lifespan) { sendtext("You have exceeded your lifespan!", self); }`

#### PC fields
- **[profession]** - PC properties: `if (activator.profession == PROFESSION_THIEF) { sendtext("Welcome, shadow walker!", activator); }`
- **[guild]** - PC properties: `if (activator.guild == "thief") { sendtext("Welcome, brother of the shadows!", activator); }`

#### NPC fields
- **[npcflags]** - NPC properties: `npcflags(npc, npc.npcflags | NPC_AGGRESSIVE); // Make NPC aggressive`

### String Processing
- **[length]** - Get string length: `l := length("hello"); // == 5`
- **[in]** - String/list membership: `if ("fox" in "The quick brown fox") { /* found substring */ }`
- **[left]** - Extract left characters: `l := left("hello", 2); // == "he"`
- **[right]** - Extract right characters: `r := right("world.txt", 3); // == "txt"`
- **[strcmp]** - String comparison: `if (strcmp(password, stored) == 0) { sendtext("Access granted.", self); }`
- **[replace]** - Replace text: `new := replace("fox", "cat", "The quick brown fox jumps over the lazy dog.");`
- **[getword]** - Extract first word: `s:="hello sam"; w := getword(s); // w="hello", s="sam"`
- **[toupper]** - Convert to uppercase: `shout := toupper("help!"); // == "HELP!"`
- **[tolower]** - Convert to lowercase: `answer := tolower(argument); if (answer == "yes") { sendtext("Great!", self); }`
- **[strncmp]** - Compare with length limit: `if (strncmp("hello", "help", 3) == 0) { sendtext("Strings match for first 3 chars.", self); }`

### Stringlist Processing
- **[addstring]** - Add to list: `mylist := addstring(mylist, "new_item"); // Add item to list`
- **[substring]** - Remove from stringlist: `sl:={"hello","world"}; substring(sl, "world") // sl now only contains "hello"`
- **[in]** - Stringlist membership: `if ("fox" in {"hello", "fox"}) { /* found substring */ }`
- **[split]** - Split into stringlist: `words := split("hello world this is a test", " ");`
- **[getwords]** - Get stringlist: `sl := getwords("hello world"); // sl is {"hello", "world"}, original preserved`

### Extra Description Functions
- **[addextra]** - Add descriptions: `addextra(item, 0, "A finely crafted sword with ancient runes."); // Add main description`
- **[delstr]** - Delete string files: `if (delstr("news.txt")) { sendtext("News file deleted.", self); }`
- **[extra]** - .extra field: `desc := item.extra.descr; // Get extra description text`
- **[extraptr]** - Extra description pointer: `ex := self.extra; if (ex != null) { sendtext("Found: " + ex.descr, self); }`

**⚠️ Critical Notes:**
- Stringlists use dot notation: `words.[0]` NOT `words[0]`
- Convert integers for concatenation: `"Value: " + itoa(num)`
- substring() removes from stringlists, doesn't extract substrings

## 💬 **Communication System**

### Communication Functions
- **[sendtext]** - Basic messaging: `sendtext("Hello, " + player_name + "!", target); // Send formatted text to specific player`
- **[pagestring]** - Paginated output: `pagestring("a very long string will be paged...", self);`
- **[prompt]** - User interface: `self.prompt := "[%n%h/%Hhp %m/%M]> "; // Set custom prompt with health/mana display`
- **[act]** - Message formatting: `act("You hit $3n for $2d damage!", A_ALWAYS, self, null, victim, damage, TO_CHAR);`
- **[sact]** - String formatting: `desc := sact("$1n is standing here.", A_SOMEONE, target, null, null, TO_CHAR); // Format message as string`

## ⚔️ **Combat System**

### Combat Functions
- **[set_fighting]** - Set combat: `set_fighting(self, intruder); // Start fighting intruder`
- **[stop_fighting]** - Stop combat: `stop_fighting(self, null); // Stop all combat`
- **[attack_type]** - Combat data: `current_attack := self.attack_type; sendtext("Your current attack type is: " + weapon_name(current_attack), self);`
- **[weapon_info]** - Combat data: `wpn_info := weapon_info(WPN_SWORD); if (wpn_info.[0] == 2) { sendtext("Two-handed weapon", self); }`
- **[weapon_name]** - Combat data: `name := weapon_name(WPN_SWORD); sendtext("Weapon type: " + name, self);`
- **[meleeattack]** - Attack actions: `damage := meleeattack(self, victim, 0, 0, TRUE); if (damage > 0) { act("You hit $1n for $2d damage!", A_ALWAYS, self, victim, damage, TO_CHAR); }`
- **[attack_spell]** - Attack actions: `damage := attack_spell(SPL_FIREBALL, caster, target, 0, ""); if (damage > 0) { act("Your fireball deals " + itoa(damage) + " damage!", A_ALWAYS, caster, null, target, damage, TO_CHAR); }`
- **[cast_spell]** - Spell casting: `damage := cast_spell(SPL_FIREBALL, self, self, target, 0, ""); if (damage > 0) { send("Fireball deals " + itoa(damage) + " damage!", self); }`
- **[meleedamage]** - Combat calculations: `damage := meleedamage(attacker, 50); // Calculate melee damage with base 50`
- **[defensive]** - Combat calculations: `defense_bonus := defender.defensive - attacker.defensive; if (defense_bonus > 50) { act("Superior defense!", A_ALWAYS, defender, null, null, TO_CHAR); }`
- **[offensive]** - Combat calculations: `off_bonus := target.offensive - attacker.offensive; if (off_bonus > 20) { act("Significant offensive advantage!", A_ALWAYS, attacker, null, null, TO_CHAR); }`
- **[change_speed]** - Combat timing: `change_speed(target, 12); // Delay target's next combat action by 12 pulses`

## ✨ **Affects & Magic System**

### Affect Functions
- **[addaff]** - Add affects: `addaff(player, ID_SANCTUARY, -1, 0, 0, 0, 0, 0, APF_DURATION | APF_FIRST, 0); // Permanent sanctuary`
- **[isaff]** - Check affects: `if (isaff(player, ID_BLIND)) { send("You are blind!", player); }`
- **[getaffects]** - Get all affects: `affects := getaffects(player); // Get list of all active affects`

## 💰 **Economic System**

### Money Management
- **[purse]** - Character money: `gold := purse(player, GOLD_PIECE); if (gold >= 100) { sendtext("You have enough gold!", player); }`
- **[transfermoney]** - Money operations: `success := transfermoney(buyer, seller, 1000); // Transfer 1000 coins`
- **[moneystring]** - Money operations: `price := moneystring(2500, 1); // "25 gold pieces" or "25 gp"`

## ⏰ **Time & Calendar**

### Time Functions
- **[mudhour]** - Game time: `hour := mudhour(); if (hour >= 6 and hour < 18) { sendtext("Shop is open!", self); }`
- **[mudday]** - Game time: `h := mudhour(); d := mudday(); m := mudmonth(); y := mudyear();`
- **[mudmonth]** - Game time: `h := mudhour(); d := mudday(); m := mudmonth(); y := mudyear();`
- **[mudyear]** - Game time: `h := mudhour(); d := mudday(); m := mudmonth(); y := mudyear();`
- **[asctime]** - Format time string: `time_str := asctime(); // "Mon Dec  5 12:34:56 2025"`
- **[realtime]** - Real time: `start_time := realtime(); // Record start time in seconds`
- **[wait]** - Time control: `wait(SFB_CMD, command("hello")); // Wait for "hello" command`
- **[sleep]** - Sleep control: `sleep; // Put unit to sleep`

## 🛠️ **Utility Functions**

### Mathematics
- **[rnd]** - Random number: `num := rnd(1, 100); // Random 1-100`
- **[openroll]** - Open roll: `roll := openroll(100, 5); // Open-ended 1-100 roll with 5% continuation chance`
- **[max]** - Maximum: `result := max(a, b); // Larger of two numbers`
- **[min]** - Minimum: `result := min(a, b); // Smaller of two numbers`
- **[abs]** - Absolute: `result := abs(-5); // Returns 5`

### System Utilities
- **[log]** - Logging: `log("Player " + self.name + " completed quest at " + asctime());`
- **[flog]** - File logging: `flog("quests.log", self.name + " completed quest: " + quest_name, "a");`
- **[logcrime]** - Crime logging: `logcrime(thief, victim, CRIME_STEALING);`
- **[info]** - Player info: `email_expd := "$email" in pc.info; if (email_expd != null) email := email_expd.descr;`
- **[hasfunc]** - Function check: `if (item.hasfunc == 1) { act("$1n has special powers.", A_ALWAYS, item, null, null, TO_ROOM); }`
- **[access]** - Access control: `if (access(player, restricted_zone) >= 3) { sendtext("Access granted!", player); }`
- **[reboot]** - System restart: `act("Rebooting MUD now...", A_ALWAYS, self, null, null, TO_ALL); reboot(0);`
- **[dispatch]** - External messaging: `dispatch("discord msg #bugs @" + self.name + " bug report: " + report_text); // Send to external dispatcher`
- **[gamestate]** - Player state: `gamestate(player, GS_MENU); // Put player in menu mode`
- **[getcmd]** - Command lookup: `cmd := getcmd("say"); if (cmd) { sendtext("Command level: " + itoa(cmd.level), self); }`
- **[getinteger]** - System values: `mana_reg := getinteger(DIL_GINT_MANAREG, player, 0); // Get mana regen rate`

## 📋 **Unit Operations**

### Finding & Managing Units
- **[findunit]** - Unit search: `target := findunit(self, "sword", FIND_UNIT_INVEN, null); // Find sword in inventory`
- **[findsymbolic]** - Unit search: `target := findsymbolic("sword@midgaard"); if (target != null) { act("Found: $1n", A_ALWAYS, self, target, null, TO_CHAR); }`
- **[findrndunit]** - Unit search: `random_pc := findrndunit(self, FIND_UNIT_ZONE, UNIT_ST_PC); if (random_pc != null) { act("Random player: $1n", A_ALWAYS, self, random_pc, null, TO_CHAR); }`
- **[delunit]** - Unit lifecycle: `if (delunit("temp_guardian.unit")) { act("Temporary unit deleted.", A_ALWAYS, self, null, null, TO_CHAR); }`
- **[destroy]** - Unit lifecycle: `item := findunit(self, "sword", FIND_UNIT_IN_ME, null); if (item) { destroy(item); act("Item destroyed!", self, null, null, TO_CHAR); }`
- **[clone]** - Unit lifecycle: `copy := clone(original_item); if (copy != null) { link(copy, self); }`
- **[getfollower]** - Follower access: `follower := getfollower(leader, 0); // Get first follower`
- **[getopponent]** - Opponent access: `opponent := getopponent(character, 0); // Get first opponent`

### Unit Validation
- **[can_carry]** - Carry validation: `result := can_carry(player, item, 1); if (result == 0) { /* can carry */ }`


### Finding & Managing Rooms
- **[findroom]** - Room operations: `room := findroom("temple@midgaard"); if (room != null) { link(self, room); }`
- **[pathto]** - Room operations: `direction := pathto(self, target_room); // Get direction to target`
- **[setroomexit]** - Room connections: `setroomexit(room, DIR_NORTH, target_room); // Create north exit`
- **[lastroom]** - Previous location: `prev_room := lastroom(player); if (prev_room) { sendtext("You came from: " + prev_room.name, player); }`

### Object Management
- **[equip]** - Get Equipment: `weapon := equip(self, WEAR_HANDS); if (weapon != null) { sendtext("Wielding: " + weapon.name, self); }`
- **[unequip]** - Remove Equipment: `weapon := equipment(self, WEAR_WIELD); if (weapon != null) { unequip(weapon); act("You unequip " + weapon.title, A_ALWAYS, self, null, null, TO_CHAR); }` 
- **[equipment]** - Check Equipment: `weapon := equipment(player, WEAR_WIELD); if (weapon != null) { send("You are wielding " + weapon.name, player); } else { send("You are not wielding anything.", player); }`
- **[addequip]** - Add Equipment: `ring := load("ring@midgaard"); if (ring != null) { addequip(ring, WEAR_FINGER_L); }`
- **[fits]** - Equipment validation: `reason := fits(player, armor, -1); if (reason != "") { sendtext("Armor doesn't fit: " + reason, player); }`

### Movement & Linking & Lighting
- **[link]** - Unit positioning: `link(item, player); // Move item to player's inventory`
- **[remove]** - Inventory management: `remove(mylist, 2); // Remove element at index 2`
- **[insert]** - Inventory management: `insert(mylist, 1, "new_item"); // Insert at position 1`
- **[islight]** - Lighting: `if (islight(self)) { sendtext("This item provides light.", self); }`


## 🎭 **DIL Program Management**

### Program Structure
- **[dilbegin]** - Program definition: `dilbegin my_function(param : string);`
- **[var]** - Variable declaration: `dilbegin f(); var i : integer; name : string; code { /* program logic here */ } dilend`
- **[external]** - External function: `dilbegin f(); external integer my_func@function(u : unitptr); code { /* program logic here */ } dilend`
- **[code]** - Code block: `dilbegin f(); code { /* program logic here */ } dilend`
- **[dilend]** - Program definition: `} dilend`

### Program Control
- **[priority]** - Execution control: `priority; // Block other DIL programs until nopriority called`
- **[nopriority]** - Execution control: `nopriority(); // Allow other DIL programs to execute again`
- **[waitnoop]** - Program flow: `waitnoop; // Temporarily release secure constraints for command execution`

### Event Handling
- **[secure]** - Security control: `secure(victim, victim_fled); // Jump to victim_fled: if target disappears`
- **[unsecure]** - Security control: `secure(item, :item_lost); // Process item safely; unsecure(item); // Remove protection when done` 
- **[on_activation]** - Event handlers: `on_activation((self.position <= POSITION_SLEEPING) or (self.position == POSITION_FIGHTING), skip); // Skip if asleep or fighting`
- **[interrupt]** - Program flow: `intr_id := interrupt(SFB_MSG, argument == "help", help_handler); // Handle help requests`
- **[clear]** - Program flow: `clear(intridx); // Remove interrupt handler by index`

### DIL Operations
- **[dilcall]** - Program management: `success := dilcall("heal_spell", self, 0, "minor"); // Call DIL template`
- **[dilfind]** - Program management: `if (dilfind("quest_active", player)) { sendtext("Quest already in progress.", self); }`
- **[dilcopy]** - Program management: `new_prog := dilcopy("guard_ai", self); // Create guard AI instance`
- **[dildestroy]** - Program management: `count := dildestroy("temp_effect", self); // Remove temporary effect`
- **[sendtoalldil]** - Inter-program communication: `sendtoalldil("SYSTEM: Reboot in 5 minutes!", "admin_*"); // Alert all admins`
- **[store]** - Inter-program communication: `store(chest, "chest_backup." + chest.zoneidx, TRUE); // Save chest and contents`

### DIL Message Functions
- **[send]** - Inter-program messaging: `send("task_complete"); pause; send("cleanup_ready"); // Send to waiting DIL programs with SFB_MSG`
- **[sendto]** - Basic messaging: `sendto("The sword glows with ancient power.", self); // Send to all DIL programs within a unit`
- **[sendtoall]** - Broadcast functions: `sendtoall("SYSTEM SHUTDOWN IN 10 MINUTES!", "sys_control"); // Send to all matching DIL programs globally`
- **[sendtoalldil]** - Broadcast functions: `sendtoall("NEW SPELL AVAILABLE: fireball", "spell_*"); // Send to specific DIL programs by pattern`

### PC multi-line editing function
- **[beginedit]** - Start editing mode: `beginedit(self); wait(SFB_EDIT, self == activator); // Get edited text in argument`
- **[killedit]** - Force exit editing: `killedit(self); // Stop editing session immediately`
- **[editing]** - Check editing status: `if (self.editing) { sendtext("Already editing!", self); }`

### Development Tools
- **[shell]** - Execute system command: `result := shell("backup_script.sh"); // Returns 0 if thread created`
- **[exec]** - Execute a command: `exec("say Hello there!", target); // Force target char to speak`

## 🌍 **World Management**

### Zone management
- **[findzone]** - Zone operations: `zone := findzone("midgaard"); if (zone != null) { sendtext("Found zone: " + zone.name, self); }`
- **[zoneidx]** - Zone properties: `unit_id := self.zoneidx; // Returns "name@zone" format`
- **[resetmode]** - Zone properties: `mode := zone.resetmode; // Get zone reset mode (read-only)`
- **[resettime]** - Zone properties: `zone.resettime := 30; // Set reset interval to 30 minutes`
- **[zonereset]** - Zone resets: `zonereset(zone); // Reset all NPCs and objects in zone`
- **[help]** - Zone information: `target_zone := findzone(zone_name); help_text := target_zone.help; act(help_text, A_ALWAYS, self, null, null, TO_CHAR);`
- **[roomcount]** - Zone properties: `count := zone.roomcount; // Get number of rooms in zone`
- **[rooms]** - Zone properties: `room := zone.rooms; while (room) { sendtext(room.name, self); room := room.next; }`
- **[npcs]** - Zone properties: `npc := npcs(zone); while (npc) { sendtext(npc.name, self); npc := npc.next; }`
- **[npccount]** - Zone properties: `count := npccount(zone); // Get number of NPCs in zone`
- **[objs]** - Field Object operations: `zone := findzone("midgaard"); first_obj := zone.objs; while (first_obj != null) { act("Zone object: $1n", A_ALWAYS, self, first_obj, null, TO_CHAR); first_obj := first_obj.next; }`
- **[objcount]** - Object operations: `zone := findzone("midgaard"); count := zone.objcount; act("Zone has $1d objects", A_ALWAYS, self, count, null, TO_CHAR);`
- **[creators]** - Zone authors: `if (zone.creators) { foreach (name in zone.creators) { act("Creator: " + name, self); } }`
- **[fname]** - Zone filename: `zone_file := zone.fname; sendtext("Zone file: " + zone_file, self);`

### List management
- **[global_head]** - Inter-program communication: `first_unit := global_head(); // Get first unit in global list`
- **[room_head]** - Room operations: `room := room_head(); while (room) { sendtext(room.name, self); room := room.next; }`
- **[obj_head]** - Object operations: `first_obj := obj_head(); while (first_obj != null) { act("Object: $1n", A_ALWAYS, self, first_obj, null, TO_CHAR); first_obj := first_obj.next; }`
- **[pc_head]** - PC operations: 
- **[npc_head]** - NPC operations: `npc := npc_head(); while (npc) { sendtext(npc.name, self); npc := npc.next; }`
- **[zone_head]** - Zone operations: `first_zone := zone_head(); // Get first zone in global list`
- **[zhead]** - Zone operations: `zone := zhead(); while (zone) { sendtext(zone.name, self); zone := zone.next; }`
- **[chead]** - Command operations: `cmd := chead(); while (cmd) { act("Command: " + cmd.name, A_ALWAYS, self, null, null, TO_CHAR); cmd := cmd.next; }`
- **[command_head]** - Command list: `cmd := command_head(); while (cmd) { act("Found: " + cmd.name, self); cmd := cmd.next; }`
- **[ghead]** - Global list: `unit := ghead(); while (unit) { sendtext(unit.name, self); unit := unit.gnext; }` 

#### List Operations
- **[next]** - Next element: `item := item.next; // Move to next item in list`
- **[gnext]** - Global next: `unit := unit.gnext; // Move to next unit in global list`
- **[gprevious]** - Previous element: `prev_unit := unit.gprevious; // Get previous unit in global list`

## 🔐 **Security & Validation**

### Security Functions
- **[check_password]** - Account management: `if (check_password(player, argument)) { send("Password verified.", player); } else { send("Invalid password.", player); }`
- **[set_password]** - Account management: `set_password(player, "newpass123"); // Change player password`
- **[delete_player]** - Account management: `if (isplayer("troublemaker")) { delete_player("troublemaker"); }`
- **[access]** - Security control: `if (access(player, restricted_zone) >= 3) { sendtext("Access granted!", player); }`
- **[isplayer]** - Player validation: `if (not isplayer(player_name)) { sendtext("Player not found.", self); }`

### God Functions
- **[switch]** - Character control: `switch(self, target); sendtext("You have switched to " + target.name + ".", self);`

### Pay to Play functions (not in use)
- **[paycheck]** - Character money: `if (paycheck(shop_room, player)) { sendtext("Access granted!", player); }`
- **[acc_balance]** - Bank accounts: `balance := player.acc_balance; if (balance >= 10000) { sendtext("You have $100+ in your account!", player); }`
- **[acc_total]** - Bank accounts: `total_credit := player.acc_total; if (total_credit >= 50000) { sendtext("Premium account!", player); }`
- **[acc_modify]** - Bank accounts: `acc_modify(player, -5000); // Remove $50 from account`

### Color Functions (not in use)
- **[addcolor]** - Add colors: `addcolor(player, "&red"); // Add red to player's color palette`
- **[delcolor]** - Remove colors: `delcolor(player, "&red"); // Remove red from player's color palette`
- **[changecolor]** - Modify colors: `if (changecolor(player, "clan_color", "&c+w&bn")) { send("Clan color updated!", player); }`
- **[getcolor]** - Color processing: `processed := getcolor("&brBold red text&n"); // Process color codes`

