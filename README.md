# DikuMUD3
DikuMUD III using HTML and websockets. 

D-Day June 21, 2020

// How to build & launch

//
// First build the binaries
//
cd vme/src/
make all # add -j8 to compile on 8 threads in parallel

//
// Make sure .def files are pre-processed
//
cd ../etc/
make all


#
# Then compile the zones
#
cd ../zone/
../bin/vmc -m -I../include/ *.zon

#
# Now you're ready to launch, open four tabs in shell
#

cd ../bin/
./vme # tab1
tail -f vme.log # tab2
./mplex -w -p 4280 # tab3
tail -f mplex.log #tab4

#
# You can also launch a telnet mplex using e.g. 'mplex -p 4242'
# And then telnet localhost 4242. But telnet is best for debugging

#
# To open the client
#
cd ../www/
firefox mudenter.html

# host = localhost
# port = 4280
# And you'll see the welcome screen in Firefox
