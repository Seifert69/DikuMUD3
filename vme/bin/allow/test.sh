#!/usr/bin/bash

# Place any executable file in this directory that you want to allow
# running from DIL. Symbolic links and scripts are your friends here
# E.g. ln -s /usr/bin/gh

sleep 1
echo `/usr/bin/date` >> ./allow/myout.txt
sleep 1