%.obj: %.c
        ccn64 -c -g -DSN64 -DF3DEX_GBI_2 $*.c -I.

topgun.bin: topgun.obj dram_stack.obj rdp_output.obj \
		av8b.obj cfb.obj ground.obj static.obj zbuffer.obj \
		topgun.lk
	psylink /c /s /p /m /g @topgun.lk,topgun.bin,topgun.sym,topgun.map
	setcsum topgun.bin 0x80000400

topgun.obj: topgun.c
dram_stack.obj: dram_stack.c
rdp_output.obj: rdp_output.c
av8b.obj: av8b.c
cfb.obj: cfb.c
ground.obj: ground.c
static.obj: static.c
zbuffer.obj: zbuffer.c

clean:
	del *.obj
	del topgun.bin
	del topgun.sym
	del topgun.map
