#!smake -k
#
# Makefile for  NINTENDO64 SAMPLE PROGRAM 1
#
#	Copyright (C) 1997, NINTENDO Co,Ltd.
#

include $(ROOT)/usr/include/make/PRdefs

APP =		nnsample1.out

OPTIMIZER =	-g

TARGETS =	nnsample1.n64

TEXHFILES =

HFILES =	def.h message.h vram.h nnsched.h segment.h main.h  \
		controller.h audio.h graphic.h main01.h cont_finish.h \
		fiter_1s1.h fiter_2s1.h reflect.h 

CODEFILES   = 	boot.c rdp_output.c dram_stack.c \
		main.c message.c nnsched.c main00.c controller.c audio.c \
		graphic.c graphic_00.c gfxyield.c gfxinit.c main01.c  \
		graphic_01.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	cfb.c zbuf.c gfxdlistbuf.c fiter_1s1.c  fiter_2s1.c \
		cont_finish.c

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

$(CODESEGMENT):	$(CODEOBJECTS) Makefile
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)





# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
