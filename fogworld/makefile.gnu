%.obj: %.c
        ccn64 -c -g -DSN64 -DF3DEX_GBI_2 $*.c -I. -Ic:/ultra/usr/include -Ic:/ultra/usr/include/pr -Ic:/sn_win/include

fogworld.bin: boot.obj game.obj controller.obj font.obj \
	gfxinit.obj gfxstatic.obj texture.obj cfb.obj rsp_cfb.obj zbuffer.obj \
	fogworld.lk
	psylink /c /s /p /m /g @fogworld.lk,fogworld.bin,fogworld.sym,fogworld.map
	setcsum fogworld.bin 0x80000400

boot.obj: boot.c
game.obj: game.c
controller.obj: controller.c
font.obj: font.c
dram_stack.obj: dram_stack.c
gfxinit.obj: gfxinit.c
gfxstatic.obj: gfxstatic.c
texture.obj: texture.c
cfb.obj: cfb.c
rsp_cfb.obj: rsp_cfb.c
zbuffer.obj: zbuffer.c

clean:
	del *.obj
	del fogworld.bin
	del fogworld.sym
	del fogworld.map
