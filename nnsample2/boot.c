/*============================================================================

		NINTENDO64 TECHNICAL SUPPORT CENTER 
		
		    NINTENDO64 SAMPLE PROGRAM 2

		Copyright (C) 1997, NINTENDO Co., Ltd.

============================================================================*/
#include <ultra64.h>
#include <PR/ramrom.h>
#include <assert.h>

#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"

u64 bootStack[STACKSIZE/sizeof(u64)];
static void idle(void *);      /* Idle Thead */

/* define thread structure */
static OSThread idleThread;
static OSThread mainThread;

#ifdef USE_RMON
static OSThread rmonThread;
#endif /* USE_RMON */


/* define thread stack */
static u64 idleThreadStack[STACKSIZE/sizeof(u64)];
static u64 mainThreadStack[STACKSIZE/sizeof(u64)];
#ifdef USE_RMON
static u64 rmonThreadStack[STACKSIZE/sizeof(u64)];
#endif /* USE_RMON */

/* external function */
extern void mainproc(void *);

/*-----------------------
  Boot                
  Program start here   
-----------------------*/
void boot(void)
{
  /* initilize software and hardware */
  osInitialize();

  /* Source for PARTNER-N64 */
  /* ptstart(); */ 

  /* create and start idle thread */
  osCreateThread(&idleThread,IDLE_THREAD_ID, idle, (void *)0,
		 (idleThreadStack+STACKSIZE/sizeof(u64)), IDLE_THREAD_PRI);
  osStartThread(&idleThread);
}

/*--------------------------
  Idle Thread
---------------------------*/
static void idle(void *arg)
{
  /* create and start PI manager thread */
  osCreatePiManager((OSPri)OS_PRIORITY_PIMGR, &PiMessageQ, PiMessages,
		    NUM_PI_MSGS);


/* please remove this comment if rmon is used */

#ifdef USE_RMON
  osCreateThread(&rmonThread, 0, rmonMain, (void *)0,
                   (void *)(rmonThreadStack+RMON_STACKSIZE/8), 
                   (OSPri) OS_PRIORITY_RMON );
  osStartThread(&rmonThread);
#endif /* USE_RMON */

  /* start main thread */
  osCreateThread(&mainThread, MAIN_THREAD_ID, mainproc,arg,
		 (mainThreadStack+STACKSIZE/sizeof(u64)),MAIN_THREAD_PRI);
  osStartThread(&mainThread);

  /* Here there is an idle loop */
  for(;;);
}




