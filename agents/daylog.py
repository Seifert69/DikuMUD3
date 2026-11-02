#!/usr/bin/env python3
"""
A day of the helper agent's mud session, cut down for the daily review (run-review.sh): what he
typed, what was said to him and by him, deaths, kills and experience, quests, hunger and
thirst, what he got, gave and dropped, and where he went. Room descriptions, monsters in the
room and fight rounds are left out; walks are one line ("> walk e e s (3) to room@zone").

usage: daylog.py session-<date>.log > day.txt
"""
import re, sys

MOVES = {"n", "s", "e", "w", "u", "d", "ne", "nw", "se", "sw", "north", "south", "east", "west",
         "up", "down", "northeast", "northwest", "southeast", "southwest"}
KEEP = re.compile(
    r"tells you|You tell|says '|You say|asks '|You ask|shouts|whispers|"
    r"has entered the world|has left the world|"
    r"\bdie[sd]?\b|killed|is dead|R\.I\.P|experience|\blevel\b|"
    r"[Qq]uest|Ongoing|Complete|"
    r"thirsty|hungry|You are full|You drink|You eat|"
    r"You get|You give|gives you|You drop|You receive|You buy|You sell|That cost|"
    r"You wield|You wear|You stop using|You bury|"
    r"You have \d|You are a \d|You are carrying|You can't|You cannot|too heavy|hands are full|"
    r"teaches you|You practice|You learn|"
    r"platinum|gold piece|silver piece|copper piece|iron piece|coins?\b",
    re.I)


def main():
    out, walk, room = [], [], ""
    skip_descr = False

    def emit(line):
        if out and out[-1][0] == line:
            out[-1][1] += 1
        else:
            out.append([line, 1])

    def flush():
        if walk:
            emit("> walk %s (%d) to %s" % (" ".join(walk), len(walk), room))
            walk.clear()

    for line in open(sys.argv[1], encoding="utf-8", errors="replace"):
        line = line.rstrip()
        if skip_descr:  # the room text after "[room ...]": indented lines and what's in the room
            if line and (line[0].isspace() or re.search(r" is here|stands here|\[x\d+\]", line)):
                continue
            skip_descr = False
        if line.startswith("> "):
            cmd = line[2:].strip()
            if cmd.lower() in MOVES:
                walk.append(cmd.lower())
                continue
            flush()
            emit(line[:300])
            continue
        if line.startswith("[room "):
            room = line[6:].replace("]", "", 1)
            skip_descr = True
            if not walk:
                emit("  at " + room)
            continue
        if not line.strip() or re.match(r"^\s+\d{4}-\d\d-\d\d \d\d:\d\d ", line):
            continue  # blank lines and the remembered lines mud.py replays at a logon
        if (line.startswith("[") and not line.startswith("[x")) or KEEP.search(line):
            flush()
            emit(line[:300])
    flush()
    for line, n in out:
        print(line if n == 1 else "%s  [x%d]" % (line, n))


if __name__ == "__main__":
    main()
