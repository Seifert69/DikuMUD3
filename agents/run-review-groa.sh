#!/bin/bash
#
# The daily review of Groa (~/groa/CLAUDE.md) by her coach (bin/coach-groa.md): a larger model
# reads her day, every question she sent to the expert, and writes her FAQ (questions the
# expert answered, put so that Haiku can answer them next time), her lessons, and a report for
# the people who run her. run-groa.sh runs it once a day, between conversations.
#
#   run-review-groa.sh [day]     review that day (default: yesterday)
#
# The coach works in ~/groa/review/<day>/ and may write nothing else; this script then installs
# its faq.md in ~/groa/knowledge/ and its lessons.txt in ~/groa/memory/. Each day is tried
# once: the directory marks it.
#
cd ~/groa || exit 1
export PATH=$HOME/.local/bin:$PATH
day=${1:-$(date -d yesterday +%F)}
LOGDIR=$HOME/.local/state/mud/groa
dir=$HOME/groa/review/$day
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
cp "$LOGDIR/expert-$day.log" "$dir/expert.txt" 2>/dev/null || echo "(no expert calls)" > "$dir/expert.txt"
touch knowledge/faq.md memory/lessons.txt 2>/dev/null
cp knowledge/faq.md "$dir/faq.md" 2>/dev/null || : > "$dir/faq.md"
cp memory/lessons.txt "$dir/lessons.txt"
cp "$dir/faq.md" "$dir/faq.orig"
cp -p memory/lessons.txt "$dir/lessons.orig"

settings='{
  "env": {"CLAUDE_CODE_MAX_OUTPUT_TOKENS": "32000", "MAX_THINKING_TOKENS": "31999"},
  "alwaysThinkingEnabled": true,
  "permissions": {
    "allow": ["Read(~/groa/**)", "Edit(~/groa/review/**)", "Read(~/ref/**)"],
    "deny": ["Bash", "WebFetch", "WebSearch", "Edit(~/groa/knowledge/**)", "Edit(~/groa/memory/**)",
             "Edit(~/groa/answers/**)", "Read(~/groa/.claude/**)"]
  }
}'
echo "review of $day: coach started $(date '+%F %T')"
timeout 1800 claude -p --model claude-opus-5-5 --effort high --settings "$settings" \
    --append-system-prompt-file bin/coach-groa.md \
    "Review Groa's day $day. Your files are in ~/groa/review/$day/. Write faq.md, lessons.txt and report.md there." \
    > "$dir/coach.log" 2>&1
echo "review of $day: coach exited with code $? at $(date '+%F %T')"

# Install what the coach wrote, unless it's missing or much too short, or Groa changed her lessons
if [ -s "$dir/faq.md" ] && ! cmp -s "$dir/faq.orig" "$dir/faq.md" &&
   (($(wc -l < "$dir/faq.md") >= $(wc -l < "$dir/faq.orig") / 2)); then
    chmod u+w knowledge knowledge/faq.md
    cp "$dir/faq.md" knowledge/faq.md.new && mv knowledge/faq.md.new knowledge/faq.md
    chmod a-w knowledge knowledge/faq.md
    echo "installed faq.md ($(wc -l < "$dir/faq.orig") -> $(wc -l < knowledge/faq.md) lines)"
fi
if ! cmp -s "$dir/lessons.orig" memory/lessons.txt; then
    echo "kept memory/lessons.txt: it changed during the review"
elif ! cmp -s "$dir/lessons.orig" "$dir/lessons.txt" && [ -s "$dir/lessons.txt" ]; then
    cp "$dir/lessons.txt" memory/lessons.txt.new && mv memory/lessons.txt.new memory/lessons.txt
    echo "installed lessons.txt ($(wc -l < "$dir/lessons.orig") -> $(wc -l < memory/lessons.txt) lines)"
fi
