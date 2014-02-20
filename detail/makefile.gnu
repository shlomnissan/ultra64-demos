%.obj: %.c
        ccn64 -c -g -DSN64 -DF3DEX_GBI_2 $*.c -I.

detail.bin: main.obj walk_around.obj matrix.obj vector.obj dram_stack.obj \
	static.obj cfb.obj rsp_cfb.obj rsp_czb.obj czb.obj \
	detail.lk
	psylink /c /s /p /m /g @detail.lk,detail.bin,detail.sym,detail.map
	setcsum detail.bin 0x80200000

main.obj: main.c
walk_around.obj: walk_around.c
matrix.obj: matrix.c
vector.obj: vector.c
dram_stack.obj: dram_stack.c
static.obj: static.c
cfb.obj: cfb.c
rsp_cfb.obj: rsp_cfb.c
rsp_czb.obj: rsp_czb.c
czb.obj: czb.c

clean:
	del *.obj
	del detail.bin
	del detail.sym
	del detail.map
