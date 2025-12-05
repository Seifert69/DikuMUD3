# DikuMUD3 DIL (DikuMUD Interactive Language) Reference

This document provides a comprehensive AGENTA reference for DIL - the scripting language used in DikuMUD3 for creating interactive game content, NPCs, objects and complex game systems. 

## Compiling a zone
To compile a zone: cd vme/zone/ && vme/bin/vmc -I../include/ zonename.zon

## Zones with good DIL examples
vme/zone/haon-dor.zon
vme/zone/udgaard.zon
vme/zone/midgaard.zon
vme/zone/cypress.zon

## Quest helper DIL functions
Look in vme/zone/quests.zon for the DIL library of quest helper functions broadly exemplified in vme/zone/haon-dor.zon
Look in vme/zone/randomt.zon for generating random treasure in DIL

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
- **[atoi]** - String to integer
- **[itoa]** - Integer to string

### Built-in Variables
- **[self]** - Current executing unit
- **[activator]** - Unit that triggered program
- **[argument]** - Command arguments
- **[command]** - Command name
- **[cmdstr]** - Full command string

### Control Structures
- **[if]** - Conditional statement
- **[switch]** - Multi-way conditional
- **[while]** - Loop construct
- **[goto]** - Unconditional jump
- **[break]** - Exit loop
- **[continue]** - Continue iteration
- **[return]** - Return from function
- **[quit]** - Terminate program

### Operators
- **[and]**, **[or]**, **[not]** - Logical operators
- **[set]**, **[unset]**, **[isset]** - Bit operations

## 🎮 **Game Systems**

### Character System
- **[abilities]** - Character attributes (STR, DEX, CON, etc.)
- **[ability_points]**, **[ability_levels]**, **[ability_costs]** - Ability management
- **[skills]**, **[skill_points]**, **[skill_levels]**, **[skill_costs]** - Skill system
- **[spells]**, **[spell_levels]**, **[spell_costs]** - Magic system
- **[weapons]**, **[weapon_levels]**, **[weapon_costs]** - Combat skills

### Character Fields
- **[level]**, **[vlevel]** - Character levels
- **[exp]**, **[experience]**, **[exptol]** - Experience system
- **[hp]**, **[max_hp]** - Health points
- **[mana]**, **[max_mana]** - Magic points
- **[endurance]**, **[max_endurance]** - Stamina
- **[position]**, **[defaultpos]** - Character states
- **[fighting]**, **[opponent]** - Combat status

### Object System
- **[name]**, **[title]**, **[descr]** - Object identification
- **[weight]**, **[baseweight]** - Object weight
- **[value]**, **[cost]**, **[rent]** - Economic properties
- **[capacity]**, **[manipulate]** - Container properties
- **[objectflags]**, **[objecttype]** - Object classification

### Room System
- **[roomflags]** - Room properties
- **[bright]**, **[light]**, **[islight]** - Lighting
- **[exit_to]**, **[exit_names]**, **[exit_info]** - Room exits
- **[mapx]**, **[mapy]** - Coordinates

## 💬 **Communication System**

### Message Functions
- **[send]**, **[sendtext]**, **[sendto]** - Basic messaging
- **[act]**, **[sact]** - Formatted social messages
- **[sendtoall]**, **[sendtoalldil]** - Broadcast functions
- **[pagestring]** - Paginated output
- **[prompt]**, **[clear]**, **[help]** - User interface

### Text Processing
- **[length]**, **[substring]**, **[left]**, **[right]** - String manipulation
- **[strcmp]**, **[strncmp]** - String comparison
- **[split]**, **[replace]**, **[getword]**, **[getwords]** - Text processing
- **[toupper]**, **[tolower]** - Case conversion

## ⚔️ **Combat System**

### Combat Functions
- **[set_fighting]**, **[stop_fighting]** - Combat control
- **[attack_type]**, **[weapon_info]**, **[weapon_name]** - Combat data
- **[meleeattack]**, **[attack_spell]**, **[cast_spell]** - Attack actions
- **[meleedamage]**, **[defensive]**, **[offensive]** - Combat calculations

## 💰 **Economic System**

### Money Management
- **[purse]**, **[paycheck]** - Character money
- **[acc_balance]**, **[acc_total]**, **[acc_modify]** - Bank accounts
- **[transfermoney]**, **[moneystring]** - Money operations

## ⏰ **Time & Calendar**

### Time Functions
- **[mudday]**, **[mudhour]**, **[mudmonth]**, **[mudyear]** - Game time
- **[realtime]**, **[heartbeat]**, **[asctime]** - Real time
- **[wait]**, **[sleep]** - Time control

## 🛠️ **Utility Functions**

### Mathematics
- **[rnd]**, **[openroll]** - Random numbers
- **[max]**, **[min]**, **[abs]** - Math functions

### List Operations
- **[integerlist]**, **[stringlist]** - List types
- **[next]**, **[previous]**, **[head]** - List navigation

### System Utilities
- **[log]**, **[flog]**, **[logcrime]** - Logging
- **[debug]**, **[info]**, **[hasfunc]** - Debugging
- **[secure]**, **[access]**, **[reboot]** - Administration

## 🎭 **DIL Program Management**

### Program Structure
- **[dilbegin]**, **[dilend]** - Program definition
- **[var]**, **[code]** - Program structure

### Program Control
- **[priority]**, **[nopriority]** - Execution control
- **[interrupt]**, **[waitnoop]** - Program flow

### Event Handling
- **[on_activation]**, **[on_goto]** - Event handlers

### DIL Operations
- **[dilcall]**, **[dilfind]**, **[dilcopy]**, **[dildestroy]** - Program management
- **[sendtoalldil]**, **[global_head]**, **[store]** - Inter-program communication

### Development Tools
- **[beginedit]**, **[killedit]**, **[editing]** - Editing
- **[shell]**, **[exec]** - Testing

## 🌍 **World Management**

### Zone System
- **[zone_head]**, **[zhead]**, **[findzone]** - Zone operations
- **[zoneidx]**, **[resetmode]**, **[resettime]** - Zone properties
- **[zonereset]** - Zone resets

### Room Management
- **[room_head]**, **[findroom]**, **[pathto]** - Room operations
- **[rooms]**, **[roomcount]** - Room lists
- **[setroomexit]** - Room connections

### NPC Management
- **[npc_head]**, **[npcs]**, **[npccount]** - NPC operations
- **[npcflags]**, **[profession]**, **[guild]** - NPC properties
- **[master]**, **[follower]**, **[followercount]** - Social relationships

### Object Management
- **[obj_head]**, **[objs]**, **[objcount]** - Object operations
- **[equip]**, **[unequip]**, **[equipment]**, **[addequip]** - Equipment

## 📋 **Unit Operations**

### Finding & Creating
- **[findunit]**, **[findsymbolic]**, **[findrndunit]** - Unit search
- **[load]**, **[destroy]**, **[delunit]**, **[clone]** - Unit lifecycle

### Movement & Linking
- **[transfer]**, **[link]**, **[inside]**, **[outside]** - Unit positioning
- **[remove]**, **[insert]** - Inventory management

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

