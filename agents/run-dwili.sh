#!/bin/bash
#
# Keeps the helper agent (~/CLAUDE.md) running: Claude Code in print mode, so that when the model
# ends its turn the loop below starts it again with the conversation continued. A conversation
# grows with every turn and each call reads all of it, so a new one starts each day, after
# FRESH seconds, and after QUIET seconds while no players but Papi are online; what the agent
# has learned is in ~/memory.
#
# Run it in tmux as the agent's user:  tmux new -d -s dwili ~/bin/run-dwili.sh
# Stop it:  touch ~/.dwili-stop  (it stops after the current turn), or kill the tmux session.
# Logs:  ~/.local/state/mud/claude-<date>.log (what the model says), session-<date>.log (the mud).
#
# Once a day, before the first new conversation after REVIEW_HOUR, run-review.sh has his coach
# review the day before and write his plan (~/memory/plan.txt).
#
cd ~ || exit 1
export PATH=$HOME/.local/bin:$PATH
export LC_TIME=C  # "Monday", not the account's locale: the date goes into his prompt
LOGDIR=$HOME/.local/state/mud
mkdir -p -m 700 "$LOGDIR"
rm -f ~/.dwili-stop

FIRST="Read memory/plan.txt, guide.txt, lessons.txt, notes.txt and dont-ask-again.txt, then start your loop: wait with bin/mud.py read 300, act on what happens, and read again. Never end your turn on your own."
AGAIN="Go on with your loop: wait with bin/mud.py read 300, act on what happens, and read again. Never end your turn on your own."

FRESH=7200
QUIET=1800
REVIEW_HOUR=6

day=""
began=0
while [ ! -e ~/.dwili-stop ]; do
    today=$(date +%F)
    log=$LOGDIR/claude-$today.log
    if ! bin/mud.py status >/dev/null 2>&1; then
        echo "=== $(date '+%F %T') not logged in: mud.py start" >> "$log"
        bin/mud.py start >> "$log" 2>&1
    fi
    age=$(($(date +%s) - began))
    # "[logged in; other players online: Iolo, Papi]", or "none"
    others=$(bin/mud.py status 2>/dev/null | sed -n 's/^\[logged in; other players online: \(.*\)\]$/\1/p' |
             tr -d ' ' | tr ',' '\n' | grep -v -x -e Papi -e none)
    if [ "$today" != "$day" ] || ((age >= FRESH)) || { [ -z "$others" ] && ((age >= QUIET)); }; then
        if ((10#$(date +%H) >= REVIEW_HOUR)) && [ ! -e ~/review/"$(date -d yesterday +%F)" ]; then
            echo "=== $(date '+%F %T') daily review" >> "$log"
            bin/run-review.sh >> "$log" 2>&1
        fi
        args=(-p "It's $(date '+%A %F, %H:%M'). $FIRST")
        day=$today
        began=$(date +%s)
    else
        args=(-p --continue "It's $(date '+%A %F, %H:%M'). $AGAIN")
    fi
    echo "=== $(date '+%F %T') claude ${args[*]:0:2}" >> "$log"
    started=$(date +%s)
    # A turn can last for hours; cut it off so the next one can start a fresh conversation
    timeout $FRESH claude "${args[@]}" >> "$log" 2>&1
    echo "=== $(date '+%F %T') claude exited with code $?" >> "$log"
    # A turn that ends at once (no login, a usage limit, an error) waits longer before the next
    if (($(date +%s) - started < 60)); then
        sleep 300
    else
        sleep 5
    fi
done
echo "=== $(date '+%F %T') stopped (~/.dwili-stop)" >> "$LOGDIR/claude-$(date +%F).log"
rm -f ~/.dwili-stop
