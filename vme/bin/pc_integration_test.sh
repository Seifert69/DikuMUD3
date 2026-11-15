#!/bin/bash
# The DIL tests that need a player (vme/zone/diltest.zon). Boots the game with the zones compiled
# here, starts a telnet mplex, logs in the immortal_name god (created on a fresh lib/), goes to
# lab@diltest and types diltest. Exit code 0 when every player test passed.
#
# The god's password is $PCTEST_PASSWORD (default pctest12): on a lib/ where that character exists
# with another password, set it or the login fails. The mplex listens on $PCTEST_PORT (default 4243).
cd "$(dirname "$0")"
PORT=${PCTEST_PORT:-4243}
PW=${PCTEST_PASSWORD:-pctest12}
MOTHER=$(awk -F= '/^ *[Pp]ort *=/{gsub(/ /,"",$2); print $2; exit}' ../etc/server.cfg)
NAME=$(awk -F= '/^ *immortal_name *=/{gsub(/[ ~]/,"",$2); print $2; exit}' ../etc/server.cfg)

# The game's own zonelist, without the test zone, and only the zones compiled here (a public clone
# has no private zones), with diltest.
cp ../etc/zonelist ../etc/zonelist.pctest-saved
trap 'mv ../etc/zonelist.pctest-saved ../etc/zonelist; kill $MPLEX $VME 2>/dev/null' EXIT
awk '/^#/ {print; next} NF {if ($1 != "test" && system("test -f ../zone/" $1 ".data") == 0) print}' \
    ../etc/zonelist.pctest-saved > ../etc/zonelist
grep -q '^diltest ' ../etc/zonelist || echo 'diltest 1120tw0008       1 0 255 0' >> ../etc/zonelist

# A server stopped a moment ago may still hold the mother port
timeout 60 bash -c "while (echo > /dev/tcp/127.0.0.1/$MOTHER) 2>/dev/null; do sleep 1; done"
rm -f vme.log
./vme > /dev/null 2>&1 &
VME=$!
timeout 120 bash -c 'until grep -q "Entering game loop" vme.log 2>/dev/null; do kill -0 '$VME' || exit 1; sleep 0.5; done' ||
    { echo "vme didn't boot"; tail -20 vme.log; exit 1; }
./mplex -p "$PORT" -s "$MOTHER" -l pctest-mplex.log > /dev/null 2>&1 &
MPLEX=$!
timeout 30 bash -c "until (echo > /dev/tcp/127.0.0.1/$PORT) 2>/dev/null; do sleep 0.5; done" ||
    { echo "mplex didn't start"; exit 1; }

timeout 300 python3 -u pc_test.py "$PORT" "$NAME" "$PW"
result=$?
grep -E 'diltest/P[0-9]+//ERROR|diltest//' vme.log
exit $result
