#! smake
#---------------------------------------------------------------------
#	Copyright (C) 1997, Nintendo.
#	
#	File		Makefile
#	Coded    by	Yoshitaka Yasumoto.	Mar 19, 1997.
#	Modified by	
#	Comments	
#	
#	$Id: Makefile,v 1.8 1997/08/07 03:24:15 yasu Exp $
#---------------------------------------------------------------------

include $(ROOT)/usr/include/make/PRdefs

APP =		sample.out
OPTIMIZER =	-g

TARGETS =	sample.n64
CODEFILES =	system.c main.c uc_assert.c action.c sprite.c
CODEOBJECTS =	$(CODEFILES:.c=.o)
CODESEGMENT =	codesegment.o

STATICFILES =	static.c texture.c sprite_ball.c
STATICOBJECTS =	$(STATICFILES:.c=.o)
STATICSEGMENT =	staticsegment.o

BGFILES =	l2_tv.c
BGOBJECTS =	$(BGFILES:.c=.o)

DATAFILES =	system_cfb.c system_rspbuf.c
DATAOBJECTS =	$(DATAFILES:.c=.o)

OBJECTS =	$(CODESEGMENT) $(STATICSEGMENT) $(DATAOBJECTS) $(BGOBJECTS)

MAPFILE =	load.map

LCINCS  =	-I. -I$(ROOT)/usr/include -I$(ROOT)/usr/include/PR
LCOPTS  =	-G 0

#LCDEFS  =	-DDEBUG -DF3DEX_GBI
LCDEFS  =	-DDEBUG -DF3DEX_GBI -DRSP_DEBUG

LDIRT   =	$(APP) *.dat *.dbg *.lst *.sym *~ $(MAPFILE) \
		Makedepend
LDFLAGS =	$(MKDEPOPT) -L$(LIB) -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(STATICSEGMENT): $(STATICOBJECTS)
		$(LD) -o $(STATICSEGMENT) -r $(STATICOBJECTS) $(LDFLAGS)

$(TARGETS):		spec $(OBJECTS)
		$(MAKEROM) $(LCDEFS) -m -r $(TARGETS) spec > $(MAPFILE)


# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
