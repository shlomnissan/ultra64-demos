%.obj: %.c
        ccn64 -g -c -I. -DSN64 $*.c

soundmonkey.bin: soundmonkey.obj \
        soundmonkey.lk
        psylink /c /s /p /m /g @soundmonkey.lk,soundmonkey.bin,soundmonkey.sym,soundmonkey.map
        setcsum soundmonkey.bin 0x80000400

soundmonkey.obj: soundmonkey.c

clean:
        del *.obj
        del soundmonkey.bin
        del soundmonkey.sym
        del soundmonkey.map

