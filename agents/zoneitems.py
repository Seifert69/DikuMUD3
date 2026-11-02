#!/usr/bin/env python3
"""
Where each object in the zone sources comes from, for questions such as "where do I get an
axe?": one line per object,
       obj@zone<TAB>title<TAB>names<TAB>source; source; ...
where a source is one of
       shop <keeper> <keeper@zone> in <room@zone>        a shopkeeper sells it
       floor <room@zone>                                  it lies there after a reset
       carried by <monster> <monster@zone> in <room@zone> a monster has it (kill or ask)
       in <container> <container@zone> in <room@zone>     inside a chest, a corpse...
       given by <unit> <unit@zone> in <room@zone> (dil)   a script hands it out: a quest
                                                          reward, a statue you pray at...
       given in <room> <room@zone> (dil)                  a script started in that room
       script <dil@zone>                                  a script loads it; who runs it
                                                          is decided by other scripts
An object with no source found (made by macros or by scripts that build the name) has "-".

usage: zoneitems.py ZONEDIR > items.txt
"""
import collections, glob, os, re, sys

from zonemap import uncomment

WORD = re.compile(r"[A-Za-z0-9_]+(?:[@/][A-Za-z0-9_]+)?")
STRING = re.compile(r'"((?:[^"\\]|\\.)*)"')
REF = re.compile(r"^([a-z0-9_]+)([@/])([a-z0-9_]+)$")
DILCOPY = re.compile(r'dilcopy\s*\(?\s*"?([a-z0-9_]+(?:@[a-z0-9_]+)?)', re.I)
DILBEGIN = re.compile(r"^\s*dilbegin\s+(?:aware\s+|recall\s+|integer\s+|string\s+|unitptr\s+|stringlist\s+|intlist\s+)*([a-z0-9_]+)", re.I)
CALL = re.compile(r"\b([a-z0-9_]+)(?:@([a-z0-9_]+))?\s*\(", re.I)
SHOP = re.compile(r"dilcopy\s+shopkeeper\w*@function\s*\(\s*\{([^}]*)\}", re.I | re.S)
UNIT_SECTIONS = ("rooms", "objects", "mobiles")
RESET_WORDS = {"load", "equip", "follow", "door", "remove", "purge", "random", "nop", "complete"}


def norm(ref, zone):
    """sym, sym@zone or zone/sym as sym@zone"""
    ref = ref.lower()
    m = REF.match(ref)
    if m:
        return "%s@%s" % ((m.group(1), m.group(3)) if m.group(2) == "@" else (m.group(3), m.group(1)))
    return "%s@%s" % (ref, zone)


def refs_in(code, zone):
    """sym@zone and zone/sym strings in DIL code"""
    return {norm(s, zone) for s in STRING.findall(code) if REF.match(s.lower().split()[0] if s.strip() else "")}


