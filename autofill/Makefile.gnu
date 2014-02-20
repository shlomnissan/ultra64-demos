%.obj: %.c
        ccn64 -c -g -G0 -DSN64 -DF3DEX_GBI_2 $*.c -I.

autofill.bin: fill.obj static.obj tris.obj controller.obj timer.obj rdp_output.obj \
      dram_stack.obj \
	zbuffer.obj cfb.obj cfb_b.obj \
	autofill.lk
	psylink /c /s /p /m /g @autofill.lk,autofill.bin,autofill.sym,autofill.map
	setcsum autofill.bin 0x80200000

fill.obj: fill.c
static.obj: static.c
tris.obj: tris.c
controller.obj: controller.c
timer.obj: timer.c
rdp_output.obj: rdp_output.c
dram_stack.obj: dram_stack.c
zbuffer.obj: zbuffer.c
cfb.obj: cfb.c
cfb_b.obj: cfb_b.c

clean:
	del *.obj
	del autofill.bin
	del autofill.sym
	del autofill.map
