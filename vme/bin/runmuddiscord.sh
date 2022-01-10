#!/bin/bash
#
# No parameters
#
# Don't forget in vme/bin/ to
#    pip3 install virtualenv
#    virtualenv pythonEnv  (or you can use: python3 -m venv ./pythonEnv)
#    source pythonEnv/bin/activate
#    python3 -m pip install -U discord.py
#

EXECUTABLE="muddiscord.py"
FULLCMD="python3 -u $EXECUTABLE"
LOG_FILE="$VME_ROOT/log/muddiscord.log"

trap "" 1 2 3
umask 007

#
# Test is another copy of this script is running for the current user.
#

script_name=$(basename -- "$0")
SCRIPTPIDS="$(pgrep -x $script_name -u $USER | grep -v "$$")"
if [ ! -z "${SCRIPTPIDS}" ]; then
   #echo "1. Running script PIDs = $SCRIPTPIDS which is the child process checking this script"
   MYPID2="$(ps -o pid= --noheaders -p $SCRIPTPIDS)"
   if [ ! -z "${MYPID2}" ]; then
      echo "There's another copy of this script running for PID $MYPID2"
      ps -l -p $MYPID2
      exit 7
   fi
fi

#
# Test for the VME_ROOT variable and guess it if it is not available.
#
if [ -z ${VME_ROOT} ]; then
   echo "VME_ROOT not set $VME_ROOT, guessing it is at {$PWD/..}"
   VME_ROOT="$PWD/.."
fi

#
# Test that there is a vme executable at $VME_ROOT/bin/$EXECUTABLE
#
if [ ! -f ${VME_ROOT}/bin/$EXECUTABLE ]; then
   echo "No $EXECUTABLE executable at $VME_ROOT/bin/$EXECUTABLE"
   exit 1
fi

#
# Test if the executable is running already
#
MYPID="$(pgrep -lxf "$FULLCMD" -u $USER)"
if [ -n "${MYPID}" ]; then
   echo "An instance of $FULLCMD is already running [$MYPID]."
   echo "Please stop it and try again."
   exit 1
fi

#
# I tried to detect if the script was running already but that
# was pretty clonky  with both pgrep and pidof when you both
# need to check for the user (only pgrep) and then can't exclude
# child processes. So executable check only.

#
# Rotate the logs. It's simple. It works
#
cd $VME_ROOT/log
if [ -f muddiscord.log ]; then
   mv -f muddiscord.log.9.gz muddiscord.log.10.gz 2>/dev/null
   mv -f muddiscord.log.8.gz muddiscord.log.9.gz  2>/dev/null
   mv -f muddiscord.log.7.gz muddiscord.log.8.gz  2>/dev/null
   mv -f muddiscord.log.6.gz muddiscord.log.7.gz  2>/dev/null
   mv -f muddiscord.log.5.gz muddiscord.log.6.gz  2>/dev/null
   mv -f muddiscord.log.4.gz muddiscord.log.5.gz  2>/dev/null
   mv -f muddiscord.log.3.gz muddiscord.log.4.gz  2>/dev/null
   mv -f muddiscord.log.2.gz muddiscord.log.3.gz  2>/dev/null
   mv -f muddiscord.log.1.gz muddiscord.log.2.gz  2>/dev/null
   gzip muddiscord.log
   mv -f muddiscord.log.gz muddiscord.log.1.gz
fi

#
# Switch to the default binary directory
#
cd $VME_ROOT/bin
echo "*** $EXECUTABLE started by $USER at `date`. ***" >> $LOG_FILE
echo `uptime` >> $LOG_FILE

source ./pythonEnv/bin/activate
$FULLCMD 2>&1 >> $LOG_FILE
deactivate

res="$?"
echo "*** $EXECUTABLE exited with code [$res] ***" >> $LOG_FILE

script_name=$(basename -- "$0")
echo "Re-running $VME_ROOT/bin/$script_name"
sleep 1
exec $VME_ROOT/bin/$script_name >/dev/null 2>&1
