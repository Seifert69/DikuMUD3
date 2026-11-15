#!/usr/bin/env python3
"""
Runs the DIL player tests (vme/zone/diltest.zon) through a telnet mplex: logs in the
immortal_name god (creating the character on a fresh server), goes to lab@diltest, types
diltest and waits for the result.

usage: pc_test.py PORT NAME PASSWORD
Exit code 0 when every player test passed, 1 otherwise. Prints what the game said.
"""
import re, socket, sys, time

port, name, pw = int(sys.argv[1]), sys.argv[2], sys.argv[3]
s = socket.create_connection(("127.0.0.1", port), timeout=10)
s.settimeout(0.2)
rest = ""  # what came after the last match, read again by the next call


def read(pat=None, quiet=None, timeout=30):
    """Print what the game says until it matches pat (ignoring case), or has been quiet for
    `quiet` seconds, or `timeout` passes. Returns the match (None without one)."""
    global rest
    buf, rest = rest, ""
    if pat and (m := re.search(pat, buf, re.I)):
        rest = buf[m.end():]
        return m
    end = time.time() + timeout
    last = time.time()
    while time.time() < end and not (quiet and time.time() - last >= quiet):
        try:
            d = s.recv(65536)
        except socket.timeout:
            continue
        if not d:
            break
        txt = re.sub(rb"\xff[\xfb-\xfe].|\xff[\xf0-\xfa]", b"", d).decode("latin1")
        txt = re.sub(r"\x1b\[[0-9;]*[A-Za-z]", "", txt)
        print(txt, end="")
        buf += txt
        last = time.time()
        if "*** Return for more ***" in txt:
            send("")
        if pat and (m := re.search(pat, buf, re.I)):
            rest = buf[m.end():]
            return m
    return None


def send(line):
    s.sendall((line + "\r\n").encode())


def expect(pat, timeout=30):
    m = read(pat, timeout=timeout)
    if not m:
        print("\npc_test: no '%s' from the game" % pat)
        sys.exit(1)
    return m


expect(r"By what name")
send(name)
if expect(r"right|password").group(0).lower() == "right":  # a new character
    send("y")
    for pat, line in ((r"new password", pw), (r"retype", pw), (r"gender", "1"),
                      (r"profession", "1"), (r"race", "1"), (r"personality", "2")):
        expect(pat)
        send(line)
else:
    send(pw)
m = expect(r"(\w)\) Enter (the game|Valhalla)|Reconnecting")
if m.group(1):  # the menu; a character still in the game is reconnected without one
    send(m.group(1))
read(quiet=2)  # a new character's first entry runs an introduction that takes no commands
send("goto lab@diltest")
expect(r"DIL Test Laboratory")
send("diltest")
m = read(r"diltest: [^\n]*(passed|failed checks)[^\n]*", timeout=120)
send("quit")
read(r"Goodbye", timeout=5)
if not m:
    print("\npc_test: the tests didn't finish")
    sys.exit(1)
sys.exit(0 if "all player tests passed" in m.group(0) else 1)
