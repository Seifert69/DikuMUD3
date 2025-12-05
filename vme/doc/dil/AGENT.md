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

### Control Structures
- **[if]** - Conditional statement: `if (self.hp > 10) { exec("say Hehe!", self); } else { exec("say ouch!", self); }`
- **[while]** - Loop construct: `while (self.inside) { if (self.position & POSITION_SLEEPING) break; pause; }`
- **[foreach]** - Loop construct: `foreach (UNIT_ST_PC|UNIT_ST_NPC, u) { if (u.hp < u.max_hp) { u.hp := u.hp + 6; } pause; }`
- **[goto]** - Unconditional jump: `:start: exec("say Hello", self); pause; goto start;`
- **[on_goto]** - Goto construct: `on direction goto (north, south, east, west); // 0=north, 1=south, etc.`
- **[break]** - Exit loop: `foreach (UNIT_ST_PC, target) { if (target.hp < target.max_hp / 2) { break; } }`
- **[continue]** - Continue iteration: `foreach (UNIT_ST_OBJ, item) { if (not isset(item.manipulate, MANIPULATE_TAKE)) continue; }`
- **[return]** - Return from function: `return(damage); // Functions: return value; Procedures: return;`
- **[quit]** - Terminate program: `if (input == "exit") { sendtext("Goodbye!", self); quit; }`
**⚠️ IMPORTANT:** DIL does NOT support C-style `for` loops. Use `while` loops instead.

### Operators
- **[and]** - Logical operators: `if (self.level >= 10 and self.hp > 50) { /* ready for combat */ }`
- **[or]** - Logical operators: `if (self.level < 10 or self.hp <= 0) { /* not ready */ }`
- **[not]** - Logical operators: `if (not (self.fighting)) { /* not in combat */ }`
- **[set]** - Bit operations: `set(self.pcflags, PC_WIZARD); // Set wizard mode flag`
- **[unset]** - Bit operations: `unset(self.charflags, CHAR_SELF_DEFENCE); // Clear defence flag`
- **[isset]** - Bit operations: `if (isset(self.flags, UNIT_FL_BURIED)) { /* item is buried */ }`

### Character System
- **[abilities]** - Character attributes: `str := self.abilities[ABIL_STR]; dex := self.abilities[ABIL_DEX];`
- **[ability_points]** - Available points: `points := self.ability_points; if (points >= 5) { /* can train */ }`
- **[skills]** - Skill system: `skill := self.skills.[SKI_BASH];`
- **[spells]** - Magic system: `spell := self.spells.[SPL_FIREBALL];`

### Object Fields
- **[name]** - Object identification: `item_name := item.name; sendtext("This is called: " + item_name, self);`
- **[title]** - Object identification: `obj.title := "A shiny sword"; sendtext("Item title updated to: " + obj.title, self);`
- **[descr]** - Object identification: `obj.title := "A shiny sword";`
- **[weight]** - Weight: `total_weight := pc.weight; capacity := pc.capacity; if (total_weight > capacity) { /* overburdened */ }`
- **[baseweight]** - Weight: `total := obj.weight * obj.count;`
- **[value]** - Economics: `damage := weapon.value[1]; // Get weapon damage from value array`
- **[cost]** - Economics: `obj.cost := 1000; obj.rent := 10;`
- **[rent]** - Economics: `obj.cost := 1000; obj.rent := 10;`
- **[objectflags]** - Classification: `if (isset(obj.objectflags, OBJ_NO_DUAL)) { sendtext("Too large for dual-wielding.", self); }`
- **[objecttype]** - Classification: `if (obj.flags & OBJ_FL_MAGIC) { /* magic item */ }`

### Room Fields
- **[roomflags]** - Room properties: `if (isset(my_room.roomflags, ROOM_FL_DARK)) { sendtext("This room is dark.", self); }`
- **[bright]** - Lighting: `torch_brightness := bright(self); sendtext("Torch brightness: " + itoa(torch_brightness), self);`
- **[light]** - Lighting: `light_count := self.light; sendtext("This unit contains " + itoa(light_count) + " light sources.", self);`
- **[islight]** - Lighting: `if (islight(self)) { sendtext("This item provides light.", self); }`
- **[exit_to]** - Room exits: `if (room.exit_to[NORTH] != null) { sendtext("There is an exit to the north.", self); }`
- **[exit_names]** - Room exits: `if (room.exit_to[DIR_NORTH]) { /* north exit */ }`
- **[mapx]** - Coordinates: `if (room.mapx != -1 and room.mapy != -1) { act("Room coordinates: X=$2d, Y=$3d", A_ALWAYS, self, room.mapx, room.mapy, TO_CHAR); }`
- **[mapy]** - Coordinates: `distance := (room2.mapx - room1.mapx) + (room2.mapy - room1.mapy);`

