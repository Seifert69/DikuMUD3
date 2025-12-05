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
- **[integer]** - Numeric data type
- **[string]** - Text data type  
- **[unitptr]** - Unit pointer (characters, objects, rooms)
- **[integerlist]** - Integer collections
- **[stringlist]** - String collections
- **[null]** - Null value

### Type Conversion
- **[atoi]** - String to integer: `num := atoi("42"); // num == 42`
- **[itoa]** - Integer to string: `str := itoa(100); // str == "100"`

### Built-in Variables
- **[self]** - Current executing unit
- **[activator]** - Unit that triggered program
- **[argument]** - Command arguments
- **[command]** - Command name (use with `command("text")` syntax)
- **[cmdstr]** - Full command string

### Control Structures
- **[if]** - Conditional statement: `if (condition) { /* code */ }`
- **[switch]** - Multi-way conditional: `switch (value) { case 1: /* code */ }`
- **[while]** - Loop construct: `while (condition) { /* code */ }`
- **[for_each]** - Loop construct: `for each // TODO`
- **[goto]** - Unconditional jump: `goto labelname; // Jump to label`
- **[on_goto]** - Goto construct: `on i goto label0, label1;` (0 is label0)
- **[break]** - Exit loop: `break; // Exit current loop`
- **[continue]** - Continue iteration: `continue; // Skip to next iteration`
- **[return]** - Return from function: `return value;`
- **[quit]** - Terminate program: `quit;`
**⚠️ IMPORTANT:** DIL does NOT support C-style `for` loops. Use `while` loops instead.

### Operators
- **[and]** - Logical operators
- **[or]** - Logical operators
- **[not]** - Logical operators
- **[set]** - Bit operations
- **[unset]** - Bit operations
- **[isset]** - Bit operations

## 🎮 **Game Systems**

### Operators
- **[and]** - Logical operators: `if (a and b) { /* code */ }`
- **[or]** - Logical operators: `if (a and b) { /* code */ }`
- **[not]** - Logical operators: `if (a and b) { /* code */ }`
- **[set]** - Bit operations: `flags := flags | UNIT_FL_NO_MOB;`
- **[unset]** - Bit operations: `flags := flags | UNIT_FL_NO_MOB;`
- **[isset]** - Bit operations: `flags := flags | UNIT_FL_NO_MOB;`
- **[in]** - String/list membership: `if ("key" in mylist) { /* found */ }`

### Character System
- **[abilities]** - Character attributes: `str := self.abilities.[ABIL_STR];`
- **[ability_points]** - Available points: `points := self.ability_points;`
- **[skills]** - Skill system: `skill := self.skills.[SKI_BASH];`
- **[spells]** - Magic system: `spell := self.spells.[SPL_FIREBALL];`

### Character Fields
- **[level]** - Character levels: `lvl := self.level; exp := self.exp;`
- **[vlevel]** - Character levels: `lvl := self.level; exp := self.exp;`
- **[hp]** - Health: `hp_pct := (self.hp * 100) / self.max_hp;`
- **[max_hp]** - Health: `hp_pct := (self.hp * 100) / self.max_hp;`
- **[position]** - States: `if (self.position == POSITION_FIGHTING) { /* in combat */ }`
- **[defaultpos]** - States: `if (self.position == POSITION_FIGHTING) { /* in combat */ }`
- **[fighting]** - Combat: `opp := self.fighting;`
- **[opponent]** - Combat: `opp := self.fighting;`

### Object System
- **[name]** - Object identification: `obj.title := "A shiny sword";`
- **[title]** - Object identification: `obj.title := "A shiny sword";`
- **[descr]** - Object identification: `obj.title := "A shiny sword";`
- **[weight]** - Weight: `total := obj.weight * obj.count;`
- **[baseweight]** - Weight: `total := obj.weight * obj.count;`
- **[value]** - Economics: `obj.cost := 1000; obj.rent := 10;`
- **[cost]** - Economics: `obj.cost := 1000; obj.rent := 10;`
- **[rent]** - Economics: `obj.cost := 1000; obj.rent := 10;`
- **[objectflags]** - Classification: `if (obj.flags & OBJ_FL_MAGIC) { /* magic item */ }`
- **[objecttype]** - Classification: `if (obj.flags & OBJ_FL_MAGIC) { /* magic item */ }`

