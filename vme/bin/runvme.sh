#!/bin/bash
#
# runmud
# Usage:  Starting and maintaining run of VME Server
#
umask 007

#
# if you intend to run this not from the bin directory uncommet the next line
# and change the path to equal the bin dir
cd /home/val/vme/bin


#NOP=`ps -wx | grep "vme" | grep -v "runmplex" |grep -v "startvme" | grep -v "runvme"| wc | awk '{ print $1;}'`
#STOP=`expr $NOP \> 1`

#if [ $STOP -eq 1 ]
#then

#	echo "VME Server Already up, terminating.\n"#
#	exit 0

#fi

gzip -f ../log/vme.log >/dev/null 2>&1

nice -n 20 ./organize.sh >/dev/null 2>&1

echo "*** VME Server started by $USER at `date`. ***" > ../log/vme.log
echo `uptime` >> ../log/vme.log

rm -f ../lib/ply/player.tmp >/dev/null 2>&1

mv ../zone/nxtzon/* ../zone >/dev/null 2>&1

./vme -s../etc/server.cfg -l../log/vme.log

let "res = 1"
echo "*** vme result was: $res ***" >> ../log/vme.log
if [ $res -eq 0 ]
then
{
   echo "*** VME Server shutdown at `date`." >> ../log/vme.log;
	 exit 0
}
else
{
   echo "*** VME Server about to reboot at `date`." >> ../log/vme.log
}
fi

sleep 60 

exec ./runvme.sh >/dev/null 2>&1
