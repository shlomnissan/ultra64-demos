%.obj: %.c
        ccn64 -c -g -DSN64 -DF3DEX_GBI_2 $*.c -I.

spgame.bin: main.obj spgame.obj font.obj pics.obj dram_stack.obj \
		static.obj cfb.obj rsp_cfb.obj \
		spgame.lk
	psylink /c /s /p /m /g @spgame.lk,spgame.bin,spgame.sym,spgame.map
	setcsum spgame.bin 0x80000400

main.obj: main.c
spgame.obj: spgame.c
font.obj: font.c
pics.obj: pics.c
dram_stack.obj: dram_stack.c
static.obj: static.c
cfb.obj: cfb.c
rsp_cfb.obj: rsp_cfb.c

clean:
	del *.obj
	del spgame.bin
	del spgame.sym
	del spgame.map
