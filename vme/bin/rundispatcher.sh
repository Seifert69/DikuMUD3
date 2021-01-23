#!/bin/bash
#
# No parameters

# Get the general environment variables
#
# $1 should be a TAG uniquely identifiable in ps ax | grep TAG
#
source runenv.sh dispatcher

echo "[" $$ "]" `date` $SCRIPT "starting" >> $LOG_FILE
source ./pythonEnv/bin/activate
python3 -u ./dispatcher.py >> $LOG_FILE
deactivate
echo "[" $$ "]" `date` $SCRIPT "terminated" >> $LOG_FILE
sleep 1
exec $SCRIPT $1

# Don't forget in vme/bin/ to
#
# pip3 install virtualenv
# virtualenv pythonEnv
# source pythonEnv/bin/activate
# python3 -m pip install -U discord.py
