#!smake -k
#
# Makefile for  NINTENDO64 SAMPLE PROGRAM 2
#
#	Copyright (C) 1997, NINTENDO Co,Ltd.
#

include $(ROOT)/usr/include/make/PRdefs

APP =		nnsample2.out

OPTIMIZER =	-g

TARGETS =	nnsample2.n64

TEXHFILES =

HFILES =	def.h message.h vram.h nnsched.h segment.h main.h  \
		audio.h graphic.h controller.h far_.h middle_.h near_.h


CODEFILES   = 	boot.c rdp_output.c dram_stack.c \
		main.c message.c nnsched.c main00.c controller.c audio.c \
		graphic.c graphic_00.c gfxyield.c gfxinit.c \
		gfxperfm.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	cfb.c zbuf.c gfxdlistbuf.c lodmodel.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =
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
