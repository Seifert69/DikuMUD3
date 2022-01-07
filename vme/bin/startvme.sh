#!/bin/bash
#
# startvme.sh
# Usage:  Starting everything VME related
#
# $VME_ROOT needs to be set to the vme/ folder of your installation
#

#
# Test for the VME_ROOT variable and guess it if it is not available.
#
if [ -z ${VME_ROOT} ]; then
   echo "VME_ROOT not set $VME_ROOT, guessing it is at {$PWD/..}"
   VME_ROOT="$PWD/.."
fi

$VME_ROOT/bin/runvme.sh &
$VME_ROOT/bin/runmplex.sh 4442 &
$VME_ROOT/bin/runmplex.sh 4452 &
$VME_ROOT/bin/runmplex.sh 4480 -w &
$VME_ROOT/bin/rundispatcher.sh & 
$VME_ROOT/bin/runmuddiscord.sh &
