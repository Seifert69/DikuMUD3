#!/usr/bin/env python3
"""Who plays on Valhalla, and how, for Lokee's planner. Installed as ~/bin/lokee-players.

  lokee-players sample     reads `users` and `where` output on stdin and adds one line per
                           player to ~/lokee/seen/<date>.txt: time, name, level, room@zone
  lokee-players profile    writes ~/lokee/players.txt from the last 30 days of samples and
                           the plans: when each player comes, for how long, where they play,
                           with whom, and the events they had. Samples older than that go.

The loop (run-lokee.sh) samples every 10 minutes. Only what the game shows a god is kept
(levels, rooms, who is online together), never players' addresses. Times are UTC, the mud's.
"""
import calendar, collections, glob, os, re, statistics, sys, time

HOME = os.path.expanduser("~/lokee")
SEEN = os.path.join(HOME, "seen")
KEEP_DAYS = 30
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
    """The plans a player was in: id and how it ended."""
    out = []
    for path in sorted(glob.glob(os.path.join(HOME, "plans", "*.md"))):
        text = open(path).read()
        head = "\n".join(l for l in text.split("\n") if re.match(r"(FOR|PLAYED|WHEN):", l))
        if re.search(r"\b%s\b" % name, head):
            state = re.search(r"^STATE: *(\w+)", text, re.M)
            out.append("%s (%s)" % (os.path.basename(path)[:-3], state.group(1) if state else "?"))
    return out


def profile():
    rows = load()
    by = collections.defaultdict(list)
    for r in rows:
        by[r[1]].append(r)
    at = collections.defaultdict(dict)  # sample time -> player -> zone
    for t, name, _, room in rows:
        at[t][name] = room.split("@")[-1]
    out = ["Players seen in the last %d days (samples every 10 minutes; times UTC), written %s."
           % (KEEP_DAYS, time.strftime("%Y-%m-%d %H:%M", time.gmtime())),
           "Most seen first. A zone's share is of the times they were seen.", ""]
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
        top = sorted(h for h, _ in hours.most_common(4))
        zones = collections.Counter(r[3].split("@")[-1] for r in rs if r[3] != "?")
        seen = sum(zones.values()) or 1
        mates = collections.Counter()
        for t, *_ in rs:
            mine = at[t].get(name)
            mates.update(o for o, z in at[t].items() if o != name and z == mine)
        days = len({time.strftime("%Y-%m-%d", time.gmtime(t)) for t, *_ in rs})
        first, now = rs[0][2], rs[-1][2]
        out.append("%s, level %d%s; last seen %s in %s" % (
            name, now, " (was %d)" % first if first != now else "",
            time.strftime("%Y-%m-%d %H:%M", time.gmtime(rs[-1][0])), rs[-1][3]))
        out.append("  plays: %d days, %d sessions, usually %d minutes (longest %d); most often at %s"
                   % (days, len(sessions), statistics.median(mins), max(mins),
                      ", ".join("%02d" % h for h in top)))
        out.append("  where: " + ", ".join("%s %d%%" % (z, 100 * n / seen) for z, n in zones.most_common(5)))
        together = [o for o, n in mates.most_common(3) if n >= max(2, len(rs) // 5)]
        if together:
            out.append("  with: " + ", ".join(together))
        ev = events(name)
        if ev:
            out.append("  events: " + ", ".join(ev))
        out.append("")
    with open(os.path.join(HOME, "players.txt.new"), "w") as f:
        f.write("\n".join(out))
    os.replace(os.path.join(HOME, "players.txt.new"), os.path.join(HOME, "players.txt"))


if __name__ == "__main__":
    if sys.argv[1:] == ["sample"]:
        sample(sys.stdin.read())
    elif sys.argv[1:] == ["profile"]:
        profile()
    else:
        sys.exit(__doc__)
