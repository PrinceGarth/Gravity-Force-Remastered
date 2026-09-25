# 
# GRAVITY FORCE makefile
#
# 8/2000 by Jens Hassler
#
#
# make         -- compiles, zips, installs
# make all     -- compiles the source
# make zip     -- zips executable
# make install -- installs the exe
# make clean   -- cleans up
#
# make ed      -- makes "editor" (argh) (run "make all" first) 
#                 and installs it
#

DOSEXE = gfnew.exe
DOSEDEXE = gfed.exe

CC = -gcc
FLAGS = -O2 -m486 -ffast-math -fomit-frame-pointer
EDFLAGS =
LIBS = -lalttf -ljgmod -lalleg
EDLIBS = -lalleg

MOVE = move

ifneq ($(wildcard $(DJDIR)/bin/upx.exe),)
ZIPPER = $(DJDIR)/bin/upx.exe
else
ifneq ($(wildcard $(DJDIR)/bin/djp.exe),)
ZIPPER = $(DJDIR)/bin/djp.exe -s
endif
endif


SRCS =  gf.c gfcnfg.c gfcontrl.c gfdraw.c gfg2pdog.c gfgsngle.c \
	gfgsrace.c gfinit.c gflevel.c gflinit.c gflink.c gflog.c \
	gfmap.c gfmenu.c gfmes.c gfmisc.c gfnetw.c gfosd.c gfpal.c \
	gfpanel.c gfrace.c gfsenm.c gfsobj.c gfsound.c gfspibul.c \
	gfspln.c gfsprt.c gfstat.c mappyal.c

EDSRCS = gfedact.c gfeddlg.c gfedmain.c gfedinit.c
EDAOBJS = gfedraw.o gfcnfg.o gfspln.o mappyal.o

OBJS = $(subst .c,.o,$(SRCS))

EDOBJS = $(subst .c,.o,$(EDSRCS))


gf: $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(DOSEXE)

all: gf zip install

zip:
	$(ZIPPER) $(DOSEXE)

install:
	$(MOVE) $(DOSEXE) ..

clean:
	del *.o

edcomp: $(EDOBJS)
	$(CC) $(EDOBJS) $(EDAOBJS) $(LIBS) -o $(DOSEDEXE)

ed: edcomp
	$(MOVE) $(DOSEDEXE) ..


$(OBJS): $(SRCS)
	$(CC) $(FLAGS) -c $*.c -o $*.o

$(EDOBJS): $(EDSRCS)
	$(CC) $(EDFLAGS) -c $*.c -o $*.o

       