### Character Fields
- **[level]** - Character levels: `if (self.level >= 10) { sendtext("You are experienced!", self); }`
- **[vlevel]** - Character levels: `lvl := self.level; exp := self.exp;`
- **[hp]** - Health: `hp_pct := (self.hp * 100) / self.max_hp; if (self.hp < 10) { /* low health */ }`
- **[max_hp]** - Health: `hp_pct := (self.hp * 100) / self.max_hp; target.hp := target.max_hp; // Full heal`
- **[position]** - States: `if (self.position == POSITION_FIGHTING) { /* in combat */ } else if (self.position == POSITION_SLEEPING) { /* asleep */ }`
- **[defaultpos]** - States: `if (self.position == POSITION_FIGHTING) { /* in combat */ }`
- **[fighting]** - Combat: `opp := self.fighting; if (opp != null) { sendtext("Fighting " + opp.name, self); }`
- **[opponent]** - Combat: `opp := self.fighting;`
- **[master]** - Social relationships
- **[follower]** - Social relationships
- **[followercount]** - Social relationships

#### PC fields
- **[profession]** - PC properties
- **[guild]** - PC properties


## 💬 **Communication System**

### Communication Functions
- **[sendtext]** - Basic messaging: `sendtext("Hello, " + player_name + "!", target); // Send formatted text to specific player`
- **[pagestring]** - Paginated output: `pagestring(help_text, self); // Display multi-page content with pagination`
- **[prompt]** - User interface: `self.prompt := "[%n%h/%Hhp %m/%M]> "; // Set custom prompt with health/mana display`
- **[act]** - Message formatting: `act("You hit $3n for $2d damage!", A_ALWAYS, self, null, victim, damage, TO_CHAR);`
- **[sact]** - String formatting: `desc := sact("$1n is standing here.", A_SOMEONE, target, null, null, TO_CHAR);`

### Message Functions
- **[send]** - Inter-program messaging: `send("task_complete"); pause; send("cleanup_ready"); // Send to waiting DIL programs with SFB_MSG`
- **[sendto]** - Basic messaging: `sendto("The sword glows with ancient power.", self); // Send to all DIL programs within a unit`
- **[sendtoall]** - Broadcast functions: `sendtoall("SYSTEM SHUTDOWN IN 10 MINUTES!", "sys_control"); // Send to all matching DIL programs globally`
- **[sendtoalldil]** - Broadcast functions: `sendtoall("NEW SPELL AVAILABLE: fireball", "spell_*"); // Send to specific DIL programs by pattern`

### String Processing
- **[length]** - Get string length: `l := length("hello"); // == 5`
- **[in]** - String/list membership: `if ("fox" in "The quick brown fox") { /* found substring */ }`
- **[left]** - Extract left characters: `l := left("hello", 2); // == "he"`
- **[right]** - Extract right characters: `r := right("world.txt", 3); // == "txt"`
- **[strcmp]** - String comparison: `if (strcmp("apple", "banana") < 0) { sendtext("apple comes before banana.", self); }`
- **[replace]** - Replace text: `new := replace("The quick brown fox jumps over the lazy dog.", "cat", "text");`
- **[getword]** - Extract first word: `s:="hello sam"; w := getword(s);` (w == "hello", s =="sam")`
- **[toupper]** - Convert to uppercase: `u := toupper("hello"); // == "HELLO"`
- **[tolower]** - Convert to lowercase: `l := tolower("HellO"); // == "hello"`
- **[strncmp]** - Compare with length limit: `if (strncmp("hello", "help", 3) == 0) { sendtext("Strings match for first 3 chars.", self); }`

### Stringlist Processing
- **[substring]** - Remove from stringlist: `substring(mylist, "text")` (NOT extract substring)
- **[in]** - Stringlist membership: `if ("fox" in {"hello", "fox"}) { /* found substring */ }`
- **[split]** - Split into stringlist: `words := split("hello world this is a test", " ");`
- **[getwords]** - Get stringlist: `sl := getwords("hello world");` (sl is {"hello", "world"}, original preserved)

