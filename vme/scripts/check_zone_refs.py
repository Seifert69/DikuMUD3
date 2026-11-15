#!/usr/bin/env python3
"""
Checks the references between zones: every name@zone a zone uses must name a unit or DIL template
that exists. vmc compiles one zone at a time, so it can't see these; a misspelt or removed name
only shows up in the game (a load that loads nothing, an exit to nowhere, a dilcopy that copies
nothing, a call that fails).

It reads each zone as vmc's preprocessor leaves it (vmc -p) and takes these as references:
- exits (`north to x@zone`), a room's `in x@zone`, `key x@zone`, and in %reset the units after
  load, equip, door, purge, remove, follow, into and in; a name without @zone means this zone,
  and the old form zone/name is read as name@zone;
- `dilcopy x@zone(...)`, and calls `x@zone(...)` in DIL: a template;
- best effort: a string that is exactly "x@zone" ("load("x@zone")", findsymbolic, dilfind,
  link, ...): a unit or a template; not a zone's title, nor a string joined to others with +.
`key nokey` is the zones' way of saying a door has no key, so nokey is never looked up; the
references of the DIL test zone (test.zon) aren't checked: its tests use missing names on purpose.

Zones listed in vme/etc/zonelist but not present (the private zones, in a public checkout) are
skipped; a zone that is neither present nor listed is reported.

usage: check_zone_refs.py [--vmc VMC] [--zones DIR]... [--baseline FILE] [--write-baseline FILE]
  --zones DIR         also read the .zon files in DIR (e.g. a checkout of the private zones)
  --baseline FILE     references listed in FILE (zone<TAB>name@zone) are known and not reported
  --write-baseline F  write every unresolved reference to F (to start or refresh a baseline; the
                      notes after entries already in F are kept)
Exit code 1 when an unresolved reference isn't in the baseline, or a baseline entry is no longer
needed (remove it, so that the reference can't break again unseen); 2 when vmc can't preprocess
a zone, or two files are the same zone.
"""
import argparse
import glob
import os
import re
import subprocess
import sys
from concurrent.futures import ProcessPoolExecutor

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
VME = os.path.join(ROOT, "vme")
TOKEN = re.compile(
    r'(?P<str>"(?:[^"\\]|\\.)*")'
    r"|(?P<line>^#line\s+(?P<lno>\d+)(?:\s+\"(?P<file>[^\"]*)\")?[^\n]*)"
    r"|(?P<comment>//[^\n]*|/\*.*?\*/)"
    r"|(?P<name>[A-Za-z_][A-Za-z0-9_]*(?:@[A-Za-z_][A-Za-z0-9_]*)?)"
    r"|(?P<nl>\n)"
    r"|(?P<other>\S)",
    re.S | re.M,
)
EXACT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*@[A-Za-z_][A-Za-z0-9_]*")
SECTIONS = {"zone", "dil", "rooms", "objects", "mobiles", "reset", "end"}
UNIT_SECTIONS = {"%rooms", "%objects", "%mobiles"}
RESET_WORDS = {"load", "equip", "door", "purge", "remove", "follow", "into", "in"}
NOT_LOOKED_UP = {"nokey"}  # `key nokey`: a door without a key
NOT_CHECKED = {"test"}     # the DIL test zone names missing units on purpose
UNIT, TEMPLATE = 1, 2      # what a reference may name (a string may name either)


def read_lines(path):
    """The words of each line of path, without blank lines and # comments."""
    with open(path) as f:
        return [line.split() for line in f if line.strip() and not line.startswith("#")]


def tokens(path, text):
    """(kind, value, file, line) for each string, name and other character of text."""
    file, lno = os.path.basename(path), 1
    toks = []
    for m in TOKEN.finditer(text):
        if m.group("line"):
            lno = int(m.group("lno")) - 1  # the newline ending the marker line adds one
            if m.group("file"):
                file = os.path.basename(m.group("file"))
        elif m.group("nl"):
            lno += 1
        elif m.group("comment"):
            lno += m.group("comment").count("\n")
        elif m.group("str") is not None:
            toks.append(("str", m.group("str")[1:-1], file, lno))
            lno += m.group("str").count("\n")
        else:
            toks.append(("name" if m.group("name") else "other", m.group(0), file, lno))
    return toks


