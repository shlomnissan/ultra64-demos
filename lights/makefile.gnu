%.obj: %.c
        ccn64 -g -c -I. -DDEBUG -DSN64 -DF3DEX_GBI_2 $*.c

lights.bin:  teapot.obj controller.obj dram_stack.obj \
	static.obj zbuffer.obj cfb.obj \
	lights.lk
	psylink /c /s /p /m /g @lights.lk,lights.bin,lights.sym,lights.map
	setcsum lights.bin 0x80200000

teapot.obj: teapot.c
controller.obj: controller.c
dram_stack.obj: dram_stack.c
static.obj: static.c
zbuffer.obj: zbuffer.c
cfb.obj: cfb.c

clean:
	del *.obj
	del lights.bin
	del lights.sym
	del lights.map

