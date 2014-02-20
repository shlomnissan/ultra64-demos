#!smake
include $(ROOT)/usr/include/make/PRdefs

APP =		game.out

TARGETS =	game.n64

HFILES =	hostio.h

BOOTFILES   =	game.c

BOOTOBJECTS =	$(BOOTFILES:.c=.o)

BOOTSEGMENT =	boot.o

OBJECTS =	$(BOOTSEGMENT)

LCINCS =	-I.
LCOPTS =	-G 0
LCDEFS =	-DPTN64
LDFLAGS =	$(MKDEPOPT) -L$(LIB) -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc

LDIRT  =	$(APP)

default:	$(TARGETS)

include $(COMMONRULES)

$(BOOTSEGMENT):	$(BOOTOBJECTS)
		$(LD) -o $(BOOTSEGMENT) -r $(BOOTOBJECTS) $(LDFLAGS)

$(TARGETS) $(APP):	$(OBJECTS)
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)

# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
