/*============================================================================

		NINTENDO64 TECHNICAL SUPPORT CENTER 
		
		    NINTENDO64 SAMPLE PROGRAM 1

		Copyright (C) 1997, NINTENDO Co., Ltd.

============================================================================*/
/*============================================================================
  audio.c

  Ver0.1  1996/11/15
	Created as sample for NTSC scheduler.
  Ver0.1a 1996/11/19
 	Fixed patchy sound.
	Fixed error when compiling for TYPE0 MIDI.
  Ver0.2  1996/12/25
  	Added auSeqPlayerState function.
	Added auSeqPlayerStop function.
	Added PRE NMI message process to audioProc function.
Fixed fadeout of PRE NMI message.
Changed comments to SJIS.

  <!!!USE SJIS CODE!!!>


     This library is for use with the scheduler (NN Scheduler) developed by the Nintendo Technical Support Center .  Various changes must be made to use this library with another scheduler.

     The library still contains only the minimum set of functions.  Please create other necessary functions for yourself. 
     Also, the parameters are not properly adjusted, so please change the audio.h file to adjust parameters.

     You are free to use the source code, but please send us feedback concerning any bugs.



(1)  Specifications

	Two sequence players are created.  In other words, two melodies can be played at the same time.  However, they will stop if the specified maximum number of sounds is exceeded, so be careful.

(2)  Method of use
1.     Call auAudioInit()and initialize the parameters to start the synthesizer.
2.  Call auSeqplayerInit() and set the necessary data in the sequence player.
        Call  auSndPlayerInit()and set the necessary data in the sound player
3.  Start the scheduler with nnCreateScheduler() and then start the audio thread with auCreateAudioThread()
4.  To playback a sequence, use auSeqPlayerSetFile() to read the sequence file specified by the sequence player and file number.   Playback using auSeqPlayerPlay().
                        To play a sound, playback sound number specified by auSndPlay(sound number).
 

============================================================================*/
#include <ultra64.h>
#include <PR/ramrom.h>
#include <assert.h>
#include "nnsched.h"
#include "audio.h"


ALHeap audio_heap;                          /* audio heap structur */
u8     audio_heap_buf[AUDIO_HEAP_SIZE];     /* audio heap buffer  */
Acmd   *audio_cmdlist_ptr[2];                   /* cmd list    */
OSTask *audio_tlist_ptr[2];                     /* audio task list */
s16    *audio_buffer_ptr[3];                    /* audio output buffer */

ALBank *soundBank_ptr = NULL;
ALBankFile *midi_buffer_ptr = NULL;             /* seq midi bank data */
ALBankFile *snd_buffer_ptr = NULL;            /* sound bank data */
ALSeqFile  *seqHeaderfile_ptr = NULL;           /* seqence header file */
OSMesgQueue audioDmaMessageQ;
OSMesgQueue audioRomMessageQ;
OSMesg      audioDmaMessageBuf;
OSMesg      audioRomMessageBuf;
OSIoMesg    audioDmaIOMesgBuf[AUDIO_DMA_QUEUE_SIZE];
s32         nextDMA = 0;

static OSThread audioThread;
static u64      audioThreadStack[AUDIO_STACKSIZE/sizeof(u64)];

ALGlobals    audio_global;
ALSynConfig  audio_config;

/*-----sound player -----*/
ALSndPlayer   sndplayer;
ALSndpConfig  sndplayer_config;
ALInstrument* snd_inst;
ALBank        *snd_bank_ptr;
ALSndId       *snd_id;
u32           snd_alloc_num;

/* DMA buffer used by the sequence player DMA callback routine       */
typedef struct {
    ALLink      node;        /* link list pointer */
    int         startAddr;   /* address of data in ROM */
    u32         lastFrame;   /* value of last frame counter used */
    char        *ptr;        /* value of buffer pointer */
} DMABuffer;

/* DMA status used by the sequence player DMA callback routine */
/* Structure storing the DMABuffer usage state */
typedef struct {
    u8          initialized;  /* flag whether already initialized */
    DMABuffer   *firstUsed;   /* Start of DMABuffer link list now in use */
    DMABuffer   *firstFree;   /* Start of free DMABuffer */

} DMAState;

Audio_seqplayer seqplayer[2];
DMAState    dmaState;
DMABuffer   dmaBuffs[AUDIO_DMA_BUFFER_NUM];
u32         audio_framecnt = 0;
ALDMAproc dmaNew(DMAState **);


