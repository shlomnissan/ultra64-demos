%.obj: %.c
        ccn64 -c -g -DSN64 -DF3DEX_GBI_2 $*.c -I.

bumpmap.bin: bumpmap.obj dram_stack.obj \
        static.obj zbuf.obj cfb.obj rsp_cfb.obj \
	bumpmap.lk
	psylink /c /s /p /m /g @bumpmap.lk,bumpmap.bin,bumpmap.sym,bumpmap.map
	setcsum bumpmap.bin 0x80200000

bumpmap.obj: bumpmap.c
static.obj: static.c
zbuf.obj: zbuf.c
cfb.obj: cfb.c
rsp_cfb.obj: rsp_cfb.c
dram_stack.obj: dram_stack.c

clean:
	del *.obj
	del bumpmap.bin
	del bumpmap.sym
	del bumpmap.map