**⚠️ Critical Notes:**
- Stringlists use dot notation: `words.[0]` NOT `words[0]`
- Convert integers for concatenation: `"Value: " + itoa(num)`
- substring() removes from stringlists, doesn't extract substrings

## ⚔️ **Combat System**

### Combat Functions
- **[set_fighting]** - Set combat: `set_fighting(attacker, target);`
- **[stop_fighting]** - Stop combat: `stop_fighting(unit);`
- **[attack_type]** - Combat data: `current_attack := self.attack_type; sendtext("Your current attack type is: " + weapon_name(current_attack), self);`
- **[weapon_info]** - Combat data: `wpn_info := weapon_info(WPN_SWORD); hands := wpn_info.[0]; // 1=one-handed, 2=two-handed`
- **[weapon_name]** - Combat data: `name := weapon_name(WPN_SWORD); sendtext("Weapon type: " + name, self);`
- **[meleeattack]** - Attack actions: `damage := meleeattack(self, victim, 0, 0, TRUE); if (damage > 0) { act("You hit $1n for $2d damage!", A_ALWAYS, self, victim, damage, TO_CHAR); }`
- **[attack_spell]** - Attack actions: `damage := attack_spell(SPL_FIREBALL, caster, target, 0, ""); if (damage > 0) { act("Your fireball deals " + itoa(damage) + " damage!", A_ALWAYS, caster, null, target, damage, TO_CHAR); }`
- **[meleedamage]** - Combat calculations: `damage := meleedamage(attacker, 50); // Calculate melee damage with base 50`
- **[defensive]** - Combat calculations: `defense_bonus := defender.defensive - attacker.defensive; if (defense_bonus > 50) { act("Superior defense!", A_ALWAYS, defender, null, null, TO_CHAR); }`
- **[offensive]** - Combat calculations: `off_bonus := target.offensive - attacker.offensive; if (off_bonus > 20) { act("Significant offensive advantage!", A_ALWAYS, attacker, null, null, TO_CHAR); }`

## 💰 **Economic System**

### Money Management
- **[purse]** - Character money: `coins := purse(player);`
- **[paycheck]** - Character money: `coins := purse(player);`
- **[acc_balance]** - Bank accounts: `balance := acc_balance(player);`
- **[acc_total]** - Bank accounts: `balance := acc_balance(player);`
- **[acc_modify]** - Bank accounts: `balance := acc_balance(player);`
- **[transfermoney]** - Money operations: `amount_str := moneystring(1000, 1);`
- **[moneystring]** - Money operations: `amount_str := moneystring(1000, 1);`

## ⏰ **Time & Calendar**

### Time Functions
- **[mudhour]** - Game time: `h := mudhour(); d := mudday(); m := mudmonth(); y := mudyear();`
- **[mudday]** - Game time: `h := mudhour(); d := mudday(); m := mudmonth(); y := mudyear();`
- **[mudmonth]** - Game time: `h := mudhour(); d := mudday(); m := mudmonth(); y := mudyear();`
- **[mudyear]** - Game time: `h := mudhour(); d := mudday(); m := mudmonth(); y := mudyear();`
- **[asctime]** - Format time string: `time_str := asctime(); // "Mon Dec  5 12:34:56 2025"`
- **[realtime]** - Real time: `t := realtime(); // Get system time`
- **[wait]** - Time control: `wait; // Pause execution`
- **[sleep]** - Sleep control: `sleep; // Put unit to sleep`
- **[mudday]** - Game time
- **[mudhour]** - Game time
- **[mudmonth]** - Game time
- **[mudyear]** - Game time
- **[realtime]** - Real time
- **[heartbeat]** - Real time
- **[asctime]** - Real time
- **[wait]** - Time control
- **[sleep]** - Time control

## 🛠️ **Utility Functions**

### Mathematics
- **[rnd]** - Random number: `num := rnd(1, 100); // Random 1-100`
- **[openroll]** - Open roll: `roll := openroll(20, 5, 3); // Roll 3d6, keep best 3`
- **[max]** - Maximum: `result := max(a, b); // Larger of two numbers`
- **[min]** - Minimum: `result := min(a, b); // Smaller of two numbers`
- **[abs]** - Absolute: `result := abs(-5); // Returns 5`

### List Operations
- **[integerlist]**, **[stringlist]** - List types: `nums := {1, 2, 3}; words := {"hello", "world"};`
- **[next]** - Next element: `next(nums); // Returns 2`
- **[previous]** - Previous element: `prev(nums); // Returns 2`
- **[head]** - First element: `first := head(nums); // Returns 1`
- **[addstring]** - Add to list: `addstring(mylist, "new"); // Adds "new" to end`

