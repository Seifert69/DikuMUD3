#!/bin/bash
#
# Keeps Lokee, the hidden game master (~/lokee/CLAUDE.md), in the game. Like Groa, no model runs
# while nothing happens: this loop reads the mud itself and wakes Haiku (one turn, a fresh
# conversation) when a player logs on, when one of Lokee's characters reports ("[gm hrafn] ..."),
# when Papi tells Lokee something, or now and then while an event runs. Haiku decides what to
# do; it calls bin/lokee-sonnet to play the characters and bin/lokee-opus to plan.
#
# In December (the Yule Wheel, holiday22) it has the player Jolnir fetch his letters once an hour
# (bin/jolnir-mail.py, into wishes/<player>.txt) and wakes Haiku when a player's wishes are new:
# Lokee reads them and grants one gift with gmyule, then writes wishes/<player>.decided.
#
# Every 10 minutes it notes who is online, their levels and rooms (bin/lokee-players sample,
# into seen/), and once a day, when no event runs, it has Opus plan ahead in the background
# (bin/lokee-ahead): from players.txt (bin/lokee-players profile), events made for the players
# who come often, ready to start when they're online.
#
# Run it in tmux as the agent's user:  tmux new -d -s lokee ~/bin/run-lokee.sh
# Stop it:  touch ~/.lokee-stop  (it stops after the current turn), or kill the tmux session.
# Logs:  ~/.local/state/mud/lokee/claude-<date>.log (what the models say), session-<date>.log
# (the mud).
#
# ~/lokee holds CLAUDE.md (lokee.md), lokee-haiku.md, lokee-sonnet.md, lokee-opus.md, orders.txt
# (Papi's standing orders, if any), plans/, seen/ and players.txt (who plays, and how), forbidden.txt (sym@zone it may not load), memory/,
# .claude/settings.json and bin, a link to ~/bin; ~/.config/valhalla/lokee is its character
# (role=gm, so mud.py keeps it to the game-master commands).
#
cd ~/lokee || exit 1
export MUD_AGENT=lokee
export PATH=$HOME/.local/bin:$PATH
export LC_TIME=C
export TZ=UTC  # the mud's clock: the times in "[gm ...]" lines
LOGDIR=$HOME/.local/state/mud/lokee
mkdir -p -m 700 "$LOGDIR"
rm -f ~/.lokee-stop
[ -d plans/.git ] || git -C plans init -q
git -C plans add -A >/dev/null 2>&1 && git -C plans commit -qm "$(date '+%F %T') start" >/dev/null 2>&1

# What wakes it: a player logging on (not the helpers or Papi), its characters' reports, Papi's
# tells, a lost connection
# (mud.py prints "[Asta logged on]" for a game master; the game doesn't tell an invisible god)
WAKE='^\[[A-Z][a-z]+ logged on\]|^\[gm |^\[tell Papi\]|the mud closed the connection|logging in again failed'
# (and a lure playing its steps: it needs nothing from Lokee)
QUIET='^\[(Dwili|Groa|Papi|Lokee|Jolnir) logged on\]|^\[gm [A-Za-z]+\] [0-9:]+ lure step '
TURN=3600     # the longest one turn may take
CHECK=900     # while an event runs, look in on it this often (seconds)
SAMPLE=600    # note who is online this often (seconds)
AHEAD_AT=10   # the hour (UTC) from which the day's planning ahead may start
last=$(date +%s)
sampled=0

