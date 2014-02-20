#! smake -f
#---------------------------------------------------------------------#
#	Copyright (C) 1997, Nintendo.
#	
#	File		Makefile
#	Coded    by	Yoshitaka Yasumoto.	Sep 12, 1997.
#	Modified by	
#	
#	$Id: Makefile,v 1.10 1997/11/26 09:10:38 mitu Exp $
#---------------------------------------------------------------------#

##### BASE= {irix.cc, pc}
BASE      =	irix.cc
#ifndef	IRIX
BASE     =	pc
#endif

include ../../makedefs.$(BASE)

TARGETS   =	libzsort.a
OPTIMIZER =	-O2

HFILES    =	gzpoly.h dirtydcache.h
CFILES    =	zarray.c zsetmmtx.c zcalcpoly.c zdrawpoly.c zdrawpoly_sh.c \
		zdrawpoly_tx.c
ASFILES   =	dirtydcache.s dirtydcache16.s
OBJECTS   =	$(CFILES:.c=.o) $(ASFILES:.s=.o)

LCINCS +=	-I.
LCDEFS +=	-DDEBUG -DF3DEX_GBI -DF3DEX_GBI_2
LCDEFS +=	-DFOG
LDIRT   =	*~ Makedepend

default:	$(TARGETS)

install:	default
		$(INSTALL) -m 444 -F /usr/src/PR/demos/gzsort/cubes-1/libzsort \
				$(HFILES) $(CFILES) $(ASFILES) Makefile

clean:;		rm -rf $(DIRT)

clobber:;	rm -rf $(DIRT) $(TARGETS)

libzsort.a:	$(OBJECTS)
		$(AR) cur $@ $(OBJECTS)

#======== End of Makefile ========
