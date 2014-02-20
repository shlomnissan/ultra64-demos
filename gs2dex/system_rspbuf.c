/*---------------------------------------------------------------------
	Copyright (C) 1997, Nintendo.
	
	File		system_rspbuf.c
	Coded    by	Yoshitaka Yasumoto.	Jan 23, 1997.
	Modified by	
	Comments	
	
	$Id: system_rspbuf.c,v 1.2 1997/08/07 03:24:24 yasu Exp $
  ---------------------------------------------------------------------*/
#include	<ultra64.h>
#include	"system.h"

u64	system_rdpfifo[RDPFIFO_SIZE];
u64	system_rspyield[OS_YIELD_DATA_SIZE/sizeof(u64)];

/*======== End of system_rspbuf.c ========*/
