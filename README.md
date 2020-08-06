# DikuMUD3
DikuMUD III using HTML and websockets. 

D-Day June 21, 2020

Get in touch if you'd like to contribute. There's a long to-do list. Everything from non-programming to HTML to C(++). Drop a mail to seifert at dikumud.com. Look at the end of this file for a few items.

Documentation goes here:

https://wiki.dikumud.net/wiki/DikuMUD

The Wiki docs are sync'ed into this GitHUB repo daily. Many thanks to @Damien Davison. 

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

     If Anyone interested in helping out mapping coordinates or generating maps let me know.
     I've been using cMUD to do some test mapping. I think I have two young helpers that can assist 
     me with the actual mapping and turning a boring map into a hand-drawn piece of art :)
     
   * Anyone wanting to improve the client. Ability to choose multiple CSS files for color.
     That annoying Firefox security bar popping up over the text input could probably be 
     circumvented by asking for the password in a pop-over rather than changing the input field type.
     Ways to improve the accessible client further. 
     
