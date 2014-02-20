%.obj: %.c
        ccn64 -c -g -DSN64 -DF3DEX_GBI_2 $*.c -I.

polyline.bin: lines.obj static.obj dram_stack.obj \
		zbuffer.obj cfb.obj \
		polyline.lk
	psylink /c /s /p /m /g @polyline.lk,polyline.bin,polyline.sym,polyline.map
	setcsum polyline.bin 0x80000400

lines.obj: lines.c
static.obj: static.c
dram_stack.obj: dram_stack.c
zbuffer.obj: zbuffer.c
cfb.obj: cfb.c

clean:
	del *.obj
	del polyline.bin
	del polyline.sym
	del polyline.map
