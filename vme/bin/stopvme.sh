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

#
# Stop the scripts
#
PIDS="$(pgrep -x runvme.sh -u $USER)"
if [ ! -z "${PIDS}" ]; then
   kill -9 $PIDS
fi

PIDS="$(pgrep -x runmplex.sh -u $USER)"
if [ ! -z "${PIDS}" ]; then
   kill -9 $PIDS
fi

PIDS="$(pgrep -x rundispatcher.sh -u $USER)"
if [ ! -z "${PIDS}" ]; then
   kill -9 $PIDS
fi

PIDS="$(pgrep -x runmuddiscord.sh -u $USER)"
if [ ! -z "${PIDS}" ]; then
   kill -9 $PIDS
fi

#
# stop the executables
#
PIDS="$(pgrep -x vme -u $USER)"
#echo "vme: $PIDS"
if [ ! -z "${PIDS}" ]; then
   kill -SIGINT $PIDS
fi


PIDS="$(pgrep -x mplex -u $USER)"
if [ ! -z "${PIDS}" ]; then
   kill -SIGQUIT $PIDS
fi

PIDS="$(pgrep -x "python3 -u muddiscord.py" -u $USER)"
if [ ! -z "${PIDS}" ]; then
   kill -9 $PIDS
fi

PIDS="$(pgrep -x "python3 -u dispatcher.py" -u $USER)"
if [ ! -z "${PIDS}" ]; then
   kill -9 $PIDS
fi
