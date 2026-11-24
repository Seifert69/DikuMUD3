#!/bin/bash
set -x
TIMEOUT=300s
# vme -j <zone>: the zone as JSON lines after the boot, with every program and its arguments.
# It boots the game's own zonelist (the whole public world): zonelist.test ends before haon_dor.
# vme appends to its log: each run starts with none, so that it reads only its own lines
rm -f dump_haon_dor.jsonl vme_dump.log
if ! timeout --signal=KILL ${TIMEOUT} ./vme -l vme_dump.log -j haon_dor; then
  cat vme_dump.log
  exit 1
fi
python3 - <<'EOF' || exit 1
import json
lines = [json.loads(line) for line in open('dump_haon_dor.jsonl')]
assert {'money', 'zone', 'room', 'npc', 'object', 'prototype'} <= {x['kind'] for x in lines}
coins = {c['abbrev']: c['value'] for c in lines[0]['coins']}
assert coins['cp'] == 8 * coins['ip'], coins
zone = next(x for x in lines if x['kind'] == 'zone')
assert zone['unit']['bonusband'] == 4, zone['unit'].get('bonusband')  # haon_dor's treasure band, BONUS_EXCELLENT

def unit_data(x):  # an NPC's unit_data is inside its char_data
    u = x['unit']
    return u['unit_data'] if 'unit_data' in u else u['char_data']['unit_data']

def programs(x):
    return {f['name']: f.get('args') for f in unit_data(x)['func']}

protos = {x['id']: x for x in lines if x['kind'] == 'prototype'}
# killer_rabbit is loaded by no reset, only as a prototype: its dilcopys as the zone file gives them
rabbit = programs(protos['killer_rabbit@haon_dor'])
assert rabbit['wander_zones@function'] == ['@loadzone', 12, 0, 0], rabbit
assert 'comb_poison@function' in rabbit, rabbit
chief = next(x for x in lines if x['kind'] == 'npc' and x['id'] == 'orc_chief@haon_dor')
assert chief['room'] == 'orc_cave_chief@haon_dor', chief['room']
assert programs(chief)['teamwork@function'] == ['orc'], programs(chief)
assert {'id': 'orc_scimitar@haon_dor', 'equipped': 'wielded'} in chief['derived']['contents']
assert chief['derived']['money'] == 8 * coins['cp'], chief['derived']  # 8 copper pieces
scimitar = protos['orc_scimitar@haon_dor']
assert scimitar['derived']['worth'] == 2 * coins['sp'], scimitar['derived']  # cost 2 SILVER_PIECE
potion = protos['blue_potion@haon_dor']['derived']['magic']
assert potion == {'power': 50, 'spells': ['cure light wounds']}, potion
web = next(x for x in lines if x['id'] == 'rn_6130@haon_dor')
up = web['unit']['room_data']['dir_option'][4]
assert (up['to_room'], up['difficulty'], up['exit_info']) == ('rn_6132@haon_dor', 25, ['climb']), up
print('vme -j: %d lines checked' % len(lines))
EOF
# -t: 30 seconds of game first, so the random treasure has taken its shape
if ! timeout --signal=KILL ${TIMEOUT} ./vme -l vme_dump.log -j haon_dor -t 30; then
  cat vme_dump.log
  exit 1
fi
python3 - <<'EOF' || exit 1
import json
lines = [json.loads(line) for line in open('dump_haon_dor.jsonl')]
rnd = [x for x in lines if x['kind'] == 'object' and x['id'].startswith('rnd')]
assert rnd and all(x['unit']['unit_data']['title'] for x in rnd), [x['id'] for x in rnd]
print('vme -j -t: %d pieces of random treasure made' % len(rnd))
EOF
rm -f dump_haon_dor.jsonl vme_dump.log
# The run uses zonelist.test; the game's own zonelist is put back afterwards (unless a killed
# run left zonelist.test in its place, which is then not taken for the game's)
if ! cmp -s ../etc/zonelist ../etc/zonelist.test; then
  cp ../etc/zonelist ../etc/zonelist.saved
  trap 'mv ../etc/zonelist.saved ../etc/zonelist' EXIT
fi
cp ../etc/zonelist.test ../etc/zonelist
rm -f vme.log
# -w: time-warped, the tests' waits pass without waiting for the clock
timeout --signal=KILL ${TIMEOUT} ./vme -w
result=$?
#echo "RESULT:${result}"
if [ ${result} -ne 0 ]; then
  if [ ${result} -eq 137 ]; then
    # see timeout man page for return vals
    echo "TIMEOUT: vme failed to exit after ${TIMEOUT}"
  else
    cat vme.log
  fi
  exit 1
fi
# A clean exit before the tests are done (a caught exception, an early reboot) fails as well
if ! grep -q 'Lab director finished running all tests' vme.log; then
  cat vme.log
  exit 1
fi
exit 0
