#!/usr/bin/env python3
"""
Jolnir's post, for the Yule Wheel (holiday22): logs in as the player Jolnir (MUD_AGENT=jolnir,
through mud.py), fetches his letters at Udgaard's post office, and adds each letter to
~/lokee/wishes/<player>.txt for Lokee, who reads all of a player's letters and records one gift
with gmyule (Jolnir gives each player one gift a year). The letters are read here and shredded
in the post office, so no one else reads them.

  ~/lokee/wishes/<player>.txt    "FROM: <Player>", then each letter, oldest first:
                                 "DATE: <the letter's date>" and its text

usage: jolnir-mail.py
"""
import os
import re
import subprocess
import sys
import time

WISHES = os.path.expanduser("~/lokee/wishes")
MUD = os.path.expanduser("~/bin/mud.py")
ENV = dict(os.environ, MUD_AGENT="jolnir")
DATE = re.compile(r"[A-Z][a-z]{2} [A-Z][a-z]{2} +\d+ \d\d:\d\d:\d\d \d{4}")  # asctime()


def mud(*args, timeout=60):
    r = subprocess.run([MUD] + list(args), env=ENV, capture_output=True, text=True, timeout=timeout)
    return r.stdout + r.stderr


def walk(to):
    """Walk to room@zone by mud.py's map, from where Jolnir stands (his session is the web
    client's, which shows mud.py the room); False if there's no way."""
    out = mud("way", to, "--steps")
    if "mud.py:" in out:
        print("no way to %s: %s" % (to, out.strip()), file=sys.stderr)
        return False
    for step in out.split():
        if step.endswith("*"):
            mud("send", "open door")
        mud("send", step.rstrip("*!").replace(":", " "))  # enter:barrel is "enter barrel"
    return True


def letter(text):
    """(player, date, wish) from a letter as `look` shows it, or None."""
    m = re.search(r"From: *([A-Z][a-z]+)\s+(" + DATE.pattern + r")\s*(.*)", text, re.S)
    if not m:
        return None
    wish = re.sub(r"\n\S*\d+m/\d+e/\d+h>.*", "", m.group(3), flags=re.S)  # cut at the prompt
    return m.group(1), m.group(2), " ".join(wish.split())


def when(date):
    return time.mktime(time.strptime(" ".join(date.split()), "%a %b %d %H:%M:%S %Y"))


def save(player, date, wish):
    """Add a letter to the player's file, oldest first; returns False if it's there already."""
    path = os.path.join(WISHES, player.lower() + ".txt")
    try:
        with open(path) as f:
            letters = re.findall(r"^DATE: (.+)\n(.*)$", f.read(), re.M)
    except OSError:
        letters = []
    if (date, wish) in letters:
        return False
    letters.append((date, wish))
    letters.sort(key=lambda dw: when(dw[0]))
    with open(path + ".new", "w") as f:
        f.write("FROM: %s\n" % player + "".join("DATE: %s\n%s\n" % dw for dw in letters))
    os.replace(path + ".new", path)
    return True


def unread(text):
    """A letter letter() can't read: kept whole in wishes/unread/, for Papi to look at."""
    d = os.path.join(WISHES, "unread")
    os.makedirs(d, exist_ok=True)
    with open(os.path.join(d, time.strftime("%Y%m%d-%H%M%S-") + str(os.getpid()) + ".txt"), "a") as f:
        f.write(text + "\n")
    print("jolnir: a letter I can't read is in wishes/unread/", file=sys.stderr)


def main():
    os.makedirs(WISHES, exist_ok=True)
    out = mud("start", timeout=90)
    if "mud.py:" in out and "logged in" not in out:
        sys.exit("jolnir: " + out.strip())
    try:
        if not walk("postoffice@udgaard"):
            return
        mud("send", "request")
        # every letter he carries, new ones and any a round before left behind
        n = 0
        got = []
        while n < 500:
            out = mud("send", "look %d.letter" % (n + 1))
            if "From:" not in out:
                break
            n += 1
            w = letter(out)
            if not w:  # kept, so the shredder doesn't lose a wish
                unread(out)
            elif save(*w) and w[0] not in got:
                got.append(w[0])
        for _ in range(n + 5 if n else 0):
            if re.search(r"No such thing|don't|do not|You have no|What|Put 'in'",
                         mud("send", "put letter in shredder")):
                break
        print("jolnir: %d letters, new from %s" % (n, ", ".join(got) or "no one"))
        walk("temple@udgaard")
    finally:
        mud("stop")


if __name__ == "__main__":
    main()
