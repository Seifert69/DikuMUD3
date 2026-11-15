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
seen = ""


def read_until(pat, timeout=30):
    """Read until the text since the last call matches pat; returns that text."""
    global seen
    buf = ""
    end = time.time() + timeout
    while time.time() < end:
        try:
            d = s.recv(65536)
            if not d:
                break
            txt = re.sub(rb"\xff[\xfb-\xfe].|\xff[\xf0-\xfa]", b"", d).decode("latin1")
            buf += re.sub(r"\x1b\[[0-9;]*m", "", txt)
            if re.search(pat, buf):
                break
        except socket.timeout:
            pass
    seen += buf
    print(buf, end="")
    return buf


def read_quiet(quiet=2, timeout=30):
    """Read until the game has said nothing for `quiet` seconds."""
    global seen
    buf = ""
    end = time.time() + timeout
    last = time.time()
    while time.time() < end and time.time() - last < quiet:
        try:
            d = s.recv(65536)
            if not d:
                break
            buf += re.sub(r"\x1b\[[0-9;]*m", "", re.sub(rb"\xff[\xfb-\xfe].|\xff[\xf0-\xfa]", b"", d).decode("latin1"))
            last = time.time()
        except socket.timeout:
            pass
    seen += buf
    print(buf, end="")


def send(line):
    s.sendall((line + "\r\n").encode())


def answer(pat, line, timeout=30):
    if not re.search(pat, read_until(pat, timeout)):
        print("\npc_test: no '%s' from the game" % pat)
        sys.exit(1)
    send(line)


answer(r"By what name", name)
t = read_until(r"(right|password)", 30)
if re.search(r"right", t):  # a new character
    send("y")
    answer(r"new password", pw)
    answer(r"retype", pw)
    answer(r"gender", "1")
    answer(r"profession", "1")
    answer(r"race", "1")
    answer(r"personality", "2")
else:
    send(pw)
t = read_until(r"Make your choice|Reconnecting", 30)
if "Make your choice" in t:
    send("3" if "Enter the game" in t else "1")
elif "Reconnecting" not in t:  # the character was still in the game: no menu
    print("\npc_test: no menu")
    sys.exit(1)
read_quiet()  # a new character's first entry runs an introduction that takes no commands
send("goto lab@diltest")
read_until(r"DIL Test Laboratory", 30)
send("diltest")
t = read_until(r"diltest: [^\n]*(passed|failed checks)[^\n]*", 120)
m = re.search(r"diltest: [^\n]*", t)
send("quit")
read_until(r"Goodbye|$^", 5)
if not m:
    print("\npc_test: the tests didn't finish")
    sys.exit(1)
sys.exit(0 if "all player tests passed" in m.group(0) else 1)
