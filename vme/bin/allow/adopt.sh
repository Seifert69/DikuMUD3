#!/bin/bash
#
# Read compile.sh first.
#
# This is step 2. On the prodution server when you 'adopt universe' this
# script is triggered which will eventually run the toprod.sh script
#
#
# Place any executable file in this directory that you want to allow
# running from DIL. Symbolic links and scripts are your friends here
# E.g. ln -s /usr/bin/gh
#

umask 000

if [ -z ${VME_ROOT} ]; then
   echo "VME_ROOT not set [$VME_ROOT]"
   exit 1
fi

if [[ ! -d "$VME_ROOT/bin/" ]]; then
    echo "$VME_ROOT might be set incorrectly, cannot find $VME_ROOT/bin"
    exit
fi

BUILDPATH=$VME_ROOT/../
LONGLOGNAME=$VME_ROOT/bin/allow/adopt.log
LOGFILE=$VME_ROOT/lib/dilfiles/strings/adopt.log

if [[ ! -d "$BUILDPATH" ]]; then
    echo "$BUILDPATH might be set incorrectly, cannot find build directory" 2>&1 >> $LOGFILE
    exit
fi


script_name=$(basename -- "$0")
echo $script_name

if pidof -cx "$script_name" -o $$ >/dev/null;then
   echo "An another instance of $scriptname is already running, aborting." 2>&1 >> $LOGFILE
fi


function myexec {
    eval $@
    if [ $? -ne 0 ]; then
        echo "ERROR running " $@ >> $LOGFILE
    fi
    return
}

rm -f $LOGFILE
rm -f $LONGLOGNAME
echo "*** BEGIN adopt " `date` 2>&1 >> $LONGLOGNAME
echo "*** BEGIN adopt " `date` 2>&1 >> $LOGFILE

myexec $BUILDPATH/toprod.sh 2>&1 >> $LONGLOGNAME

echo "*** END adopt " `date` 2>&1 >> $LONGLOGNAME
echo "*** END adopt " `date` 2>&1 >> $LOGFILE
