/*============================================================================

		NINTENDO64 TECHNICAL SUPPORT CENTER 
		
		    NINTENDO64 SAMPLE PROGRAM 1

		Copyright (C) 1997, NINTENDO Co., Ltd.

============================================================================*/
#ifndef _AUDIO_H_
#define _AUDIO_H_

/*------------------------- debug flag ----------------------------*/

/*
  Debug flag definitions
  If debug flags are needed please define them.  Otherwise, undefine them.
*/
#define _AUDIO_DEBUG_           /* debug flag*/

#ifdef _AUDIO_DEBUG_
/*
  If the following flags are to be used, then the _AUDIO_DEBUG_ flag must be made valid.
*/
#undef _AUDIO_DEBUG_DMA_       /* Displays DMA start number */
#undef _AUDIO_DEBUG_PROC_      /* Displays CPU duty factor */

#endif /* _AUDIO_DEBUG */
/*------------------------- define --------------------------------*/
/*
  Please use this flag if compact MIDI format data is being used.  
  Undef if Type () MIDI sequence player is being used.
*/

#undef  _AUDIO_COMPACTMIDI_

/*
  These parameters must be suitably changed.  If not they will not operate well.
*/
#define AUDIO_HEAP_SIZE          500000  /* heap region size */
#define AUDIO_CLIST_SIZE_MAX     5000    /* command list buffer size */
#define AUDIO_BUFFER_MAX         0x2000  /* synthesizer buffer size */
#define AUDIO_OUTPUT_RATE        44100   /* set frequency */
#define AUDIO_VVOICE_MAX         64      /* maximum virtual voice number */
#define AUDIO_PVOICE_MAX         32      /* maximum physical voice number */
#define AUDIO_UPDATE_MAX         64      /* maximum number of synthesizer updates */

/* set sequence player parameters */
#define AUDIO_SEQ_VVOICE_MAX     64      /* maximum virtual voice number */
#define AUDIO_SEQ_EVTCOUNT_MAX   64      /* event number */
#define AUDIO_SEQ_CHANNEL_MAX    16      /* MIDI channel number (normally 16) */

/* set sound player parameters */
#define AUDIO_SND_VOICE_MAX      32      /* maximum sound number */  
#define AUDIO_SND_EVTCOUNT_MAX   64      /*maximum event number */

#define AUDIO_MESGS_MAX          8

/*

   Sequence data buffers

Set the maximum value among sequence data files.
Fixedly allocate data buffers.
If necessary, change to automatic allocation.
 */
#define AUDIO_SEQDATA_SIZE_MAX   0x10000

#define AUDIO_EXTRA_SAMPLES      80

/*
 Make use of DMA buffers for sequence player like cache, deleting when past the AUDIO_BUFFER_USE_FRAME number.  The actual DMAbuffer number and size required differs depending on the software, so a check is necessary.
   
*/
#define AUDIO_DMA_BUFFER_NUM    32        /* DMA buffer number" */
#define AUDIO_DMA_BUFFER_SIZE   2048      /* DMA buffer size */
#define AUDIO_DMA_QUEUE_SIZE    32        /* DMA message queue size */
#define AUDIO_BUFFER_USE_FRAME  2         /* DMABuffer frame interval to clear */


#define AUDIO_STACKSIZE        0x2000         /* audio thread stack size */
#define AUDIO_THREAD_PRI       50
#define AUDIO_THREAD_ID        5

#define AUDIO_NUM_FIELDS       1             /* retrace interval  (1 or 2)*/

/*------------------------- values --------------------------------*/
/* sequence player structure  */
typedef struct {
  ALSeqpConfig seqconfig;
#ifdef _AUDIO_COMPACTMIDI_
  ALCSPlayer  seqplayer;     /* sequence player structure */
  ALCSeq       sequence;
#else
  ALSeqPlayer seqplayer;      /* sequence player structure */
  ALSeq       sequence;
#endif /*_AUDIO_COMPACTMIDI_ */

  u8  *seqdata_ptr;           /* sequence data */
  u32 seqno;                  /* sequence No. */
  u32 state;                  /*  sequence player state */
} Audio_seqplayer;

extern Audio_seqplayer seqplayer[2];

/*------------------------- functions --------------------------------*/
void auRomRead(u32 , void* , u32);
extern void auAudioInit(void);
extern void auReadMidiData(u32, u32);
extern void auReadSoundData(u32, u32);
extern void auReadSeqFileHeader(u32);
extern void auReadSeqFile(s32);
extern void auCleanDMABuffers(void);
extern void auCreateAudioThread(NNSched* );
extern void auSeqPlayerInit(u8* , u8*, u8*, u8*, u8*);
extern void auSeqPlayerSetFile(u32,s32);
extern int  auSeqPlayerPlay(u32);
extern s32  auSeqPlayerState(u32);
extern void auSeqPlayerStop(u32);
extern void auSndPlayerInit(u8* ,u8* ,u8*);
extern void auSndPlay(u32);
extern void auSndStop(u32);
extern s32  auSndGetState(u32);
#endif /* _AUDIO_H_ */









