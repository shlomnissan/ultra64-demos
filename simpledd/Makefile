#!smake
include $(ROOT)/usr/include/make/PRdefs

# to make the tags file do "make simpletags"
# to make just the simple_d directory do "make SUBDIRS=simple_d"

MAKEROM=/usr/sbin/makerom
MAKEDISK=/usr/sbin/makedisk

SUBDIRS=simple_d simple_dd simple_rom
COMMONPREF = simple

APP =		simple

HFILES =		\
	simple.h	\
	audio.h		\
	gfx.h		\
	misc.h		\
	gfx_tri.h	\
	gfx_vtx.h

CODEFILES =		\
	audio.c		\
	audiomgr.c	\
	controller.c	\
	gfx.c		\
	gfxstack.c	\
	gfxyield.c	\
	misc.c		\
	simple.c	\
	sched.c		\
	drawfont.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

CODESEGMENT =	codesegment.o

# Data files that have their own segments:

DATAFILES =		\
	gfxdynamic.c	\
	gfxdlists.c	\
	gfxzbuffer.c	\
	gfxinit.c	\
	gfxstatic.c	\
	gfxcfb.c	

DATAOBJECTS =	$(DATAFILES:.c=.o)

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCINCS =	-I.. -I$(ROOT)/usr/include/PR 
LCOPTS =	-g $(DFLAG) -fullwarn -non_shared -G 0 -Xcpluscomm
LCDEFS =	

LDIRT  =	load.map

LDFLAGS =	$(MKDEPOPT) -nostdlib -L$(ROOT)/usr/lib -L$(ROOT)/usr/lib/PR 

.PATH: ..

sinclude locdefs

#include $(COMMONRULES)

default:
	for i in $(SUBDIRS) ; do \
	    echo ==== $$i === ; \
	    cd $$i ; \
	    $(MAKE) -f ../Makefile loc_$$i ; \
	    cd .. ; \
	done

$(COMMONTARGS) :
	for i in $(SUBDIRS) ; do \
	    echo ==== clobber $$i === ; \
	    cd $$i ; \
	    $(MAKE) -f ../Makefile loc_$@ ; \
	    cd .. ; \
	done

include $(COMMONRULES)

install:	default
		$(INSTALL) -m 444 -F /usr/src/PR/simple \
			$(HFILES) $(CODEFILES) \
			$(DATAFILES) Makefile spec \
			simple/log.fmt simple_d/locdefs \
			simple/locdefs simple_rom/locdefs

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -m -o $(CODESEGMENT) -r $(CODEOBJECTS) \
			 $(LDFLAGS) > load.map

rom:	../spec $(OBJECTS)
		$(MAKEROM) $(MAKEROMDEFS) ../spec


disk:	../spec $(OBJECTS)
		$(MAKEDISK) $(MAKEDISKDEFS) $(MAKEROMDEFS) ../spec
