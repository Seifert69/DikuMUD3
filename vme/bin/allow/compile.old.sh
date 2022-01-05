#!/bin/bash

# Place any executable file in this directory that you want to allow
# running from DIL. Symbolic links and scripts are your friends here
# E.g. ln -s /usr/bin/gh

umask 000

HOMEPATH=/home/mud
BUILDPATH=/home/mud/build
VMEPATH=$HOMEPATH/vme2.0/
LONGLOGNAME=$VMEPATH/bin/allow/compile.log
LOGFILE=$VMEPATH/lib/dilfiles/strings/compile.log

function myexec {
    eval $@
    if [ $? -ne 0 ]; then
        echo "ERROR running " $@ >> $LOGFILE
    fi
    return
}

rm -f $LOGFILE
rm -f $LONGLOGNAME
echo `/bin/date` 2>&1 >> $LONGLOGNAME
echo `/bin/date` 2>&1 >> $LOGFILE
myexec cd $HOMEPATH/build/
myexec $HOMEPATH/build/buildit.sh 2>&1 >> $LONGLOGNAME
myexec $HOMEPATH/build/totest.sh 2>&1 >> $LONGLOGNAME
cd $VMEPATH/bin/allow/

echo "*** DONE ***" `date` 2>&1 >> $LONGLOGNAME
echo "*** DONE ***" `date` 2>&1 >> $LOGFILE
