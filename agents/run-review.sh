#!/bin/bash
#
# The daily review of the helper agent Dwili (~/dwili/CLAUDE.md) by his coach (bin/coach.md): a larger
# model reads his day and writes his plan for today, his notes and lessons rewritten short, and
# a report for the people who run him. run-dwili.sh runs it once a day, in the background, while
# Dwili goes on playing; lines he adds to his notes or lessons meanwhile are kept (see below).
#
#   run-review.sh [day]     review that day (default: yesterday)
#
# The coach works in ~/dwili/review/<day>/ and may write nothing else; this script then installs
# its plan.txt, notes.txt and lessons.txt in ~/dwili/memory. The old notes and lessons stay in
# ~/dwili/review/<day>/ as *.orig. Each day is tried once: the directory marks it.
#
cd ~/dwili || exit 1
export PATH=$HOME/.local/bin:$PATH
day=${1:-$(date -d yesterday +%F)}
LOGDIR=$HOME/.local/state/mud/dwili
dir=$HOME/dwili/review/$day
if [ -e "$dir" ]; then
    echo "review of $day: tried already ($dir)"
    exit 0
fi
if [ ! -s "$LOGDIR/session-$day.log" ]; then
    echo "review of $day: no session log"
    exit 0
fi
mkdir -p -m 700 "$dir"

bin/daylog.py "$LOGDIR/session-$day.log" > "$dir/day.txt"
cp "$LOGDIR/claude-$day.log" "$dir/claude.txt" 2>/dev/null
for f in notes lessons; do
    cp -p "memory/$f.txt" "$dir/$f.orig"
    cp "memory/$f.txt" "$dir/$f.txt"
done
[ -e memory/plan.txt ] && cp memory/plan.txt "$dir/plan.txt"

settings='{
  "env": {"CLAUDE_CODE_MAX_OUTPUT_TOKENS": "32000", "MAX_THINKING_TOKENS": "31999"},
  "alwaysThinkingEnabled": true,
  "permissions": {
    "allow": ["Read(~/dwili/review/**)", "Write(~/dwili/review/**)", "Edit(~/dwili/review/**)",
              "Read(~/dwili/memory/**)", "Read(~/zones/**)", "Read(~/dwili/CLAUDE.md)"],
    "deny": ["Bash", "Write(~/dwili/memory/**)", "Edit(~/dwili/memory/**)", "WebFetch", "WebSearch"]
  }
}'
echo "review of $day: coach started $(date '+%F %T')"
timeout 1800 claude -p --model claude-opus-5-5 --effort high --settings "$settings" \
    --append-system-prompt-file bin/coach.md \
    "Review Dwili's day $day. Your files are in ~/dwili/review/$day/. Write plan.txt, notes.txt, lessons.txt and report.md there." \
    > "$dir/coach.log" 2>&1
echo "review of $day: coach exited with code $? at $(date '+%F %T')"

# Install what the coach wrote, unless it's missing or much too short. Lines Dwili added to a
# file during the review (in memory/ but not in the .orig) go at the end of the coach's version.
if [ -s "$dir/plan.txt" ] && grep -q "^GOAL" "$dir/plan.txt"; then
    cp "$dir/plan.txt" memory/plan.txt.new && mv memory/plan.txt.new memory/plan.txt
    echo "installed plan.txt"
fi
for f in notes lessons; do
    if (($(wc -l < "$dir/$f.txt") < 5 && $(wc -l < "$dir/$f.orig") >= 5)); then
        echo "kept memory/$f.txt: the coach's version is too short"
    elif ! cmp -s "$dir/$f.orig" "$dir/$f.txt"; then
        grep -v -x -F -f "$dir/$f.orig" "memory/$f.txt" > "$dir/$f.added"
        cat "$dir/$f.txt" "$dir/$f.added" > "memory/$f.txt.new" && mv "memory/$f.txt.new" "memory/$f.txt"
        echo "installed $f.txt ($(wc -l < "$dir/$f.orig") -> $(wc -l < "memory/$f.txt") lines," \
             "$(wc -l < "$dir/$f.added") added by Dwili during the review)"
    fi
done
