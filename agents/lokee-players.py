#!/usr/bin/env python3
"""Who plays on Valhalla, and how, for Lokee's planner. Installed as ~/bin/lokee-players.

  lokee-players sample     reads `users` and `where` output on stdin and adds one line per
                           player to ~/lokee/seen/<date>.txt: time, name, level, room@zone
  lokee-players profile    writes ~/lokee/players.txt, a rough sketch of the last 14 days, one
                           line per player: level, days and sessions, hours, zones, with whom,
                           and their events. Samples older than that go.

The loop (run-lokee.sh) samples every 10 minutes. Only what the game shows a god is kept
(levels, rooms, who is online together), never players' addresses. Times are UTC, the mud's.
"""
import calendar, collections, glob, os, re, statistics, sys, time

HOME = os.path.expanduser("~/lokee")
SEEN = os.path.join(HOME, "seen")
KEEP_DAYS = 14
SESSION_GAP = 25 * 60  # seconds without a sample that end a session (samples are 10 minutes apart)
HELPERS = {"Dwili", "Groa", "Lokee", "Papi"}


def sample(text):
    levels = {m.group(2): int(m.group(1))
              for m in re.finditer(r"^<\s*(\d+) >\s+([A-Z][a-z]+)\s", text, re.M)}
    rooms = {m.group(1): m.group(2)
             for m in re.finditer(r"^([A-Z][a-z]+)\s+- .*\[([a-z0-9_]+@[a-z0-9_]+)\]\s*$", text, re.M)}
    now = time.gmtime()
    os.makedirs(SEEN, mode=0o700, exist_ok=True)
    with open(os.path.join(SEEN, time.strftime("%Y-%m-%d.txt", now)), "a") as f:
        for name, level in sorted(levels.items()):
            if name not in HELPERS and level > 0:  # level 0: a connection still in the menu
                f.write("%s %s %d %s\n" % (time.strftime("%Y-%m-%dT%H:%M", now), name, level,
                                           rooms.get(name, "?")))


def load():
    cut = time.time() - KEEP_DAYS * 86400
    rows = []
    for path in sorted(glob.glob(os.path.join(SEEN, "*.txt"))):
        day = os.path.basename(path)[:10]
        if calendar.timegm(time.strptime(day, "%Y-%m-%d")) < cut - 86400:
            os.remove(path)
            continue
        for line in open(path):
            p = line.split()
            if len(p) == 4:
                t = calendar.timegm(time.strptime(p[0], "%Y-%m-%dT%H:%M"))
                rows.append((t, p[1], int(p[2]), p[3]))
    return rows


def events(name):
    """The plans a player was in: how many, and the last one with how it ended."""
    out = []
    for path in sorted(glob.glob(os.path.join(HOME, "plans", "*.md"))):
        text = open(path).read()
        head = "\n".join(l for l in text.split("\n") if re.match(r"(FOR|PLAYED|WHEN):", l))
        if re.search(r"\b%s\b" % name, head):
            state = re.search(r"^STATE: *(\w+)", text, re.M)
            out.append("%s (%s)" % (os.path.basename(path)[:-3], state.group(1) if state else "?"))
    return "%d, last %s" % (len(out), out[-1]) if out else ""


def profile():
    rows = load()
    by = collections.defaultdict(list)
    for r in rows:
        by[r[1]].append(r)
    at = collections.defaultdict(dict)  # sample time -> player -> zone
    for t, name, _, room in rows:
        at[t][name] = room.split("@")[-1]
    out = ["Who played in the last %d days, one line each, most seen first (samples every 10 minutes;"
           " times UTC; written %s)." % (KEEP_DAYS, time.strftime("%Y-%m-%d %H:%M", time.gmtime())), ""]
    for name, rs in sorted(by.items(), key=lambda kv: -len(kv[1])):
        rs.sort()
        sessions, start, last = [], rs[0][0], rs[0][0]
        for t, *_ in rs[1:]:
            if t - last > SESSION_GAP:
                sessions.append((start, last))
                start = t
            last = t
        sessions.append((start, last))
        mins = [max(10, (b - a) / 60 + 10) for a, b in sessions]
        hours = collections.Counter(time.gmtime(t).tm_hour for t, *_ in rs)
        zones = collections.Counter(r[3].split("@")[-1] for r in rs if r[3] != "?")
        mates = collections.Counter()
        for t, *_ in rs:
            mine = at[t].get(name)
            mates.update(o for o, z in at[t].items() if o != name and z == mine)
        days = len({time.strftime("%Y-%m-%d", time.gmtime(t)) for t, *_ in rs})
        first, now = rs[0][2], rs[-1][2]
        line = "%s %d%s: %d day%s, %d session%s of about %d minutes, at %s; %s" % (
            name, now, " (was %d)" % first if first != now else "",
            days, "s"[days == 1:], len(sessions), "s"[len(sessions) == 1:], statistics.median(mins),
            ", ".join("%02d" % h for h in sorted(h for h, _ in hours.most_common(3))),
            ", ".join(z for z, _ in zones.most_common(3)))
        together = [o for o, n in mates.most_common(3) if n >= max(2, len(rs) // 5)]
        if together:
            line += "; with " + ", ".join(together)
        line += "; last seen %s" % time.strftime("%m-%d", time.gmtime(rs[-1][0]))
        ev = events(name)
        if ev:
            line += "; events %s" % ev
        out.append(line)
    # Written by hand when sampling began; kept until the samples cover the whole window
    earlier = os.path.join(HOME, "players-earlier.txt")
    if os.path.exists(earlier) and time.time() - os.path.getmtime(earlier) < KEEP_DAYS * 86400:
        out += ["", open(earlier).read().rstrip()]
    with open(os.path.join(HOME, "players.txt.new"), "w") as f:
        f.write("\n".join(out) + "\n")
    os.replace(os.path.join(HOME, "players.txt.new"), os.path.join(HOME, "players.txt"))


if __name__ == "__main__":
    if sys.argv[1:] == ["sample"]:
        sample(sys.stdin.read())
    elif sys.argv[1:] == ["profile"]:
        profile()
    else:
        sys.exit(__doc__)
