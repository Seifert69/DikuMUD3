# DikuMUD3 DIL (DikuMUD Interactive Language) Reference

This document provides a comprehensive AGENT.md reference for DIL - the scripting language used in DikuMUD3 for creating interactive game content, NPCs, objects and complex game systems. 

## Compiling a zone
To compile a zone: cd vme/zone/ && vme/bin/vmc -I../include/ zonename.zon

## Zones with good DIL examples
- vme/zone/haon_dor.zon
- vme/zone/udgaard.zon
- vme/zone/midgaard.zon
- vme/zone/cypress.zon

## Quest helper DIL functions
Look in **vme/zone/quests.zon** for the DIL library of quest helper functions broadly exemplified in **vme/zone/haon-dor.zon**.
Look in **vme/zone/randomt.zon** for generating random treasure in DIL.

## 📚 **Core Documentation**
**🔗 Quick Access**: This condensed list covers all 314+ DIL language features. Each entry is reference their respective .wiki files in the DikuMUD3 DIL Manual documentation system. [string] for example is detailed in **vme/src/mcp/string.wiki**

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
- **[medium]** - Operation medium: `container := medium; if (container != null) { act("Using $1n as medium.", A_ALWAYS, self, container, null, TO_CHAR); }`
- **[power]** - Action power: `if (power > 100) { sendtext("This is a very powerful spell!", self); }`

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
- **[block]** - Block commands: `wait(SFB_CMD, command("forbidden")); block; send("Command blocked!", activator);`

**⚠️ IMPORTANT:** DIL does NOT support C-style `for` loops. Use `while` loops instead.

### Common Patterns
Use these templates for typical DIL tasks to ensure structured code.

- **Infinite Event Loop (NPC AI):**
```
  // Skip if NPC is busy
  on_activation((self.position <= POSITION_SLEEPING) or (self.position == POSITION_FIGHTING), skip);
:start:
  wait(SFB_CMD | SFB_MSG, activator.type == UNIT_ST_PC);  // Wait for any command or message from PC
  if (not visible(self, activator)) // We deal with players we can see
    goto start;
  u := activator;
  secure(u, start); // If pc disappears we jump to start
  // Handle event... 
:cleanup:
  unsecure(u);
  goto start;

- **Secure Target Handling (Prevent Null Pointers):**
  target := findunit(self, "playername", FIND_UNIT_SURRO, UNIT_ST_PC);
  if (target != null) {
    secure(target, lost_target);
    // Safe operations on target...
  }
  unsecure(target);
:lost_target:
  log("Target lost");
```

- **Random Decision Tree:**
```
chance := rnd(1, 100);
if (chance <= 30) {
  // Action 1
} else if (chance <= 70) {
  // Action 2
} else {
  // Action 3
}

- **Quest Flag Check/Update (Using Extra):**
if ("rabbit quest ongoing" in activator.quests) {
  sendtext("Bring me that skin.", activator);
} else if ("rabbit quest complete" in activator.quests) {
  sendtext("Quest already complete!", activator);
} else {
   addextra(pc.quests, {"rabbit quest ongoing"}, "kill the killer rabbit");
}
```

- **Coordinated NPC Movement and Event Triggering (Multi-NPC AI):**
```
  external
    integer walk_room@function (s:string,i:integer);  // Import pathfinding function
  var
    tf:integer;  // Temp for walk result
    target_room: string;  // Destination symbolic name
  code
  {
    // Skip if busy or invalid state
    on_activation((self.position <= POSITION_SLEEPING) or (self.position == POSITION_FIGHTING), skip);
:start:
    // Wait for trigger (e.g., message from another DIL)
    wait(SFB_MSG, argument == "raid_start");
    // Move to target with pathfinding
    target_room := "cabin@haon_dor";
    tf := walk_room@function(target_room, 4);  // 4 = speed/mode; adjust as needed
    if (tf == TRUE)
    {
       // Perform action at destination
       exec("emote raids the cabin!", self);
       pause;
       // Signal completion to other NPCs/DILs
       sendto("raid_complete", findsymbolic("orc_chief@haon_dor"));
    }

    // Return or loop
    tf := FALSE;
    while (tf == FALSE)
      tf := walk_room@function("orc_cave@haon_dor", 4);
    goto start;  // Back to waiting loop
  }
  // The orc chief could send a message like this
  //    sendto("raid_start", findsymbolic("orc1@somezone"));
```
- **Advanced pattern for handcuffing player**
```
dilbegin cuff_target(deputy : unitptr, targ : unitptr);
var
  cuffs : unitptr;
code {
  act("You cuff $3n.", A_SOMEONE, deputy, null, targ, TO_CHAR);
  follow(targ, deputy);  // Force target to follow
  unequip(equipment(targ, WEAR_WRIST_R));  // Remove existing wrist item

  cuffs := load("cuffs@midgaard"); // Now inside the deputy
  link(cuffs, targ);     // Move cuffs to target
  addequip(cuffs, WEAR_WRIST_R);  // Equip cuffs on the char it is .inside
  dilcopy("cuffed@midgaard("+self.name+")", targ);  // Apply restraint DIL to cuffs
  return;
}
dilend
```

