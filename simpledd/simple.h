/*
 * simple.h
 */

/**********************************************************************
 This source code modified by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Modifications:
	Modified to do EEPROM stuff-- slightly buggy
	Modified to conditionally work with DD
 *********************************************************************/

#ifndef __simple__
#define __simple__

#define	STACKSIZEBYTES	0x2000

#define	STATIC_SEGMENT		1
#define	DYNAMIC_SEGMENT		2

#ifdef _LANGUAGE_C

#include <ultra64.h>
#include "gfx.h"
#ifdef DD64
#include <leo.h>
#include "dd.h"
#else
#include "cart.h"
#include "eeprom.h"
#endif

#define MAX_MESGS	16
#define DMA_QUEUE_SIZE  200
#define NUM_FIELDS      1       /* 1 = 60 Hz video interrupts, 2 = 30 Hz, etc */

#define UTIL_PRIORITY		1
#define INIT_PRIORITY		10
#define GAME_PRIORITY		10
#define AUDIO_PRIORITY		12
#define EEPROM_PRIORITY		13
#define SCHEDULER_PRIORITY	15

#define LOG_SCHEDULE_GFX_TASK	    101
#define LOG_RDP_DONE		    102
#define LOG_RETRACE		    103
#define LOG_INTR		    104

/* define a message after the set used by the scheduler */
#define SIMPLE_CONTROLLER_MSG	    (OS_SC_LAST_MSG+1)

#ifdef _FINALROM
/* cc will give warnings about argument mismatch in rom version */
#define PRINTF 
#else
#define PRINTF osSyncPrintf
#endif

extern OSMesgQueue      gfxFrameMsgQ;
extern OSMesg           gfxFrameMsgBuf[MAX_MESGS];
extern OSMesgQueue      siMsgQ;
extern OSMesg           siMsgBuf[MAX_MESGS];

#ifndef _FINALROM
extern OSLog *log;
extern int logging;
#endif

extern char _gfxdlistsSegmentStart[], _gfxdlistsSegmentEnd[];
#ifndef DD64
extern char _staticSegmentRomStart[], _staticSegmentRomEnd[];
extern char _tableSegmentRomStart[], _tableSegmentRomEnd[];
extern char _seqSegmentRomStart[], _seqSegmentRomEnd[];
extern char _bankSegmentRomStart[], _bankSegmentRomEnd[];
extern char _fontSegmentRomStart[], _fontSegmentRomEnd[];
extern char _fontSegmentStart[];
#else
#ifdef PARTNER_PC
extern short _staticSegmentDiskStart, _staticSegmentDiskEnd;
extern short _tableSegmentDiskStart, _tableSegmentDiskEnd;
extern short _seqSegmentDiskStart, _seqSegmentDiskEnd;
extern short _bankSegmentDiskStart, _bankSegmentDiskEnd;
#else
extern char _staticSegmentDiskStart[], _staticSegmentDiskEnd[];
extern char _tableSegmentDiskStart[], _tableSegmentDiskEnd[];
extern char _seqSegmentDiskStart[], _seqSegmentDiskEnd[];
extern char _bankSegmentDiskStart[], _bankSegmentDiskEnd[];
#endif
#endif

extern OSMesgQueue      dmaMessageQ;
extern OSMesg           dmaMessageBuf;
extern OSMesg           PiMessages[DMA_QUEUE_SIZE];
extern OSMesgQueue      PiMessageQ;

void     initAudio(void);
void     initGFX(void);
void     initCntrl(void);
void     createGfxTask(GFXInfo *i);
void     UpdateController(OSMesgQueue*);

#ifndef DD64
extern u8 myFont[192][128]; /* This font used when no DD exists. */
extern int myFontDx[];
extern int myFontDy[];
extern int myFontCy[];
#endif
void doFont(void);

#define DM_INVISIBLE 0
#define DM_VISIBLE 1

#define DM_READ 0
#define DM_WRITE 1
#define DM_GOAWAY 2
#define DM_NUMITEMS 3

extern int DataMenu, DataMenuPos;

extern int ControllerOperating;

#ifdef DD64
#define dataFullRead ddFullRead
#define dataFullWrite ddFullWrite
#define dataBuffer ddBuffer
#else
#define dataFullRead eepromFullRead
#define dataFullWrite eepromFullWrite
#define dataBuffer eepromBuffer
#endif

#define DATA_BUFFER_SIZE 512


extern OSScMsg       controllermsg;

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#endif /* _LANGUAGE_C */

#endif


