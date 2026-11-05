#!/bin/bash
#
# Lokee's stronger minds, installed as ~/bin/lokee-call with two links to it:
#   bin/lokee-sonnet "<situation>"   the actor: plays the characters, handles surprises
#   bin/lokee-plan "<task>"          the quick planner (Sonnet): a small plans/<id>.md
#   bin/lokee-opus "<task>"          the thorough planner: bigger events Papi asks for
#   bin/lokee-ahead "<task>"         Opus planning ahead, once a day from the loop (run-lokee.sh)
# Each runs one turn of Claude Code in ~/lokee (CLAUDE.md is lokee.md) with its own part,
# lokee-sonnet.md or lokee-opus.md (both planners), and prints its last message for the caller.
#
cd ~/lokee || exit 1
export MUD_AGENT=lokee
export PATH=$HOME/.local/bin:$PATH
export LC_TIME=C
LOGDIR=$HOME/.local/state/mud/lokee
log=$LOGDIR/claude-$(date +%F).log

case "$(basename "$0")" in
    lokee-sonnet) role=sonnet; part=sonnet; model=claude-sonnet-5-5; turn=1800 ;;
    lokee-plan) role=plan; part=opus; model=claude-sonnet-5-5; turn=600 ;;
    lokee-opus) role=opus; part=opus; model=claude-opus-5-5; turn=1200 ;;
    lokee-ahead) role=ahead; part=opus; model=claude-opus-5-5; turn=3600 ;;
    *) echo "call me as lokee-sonnet, lokee-plan, lokee-opus or lokee-ahead" >&2; exit 2 ;;
esac
[ -n "$*" ] || { echo "usage: $(basename "$0") \"<situation or task>\"" >&2; exit 2; }

if [ $part = opus ] && [ "${LOKEE_STATE:-}" != keep ]; then
    # who is online, their levels and where they are, for the planner
    # (users shows levels, and players' addresses, which the planner has no use for;
    # LOKEE_STATE=keep: the loop wrote state.txt already, and is reading the mud itself)
    { date '+%F %H:%M'; bin/mud.py send "users" 2>&1 | sed 's/ *\[[^]]*\]$//'
      bin/mud.py send "where" 2>&1; } > state.txt
fi

echo "=== $(date '+%F %T') $role: $*" >> "$log"
# (a planner may think for minutes before it says anything)
[ $part = opus ] && export CLAUDE_TURN_IDLE=900
out=$(bin/claude-turn $turn -p --model $model --append-system-prompt "$(cat lokee-$part.md)" \
      "It's $(date '+%A %F, %H:%M'). $*" 2>&1)
code=$?
echo "$out" >> "$log"
echo "=== $(date '+%F %T') $role exited with code $code" >> "$log"
echo "$out"