### Room System
- **[roomflags]** - Room properties: `if (room.flags & UNIT_FL_INDOORS) { /* inside */ }`
- **[bright]** - Lighting: `if (islight(room)) { /* lit room */ }`
- **[light]** - Lighting: `if (islight(room)) { /* lit room */ }`
- **[islight]** - Lighting: `if (islight(room)) { /* lit room */ }`
- **[exit_to]** - Room exits: `if (room.exit_to[DIR_NORTH]) { /* north exit */ }`
- **[exit_names]** - Room exits: `if (room.exit_to[DIR_NORTH]) { /* north exit */ }`
- **[mapx]** - Coordinates: `room.mapx := 100; room.mapy := 200;`
- **[mapy]** - Coordinates: `room.mapx := 100; room.mapy := 200;`

## 💬 **Communication System**

### Message Functions
- **[send]** - Basic messaging
- **[sendtext]** - Basic messaging
- **[sendto]** - Basic messaging
- **[act]** - Formatted social messages
- **[sact]** - Formatted social messages
- **[sendtoall]** - Broadcast functions
- **[sendtoalldil]** - Broadcast functions
- **[pagestring]** - Paginated output
- **[prompt]** - User interface
- **[clear]** - User interface
- **[help]** - User interface

### Text Processing
- **[length]** - Get string length: `l := length("hello"); // == 5`
- **[substring]** - Remove from stringlist: `substring(mylist, "text")` (NOT extract substring)
- **[left]** - Extract left characters: `l := left("hello", 2); // == "he"`
- **[right]** - Extract right characters: `r := right("world.txt", 3); // == "txt"`
- **[strcmp]** - String comparison: `if (strcmp("a", "b") == 0)` (use `==` for simple equality)
- **[split]** - Split into stringlist: `words := split("hello world", " ");`
- **[replace]** - Replace text: `new := replace("old", "new", text);`
- **[getword]** - Extract first word: `s:="hello sam"; w := getword(s);` (w == "hello", s =="sam")
- **[getwords]** - Get word list: `sl := getwords("hello world");` (sl is {"hello", "world"}, original preserved)
- **[toupper]** - Convert to uppercase: `u := toupper("hello"); // == "HELLO"`
- **[tolower]** - Convert to lowercase `l := tolower("HellO"); // == "hello"`
- **[strncmp]** - Compare with length limit: `if (strncmp("hello", "help", 3) == 0)`

**⚠️ Critical Notes:**
- Stringlists use dot notation: `words.[0]` NOT `words[0]`
- Convert integers for concatenation: `"Value: " + itoa(num)`
- substring() removes from stringlists, doesn't extract substrings

## ⚔️ **Combat System**

### Combat Functions
- **[set_fighting]** - Set combat: `set_fighting(attacker, target);`
- **[stop_fighting]** - Stop combat: `stop_fighting(unit);`
- **[attack_type]** - Combat data
- **[weapon_info]** - Combat data
- **[weapon_name]** - Combat data
- **[meleeattack]** - Attack actions
- **[attack_spell]** - Attack actions
- **[cast_spell]** - Attack actions
- **[meleedamage]** - Combat calculations: `dmg := meleedamage(attacker, 50);`
- **[defensive]** - Combat calculations: `dmg := meleedamage(attacker, 50);`
- **[offensive]** - Combat calculations: `dmg := meleedamage(attacker, 50);`

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
- **[log]** - Logging
- **[flog]** - Logging
- **[logcrime]** - Logging
- **[debug]** - Debugging
- **[info]** - Debugging
- **[hasfunc]** - Debugging
- **[secure]** - Administration
- **[access]** - Administration
- **[reboot]** - Administration

## 🎭 **DIL Program Management**

### Program Structure
- **[dilbegin]** - Program definition
- **[dilend]** - Program definition
- **[var]** - Program structure
- **[code]** - Program structure

### Program Control
- **[priority]** - Execution control
- **[nopriority]** - Execution control
- **[interrupt]** - Program flow
- **[waitnoop]** - Program flow

### Event Handling
- **[on_activation]** - Event handlers
- **[on_goto]** - Event handlers

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

### Room Management
- **[room_head]** - Room operations
- **[findroom]** - Room operations
- **[pathto]** - Room operations
- **[rooms]** - Room lists
- **[roomcount]** - Room lists
- **[setroomexit]** - Room connections

### NPC Management
- **[npc_head]** - NPC operations
- **[npcs]** - NPC operations
- **[npccount]** - NPC operations
- **[npcflags]** - NPC properties
- **[profession]** - NPC properties
- **[guild]** - NPC properties
- **[master]** - Social relationships
- **[follower]** - Social relationships
- **[followercount]** - Social relationships

### Object Management
- **[obj_head]** - Object operations
- **[objs]** - Object operations
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

## 🏷️ **Constants & Types** (vme.h and values.h)

### ...
- **[SFB_*]** - State function bits (SFB_CMD, SFB_COM, SFB_DEAD, etc.)

