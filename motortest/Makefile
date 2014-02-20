include $(ROOT)/usr/include/make/PRdefs

OPTIMIZER	= -g
#OPTIMIZER	= -O2 

APP		= main.out
TARGETS		= main.n64

HFILES		= motortest.h nu64sys.h thread.h

CODEFILES	= motortest.c nu64sys.c asci.c
CODEOBJECTS	= $(CODEFILES:.c=.o)

DATAFILES	= cfb1.c
DATAOBJECTS	= $(DATAFILES:.c=.o)

CODESEGMENT	= codesegment.o

OBJECTS		= $(CODESEGMENT) $(DATAOBJECTS)

LCINCS		= -I. -I$(ROOT)/usr/include/PR -I$(ROOT)/usr/include

LCOPTS =	-G 0

LDIRT		= $(APP) cont *.so

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

SOURCES =	Makefile spec $(HFILES) $(CODEFILES) \
		$(DATAFILES) README README.jpn

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
	$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS) $(APP):	spec $(OBJECTS)
	$(MAKEROM) -r $(TARGETS) spec

nu64sys.o:	nu64sys.c nu64sys.h

asci.o:		asci.c

motortest.o:	motortest.c motortest.h


# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
