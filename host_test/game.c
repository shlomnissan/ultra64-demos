/**************************************************************************
 *                                                                        *
 *               Copyright (C) 1995, Silicon Graphics, Inc.               *
 *                                                                        *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright  law.  They  may not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *                                                                        *
 *************************************************************************/

#include <ultra64.h>

#define	STACKSIZE	0x2000

u64	bootStack[STACKSIZE/sizeof(u64)];

static void	idle(void *);
static void	mainproc(void *);

static OSThread	idleThread;
static u64	idleThreadStack[STACKSIZE/sizeof(u64)];

static OSThread	mainThread;
static u64	mainThreadStack[STACKSIZE/sizeof(u64)];

static OSThread	rmonThread;
static u64      rmonStack[RMON_STACKSIZE/sizeof(u64)];

#define	NUM_PI_MSGS	8
static OSMesg PiMessages[NUM_PI_MSGS];
static OSMesgQueue PiMessageQ;

#define RDB_SEND_BUF_SIZE   2000

u8  rdbSendBuf[RDB_SEND_BUF_SIZE];

void
boot(void)
{
    
    osInitialize();
    
    osCreateThread(&idleThread, 1, idle, (void *)0,
		   idleThreadStack+STACKSIZE/sizeof(u64), 10);
    osStartThread(&idleThread);

    /* never reached */
}

static void
idle(void *arg)
{
    osCreateViManager(OS_PRIORITY_VIMGR);
    osViSetMode(&osViModeTable[OS_VI_NTSC_LAN1]);

    osInitRdb(rdbSendBuf, RDB_SEND_BUF_SIZE);
    /*
     * Create main thread
     */
    osCreateThread(&mainThread, 3, mainproc, arg,
		   mainThreadStack+STACKSIZE/sizeof(u64), 10);
    
    osStartThread(&mainThread);

    /*
     * Become the idle thread
     */
    osSetThreadPri(0, 0);

    for (;;);
}

u8  ReadBlockBuf[0x100000];

/*
 * This is the main routine of the app. We block waiting for the indy
 * to send us a command block. Once we get that command we do something,
 * like print an answer, get a block of data, or send a block of data.
 */
static void
mainproc(void *arg)
{
    int i;

    osReadHost(ReadBlockBuf,sizeof(ReadBlockBuf));
    osWriteHost(ReadBlockBuf,sizeof(ReadBlockBuf));

    for(i=0;i<100;++i){
	osReadHost(ReadBlockBuf,10);
	osWriteHost(ReadBlockBuf,10);
    }
    for(;;) ;
}