- ** Guard routine utilizing vme/zone/quest.zon DIL functions ** 
```
dilbegin guardroutine(guardloc: string, dayguard: integer);
var sch: intlist;
code {
  if (dayguard) sch := {5, 18};  // Day schedule
  while (TRUE) {
    pause;
    if (mudhour == sch.[0]) {  // Wake/move at hour
      DailyRoutine@quests({"wake", "walkto::$2t", ...}, "gate@midgaard");
    }
    if (mudhour == sch.[1]) {  // Evening routine
      DailyRoutine@quests({"walkto::grunting_inn@midgaard", "buy beer", "sleep"}, "");
    }
  }
}
dilend
```

### Operators
- **[and]** - Logical operators: `if (self.level >= 10 and self.hp > 50) { /* ready for combat */ }`
- **[or]** - Logical operators: `if (self.level < 10 or self.hp <= 0) { /* not ready */ }`
- **[not]** - Logical operators: `if (not (self.fighting)) { /* not in combat */ }`
- **[set]** - Bit operations: `set(self.pcflags, PC_WIZARD); // Set wizard mode flag`
- **[unset]** - Bit operations: `unset(self.charflags, CHAR_SELF_DEFENCE); // Clear defence flag`
- **[isset]** - Bit operations: `if (isset(self.flags, UNIT_FL_BURIED)) { /* item is buried */ }`