### System Utilities
- **[log]** - Logging: `log("Player " + self.name + " completed quest at " + asctime());`
- **[flog]** - File logging: `flog("quests.log", self.name + " completed quest: " + quest_name, "a");`
- **[logcrime]** - Crime logging: `logcrime(thief, victim, CRIME_STEALING);`
- **[info]** - Player info: `email_expd := "$email" in pc.info; if (email_expd != null) email := email_expd.descr;`
- **[hasfunc]** - Function check: `if (target.hasfunc == 1) sendtext("Unit has DIL programs attached.", self);`
- **[secure]** - Security control: `secure(target, target_lost); // Jump to target_lost: if unit disappears`
- **[access]** - Access control: `if (access(player, admin_zone) >= 3) sendtext("You have admin access.", self);`
- **[reboot]** - System restart: `act("Rebooting MUD now...", A_ALWAYS, self, null, null, TO_ALL); reboot(0);`

## 🎭 **DIL Program Management**

### Program Structure
- **[dilbegin]** - Program definition: `dilbegin my_function(param : string);`
- **[dilend]** - Program definition: `} dilend`
- **[var]** - Variable declaration: `var i : integer; name : string;`
- **[external]** - External function: `external int my_func(unitptr : unitptr);`
- **[code]** - Code block: `code { /* program logic here */ }`

### Program Control
- **[priority]** - Execution control: `priority; // Block other DIL programs until nopriority called`
- **[nopriority]** - Execution control: `nopriority(); // Allow other DIL programs to execute again`
- **[waitnoop]** - Program flow: `waitnoop; // Temporarily release secure constraints for command execution`

### Event Handling
- **[on_activation]** - Event handlers
- **[clear]** - User interface: `clear(intridx); // Remove interrupt handler by index`
- **[interrupt]** - Program flow

### DIL Operations
- **[dilcall]** - Program management
- **[dilfind]** - Program management
- **[dilcopy]** - Program management
- **[dildestroy]** - Program management
- **[sendtoalldil]** - Inter-program communication
- **[global_head]** - Inter-program communication
- **[store]** - Inter-program communication

### Development Tools
- **[beginedit]** - Editing
- **[killedit]** - Editing
- **[editing]** - Editing
- **[shell]** - Testing
- **[exec]** - Testing

## 🌍 **World Management**

### Zone System
- **[zone_head]** - Zone operations
- **[zhead]** - Zone operations
- **[findzone]** - Zone operations
- **[zoneidx]** - Zone properties
- **[resetmode]** - Zone properties
- **[resettime]** - Zone properties
- **[zonereset]** - Zone resets
- **[help]** - Zone information: `target_zone := findzone(zone_name); help_text := target_zone.help; act(help_text, A_ALWAYS, self, null, null, TO_CHAR);`

### Room Management
- **[room_head]** - Room operations
- **[findroom]** - Room operations
- **[pathto]** - Room operations
- **[rooms]** - Room lists
- **[roomcount]** - Room lists
- **[setroomexit]** - Room connections

### NPC fields
- **[npc_head]** - NPC operations
- **[npcs]** - NPC operations
- **[npccount]** - NPC operations
- **[npcflags]** - NPC properties

### Object Management
- **[obj_head]** - Object operations
- **[objs]** - Field Object operations
- **[objcount]** - Object operations
- **[equip]** - Equipment
- **[unequip]** - Equipment
- **[equipment]** - Equipment
- **[addequip]** - Equipment

## 📋 **Unit Operations**

### Finding & Creating
- **[findunit]** - Unit search
- **[findsymbolic]** - Unit search
- **[findrndunit]** - Unit search
- **[load]** - Unit lifecycle
- **[destroy]** - Unit lifecycle
- **[delunit]** - Unit lifecycle
- **[clone]** - Unit lifecycle

### Movement & Linking
- **[transfer]** - Unit positioning
- **[link]** - Unit positioning
- **[inside]** - Unit positioning
- **[outside]** - Unit positioning
- **[remove]** - Inventory management
- **[insert]** - Inventory management

## 🔐 **Security & Validation**

### Security Functions
- **[check_password]** - Account management
- **[set_password]** - Account management
- **[delete_player]** - Account management
- **[secure]** - Security control
- **[access]** - Security control

### God Functions
- **[switch]** - Character control: `switch(self, target); sendtext("You have switched to " + target.name + ".", self);`
