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
       "again": true on a send step: send it again after logging in again (e.g. "goto compiler")
Prints everything the server says (colour codes and telnet negotiation removed).
A player who sends a tell gets one answer per session: Papi is being driven by his AI assistant.

If the connection closes during the steps (the test mud's totest.sh restarts the mplexers at the
end of a compile), it logs in again, sends the "again" steps and goes on with the step it was at.
"""
import json, os, re, socket, sys, time

host, port, name, steps_file = sys.argv[1], int(sys.argv[2]), sys.argv[3], sys.argv[4]
pw = os.environ.get("TN_PW", "")
steps = json.load(open(steps_file))

RELOGIN_TIMEOUT = 600  # seconds to keep trying to log in again after the connection closed
RELOGIN_EVERY = 15

s = None
closed = True
buf = ""
seen = 0
REPLY = "Right now I'm Papi's AI assistant."
answered = set()


class LoginFailed(Exception):
    def __init__(self, msg, retry):
        super().__init__(msg)
        self.retry = retry  # worth trying again later (the server or mplex is not up yet)


def connect():
    global s, closed, buf, seen
    s = socket.create_connection((host, port), timeout=15)
    s.settimeout(0.3)
    closed = False
    buf = ""
    seen = 0


def send_raw(data):
    global closed
    if closed:
        return
    try:
        s.sendall(data)
    except OSError:
        closed = True


def read(t):
    global buf, closed
    end = time.time() + t
    while time.time() < end:
        if closed:
            return False
        try:
            d = s.recv(65536)
        except socket.timeout:
            continue
        except OSError:
            d = b""
        if not d:
            print("\n[connection closed]")
            closed = True
            s.close()
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
            send_raw(("tell " + who.lower() + " " + REPLY + "\r\n").encode())
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
    send_raw((line + "\r\n").encode())


def read_all_pages(t):
    """Reads for t seconds, answering "*** Return for more ***" so long texts come out whole."""
    read(t)
    for _ in range(200):
        # the marker may be followed by the prompt (e.g. "TEST>")
        if closed or not re.search(r"\*\*\* Return for more \*\*\*\s*([^\n]{0,40}>\s*)?$", buf[-200:]):
            return
        send_raw(b"\r\n")
        read(1.5)


def die(msg):
    print("\n[FAILED] " + msg)
    sys.exit(1)


def login():
    """Connects and logs in; raises LoginFailed."""
    try:
        connect()
    except OSError as e:
        raise LoginFailed("cannot connect: " + str(e), True)
    if not wait_for(r"By what name", 30):
        raise LoginFailed("no name prompt", True)
    send(name)
    m = wait_for(r"please enter your password|Press \[enter\]|Did I get that right", 30)
    if not m:
        raise LoginFailed("no password prompt", True)
    if "Did I get" in m.group(0):
        raise LoginFailed("the character does not exist here", False)
    if "Press [enter]" in m.group(0):
        send("")
        if not wait_for(r"please enter your password", 120):
            raise LoginFailed("no password prompt after the crack notice", True)
    if not pw:
        raise LoginFailed("TN_PW is not set", False)
    send(pw)
    m = wait_for(r"Wrong password|Throw the other copy out|you last visited", 30)
    if not m:
        raise LoginFailed("no answer to the password", True)
    if "Wrong" in m.group(0):
        raise LoginFailed("login refused", False)
    if "Throw" in m.group(0):
        # The character is playing already, maybe by its owner: only throw that copy out when told to.
        # Logging in again, the other copy can be our own old connection the server hasn't dropped yet.
        if os.environ.get("TN_THROW") != "1":
            raise LoginFailed("already playing; set TN_THROW=1 to throw the other copy out", relogging)
        send("Yes")  # older servers want the whole word
    read(4)
    if re.search(r"Make your choice", new_text()):
        # The login menu: "3) Enter the game." for players, "1) Enter Valhalla" for gods
        m = re.search(r"(\w)\) Enter (the game|Valhalla)", new_text())
        if not m:
            raise LoginFailed("no way into the game in the menu", False)
        send(m.group(1))
        read(4)
        if re.search(r"Invalid Selection|Make your choice", new_text()):
            raise LoginFailed("still in the menu", False)
    if closed:
        raise LoginFailed("connection closed while logging in", True)
    print("\n[logged in]")


def relogin():
    """Logs in again after the connection closed, and sends the steps marked "again"."""
    end = time.time() + RELOGIN_TIMEOUT
    while True:
        time.sleep(RELOGIN_EVERY)
        print("\n[logging in again]")
        try:
            login()
            break
        except LoginFailed as e:
            print("\n[login failed: " + str(e) + "]")
            if s:
                s.close()
            if not e.retry or time.time() > end:
                die("could not log in again: " + str(e))
    for st in steps:
        if st.get("again"):
            send(st["send"])
            read_all_pages(st.get("after", 2))


relogging = False
try:
    login()
except LoginFailed as e:
    die(str(e))
relogging = True

for st in steps:
    if closed:
        if st.get("send") == "quit":
            break  # gone already
        relogin()
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
            if closed:
                relogin()
                continue
            read(st.get("every", 60) - 5)
            if closed:
                relogin()
print("\n[all steps done]")
if not closed:
    s.close()
