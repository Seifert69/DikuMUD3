#!/bin/bash
#
# Use for python runs
#
# $1 should be the binary to execute
# $2 should be the parameters to the binary (enclose in quotes)
# $3 should be a TAG uniquely identifiable in ps ax | grep TAG
#
# E.g.
#    $0         $1      $2   $3
# runloop.sh dispatcher.py "" MUDTAG
# runloop.sh muddiscord.py "" MUDTAG
#

# Get the general environment variables
source ./runenv.sh $1

echo "*** [" $$ "]" `date` $BINARY " starting by $USER ***" >> $LOG_FILE
echo `uptime` >> $LOG_FILE

source ./pythonEnv/bin/activate
python3 -u ./$1 $3 2>&1 >> $LOG_FILE
deactivate
echo "*** [" $$ "]" `date` $BINARY " terminated ***" >> $LOG_FILE
sleep 1
exec $BINARY $1 $2 $3

# Don't forget in vme/bin/ to
#
# pip3 install virtualenv
# virtualenv pythonEnv
# source pythonEnv/bin/activate
# python3 -m pip install -U discord.py
