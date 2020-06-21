# Makefile for lib/ files.
#
# To use, write: make

LIBDIR = ../lib/


all:	$(LIBDIR)races.dat $(LIBDIR)spells.dat $(LIBDIR) \
      	$(LIBDIR)weapons.dat $(LIBDIR)abilities.dat $(LIBDIR)color.dat \
		$(LIBDIR)commands.dat $(LIBDIR)professions.dat 

$(LIBDIR)races.dat: races.def
	perl ../bin/defconvert.perl ./races.def $(LIBDIR)races.dat

$(LIBDIR)spells.dat: spells.def
	perl ../bin/defconvert.perl ./spells.def $(LIBDIR)spells.dat

$(LIBDIR)commands.dat: commands.def
	perl ../bin/defconvert.perl ./commands.def $(LIBDIR)commands.dat

$(LIBDIR)weapons.dat: weapons.def
	perl ../bin/defconvert.perl ./weapons.def $(LIBDIR)weapons.dat

$(LIBDIR)abilities.dat: abilities.def
	perl ../bin/defconvert.perl ./abilities.def $(LIBDIR)abilities.dat

$(LIBDIR)professions.dat: professions.def
	perl ../bin/defconvert.perl ./professions.def $(LIBDIR)professions.dat

$(LIBDIR)color.dat: color.def
	../bin/defcomp -c ./color
	mv color.dat $(LIBDIR)

clean:
	-rm -f  $(LIBDIR)spells.dat $(LIBDIR)weapons.dat $(LIBDIR)abilities.dat $(LIBDIR)races.dat \
	        $(LIBDIR)professions.dat $(LIBDIR)color.dat  $(LIBDIR)commands.dat  \
			$(LIBDIR)tmp.cpp $(LIBDIR)tmp.lin $(LIBDIR)swap.blk
