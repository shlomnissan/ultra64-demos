#!smake -k -J 4
#
# Makefile for sprite demo application 
#
#

include $(ROOT)/usr/include/make/PRdefs

APP =		sprite.out

#OPTIMIZER =	-g

TARGETS =	sprite.n64

CODEFILES   =	sprite.c controller.c 
CODEOBJECTS =	$(CODEFILES:.c=.o)

HFILES      =   controller.h dk.h dk7.h sprite.h static.h

DATAFILES   =	cfb.c 

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =
LCINCS =	-I. -I$(ROOT)/usr/include/PR
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
