#!/usr/bin/env python3
import os
import subprocess
import sys
import time

# ------------------------------------------------------------------
# update_all_dil_help.py
# Downloads ALL DIL manual pages using getwikidilpage.py
# Works with pages that have () and pages that don't
# ------------------------------------------------------------------

# Full list of DIL functions / intrinsics (exactly as they appear in the manual)
FUNCTIONS = [
    "string", "stringlist", "integer", "integerlist", "extraptr", "cmdptr", "unitptr",
    "zoneptr", "SFB_CMD", "SFB_DONE", "SFB_TICK", "SFB_COM", "SFB_DEAD", "SFB_MSG",
    "SFB_PRE",
    "cmdstr", "excmdstr", "excmdstr_case", "self", "activator", "target", "medium",
    "power", "argument", "heartbeat", "null", "weather", "realtime",
    "mudmud(day,month,..)", "if", "goto", "while", "break", "continue", "on_..._goto_...", "foreach", "in",
    "quit", "return", "UNIT_ST_OBJ", "UNIT_ST_ROOM", "UNIT_ST_PC and UNIT_ST_NPC",
    "UNIT_ST_NPC", "UNIT_ST_PC", "asctime", "strcmp", "strncmp", "textformat",
    "spellindex", "spellinfo", "beginedit", "cancarry", "fits", "replace",
    "restore", "meleeattack", "meleedamage", "mid", "moneystring", "equipment",
    "tolower", "toupper", "visible", "opponent", "purse", "atoi", "check_password",
    "command", "delstr", "delunit", "dildestroy", "dilfind", "itoa", "isaff",
    "islight", "isplayer", "isset", "paycheck", "filesize", "findunit",
    "findrndunit", "findroom", "findsymbolic", "getword", "getwords", "ghead",
    "split", "left", "length", "load", "loadstr", "openroll", "pathto",
    "pagestring", "right", "rnd", "follow", "flog", "logcrime", "acc_modify",
    "strdir", "set_password", "store", "position_update", "send_done", "send_pre",
    "set", "unset", "addcolor", "delcolor", "getcolor", "changecolor", "gamestate",
    "addextra", "addstring", "subextra", "substring", "stop_fighting", "subaff",
    "addaff", "destroy", "walkto", "link", "weapon_name", "weapon_info",
    "skill_name", "reboot", "killedit", "experience", "act", "exec", "wait",
    "savestr", "remove", "reset_level", "reset_vlevel", "reset_race", "secure",
    "unitdir", "unsecure", "block", "priority", "nopriority", "addequip",
    "unequip", "delete_player", "dilcopy", "sendtext", "change_speed",
    "transfermoney", "set_fighting", "set_weight_base", "set_weight", "setbright",
    "log", "send", "sendto", "sendtoall", "sendtoalldil", "cast_spell",
    "attack_spell", "insert", "interrupt", "clear", "on_activation", "shell",
    "getaffects"
]

# Name of your single-page downloader script
DOWNLOADER = "getwikidilpage.py"

if not os.path.isfile(DOWNLOADER):
    print(f"Error: {DOWNLOADER} not found in current directory!")
    print("Make sure both scripts are in the same folder.")
    sys.exit(1)

print(f"Starting update of {len(FUNCTIONS)} DIL help pages...")
print("(Running in background with & — press Ctrl+C to stop early)\n")

failed = []

for func in FUNCTIONS:
    print(f"Fetching: {func:25}", end=" ")
    result = subprocess.run(["python3", DOWNLOADER, func], capture_output=True, text=True)
    
    if result.returncode == 0 and os.path.isfile(func.lower() + ".hlp"):
        size = os.path.getsize(func.lower() + ".hlp")
        print(f"Done ({size} bytes)")
    else:
        print("Failed")
        failed.append(func)
    
    # Be nice to the server
    time.sleep(0.2)

print("\n" + "="*60)
print("UPDATE COMPLETE!")
print(f"Successfully downloaded: {len(FUNCTIONS) - len(failed)}")
if failed:
    print(f"Failed ({len(failed)}): {', '.join(failed)}")
else:
    print("All pages downloaded successfully!")
print("="*60)