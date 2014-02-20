%.obj: %.c
        ccn64 -c -g -DSN64 $*.c -I.

fault.bin: fault.obj printfault.obj \
        fault.lk
        psylink /c /s /p /m /g @fault.lk,fault.bin,fault.sym,fault.map
        setcsum fault.bin 0x80000400

fault.obj: fault.c
printfault.obj: printfault.c

clean:
	del *.obj
        del fault.bin
        del fault.sym
        del fault.map
