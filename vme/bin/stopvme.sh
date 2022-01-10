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

echo "zapping all scripts and binaries running the VME and company."

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

PIDS="$(pgrep -x rundispatcher.sh -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing rundispatcher.sh"
   kill -9 $PIDS
fi

PIDS="$(pgrep -x runmuddiscord.sh -u $USER)"
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

PIDS="$(pgrep -x "python3 -u muddiscord.py" -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing muddiscord.py"
   kill -9 $PIDS
fi

PIDS="$(pgrep -x "python3 -u dispatcher.py" -u $USER)"
if [ ! -z "${PIDS}" ]; then
   echo "killing dispatcher.py"
   kill -9 $PIDS
fi

echo "sleeping 5 seconds and making sure no VME processes are still running"
sleep 5

PIDS="$(pgrep -x vme -u $USER)"
#echo "vme: $PIDS"
if [ ! -z "${PIDS}" ]; then
   echo "this shouldn't happen, killing vme again."
   kill -9 $PIDS
fi
