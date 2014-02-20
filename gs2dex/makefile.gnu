%.obj: %.c
        ccn64 -c -g -DSN64 $*.c -I.

gs2dex.bin: system.obj main.obj uc_assert.obj action.obj sprite.obj \
	static.obj texture.obj sprite_ball.obj \
	l2_tv.obj \
	system_cfb.obj system_rspbuf.obj \
	gs2dex.lk
	psylink /c /s /p /m @gs2dex.lk,gs2dex.bin,gs2dex.sym,gs2dex.map
	setcsum gs2dex.bin

system.obj: system.c
main.obj: main.c
uc_assert.obj: uc_assert.c
action.obj: action.c
sprite.obj: sprite.c
static.obj: static.c
texture.obj: texture.c
sprite_ball.obj: sprite_ball.c
l2_tv.obj: l2_tv.c
system_cfb.obj: system_cfb.c
system_rspbuf.obj: system_rspbuf.c

# LCDEFS  =	-DDEBUG -DF3DEX_GBI -DRSP_DEBUG

