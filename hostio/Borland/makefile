BIN	= $(BCC)\bin
INC	= $(BCC)\include
PRINC	= $(ROOT)\usr\include
LIB	= $(BCC)\lib
PRLIB	= $(ROOT)\usr\lib\borland

OBJS	= host.obj

%.obj:%.c
	$(BIN)\bcc32 -c -v -tWC -I$(INC);$(PRINC) -DWIN32 -D_LANGUAGE_C $*.c

host.exe : $(OBJS)
	echo -v -Tpe -ap -L$(LIB);$(PRLIB) c0x32.obj + >lnk.cmd
	echo $(OBJS) >>lnk.cmd
	$(BIN)\tlink32 @lnk.cmd,host.exe,,cw32.lib import32.lib hostiopc.lib

LDIRT	= *.obj *.exe *.map *.cmd

include $(ROOT)/usr/include/make/commonrules
