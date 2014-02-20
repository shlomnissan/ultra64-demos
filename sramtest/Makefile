include $(ROOT)/usr/include/make/PRdefs

APP		= main.out

OPTIMIZER	= -g -DRMON -DDEBUG

TARGETS		= main.n64

HFILES		= nu64sys.h sram.h

CODEFILES	= main.c sraminit.c

CODEOBJECTS	= main.c sraminit.c

DATAFILES	= readbuf.c writebuf.c

DATAOBJECTS	= $(DATAFILES:.c=.o)

CODESEGMENT	= codesegment.o

OBJECTS		= $(CODESEGMENT) $(DATAOBJECTS)

LCINCS		= -I. -I$(ROOT)/usr/include/PR -I $(ROOT)/usr/include

LCOPTS		= -G 0

LDIRT		= $(APP) main *.so

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

default: $(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT): main.o sraminit.o
	$(LD) -o $(CODESEGMENT) -r main.o sraminit.o $(LDFLAGS)

$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)

# for exeGCC CELF
ifeq ($(GCC_CELF), ON)
CELFDIR = .
include $(CELFRULES)
$(CODEOBJECTS) $(DATAOBJECTS) :	$(CELFINFOFILES)
endif
