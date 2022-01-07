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
MYPID="$(pgrep -lxf "$VME_ROOT/bin/$EXECUTABLE $PARAMS" -u $USER)"
if [ -n "${MYPID}" ]; then
   echo "An instance of $EXECUTABLE $PARAMS is already running [$MYPID]."
   echo "Please stop it and try again."
   exit 1
fi

#
# Read the mother port from the server.cfg
#
MOTHER_PORT=$(grep "^Port" $VME_ROOT/etc/server.cfg | grep "=" | gawk '{ print $3 }')
if [ -z ${MOTHER_PORT} ]; then
   MOTHER_PORT="4999"
   echo "MOTHER_PORT not set, setting to default $MOTHER_PORT"
fi
#echo "MOTHER_PORT=$MOTHER_PORT"

PARAMS="-p $PORT $2 -s $MOTHER_PORT -l $VME_ROOT/log/mplex-$PORT.log"

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

echo "running: $VME_ROOT/bin/$EXECUTABLE $PARAMS"
$VME_ROOT/bin/$EXECUTABLE $PARAMS

res="$?"
echo "*** $EXECUTABLE exited with code [$res] ***" >> $VME_ROOT/log/mplex-$PORT.log

script_name=$(basename -- "$0")
#echo "exec $VME_ROOT/bin/$script_name"
exec $VME_ROOT/bin/$script_name >/dev/null 2>&1
