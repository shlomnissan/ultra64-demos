%.obj: %.c
        ccn64 -g -c -DSN64 $*.c

onetri.bin: onetri.obj dram_stack.obj rdp_output.obj rdp_output_len.obj \
		static.obj cfb.obj rsp_cfb.obj \
		onetri.lk
        psylink /c /s /p /m /g @onetri.lk,onetri.bin,onetri.sym,onetri.map
	setcsum onetri.bin 0x80000400

onetri.obj: onetri.c
dram_stack.obj: dram_stack.c
rdp_output.obj: rdp_output.c
rdp_output_len.obj: rdp_output_len.c
static.obj: static.c
cfb.obj: cfb.c
rsp_cfb.obj: rsp_cfb.c

clean:
	del *.obj
	del onetri.bin
	del onetri.sym
	del onetri.map

