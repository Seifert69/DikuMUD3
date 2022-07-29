# DikuMUD3
DikuMUD III using HTML, websockets and live Discord integrated.

D-Day June 21, 2020

Look at the end of this file if you want to contribute.

Join us on Discord (live integrated):

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

Optional:
 * Doxygen (sudo apt-get install doxygen graphviz dia mscgen)
   
How to build & launch (using the new cmake)

1) First build the binaries:

        See [CMake](README_cmake.md)* for more details.

       cd DikuMUD3/
       cmake .
       make all -j8 # -j8 to compile on 8 threads in parallel
       make test # optionally run the unit tests

*Code documentation will be generated in docs/ after the build completes. [docs/index.html](docs/index.html)*

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

Get in touch if you'd like to contribute. Drop a mail to seifert at dikumud.com. 
Contribution can be anything from world building, to creating tables in excel,
to coding.

   * World builders are most welcome.
   * Some issues on Git to look at.
   * Need help creating random treasures. Got something working but need to polish it. 
     Also missing e.g. random descriptions and a way to generate random descriptions.

---
     
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