### Unit Fields
- **[type]** - Unit type: `if (target.type == UNIT_ST_PC) { sendtext("This is a player character.", self); }`
- **[inside]** - Unit positioning: `item := chest.inside; if (item != null) { sendtext("Found: " + item.name, self); }`
- **[outside]** - Unit positioning: `container := item.outside; if (container != null) { act("$1n is in $2n", A_ALWAYS, self, item, container, TO_CHAR); }`
- **[bright]** - Lighting: `torch_brightness := bright(self); sendtext("Torch brightness: " + itoa(torch_brightness), self);`
- **[setbright]** - Set brightness: `setbright(torch, 3); // Make torch emit more light`
- **[light]** - Lighting: `light_count := self.light; sendtext("This unit contains " + itoa(light_count) + " light sources.", self); }`
- **[illum]** - Contained light: `if (container.illum > 0) { sendtext("Container glows from within.", self); }`
- **[name]** - Object identification: `item_name := item.name; sendtext("This is called: " + item_name, self); }`
- **[title]** - Object identification: `obj.title := "A shiny sword"; sendtext("Item title updated to: " + obj.title, self); }`
- **[descr]** - Object identification: `extra.descr := "This item glows with magical energy."; // Set extra description`
- **[inside_descr]** - Internal description: `room.inside_descr := "This room has been modified."; // Change room description`
- **[outside_descr]** - External description: `target.outside_descr := "A mysterious figure stands here."; // Set external description`
- **[weight]** - Weight: `total_weight := pc.weight; capacity := pc.capacity; if (total_weight > capacity) { /* overburdened */ }`
- **[baseweight]** - Weight: `empty_weight := baseweight(chest); // Get chest weight without contents`
- **[set_weight]** - Set total weight: `set_weight(item, 50); // Set total weight including contents`
- **[set_weight_base]** - Set base weight: `set_weight_base(item, 20); // Set empty weight of object`
- **[capacity]** - Carrying capacity: `max_cap := capacity(self); remaining := max_cap - weight(self); send("Capacity: " + itoa(remaining) + " remaining", self);`
- **[flags]** - Unit flags: `if (isset(self.flags, UNIT_FL_INVISIBLE)) { unset(self.flags, UNIT_FL_INVISIBLE); act("You become visible!", A_ALWAYS, self, null, null, TO_CHAR); }`
- **[idx]** - Unique ID: `guard_id := guard.idx; // Get unique identifier for tracking`
- **[key]** - Lock identifier: `if (item.key != "") { sendtext("Key required: " + item.key, self); }`
- **[loadcount]** - Instance tracking: `if (item.loadcount > 10) { destroy(item); log("Too many instances loaded"); }`
- **[nameidx]** - Database name: `unit_id := item.nameidx + "@" + item.zoneidx; // Unique reference`
- **[manipulate]** - Object handling: `if (isset(item.manipulate, MANIPULATE_TAKE)) { sendtext("You can take this item.", self); }`
- **[minv]** - Wizard invisibility: `if (item.minv > self.level) { sendtext("You can't see this item.", self); }`
- **[names]** - Name list: `item_names := item.names; if ("sword" in item_names) { sendtext("This is a sword.", self); }`
- **[opendiff]** - Lock difficulty: `if (chest.opendiff > 20) { sendtext("This chest has a very difficult lock.", self); }`
- **[openflags]** - Open/lock state: `if (isset(door.openflags, EX_LOCKED)) { sendtext("The door is locked.", self); }`
- **[zone]** - Zone name: `zone_name := item.zone; sendtext("This item belongs to zone: " + zone_name, self); }`

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
- **[movement]** - Terrain type: `if (room.movement == SECT_WATER_SAIL) { sendtext("This area requires swimming or a boat!", self); }`

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
- **[opponentcount]** - Combat opponents: `if (self.opponentcount > 1) { sendtext("You are fighting multiple opponents!", self); }`
- **[follow]** - Following control: `follow(prisoner, guard); // Make prisoner follow guard`
- **[abilities]** - Character attributes: `str := self.abilities[ABIL_STR]; dex := self.abilities[ABIL_DEX];`
- **[race]** - Character race: `if (self.race == RACE_ELF) { sendtext("You are an elf!", self); }`
- **[sex]** - Character gender: `if (activator.sex == SEX_MALE) { sendtext("Greetings, sir!", activator); }`
- **[speed]** - Combat speed: `if (self.speed > target.speed) { sendtext("You strike first!", self); }`
- **[ability_points]** - Available points: `points := self.ability_points; if (points >= 5) { /* can train */ }`
- **[ability_costs]** - Training costs: `cost := self.ability_costs[ABIL_STR]; if (money >= cost) { /* can afford training */ }`
- **[ability_levels]** - Training requirements: `required := self.ability_levels[ABIL_STR]; if (level >= required) { /* can train */ }`
- **[skill_points]** - Available skill points: `points := self.skill_points; if (points >= 10) { sendtext("You can train skills!", self); }`
- **[skills]** - Skill system: `skill := self.skills.[SKI_BASH];`
- **[skill_costs]** - Skill training costs: `cost := self.skill_costs[SKI_SWORD]; if (self.exp >= cost) { sendtext("Can afford skill!", self); }`
- **[skill_levels]** - Current skill levels: `level := self.skill_levels[SKI_SWORD]; sendtext("Sword skill: " + itoa(level), self);`
- **[spells]** - Magic system: `spell := self.spells.[SPL_FIREBALL];`
- **[spell_costs]** - Spell learning costs: `cost := self.spell_costs[SPL_FIREBALL]; if (self.exp >= cost) { sendtext("Can afford spell!", self); }`
- **[spell_levels]** - Current spell levels: `level := self.spell_levels[SPL_FIREBALL]; sendtext("Fireball level: " + itoa(level), self); }`
- **[alignment]** - Moral standing: `align := alignment(self); if (align < -500) { send("You are evil!", self); }`
- **[birth]** - Character creation: `age_days := (realtime - birth(self)) / 86400; send("You are " + itoa(age_days) + " days old.", self); }`
- **[drunk]** - Character state: `if (self.drunk > 10) { sendtext("You are drunk!", self); }`
- **[full]** - Hunger level: `if (self.full > 20) { sendtext("You are very hungry.", self); } else if (self.full < 5) { sendtext("You feel quite full.", self); }`
- **[thirst]** - Thirst level: `if (self.thirst < 5) { sendtext("You are extremely thirsty!", self); } else { self.thirst := 24; // Fully quench thirst }`
- **[endurance]** - Physical stamina: `if (self.endurance < 2) { sendtext("You are exhausted!", self); } else { self.endurance := self.endurance - 1; }`
- **[max_endurance]** - Maximum stamina: `end_pct := (self.endurance * 100) / self.max_endurance; sendtext("Endurance: " + itoa(end_pct) + "%", self); }`
- **[mana]** - Magic points: `if (self.mana < 50) { sendtext("Not enough mana!", self); } else { self.mana := self.mana - 50; }`
- **[max_mana]** - Maximum magic: `mana_pct := (self.mana * 100) / self.max_mana; sendtext("Mana: " + itoa(mana_pct) + "%", self); }`
- **[charflags]** - Character flags: `if (isset(activator.charflags, CHAR_OUTLAW)) { send("You are an outlaw!", activator); }`
- **[crimes]** - Criminal record: `if (activator.crimes > 0) { send("You have " + itoa(activator.crimes) + " crimes on record.", activator); }`
- **[height]** - Physical size: `height_diff := pc.height - armor.height; // Check size compatibility`
- **[hometown]** - Player origin: `link(player, findroom(player.hometown)); // Send player home`
- **[lifespan]** - Maximum age: `if (age > self.lifespan) { sendtext("You have exceeded your lifespan!", self); }`
- **[natural_armour]** - Base armor: `if (npc.natural_armour > 50) { sendtext("This NPC has heavy natural armor.", self); }`
- **[position_update]** - Update position: `target.hp := target.hp - damage; position_update(target); // Update after HP change`
- **[reset_level]** - Reset to level 1: `reset_level(player); // Reset character to starting level`
- **[reset_race]** - Reset race attributes: `reset_race(player); // Reset weight, height, age, lifespan, training costs`
- **[reset_vlevel]** - Reset virtual level: `reset_vlevel(player); // Reset virtual level to 1`
- **[weapon_costs]** - Weapon training costs: `cost := self.weapon_costs[WPN_SWORD]; if (money >= cost) { sendtext("Can afford sword training!", self); }`
- **[weapon_levels]** - Weapon skill levels: `level := self.weapon_levels[WPN_SWORD]; sendtext("Sword skill level: " + itoa(level), self); }`
- **[weapons]** - Weapon skills: `skill := self.weapons[WPN_SWORD]; if (skill > 0) { sendtext("You know sword fighting!", self); }`

