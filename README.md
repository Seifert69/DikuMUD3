# DikuMUD3
DikuMUD III using HTML and websockets. 

D-Day June 21, 2020

Get in touch if you'd like to contribute. There's a long to-do list. Everything from non-programming to HTML to C(++). Drop a mail to seifert at dikumud.com. Look at the end of this file for a few items.

Documentation goes here:

https://wiki.dikumud.net/wiki/DikuMUD

Will get a copy sync'ed to GitHUB soon, many thanks to @ Damien Davison for help with that. 

Pre-requisites:
   * gcc/g++
   * flex/bison (sudo apt-get install bison flex)
   * boost devel (sudo apt-get install libboost-all-dev)



How to build & launch

1) First build the binaries:

       cd vme/src/
       make all # add -j8 to compile on 8 threads in parallel

2) Make sure `.def` files are pre-processed:

       cd ../etc/
       make all

3) Then compile the zones:

       cd ../zone/
       ../bin/vmc -m -I../include/ *.zon

4) Now you're ready to launch, open four tabs in shell:

       cd ../bin/
       ./vme # tab1
       tail -f vme.log # tab2
       ./mplex -w -p 4280 # tab3
       tail -f mplex.log #tab4

You can also launch a telnet mplex using e.g. `mplex -p 4242`
And then `telnet localhost 4242`. But telnet is best for debugging

5) To open the client

       cd ../www/
       firefox index.html

Set the host to your fqdn or localhost and set the port to match mplex (4280 if you used that)
And you'll see the welcome screen in Firefox

---

I would appreciate help with:

   * Mapping: Anyone interested in helping out mapping coordinates or generating maps. 
     I've been using cMUD to do some test mapping. I think I have two young helpers that can assist 
     me with the actual mapping and turning a boring map into a hand-drawn piece of art :)
     
     However, I also noticed for example this project. Maybe that'd better than cMUD for mapping?
     
     https://github.com/demonnic/mudlet-map-reader

   * Anyone wanting to improve the client. Ability to choose multiple CSS files for color.
     That annoying Firefox security bar popping up over the text input could probably be 
     circumvented by asking for the password in a pop-over rather than changing the input field type.
     Ways to improve the accessible client further. 
     
