#!/bin/bash
#
# Keeps Groa, the Oracle of Midgaard (~/groa/CLAUDE.md), in the game. Unlike Dwili she only
# answers, so no model runs while nothing happens: this loop reads the mud itself and starts
# Claude Code (one turn, a fresh conversation) when a player speaks to her or she is hungry or
# thirsty, with what happened in the prompt. She reads on while the conversation lasts and
# ends her turn when it goes quiet.
#
# Run it in tmux as the agent's user:  tmux new -d -s groa ~/bin/run-groa.sh
# Stop it:  touch ~/.groa-stop  (it stops after the current turn), or kill the tmux session.
# Logs:  ~/.local/state/mud/groa/claude-<date>.log (what the model says), session-<date>.log
# (the mud), expert-<date>.log (her expert, bin/ask-expert).
#
# ~/groa holds her CLAUDE.md, knowledge/ (mechanics.md, teachers.txt), answers/ (written by the
# expert), memory/, .claude/settings.json (her model and permissions) and bin, a link to
# ~/bin; ~/.config/valhalla/groa is her character.
#
cd ~/groa || exit 1
export MUD_AGENT=groa
export PATH=$HOME/.local/bin:$PATH
export LC_TIME=C
LOGDIR=$HOME/.local/state/mud/groa
mkdir -p -m 700 "$LOGDIR"
rm -f ~/.groa-stop

# What wakes her: a player talking (mud.py marks it), hunger and thirst, a lost connection
WAKE='^\[(tell|say|ask|whisper|emote) [A-Z][a-z]+\]|You are (very )?(hungry|thirsty)|the mud closed the connection|logging in again failed'
TURN=3600  # the longest one turn may take

while [ ! -e ~/.groa-stop ]; do
    log=$LOGDIR/claude-$(date +%F).log
    if ! bin/mud.py status >/dev/null 2>&1; then
        echo "=== $(date '+%F %T') not logged in: mud.py start" >> "$log"
        bin/mud.py start >> "$log" 2>&1 || { sleep 300; continue; }
    fi
    text=$(bin/mud.py read 300 2>&1)
    if ! grep -qE "$WAKE" <<< "$text"; then
        continue
    fi
    echo "=== $(date '+%F %T') woken" >> "$log"
    timeout $TURN claude -p "It's $(date '+%A %F, %H:%M'). Read knowledge/mechanics.md, then deal with this, which just happened where you are:

$text

Then read on with bin/mud.py read 120 while the conversation lasts, and end your turn when it has been quiet for about 5 minutes." >> "$log" 2>&1
    echo "=== $(date '+%F %T') claude exited with code $?" >> "$log"
done
echo "=== $(date '+%F %T') stopped (~/.groa-stop)" >> "$LOGDIR/claude-$(date +%F).log"
rm -f ~/.groa-stop
