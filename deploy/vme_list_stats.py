"""Filter the output of `vme -L` down to what player statistics need (issue #448).

stdout: one JSON line per listed character with level, virtual level, race, profession, sex,
creation time, last connect time and seconds played; the input for vstats_build.py.
No names, ids, emails or maiden names.
stderr: a summary and a player id check, counts only.
Exits 1 if the listing isn't a complete `vme -L` listing in the format this script knows.

Pipe the listing straight in, so the names, emails and maiden names in it never reach a file.
From vme/bin; -L opens no port and, without -l, logs to the terminal, so a live server is fine:
    ./vme -L | python3 -I ../../deploy/vme_list_stats.py > players.jsonl 2> summary.txt
"""
import json, re, sys
from collections import Counter

HEADER = ("name;id;Level;Admin;days since login;email;maiden;created;birth;played;race;profession;sex;"
          "virtual level;created epoch;last connect epoch;played seconds;flags")
NAMES = HEADER.split(";")

sys.stdin.reconfigure(errors="replace")  # email and maiden name are whatever bytes the player typed
ids = Counter()
rejected = errors = unknown = above_top = below_zero = 0
next_id = None
header_seen = False
for line in sys.stdin:
    line = line.rstrip("\n")
    f = line.split(";")
    if line.startswith('"') and len(f) == len(NAMES):  # a listed character
        r = dict(zip(NAMES, f))
        ids[int(r["id"])] += 1
        print(json.dumps({"lvl": int(r["Level"]), "vlvl": int(r["virtual level"]), "race": r["race"],
                          "prof": r["profession"], "sex": r["sex"], "created": int(r["created epoch"]),
                          "last": int(r["last connect epoch"]), "played": int(r["played seconds"])},
                         separators=(",", ":")))
    elif line.startswith('"') and len(f) >= 2 and f[1].startswith("ERROR: "):
        if f[1] == "ERROR: Corrupt":
            rejected += 1   # unreadable, illegal name or failed sanity check
        else:
            errors += 1     # the loader threw
    elif line.startswith('"'):
        unknown += 1        # a character line with the wrong number of columns
    elif line == HEADER:
        header_seen = True
    elif line.startswith("Maximum ID exceeded"):  # sanity_check(), rejected
        above_top += 1
    elif line.startswith("Illegal ID < 0!"):
        below_zero += 1
    elif m := re.match(r"Maximum ID was -?\d+ / Top (-?\d+)$", line):
        next_id = int(m.group(1))

shared = {i: n for i, n in ids.items() if n > 1}
print(f"listed: {sum(ids.values())} characters; {rejected} rejected by vme -L (unreadable, illegal name "
      f"or failed sanity check); {errors} failed to load", file=sys.stderr)
print(f"player ids: {len(ids)} distinct; {len(shared)} shared by {sum(shared.values())} characters; "
      f"{below_zero + sum(n for i, n in ids.items() if i <= 0)} characters with id 0 or below", file=sys.stderr)
if next_id is not None:
    print(f"next id (players.id): {next_id}; {above_top + sum(n for i, n in ids.items() if i >= next_id)} "
          f"characters with an id at or above it", file=sys.stderr)
if unknown:
    print(f"ERROR: {unknown} character lines without {len(NAMES)} columns", file=sys.stderr)
if unknown or not header_seen or next_id is None or not ids:
    print("ERROR: not a complete vme -L listing in the expected format (header, characters and the "
          "'Maximum ID was' line); vme -L and this script may be from different versions", file=sys.stderr)
    sys.exit(1)
