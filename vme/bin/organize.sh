#!/bin/sh
#
# organize.sh
#
umask 007

./cyclic ../log vme.log.gz ../log >/dev/null 2>&1 
#gzip -f core
./cyclic ./  core  ./ >/dev/null 2>&1
