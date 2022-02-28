#!/bin/bash
#
# runmplex.sh
#
# $1 is the port number.
# $2 are any other optional parameters to pass to mplex
#
# $VME_ROOT needs to be set to the vme/ folder of your installation
#

EXECUTABLE="mplex"
PORT=$1

if [ -z ${PORT} ]; then
   echo "Port number not set, defaulting to 4242."
   PORT="4242"
fi

trap "" 1 2 3
umask 007

#
# Test is another copy of this script is running for the current user.
#

#echo "PID = $$"
#script_name=$(basename -- "$0")
script_name="$(ps -o command= -p $$)"
#echo "script name = $script_name"
SCRIPTPIDS="$(pgrep -u $USER --exact --full "$script_name" | grep -v "$$")"
#echo "pgrep -u $USER --exact --full "$script_name""
#echo "scids=$SCRIPTIDS"

if [ ! -z "${SCRIPTPIDS}" ]; then
   #echo "1. Running script PIDs = $SCRIPTPIDS which is the child process checking this script"
   MYPID2="$(ps -o pid= --noheaders -p $SCRIPTPIDS)"
   if [ ! -z "${MYPID2}" ]; then
      echo "There's another copy of this script running for PID $MYPID2" >> $VME_ROOT/log/mplex-$PORT.log
      ps -o pid -o command= -p $MYPID2
      exit 7
   fi
fi

#
# Test for the VME_ROOT variable and guess it if it is not available.
#
if [ -z ${VME_ROOT} ]; then
   echo "VME_ROOT not set $VME_ROOT, guessing it is at {$PWD/..}" >> $PWD/../log/mplex-$PORT.log
   VME_ROOT="$PWD/.."
fi

#
# Test that there is a vme executable at $VME_ROOT/bin/$EXECUTABLE
#
if [ ! -f ${VME_ROOT}/bin/$EXECUTABLE ]; then
   echo "No $EXECUTABLE executable at $VME_ROOT/bin/$EXECUTABLE" >> $VME_ROOT/log/mplex-$PORT.log
   exit 1
fi


#
# Test if the executable is running already
#
MYPID="$(pgrep -ax "$EXECUTABLE" -u $USER) | cut -f 2"
if [ -n "${MYPID}" ]; then
   MYPORT="$(echo $MYPID | grep $PORT)"
   if [ -n "${MYPORT}" ]; then
      echo "An instance of $EXECUTABLE on port $PORT is already running." >> $VME_ROOT/log/mplex-$PORT.log
      echo "Please stop it and try again." >> $VME_ROOT/log/mplex-$PORT.log
      exit 1
   fi
fi

#
# Read the mother port from the server.cfg
#
MOTHER_PORT=$(grep "^Port" $VME_ROOT/etc/server.cfg | grep "=" | gawk '{ print $3 }')
if [ -z ${MOTHER_PORT} ]; then
   MOTHER_PORT="4999"
   echo "MOTHER_PORT not set, setting to default $MOTHER_PORT" >> $VME_ROOT/log/mplex-$PORT.log
fi
#echo "MOTHER_PORT=$MOTHER_PORT"



PARAMS="-p $PORT -s $MOTHER_PORT -l $VME_ROOT/log/mplex-$PORT.log"

if [ ! -z $2 ]; then
    echo "PARAMS = [$PARAMS]; dollar2=[$2]" >> $VME_ROOT/log/mplex-$PORT.log
    PARAMS="$PARAMS $2"
    echo "PARAMS=[$PARAMS]" >> $VME_ROOT/log/mplex-$PORT.log
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
if [ -f mplex-$PORT.log ]; then
   mv -f mplex-$PORT.log.9.gz mplex-$PORT.log.10.gz 2>/dev/null
   mv -f mplex-$PORT.log.8.gz mplex-$PORT.log.9.gz  2>/dev/null
   mv -f mplex-$PORT.log.7.gz mplex-$PORT.log.8.gz  2>/dev/null
   mv -f mplex-$PORT.log.6.gz mplex-$PORT.log.7.gz  2>/dev/null
   mv -f mplex-$PORT.log.5.gz mplex-$PORT.log.6.gz  2>/dev/null
   mv -f mplex-$PORT.log.4.gz mplex-$PORT.log.5.gz  2>/dev/null
   mv -f mplex-$PORT.log.3.gz mplex-$PORT.log.4.gz  2>/dev/null
   mv -f mplex-$PORT.log.2.gz mplex-$PORT.log.3.gz  2>/dev/null
   mv -f mplex-$PORT.log.1.gz mplex-$PORT.log.2.gz  2>/dev/null
   gzip mplex-$PORT.log
   mv -f mplex-$PORT.log.gz mplex-$PORT.log.1.gz
fi

#
# Switch to the default binary directory
#
cd $VME_ROOT/bin
echo "*** $EXECUTABLE started by $USER at `date`. ***" >> $VME_ROOT/log/mplex-$PORT.log
echo `uptime` >> $VME_ROOT/log/mplex-$PORT.log

#echo "running: $VME_ROOT/bin/$EXECUTABLE $PARAMS"
$VME_ROOT/bin/$EXECUTABLE $PARAMS

res="$?"
echo "*** $EXECUTABLE exited with code [$res] ***" >> $VME_ROOT/log/mplex-$PORT.log

script_name=$(basename -- "$0")
#echo "exec $VME_ROOT/bin/$script_name"
exec $VME_ROOT/bin/$script_name $1 $2 >/dev/null 2>&1
