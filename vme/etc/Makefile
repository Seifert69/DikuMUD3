# Makefile for converting def to dat files.
#
# To use, write: make all

# default is the etc/ directory
OUTDIR = ../etc/


all:	$(OUTDIR)races.dat $(OUTDIR)spells.dat $(OUTDIR) \
      	$(OUTDIR)weapons.dat $(OUTDIR)abilities.dat $(OUTDIR)color.dat \
		$(OUTDIR)commands.dat $(OUTDIR)professions.dat 

$(OUTDIR)races.dat: races.def
	perl ../bin/defconvert.perl ./races.def $(OUTDIR)races.dat

$(OUTDIR)spells.dat: spells.def
	perl ../bin/defconvert.perl ./spells.def $(OUTDIR)spells.dat

$(OUTDIR)commands.dat: commands.def
	perl ../bin/defconvert.perl ./commands.def $(OUTDIR)commands.dat

$(OUTDIR)weapons.dat: weapons.def
	perl ../bin/defconvert.perl ./weapons.def $(OUTDIR)weapons.dat

$(OUTDIR)abilities.dat: abilities.def
	perl ../bin/defconvert.perl ./abilities.def $(OUTDIR)abilities.dat

$(OUTDIR)professions.dat: professions.def
	perl ../bin/defconvert.perl ./professions.def $(OUTDIR)professions.dat

$(OUTDIR)color.dat: color.def
	../bin/defcomp -c ./color

clean:
	-rm -f  $(OUTDIR)spells.dat $(OUTDIR)weapons.dat $(OUTDIR)abilities.dat $(OUTDIR)races.dat \
	        $(OUTDIR)professions.dat $(OUTDIR)color.dat  $(OUTDIR)commands.dat  \
			$(OUTDIR)tmp.cpp $(OUTDIR)tmp.lin $(OUTDIR)swap.blk
