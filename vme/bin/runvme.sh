#!/bin/bash
#
# runvme.sh
# Usage:  Starting and maintaining run of VME Server
#
# $VME_ROOT needs to be set to the vme/ folder of your installation
#
EXECUTABLE="vme"

umask 007

#
# Test is another copy of this script is running for the current user.
#

#echo "script_name = $script_name"
#echo "dollar 0 = $0"
#echo "user = $USER"
#echo "PID = $$"
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
MYPID="$(pgrep -lx $EXECUTABLE -u $USER)"
if [ -n "${MYPID}" ]; then
   echo "An instance of $EXECUTABLE is already running [$MYPID]."
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
if [ -f vme.log ]; then
   mv -f vme.log.9.gz vme.log.10.gz 2>/dev/null
   mv -f vme.log.8.gz vme.log.9.gz  2>/dev/null
   mv -f vme.log.7.gz vme.log.8.gz  2>/dev/null
   mv -f vme.log.6.gz vme.log.7.gz  2>/dev/null
   mv -f vme.log.5.gz vme.log.6.gz  2>/dev/null
   mv -f vme.log.4.gz vme.log.5.gz  2>/dev/null
   mv -f vme.log.3.gz vme.log.4.gz  2>/dev/null
   mv -f vme.log.2.gz vme.log.3.gz  2>/dev/null
   mv -f vme.log.1.gz vme.log.2.gz  2>/dev/null
   gzip vme.log
   mv -f vme.log.gz vme.log.1.gz
fi

#
# Ought to rotate cores here too,
#


#
# Switch to the default binary directory
#
cd $VME_ROOT/bin
echo "*** VME Server started by $USER at `date`. ***" >> $VME_ROOT/log/vme.log
echo `uptime` >> $VME_ROOT/log/vme.log

rm -f $VME_ROOT/lib/ply/player.tmp >/dev/null 2>&1
mv $VME_ROOT/zone/nxtzon/* ../zone >/dev/null 2>&1

$VME_ROOT/bin/vme -s$VME_ROOT/etc/server.cfg -l$VME_ROOT/log/vme.log

res="$?"
echo "*** vme exited with code [$res] ***" >> $VME_ROOT/log/vme.log

script_name=$(basename -- "$0")
#echo "exec $VME_ROOT/bin/$script_name"
sleep 1
exec $VME_ROOT/bin/$script_name >/dev/null 2>&1
