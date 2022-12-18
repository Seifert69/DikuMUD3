#!/bin/sh

service nginx restart
cd /dikumud3/vme/bin
tail -F vme.log mplex.log &
./vme&
./mplex -w -p 4280
