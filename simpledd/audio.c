/**********************************************************************
 * audio.c
 *
 * This code implements the application audio thread. This is done
 * through an audio manager layer. The audio manager, does all of the 
 * neccessary initialization of the audio, and signs in to the scheduler.
 * At the begining of each video retrace, the scheduler sends a message
 * that wakes up the audio thread, which calls alAudioFrame to build an
 * audio task. Once this task is built, it is sent to the scheduler, that
 * will then send the task to the rsp for execution. 
 * 
 * Copyright 1993, Silicon Graphics, Inc.
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
 *********************************************************************/

/**********************************************************************
 This source code modified by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Modifications:
	Altered to work with Disk Drive and pre-loaded table data.
 *********************************************************************/

#include <ultralog.h>
#include "simple.h"
#include "audio.h"

extern OSSched         sc;

/**** audio globals ****/
u8 audioHeap[AUDIO_HEAP_SIZE];

ALSeqPlayer	   *seqp;
static u8          *seqPtr;
static s32         seqLen;
static ALSeq       *seq;
static ALSeqMarker seqStart;
static ALSeqMarker seqEnd;

ALHeap             hp;

void initAudio(void) 
{
    ALBankFile    *bankPtr;
    u32           bankLen;
    ALSynConfig   c;
    ALSeqpConfig  seqc;
    amConfig      amc;
#ifdef DD64
    s32		bankByteLen;
    s32		seqByteLen;
    unsigned int* tablePtr;
    u32         tableLen;
#endif
    
    alHeapInit(&hp, audioHeap, sizeof(audioHeap));    

    /*
     * Load the bank file from ROM
     */
#ifndef DD64
    bankLen = _bankSegmentRomEnd - _bankSegmentRomStart;
    bankPtr = alHeapAlloc(&hp, 1, bankLen);
    cartCopy(_bankSegmentRomStart, (char *)bankPtr, bankLen);
#else
    bankLen = (u32)_bankSegmentDiskEnd - (u32)_bankSegmentDiskStart;
    LeoLBAToByte((u32)_bankSegmentDiskStart, bankLen,
		&bankByteLen);
    bankPtr = alHeapAlloc(&hp, 1, (u32)bankByteLen);
    ddCopy((u32)_bankSegmentDiskStart, (char *)bankPtr, bankLen);
#endif
    
#ifndef DD64
    alBnkfNew(bankPtr, (u8 *) _tableSegmentRomStart);
#else
    tablePtr = (unsigned int*)0x80400000;
    tableLen = (u32)_tableSegmentDiskEnd - (u32)_tableSegmentDiskStart;
    ddCopy((u32)_tableSegmentDiskStart, (char *)tablePtr, tableLen);
    
    alBnkfNew(bankPtr, (u8 *) tablePtr);
#endif

    /*
     * Load the sequence file from ROM
     */
#ifndef DD64
    seqLen = _seqSegmentRomEnd - _seqSegmentRomStart;
    seqPtr = alHeapAlloc(&hp, 1, seqLen);
    cartCopy(_seqSegmentRomStart, (char *) seqPtr, seqLen);
#else
    seqLen = (u32)_seqSegmentDiskEnd - (u32)_seqSegmentDiskStart;
    LeoLBAToByte((u32)_seqSegmentDiskStart, seqLen,
		&seqByteLen);
    seqPtr = alHeapAlloc(&hp, 1, (u32)seqByteLen);
    ddCopy((u32)_seqSegmentDiskStart, (char *) seqPtr, seqLen);
#endif

    /*
     * Create the Audio Manager
     */
    c.maxVVoices = MAX_VOICES;
    c.maxPVoices = MAX_VOICES;
    c.maxUpdates = MAX_UPDATES;
    c.dmaproc    = 0;                  /* audio mgr will fill this in */
    c.fxType	 = AL_FX_SMALLROOM;
    c.outputRate = 0;                  /* audio mgr will fill this in */
    c.heap       = &hp;
    
    amc.outputRate = 44100;
    amc.framesPerField = NUM_FIELDS;
    amc.maxACMDSize = MAX_RSP_CMDS;
 
    amCreateAudioMgr(&c, AUDIO_PRIORITY, &amc);
    
    /*
     * Create the sequence and the sequence player
     */
    seqc.maxVoices      = MAX_VOICES;
    seqc.maxEvents      = MAX_EVENTS;
    seqc.maxChannels    = 16;
    seqc.heap           = &hp;
    seqc.initOsc        = 0;
    seqc.updateOsc      = 0;
    seqc.stopOsc        = 0;
#ifdef _DEBUG
    seqc.debugFlags     = NO_VOICE_ERR_MASK |NOTE_OFF_ERR_MASK | NO_SOUND_ERR_MASK;
#endif
    seqp = alHeapAlloc(&hp, 1, sizeof(ALSeqPlayer));
    alSeqpNew(seqp, &seqc);

    seq = alHeapAlloc(&hp, 1, sizeof(ALSeq));
#ifndef DD64
    alSeqNew(seq, seqPtr, seqLen);    
#else
    alSeqNew(seq, seqPtr, seqByteLen);    
#endif
    alSeqNewMarker(seq, &seqStart, 0);
    alSeqNewMarker(seq, &seqEnd, -1);

    alSeqpLoop(seqp, &seqStart, &seqEnd, -1);
    alSeqpSetSeq(seqp, seq);
    alSeqpSetBank(seqp, bankPtr->bankArray[0]);
    alSeqpPlay(seqp);
}

