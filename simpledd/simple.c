/**********************************************************************
 * simple.c
 *
 * simple - Basic example of an Ultra64 application including:
 *     Multiprocessing between host and RSP
 *     Graphics
 *     Multiple controllers for user input
 *     Audio, both music and sound effects
 *
 * simple works by having several threads of various priority.
 *     rmon thread. Technically not part of the game. Used for debugging.
 *          Its main purpose is to send printf's back to host machine.
 *          A final version of the game wouldn't used this thread.
 *     scheduler thread. The highest priority thread of the game. Its 
 *          function is to see that audio and graphics tasks get built
 *          and executed in a timely fashion.
 *     audio thread. The second highest thread. Creates the audio task
 *          lists to be executed by the audio microcode to synthesize 
 *          the audio. Audio should run at a higher priority than 
 *          graphics, since if a frame of audio gets dropped, it will
 *          cause a serious click, but a frame of graphics can always 
 *          redraw the last frame.
 *     game thread. Loops waiting for messages from the scheduler. 
 *          Upon receiving retrace messages, will branch to the graphics 
 *          routine and the controller read routine.
 *     idle thread. Starts out as the init thread, but becomes the idle
 *          thread. Every game needs one of these, don't get rid of it.
 *
 * Copyright 1995, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics,
 * Inc.; the contents of this file may not be disclosed to third
 * parties, copied or duplicated in any form, in whole or in part,
 * without the prior written permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to
 * restrictions as set forth in subdivision (c)(1)(ii) of the Rights
 * in Technical Data and Computer Software clause at DFARS
 * 252.227-7013, and/or in similar or successor clauses in the FAR,
 * DOD or NASA FAR Supplement. Unpublished - rights reserved under the
 * Copyright Laws of the United States.
 **********************************************************************/

/**********************************************************************
 This source code modified by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Modifications:
	Modified to deal with EPI setup for DD ROM images.
	Modified to handle EEPROM use.
 *********************************************************************/


#include <ultra64.h>
#include <sched.h>
#include "misc.h"
#include "simple.h"
#include "audio.h"
#include "gfx.h"

/**** threads used by this file ****/
static OSThread gameThread;
static OSThread initThread;

/**** Stack for boot code.  Space can be reused after 1st thread starts ****/
u64    bootStack[STACKSIZEBYTES/sizeof(u64)];

/**** Stacks for the threads, divide by 8 which is the size of a u64 ****/
static u64      gameThreadStack[STACKSIZEBYTES/sizeof(u64)];
static u64      initThreadStack[STACKSIZEBYTES/sizeof(u64)];

/**** threads and stack used by rmon ****/
#ifndef _FINALROM
static OSThread rmonThread;
static u64      rmonStack[RMON_STACKSIZE/sizeof(u64)];
#endif

/**** function prototypes for private functions in this file ****/
static void     gameproc(void *);
static void     initproc(char *);
static void     initGame(void);

/**** message queues and message buffers used by this app ****/
OSMesgQueue      dmaMessageQ;
OSMesg           dmaMessageBuf;

/**** Graphics variables used in this file ****/
OSMesgQueue     gfxFrameMsgQ;
OSMesg          gfxFrameMsgBuf[MAX_MESGS];
Gfx             *glistp;
GFXInfo         gInfo[2];

/* Serial message queue stuff */
OSMesgQueue     siMsgQ;
OSMesg          siMsgBuf[MAX_MESGS];

/**** Scheduler globals ****/
OSSched         sc;
OSMesgQueue	*sched_cmdQ;
u64             scheduleStack[OS_SC_STACKSIZE/8];
OSScClient      gfxClient;

/**** Controller globals ****/
extern u32      validcontrollers;

OSTime  lastTime;

#ifndef _FINALROM
u8      rdbSendBuf[2048];

/**** logging stuff, used for debugging only. ****/
#include <ultralog.h>
#define LOG_LEN 0x8000
OSLog logger;
OSLog *log = &logger;
u32 logData[LOG_LEN];
#endif

/**********************************************************************
 *
 * boot is the equivalent of main(). This is where your application will
 * start its execution. This thread should  be kept very simple, and 
 * should start another thread that will ultimately become the idle 
 * thread. Execution will never return from the first call of 
 * osStartThread. In this example, we parse commmand line arguments 
 * passed from gload. In a real application you won't be able to do this.
 *
 **********************************************************************/
void boot(void *arg)
{
    u32    i;
    u32    *argp;
    u32    argbuf[16];
    
    osInitialize();

#ifndef _FINALROM
    argp = (u32 *)RAMROM_APP_WRITE_ADDR;
    for (i=0; i < sizeof(argbuf)/4; i++, argp++) 
        osPiRawReadIo((u32)argp, &argbuf[i]);   /* Assume no DMA */

    parse_args((char *)argbuf);

#else

    parse_args(NULL);

#endif
    
    
    osCreateThread(&initThread, 1, (void(*)(void *))initproc, arg,
                  (void *)(initThreadStack+(STACKSIZEBYTES/sizeof(u64))), 
		   (OSPri)INIT_PRIORITY);

    osStartThread(&initThread);
}