#ifdef  _AUDIO_DEBUG_DMA_
static u32 dma_frame_max = 0;   /*  DMA callback maximum value */
static u32 dma_call_cnt =0;     /* DMA callback call count  */
static u32 dma_use_cnt =0;      /* Actual number of times DMA used */
#endif /* _AUDIO_DEBUG_DMA_


/*-----------------------------------------------------------------------------
  DMA transfer (used with audio library )
  u32   src_addr      :   read origin cassette address  
  void* dest_addr  :  output to memory address
  u32   size               :   transfer size
-----------------------------------------------------------------------------*/
void auRomRead(u32 src_addr, void* dest_addr, u32 size)
{
  OSIoMesg    DmaIoMessageBuf;
  
  /* Invalidate CPU cache */
  osInvalDCache((void*)dest_addr, (s32)size);

  /* start DMA read */
  osPiStartDma(&DmaIoMessageBuf, OS_MESG_PRI_NORMAL, OS_READ, src_addr,
	       dest_addr, size, &audioRomMessageQ);

  /* wait DMA read end */
  (void)osRecvMesg(&audioRomMessageQ, NULL, OS_MESG_BLOCK);

}


/*-----------------------------------------------------------------------------
  Initialize the audio library

  Buffer allocation and initialization of parameters used by audio library.
  Called here first.
-----------------------------------------------------------------------------*/
void auAudioInit(void)
{

  /*initialize audio heap */
  alHeapInit(&audio_heap, audio_heap_buf, AUDIO_HEAP_SIZE);
  
  /* create command list buffer */
  audio_cmdlist_ptr[0] = alHeapAlloc(&audio_heap, 1,
				     AUDIO_CLIST_SIZE_MAX*sizeof(Acmd));
  audio_cmdlist_ptr[1] = alHeapAlloc(&audio_heap, 1,
				     AUDIO_CLIST_SIZE_MAX*sizeof(Acmd));
  
  /* create task list buffer */
  audio_tlist_ptr[0] = alHeapAlloc(&audio_heap, 1, sizeof(OSTask));
  audio_tlist_ptr[1] = alHeapAlloc(&audio_heap, 1, sizeof(OSTask));

  /* create audio buffer */
  audio_buffer_ptr[0] = alHeapAlloc(&audio_heap, 1,
				    sizeof(s32)*AUDIO_BUFFER_MAX);
  audio_buffer_ptr[1] = alHeapAlloc(&audio_heap, 1,
				    sizeof(s32)*AUDIO_BUFFER_MAX);
  audio_buffer_ptr[2] = alHeapAlloc(&audio_heap, 1,
				    sizeof(s32)*AUDIO_BUFFER_MAX);


  /* create message queue for DMA */
  osCreateMesgQueue(&audioDmaMessageQ, &audioDmaMessageBuf, 1);
  osCreateMesgQueue(&audioRomMessageQ, &audioRomMessageBuf, 1);

  /* Clear  DMABuffer  initialization flag */
  dmaState.initialized = 0;

  /* initialize audio library */
  audio_config.outputRate = osAiSetFrequency(AUDIO_OUTPUT_RATE);
  audio_config.maxVVoices = AUDIO_VVOICE_MAX;  /* maximum virtual voice number */
  audio_config.maxPVoices = AUDIO_PVOICE_MAX;  /* maximum physical voice number */
  audio_config.maxUpdates = AUDIO_UPDATE_MAX;  
  audio_config.dmaproc    = &dmaNew;            /* DMA callback function */
  audio_config.fxType     = AL_FX_SMALLROOM;    /* Effect type */
  audio_config.heap       = &audio_heap;        /* audio heap */
  audio_config.params     = 0;                  /* Custom effect */
  alInit(&audio_global, &audio_config);
}



/*---------------------------------------------------------------

   DMA CallBack for sequence player 

   DMA callback routine called by sequence player

	If there is a needed value in DMABuffer, the value in that buffer is returned, otherwise DMA reads data into the buffer.  This contrivance lowers the DMA call count.  However, the unused portion is eliminated from 2 frames.  This control is most effective changed to LIFO like in CPU cache.
---------------------------------------------------------------*/

	


