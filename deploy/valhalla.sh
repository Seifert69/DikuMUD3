#!/bin/bash
#
# Deploys to valhalla.com. Run from the repository root:
#
#   deploy/valhalla.sh client <label>   web client files to /www/valhalla/v3 (old files kept as *.bak-<label>)
#   deploy/valhalla.sh test [resume]    test mud: compile master, reboot, check it runs
#                                       (resume: the button was pressed already, just watch)
#   deploy/valhalla.sh prod [resume]    production: adopt the test build, reboot when no mortals are on, check
#
# If Papi is logged in already they stop, unless TN_THROW=1 says to throw that copy out.
# test and prod log in as Papi over telnet. The password is read from ~/.config/valhalla/papi_pw
# (mode 600, outside the repository), or from $PAPI_PW when that is set.
# SSH needs the valhalla.com entry in ~/.ssh/config (user seifert).
#
#   test mud    /home/mud/vme   "Valhalla Test Mud"  telnet 4442  (has the build tree, compile.sh)
#   production  /home/val/vme   "Valhalla"           telnet 4242, websocket 4280  (adopt.sh -> toprod.sh)
#
# In both muds Papi's cottage (goto compiler) holds the compiler (press button / look screen:
# compile.log), the parallel universe (adopt universe / look symbols: adopt.log) and the
# reboot daemon (ruffle daemon: reboots as soon as no mortals are online, checked every 5 minutes).
#
set -euo pipefail

HOST=valhalla.com
SSH="ssh -o BatchMode=yes seifert@$HOST"
WEBDIR=/www/valhalla/v3
CLIENT=vme/www/client
HERE=$(cd "$(dirname "$0")" && pwd)
TN="python3 -u $HERE/tn.py"

say() { echo; echo "=== $* ($(date +%H:%M:%S))"; }

PW_FILE=~/.config/valhalla/papi_pw
if [[ -z "${PAPI_PW:-}" && -s $PW_FILE ]]; then
    PAPI_PW=$(head -n1 "$PW_FILE")
fi

steps() { # writes a steps file for tn.py and prints its name
    local f
    f=$(mktemp --suffix=.json)
    echo "$1" > "$f"
    echo "$f"
}

# PID of the vme run by a unix user (mud = test, val = production)
vme_pid() { $SSH "pgrep -u $1 -x vme" 2>/dev/null | head -1; }

# Waits until the vme of user $1 is a new process (old PID $2) and port $3 answers a login.
wait_reboot() {
    local user=$1 old=$2 port=$3 max=$4 pid
    local end=$((SECONDS + max))
    while ((SECONDS < end)); do
        pid=$(vme_pid "$user" || true)
        if [[ -n "$pid" && "$pid" != "$old" ]]; then
            echo "vme restarted: PID $old -> $pid"
            sleep 20 # zones load before the mplexers connect
            return 0
        fi
        sleep 30
    done
    echo "vme of $user did not restart within $max s"
    return 1
}

check_login() { # port, extra commands
    local f
    f=$(steps "[{\"send\": \"look\"}, $2, {\"send\": \"quit\"}]")
    TN_PW=$PAPI_PW $TN $HOST "$1" papi "$f"
    rm -f "$f"
}

client() {
    local label=${1:?give a backup label, e.g. pre-whofix}
    local files=() f live mine
    while read -r f; do
        live=$(curl -fsS "https://www.$HOST/v3/$f" 2>/dev/null | md5sum | cut -c1-32 || true)
        mine=$(md5sum < "$CLIENT/$f" | cut -c1-32)
        [[ "$live" != "$mine" ]] && files+=("$f")
    done < <(cd "$CLIENT" && find index.html core ui css -type f -name '*.*' ! -name '*.md' | sort)
    if ((${#files[@]} == 0)); then
        say "client: the live files are already up to date"
        return
    fi
    say "client: ${files[*]}"
    # index.html last: a browser that gets the new page must find the new scripts
    local order=() x
    for x in "${files[@]}"; do [[ $x != index.html ]] && order+=("$x"); done
    for x in "${files[@]}"; do [[ $x == index.html ]] && order+=("$x"); done
    for x in "${order[@]}"; do scp -q -o BatchMode=yes "$CLIENT/$x" "seifert@$HOST:$WEBDIR/$x.new"; done
    $SSH "cd $WEBDIR && for f in ${order[*]}; do [ -f \$f ] && cp -p \$f \$f.bak-$label; mv \$f.new \$f; done"
    for x in "${order[@]}"; do
        curl -fsS "https://www.$HOST/v3/$x" | cmp -s - "$CLIENT/$x" || { echo "$x: served file differs"; exit 1; }
    done
    echo "served files match"
}

# "look screen", not "look at screen": the compiler only answers to exactly that
test_mud() {
    : "${PAPI_PW:?put the Papi password in $PW_FILE}"
    local old f press='{"send": "press button", "after": 5},'
    [[ "${1:-}" == resume ]] && press=""
    old=$(vme_pid mud)
    say "test: compile master (test vme PID $old)"
    f=$(steps '[{"send": "goto compiler", "again": true}, '"$press"'
        {"poll": "look screen", "until": "\\*\\*\\* END compile", "fail": "ERROR running", "every": 60, "timeout": 5400},
        {"send": "ruffle daemon", "after": 5}, {"send": "quit"}]')
    TN_PW=$PAPI_PW $TN $HOST 4442 papi "$f"
    rm -f "$f"
    say "test: waiting for the reboot"
    wait_reboot mud "$old" 4442 1800
    say "test: login and wstat world"
    check_login 4442 '{"send": "wstat world", "after": 5}'
}

prod() {
    : "${PAPI_PW:?put the Papi password in $PW_FILE}"
    local old f adopt='{"send": "adopt universe", "after": 5},'
    [[ "${1:-}" == resume ]] && adopt=""
    old=$(vme_pid val)
    say "prod: adopt the universe (production vme PID $old)"
    f=$(steps '[{"send": "goto compiler", "again": true}, '"$adopt"'
        {"poll": "look symbols", "until": "\\*\\*\\* END adopt", "fail": "ERROR running", "every": 60, "timeout": 3600},
        {"send": "ruffle daemon", "after": 5}, {"send": "quit"}]')
    TN_PW=$PAPI_PW $TN $HOST 4242 papi "$f"
    rm -f "$f"
    say "prod: waiting for the reboot (when no mortals are online; may take hours)"
    wait_reboot val "$old" 4242 $((24 * 3600))
    say "prod: login"
    check_login 4242 '{"send": "wstat world", "after": 5}'
}

case "${1:-}" in
client) client "${2:-}" ;;
test) test_mud "${2:-}" ;;
prod) prod "${2:-}" ;;
*) sed -n '2,20p' "$0"; exit 2 ;;
esac
