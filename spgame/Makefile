#!smake -k
#
# Makefile for onetri demo application 
#
#

include $(ROOT)/usr/include/make/PRdefs

PRDEPTH = ../..

APP =		spgame.out

OPTIMIZER =	-g

TARGETS =	spgame.n64

TEXDIR  =	Texture

TEXTURES = 	${TEXDIR}/L2_TV.rgb  \
		${TEXDIR}/ball.rgba

TEXHFILES = 	./${TEXDIR}/lit_imgs.h          ./${TEXDIR}/lit_bms.h \
		./${TEXDIR}/ball_imgs.h         ./${TEXDIR}/ball_bms.h

SPRITES =       ./sp_lit.h ./sp_ball.h

HFILES =	main.h font.h font_ext.h

CODEFILES   =	main.c spgame.c font.c pics.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c cfb.c rsp_cfb.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCDEFS =
LCINCS =	-I.
LCOPTS =	-G 0
LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc 

LDIRT  =	$(APP) *.so

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