s32 dmaCallBack(s32 addr, s32 len, void *state)
{
  void        *freeBuffer;
  int         delta;
  DMABuffer   *dmaPtr,*lastDmaPtr;
  s32         addrEnd,buffEnd;


  lastDmaPtr = 0;
  dmaPtr = dmaState.firstUsed;
  addrEnd = addr+len;

#ifdef _AUDIO_DEBUG_DMA_
  dma_call_cnt++;
#endif /* _AUDIO_DEBUG_DMA_ */

  while(dmaPtr) {  /* Check if wanted data is in buffer */

      buffEnd = dmaPtr->startAddr + AUDIO_DMA_BUFFER_SIZE;
      
      if(dmaPtr->startAddr > addr) {/* startAddr is sorted in ascending order in the DMABuffer,
		                            so if a value smaller than the preceding value comes up during
			            a search, the desired value will not be in the buffer */
	break;                   
      } else if(addrEnd <= buffEnd){  /* Data is in buffer */
	dmaPtr->lastFrame = audio_framecnt;  /* Set value of current frame count */
     
	/* Calculate address of  really necessary data */
	freeBuffer = dmaPtr->ptr + addr - dmaPtr->startAddr;

	/* Return physical address of buffer */
	return (int) osVirtualToPhysical(freeBuffer);
      }
      lastDmaPtr = dmaPtr;
      dmaPtr = (DMABuffer*)dmaPtr->node.next;
    }

  /*
     Since no wanted data is in DMABuffer, set for transfer of data by DMA.
  */

  /* Get 1 free buffer from DMABuffer  */
  dmaPtr = dmaState.firstFree;
  
  assert(dmaPtr); /* Check if there is a buffer */
  
  /* NULL fetched if there is no free buffer */
  /* Pointer to start of buffer now in use is returned */
  /*  This is better than stopping */
  if(dmaPtr == NULL){
    return(int)OS_K0_TO_PHYSICAL(dmaState.firstUsed->ptr);
  }

  dmaState.firstFree = (DMABuffer*)dmaPtr->node.next;
  alUnlink((ALLink*)dmaPtr);

  /* 
     Insert DMABuffer in currently used link list such that startAddr is in ascending order.
  */
  if(lastDmaPtr) {

    /* Insert normally in link list */
      alLink((ALLink*)dmaPtr,(ALLink*)lastDmaPtr);

  } else if(dmaState.firstUsed) {

    /* Insert in start of link list */
    lastDmaPtr = dmaState.firstUsed;
    dmaState.firstUsed = dmaPtr;
    dmaPtr->node.next = (ALLink*)lastDmaPtr;
    dmaPtr->node.prev = 0;
    lastDmaPtr->node.prev = (ALLink*)dmaPtr;

  } else {

    /* If there is no DMABuffer during use */
    dmaState.firstUsed = dmaPtr;
    dmaPtr->node.next = 0;
    dmaPtr->node.prev = 0;
  
  }
  
  freeBuffer = dmaPtr->ptr;
  delta = addr & 0x1;
  addr -= delta;            /* 2 byte boundary for DMA-use address location */
  dmaPtr->startAddr = addr;
  dmaPtr->lastFrame = audio_framecnt;  /* set current frame counter */

  /* DMA transfer */
  osPiStartDma(&audioDmaIOMesgBuf[nextDMA++], OS_MESG_PRI_NORMAL, OS_READ,
	       (u32)addr, freeBuffer, AUDIO_DMA_BUFFER_SIZE, &audioDmaMessageQ);

#ifdef _AUDIO_DEBUG_DMA_
  dma_use_cnt++;
#endif /* AUDIO_DEBUG_DMA_ */

  return (int) osVirtualToPhysical(freeBuffer) + delta;
}

/*-------------------------------------------------------------
 
  sequence player DMA callback set

  Callback routine called once for initialization-use at time of registration in sequence player.  After that, nothing is performed in this routine, and it is just used to call the dmaCallBack that is returned.  
---------------------------------------------------------------*/
ALDMAproc dmaNew(DMAState **state)
{
  int         i;

  /* Check if initialized.  If not, then initialize */
  if(!dmaState.initialized) {
    
    dmaState.firstFree = &dmaBuffs[0];    /* Register start of free DMABuffer list*/
    dmaState.firstUsed = NULL;            /* No DMABuffer in use*/

    for (i = 0; i < AUDIO_DMA_BUFFER_NUM - 1; i++){
      alLink((ALLink*)&dmaBuffs[i+1],(ALLink*)&dmaBuffs[i]);
      dmaBuffs[i].ptr = alHeapAlloc(&audio_heap, 1, AUDIO_DMA_BUFFER_SIZE);

    }
    /* Set initialized flag */
    dmaState.initialized = 1;
  }
  *state = &dmaState;  /* state is never used in this case */


  return dmaCallBack;
}

