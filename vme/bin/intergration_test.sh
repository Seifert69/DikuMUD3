#!/bin/bash
set -x
TIMEOUT=300s
cp ../etc/zonelist.test ../etc/zonelist
timeout --signal=KILL ${TIMEOUT} ./vme
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
exit 0