/**********************************************************************
 *
 * initproc sets up the PI manager, and the rmon thread. It then creates
 * and starts our application's game thread. After returning from that,
 * it becomes the lowest priority thread, and functions as the idle thread.
 * Starting the rmonThread is for debugging purposes only.
 *
 **********************************************************************/
static void initproc(char *argv) 
{
#ifdef DD64
    ddInitEPI();
#else
    cartInitPI();
#endif 

#ifndef _FINALROM
    osInitRdb(rdbSendBuf,sizeof(rdbSendBuf));
    /**** Start rmonThread so you can do printf's ****/
    osCreateThread(&rmonThread, 0, rmonMain, (void *)0,
                   (void *)(rmonStack+(RMON_STACKSIZE/sizeof(u64))),
                   (OSPri) OS_PRIORITY_RMON );
    osStartThread(&rmonThread);
#endif

    /**** Create the game thread and start it up ****/
    osCreateThread(&gameThread, 6, gameproc, argv, gameThreadStack + 
		   (STACKSIZEBYTES/sizeof(u64)), (OSPri)GAME_PRIORITY);

    if (!debugger) /* set by command line to gload, when you want to use gvd. */
        osStartThread(&gameThread);

    /**** Set the thread to be the idle thread ****/
    osSetThreadPri(0, 0);
    for(;;);
}

/**********************************************************************
 *
 * A continual loop, primarily used for servicing the starts of graphic 
 * tasks and controller reads. Audio is serviced by the audio thread. 
 * You receive several message on the same queue, they are:
 *    OS_SC_RETRACE_MSG: this comes from the scheduler every retrace. 
 *         If you don't already have 2 graphics tasks either pending or 
 *         processing, start creating another.
 *    OS_SC_DONE_MSG: sent by the scheduler when a graphics task has 
 *         completed. 
 *    OS_SC_PRE_NMI_MSG: sent to indicate the reset button has been 
 *         pressed, and that the game will reboot momentarily. Might 
 *         want to fade or say goodbye.
 *
 **********************************************************************/
static void gameproc(void *argv)
{
    u32         drawbuffer = 0;
    u32         pendingGFX = 0;
    GFXMsg      *msg = NULL;
#ifndef _FINALROM
    u32         doneTask = 0;
#endif

    initGame();

    while (1) {
        (void) osRecvMesg(&gfxFrameMsgQ, (OSMesg *)&msg, OS_MESG_BLOCK);

        switch (msg->gen.type) {
            case (OS_SC_RETRACE_MSG):
#ifndef _FINALROM
                if (logging)
                    osLogEvent(log, LOG_RETRACE, 1, pendingGFX);
#endif

                /**** Create a new gfx task unless we already have 2  ****/                 
                if (pendingGFX < 2) 
                {
                    createGfxTask(&gInfo[drawbuffer]);
                    pendingGFX++;
                    drawbuffer ^= 1; /* switch the drawbuffer */
                }

                /* request latest controller information (ie poll) */
                if (validcontrollers && !ControllerOperating 
#ifndef DD64
					&& !EepromAccessing
#endif
				) {
		    		ControllerOperating = 1;
                    osContStartReadData(&siMsgQ);
                }
                break;

            case (OS_SC_DONE_MSG):
#ifndef _FINALROM
				lastTime = gInfo[doneTask].task.totalTime;
				doneTask ^= 1;
#endif
                pendingGFX--;        /* decrement number of pending tasks */
                break;
                
                
            case (OS_SC_PRE_NMI_MSG): /* stop creation of graphics tasks */
                pendingGFX += 2;
                break;
        }

        if (osRecvMesg(&siMsgQ, (OSMesg *)&msg, OS_MESG_NOBLOCK) == 0) {
#ifndef DD64
				if (!EepromAccessing) 
#endif
				{
					UpdateController(&siMsgQ);
					ControllerOperating = 0;
				}
	    }
    }
}

/**********************************************************************
 *
 * initGame sets up the message queues used, and starts the scheduler.
 * After that call routines to init the graphics, init the controllers,
 * and init the audio.
 *
 *********************************************************************/
static void initGame(void)
{    
#ifndef _FINALROM
    if (logging)
        osCreateLog(log, logData, LOG_LEN);
#endif

    /**** set up a needed message q's ****/
    osCreateMesgQueue(&dmaMessageQ, &dmaMessageBuf, 1);
    osCreateMesgQueue(&gfxFrameMsgQ, gfxFrameMsgBuf, MAX_MESGS);
    osCreateMesgQueue(&siMsgQ, siMsgBuf, MAX_MESGS);

#ifdef DD64
    ddInit();
#endif

    /**** Initialize the RCP task scheduler ****/
    osCreateScheduler(&sc, (void *)(scheduleStack + OS_SC_STACKSIZE/8),
                      SCHEDULER_PRIORITY, OS_VI_NTSC_LAN1, NUM_FIELDS);

    /**** Add ourselves to the scheduler to receive retrace messages ****/
    osScAddClient(&sc, &gfxClient, &gfxFrameMsgQ);  

    sched_cmdQ = osScGetCmdQ(&sc);

    /**** Call the initialization routines ****/
    initGFX(); 
    initCntrl();
    initAudio();
}