class Zones:
    def __init__(self):
        self.units = {}                          # sym@zone -> {kind, title, names, dils, code, shop}
        self.dil_code = {}                       # dil@zone -> its code
        self.places = collections.defaultdict(list)  # sym@zone -> [(how, parent sym@zone, room)]

    def read(self, path):
        zone = os.path.basename(path)[:-4]
        section, unit, dil, buf = None, None, None, []
        reset, in_string = [], False
        for line in uncomment(open(path, encoding="latin-1").read()).split("\n"):
            if in_string:  # inside a multi-line "..." (descriptions)
                in_string = line.count('"') % 2 == 0
                if unit and not dil:
                    unit["text"].append(line)
                continue
            m = re.match(r"^%(\w+)", line)
            if m and not dil:
                if m.group(1) == "zone" and len(line.split()) > 1:
                    zone = line.split()[1].lower()
                section = m.group(1)
                continue
            if section == "reset":
                reset.append(line)
                continue
            if dil:  # inside dilbegin ... dilend, in %dil or in a unit
                buf.append(line)
                if re.match(r"^\s*dilend", line):
                    code = "\n".join(buf)
                    self.dil_code[dil] = code
                    if unit:
                        unit["dils"].add(dil)
                    dil, buf = None, []
                continue
            m = DILBEGIN.match(line)
            if m:
                dil, buf = norm(m.group(1), zone), [line]
                continue
            if section not in UNIT_SECTIONS:
                continue
            if unit is None:
                m = re.match(r"^\s*([A-Za-z0-9_]+)\s*$", line)
                if m and m.group(1) != "end":
                    unit = {"id": norm(m.group(1), zone), "kind": section, "title": "", "names": "",
                            "dils": set(), "code": "", "text": []}
                continue
            if re.match(r"^\s*end\s*$", line):
                self.close(unit, zone)
                unit = None
                continue
            unit["text"].append(line)
            in_string = line.count('"') % 2 == 1
        self.resets(" ".join(reset), zone)

    def close(self, unit, zone):
        text = "\n".join(unit.pop("text"))
        m = re.search(r'^\s*title\s+"([^"]*)"', text, re.M)
        unit["title"] = re.sub(r"<[^>]*>|&[a-zA-Z0-9]+;?|\$\d\w", "", m.group(1)).strip() if m else ""
        m = re.search(r"^\s*names\s*\{([^}]*)\}", text, re.M)
        unit["names"] = ", ".join(STRING.findall(m.group(1))) if m else ""
        unit["dils"] |= {norm(d, zone) for d in DILCOPY.findall(text)}
        unit["shop"] = [norm(p.split()[0], zone) for s in SHOP.findall(text)
                        for p in STRING.findall(s) if p.strip()]
        unit["code"] = SHOP.sub("", text)  # strings in its fields, not what it sells
        self.units[unit["id"]] = unit

    def resets(self, text, zone):
        tokens = re.findall(r"[{}]|[A-Za-z0-9_]+(?:[@/][A-Za-z0-9_]+)?", text)
        parents, last, i = [None], None, 0
        while i < len(tokens):
            t = tokens[i]
            if t == "{":
                parents.append(last if last else parents[-1])
                last = None
            elif t == "}":
                if len(parents) > 1:
                    parents.pop()
                last = None
            elif t.lower() in ("load", "equip") and i + 1 < len(tokens):
                sym, room, j = norm(tokens[i + 1], zone), None, i + 2
                while j < len(tokens) and tokens[j] not in "{}" and tokens[j].lower() not in RESET_WORDS:
                    if tokens[j].lower() == "into" and j + 1 < len(tokens):
                        room = norm(tokens[j + 1], zone)
                    j += 1
                parent = parents[-1]
                self.places[sym].append((t.lower(), parent, room))
                last, i = sym, j
                continue
            i += 1

    def rooms_of(self, sym, seen=()):
        """The rooms sym is loaded into by the resets, following monsters and containers"""
        out = []
        for how, parent, room in self.places.get(sym, []):
            if parent and parent not in seen:
                out += self.rooms_of(parent, seen + (sym,))
            elif room:
                out.append(room)
        return sorted(set(out))

    def giver(self, sym):
        """Who or where hands something out: a unit and its rooms, or a room"""
        if self.units[sym]["kind"] == "rooms":
            return "in %s" % self.label(sym)
        return "by %s in %s" % (self.label(sym), ", ".join(self.rooms_of(sym)) or "?")

    def label(self, sym):
        u = self.units.get(sym)
        return "%s %s" % (u["title"] or sym, sym) if u else sym

    def sources(self):
        src = collections.defaultdict(list)
        for sym, places in self.places.items():
            for how, parent, room in places:
                if parent is None and room:
                    src[sym].append("floor %s" % room)
                elif parent:
                    where = ", ".join(self.rooms_of(parent)) or "?"
                    kind = self.units.get(parent, {}).get("kind")
                    verb = "carried by" if kind == "mobiles" or how == "equip" else "in"
                    src[sym].append("%s %s in %s" % (verb, self.label(parent), where))
        started = collections.defaultdict(set)  # dil -> units that dilcopy it or have it inline
        for u in self.units.values():
            for p in u["shop"]:
                src[p].append("shop %s in %s" % (self.label(u["id"]), ", ".join(self.rooms_of(u["id"])) or "?"))
            for d in u["dils"]:
                started[d].add(u["id"])
            for ref in refs_in(u["code"], u["id"].split("@")[1]):
                if ref in self.units and ref != u["id"] and self.units[ref]["kind"] == "objects":
                    src[ref].append("given %s" % self.giver(u["id"]))
        callers = collections.defaultdict(set)  # dil -> the dils that call it
        for dil, code in self.dil_code.items():
            for name, zone in CALL.findall(code):
                called = norm(name + ("@" + zone if zone else ""), dil.split("@")[1])
                if called in self.dil_code and called != dil:
                    callers[called].add(dil)

        def givers(dil):
            """The units that run dil, or a dil that calls it"""
            out, seen, todo = set(), {dil}, [dil]
            while todo:
                d = todo.pop()
                out |= started.get(d, set())
                todo += [c for c in callers[d] if c not in seen]
                seen |= callers[d]
            return out

        for dil, code in self.dil_code.items():
            for ref in refs_in(code, dil.split("@")[1]):
                if ref not in self.units or self.units[ref]["kind"] != "objects":
                    continue
                gs = sorted(g for g in givers(dil) if g in self.units)
                for g in gs:
                    src[ref].append("given %s (%s)" % (self.giver(g), dil))
                if not gs:
                    src[ref].append("script %s" % dil)
        return src


def main():
    z = Zones()
    for path in sorted(glob.glob(os.path.join(sys.argv[1], "*.zon"))):
        z.read(path)
    src = z.sources()
    for sym in sorted(s for s, u in z.units.items() if u["kind"] == "objects"):
        u = z.units[sym]
        found = list(dict.fromkeys(src.get(sym, [])))  # unique, in order
        print("%s\t%s\t%s\t%s" % (sym, u["title"], u["names"], "; ".join(found) or "-"))


if __name__ == "__main__":
    main()