#### PC fields
- **[profession]** - PC properties: `if (activator.profession == PROFESSION_THIEF) { sendtext("Welcome, shadow walker!", activator); }`
- **[guild]** - PC properties: `if (activator.guild == "thief") { sendtext("Welcome, brother of the shadows!", activator); }`
- **[pcflags]** - Player flags: `if (isset(self.pcflags, PC_PK_RELAXED)) { sendtext("You are signed up for player killing.", self); }`
- **[playtime]** - Time played: `hours_played := self.playtime / 3600; sendtext("You have played for " + itoa(hours_played) + " hours.", self); }`
- **[quests]** - Quest list: `found_quest := "dragon_slaying" in self.quests; if (found_quest) { sendtext("You have the dragon quest!", self); }`
- **[switched]** - Character control: `controlled := self.switched; if (controlled) { sendtext("You are controlling: " + controlled.name, self); }`

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
- **[mid]** - Extract substring: `weapon_type := mid("longsword +2 of fire", 1, 9); // == "longsword"`

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
- **[loadstr]** - File operations: `result := loadstr("news.txt", content); if (result > 0) { sendtext(content, self); }`
- **[savestr]** - File operations: `result := savestr("news.txt", message, "w"); if (result > 0) { sendtext("News saved!", self); }`
- **[subextra]** - Remove extra descriptions: `subextra(item.extra, "quest_marker"); // Remove quest marker from item`
- **[vals]** - Extra values: `total := extra.vals.[0] + extra.vals.[1]; // Sum numeric values from extra description`

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
- **[textformat]** - Format text: `formatted := textformat("&nHello &y" + self.name + "&nWelcome!"); // Format with escape codes`

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
- **[subaff]** - Remove affects: `subaff(player, ID_BLIND); // Remove blindness affect from player`

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
- **[weather]** - Weather conditions: `if (weather == SKY_CLOUDLESS) { sendtext("Clear skies today!", self); }`
- **[wait]** - Time control: `wait(SFB_CMD, command("hello")); // Wait for "hello" command`
- **[sleep]** - Sleep control: `sleep; // Put unit to sleep`

## 🛠️ **Utility Functions**

### Mathematics
- **[rnd]** - Random number: `num := rnd(1, 100); // Random 1-100`
- **[openroll]** - Open roll: `roll := openroll(100, 5); // Open-ended 1-100 roll with 5% continuation chance`
- **[max]** - Maximum: `result := max(a, b); // Larger of two numbers`
- **[min]** - Minimum: `result := min(a, b); // Smaller of two numbers`
- **[skill_name]** - Get skill name: `name := skill_name(SKI_SWORD); sendtext("Skill: " + name, self);`
- **[spellindex]** - Find spell index: `idx := spellindex("fireball"); if (idx != -1) { cast_spell(idx, self, self, target, 0, ""); }`
- **[spellinfo]** - Get spell details: `name := spellinfo(idx, realm, sphere, mana, off, resist, med, targ); sendtext("Spell: " + name + " (Mana: " + itoa(mana) + ")", self); }`
- **[strdir]** - List DIL files: `files := strdir("spell_*"); foreach (file in files) { sendtext("Found: " + file + ".dil", self); }`

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
- **[loglevel]** - Command logging: `cmd := getcommand("kick"); if (cmd.loglevel > 0) { sendtext("Command logs at level " + cmd.loglevel, self); }`

## 📋 **Unit Operations**

### Finding & Managing Units
- **[findunit]** - Unit search: `target := findunit(self, "sword", FIND_UNIT_INVEN, null); // Find sword in inventory`
- **[findsymbolic]** - Unit search: `target := findsymbolic("sword@midgaard"); if (target != null) { act("Found: $1n", A_ALWAYS, self, target, null, TO_CHAR); }`
- **[findrndunit]** - Unit search: `random_pc := findrndunit(self, FIND_UNIT_ZONE, UNIT_ST_PC); if (random_pc != null) { act("Random player: $1n", A_ALWAYS, self, random_pc, null, TO_CHAR); }`
- **[target]** - Symbolic search: `guard := target("guard@midgaard"); if (guard) { sendtext("Found guard: " + guard.name, self); }`
- **[symname]** - Get symbolic name: `sym := symname(target); if (sym != "") { sendtext("Symbolic name: " + sym, self); }`
- **[unitdir]** - File search: `files := unitdir("corpse*"); // Find all corpse files in units directory`
- **[visible]** - Visibility check: `if (visible(self, target)) { sendtext("You can see " + target.name, self); }`
- **[load]** - Unit creation: `item := load("sword@midgaard"); if (item != null) { link(item, self); act("Item created!", self, null, null, TO_CHAR); }`
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
- **[restore]** - Unit restoration: `item := restore("saved_item", null); if (item != null) { link(item, self); }`

