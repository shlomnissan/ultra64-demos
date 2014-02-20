%.obj: %.c
        ccn64 -g -c -I. -DSN64 -DF3DEX_GBI_2 $*.c

tile_rect2d.bin: main.obj tile_rectangle.obj controller.obj dram_stack.obj \
		    static.obj cfb.obj rsp_cfb.obj \
                    tile_rect2d.lk
        psylink /c /s /p /m /g @tile_rect2d.lk,tile_rect2d.bin,tile_rect2d.sym,tile_rect2d.map
        setcsum tile_rect2d.bin 0x80000400

main.obj: main.c
tile_rectangle.obj: tile_rectangle.c
controller.obj: controller.c
dram_stack.obj: dram_stack.c
static.obj: static.c
cfb.obj: cfb.c
rsp_cfb.obj: rsp_cfb.c

clean:
	del *.obj
        del tile_rect2d.bin
        del tile_rect2d.sym
        del tile_rect2d.map