/*-----------------------------------------------------------------

  DMA buffer cleanup

  Search DMABuffer, and if more than 2 frames not being used,
  vacate the buffer and add it to buffer link list.
-----------------------------------------------------------------*/
void auCleanDMABuffers(void)
{
  DMABuffer  *dmaPtr,*nextPtr;
  
  dmaPtr = dmaState.firstUsed;
  while(dmaPtr) {
    nextPtr = (DMABuffer*)dmaPtr->node.next;

    /*
       Vacate buffer and add to buffer link list after passage of 
       more than AUDIO_BUFFER_USE_FRAME number.
       Buffers must be increased to boost the AUDIO_BUFFER_USE_FRAME 
       number.  Reducing the number increases the DMA use count.
    */
    if(dmaPtr->lastFrame + AUDIO_BUFFER_USE_FRAME  < audio_framecnt){
      if(dmaState.firstUsed == dmaPtr){
	dmaState.firstUsed = (DMABuffer*)dmaPtr->node.next;
      }
      alUnlink((ALLink*)dmaPtr);
      if(dmaState.firstFree){
	alLink((ALLink*)dmaPtr,(ALLink*)dmaState.firstFree);
      }else{
	dmaState.firstFree = dmaPtr;
	dmaPtr->node.next = 0;
	dmaPtr->node.prev = 0;
      }
    }
    dmaPtr = nextPtr;
  }
#ifdef _AUDIO_DEBUG_DMA_
 
  if(nextDMA >dma_frame_max){
    dma_frame_max = nextDMA;
  }

#endif /* _AUDIO_DEBUG_DMA_ */

  nextDMA=0;
  audio_framecnt++;  
}


/*------------------------------------------------------

  audio thread 

     Calculate synthesize data with a retrace message from the scheduler, and send message to scheduler to start audio task.  This is created with auCreateAudioThread()‚ so it does not need to be started from outside.
------------------------------------------------------*/