### DIL Message Functions
- **[send]** - Inter-program messaging: `send("task_complete"); pause; send("cleanup_ready"); // Send to waiting DIL programs with SFB_MSG`
- **[send_done]** - Completion messages: `send_done("cast", self, null, target, 1, "fireball", null, 0); // Notify spell completion`
- **[send_pre]** - Preparation messages: `result := send_pre("cast", self, null, target, 1, "fireball", null, 0); if (result == SFR_BLOCK) { sendtext("Spell blocked!", self); }`
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
- **[loadlevel]** - Zone restrictions: `if (self.level < zone.loadlevel) { sendtext("Zone requires level " + zone.loadlevel, self); }`
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
- **[notes]** - Zone documentation: `zone.notes := "Administrative zone - restricted access"; // Set zone notes`

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
- **[payonly]** - Payment status: `if (zone.payonly) { sendtext("This zone requires payment to access.", self); }`

### Color Functions (not in use)
- **[addcolor]** - Add colors: `addcolor(player, "&red"); // Add red to player's color palette`
- **[delcolor]** - Remove colors: `delcolor(player, "&red"); // Remove red from player's color palette`
- **[changecolor]** - Modify colors: `if (changecolor(player, "clan_color", "&c+w&bn")) { send("Clan color updated!", player); }`
- **[getcolor]** - Color processing: `processed := getcolor("&brBold red text&n"); // Process color codes`

## 🏷️ **Constants & Types** (vme/include/vme.h and vme/include/values.h)

### SFB
## 🏷️ **Constants & Types** (vme/include/vme.h and vme/include/values.h)
These are #define constants from the source headers. Use them in DIL for flags, types, etc. (e.g., `if (self.type == UNIT_ST_PC)`). Values are numeric; descriptions are based on code comments/context.

### State Function Bits (SFB_*)
Used in wait(), interrupt(), etc., for event triggers.

| Constant | Description |
|----------|-------------|
| SFB_CMD  | Normal command trigger. |
| SFB_TICK | Tick-message trigger.   |
| SFB_DEAD | Death-message trigger. |
| SFB_COM  | Combat-event trigger. |
| SFB_MSG  | Message-event trigger. |
| SFB_SAVE | Save-event trigger. |
| SFB_DONE | Indicates something has been done. |
| SFB_ALL  | (SFB_CMD \| SFB_TICK \| SFB_DEAD \| SFB_COM \| SFB_MSG \| SFB_SAVE) | All common triggers. |

### Unit Types (UNIT_ST_*)
For unit.type checks (e.g., in findunit()).

| Constant | Description |
|----------|-------------|
| UNIT_ST_NPC | Non-player character. |
| UNIT_ST_PC | Player character. |
| UNIT_ST_ROOM | Room. |
| UNIT_ST_OBJ | Object. |

### Positions (POSITION_*)
For char.position, defaultpos, etc.

| Constant | Description |
|----------|-------------|
| POSITION_DEAD Dead (use corpse). |
| POSITION_MORTALLYW | Mortally wounded. |
| POSITION_INCAP | Incapacitated. |
| POSITION_STUNNED | Stunned. |
| POSITION_SLEEPING | Sleeping. |
| POSITION_RESTING | Resting. |
| POSITION_SITTING | Sitting. |
| POSITION_FIGHTING | Fighting (do not set manually). |
| POSITION_STANDING | Standing. |

### Character Flags (CHAR_*)
For char.charflags.

| Constant |  Description |
|----------|--------------|
| CHAR_PROTECTED | Protected by law system. |
| CHAR_LEGAL_TARGET | Internal (do not use). |
| CHAR_OUTLAW | Outlaw status (do not use). |
| CHAR_GROUP | In group (do not use). |
| CHAR_BLIND | Blinded. |
| CHAR_HIDE | Hidden. |
| CHAR_MUTE | Mute. |
| CHAR_SNEAK | Sneaking. |
| CHAR_DETECT_ALIGN | Detect alignment (PCs only). |
| CHAR_DETECT_INVISIBLE | See invisible. |
| CHAR_DETECT_MAGIC | Detect magic (PCs only). |
| CHAR_DETECT_POISON | Detect poison (PCs only). |
| CHAR_DETECT_UNDEAD | Detect undead (PCs only). |
| CHAR_DETECT_CURSE | Detect curse (PCs only). |
| CHAR_DETECT_LIFE | Detect life (PCs only). |
| CHAR_WIMPY | Wimpy mode (flee low HP). |
| CHAR_SELF_DEFENCE | Internal use (do not use). |
| CHAR_PEACEFUL | No auto-attack. |
| CHAR_KILL_SELF | Self-kill detection. |

### Object Flags (OBJ_*)
For obj.objectflags.

| Constant | Description |
|----------|-------------|
| OBJ_NO_UNEQUIP |Cannot unequip. |
| OBJ_TWO_HANDS | Requires two hands. |
| OBJ_NOCOVER | Doesn't cover body part. |
| OBJ_NO_DUAL | Cannot dual-wield. |
| OBJ_VEHICLE | Vehicle type (extended). |

(Truncate for brevity; full list includes OBJ_MAGIC from snippets.)

### Room Flags (ROOM_*)
For room.roomflags (e.g., UNIT_FL_INDOORS in zone).

| Constant | Description |
|----------|-------------|
| UNIT_FL_INDOORS Indoors (no weather). |
| UNIT_FL_NO_WEATHER | No weather effects. |
| UNIT_FL_NO_MOB | No mobiles allowed. |
| ROOM_FL_DARK | Dark

