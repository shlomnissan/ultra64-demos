%.obj: %.c
	ccn64 -c -DSN64 $*.c -I.

spritemonkey.bin: sprite.obj controller.obj \
			cfb.obj static.obj \
			spritemonkey.lk
	psylink /c /s /p /m /g @spritemonkey.lk,spritemonkey.bin,spritemonkey.sym,spritemonkey.map
	setcsum spritemonkey.bin 0x80000400

sprite.obj: sprite.c
controller.obj: controller.c
cfb.obj: cfb.c

static.obj: static.sgi
	elfconv static.sgi

clean:
	del *.obj
	del spritemonkey.bin
	del spritemonkey.sym
	del spritemonkey.map
