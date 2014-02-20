%.obj: %.c
        ccn64 -g -c -I. -DSN64 -DF3DEX_GBI_2 $*.c

tile_rect3d.bin: main.obj tile_rectangle.obj controller.obj dram_stack.obj \
                 static.obj cfb.obj rsp_cfb.obj cafe.obj road.obj world.obj \
                 tile_rect3d.lk
        psylink /c /s /p /m /g @tile_rect3d.lk,tile_rect3d.bin,tile_rect3d.sym,tile_rect3d.map
        setcsum tile_rect3d.bin 0x80000400

main.obj: main.c
tile_rectangle.obj: tile_rectangle.c
controller.obj: controller.c
dram_stack.obj: dram_stack.c
static.obj: static.c
cfb.obj: cfb.c
rsp_cfb.obj: rsp_cfb.c
cafe.obj: cafe.c
road.obj: road.c
world.obj: world.c

clean:
	del *.obj
        del tile_rect3d.bin
        del tile_rect3d.sym
        del tile_rect3d.map

