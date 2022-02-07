#!/bin/bash
#
# stopvme.sh
# Usage:  Stops everything VME related
#
# $VME_ROOT needs to be set to the vme/ folder of your installation
#


#
# Test for the VME_ROOT variable and guess it if it is not available.
#
if [ -z ${VME_ROOT} ]; then
   echo "VME_ROOT not set $VME_ROOT, guessing it is at {$PWD/..}"
   VME_ROOT="$PWD/.."
fi

echo "Scanning for VME scripts and binaries to stop for $USER."

#
# Stop the scripts
#
PIDS="$(pgrep -x runvme.sh -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing runvme.sh"
   kill -9 $PIDS
fi

PIDS="$(pgrep -x runmplex.sh -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing runmplex.sh"
   kill -9 $PIDS
fi

# 15 character limit in pgrep without -f; hence .s
PIDS="$(pgrep -x rundispatcher.s -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing rundispatcher.sh"
   kill -9 $PIDS
fi

# 15 character limit in pgrep without -f; hence .s
PIDS="$(pgrep -x runmuddiscord.s -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing runmuddiscord.sh"
   kill -9 $PIDS
fi

#
# stop the executables
#
PIDS="$(pgrep -x vme -u $USER)"
#echo "vme: $PIDS"
if [ ! -z "${PIDS}" ]; then
   echo "killing vme"
   kill -SIGINT $PIDS
fi


PIDS="$(pgrep -x mplex -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing mplex"
   kill -SIGQUIT $PIDS
fi

PIDS="$(pgrep -xf "python3 -u muddiscord.py" -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing muddiscord.py"
   kill -9 $PIDS
fi

PIDS="$(pgrep -xf "python3 -u dispatcher.py" -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing dispatcher.py"
   kill -9 $PIDS
fi

sleep 2

PIDS="$(pgrep -x vme -u $USER)"
#echo "vme: $PIDS"
if [ ! -z "${PIDS}" ]; then
   echo "this shouldn't happen, killing vme again."
   sleep 5
   echo "sleeping 5 seconds and making sure no VME processes are still running"
   kill -9 $PIDS
fi
echo "Done scanning."