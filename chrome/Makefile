#!smake -k
#
# Makefile for chrome effect demo application 
#
#
#

include $(ROOT)/usr/include/make/PRdefs

APP =		letters.out

OPTIMIZER =	-g

TARGETS =	letters.n64

TEXTURES =	./Texture/metallic.rgb ./Texture/mahogany.rgb ./Texture/smroad.rgb ./Texture/gameday.rgb

TEXHFILES =	./mahogany.h ./metallic.h ./smroad.h ./sp_gameday.h

HFILES =	letters.h u64_tri.h u64_vtx.h

CODEFILES   =	letters.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c	\
		zbuffer.c	\
		cfb.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(TEXHFILES) $(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =
LCINCS =	-I.
LCOPTS =	-G 0

LDIRT  =	$(APP) *.so

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -L$(ROOT)/usr/lib/PR -lgultra_d  -L$(GCCDIR)/mipse/lib -lkmc

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(TEXHFILES) $(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(OBJECTS): $(HFILES)

$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)

# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
