%.obj: %.c
	ccn64 -g -c -DSN64 $*.c -I.

playseq.bin: playseq.obj \
		playseq.lk
	psylink /c /s /p /m /g @playseq.lk,playseq.bin,playseq.sym,playseq.map
	setcsum playseq.bin 0x80000400

playseq.obj: playseq.c

clean:
	del *.obj
	del playseq.bin
	del playseq.sym
	del playseq.map
