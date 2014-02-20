%.obj: %.c
        ccn64 -c -g -DSN64 $*.c -I.

ginv.bin: ginv.obj \
        ginv.lk
        psylink /c /s /p /m /g @ginv.lk,ginv.bin,ginv.sym,ginv.map
        setcsum ginv.bin 0x80000400

ginv.obj: ginv.c

clean:
	del *.obj
        del ginv.bin
        del ginv.sym
        del ginv.map
