#
# Makefile for application test case 'tron'
#

include $(ROOT)/usr/include/make/PRdefs

APP =		tron.out

OPTIMIZER =	-g

TARGETS =	tron.n64

HFILES =	tron.h

CODEFILES =	tron.c dram_stack.c dram_yield.c
CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES =	dynamic.c zbuffer.c init.c cfb.c grid.c
DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o
CODESEGMENT_EM = codesegment_em.o

OBJECTS =	$(CODESEGMENT) $(CODEOBJECTS) $(DATAOBJECTS)

LCINCS =	-I. -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0

LDIRT  =	$(APP)

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

SOURCES =	Makefile spec $(HFILES) $(CODEFILES) \
		dynamic.c zbuffer.c init.c cfb.c makegrid.c

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
