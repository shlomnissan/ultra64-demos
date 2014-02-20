#include	<ultra64.h>

#include	"thread.h"

#define STACKSIZE	0x3000

/*
 *		BOOT section
 */
	u64		bootStack[STACKSIZE/8];

/*
 *		IDLE thread
 */
static	void		idle(void *);
static	OSThread	idleThread;
static	u64		idleThreadStack[STACKSIZE/8];

/*
 *		MAIN thread
 */
extern	void		mainproc(void *);
static	OSThread	mainThread;
static	u64		mainThreadStack[STACKSIZE/8];

/*
 *		RMON thread
 */
#ifdef	_DEBUG
static	OSThread	rmonThread;
static	u64		rmonThreadStack[RMON_STACKSIZE/8];
#endif

/*
 *		Pi message queue
 */
#define	NUM_PI_MSGS     8
static	OSMesg		PiMessages[NUM_PI_MSGS];
static	OSMesgQueue	PiMessageQ;

/*
 *		Other message queues
 */
OSMesgQueue	retraceMessageQ;
OSMesg		retraceMessageBuf;

/*---------------------------------------------------------------------*
 *	IDLE THREAD (Pri.= 0)
 *---------------------------------------------------------------------*/
static void	idle(void *arg)
{
  /*
   *		Initialize Vi manager
   */
  osCreateViManager(OS_PRIORITY_VIMGR);
  osViSetMode(&osViModeTable[OS_VI_NTSC_HPF1]);
  
  /*
   *		Initialize Pi manager
   */
  osCreatePiManager((OSPri)OS_PRIORITY_PIMGR,
		    &PiMessageQ, PiMessages, NUM_PI_MSGS);
  
#ifdef	_DEBUG
  /*
   *		Create & start RMON thread
   */
  osCreateThread(&rmonThread, 0, rmonMain, (void *)0,
		 (void *)(rmonThreadStack+RMON_STACKSIZE/8),
		 (OSPri)OS_PRIORITY_RMON );
  osStartThread(&rmonThread);
#endif
  
  /*
   *		Setup message queue
   */
  osCreateMesgQueue(&retraceMessageQ, &retraceMessageBuf, 1);
  osViSetEvent(&retraceMessageQ, NULL, 1);			/* retrace */

  /* 
   *		Create & start MAINPROC thread
   */  
  osCreateThread(&mainThread, TID_MAINPROC, mainproc, (void *)0,
		 (void *)(mainThreadStack+STACKSIZE/8), 10);
  
#ifndef	DEBUG
  osStartThread(&mainThread);
#endif
  
  /*
   *		Now idling
   */
  osSetThreadPri(0, 0);
  for(;;);
}

/*---------------------------------------------------------------------*
 *	BOOT PART
 *---------------------------------------------------------------------*/
 void boot(void)
{  
  /*
   *		Initialize OS
   */

  osInitialize();
  
  /*
   *		Create idle thread & start it
   */
  osCreateThread(&idleThread, TID_IDLE, idle, (void*)0,
		 idleThreadStack+STACKSIZE/8, 10);
  osStartThread(&idleThread);
}
