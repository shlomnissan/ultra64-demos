%.obj: %.c
        ccn64 -c -DSN64 -DF3DEX_GBI_2 -DUSE_CFB32 $*.c -I.

spritemonkeyex.bin: main.obj dram_stack.obj tile_rect.obj task.obj rcprdp.obj \
                  framebuffer.obj dma.obj cont.obj game.obj \
                static.obj cfb.obj rsp_cfb.obj dk.obj dk7.obj road.obj l2_tv.obj \
                spritemonkeyex.lk
        psylink /c /s /p /m /g @spritemonkeyex.lk,spritemonkeyex.bin,spritemonkeyex.sym,spritemonkeyex.map
        setcsum spritemonkeyex.bin 0x80000400

main.obj: main.c
dram_stack.obj: dram_stack.c
tile_rect.obj: tile_rect.c
task.obj: task.c
rcprdp.obj: rcprdp.c
framebuffer.obj: framebuffer.c
dma.obj: dma.c
cont.obj: cont.c
game.obj: game.c
static.obj: static.c
cfb.obj: cfb.c
rsp_cfb.obj: rsp_cfb.c
dk.obj: dk.c
dk7.obj: dk7.c
road.obj: road.c
l2_tv.obj: l2_tv.c

clean:
	del *.obj
        del spritemonkeyex.bin
        del spritemonkeyex.sym
        del spritemonkeyex.map
