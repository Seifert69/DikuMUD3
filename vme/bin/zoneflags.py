#!/usr/bin/env python3
"""
Finds flag fields given the wrong kind of flag in zone sources. vmc can't: the preprocessor
turns every constant into a number first, so `flags {CHAR_SNEAK}` quietly sets the unit flag
with the same bit (UNIT_FL_NO_WEATHER).

    flags       UNIT_FL_*                    every unit
    romflags    OBJ_* on objects, CHAR_* on monsters (rooms have no constants of their own)
    manipulate  MANIPULATE_*
    open        EX_* (containers, and doors in exits)
    npcflags    NPC_*

A line per mistake: file:line: field {...CONSTANT...}: what it is and what belongs there.
Exits 1 when it finds any.

usage: zoneflags.py [file.zon|file.h ...]   (default: vme/zone/*.zon and vme/include/*.h)
"""
import bisect, glob, os, re, sys

KINDS = {"UNIT_FL_": "unit", "OBJ_": "object", "CHAR_": "monster", "EX_": "exit",
         "MANIPULATE_": "manipulate", "NPC_": "npc"}
FIELDS = {"flags": {"unit"}, "manipulate": {"manipulate"}, "open": {"exit"}, "npcflags": {"npc"}}
ROMFLAGS = {"objects": {"object"}, "mobiles": {"monster"}, "rooms": {"room"}}  # no "room" constants
ANY_UNIT = {"object", "monster"}  # romflags in an include file's macro: can't tell which

# A field at the start of a line (or of a macro's body), with a {...} list or the rest of the line
FIELD_RE = re.compile(r"^[ \t]*(?:#define[ \t]+\w+(?:\([^)]*\))?[ \t]+)?"
                      r"(flags|romflags|manipulate|open|npcflags)\b[ \t]*(\{[^}]*\}|[^\n]*)", re.M)


def blank(m):
    """The match replaced by its newlines alone, so line numbers stay right."""
    return "\n" * m.group().count("\n")


def kind(const):
    return next((k for p, k in KINDS.items() if const.startswith(p)), None)


def scan(path):
    """Yields (line number, field, constant, its kind, the kinds that belong there)."""
    with open(path, encoding="latin-1") as f:
        text = f.read()
    text = re.sub(r"/\*.*?\*/|//[^\n]*", blank, text, flags=re.S)
    text = re.sub(r"\bdilbegin\b.*?\bdilend\b", blank, text, flags=re.S)  # DIL has its own flags
    sections = [(m.start(), m.group(1)) for m in re.finditer(r"^[ \t]*%(\w+)", text, re.M)]
    starts = [s for s, _ in sections]
    for m in FIELD_RE.finditer(text):
        field = m.group(1)
        i = bisect.bisect(starts, m.start())
        want = FIELDS.get(field) or ROMFLAGS.get(sections[i - 1][1] if i else None, ANY_UNIT)
        for const in re.findall(r"\b[A-Z][A-Z0-9_]+\b", m.group(2)):
            k = kind(const)
            if k and k not in want:
                yield text.count("\n", 0, m.start(1)) + 1, field, const, k, want


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    paths = sys.argv[1:] or sorted(glob.glob(os.path.join(here, "../zone/*.zon")) +
                                   glob.glob(os.path.join(here, "../include/*.h")))
    found = False
    for path in paths:
        for line, field, const, k, want in scan(path):
            print("%s:%d: %s {...%s...}: a %s flag where %s flags belong"
                  % (os.path.relpath(path), line, field, const, k,
                     " or ".join(sorted(want))))
            found = True
    sys.exit(1 if found else 0)


if __name__ == "__main__":
    main()