void audioProc(void* arg)
{
  OSMesgQueue  msgQ;
  OSMesg       msgbuf[AUDIO_MESGS_MAX];
  OSMesgQueue  rtn_msgQ;
  OSMesg       rtn_msgbuf[AUDIO_MESGS_MAX];
  NNScClient client;
  Acmd         *cmdlist_ptr;

  static s32   current_buffer = 0;
  static s32   current_audio_buffer = 1;
  static s16   audioSamples[3] = {0,0,0};
  static s32   cmdlist_len;

  s32          current_audio;
  s16*         audioOp;
  s32          frameSize;
  s32          minFrameSize;
  s32          samplesLeft = 0;  
  f32          fsize;
  OSMesgQueue  *sched_audioMQ;
  NNScTask   task[2];
  NNScTask   *au_task;
  short*       msg_type = NULL;
  u32          pre_nmi_flag =0;
  s16          get_seq_vol[2];
  s16          set_seq_vol[2];
  u32          cnt;

#ifdef _AUDIO_DEBUG_DMA_
  f32 hits;
#endif /* _AUDIO_DEBUG_DMA_ */

  fsize = (f32)AUDIO_NUM_FIELDS * audio_config.outputRate/ (f32)60;
  frameSize = (s32) fsize;
  if(frameSize < fsize) frameSize++;
  if(frameSize & 0x0f) frameSize = (frameSize & ~0xf) + 0x10;
  minFrameSize = frameSize - 16;

  /* create message queue for VI retrace */
  osCreateMesgQueue( &msgQ, msgbuf, AUDIO_MESGS_MAX);
  osCreateMesgQueue( &rtn_msgQ, rtn_msgbuf, AUDIO_MESGS_MAX);

  nnScAddClient((NNSched*)arg, &client, &msgQ);
  sched_audioMQ = nnScGetAudioMQ((NNSched*)arg);
  
  while(1){
    (void)osRecvMesg(&msgQ, (OSMesg*)&msg_type, OS_MESG_BLOCK);

    switch(*msg_type){
    case NN_SC_RETRACE_MSG:      /* Process retrace message */ 
      
      /* Fadeout when PRE NMI comes */
      switch(pre_nmi_flag){
      case 1: /* Fadeout */
	for(cnt = 0;cnt < 2; cnt++){

	  /* Within around 0.5 second, so for a little extra make 28frame 0 */
	  set_seq_vol[cnt] -= get_seq_vol[cnt]/28;  
	  if(set_seq_vol[cnt] < 0){
	    set_seq_vol[cnt] = 0;
	  }
#ifdef _AUDIO_COMPACTMIDI_
	  alCSPSetVol(&seqplayer[cnt].seqplayer, set_seq_vol[cnt]);
#else
	  alSeqpSetVol(&seqplayer[cnt].seqplayer, set_seq_vol[cnt]);
#endif /* _AUDIO_COMPACTMIDI_ */
	}
	/*
	   Check for end of fadeout process.  If over do not create task so task will stop.   
	*/
	if((set_seq_vol[0] == 0) && (set_seq_vol[1] == 0)){
	  pre_nmi_flag++;
	}
	break;
      case 2:
	/* End without creating task */
	continue;
	break;
      }
#ifdef _AUDIO_DEBUG_DMA_
      if(!(audio_framecnt % 60)){
	if(dma_call_cnt == 0 ){
	  hits =0;
	} else {
	  hits = (((f32)dma_call_cnt-(f32)dma_use_cnt)*100.0)/(f32)dma_call_cnt;
	}
	osSyncPrintf("dma max = %d /f, dma_call = %d, dma_use = %d, Hits = %f\n",dma_frame_max, dma_call_cnt, dma_use_cnt,hits);
	dma_call_cnt =0;
	dma_use_cnt =0;
      }
#endif /* _AUDIO_DEBGU_DMA_ */
      au_task = &task[current_buffer];
      cmdlist_ptr = audio_cmdlist_ptr[current_buffer];
      current_audio = current_audio_buffer % 3;

      auCleanDMABuffers();
      
      audioOp = (s16*) osVirtualToPhysical(audio_buffer_ptr[current_audio]);
      audioSamples[current_audio] = 16 + 
	(frameSize - samplesLeft + AUDIO_EXTRA_SAMPLES) & ~0x0f;
      if(audioSamples[current_audio] < minFrameSize)
	audioSamples[current_audio] = minFrameSize;
    
      cmdlist_ptr = alAudioFrame(cmdlist_ptr, &cmdlist_len, audioOp, audioSamples[current_audio]);
    

      au_task->next      = 0;                    /* paranoia */
      au_task->msgQ      = &rtn_msgQ;            /* reply to when finished */
      au_task->msg       = 0;                    /* reply with this message */
      
      au_task->list.t.data_ptr    = (u64 *)audio_cmdlist_ptr[current_buffer];
      au_task->list.t.data_size   = (cmdlist_ptr -audio_cmdlist_ptr[current_buffer])* sizeof(Acmd);
      au_task->list.t.type  = M_AUDTASK;
      au_task->list.t.ucode_boot = (u64 *)rspbootTextStart;
      au_task->list.t.ucode_boot_size =
	((int) rspbootTextEnd - (int) rspbootTextStart);
      au_task->list.t.ucode = (u64 *) aspMainTextStart;
      au_task->list.t.ucode_data = (u64 *) aspMainDataStart;
      au_task->list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
      au_task->list.t.dram_stack = (u64 *) NULL;
      au_task->list.t.dram_stack_size = 0;
      au_task->list.t.output_buff = (u64 *) NULL;
      au_task->list.t.output_buff_size = 0;
      au_task->list.t.yield_data_ptr = NULL;
      au_task->list.t.yield_data_size = 0;
      
      osSendMesg(sched_audioMQ, (OSMesg*)au_task, OS_MESG_BLOCK);

      samplesLeft = osAiGetLength()>>2;
/*
      if(samplesLeft == 0 && !firsttime){
	osSyncPrintf("out of samples\n");
	firsttime = 0;
      }
*/
      current_buffer ^= 1;
      current_audio_buffer++;

      osRecvMesg(&rtn_msgQ, NULL, OS_MESG_BLOCK);
      osAiSetNextBuffer(audio_buffer_ptr[current_audio],
			audioSamples[current_audio]<<2);
      break;

    case NN_SC_PRE_NMI_MSG: /* PRE NMI message*/
      /* 
	!!  Important  !!  Stop task until NMI is generated.
	 
	Please write a PRE MNI message process here.
	In this sample the sequence is faded out, but the sound is not.
	 
      */
      pre_nmi_flag++;
#ifdef _AUDIO_COMPACTMIDI_
      get_seq_vol[0] = alCSPGetVol(&seqplayer[0].seqplayer);
      get_seq_vol[1] = alCSPGetVol(&seqplayer[1].seqplayer);
#else
      get_seq_vol[0] = alSeqpGetVol(&seqplayer[0].seqplayer);
      get_seq_vol[1] = alSeqpGetVol(&seqplayer[1].seqplayer);
#endif /* _AUDIO_COMPACTMIDI_ */
      set_seq_vol[0] = get_seq_vol[0];
      set_seq_vol[1] = get_seq_vol[1];
      break;
    }
  }
  
}



