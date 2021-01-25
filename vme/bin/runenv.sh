#!/bin/bash
#
# Set common MUD runtime environment variables
# Called from the other "runxxx.sh" scripts
#

# $1 core name of the log file

trap "" 1 2 3
umask 007

BIN_DIR=./
LOG_DIR=../log/
ETC_DIR=../etc/
CYCLIC=./cyclic
BINARY=$0
MOTHERPORT=4999
TIMENOW=`date +%Y%m%d-%H%M%S`
LOG_FILE=$LOG_DIR/$1-$TIMENOW.log
