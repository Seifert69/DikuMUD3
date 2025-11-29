#!/usr/bin/env python3
import sys
import json
import urllib.request
import urllib.parse
import re

# ------------------------------------------------------------------
# getwikidilpage.py
# Retrieves a DIL manual page from wiki.dikumud.net and saves it as .hlp
# Automatically tries both "func()" and "func" titles – whichever exists.
# ------------------------------------------------------------------

if len(sys.argv) != 2:
    print("Usage: python3 getwikidilpage.py <function_name>")
    print("Example: python3 getwikidilpage.py asctime")
    sys.exit(1)

func = sys.argv[1]
print(f"Looking up DIL manual page for: {func}")


def fetch_wikitext(title: str) -> str | None:
    """Fetch raw wikitext for an exact page title. Returns text or None if missing."""
    url = (
        "https://wiki.dikumud.net/api.php"
        "?action=query"
        "&prop=revisions"
        "&rvprop=content"
        "&rvslots=main"
        "&format=json"
        "&redirects=1"
        "&titles=" + urllib.parse.quote(title)
    )

    req = urllib.request.Request(url, headers={"User-Agent": "VME-DIL-Help-Updater/2.0"})
    try:
        with urllib.request.urlopen(req) as resp:
            data = json.load(resp)
    except Exception as e:
        print(f"Network error while fetching {title}: {e}")
        return None

    try:
        page = next(iter(data["query"]["pages"].values()))
        if "missing" in page:
            return None
        rev = page["revisions"][0]
        return rev.get("slots", {"main": {"*": rev.get("*")}})["main"]["*"]
    except Exception:
        return None


# ------------------------------------------------------------------
# Try with () first (most DIL pages use it), then without
# ------------------------------------------------------------------
title_with    = f"Manual:DIL_Manual/{func}()"
title_without = f"Manual:DIL_Manual/{func}"

wikitext = fetch_wikitext(title_with)

if wikitext is None:
    print(f"  → Not found with '()', trying without parentheses...")
    wikitext = fetch_wikitext(title_without)
    used_title = title_without
else:
    used_title = title_with

if wikitext is None:
    print(f"Error: Neither '{func}()' nor '{func}' exists on the wiki.")
    sys.exit(1)

print(f"  → Found: {used_title.split('/')[-1]} ({len(wikitext)} bytes)")


# ------------------------------------------------------------------
# Light cleanup – exactly what you want in the .hlp files
# ------------------------------------------------------------------
# Remove {{DIL}} template
wikitext = wikitext.replace("{{DIL}}", "").strip()

# Convert internal DIL links to the {function} format your MUD understands
# Handles: [[Manual:DIL_Manual/foo()]]   [[Manual:DIL_Manual/foo()|foo]]   [[Manual:DIL_Manual/foo]]
wikitext = re.sub(r"\[\[Manual:DIL_Manual/([^|\]]+)(\(\))?(?:\|[^]\]]+)?\]\]", r"{\1}", wikitext)

# Optional: make sure there is exactly one trailing newline
if wikitext and not wikitext.endswith("\n"):
    wikitext += "\n"


# ------------------------------------------------------------------
# Save to file
# ------------------------------------------------------------------
filename = func.lower() + ".hlp"
with open(filename, "w", encoding="utf-8") as f:
    f.write(wikitext)

print(f"Saved → {filename}")
print("Done!")