/*--------------------------

  Read sequence file header

---------------------------*/
void auReadSeqFileHeader(u32 src_addr)
{
  s32       seqfile_size;

  seqHeaderfile_ptr = alHeapAlloc(&audio_heap, 1, 4);
  /* read sequence file header length */ 
  auRomRead(src_addr, seqHeaderfile_ptr, 4);

  seqfile_size = 4 + seqHeaderfile_ptr->seqCount * sizeof(ALSeqData);
  seqHeaderfile_ptr = alHeapAlloc(&audio_heap, 1, seqfile_size);

  /* read sequence file header */
  auRomRead(src_addr, seqHeaderfile_ptr, seqfile_size);
}


/*----------------------------------------------------
 Create and start audio thread

----------------------------------------------------*/
void auCreateAudioThread(NNSched *sched)
{

  osCreateThread(&audioThread, AUDIO_THREAD_ID, audioProc, (void *)sched,
		 (audioThreadStack+AUDIO_STACKSIZE/sizeof(u64)),
		  AUDIO_THREAD_PRI);
  osStartThread(&audioThread);

}

/*-----------------------------------------------------------------------------

  Initialize sequence player 

input
  midi_start: 	  bank file (.ctl) start ROM address
  midi_end: 	  bank file (.ctl) end ROM address
  seqheader_start:  sequence file (.sbk) start ROM address
  seqheader_end:   sequence file (.sbk) end ROM address
  midi_table_star:    wave table file (.tbl) start ROM address
-----------------------------------------------------------------------------*/

void auSeqPlayerInit(u8* midi_start, u8* midi_end,
		     u8* seqheader_start, u8* seqheader_end,
		     u8* midi_table_start) 
{
  ALBank *midiBank_ptr;
  u32    size;
  /* create sequence data buffer */
  seqplayer[0].seqdata_ptr = alHeapAlloc(&audio_heap, 1, AUDIO_SEQDATA_SIZE_MAX);
  seqplayer[1].seqdata_ptr = alHeapAlloc(&audio_heap, 1, AUDIO_SEQDATA_SIZE_MAX);

  /* initialize sequence player 0 */
  seqplayer[0].seqconfig.maxVoices   = AUDIO_SEQ_VVOICE_MAX; /* maximum virtual voice number */
  seqplayer[0].seqconfig.maxEvents   = AUDIO_SEQ_EVTCOUNT_MAX;  /* maximum internal event number */
  seqplayer[0].seqconfig.maxChannels = AUDIO_SEQ_CHANNEL_MAX;/* maximum MIDI channel number*/
  seqplayer[0].seqconfig.heap        = &audio_heap;      /* audio heap */
  seqplayer[0].seqconfig.initOsc     = 0;
  seqplayer[0].seqconfig.updateOsc   = 0;
  seqplayer[0].seqconfig.stopOsc     = 0;
  seqplayer[0].seqconfig.debugFlags  = 0;

#ifdef _AUDIO_COMPACTMIDI_
  alCSPNew(&seqplayer[0].seqplayer, &seqplayer[0].seqconfig);
#else
  alSeqpNew(&seqplayer[0].seqplayer, &seqplayer[0].seqconfig);
#endif /* _AUDIO_COMPACTMIDI_ */

  /* initialize sequence player 1 */
  seqplayer[1].seqconfig.maxVoices   = AUDIO_SEQ_VVOICE_MAX;/* maximum virtual voice number  */
  seqplayer[1].seqconfig.maxEvents   = AUDIO_SEQ_EVTCOUNT_MAX;  /* maximum internal event number  */
  seqplayer[1].seqconfig.maxChannels = AUDIO_SEQ_CHANNEL_MAX;/* maximum MIDI channel number  */
  seqplayer[1].seqconfig.heap        = &audio_heap;      /* audio heap */
  seqplayer[1].seqconfig.initOsc     = 0;
  seqplayer[1].seqconfig.updateOsc   = 0;
  seqplayer[1].seqconfig.stopOsc     = 0;
  seqplayer[1].seqconfig.debugFlags  = 0;

#ifdef _AUDIO_COMPACTMIDI_
  alCSPNew(&seqplayer[1].seqplayer, &seqplayer[1].seqconfig);
#else
  alSeqpNew(&seqplayer[1].seqplayer, &seqplayer[1].seqconfig);
#endif /* _AUDIO_COMPACTMIDI_ */

  /* read seqfileheader data */
  auReadSeqFileHeader((u32)seqheader_start);
  alSeqFileNew(seqHeaderfile_ptr, seqheader_start);

  /* read midi bank data */
  size = (u32)midi_end-(u32)midi_start;
  midi_buffer_ptr = alHeapAlloc(&audio_heap, 1, size);
  auRomRead((u32)midi_start, midi_buffer_ptr, size);

  /* Specify instrument bank used by sequence */
  alBnkfNew(midi_buffer_ptr, midi_table_start);
  midiBank_ptr = midi_buffer_ptr->bankArray[0];

#ifdef _AUDIO_COMPACTMIDI_
  alCSPSetBank(&seqplayer[0].seqplayer, midiBank_ptr);
  alCSPSetBank(&seqplayer[1].seqplayer, midiBank_ptr);
#else
  alSeqpSetBank(&seqplayer[0].seqplayer, midiBank_ptr);
  alSeqpSetBank(&seqplayer[1].seqplayer, midiBank_ptr);
#endif /* _AUDIO_COMPACTMIDI_ */

}


