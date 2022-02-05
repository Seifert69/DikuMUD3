# DikuMUD3
DikuMUD III using HTML and websockets. 

D-Day June 21, 2020

Get in touch if you'd like to contribute. Check out the issues on Git. There are a few broader issues at the bottom of this file. Everything from non-programming to HTML to C(++). Drop a mail to seifert at dikumud.com. Look at the end of this file for a few items.

Join us on Discord:

https://discord.gg/vmha2rGRA2

Documentation goes here:

https://wiki.dikumud.net/wiki/DikuMUD

The Wiki docs are sync'ed into this GitHUB repo daily. Many thanks to @Damien Davison. 

Pre-requisites:
   * gcc/g++
   * flex/bison (sudo apt-get install bison flex)
   * boost devel (sudo apt-get install libboost-all-dev)
   * OpenSSL devel (sudo apt-get install libssl-dev)
   * Rapidjson devel (sudo apt-get install rapidjson-dev)
   * Debian users look here for flex: https://github.com/Seifert69/DikuMUD3/issues?q=70


How to build & launch (using the new cmake)

1) First build the binaries:

        See [CMake](README_cmake.md)* for more details.

       cd DikuMUD3/
       cmake .
       make all -j8 # -j8 to compile on 8 threads in parallel
       make test # optionally run the unit tests

4) Now you're ready to launch, open four tabs in shell:

       cd ../bin/
       ./vme # tab1
       tail -f vme.log # tab2
       ./mplex -w -p 4280 # tab3
       tail -f mplex.log #tab4

You can also launch a telnet mplex using e.g. `mplex -p 4242`
And then `telnet localhost 4242`. You can run several mplex'ers
to the server, some supporting telnet some support web sockets.

5) To open the client

       cd ../www/client/
       firefox index.html

Set the host to your fqdn or localhost and set the port to match mplex (4280 if you used that)
And you'll see the welcome screen in Firefox

---

I would appreciate help with:

   * I would appreciate any world builder wanting to create quests to be completed
     in order to join the various guilds. Right now there are four base teachers 
     and they'll help you out for the first 10-20 levels. So some low level quest
     that e.g. gives access to the thieves guild, the paladin's guild, etc.
     It is OK if one player can complete several quests and join several guilds
     (the profession still dictates what is easy / hard to learn).
     Write me to discuss.

   * DOT files & Mapping:
   
     In the zone directory after compiling the zones, there's a valid DOT file for each zone. 
     The DOT file is the graph description language that describes how the rooms are connected
     in the zone. And also how the zone connects to other zones. 
     There is an example online tool here https://dreampuf.github.io/GraphvizOnline which
     can visualize a zone (just paste each .dot file into the digraph {}.
     Would be fantastic if this could be used to maintain maps of the world, and even more so
     if these maps could be associated with coordinates so they can be visualized in the client.
     It's rather cumbersome to do them by hand - I've done three so far. 
     https://en.wikipedia.org/wiki/DOT_(graph_description_language)

     If anyone is interested in helping out mapping coordinates or generating maps let me know.
     I've been using cMUD to do some test mapping. I think I have two young helpers that can assist 
     me with the actual mapping and turning a boring map into a hand-drawn piece of art :)
     
## Docker build/run/test

Prerequisites:
* Install Docker Desktop
* Increase the memory available to docker if the build cannot complete 
  (docker Preferences->Resources->Advanced->Memory)

### Build the docker image (building the mud from source in the process)
```console
DOCKER_BUILDKIT=1 docker build . -t dikumud3
```

### Run the mud in a new container, binding the port to localhost
```console
docker run -d -p 4280:4280 dikumud3
```
If the above gives an error like below, it may be because your distro sets `DOCKER_CONTENT_TRUST`. This stops docker from executing images that have not been digitally signed by the creator. You can disable the check by setting the env to zero.
```
[user@localhost]$ docker run -d -p 4280:4280 dikumud3
docker: you are not authorized to perform this operation: server returned 401.
See 'docker run --help'.
[user@localhost]$ env | grep DOCKER
32:DOCKER_CONTENT_TRUST=1
[user@localhost]$ DOCKER_CONTENT_TRUST=0 docker run -d -p 4280:4280 dikumud3
```
