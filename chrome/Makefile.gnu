%.obj: %.c
        ccn64 -c -g -DSN64 -DF3DEX_GBI_2 $*.c -I.

chrome.bin: letters.obj dram_stack.obj \
	static.obj zbuffer.obj cfb.obj \
	chrome.lk
	psylink /c /s /p /m /g @chrome.lk,chrome.bin,chrome.sym,chrome.map
	setcsum chrome.bin 0x80200000

letters.obj: letters.c
dram_stack.obj: dram_stack.c
static.obj: static.c
zbuffer.obj: zbuffer.c
cfb.obj: cfb.c

clean:
	del *.obj
	del chrome.bin
	del chrome.sym
	del chrome.map
