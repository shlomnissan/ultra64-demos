#!smake -k
#
# Makefile for topgun demo application 
#
#
#
include $(ROOT)/usr/include/make/PRdefs

APP =		topgun.out

OPTIMIZER =	-g

TARGETS =	topgun.n64

TEXHFILES =	ground.h sky.h tree.h
TEXTURES =	Textures/clouds.rgb Textures/harrier_num.rgba \
		Textures/stree1.rgba  Textures/terrain.rgb \
		Textures/camo2.int Textures/decal.inta

MODELS =	Models/av8b.flt

HFILES =	topgun.h

CODEFILES   =	topgun.c dram_stack.c rdp_output.c rdp_output_len.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	av8b.c		\
		cfb.c		\
		ground.c	\
		static.c	\
		zbuffer.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCINCS =	-I. -I$(ROOT)/usr/include/PR -I $(ROOT)/usr/include
LCOPTS =	-G 0

LDIRT  =	$(APP) topgun

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

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