def split(name):
    n, _, z = name.partition("@")
    return n.lower(), (z.lower() or None)


def scan(path, vmc):
    """Preprocesses and reads one zone: (zone, units, templates, refs), refs being
    (wanted, name, zone or None, file, line); or (None, error) when vmc fails."""
    d, f = os.path.split(path)
    r = subprocess.run([vmc, "-p", "-d", os.path.join(VME, "etc") + "/", "-I" + os.path.join(VME, "include") + "/", f],
                       cwd=d, capture_output=True, encoding="latin-1")
    if r.returncode or not r.stdout:
        return None, "%s: vmc -p failed: %s" % (path, (r.stderr or r.stdout).strip()[-300:])
    toks = tokens(path, r.stdout)

    def ref_at(i):
        """The unit reference starting at token i (name, name@zone or zone/name) and the index after it."""
        n, z = split(toks[i][1])
        if z is None and i + 2 < len(toks) and toks[i + 1][1] == "/" and toks[i + 2][0] == "name":
            return (toks[i + 2][1].lower(), n), i + 3
        return (n, z), i + 1

    zone = os.path.basename(path)[:-4]
    units, templates, refs = set(), set(), []
    section = None
    in_unit = False  # inside a unit definition (between its name and `end`)
    in_dil = False   # inside dilbegin ... dilend
    i = 0
    while i < len(toks):
        kind, val, f, ln = toks[i]
        nxt = toks[i + 1] if i + 1 < len(toks) else ("", "", f, ln)
        low = val.lower()
        if kind == "other" and val == "%" and not in_dil and nxt[1].lower() in SECTIONS:  # not DIL's modulo
            section = "%" + nxt[1].lower()
            # the zone's name is on the %zone line; without one vmc uses the file's name
            if section == "%zone" and i + 2 < len(toks) and toks[i + 2][0] == "name" and toks[i + 2][3] == ln:
                zone = toks[i + 2][1].lower()
            in_unit = False
            i += 2
        elif kind == "str":
            prev = toks[i - 1][1] if i else ""
            # not a zone's title ("Midgaard@Mainland"), nor part of a string built with +
            if EXACT.fullmatch(val) and prev.lower() != "title" and prev != "+" and nxt[1] != "+":
                n, z = split(val)
                refs.append((UNIT | TEMPLATE, n, z, f, ln))
            i += 1
        elif kind != "name":
            i += 1
        elif low == "dilbegin":
            # dilbegin [modifiers, return type, fnpri(...)] name(...): the name is the last word before "("
            in_dil = True
            j, name = i + 1, None
            while j < len(toks) and toks[j][1] != "(":
                if toks[j][1].lower() == "fnpri" and j + 1 < len(toks) and toks[j + 1][1] == "(":
                    j = next((k for k in range(j + 1, len(toks)) if toks[k][1] == ")"), j)
                elif toks[j][0] == "name":
                    name = toks[j][1]
                j += 1
            if name:
                templates.add(split(name)[0])
            i = j
        elif low == "dilend":
            in_dil = False
            i += 1
        elif "@" in val and nxt[1] == "(":
            n, z = split(val)
            refs.append((TEMPLATE, n, z, f, ln))
            i += 1
        elif low == "dilcopy" and nxt[0] == "name":
            n, z = split(nxt[1])
            refs.append((TEMPLATE, n, z, f, ln))
            i += 2
        elif in_dil:
            i += 1
        elif section in UNIT_SECTIONS and not in_unit:
            units.add(low)
            in_unit = True
            i += 1
        elif section in UNIT_SECTIONS and low == "end":
            in_unit = False
            i += 1
        elif nxt[0] == "name" and (
                (section in UNIT_SECTIONS and (low in ("to", "key") or (low == "in" and section == "%rooms")))
                or (section == "%reset" and low in RESET_WORDS)):
            (n, z), i = ref_at(i + 1)
            if n not in NOT_LOOKED_UP:
                refs.append((UNIT, n, z, f, ln))
        else:
            i += 1
    return zone, units, templates, refs


