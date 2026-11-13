#!/usr/bin/env python3
"""
What each player race brings, for questions such as "is a dwarf a good thief?": a block per race,
       RACE dwarf
         creation: good choice for Fighter; not recommended for Thief, Conjurer
         starting abilities: strength 6, constitution 4, ...
         abilities: strength +3, hit points +2, ..., magic -3
         weapons +2: ..., +1: ..., -1: ...
         skills ... / spells ...
         lifespan about N years (male), M (female)
The bonuses are the race lines of etc/abilities.def, commands.def (skills), spells.def and
weapons.def; a value outside the range the server accepts is ignored by the server (counts as 0)
and is left out here too. Only entries some profession can learn are listed, and only bonuses
other than 0. Starting abilities and the creation advice come from basis.zon (pcinitabils and
the race menu).

usage: raceinfo.py ETCDIR BASIS_ZON > races.txt
"""
import re, sys

RACES = ["human", "elf", "dwarf", "halfling", "gnome", "half-orc", "half-ogre", "half-elf",
         "brownie", "groll", "darkelf"]
# The DIL names in basis.zon
DIL_RACE = {"HUMAN": "human", "ELF": "elf", "DWARF": "dwarf", "HALFLING": "halfling",
            "GNOME": "gnome", "HALF_ORC": "half-orc", "HALF_OGRE": "half-ogre",
            "HALF_ELF": "half-elf", "BROWNIE": "brownie", "GROLL": "groll",
            "DARK_ELF": "darkelf"}
ABIL = {"STR": "strength", "DEX": "dexterity", "CON": "constitution", "HP": "hit points",
        "BRA": "brain", "CHA": "charisma", "DIV": "divine", "MAG": "magic"}
# (file, kind, accepted race range) as the server's loaders check it
DEFS = [("abilities.def", "abilities", (-3, 4)), ("weapons.def", "weapons", (-3, 3)),
        ("commands.def", "skills", (-3, 3)), ("spells.def", "spells", (-3, 3))]


def entries(path):
    """Yield (name, {race: bonus}, learnable) per entry of a .def file."""
    name, race, prof = None, {}, []
    def done():
        return name, race, any(p > -7 for p in prof)
    for line in open(path, errors="replace"):
        line = line.split("//")[0].strip()
        m = re.match(r"index\s*=", line)
        if m:
            if name:
                yield done()
            name, race, prof = None, {}, []
            continue
        m = re.match(r"name\s*=\s*(.+)", line)
        if m:
            name = m.group(1).strip()
            continue
        m = re.match(r"race\s+(.+?)\s*=\s*(-?\d+)", line)
        if m:
            race[m.group(1).lower()] = int(m.group(2))
            continue
        m = re.match(r"profession\s+.+?\s*=\s*(-?\d+)", line)
        if m:
            prof.append(int(m.group(1)))
    if name:
        yield done()


def bonuses(etc):
    """{race: {kind: {name: bonus}}} with the values the server actually uses."""
    out = {r: {k: {} for _, k, _ in DEFS} for r in RACES}
    for fn, kind, (lo, hi) in DEFS:
        for name, race, learnable in entries(f"{etc}/{fn}"):
            if kind != "abilities" and not learnable:
                continue
            for r, v in race.items():
                if r in out and v != 0 and lo <= v <= hi:
                    out[r][kind][name] = v
    return out


def start_abilities(basis):
    """{race: {ability: value}} from pcinitabils in basis.zon."""
    m = re.search(r"dilbegin pcinitabils\(\);(.*?)self\.skills\[", basis, re.S)
    out, cur = {}, None
    for line in m.group(1).splitlines():
        r = re.search(r"self\.race\s*==\s*RACE_(\w+)", line)
        if r:
            cur = DIL_RACE.get(r.group(1))
            out[cur] = {}
        elif re.match(r"\s*else\s*$", line):
            cur = None
        a = re.search(r"abilities\[ABIL_(\w+)\s*\]\s*:=\s*(\d+)", line)
        if a and cur:
            out[cur][ABIL[a.group(1)]] = int(a.group(2))
    return out


def creation_advice(basis):
    """{race: (good for [professions], not recommended for [professions])} from the race menu."""
    out = {r: ([], []) for r in RACES}
    norm = lambda s: s.lower().replace("dark-elf", "darkelf")
    for prof, block in re.findall(r"self\.profession == PROFESSION_(\w+)\)\s*\{(.*?)\}", basis, re.S):
        prof = prof.capitalize()
        for line in re.findall(r'"([^"]*)<br/>"', block):
            m = re.match(r"\s*(?:\d+\))?\s*([\w-]+)\s*(\(good choice\)|- not recommended)?", line)
            if m and m.group(2) and norm(m.group(1)) in out:
                out[norm(m.group(1))][0 if "good" in m.group(2) else 1].append(prof)
    return out


def lifespans(etc):
    out, cur = {}, None
    for line in open(f"{etc}/races.def", errors="replace"):
        m = re.match(r"\s*name\s*=\s*(\S+)", line)
        if m:
            cur = m.group(1).lower()
        m = re.match(r"\s*lifespan (male|female)\s*=\s*\{\s*(\d+),\s*(\d+),\s*(\d+)\}", line)
        if m and cur:
            base, n, d = map(int, m.group(2, 3, 4))
            out.setdefault(cur, {})[m.group(1)] = round(base + n * (d + 1) / 2)
    return out


def grouped(d):
    """'+2: a, b; +1: c; -1: d' with the best first."""
    by = {}
    for name, v in d.items():
        by.setdefault(v, []).append(name)
    return "; ".join(f"{v:+d}: {', '.join(sorted(by[v]))}" for v in sorted(by, reverse=True))


def main():
    etc, basis = sys.argv[1], open(sys.argv[2], errors="replace").read()
    bon, start, advice, life = bonuses(etc), start_abilities(basis), creation_advice(basis), lifespans(etc)
    for r in RACES:
        print(f"RACE {r}")
        good, bad = advice[r]
        parts = ([f"good choice for {', '.join(good)}"] if good else []) + \
                ([f"not recommended for {', '.join(bad)}"] if bad else [])
        print(f"  creation: {'; '.join(parts) or 'no warnings: fine for all four starting professions'}")
        if r in start:
            print("  starting abilities: " + ", ".join(f"{a} {v}" for a, v in start[r].items()))
        print("  abilities: " + (", ".join(f"{a} {v:+d}" for a, v in
                                           sorted(bon[r]["abilities"].items(), key=lambda x: -x[1]))
                                 or "all 0"))
        for kind in ("weapons", "skills", "spells"):
            print(f"  {kind} {grouped(bon[r][kind]) or 'all 0'}")
        if r in life:
            print(f"  lifespan about {life[r].get('male')} years (male), {life[r].get('female')} (female)")
        print()


main()
