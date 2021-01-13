#!/bin/bash
#
# No parameters

# Get the general environment variables
#
# $1 should be a TAG uniquely identifiable in ps ax | grep TAG
#
source runenv.sh dispatcher

echo "[" $$ "]" `date` $0 "starting" >> $LOG_FILE
source ./pythonEnv/bin/activate
python -u ./dispatcher.py >> $LOG_FILE
deactivate
sleep 1
exec $1 $SCRIPT

# Don't forget in vme/bin/ to
#
# pip install virtualenv
# virtualenv pythonEnv
# source pythonEnv/bin/activate
# python3 -m pip install -U discord.py