### Flags
- **[ROOM_*]** - Room flags
- **[CMD_*]** - Command flags
- **[SPL_*]** - Spell flags
- **[SKI_*]** - Skill flags
- **[ABIL_*]** - Ability flags

### Zone Macros (vme/include/macros.h)
Used in .zon files (e.g., for NPC/object defs). Not directly in DIL, but referenced.
Easy way to create all kind of standard objects or characters. 

| Macro | Expansion/Usage | Description |
|-------|-----------------|-------------|
| M_AVG_HUMAN(level, sex) | (Sets height, weight, etc. based on level/sex) | Average human NPC stats. |
| WEAPON_DEF(wpn_type, def_bonus, att_bonus) | (Sets value[0-4] for weapons) | Defines weapon type/damage. |
| BONUS_AVERAGE | 0 | Average bonus (use -5 to 5 for poor/excellent). |
| OUT_DARK_NON_NOON | (Sets outside_descr based on time) | Dark description except noon. |


## 🚨 **Critical Compilation Guidelines**

### Reserved Keywords (⚠️ IMPORTANT)
The following words are **reserved** in DIL and **cannot be used as variable names**:
- `opponent` - **Function name, not variable** - use `opp`, `target_unit`, or `target` instead
- `for` - **Not supported** - use `while` loops instead
- Other reserved words may exist during compilation

### Variable Naming Conventions
- **Maximum name length:** 16 characters for unit names
- **Use descriptive names:** `target_unit` instead of `target`
- **Use underscores:** `current_room` for readability
- **Avoid all reserved keywords** listed above

