#!/bin/bash
#
# In the basis zone there is a room with a compiler and a universe.
# When you press the button on the compiler, this script runs.
# I use it to pull all the latest from Git, kick off a full make,
# and when successfully done, it copies all the files needed over to 
# the TEST mud server. Which I then reboot to ensure everything is up
# and running. 
#
# See adopt.sh for step two.
#
# Place any executable file in this directory that you want to allow
# running from DIL. Symbolic links and scripts are your friends here
# E.g. ln -s /usr/bin/gh
#
# 
#

umask 000

if [ -z ${VME_ROOT} ]; then
   echo "VME_ROOT not set [$VME_ROOT]. Must be set to the vme/ directory of the install."
   exit 1
fi

if [[ ! -d "$VME_ROOT/bin/" ]]; then
    echo "$VME_ROOT might be set incorrectly, cannot find $VME_ROOT/bin"
    exit
fi

#
# Directory locations
#
cd $VME_ROOT/bin
BUILDPATH=$VME_ROOT/../build/
LONGLOGNAME=$VME_ROOT/bin/allow/compile.log
LOGFILE=$VME_ROOT/lib/dilfiles/strings/compile.log

if [[ ! -d "$BUILDPATH" ]]; then
    echo "$BUILDPATH might be set incorrectly, cannot find build directory"
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
echo "*** BEGIN compile" `date` 2>&1 >> $LONGLOGNAME
echo "*** BEGIN compile" `date` 2>&1 >> $LOGFILE

myexec cd $BUILDPATH
myexec $BUILDPATH/buildit.sh 2>&1 >> $LONGLOGNAME
myexec $BUILDPATH/totest.sh 2>&1 >> $LONGLOGNAME
cd $VME_ROOT/bin/allow/

echo "*** END compile " `date` 2>&1 >> $LONGLOGNAME
echo "*** END compile " `date` 2>&1 >> $LOGFILE
