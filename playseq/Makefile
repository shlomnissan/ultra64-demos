#!smake

include $(ROOT)/usr/include/make/PRdefs

APP =		playseq.out

OPTIMIZER =	-g

TARGETS =	playseq.n64

HFILES =	playseq.h

CODEFILES =	playseq.c
CODEOBJECTS =	$(CODEFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) 

LCINCS =	-I.
LCOPTS =	-G 0

LDIRT  =	$(APP) $(APP_EM) sequence.aud load.map load_em.map

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) spec -r $(TARGETS) -e $(APP)
