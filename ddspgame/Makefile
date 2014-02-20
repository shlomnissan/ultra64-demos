#!smake -k
#
# Makefile for ddspgame demo application 
#
#

include $(ROOT)/usr/include/make/PRdefs
PRDEPTH = ../..

#START_FROM_CASSETTE = YES
#FINAL = YES

ifeq ($(FINAL),YES)
	OPTIMIZER =	-O2
	LCDEFS =
	N64LIB	= -lleo -lgultra_rom
else
	OPTIMIZER =	-g
	LCDEFS += -DDEBUG
	N64LIB	= -lleo_d -lgultra_d
endif

ifeq ($(START_FROM_CASSETTE),YES)
	LCDEFS += -DSTART_FROM_CASSETTE
	DDDEFS =
else
	LCDEFS +=
	DDDEFS = -DD64 -t 0
endif

APP =		main.out
TARGETS =	main.n64

TEXDIR  =	Texture

TEXTURES = 	${TEXDIR}/L2_TV.rgb  \
		${TEXDIR}/ball.rgba

TEXHFILES = 	./${TEXDIR}/lit_imgs.h          ./${TEXDIR}/lit_bms.h \
		./${TEXDIR}/ball_imgs.h         ./${TEXDIR}/ball_bms.h

SPRITES =       ./sp_lit.h ./sp_ball.h

HFILES =	main.h font.h font_ext.h cont.h diskproc.h

CODEFILES   =	main.c spgame.c font.c pics.c setupkanji.c \
		cont.c diskproc.c messages.c static.c

CODEOBJECTS =	$(CODEFILES:.c=.o)

DATAFILES   =	static.c cfb.c rsp_cfb.c kbuffer.c readwritebuf.c

DATAOBJECTS =	$(DATAFILES:.c=.o)

CODESEGMENT =	codesegment.o

OBJECTS =	$(CODESEGMENT) $(DATAOBJECTS)

LCINCS =	-I. -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0

LDFLAGS =	$(MKDEPOPT) -L$(ROOT)/usr/lib $(N64LIB) -L$(GCCDIR)/mipse/lib -lkmc 

LDIRT  =	$(APP) *.so gwrite

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

ifeq ($(START_FROM_CASSETTE),YES)
ifeq ($(FINAL),YES)
$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) -s 9 -r $(TARGETS) spec
		makemask $(TARGETS)
else
$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM) -r $(TARGETS) spec
endif

else

$(TARGETS) $(APP):	spec $(OBJECTS)
	$(MAKEROM) $(DDDEFS) -r $(TARGETS) spec
ifeq ($(FINAL),YES)
	$(ROOT)/usr/sbin/n64mdisk n64mdisk.cfg
endif
endif
