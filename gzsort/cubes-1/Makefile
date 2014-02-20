#! smake -f
#---------------------------------------------------------------------#
#	Copyright (C) 1997, Nintendo.
#	
#	File		Makefile
#	Coded    by	Yoshitaka Yasumoto.	Sep 12, 1997.
#	Modified by	
#	
#	$Id: Makefile,v 1.8 1997/11/26 09:10:37 mitu Exp $
#---------------------------------------------------------------------#

##### BASE= {irix.cc, pc}
BASE = irix.cc
#ifndef	IRIX
BASE = pc
#endif

include ../makedefs.$(BASE)

##### APP/OPTIMIZER
APP       =	cube
OPTIMIZER =	-g

##### LIBULTRA_SUFFIX= { , _d, _rom}
#LIBULTRA_SUFFIX =	_d
#LIBULTRA_SUFFIX =	_rom
LIBULTRA_SUFFIX =	

#
HFILES =	action.h brick.h cube.h system.h uc_assert.h
CODEFILES =	action.c cube.c cube_geo.c main.c static.c system.c uc_assert.c
CODEOBJECTS =	$(CODEFILES:.c=.o)
CODESEGMENT =	codesegment.o
DATAFILES =	system_cfb.c system_rspbuf.c
DATAOBJECTS =	$(DATAFILES:.c=.o)
OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)
MAPFILE =	load.map

LIBDIR =	libzsort
LIBZSORT =	libzsort.a
LIBZSORTFLAG =	-L$(LIBDIR) -lzsort

SUBDIRS =	$(LIBDIR)

LCINCS +=	-I. -I$(LIBDIR)
LCDEFS +=	-DDEBUG -DF3DEX_GBI -DF3DEX_GBI_2
LCDEFS +=	-DFOG
LDFLAGS +=	$(LIBZSORTFLAG)
LDIRT    =	*~ $(APPNAME) $(TARGETS) $(MAPFILE) Makedepend

default:	$(TARGETS)

install: default
		$(SUBDIRS_MAKERULE)
		$(INSTALL) -m 444 -F /usr/src/PR/demos/gzsort/cubes-1 \
				$(HFILES) $(CODEFILES) $(DATAFILES) \
				Makefile spec flowers32x32.rgb

install-pc: install
		make reflect.h
		$(INSTALL) -m 444 -F /usr/src/PR/demos/gzsort/cubes-1 reflect.h

exports:;

clobber:;	rm -rf $(DIRT) $(TARGETS)
		$(SUBDIRS_MAKERULE)

clean:;		rm -rf $(DIRT)
		$(SUBDIRS_MAKERULE)

$(LIBDIR)/$(LIBZSORT):	$(LIBDIR)
#if	($(BASE)=="pc")
		make -C $(LIBDIR) BASE=$(BASE) $(LIBZSORT)
ifeq	($(BASE),"irix.cc")
#else
		cd $(LIBDIR); make BASE=$(BASE) $(LIBZSORT)
#endif
#if	($(BASE)=="pc")
endif
#endif

$(CODESEGMENT):	$(CODEOBJECTS) $(LIBDIR)/$(LIBZSORT)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS):	spec $(OBJECTS)
		$(MAKEROM) $(LCDEFS) -r $(TARGETS) spec

cube.o:		cube.h

cube_geo.o:	reflect.h brick.h cube.h

#if	($(BASE)=="pc")
ifeq	($(BASE),"irix.cc")
#endif
reflect.h:	flowers32x32.rgb
	$(RGB2C) -m tex_reflect $? > $@
#if	($(BASE)=="pc")
endif
#endif

#======== End of Makefile ========
