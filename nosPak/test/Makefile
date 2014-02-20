#!smake -k
#
# Makefile for nosPak test application 
#
#

include $(ROOT)/usr/include/make/PRdefs

LANG_JAPANESE = 

# for LANG_JAPANESE
PAKLIB =	-lgnos_jpn
# for LANG_ENGLISH
PAKLIB =	-lgnos


APP =		onetri.out

TARGETS =	onetri.n64

TEXHFILES =	brick.h

HFILES =	$(TEXHFILES) onetri.h

CODEFILES   =	onetri.c dram_stack.c rdp_output.c rdp_output_len.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c cfb.c rsp_cfb.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =
LCINCS =	-I. -I../lib
LCOPTS =	-G 0
LDFLAGS =	$(MKDEPOPT) -L../lib $(PAKLIB) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS) $(APP):	$(OBJECTS)
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)

# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
