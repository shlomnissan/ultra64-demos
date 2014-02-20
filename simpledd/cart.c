#include <ultra64.h>
#include "simple.h"

/**********************************************************************
 This source code created by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Purpose:
	Cartridge-specific functions
 *********************************************************************/

OSMesg           PiMessages[DMA_QUEUE_SIZE];
OSMesgQueue      PiMessageQ;

// cartCopy()
//
// A simple utility routine for copying data from rom to ram
// Note that the routine assumes that it has exclusive use of
// the dmaMessageQ.
//
void cartCopy(const char *src, const char *dest, const int len)
{
    OSIoMesg dmaIoMesgBuf;
    OSMesg dummyMesg;
    
    /*
     * Always invalidate cache before dma'ing data into the buffer.
     * This is to prevent a flush of the cache in the future from 
     * potentially trashing some data that has just been dma'ed in.
     * Since you don't care if old data makes it from cache out to 
     * memory, you can use the cheaper osInvalDCache() instead of one
     * of the writeback commands
     */
    osInvalDCache((void *)dest, (s32) len);
    osPiStartDma(&dmaIoMesgBuf, OS_MESG_PRI_NORMAL, OS_READ,
                 (u32)src, (void *)dest, (u32)len, &dmaMessageQ);
    (void) osRecvMesg(&dmaMessageQ, &dummyMesg, OS_MESG_BLOCK);
}

// cartInitPI()
//
// Initialize the Peripheral Interface.
//
void cartInitPI() {
    /**** Start PI Mgr for access to cartridge ****/
    osCreatePiManager((OSPri) OS_PRIORITY_PIMGR, &PiMessageQ, PiMessages,
                        DMA_QUEUE_SIZE);
}

