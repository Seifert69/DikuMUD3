#!/bin/bash
#
# Groa's expert: a thinking model that answers a question about the game's mechanics from the
# zone sources, the server code and the tables (~/ref), writes the answer into ~/groa/answers/
# so Groa knows it next time, and prints the short answer for her to pass on.
#
#   bin/ask-expert "QUESTION"
#
# At most MAX questions a day. It runs in ~/ref, so Groa's CLAUDE.md isn't loaded; it may read
# ~/ref and ~/groa/knowledge and ~/groa/answers, and write only ~/groa/answers.
#
MAX=20
q="$*"
if [ -z "$q" ]; then
    echo "usage: ask-expert QUESTION"
    exit 2
fi
export PATH=$HOME/.local/bin:$PATH
LOGDIR=$HOME/.local/state/mud/groa
mkdir -p -m 700 "$LOGDIR"
count=$LOGDIR/expert-$(date +%F).count
n=$(cat "$count" 2>/dev/null || echo 0)
if ((n >= MAX)); then
    echo "[No more questions to the expert today ($MAX a day). Tell the player you'll look into it and answer next time, and note the question in memory/questions.txt.]"
    exit 0
fi
echo $((n + 1)) > "$count"

settings='{
  "env": {"CLAUDE_CODE_MAX_OUTPUT_TOKENS": "16000", "MAX_THINKING_TOKENS": "16000"},
  "alwaysThinkingEnabled": true,
  "permissions": {
    "allow": ["Read(~/ref/**)", "Read(~/groa/knowledge/**)", "Read(~/groa/answers/**)",
              "Edit(~/groa/answers/**)"],
    "deny": ["Bash", "WebFetch", "WebSearch", "Read(~/groa/memory/**)"]
  }
}'
log=$LOGDIR/expert-$(date +%F).log
echo "=== $(date '+%F %T') $q" >> "$log"
cd ~/ref || exit 1
# Groa's tool call waits for this; stay under her Bash timeout
answer=$(timeout 840 claude -p --model claude-opus-5-5 --effort high --settings "$settings" \
    --append-system-prompt-file "$HOME/bin/expert.md" \
    "It's $(date '+%A %F'). A player asked Groa: $q" 2>>"$log")
code=$?
echo "$answer" >> "$log"
echo "=== $(date '+%F %T') exit $code" >> "$log"
if [ $code -ne 0 ] || [ -z "$answer" ]; then
    echo "[The expert couldn't answer this time. Tell the player you'll look into it, and note the question in memory/questions.txt.]"
else
    echo "$answer"
fi
