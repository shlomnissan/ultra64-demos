%.obj: %.c
        ccn64 -g -c -I. -DSN64 -DF3DEX_GBI_2 $*.c

tron.bin: tron.obj dram_stack.obj dram_yield.obj \
		dynamic.obj zbuffer.obj init.obj cfb.obj grid.obj \
		tron.lk
	psylink /c /s /p /m /g @tron.lk,tron.bin,tron.sym,tron.map
	setcsum tron.bin 0x80000400

tron.obj: tron.c
dram_stack.obj: dram_stack.c
dram_yield.obj: dram_yield.c
dynamic.obj: dynamic.c
zbuffer.obj: zbuffer.c
init.obj: init.c
cfb.obj: cfb.c
grid.obj: grid.c

clean:
	del *.obj
	del tron.bin
	del tron.sym
	del tron.map

