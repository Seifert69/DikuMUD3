#!/bin/bash
#
# Runs one claude turn for an agent, like `timeout <seconds> claude <args>`, and also ends it
# when it hangs: when for $CLAUDE_TURN_IDLE seconds (300) none of its commands runs (a Bash tool
# call: a mud.py read, a lokee-sonnet call) and its conversation (~/.claude/projects/<cwd>/)
# doesn't grow. A turn that sleeps inside itself can't hear the mud: on 2026-11-04 Lokee's
# watcher waited for its own wakeup for half an hour while a tell from Papi went unread.
#
# Only the tools an agent uses exist for it (--tools), so it has no way to wait but its own
# commands.
#
# Installed as ~/bin/claude-turn:  bin/claude-turn <seconds> -p "<prompt>" ...
#
max=$1
shift
IDLE=${CLAUDE_TURN_IDLE:-300}
TOOLS=${CLAUDE_TURN_TOOLS:-Bash,Read,Edit,Write,Grep,Glob}
dir=$HOME/.claude/projects/$(pwd | tr '/.' '--')

claude --tools "$TOOLS" "$@" &
pid=$!
start=$(date +%s)
last=$start
while kill -0 $pid 2>/dev/null; do
    sleep 10
    now=$(date +%s)
    if pgrep -P $pid -f shell-snapshots >/dev/null; then  # one of its commands is running
        last=$now
    fi
    grew=$(stat -c %Y "$dir"/*.jsonl 2>/dev/null | sort -n | tail -1)
    ((${grew:-0} > last)) && last=$grew
    if ((now - start >= max)); then
        echo "=== $(date '+%F %T') claude-turn: over ${max}s, stopped"
        kill $pid
        break
    elif ((now - last >= IDLE)); then
        echo "=== $(date '+%F %T') claude-turn: idle for ${IDLE}s (no command running, nothing said), stopped"
        kill $pid
        break
    fi
done
wait $pid
