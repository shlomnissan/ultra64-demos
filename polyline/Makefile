#!smake -k
#
# Makefile for lines demo application 
#
#

include $(ROOT)/usr/include/make/PRdefs

APP =		lines.out

OPTIMIZER =	-g

TARGETS =	lines.n64

HFILES =	lines.h static.h

CODEFILES   =	lines.c static.c 

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	zbuffer.c cfb.c 

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =
LCINCS =	-I. -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0
LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

LDIRT  =	$(APP) makesphere

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
