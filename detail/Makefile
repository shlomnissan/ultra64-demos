#!smake -k
#
# Makefile for detail texture demo application 
#
#

include $(ROOT)/usr/include/make/PRdefs

APP =		detail.out		

OPTIMIZER =	-g

TARGETS =	detail.n64

TEXHFILES =	brick.h

HFILES =	app.h walk_around.h \
		RGBA16field32.h		\
		RGBA16forest32.h 	\
		RGBA16drainage32.h	\
		I8water32.h		\
		RGBA16leaf32.h		\
		RGBA16sand64x32.h	\
		gorge_vtx.h		\
		gorge_dl.h		\
		river_dl.h		\
		vector.h		\
		matrix.h

CODEFILES   =	main.c walk_around.c matrix.c vector.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c cfb.c rsp_cfb.c rsp_czb.c czb.c

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
