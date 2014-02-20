%.obj: %.c
        ccn64 -g -c -DSN64 -DF3DEX_GBI_2 $*.c -I.

morphfaces.bin: morphfaces.obj morph.obj data.obj dram_stack.obj \
		static.obj dynamic.obj zbuf.obj cfb.obj \
		morphfaces.lk
	psylink /c /s /p /m /g /l c:\psyq\dts\n64\lib @morphfaces.lk,morphfaces.bin,morphfaces.sym,morphfaces.map
	setcsum morphfaces.bin 0x80200000

morphfaces.obj: morphfaces.c
morph.obj: morph.c
data.obj: data.c
dram_stack.obj: dram_stack.c
static.obj: static.c
dynamic.obj: dynamic.c
zbuf.obj: zbuf.c
cfb.obj: cfb.c

clean:
	del *.obj
	del morphfaces.bin
	del morphfaces.sym
	del morphfaces.map
