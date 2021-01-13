#!/bin/bash
#
# No parameters

# Get the general environment variables
#
# $1 TAG uniquely identifiable in ps ax | grep TAG
#
source runenv.sh muddiscord

echo "[" $$ "]" `date` $0 "starting" >> $LOG_FILE
source ./pythonEnv/bin/activate
python3 -u ./muddiscord.py >> $LOG_FILE
deactivate
sleep 1
exec $1 $SCRIPT

# Don't forget in vme/bin/ to
#
# pip3 install virtualenv
# virtualenv pythonEnv
# source pythonEnv/bin/activate
# python3 -m pip install -U discord.py
