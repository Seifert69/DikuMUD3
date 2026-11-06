#!/usr/bin/env python3
"""
Reads and edits wiki.dikumud.com, where the game's help texts come from (vme/etc/help/ fetches
the Help/ pages into the .hlp files the mud serves).

usage: wiki.py login                    log in, print the user and what it may do
       wiki.py list [PREFIX]            the page titles (Help/ for the help pages)
       wiki.py get TITLE [FILE]         the page's wikitext (to FILE, or printed) and its revision
       wiki.py edit TITLE FILE REV SUMMARY
                                        replace the page with FILE; REV is the revision `get` printed,
                                        and the edit is refused when the page changed since
       wiki.py create TITLE FILE SUMMARY
                                        a new page; refused when it exists

The login is a bot password (Special:BotPasswords) in ~/.config/valhalla/wiki_bot (mode 600,
outside the repository): user=Name@bot and password=... lines. It is never printed.
"""
import http.cookiejar, json, os, sys, urllib.parse, urllib.request

API = "https://wiki.dikumud.com/api.php"
CONF = os.path.expanduser("~/.config/valhalla/wiki_bot")

opener = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(http.cookiejar.CookieJar()))
opener.addheaders = [("User-Agent", "DikuMUD3 deploy/wiki.py")]


def die(msg):
    print("[FAILED] " + msg)
    sys.exit(1)


def api(post=False, **params):
    params["format"] = "json"
    data = urllib.parse.urlencode(params)
    req = urllib.request.Request(API, data.encode()) if post else urllib.request.Request(API + "?" + data)
    with opener.open(req, timeout=30) as r:
        out = json.load(r)
    if "error" in out:
        die("%s: %s" % (out["error"].get("code"), out["error"].get("info")))
    return out


def login():
    try:
        conf = dict(line.rstrip("\n").split("=", 1) for line in open(CONF) if "=" in line)
    except OSError as e:
        die("no login: %s" % e)
    token = api(action="query", meta="tokens", type="login")["query"]["tokens"]["logintoken"]
    r = api(True, action="login", lgname=conf.get("user", ""), lgpassword=conf.get("password", ""),
            lgtoken=token)["login"]
    if r.get("result") != "Success":
        die("login refused: %s" % (r.get("reason") or r.get("result")))
    return r["lgusername"]


def page(title):
    """(text, revision id) of the page, or (None, None) when it doesn't exist."""
    p = next(iter(api(action="query", prop="revisions", rvprop="ids|content", rvslots="main",
                      titles=title)["query"]["pages"].values()))
    if "missing" in p:
        return None, None
    rev = p["revisions"][0]
    return rev["slots"]["main"]["*"], rev["revid"]


def save(title, path, summary, **how):
    login()
    token = api(action="query", meta="tokens")["query"]["tokens"]["csrftoken"]
    how["assert"] = "user"  # a reserved word in Python, so not a keyword argument
    r = api(True, action="edit", title=title, text=open(path).read(), summary=summary, bot=1,
            token=token, **how)["edit"]
    if r.get("result") != "Success":
        die("not saved: %s" % json.dumps(r))
    print("[saved %s: revision %s]" % (title, r.get("newrevid", "unchanged")))


def main():
    args = sys.argv[1:]
    op = args[0] if args else ""
    if op == "login" and len(args) == 1:
        user = login()
        rights = api(action="query", meta="userinfo", uiprop="rights|groups")["query"]["userinfo"]
        print("[logged in as %s; groups %s]" % (user, ", ".join(rights.get("groups", []))))
        print("may: " + ", ".join(sorted(rights.get("rights", []))))
    elif op == "list" and len(args) <= 2:
        params = {"action": "query", "list": "allpages", "aplimit": "max", "apprefix": args[1] if len(args) > 1 else ""}
        while True:
            r = api(**params)
            for p in r["query"]["allpages"]:
                print(p["title"])
            if "continue" not in r:
                break
            params.update(r["continue"])
    elif op == "get" and len(args) in (2, 3):
        text, rev = page(args[1])
        if text is None:
            die("%s doesn't exist" % args[1])
        if len(args) == 3:
            with open(args[2], "w") as f:
                f.write(text)
        else:
            print(text)
        print("[%s: revision %s]" % (args[1], rev))
    elif op == "edit" and len(args) == 5:
        _, rev = page(args[1])
        if rev is None:
            die("%s doesn't exist: use create" % args[1])
        if str(rev) != args[3]:
            die("%s changed since revision %s (now %s): get it again" % (args[1], args[3], rev))
        save(args[1], args[2], args[4], nocreate=1, baserevid=rev)
    elif op == "create" and len(args) == 4:
        save(args[1], args[2], args[3], createonly=1)
    else:
        print(__doc__.strip())
        sys.exit(2)


if __name__ == "__main__":
    main()
