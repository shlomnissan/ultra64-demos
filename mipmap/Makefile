#!smake -k
#
# Makefile for game template
#
#

include $(ROOT)/usr/include/make/PRdefs

APP =		game.out

OPTIMIZER =	-g

TARGETS =	game.n64

TEXHFILES =	RGBA16molecule.h RGBA16moleculeMM.h RGBA16moleculeHMM.h\
		RGBA16brick.h RGBA16brickMM.h RGBA16brickHMM.h\
		RGBA16chainlink.h RGBA16chainlinkMM.h RGBA16chainlinkHMM.h\
		RGBA16mymip.h RGBA16mymipMM.h RGBA16mymipHMM.h\
		RGBA16checker.h RGBA16checkerMM.h RGBA16checkerHMM.h

HFILES =	$(TEXHFILES) boot.h game.h controller.h font.h font_ext.h \
		static.h texture.h

CODEFILES   =	boot.c game.c controller.c font.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	gfxinit.c gfxstatic.c texture.c cfb.c rsp_cfb.c zbuffer.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =	$(HW_FLAGS)
LCINCS =	-I.
LCOPTS =	-G 0
LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

LDIRT  =	$(APP)

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)

# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
