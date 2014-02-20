BIN	= $(MSVC)\bin
INC	= $(MSVC)\include
PRINC	= $(ROOT)\usr\include
LIB	= $(MSVC)\lib
PRLIB	= $(ROOT)\usr\lib\microsoft

OBJS	= host.obj

%.obj:%.c
	$(BIN)\cl /Zi /DWIN32 /D_LANGUAGE_C /c /I$(INC) /I$(PRINC) $*.c

host.exe : $(OBJS)
	echo /DEBUG /LIBPATH:$(LIB) /LIBPATH:$(PRLIB) >lnk.cmd
	echo $(OBJS) >>lnk.cmd
	$(BIN)\link @lnk.cmd hostiopc.lib

LDIRT	= host.exe *.pdb *.obj *.ilk *.map *.cmd
include $(ROOT)/usr/include/make/commonrules
