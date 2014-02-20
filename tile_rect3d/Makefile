#!smake -k
#
# Makefile for tile_rect demo application 
#
#

include $(ROOT)/usr/include/make/PRdefs


APP =		tile_rect.out

OPTIMIZER =	-g

TARGETS =	tile_rect.n64

ROADHFILES = rd01.h rd02.h rd03.h rd04.h rd05.h rd06.h
CAFEHFILES = cv01.h cv03.h cv04.h cv05.h cv06.h
WORLDHFILES = world01.h world02.h world03.h world04.h world05.h world06.h

TEXHFILES =	$(ROADHFILES) $(CAFEHFILES) $(WORLDHFILES)

HFILES =	$(TEXHFILES) tile_rect.h controller.h cafe.h road.h world.h

CODEFILES   =	main.c tile_rectangle.c controller.c dram_stack.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c cfb.c rsp_cfb.c cafe.c road.c world.c

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
