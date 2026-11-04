#!/usr/bin/env python3
"""
A map of the world from the zone sources, for `mud.py way`: one line per room,
       room@zone<TAB>title<TAB>n=room@zone e=room@zone d=room@zone*<TAB>sector
where * marks an exit with a door, ! a hidden door (search for it, then open it), and sector is the room's movement type (city, forest,
water_swim...). A room inside another room (`in`, entered like the barrel to the thieves'
guild) gets enter:barrel=room@zone in the room around it, and leave=room@zone itself.
Rooms only: no monsters, objects or DIL.

usage: zonemap.py ZONEDIR > map.txt
"""
import glob, os, re, sys

DIRS = {"north": "n", "east": "e", "south": "s", "west": "w", "up": "u", "down": "d",
        "northeast": "ne", "northwest": "nw", "southeast": "se", "southwest": "sw"}
EXIT = re.compile(r"^\s*(%s)\s+to\s+([A-Za-z0-9_]+(?:[@/][A-Za-z0-9_]+)?)(.*)" % "|".join(DIRS))
ROOM = re.compile(r"^\s*([A-Za-z0-9_]+)\s*(//.*)?$")
SECTION = re.compile(r"^%(\w+)")


def uncomment(text):
    """The zone source without /* */ and // comments; strings are kept as they are."""
    out, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if c == '"':
            j = text.find('"', i + 1)
            j = n if j < 0 else j + 1
            out.append(text[i:j])
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("\n" * text.count("\n", i, j))
            i = j
        elif text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j
        else:
            j = min([k for k in (text.find('"', i), text.find("/", i)) if k >= 0] or [n])
            if j == i:  # a lone /
                j = i + 1
            out.append(text[i:j])
            i = j
    return "".join(out)


def rooms(path):
    zone = os.path.basename(path)[:-4]
    section, room, in_string = None, None, False
    for line in uncomment(open(path, encoding="latin-1").read()).split("\n"):
        if in_string:  # inside a multi-line "..." (descriptions)
            if line.count('"') % 2:
                in_string = False
            continue
        m = SECTION.match(line)
        if m:
            if m.group(1) == "zone":
                zone = line.split()[1] if len(line.split()) > 1 else zone
            section = m.group(1)
            continue
        if section != "rooms":
            continue
        if room is None:
            m = ROOM.match(line)
            if m and m.group(1) not in ("end", "descr", "title", "names", "extra", "movement"):
                room = {"id": m.group(1).lower(), "title": "", "exits": [], "sector": "",
                        "names": [], "in": None, "enter": False, "door": False}
                exit_open = False
            continue
        if exit_open:  # the exit's keyword, key and open lines, up to its ";"
            if re.match(r"^\s*open\s*\{.*EX_OPEN_CLOSE", line) and room["exits"][-1][-1] not in "*!":
                room["exits"][-1] += "!" if "EX_HIDDEN" in line else "*"
            exit_open = ";" not in line and not re.match(r"^\s*end\s*$", line)
            if exit_open or ";" in line:
                continue
        if re.match(r"^\s*end\s*$", line):
            yield zone, room
            room = None
            continue
        m = re.match(r'^\s*title\s+"([^"]*)"', line)
        if m:
            room["title"] = re.sub(r"<[^>]*>|&[a-zA-Z0-9]+", "", m.group(1)).strip()
            continue
        m = re.match(r"^\s*names\s*\{(.*)\}", line)
        if m:
            room["names"] = re.findall(r'"([^"]+)"', m.group(1))
            continue
        m = re.match(r"^\s*in\s+([A-Za-z0-9_@/]+)\s*$", line)
        if m:
            room["in"] = m.group(1).lower()
            continue
        if re.match(r"^\s*manipulate\s*\{.*MANIPULATE_ENTER", line):
            room["enter"] = True
        if re.match(r"^\s*open\s*\{.*EX_OPEN_CLOSE", line):
            room["door"] = True
        m = re.match(r"^\s*movement\s+SECT_(\w+)", line)
        if m:
            room["sector"] = m.group(1).lower()
            continue
        m = EXIT.match(line)
        if m:
            to = m.group(2).lower()
            if "/" in to:  # zone/room
                to = "%s@%s" % tuple(reversed(to.split("/")))
            if "@" not in to:
                to += "@" + zone
            flags = m.group(3)
            door = ("!" if "EX_HIDDEN" in flags else "*") if "EX_OPEN_CLOSE" in flags else ""
            room["exits"].append("%s=%s%s" % (DIRS[m.group(1)], to, door))
            exit_open = ";" not in flags
        if line.count('"') % 2:
            in_string = True


def ref(to, zone):
    if "/" in to:  # zone/room
        to = "%s@%s" % tuple(reversed(to.split("/")))
    return to if "@" in to else to + "@" + zone


def main():
    world = {}
    for path in sorted(glob.glob(os.path.join(sys.argv[1], "*.zon"))):
        for zone, r in rooms(path):
            world["%s@%s" % (r["id"], zone)] = r
            r["zone"] = zone
    for rid, r in world.items():  # rooms in rooms: enter the inner one, leave it to the outer
        outer = r["in"] and ref(r["in"], r["zone"])
        if outer in world and r["enter"] and r["names"]:
            world[outer]["exits"].append("enter:%s=%s%s" % (r["names"][-1].split()[-1].lower(), rid, "*" if r["door"] else ""))
            r["exits"].append("leave=%s" % outer)
    for rid, r in world.items():
        print("%s\t%s\t%s\t%s" % (rid, r["title"], " ".join(r["exits"]), r["sector"]))


if __name__ == "__main__":
    main()
