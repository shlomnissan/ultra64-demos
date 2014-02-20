#
# Makefile for app "fault"
#

include $(ROOT)/usr/include/make/PRdefs

APP =		fault.out

OPTIMIZER =	-g

TARGETS =	fault.n64


CODE_HFILES =	fault.h

CODE_CFILES =	fault.c printfault.c

CODE_OBJECTS=	$(CODE_CFILES:.c=.o)

LCINCS =	-I. -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0

LDIRT  =	$(APP)

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

default:	$(TARGETS)

include $(COMMONRULES)

code.o:		$(CODE_OBJECTS) $(ROOT)/usr/lib/libgultra.a
		$(LD) -o $@ -r $(CODE_OBJECTS) $(LDFLAGS)

$(TARGETS) $(APP):	spec code.o
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)



# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
