#!smake -k
#
# Makefile for u64 dev board inventory program
#
#

include $(ROOT)/usr/include/make/PRdefs

APP =		ginv.out

OPTIMIZER =	-g

TARGETS =	ginv.n64

TEXHFILES =	brick.h

HFILES =	ginv.h

CODEFILES   =	ginv.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) 

LCINCS =	-I.
LCOPTS =	-G 0
LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

LDIRT  =	$(APP)

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	ginv.o
		$(LD) -o $(CODESEGMENT) -r ginv.o $(LDFLAGS)

$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)

# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
