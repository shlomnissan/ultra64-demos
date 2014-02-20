/*---------------------------------------------------------------------
	Copyright (C) 1997, Nintendo.
	
	File		system.h
	Coded    by	Yoshitaka Yasumoto.	Feb  3, 1997.
	Modified by	
	Comments	
	
	$Id: system.h,v 1.6 1997/08/07 03:24:23 yasu Exp $
  ---------------------------------------------------------------------*/
#define	SCREEN_WD	320
#define	SCREEN_HT	240

#define	RDPFIFO_SIZE	(4096/sizeof(u64))
#define	GLIST_LEN	512

#define	SP_BOOT_UCODE_SIZE	0x00d0	/* The true size is 0x00d0. */

#define	NUM_PI_MSGS	8		/* The PI message queue.         */
#define	STACKSIZE	0x2000		/* The stack size of the thread. */

#define	STATIC_SEGMENT	1
#define	BG_SEGMENT	2
#define	CFB_ADDRESS	0x80300000
#if 1
#define	RSPBUF_ADDRESS	0x803a0000	/* Right after cfb if there is no define. */
#endif

#ifdef _LANGUAGE_C
extern	OSMesgQueue	piMessageQ;
extern	OSMesgQueue	siMessageQ;
extern	OSMesgQueue	dmaMessageQ;
extern	OSIoMesg	dmaIOMessageBuf;
extern	OSMesgQueue	rspMessageQ;
extern	OSMesgQueue	rdpMessageQ;
extern	OSMesgQueue	retraceMessageQ;
extern	OSContStatus	contStatus[MAXCONTROLLERS];
extern	OSContPad	contPad[MAXCONTROLLERS];
extern	u8		contExist;

extern	u16		system_cfb[][SCREEN_WD*SCREEN_HT];
extern	u64		system_rdpfifo[];
extern	u64		system_rspyield[];

/*
 *  The macro for the declaration of extern of the WAVE data segment.
 */
#define	EXTERN_SEGMENT(name)	\
extern	char	_##name##SegmentStart[], _##name##SegmentEnd[], \
                _##name##SegmentRomStart[], _##name##SegmentRomEnd[]

/*
 *  The trigger macro of the segment load.
 */
#define	LoadSegment(name,adrs)		\
osPiStartDma(&dmaIOMessageBuf, OS_MESG_PRI_NORMAL, OS_READ, \
	     (u32)_##name##SegmentRomStart, adrs, \
	     (u32)_##name##SegmentRomEnd - (u32)_##name##SegmentRomStart, \
	     &dmaMessageQ)
#define	LoadSegmentAfter(name,after)	LoadSegment(name,_##after##SegmentEnd)

/*
 *  The extern declaration of the segment.
 */
EXTERN_SEGMENT(code);
EXTERN_SEGMENT(static);
EXTERN_SEGMENT(bg_rgba);
EXTERN_SEGMENT(bg_ci);

/*
 *  The print macro for DEBUG.
 */
#ifdef	DEBUG
#  define	osSyncPrintf	osSyncPrintf
#else
#  define	osSyncPrintf	if (0) osSyncPrintf
#endif

#endif	/* _LANGUAGE_C */

/*======== End of system.h ========*/
