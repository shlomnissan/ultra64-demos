%.obj: %.c
        ccn64 -c -g -DSN64 -DF3DEX_GBI_2 $*.c -I.

blockmonkey.bin : block.obj static.obj subcube.obj subplane.obj controller.obj timer.obj \
        rdp_output.obj dram_stack.obj \
	zbuffer.obj cfb.obj \
	blockmonkey.lk
	psylink /c /s /p /m /g @blockmonkey.lk,blockmonkey.bin,blockmonkey.sym,blockmonkey.map
	setcsum blockmonkey.bin 0x80200000

block.obj : block.c
static.obj : static.c
subcube.obj : subcube.c
subplane.obj : subplane.c
controller.obj : controller.c
timer.obj : timer.c
rdp_output.obj : rdp_output.c
rdp_output_len.obj : rdp_output_len.c
zbuffer.obj : zbuffer.c
cfb.obj : cfb.c
dram_stack.obj : dram_stack.c

clean:
	del *.obj
	del blockmonkey.bin
	del blockmonkey.sym
	del blockmonkey.map
