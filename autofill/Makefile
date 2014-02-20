#!smake -k
#
# Makefile for performance demo application 
#
#

include $(ROOT)/usr/include/make/PRdefs

APP =		fill.out

OPTIMIZER =	-g

TARGETS =	fill.n64

HFILES =	fill.h controller.h static.h timer.h

CODEFILES   =	fill.c static.c tris.c controller.c timer.c rdp_output.c \
				rdp_output_len.c dram_stack.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	zbuffer.c cfb.c cfb_b.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =	$(HW_FLAGS)
LCINCS =	-I. -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0
LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

LDIRT  =	$(APP)  *.ps *.data *.bak

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
