#!/bin/bash
#
# No parameters

# Get the general environment variables
#
#
exec ./runloop.sh dispatcher.py "" MUDTAG
exec $0

# Don't forget in vme/bin/ to
#
# pip3 install virtualenv
# virtualenv pythonEnv
# source pythonEnv/bin/activate
# python3 -m pip install -U discord.py