while [ ! -e ~/.lokee-stop ]; do
    log=$LOGDIR/claude-$(date +%F).log
    if ! bin/mud.py status >/dev/null 2>&1; then
        echo "=== $(date '+%F %T') not logged in: mud.py start" >> "$log"
        bin/mud.py start >> "$log" 2>&1 || { sleep 300; continue; }
        bin/mud.py send "wizinv 230" >> "$log" 2>&1
    fi
    text=$(bin/mud.py read 300 2>&1)
    if [ "$(date -u +%m)" = 12 ] && (($(date +%s) - mailed >= 3600)); then
        mailed=$(date +%s)
        bin/jolnir-mail.py >> "$log" 2>&1
    fi
    # the wishes not granted yet: no .decided, or a letter came after it
    wishes=$(for f in wishes/*.txt; do
                 [ -e "$f" ] || continue
                 [ "$f" -nt "${f%.txt}.decided" ] && echo "$f"
             done)
    running=$(grep -l '^STATE: \(running\|approved\)' plans/*.md 2>/dev/null)
    ahead=""
    if ((10#$(date +%H) >= AHEAD_AT)) && [ -z "$running" ] && [ ! -e "$LOGDIR/ahead-$(date +%F)" ]; then
        ahead=1
    fi
    if [ -n "$ahead" ] || (($(date +%s) - sampled >= SAMPLE)); then
        sampled=$(date +%s)
        # (a send also returns what came in since the read, so it goes on with the text)
        seen=$({ bin/mud.py send "users" | sed -E 's/^(<[^>]*>.*[^ ]) *\[[^]]*\]$/\1/'; bin/mud.py send "where"; } 2>&1)
        bin/lokee-players sample <<< "$seen"
        text="$text
$seen"
    fi
    if [ -n "$ahead" ]; then
        touch "$LOGDIR/ahead-$(date +%F)"
        bin/lokee-players profile
        { date '+%F %H:%M'; echo "$seen"; } > state.txt
        echo "=== $(date '+%F %T') planning ahead (bin/lokee-ahead, in the background)" >> "$log"
        LOKEE_STATE=keep nohup bin/lokee-ahead "Plan ahead, as lokee-opus.md says under \"Planning ahead\"." >/dev/null 2>&1 &
    fi
    if grep -E "$WAKE" <<< "$text" | grep -qvE "$QUIET"; then
        why="This just happened:"
    elif [ -n "$wishes" ] && [ "$wishes" != "$told_wishes" ]; then
        why="New Yule wishes have come in (wishes/): grant them. Lately:"
    elif [ -n "$running" ] && (($(date +%s) - last >= CHECK)); then
        why="Nothing called you, but an event is running ($running): look in on it. Lately:"
    else
        continue
    fi
    last=$(date +%s)
    told_wishes=$wishes
    echo "=== $(date '+%F %T') woken" >> "$log"
    orders=$(cat orders.txt 2>/dev/null)
    [ -n "$orders" ] && orders=" Papi's standing orders (orders.txt): $orders
"
    plans=$(grep -m1 -H '^STATE:' plans/*.md 2>/dev/null)
    ready=$(for f in $(grep -l '^STATE: ready' plans/*.md 2>/dev/null); do
                echo "$f: $(grep -m1 -h '^WHEN:' "$f") $(grep -m1 -h '^FITS:' "$f")"
            done)
    paused=$(for f in $(grep -l '^STATE: paused' plans/*.md 2>/dev/null); do
                echo "$f: $(grep -m1 -h '^FOR:' "$f")"
            done)
    reuse=$(for f in $(grep -l '^REUSE: yes' plans/*.md 2>/dev/null); do
                echo "$f: $(grep -m1 -h '^FITS:' "$f") $(grep -m1 -h '^PLAYED:' "$f")"
            done)
    orders="$orders
Plans (file: first line): ${plans:-none}
Plans ready for players who aren't all online yet: ${ready:-none}
Paused plans, waiting for one of their players to log on: ${paused:-none}
Yule wishes not granted yet: ${wishes:-none}
Plans to reuse: ${reuse:-none}
"
    bin/claude-turn $TURN -p --append-system-prompt "$(cat lokee-haiku.md)" "It's $(date '+%A %F, %H:%M') UTC.$orders
$why

$text

Deal with it as lokee-haiku.md says, and end your turn." >> "$log" 2>&1
    echo "=== $(date '+%F %T') claude exited with code $?" >> "$log"
    # every change to a plan is kept, whatever a turn did to the files
    git -C plans add -A >/dev/null 2>&1 &&
        git -C plans commit -qm "$(date '+%F %T')" >/dev/null 2>&1
done
echo "=== $(date '+%F %T') stopped (~/.lokee-stop)" >> "$LOGDIR/claude-$(date +%F).log"
rm -f ~/.lokee-stop
