%.obj: %.c
        ccn64 -c -g -DSN64 -DF3DEX_GBI_2 $*.c -I.

ci8fb.bin: fill.obj static.obj tris.obj controller.obj timer.obj rdp_output.obj \
        dram_stack.obj \
	zbuffer.obj cfb.obj rsp_cfb.obj \
	ci8fb.lk
	psylink /c /s /p /m /g @ci8fb.lk,ci8fb.bin,ci8fb.sym,ci8fb.map
	setcsum ci8fb.bin 0x80200000

fill.obj: fill.c
static.obj: static.c
tris.obj: tris.c
controller.obj: controller.c
timer.obj: timer.c
rdp_output.obj: rdp_output.c
dram_stack.obj: dram_stack.c
zbuffer.obj: zbuffer.c
cfb.obj: cfb.c
rsp_cfb.obj: rsp_cfb.c

clean:
	del *.obj
	del ci8fb.bin
	del ci8fb.sym
	del ci8fb.map
