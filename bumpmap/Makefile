#!smake -k
#
# Makefile for bumpmap demo application 
#
#

include $(ROOT)/usr/include/make/PRdefs

.NOTPARALLEL:

APP =		bumpmap.out

OPTIMIZER =	-g

TARGETS =	bumpmap.n64

OBJHFILES =	./obj_dls.h ./obj_texs.h ./obj_verts.h

TEXHFILES =	obj_real.h

HFILES =	$(TEXHFILES) $(OBJHFILES) bumpmap.h

CODEFILES   =	bumpmap.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c cfb.c rsp_cfb.c zbuf.c

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