def main():
    ap = argparse.ArgumentParser(description="Checks the name@zone references between zones.")
    ap.add_argument("--vmc", default=os.path.join(VME, "bin", "vmc"))
    ap.add_argument("--zones", action="append", default=[])
    ap.add_argument("--baseline")
    ap.add_argument("--write-baseline")
    a = ap.parse_args()
    a.vmc = os.path.abspath(a.vmc)  # vmc runs in each zone's directory

    paths = sorted(glob.glob(os.path.join(VME, "zone", "*.zon")))
    for d in a.zones:
        paths += sorted(glob.glob(os.path.join(os.path.expanduser(d), "*.zon")))
    zones, where, errors = {}, {}, []
    with ProcessPoolExecutor() as ex:
        for path, res in zip(paths, ex.map(scan, paths, [a.vmc] * len(paths))):
            if res[0] is None:
                errors.append(res[1])
            elif res[0] in zones:
                errors.append("%s and %s are both zone %s" % (where[res[0]], path, res[0]))
            else:
                zones[res[0]], where[res[0]] = res[1:], path
    if errors:
        print("\n".join(errors), file=sys.stderr)
        return 2
    listed = {words[0].lower() for words in read_lines(os.path.join(VME, "etc", "zonelist"))}
    known, notes = set(), {}
    if a.baseline:
        if not os.path.exists(a.baseline):
            print("%s: no such baseline" % a.baseline, file=sys.stderr)
            return 2
        known = {(words[0], words[1]) for words in read_lines(a.baseline)}
    if a.write_baseline and os.path.exists(a.write_baseline):
        notes = {(w[0], w[1]): " ".join(w[2:]) for w in read_lines(a.write_baseline) if len(w) > 2}

    unresolved = {}  # (zone, name@zone) -> the places it is used
    for z, (units, templates, refs) in sorted(zones.items()):
        if z in NOT_CHECKED:
            continue
        for wanted, n, rz, f, ln in refs:
            tz = rz or z
            if tz not in zones:
                if tz in listed:
                    continue  # a zone that isn't here (a private zone in a public checkout)
                why = "no zone %s" % tz
            elif (wanted & UNIT and n in zones[tz][0]) or (wanted & TEMPLATE and n in zones[tz][1]):
                continue
            else:
                what = {UNIT: "unit", TEMPLATE: "template"}.get(wanted, "unit or template")
                why = "no %s %s in %s" % (what, n, tz)
            unresolved.setdefault((z, "%s@%s" % (n, tz)), []).append("%s:%d: %s@%s: %s" % (f, ln, n, tz, why))

    if a.write_baseline:
        with open(a.write_baseline, "w") as out:
            out.write("# Unresolved name@zone references known when this was written (zone, reference);\n"
                      "# check_zone_refs.py reports only references not listed here. Fix and remove them.\n")
            for z, ref in sorted(unresolved):
                note = notes.get((z, ref))
                out.write("%s\t%s%s\n" % (z, ref, "\t" + note if note else ""))
        print("%d zones, %d references unresolved, written to %s" % (len(zones), len(unresolved), a.write_baseline),
              file=sys.stderr)
        return 0
    new = {k: places for k, places in unresolved.items() if k not in known}
    for places in new.values():
        print("\n".join(places))
    stale = known - unresolved.keys()
    if stale:
        print("%d baseline entr%s no longer needed (fixed): %s" % (len(stale), "y is" if len(stale) == 1 else "ies are",
              ", ".join("%s %s" % s for s in sorted(stale))), file=sys.stderr)
    print("%d zones, %d references unresolved (%d new)" % (len(zones), len(unresolved), len(new)), file=sys.stderr)
    return 1 if new or stale else 0


if __name__ == "__main__":
    sys.exit(main())
