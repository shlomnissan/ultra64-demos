BIN	= $(MSVC)\bin
INC	= $(MSVC)\include
PRINC	= $(ROOT)\usr\include
LIB	= $(MSVC)\lib
PRLIB	= $(ROOT)\usr\lib\microsoft

OBJS	= test.obj

%.obj:%.c
	$(BIN)\cl /Zi /DWIN32 /D_LANGUAGE_C /c /I$(INC) /I$(PRINC) $*.c

host.exe : $(OBJS)
	echo /DEBUG /LIBPATH:$(LIB) /LIBPATH:$(PRLIB) >lnk.cmd
	echo $(OBJS) >>lnk.cmd
	$(BIN)\link @lnk.cmd hostiopc.lib

LDIRT	= *.pdb *.obj *.ilk *.map *.cmd host.exe
include $(ROOT)/usr/include/make/commonrules