/*-----------------------------------------------------------------------------
  Sequence player seqno setup

Read sequence data

 u32 seqplayer:   sequence player number
 s32 seqno:          sequence number
-----------------------------------------------------------------------------*/
void auSeqPlayerSetFile(u32 seqplayer_no, s32 seqno)
{
  s32 seqdata_len;
  u8*  seqdata_offset;

#ifdef _AUDIO_DEBUG_
  if(seqHeaderfile_ptr->seqCount < seqno){
    osSyncPrintf("seqence no is over!!(seqcount = %d)\n",
		 seqHeaderfile_ptr->seqCount);
    return;
  }
#endif /* _AUDIO_DEBUG_ */

  seqdata_offset = seqHeaderfile_ptr->seqArray[seqno].offset;
  seqdata_len = seqHeaderfile_ptr->seqArray[seqno].len;
  if(seqdata_len & 0x01) seqdata_len++;

  /* read  sequence data */
  auRomRead((u32)seqdata_offset, seqplayer[seqplayer_no].seqdata_ptr, seqdata_len);

  seqplayer[seqplayer_no].seqno = seqno;

}

/*----------------------------------------------------------------------------
  Play sequence

Input:
  seqplayer_no: sequence player number to start playback .

 
  FALSE returned if sequence player is not in AL_STOPPED state.

----------------------------------------------------------------------------*/
int auSeqPlayerPlay(u32 seqplayer_no)
{
#ifndef _AUDIO_COMPACTMIDI_
  s32 seqdata_len;
  u32 seqno;
#endif /* _AUDIO_COMPACTMIDI_ */

#ifdef _AUDIO_DEBUG_
  if(seqplayer_no > 1){
    osSyncPrintf("seqplayer_no over!!\n");
    return FALSE;
  }
#endif /* _AUDIO_DEBUG_ */

  /* check seqplayer state. */
  if(seqplayer[seqplayer_no].seqplayer.state != AL_STOPPED){
#ifdef _AUDIO_DEBUG_
    osSyncPrintf("sequence player %d is playing\n",seqplayer_no);
#endif /* _AUDIO_DEBU_ */

    return FALSE;
  }

#ifdef _AUDIO_COMPACTMIDI_
  alCSeqNew(&seqplayer[seqplayer_no].sequence, seqplayer[seqplayer_no].seqdata_ptr);
  alCSPSetSeq(&seqplayer[seqplayer_no].seqplayer, &seqplayer[seqplayer_no].sequence);
  alCSPPlay(&seqplayer[seqplayer_no].seqplayer);
#else
  seqno = seqplayer[seqplayer_no].seqno;
  seqdata_len = seqHeaderfile_ptr->seqArray[seqno].len;
  alSeqNew(&seqplayer[seqplayer_no].sequence, seqplayer[seqplayer_no].seqdata_ptr, seqdata_len);
  alSeqpSetSeq(&seqplayer[seqplayer_no].seqplayer, &seqplayer[seqplayer_no].sequence);
  alSeqpPlay(&seqplayer[seqplayer_no].seqplayer);
#endif /* _AUDIO_COMPACTMIDI_ */
  return TRUE;
}

