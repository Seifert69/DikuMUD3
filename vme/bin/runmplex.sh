#!/bin/sh
#
# runmplex.sh
# Usage:  Starting and maintaining run of Valhalla
#
# $1 is argument to port number...
# $2 is the (optional) log-extension
# $3 are any optional parameters to pass to mplex
#
trap "" 1 2 3
umask 007

LOG_DIR=../log/
CYCLIC=./cyclic
SCRIPT=./runmplex.sh
MOTHERPORT=4999

gzip $LOG_DIR/mplex$2.log >/dev/null 2>&1
$CYCLIC $LOG_DIR mplex$2.log.gz $LOG_DIR
./mplex -p $1 -a localhost -s $MOTHERPORT $3 -l $LOG_DIR/mplex$2.log

sleep 1
exec $SCRIPT $1 $2 $3
