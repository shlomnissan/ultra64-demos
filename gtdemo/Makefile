#!smake -k
#
# Makefile for turbo demo application 
#

include $(ROOT)/usr/include/make/PRdefs

APP =		gtdemo.out

OPTIMIZER =	-g

TARGETS =	gtdemo.n64

TEXHFILES =	../Texture/RGBA16checkerMM.h

HFILES =	gtdemo.h controller.h turbo.h character.h char_data.h 	\
		timer.h walk.h punch.h

CODEFILES   =	gtdemo.c floor.c turbo.c character.c controller.c 	\
		timer.c walk.c punch.c rdp_output.c rdp_output_len.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c zbuffer.c cfb.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =	
LCINCS =	-I. -I$(ROOT)/usr/include/PR -I $(ROOT)/usr/include
LCOPTS =	-G 0

LDIRT  =	$(APP) *.so

#
# Use non debug library just so we have some demos using it to test it
#
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
