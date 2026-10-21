#!/usr/bin/env python3
"""
Telnet driver for the valhalla.com deploy: logs a character in and runs steps.

usage: tn.py host port name steps.json     (password from $TN_PW, never on the command line)

Used by deploy/valhalla.sh.

steps: {"send": "goto compiler"}                      send a line
       {"expect": "regex", "timeout": 30}              wait for output (abort on timeout)
       {"poll": "look at screen", "until": "regex",    send every `every` seconds until the
        "fail": "regex", "every": 60, "timeout": 3600}   output matches until (ok) or fail
       {"sleep": 60}
Prints everything the server says (colour codes and telnet negotiation removed).
A player who sends a tell gets one answer per session: Papi is being driven by his AI assistant.
"""
import json, os, re, socket, sys, time

host, port, name, steps_file = sys.argv[1], int(sys.argv[2]), sys.argv[3], sys.argv[4]
pw = os.environ.get("TN_PW", "")
steps = json.load(open(steps_file))

s = socket.create_connection((host, port), timeout=15)
s.settimeout(0.3)
buf = ""
seen = 0
REPLY = "Right now I'm Papi's AI assistant."
answered = set()


def read(t):
    global buf
    end = time.time() + t
    while time.time() < end:
        try:
            d = s.recv(65536)
        except socket.timeout:
            continue
        if not d:
            print("\n[connection closed]")
            return False
        d = re.sub(rb"\xff[\xfb-\xfe].|\xff[^\xff]", b"", d)
        txt = re.sub(r"\x1b\[[0-9;]*[A-Za-z]", "", d.decode("utf-8", "replace"))
        buf += txt
        sys.stdout.write(txt)
        sys.stdout.flush()
        answer_tells(txt)
    return True


def answer_tells(txt):
    for who in re.findall(r"(?m)^\s*([A-Z][a-z]+) tells you ", txt):
        if who != "Someone" and who.lower() != name.lower() and who not in answered:
            answered.add(who)
            s.sendall(("tell " + who.lower() + " " + REPLY + "\r\n").encode())
            print("\n[answered a tell from " + who + "]")


def new_text():
    return buf[seen:]


def wait_for(pat, timeout):
    global seen
    end = time.time() + timeout
    while time.time() < end:
        m = re.search(pat, new_text(), re.I | re.S)
        if m:
            seen += m.end()
            return m
        if not read(0.5):
            return None
    return None


def send(line):
    global seen
    seen = len(buf)
    s.sendall((line + "\r\n").encode())


def read_all_pages(t):
    """Reads for t seconds, answering "*** Return for more ***" so long texts come out whole."""
    read(t)
    for _ in range(200):
        # the marker may be followed by the prompt (e.g. "TEST>")
        if not re.search(r"\*\*\* Return for more \*\*\*\s*([^\n]{0,40}>\s*)?$", buf[-200:]):
            return
        s.sendall(b"\r\n")
        read(1.5)


def die(msg):
    print("\n[FAILED] " + msg)
    sys.exit(1)


# Log in
if not wait_for(r"By what name", 30):
    die("no name prompt")
send(name)
m = wait_for(r"please enter your password|Press \[enter\]|Did I get that right", 30)
if not m:
    die("no password prompt")
if "Did I get" in m.group(0):
    die("the character does not exist here")
if "Press [enter]" in m.group(0):
    send("")
    if not wait_for(r"please enter your password", 120):
        die("no password prompt after the crack notice")
if not pw:
    die("TN_PW is not set")
send(pw)
m = wait_for(r"Wrong password|Throw the other copy out|you last visited", 30)
if not m or "Wrong" in m.group(0):
    die("login refused")
if "Throw" in m.group(0):
    send("y")
read(4)
if re.search(r"Make your choice", new_text()):
    # The login menu: "3) Enter the game." for players, "1) Enter Valhalla" for gods
    m = re.search(r"(\w)\) Enter (the game|Valhalla)", new_text())
    if not m:
        die("no way into the game in the menu")
    send(m.group(1))
    read(4)
    if re.search(r"Invalid Selection|Make your choice", new_text()):
        die("still in the menu")
print("\n[logged in]")

for st in steps:
    if "send" in st:
        send(st["send"])
        read_all_pages(st.get("after", 2))
    elif "expect" in st:
        if not wait_for(st["expect"], st.get("timeout", 30)):
            die("timed out waiting for " + st["expect"])
    elif "sleep" in st:
        read(st["sleep"])
    elif "poll" in st:
        end = time.time() + st.get("timeout", 3600)
        while True:
            send(st["poll"])
            read_all_pages(5)
            if st.get("fail") and re.search(st["fail"], new_text(), re.I):
                die("poll matched failure: " + st["fail"])
            if re.search(st["until"], new_text(), re.I):
                print("\n[done: " + st["until"] + "]")
                break
            if time.time() > end:
                die("poll timed out")
            read(st.get("every", 60) - 5)
print("\n[all steps done]")
s.close()
