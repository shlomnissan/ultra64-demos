#
# This demo has the following features
#
#	CPU vertex morphing
#	mipmap texture tiling
#
#	Bill Mark
#

include $(ROOT)/usr/include/make/PRdefs

APP =		morphdemo.out

#OPTIMIZER =	-O
OPTIMIZER =	-g

TARGETS =	morphdemo.n64

HFILES =	morphdemo.h

CODEFILES =     morphdemo.c morph.c makeverts.c dram_stack.c
CODEOBJECTS =	$(CODEFILES:.c=.o)

# recompile for real & emulator environments
SPECIFICCODEFILES =
SCODEOBJECTS =	$(SPECIFICCODEFILES:.c=.o)
SCODEOBJECTS_EM = $(SPECIFICCODEFILES:.c=_em.o)

CODESEGMENT =	codesegment.o
CODESEGMENT_EM =codesegment_em.o

DATAFILES =	static.c dynamic.c zbuf.c cfb.c
DATAOBJECTS =	$(DATAFILES:.c=.o)

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)
OBJECTS_EM =	$(CODESEGMENT_EM) $(DATAOBJECTS)

LCINCS =	-I. -I$(ROOT)/usr/include/PR 
LCOPTS =	-D_DEBUG -G 0

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

LDIRT  =        $(APP) load.map

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	mariotex.h $(CODEOBJECTS) $(SCODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(SCODEOBJECTS) $(LDFLAGS)

$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)

# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