### Unit Types
- **[UNIT_ST_PC]** - Player characters
- **[UNIT_ST_NPC]** - Non-player characters  
- **[UNIT_ST_OBJ]** - Objects
- **[UNIT_ST_ROOM]** - Rooms

### Positions
- **[POSITION_STANDING]**, **[POSITION_SITTING]**, **[POSITION_RESTING]**, **[POSITION_SLEEPING]**, **[POSITION_FIGHTING]** - Character positions

### Flags
- **[CHAR_*]** - Character flags
- **[OBJ_*]** - Object flags
- **[ROOM_*]** - Room flags

## 🔐 **Security & Validation**

### Security Functions
- **[check_password]**, **[set_password]**, **[delete_player]** - Account management
- **[secure]**, **[access]** - Security control

---

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

### Loop Structures (⚠️ CRITICAL)
**✅ SUPPORTED - While Loops:**
```dil
i := 0;
while (i < 10) {
    exec("say Count: " + itoa(i), self);
    i := i + 1;
}
```

**❌ NOT SUPPORTED - For Loops:**
```dil
// This does NOT work in DIL:
for (i := 0; i < 10; i := i + 1) {
    // code
}

// Use while loop instead:
i := 0;
while (i < 10) {
    // code
    i := i + 1;
}
```

## 🔧 **Common Compilation Errors & Solutions**

### "Arg 2 of '+' not string"
**Cause:** Trying to concatenate non-string values
**Solution:** Convert integers to strings using `itoa()`
```dil
// WRONG
exec("say Value: " + number, self);

// RIGHT  
exec("say Value: " + itoa(number), self);
```

### "No such variable: X"
**Cause:** Using undeclared variables or reserved keywords
**Solution:** Check variable declarations and avoid reserved words
```dil
// WRONG
var
   opponent : unitptr;  // 'opponent' is reserved

// RIGHT
var
   opp : unitptr;
   target_unit : unitptr;
```

### "Syntax error: Token 'X'"
**Cause:** Incorrect function syntax or missing external declarations
**Solution:** Verify function signatures and add external declarations
```dil
// WRONG - missing external declaration
result := max(num1, num2);

// RIGHT
external
   integer max(a:integer, b:integer);

// Later in code:
result := max(num1, num2);
```

### "Unit name too long. truncated"
**Cause:** Unit names exceed 16 character limit
**Solution:** Use shorter unit names
```dil
// WRONG
names {"very_long_unit_name_that_exceeds_limit"}

// RIGHT
names {"short_name"}
```

### "Illegal lvalue"
**Cause:** Incorrect assignment or comparison syntax
**Solution:** Use proper assignment operators
```dil
// WRONG
if (found = 1)  // Assignment instead of comparison

// RIGHT
if (found == 1)  // Proper comparison
```

---

**💡 These critical guidelines help developers avoid the most common compilation pitfalls encountered in practical DIL development.**

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

### Loop Structures (⚠️ CRITICAL)
**✅ SUPPORTED - While Loops:**
```dil
i := 0;
while (i < 10) {
    exec("say Count: " + itoa(i), self);
    i := i + 1;
}
```

**❌ NOT SUPPORTED - For Loops:**
```dil
// This does NOT work in DIL:
for (i := 0; i < 10; i := i + 1) {
    // code
}

// Use while loop instead:
i := 0;
while (i < 10) {
    // code
    i := i + 1;
}
```

## 🔧 **Common Compilation Errors & Solutions**

### "Arg 2 of '+' not string"
**Cause:** Trying to concatenate non-string values
**Solution:** Convert integers to strings using `itoa()`
```dil
// WRONG
exec("say Value: " + number, self);

// RIGHT  
exec("say Value: " + itoa(number), self);
```

### "No such variable: X"
**Cause:** Using undeclared variables or reserved keywords
**Solution:** Check variable declarations and avoid reserved words
```dil
// WRONG
var
   opponent : unitptr;  // 'opponent' is reserved

// RIGHT
var
   opp : unitptr;
   target_unit : unitptr;
```

### "Syntax error: Token 'X'"
**Cause:** Incorrect function syntax or missing external declarations
**Solution:** Verify function signatures and add external declarations
```dil
// WRONG - missing external declaration
result := max(num1, num2);

// RIGHT
external
   integer max(a:integer, b:integer);

// Later in code:
result := max(num1, num2);
```

### "Unit name too long. truncated"
**Cause:** Unit names exceed 16 character limit
**Solution:** Use shorter unit names
```dil
// WRONG
names {"very_long_unit_name_that_exceeds_limit"}

// RIGHT
names {"short_name"}
```

