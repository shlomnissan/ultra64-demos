/*============================================================================

		NINTENDO64 TECHNICAL SUPPORT CENTER 
		
		    NINTENDO64 SAMPLE PROGRAM 2

		Copyright (C) 1997, NINTENDO Co,Ltd.

============================================================================*/
#define F3DEX_GBI
#include <ultra64.h>
#include <PR/ramrom.h>
#include <assert.h>

#include "message.h"
#include "def.h"

/* dram stack buffer */
u64 dram_stack[SP_DRAM_STACK_SIZE64];