# ZONE example with DIL examples
```
/*
filename    minimal-zone
password    testpass
changedby   AI Assistant
EmailAdd
request     compile
version     1
END HEADER*/

#include <macros.h>

%zone
title "Minimal Test Zone@Mainland"
lifespan 10
reset RESET_ANYHOW
creators {"ai_assistant"}

notes
"A minimal zone demonstrating all major Haon-Dor zone fields and expressions.
Contains two comprehensive DIL examples:
1. comprehensive_demo() - Advanced DIL patterns for LLM learning
2. simple_demo() - Basic DIL patterns for beginners
Both examples demonstrate key concepts: variables, control flow, string/integer
operations, unit manipulation, combat, time functions, and more."

help
"This is a minimal test zone created to demonstrate all of the key zone fields
and expressions used in Haon-Dor. Contains comprehensive DIL examples perfect
for learning DIL programming patterns. Zone by AI Assistant."

%dil
/* ========================================================================
   COMPREHENSIVE DIL EXAMPLE - Perfect for LLM Learning
   This example demonstrates the most important DIL concepts and patterns
   ======================================================================== */

dilbegin demo();
var
   target_player : unitptr;     // Unit pointer for finding players
   random_num : integer;        // For random number generation
   message_text : string;       // For string manipulation
   item_found : unitptr;        // For object manipulation
   oppo : unitptr;              // For combat operations
   plyr : unitptr;
   health_pct : integer;        // For calculations
   current_hour : integer;      // For time functions
   intr_hdl : integer;
   sl : stringlist;
   i : integer;

code
{
   // Set heartbeat to check every 5 seconds
   heartbeat := PULSE_SEC * 5;
   
   // Skip execution if NPC is asleep or fighting
   on_activation((self.position <= POSITION_SLEEPING), skip);
   interrupt(SFB_COM, activator == self, imfighting);

 :main_loop:
   // ====================================================================
   // 1. COMMAND WAITING - Basic interaction pattern
   // ====================================================================
   wait(SFB_CMD, command("say"));

   if (not ("hello" in argument) and not ("greet" in argument))
     goto main_loop;
   
   if (activator.type == UNIT_ST_PC)
   {
      plyr := activator;
      secure(plyr, main_loop);

      // String concatenation with itoa() for numbers
      message_text := "Greetings, " + plyr.name + "! Your level is " + itoa(plyr.level) + ".";
      exec("say " + message_text, self);
      pause; // After this pause plyr is null
      
      // Random number generation and conditional logic
      random_num := rnd(1, 100);
      if (random_num > 70)
      {
         exec("emote smiles warmly.", self);
         pause;
      }
      else if (random_num > 30)
      {
         exec("emote nods politely.", self);
         pause;
      }
      else
      {
         exec("emote watches you with interest.", self);
         pause;
      }
   }

   // ====================================================================
   // 2. TIME FUNCTIONS - Demonstrate game time awareness
   // ====================================================================
   current_hour := mudhour;
   sl := {"say Good morning to you!", "say Good afternoon!", "say What a lovely time to be alive!"};
   if (current_hour >= 6 and current_hour < 12) 
      i := 0;
   else if (current_hour >= 12 and current_hour < 18)
      i := 1;
   else
      i := 2;

   exec(sl.[i], self);
   pause;

   // ====================================================================
   // 3. UNIT SEARCHING - Find and interact with nearby units
   // ====================================================================
   target_player := findrndunit(self, FIND_UNIT_SURRO, UNIT_ST_PC);
   if (target_player != null)
   {
      // Health percentage calculation
      health_pct := (target_player.hp * 100) / target_player.max_hp;
      
      if (health_pct < 50)
      {
         act("$1n says 'You look wounded! Let me help you.'",
             A_SOMEONE, self, null, null, TO_ROOM);
         pause;
         
         // Healing demonstration
         target_player.hp := target_player.hp + 10;
         act("$1n touches $3n, who feels slightly better.",
             A_SOMEONE, self, null, target_player, TO_NOTVICT);
         act("$1n touches you, and you feel slightly better.",
             A_SOMEONE, self, null, target_player, TO_VICT);
      }
   }

   // ====================================================================
   // 4. OBJECT MANIPULATION - Search for and handle items
   // ====================================================================
   foreach (UNIT_ST_OBJ, item_found)
   {
      if (item_found.outside == self)  // Item is on the NPC
      {
         if (item_found.objecttype == ITEM_WEAPON)
         {
            exec("say I am armed with " + item_found.title, self);
            break;  // Exit foreach loop after finding first weapon
         }
      }
      else if (item_found.outside == self.outside)  // Item is in the same room
      {
         if (item_found.objecttype == ITEM_CONTAINER)
         {
            exec("say I see a container here: " + item_found.title, self);
            break;
         }
      }
   }
   pause;

   // ====================================================================
   // 5. COMBAT OPERATIONS - Basic combat AI
   // ====================================================================
   if (self.position == POSITION_FIGHTING)
   {
      oppo := self.fighting;
      if (oppo != null)
      {
         // Combat taunts based on oppo type
         if (oppo.type == UNIT_ST_PC)
         {
            exec("say You should not have challenged me!", self);
         }
         else
         {
            exec("say Die, foul creature!", self);
         }
         pause;
         
         // Wait for combat to end
         while (self.position == POSITION_FIGHTING)
            pause;
      }
   }

   // ====================================================================
   // 6. MOVEMENT AND EXPLORATION - Room navigation
   // ====================================================================
   random_num := rnd(1, 10);
   if (random_num <= 3)  // 30% chance to move
   {
      exec("say I think I'll take a little walk.", self);
      pause;
      
      // Try random directions (simplified example)
      random_num := rnd(1, 4);
      if (random_num == 1)
         exec("north", self);
      else if (random_num == 2)
         exec("south", self);
      else if (random_num == 3)
         exec("east", self);
      else
         exec("west", self);
      
      pause;
   }

   // ====================================================================
   // 7. ALIGNMENT AND ROLEPLAYING - React to player alignment
   // ====================================================================
   if (plyr.type == UNIT_ST_PC)
   {
      if (plyr.alignment < -300)
      {
         exec("say Your evil nature disturbs me.", self);
         pause;
         exec("emote watches you warily.", self);
      }
      else if (plyr.alignment > 300)
      {
         exec("say It is good to see a noble soul like yourself!", self);
         pause;
         exec("emote smiles warmly.", self);
      }
      else
      {
         exec("say You seem to walk the middle path.", self);
         pause;
      }
   }

   // ====================================================================
   // 8. MEMORY AND STATE - Simple quest/interaction tracking
   // ====================================================================
   // Check if player has a specific quest flag (simplified example)
   if (plyr.type == UNIT_ST_PC)
   {
      // This would normally check quest flags, simplified for demo
      exec("say I remember meeting you before.", self);
      pause;
   }

   unsecure(plyr);
   // Loop back to main loop
   goto main_loop;

:imfighting:
   exec("say Darn it! I hate being in combat all the time", self);
   // We could clear the interrupt here with
   // clear(intr_hdl);
   // But let's keep it coming.
   goto main_loop;
}
dilend

/* ========================================================================
   SIMPLE DIL EXAMPLE - Basic patterns for beginners
   This demonstrates fundamental DIL concepts in a simple, clear way
   ======================================================================== */

dilbegin basic();
var
   player : unitptr;          // Unit pointer for players
   roll : integer;            // Random number storage
   greeting : string;         // String manipulation

code
{
   // Check every 10 seconds
   heartbeat := PULSE_SEC * 10;
   
   // Don't run if asleep or fighting
   on_activation((self.position <= POSITION_SLEEPING) or
                 (self.position == POSITION_FIGHTING), skip);

   :start:
   
   // Wait for any command from players or npcs
   wait(SFB_CMD, TRUE);
   
   // Only respond to players, not other NPCs
   if (activator.type != UNIT_ST_PC)
      goto start;
   
   player := activator;

   // If the char leaves, reset and go back to start
   // right after the exec, player can be reset
   // unless we add this secure
   secure(player, start);

   // Basic greeting with string concatenation
   greeting := "Hello, " + player.name + "!";
   exec("say " + greeting, self);
   pause;

   // We cannot use activator down here because it'll be null
   
   // Random response pattern
   roll := rnd(1, 3);
   if (roll == 1)
   {
      exec("say It's a fine day for adventure!", self);
   }
   else if (roll == 2)
   {
      exec("hug " + player.name, self);
   }
   else
   {
      exec("say May fortune favor you!", self);
   }
   pause;
   
   // Simple health check
   if (player.hp < player.max_hp / 2)
   {
      exec("say You look wounded - be careful!", self);
      pause;
   }
   
   unsecure(player);

   // Loop back
   goto start;
}
dilend

%rooms

/* Minimal room demonstrating all key room fields */
minimal_room

names {"minimal room", "room"}
title "A Minimal Room"
descr "This is a minimal room demonstrating all the key room fields and
expressions used in Haon-Dor. The room is simple but contains examples of
all major room properties."

extra {"wall", "walls"}
"The walls are made of simple stone blocks."

extra {"floor", "ground"}
"The floor is made of rough wooden planks."

extra {"ceiling"}
"The ceiling is low and made of dark wooden beams."

movement SECT_INSIDE
gmap(100,100)

flags {UNIT_FL_INDOORS, UNIT_FL_NO_WEATHER, UNIT_FL_NO_MOB}
OUT_DARK_NON_NOON

end

/* Second room for exit demonstrations */
minimal_room2

title "Another Minimal Room"
descr "This is the second minimal room, used to demonstrate various exit
types and room connections."

extra {"ceiling"}
"The ceiling is low and made of dark wooden beams."

movement SECT_INSIDE
gmap(100,101)

flags {UNIT_FL_INDOORS, UNIT_FL_NO_WEATHER}

end

/* Third room for special terrain */
minimal_room3

title "Room with Special Terrain"
descr "This room demonstrates special terrain types and movement
sectors. The ground here is different from other rooms."

extra {"terrain", "ground"}
"The terrain here is rough and uneven, making movement difficult."

movement SECT_FOREST
gmap(100,99)

OUT_DARK_NON_NOON

end

%objects

/* Minimal object demonstrating all key object fields */
minimal_key

names {"key", "minimal key"}
title "a small minimal key"
descr "This is a small, simple key that seems to fit minimal locks."

extra {"lock", "mechanism"}
"The key has a simple mechanism with basic teeth."

extra {"$identify"}
"This key can be identified to reveal its purpose."

cost 10 IRON_PIECE
rent 1 IRON_PIECE

manipulate {MANIPULATE_TAKE, MANIPULATE_HOLD}
type ITEM_KEY


weight 1
bright 0

key minimal_key

end

/* Weapon example */
minimal_sword

names {"sword", "minimal sword"}
title "a minimal sword"
descr "This is a simple, unadorned sword with minimal decoration."

extra {"blade", "hilt"}
"The blade is plain but sharp. The hilt is wrapped in basic leather."

extra {"$identify"}
"This sword can be identified to reveal its craftsmanship."

extra {"$improved identify"}
"This sword can be identified to reveal its craftsmanship."

WEAPON_DEF(WPN_SCIMITAR, BONUS_AVERAGE, BONUS_AVERAGE)

cost 50 IRON_PIECE
rent 5 IRON_PIECE
flags {UNIT_FL_MAGIC}

weight 10
bright 0

end

/* Container example */
minimal_chest

names {"chest", "minimal chest"}
title "a minimal chest"
descr "This is a simple wooden chest with minimal decoration."

extra {"lock", "hinges"}
"The chest has a simple lock and basic metal hinges."

extra {"$identify"}
"This chest can be identified to reveal its contents capacity."

cost 25 IRON_PIECE
rent 2 IRON_PIECE

manipulate {MANIPULATE_TAKE, MANIPULATE_HOLD}
type ITEM_CONTAINER


weight 15
bright 0

capacity 50
key minimal_key

end

%mobiles

/* Minimal NPC demonstrating all key NPC fields */
minimal_guard

names {"guard", "minimal guard"}
title "a minimal guard"
descr "This is a simple guard with minimal equipment and basic appearance."

extra {"armor", "weapon"}
"The guard wears basic leather armor and carries a simple sword."

extra {"$identify"}
"This guard can be identified to reveal his combat capabilities."

dilcopy demo@minimal_zone();

M_AVG_HUMAN(5, SEX_MALE)

alignment 100
exp 100

position POSITION_STANDING
end

/* Second NPC for variety */
minimal_merch

names {"merchant", "minimal merchant"}
title "a minimal merchant"
descr "This is a simple merchant with basic goods and minimal haggling skills."

extra {"goods", "wares"}
"The merchant has a small selection of basic goods for sale."

extra {"$identify"}
"This merchant can be identified to reveal her shop inventory."

dilcopy basic@minimal_zone();

M_AVG_HUMAN(3, SEX_FEMALE)

alignment 0
exp 50

position POSITION_STANDING
end

%reset

/* Reset actions demonstrating all key reset expressions */

/* Load NPCs */
load minimal_guard into minimal_room max 1
{
    equip minimal_sword position WEAR_WIELD max 1
}

load minimal_merch into minimal_room2 max 1

/* Load objects */
load minimal_key into minimal_guard max 1
load minimal_chest into minimal_room max 1
load minimal_key into minimal_chest max 1
load minimal_sword into minimal_room max 1



/* Door states */
door minimal_room NORTH {EX_OPEN_CLOSE}

/* Local objects (zone-wide) */
load minimal_key local 5

/* Zone max objects */
load minimal_sword zonemax 10

/* Zone reset time */

%end
```