### "Illegal lvalue"
**Cause:** Incorrect assignment or comparison syntax
**Solution:** Use proper assignment operators
```dil
// WRONG
if (found = 1)  // Assignment instead of comparison

// RIGHT
if (found == 1)  // Proper comparison
```

---

**💡 These critical guidelines help developers avoid the most common compilation pitfalls encountered in practical DIL development.**


## 🚨 **Critical Compilation Guidelines**

### Reserved Keywords
All the 315+ language keywords are reserved and cannot be used for variables names.
Examples of illegal variable names: `opponent`, `activator`, `for`, etc.
Other reserved words may exist during compilation

### Variable Naming Conventions
- Maximum name length: 16 characters for unit names
- Use underscores for readability: `current_room`
- Avoid all reserved keywords listed above

### Built-in vs External Functions

#### External Functions (Require Declaration)
In this example you locally declare and use DIL functions that exist in zone/function.zon
```dil external
     integer max@function(a:integer, b:integer);
     integer min@function(a:integer, b:integer);
```

### String Operations
**Correct Syntax:**
```dil
// String comparison
if (string1 == string2)

// Command checking (CRITICAL - use function syntax)
if (command("say"))

// String concatenation
message := "Hello " + name;

// String list access (CRITICAL - use dot notation)
mylist := {"item1", "item2"};
item := mylist.[0];  // Note: dot notation REQUIRED
```

**Incorrect Syntax (Causes Compilation Errors):**
```dil
// Wrong string comparison
if (string1 $= string2)  // ERROR - use ==

// Wrong command checking (CRITICAL ERROR)
if (command == "say")  // ERROR - use command("say")

// Wrong string list access (CRITICAL ERROR)
item := mylist[0];  // ERROR - use mylist.[0] with dot
```

**Incorrect Syntax (Causes Compilation Errors):**
```dil
// Wrong string comparison
if (string1 $= string2)  // ERROR

// Wrong command checking  
if (command == "say")  // ERROR

// Wrong string list access
item := mylist[0];  // ERROR - need dot notation mylist.[0];
```

### Unit Field Access
**Correct:**
```dil
unit.title        // Unit's title
unit.type         // Unit type (UNIT_ST_PC, UNIT_ST_NPC, etc.)
unit.position     // Position state
unit.outside      // Room containing unit
unit.fighting     // Current opponent (CRITICAL: NOT unit.opponent - reserved)
```

**Incorrect (Causes Compilation Errors):**
```dil
unit.name         // May cause errors - use unit.title instead
unit.opponent     // RESERVED KEYWORD - use unit.fighting instead
```

**Incorrect (Causes Compilation Errors):**
```dil
unit.name         // May cause errors
unit.opponent     // Reserved keyword - use unit.fighting instead
```

### Loop Structures
**Supported - While Loops:**
```dil
i := 0;
while (i < 10) {
    exec("say Count: " + itoa(i), self);
    i := i + 1;
}
```

**Not Supported - For Loops:**
```dil
// This does NOT work in DIL:
for (i := 0; i < 10; i := i + 1) {
    // code
}

// Use while loop instead:
i := 0;
while (i < 10) {
    // code
    i := i + 1;
}
```

## 🔧 **Common Compilation Errors & Solutions**

### "Arg 2 of '+' not string"
**Cause:** Trying to concatenate non-string values
**Solution:** Convert integers to strings using `itoa()`
```dil
// WRONG
exec("say Value: " + number, self);

// RIGHT  
exec("say Value: " + itoa(number), self);
```

### "No such variable: X"
**Cause:** Using undeclared variables or reserved keywords
**Solution:** Check variable declarations and avoid reserved words
```dil
// WRONG
var
   opponent : unitptr;  // 'opponent' is reserved

// RIGHT
var
   opp : unitptr;
   target_unit : unitptr;
```

### "Syntax error: Token 'X'"
**Cause:** Incorrect function syntax or missing external declarations
**Solution:** Verify function signatures and add external declarations
```dil
// WRONG - missing external declaration
result := max(num1, num2);

// RIGHT
external
   integer max(a:integer, b:integer);

// Later in code:
result := max(num1, num2);
```

### "Unit name too long. truncated"
**Cause:** Unit names exceed 16 character limit
**Solution:** Use shorter unit names
```dil
// WRONG
names {"very_long_unit_name_that_exceeds_limit"}

// RIGHT
names {"short_name"}
```

### "Illegal lvalue"
**Cause:** Incorrect assignment or comparison syntax
**Solution:** Use proper assignment operators
```dil
// WRONG
if (found = 1)  // Assignment instead of comparison

// RIGHT
if (found == 1)  // Proper comparison
```
