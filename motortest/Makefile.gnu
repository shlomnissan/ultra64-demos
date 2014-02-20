%.obj: %.c
        ccn64 -c -g -DSN64 $*.c -I.

motor.bin: motortest.obj nu64sys.obj asci.obj \
		cfb1.obj \
		motor.lk
	psylink /c /s /p /m /g @motor.lk,motor.bin,motor.sym,motor.map
	setcsum motor.bin 0x80000400

motortest.obj: motortest.c
nu64sys.obj: nu64sys.c
asci.obj: asci.c
cfb1.obj: cfb1.c

clean:
	del *.obj
	del motor.bin
	del motor.sym
	del motor.map
