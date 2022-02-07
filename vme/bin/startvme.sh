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

#
# Start the VME
#

echo "Starting vme: $VME_ROOT/bin/runvme.sh &"
$VME_ROOT/bin/runvme.sh &

#
# Start the mplex'ers defined in server.cfg
#
mplexers=$(grep "^mplexer" $VME_ROOT/etc/server.cfg | cut -d "=" -f 2)

ORGIFS=$IFS
IFS=$'\n'       # make newlines the only separator
for m in $mplexers
do
   IFS=$ORGIFS  # need to revert or parameters will be passed incorrectly
   echo "Starting mplex: $VME_ROOT/bin/runmplex.sh $m &"
   $VME_ROOT/bin/runmplex.sh $m &
   IFS=$'\n'       # make newlines the only separator
done
IFS=$ORGIFS

#
# Launch the Dispatcher. Right now only used for Discord...
# but who knows...
#
echo "Starting rundispatcher.sh: $VME_ROOT/bin/rundispatcher.sh &"
$VME_ROOT/bin/rundispatcher.sh & 

#
# Launch the Discord integration if the token is present
#

if [ ! -f ${VME_ROOT}/bin/discord.token ]; then
   echo "Skipping discord integration because no token file present at $VME_ROOT/bin/discord.token"
else
   echo "Starting runmuddiscord.sh: $VME_ROOT/bin/runmuddiscord.sh &"
   $VME_ROOT/bin/runmuddiscord.sh &
fi

echo "Done startvme.sh"