/*----------------------------------------------------------------------------
   Stop sequence stop

Input:
  seqplayer_no:  sequence player number to stop.
----------------------------------------------------------------------------*/
void auSeqPlayerStop(u32 seqplayer_no)
{
#ifdef _AUDIO_COMPACTMIDI_
  alCSPStop(&seqplayer[seqplayer_no].seqplayer);
#else 
  alSeqpStop(&seqplayer[seqplayer_no].seqplayer);
#endif /* _AUDIO_COMPACTMIDI_ */
}



/*----------------------------------------------------------------------------
  Sequence state

Input:
  seqplayer_no:  sequence player number to get state for.

Returns the sequence player state.
----------------------------------------------------------------------------*/
s32 auSeqPlayerState(u32 seqplayer_no)
{
#ifdef _AUDIO_COMPACTMIDI_
  /* There is no man , but there is libaudio.h so it is used  */
  return alCSPGetState(&seqplayer[seqplayer_no].seqplayer);
#else 
/*  return alSeqpGetState(&seqplayer[seqplayer_no].seqplayer);*/
  return seqplayer[seqplayer_no].seqplayer.state;

#endif /* _AUDIO_COMPACTMIDI_ */
}

/*-----------------------------------------------------------------------------
Initialize sound player

Input
  sndbankstart_ptr:  sound bank file (.ctl) start ROM address
  sndbankend_ptr:   sound bank file (.ctl) end ROM address
  sndtablestart_ptr:   wave table file (.tbl) start ROM address
-----------------------------------------------------------------------------*/
void auSndPlayerInit(u8* sndbankstart_ptr, u8* sndbankend_ptr,
		     u8* sndtablestart_ptr)
{
  u32 size;
  u32 cnt;

  /* read sound bank(.ctl file) */
  size = (u32)sndbankend_ptr-(u32)sndbankstart_ptr;
  snd_buffer_ptr = alHeapAlloc(&audio_heap, 1, size);
  auRomRead((u32)sndbankstart_ptr, snd_buffer_ptr, size);

  /* soundplayer initialize */
  sndplayer_config.maxSounds   = AUDIO_SND_VOICE_MAX;
  sndplayer_config.maxEvents   = AUDIO_SND_EVTCOUNT_MAX;
  sndplayer_config.heap        = &audio_heap;
  alSndpNew(&sndplayer, &sndplayer_config);

  /* bankfile initialize */
  alBnkfNew(snd_buffer_ptr, sndtablestart_ptr);

  /*
Register sound data in sound player in the order it is stored in the bank file.
  */
  snd_bank_ptr = snd_buffer_ptr->bankArray[0];
  snd_inst = snd_bank_ptr->instArray[0];
  snd_id = alHeapAlloc(&audio_heap, 1, snd_inst->soundCount*sizeof(ALSndId));

  for(cnt = 0, snd_alloc_num =0; cnt < snd_inst->soundCount; cnt++){
    snd_id[cnt] = alSndpAllocate(&sndplayer, snd_inst->soundArray[cnt]);
    if(snd_id[cnt] != -1) {
      snd_alloc_num++;
    }
  }
}

/*-----------------------------------------------------------------------------
  Playback sound

Input
  sndno:  sound number to playback.
-----------------------------------------------------------------------------*/
void auSndPlay(u32 sndno)
{
  s32 status;

  alSndpSetSound(&sndplayer, snd_id[sndno]);
  status = alSndpGetState(&sndplayer);
  if(status == AL_STOPPED){
    alSndpPlay(&sndplayer);
  }

}

/*-----------------------------------------------------------------------------
Stop sound

Input
  sndno:  sound number to stop.
-----------------------------------------------------------------------------*/
void auSndStop(u32 sndno)
{
  alSndpSetSound(&sndplayer,snd_id[sndno]);
  alSndpStop(&sndplayer);
}

/*-----------------------------------------------------------------------------
  Get sound state

Input
  sndno:  sound number to get state for.

Return:  the state of the sound specified by sndno
  
-----------------------------------------------------------------------------*/
s32 auSndGetState(u32 sndno)
{
  alSndpSetSound(&sndplayer, snd_id[sndno]);
  return alSndpGetState(&sndplayer);
}




