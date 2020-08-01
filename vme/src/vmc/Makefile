# The makefile this makefile includes
MAKEFILE =	Makefile.ubuntu
BINDIR = ../../bin/

# You may have to change the use of include, ifneq, and endif to fit your make.
#
# Oct 22, 1993 [gnort] : Did a major overhaul of Makefile.
# Jun 25, 1994 [gnort] : And another one.
# Feb 03, 1995 [gnort] : Split the makefile into architecture dependant parts.
#
# After adding/changing #include's in sourcefiles or adding new sourcefiles:
# > make depend
#
# If make complains about a file `.depends' not present:
# > touch .depends ; make depend
#
# NOTE!!  Using lex instead of flex may cause severe grief, if you forget
# to change the declaration in vmcpar.y from *yytext to yytext[] as well!!

#
#
#  Set the flag "-DMUD_DEBUG" to get full memory debug, type checks,
#

SYSFLAGS	= -DMUD_DEBUG -g
#-O9
#-DMEMORY_DEBUG #-DMEMCHECK -DPROFILE -pg

include $(MAKEFILE)

CFLAGS		= -I. -I.. -I vmc -DVMC $(DEBUG) $(SYSPORT) $(SYSFLAGS) $(WARN) $(XCFL)
#-ftemplate-depth-30

CPPFLAGS	= -I.  -I.. -DVMC_SRC -DVMC $(DEBUG) $(SYSPORT) $(SYSFLAGS) $(WARN) $(XCPPFL)
#-ftemplate-depth-30


# (f)lex/yacc/bison generated c-files are notorious for their lousy C
# hence, no warnings...
YCFLAGS	= 	-I .. -DVMC_SRC $(DEBUG) $(SYSPORT) $(SYSFLAGS) $(NOWARN) $(XCFL) 

LINK.cpp=	$(CPP) $(CPPFLAGS) $(LDFLAGS)
LINK.c	=	$(LD) $(CFLAGS) $(LDFLAGS)


# SOURCES
# c-files for VMC
VMC_SRCS = vmc.cpp vmc_process.cpp utility.cpp  structs.cpp destruct.cpp dilshare.cpp membug.cpp spec_assign.cpp \
money.cpp pp1.cpp pp2.cpp pp3.cpp pp4.cpp pp5.cpp pp6.cpp pp7.cpp pp8.cpp 
VMC_PSRC = tmp_vmcpar.cpp tmp_vmclex.cpp tmp_dilpar.cpp tmp_dillex.cpp
# OBJECTS

TVMC_OBJS  =  $(VMC_SRCS:.cpp=.o)  vmc_db_file.o
VMC_OBJS = $(addprefix $(PLATFORMDIR)/$(PLATFORM)/vmc/,$(TVMC_OBJS))

TVMC_POBJS  =	$(VMC_PSRC:.cpp=.o) 
VMC_POBJS = $(addprefix $(PLATFORMDIR)/$(PLATFORM)/vmc/,$(TVMC_POBJS))

TSERV_OBJS = color.o textutil.o namelist.o files.o extra.o \
            bytestring.o common.o intlist.o
SERV_OBJS = $(addprefix $(PLATFORMDIR)/$(PLATFORM)/,$(TSERV_OBJS))
   # ------------------------------ oOoOoOo ------------------------------ #

all:	$(BINDIR)vmc

#$(VMC_SRCS:.cpp=.o):

$(VMC_OBJS): 
	$(CPP) $(CPPFLAGS) -g -c $(subst $(PLATFORMDIR)/$(PLATFORM)/vmc/,,$(@:.o=.cpp)) -o $@
$(VMC_POBJS): $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_vmcpar.cpp $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_vmclex.cpp $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_dilpar.cpp $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_dillex.cpp

	$(CPP) $(CPPFLAGS) -g -I $(PLATFORMDIR)/$(PLATFORM)/vmc/ -c $(@:.o=.cpp) -o $@

clean:
	-rm  $(PLATFORMDIR)/vmc/*
	-rm  $(BINDIR)/vmc
	-rm  $(PLATFORMDIR)/$(TSERV_OBJS)

# Need to get the tmp_*.c files into .depends as well
depend:	
	@echo Updating dependencies...
	@$(MKDPND) -I .. $(VMC_SRCS) | ./depend.awk > .depends
#	@$(MKDPND) -I .. $(VMC_SRCS) > .depends
	@echo Done.

TAGS:	$(SERV_SRCS) $(SERV_HDRS) # Only tags for server...
	@echo Building TAGS file....
	@etags $(SERV_SRCS) $(SERV_HDRS)
	@echo Done.

$(BINDIR)vmc:	$(VMC_OBJS) $(VMC_POBJS)
	$(LINK.c) $(OPT) $(VMC_OBJS) $(VMC_POBJS) $(SERV_OBJS) -lfl -g -o $@ 
#-lccmalloc -ldl


$(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_vmcpar.cpp: vmcpar.y
	$(YACC) --debug $(YFLAGS) vmcpar.y
	@-if [ -f y.output ]; then mv y.output $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_vmcpar.output; fi
	@cp y.tab.c $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_vmcpar.cpp
	@cp y.tab.h $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_vmcpar.h

$(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_vmclex.cpp:	vmclex.l
	$(LEX) -t vmclex.l > $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_vmclex.cpp

$(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_dilpar.cpp: dilpar.y
	$(YACC) --debug -d -v -pdil dilpar.y
	@-if [ -f dilpar.output ]; then mv dilpar.output $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_dilpar.output; fi
	@cp dilpar.tab.c $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_dilpar.cpp
	@cp dilpar.tab.h $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_dilpar.h

$(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_dillex.cpp:	dillex.l
	$(LEX) -Pdil -t dillex.l  > $(PLATFORMDIR)/$(PLATFORM)/vmc/tmp_dillex.cpp 

#.include ".depends"		# For `normal' make
#include .depends		# For gnu-make
