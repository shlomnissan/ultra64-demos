#!smake -k
#
#

include $(ROOT)/usr/include/make/PRdefs

APP =		pfs.out

OPTIMIZER =	-g

TARGETS =	pfs.n64

TEXHFILES =	textlib.h

HFILES =	$(TEXHFILES) thread.h

CODEFILES   =	main.c nu64sys.c textlib.c asci.c eep_test.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	cfb.c 

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

#LCDEFS =	-DFORMAT -DCREATE  -D_DEBUG
LCDEFS =	-D_DEBUG  
LCINCS =	-I. -I$(ROOT)/usr/include -I$(ROOT)/usr/include/PR
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
