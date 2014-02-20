#! smake -f
#---------------------------------------------------------------------#
#	Copyright (C) 1997, Nintendo.
#	
#	File		Makefile
#	Coded    by	Yoshitaka Yasumoto.	Sep 12, 1997.
#	Modified by	
#	
#	$Id: Makefile,v 1.3 1997/11/26 09:10:39 mitu Exp $
#---------------------------------------------------------------------#

##### BASE= {irix.cc, pc}
BASE = irix.cc
#ifndef	IRIX
BASE = pc
#endif

include ../makedefs.$(BASE)

##### APP/OPTIMIZER
APP       =	zonetri
OPTIMIZER =	-g

##### LIBULTRA_SUFFIX= { , _d, _rom}
#LIBULTRA_SUFFIX =	_d
#LIBULTRA_SUFFIX =	_rom
LIBULTRA_SUFFIX =	

#
HFILES =	system.h
CODEFILES =	main.c static.c system.c
CODEOBJECTS =	$(CODEFILES:.c=.o)
CODESEGMENT =	codesegment.o
DATAFILES =	system_cfb.c system_rspbuf.c
DATAOBJECTS =	$(DATAFILES:.c=.o)
OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)
MAPFILE =	load.map

LCINCS +=	-I.
LCDEFS +=	-DDEBUG -DF3DEX_GBI -DF3DEX_GBI_2
LDIRT    =	*~ $(APPNAME) $(TARGETS) $(MAPFILE) Makedepend

default:	$(TARGETS)

install install-pc: default
		$(INSTALL) -m 444 -F /usr/src/PR/demos/gzsort/zonetri \
				$(HFILES) $(CODEFILES) $(DATAFILES) \
				Makefile spec

exports:;

clobber:;	rm -rf $(DIRT) $(TARGETS)

clean:;		rm -rf $(DIRT)

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS):	spec $(OBJECTS)
		$(MAKEROM) $(LCDEFS) -r $(TARGETS) spec

#======== End of Makefile ========
