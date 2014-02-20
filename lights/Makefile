#!smake -k
#
# Makefile for teapot demo application 
#
#
#

include $(ROOT)/usr/include/make/PRdefs

APP =		teapot.out

OPTIMIZER =	-g

TARGETS =	teapot.n64

TEXHFILES =	teapot_shad64.h

HFILES =	I4spec.h controller.h teapot_tri.h \
		IA8phong.h teapot.h teapot_vtx.h


CODEFILES   =	teapot.c controller.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c	\
		zbuffer.c	\
		cfb.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =	-DUSE_HIGHLIGHT -DUSE_MIPMAP
LCINCS =	-I. -I$(ROOT)/usr/include/PR -I $(ROOT)/usr/include
LCOPTS =	-G 0

LDIRT  =	$(APP) teapot *.so

#
# Use non debug library just so we have some demos using it to test it
#
LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra -L$(GCCDIR)/mipse/lib -lkmc 

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
