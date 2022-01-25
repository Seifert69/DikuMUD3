BINDIR = ../../bin
PDIR=../../build

$(BINDIR)/defcomp: $(PDIR)/defcomp.o
	g++ -g -o $@ $(PDIR)/defcomp.o
$(PDIR)/defcomp.o:  defcomp.cpp
	g++ -g -c defcomp.cpp -o $(PDIR)/defcomp.o

