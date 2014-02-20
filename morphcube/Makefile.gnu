%.obj: %.c
        ccn64 -g -c -DSN64 -DF3DEX_GBI_2 $*.c -I.

morphcube.bin: morphdemo.obj morph.obj makeverts.obj dram_stack.obj \
		static.obj dynamic.obj zbuf.obj cfb.obj \
		morphcube.lk
	psylink /c /s /p /m /g @morphcube.lk,morphcube.bin,morphcube.sym,morphcube.map
	setcsum morphcube.bin 0x80200000

morphdemo.obj: morphdemo.c
morph.obj: morph.c
makeverts.obj: makeverts.c
dram_stack.obj: dram_stack.c
static.obj: static.c
dynamic.obj: dynamic.c
zbuf.obj: zbuf.c
cfb.obj: cfb.c

clean:
	del *.obj
	del morphcube.bin
	del morphcube.sym
	del morphcube.map
