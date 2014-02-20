#!smake

include $(ROOT)/usr/include/make/PRdefs

APP =		soundmonkey.out
OPTIMIZER =	-g

TARGETS =	soundmonkey.n64

HFILES =	soundmonkey.h

CODEFILES =	soundmonkey.c
CODEOBJECTS =	$(CODEFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) 

LCINCS =	-I. -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0

LDIRT  =	$(APP) $(APP_EM) monkey.aud load.map load_em.map

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra -L$(GCCDIR)/mipse/lib -lkmc 

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
