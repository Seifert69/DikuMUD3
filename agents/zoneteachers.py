#!/usr/bin/env python3
"""
Every teacher in the zone sources, for questions such as "where can I practice dagger?": a block
per teacher,
       TEACHER sym@zone "title" in room@zone[, room@zone] [members of: guild]
         <type> (guild "<guild name>"; everything profession <p> may learn, adjusted below)
         <skill> max <n>[ guild level <n>], ...
where <type> is abilities, skills, spells or weapons. A teacher that names a profession teaches
everything that profession can learn (its cost table, etc/), not only what is listed; the listed
entries add to that or change its limits. "members of" lists the guilds whose TrainMemberChk
keeps everyone else away.

The zones are run through the zone compiler's preprocessor (vmc -p), since most teachers are
macros in include/guilddef.h.

usage: zoneteachers.py ZONEDIR [VMC] > teachers.txt
       (VMC defaults to vme/bin/vmc next to this script; money comes from vme/etc,
        includes from ZONEDIR/include. When ZONEDIR/zonelist is there, the server's list of
        the zones it loads, only those are read.)
"""
import os, re, shutil, subprocess, sys, tempfile

from zoneitems import Zones, norm

HERE = os.path.dirname(os.path.abspath(__file__))
TYPES = ("abilities", "spells", "skills", "weapons")
STRING = re.compile(r'"((?:[^"\\]|\\.)*)"', re.S)


def zone_files(zonedir):
    """The .zon files the server loads (all of them when there is no zonelist)"""
    names = sorted(n for n in os.listdir(zonedir) if n.endswith(".zon"))
    try:
        lines = open(os.path.join(zonedir, "zonelist")).read().splitlines()
    except OSError:
        return names
    loaded = {ln.split()[0] + ".zon" for ln in lines if ln.strip() and not ln.startswith("#")}
    return [n for n in names if n in loaded]


def preprocess(zonedir, vmc):
    """zone file name -> its text after vmc -p, compiled in a scratch copy (vmc writes files next to it)"""
    out = {}
    etc = os.path.join(HERE, "..", "vme", "etc") + "/"
    inc = os.path.join(zonedir, "include") + "/"
    with tempfile.TemporaryDirectory() as tmp:
        for name in zone_files(zonedir):
            shutil.copy(os.path.join(zonedir, name), tmp)
            r = subprocess.run([vmc, "-p", "-d", etc, "-I" + inc, name], cwd=tmp,
                               capture_output=True, text=True, encoding="latin-1")
            out[name] = r.stdout
    return out


def units(text, zone):
    """(sym@zone, unit text) for every unit in a preprocessed zone"""
    section, sym, buf, in_string = None, None, [], False
    for line in text.split("\n"):
        if in_string:
            buf.append(line)
            in_string = line.count('"') % 2 == 0
            continue
        m = re.match(r"^%(\w+)", line)
        if m:
            if m.group(1) == "zone" and len(line.split()) > 1:
                zone = line.split()[1].lower()
            section = m.group(1)
            continue
        if section not in ("rooms", "objects", "mobiles"):
            continue
        if sym is None:
            m = re.match(r"^\s*([A-Za-z0-9_]+)\s*$", line)
            if m and m.group(1) != "end":
                sym, buf = norm(m.group(1), zone), []
            continue
        if re.match(r"^\s*end\s*$", line):
            yield sym, "\n".join(buf)
            sym = None
            continue
        buf.append(line)
        in_string = line.count('"') % 2 == 1


def teach_data(s):
    """The parsed text of a SFUN_TEACH_INIT: (type, profession, guild, [(skill, max, guild level)])"""
    fields = [f.strip() for f in s.split(";")]
    if len(fields) < 10:
        return None
    kind, prof, guild = fields[0], fields[1], fields[2]
    rest, entries, i = fields[10:], [], 0
    while i + 5 <= len(rest) and rest[i]:
        try:
            glevel, top, name = int(rest[i]), int(rest[i + 1]), rest[i + 2]
        except ValueError:
            break
        i += 5  # level, max, name, gold min, gold max
        while i < len(rest) and rest[i] not in ("0", ""):  # the obsolete point costs, then 0
            i += 1
        i += 1
        entries.append((name, top, glevel))
    return kind, prof, guild, entries


def main():
    zonedir = sys.argv[1]
    vmc = sys.argv[2] if len(sys.argv) > 2 else os.path.join(HERE, "..", "vme", "bin", "vmc")
    z = Zones()
    for name in zone_files(zonedir):
        z.read(os.path.join(zonedir, name))
    shown = set()
    for name, text in preprocess(zonedir, vmc).items():
        for sym, body in units(text, name[:-4]):
            if sym in shown:
                continue
            specials = re.findall(r'special\s+25\s+"((?:[^"\\]|\\.)*)"', body, re.S)
            if not specials:
                continue
            shown.add(sym)
            m = re.search(r'^\s*title\s+"([^"]*)"', body, re.M)
            title = re.sub(r"<[^>]*>", "", m.group(1)).strip() if m else ""
            members = sorted(set(a.strip().strip('"') for a in
                                 re.findall(r"TrainMemberChk@guilds\s*\(\s*([^,)]*)", body)))
            rooms = ", ".join(z.rooms_of(sym)) or "?"
            print('TEACHER %s "%s" in %s%s' % (sym, title, rooms,
                                              " members of: " + ", ".join(members) if members else ""))
            for s in specials:
                t = teach_data(" ".join(s.split()))
                if not t:
                    print("  (unreadable teacher data)")
                    continue
                kind, prof, guild, entries = t
                how = ["guild \"%s\"" % guild if guild else "no guild"]
                if prof:
                    how.append("everything profession %s may learn, adjusted below" % prof)
                print("  %s (%s)" % (kind, "; ".join(how)))
                if entries:
                    print("    " + ", ".join("%s max %d%s" % (n, top, " guild level %d" % g if g else "")
                                              for n, top, g in entries))
            print()


if __name__ == "__main__":
    main()
