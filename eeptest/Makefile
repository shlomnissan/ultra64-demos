#!smake -k
#
# Makefile for ddspgame demo application 
#
#


#DEBUG = YES
#FINAL = YES

include $(ROOT)/usr/include/make/PRdefs

ifeq ($(FINAL), YES)

OPTIMIZER =	-O2
LCDEFS =
N64LIB	= -lgultra_rom

else

OPTIMIZER =	-g
ifeq ($(DEBUG), YES)
LCDEFS = -DDEBUG
else
LCDEFS = 
endif
N64LIB	= -lgultra_d

endif

APP		= main.out
TARGETS		= main.n64

HFILES		= nu64sys.h thread.h eeptest.h graph.h

CODEFILES	= eeptest.c nu64sys.c graph.c  asci.c
CODEOBJECTS	= $(CODEFILES:.c=.o)

DATAFILES	=  cfb1.c
DATAOBJECTS	= $(DATAFILES:.c=.o)

CODESEGMENT	= codesegment.o

OBJECTS		= $(CODESEGMENT) $(DATAOBJECTS)

LCINCS		= -I. -I$(ROOT)/usr/include/PR -I$(ROOT)/usr/include

LCOPTS =	-G 0

LDIRT		= $(APP) cont *.so

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib $(N64LIB) -L$(GCCDIR)/mipse/lib -lkmc 

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
	$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

ifeq ($(FINAL), YES)
$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) -s 9 -r $(TARGETS) spec
		makemask $(TARGETS)
else
$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) -r $(TARGETS) spec
endif
