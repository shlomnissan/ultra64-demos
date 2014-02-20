#include <ultra64.h>
#include <leo.h>
#include "simple.h"

/**********************************************************************
 This source code created by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Purpose:
	DD-specific functions.
 *********************************************************************/

static OSMesg LeoMesgBuffer[NUM_LEO_MESGS];
static OSMesgQueue LeoMesgQ;
static LEOCmd cmdBlock;
static LEODiskID diskID;

static OSPiHandle* ddRomHandle;
static OSIoMesg    dmaIOMessageBuf;

static unsigned char ddBuffer[MAX_LBA_BYTE_SIZE];
static int dataStoreLBA;

// ddInit()
//
// Do everything necessary to start up the DD.
//
void ddInit() {
  LEOCapacity lcap;
  s32 error;
  int i;
  unsigned char counter;

  osCreateMesgQueue(&LeoMesgQ, LeoMesgBuffer, NUM_LEO_MESGS);

  error = LeoCreateLeoManager((OSPri)OS_PRIORITY_LEOMGR - 1,
			(OSPri)OS_PRIORITY_LEOMGR,
			LeoMesgBuffer, NUM_LEO_MESGS); 
  if (error != LEO_ERROR_GOOD) {
    PRINTF("error creating leo manager, %d\n", error);
  }

  LeoReadDiskID(&cmdBlock, &diskID, &LeoMesgQ);
  osRecvMesg(&LeoMesgQ, (OSMesg*)&error, OS_MESG_BLOCK);

  // Determine where to write on disk
  LeoReadCapacity(&lcap, OS_WRITE);
  dataStoreLBA = lcap.startLBA;
  
  for (i=0, counter=0; i<DATA_BUFFER_SIZE; i++, counter++) {
    ddBuffer[i] = counter;
  } 
}

// ddCopy()
//
// Copy a chunk of the disk into RAM.
//
void ddCopy(s32 src, const char* dest, s32 length) {
	OSMesg dummyMesg;
	s32 numBytes;	

	LeoLBAToByte(src, length, &numBytes);
	osInvalDCache((void*)dest, (s32)numBytes);
	LeoReadWrite(&cmdBlock, OS_READ, src, (void*)dest, length, 
			&dmaMessageQ);
	osRecvMesg(&dmaMessageQ, &dummyMesg, OS_MESG_BLOCK);
}

// ddInitEPI()
//
// Grab the DD ROM handler.
//
void ddInitEPI() {
	ddRomHandle = osDriveRomInit();
}

// ddGetFontChar()
//
// DMA the requested font from the DD ROM to RAM.
//
void ddGetFontChar(int romOffset, unsigned char* addr, 
			OSMesgQueue* msgQ) {

        dmaIOMessageBuf.hdr.pri         = OS_MESG_PRI_NORMAL;
        dmaIOMessageBuf.hdr.retQueue    = msgQ;
        dmaIOMessageBuf.dramAddr        = addr;
        dmaIOMessageBuf.devAddr         = DDROM_FONT_START + romOffset;
        dmaIOMessageBuf.size            = 128;

        ddRomHandle->transferInfo.cmdType = OS_OTHERS;
        osEPiStartDma(ddRomHandle, &dmaIOMessageBuf, OS_READ);

}


// ddFullRead()
//
// Read the whole saved game data area to RAM.
//
void ddFullRead(OSMesgQueue* msgQ) {
	int i,j;

	ddCopy(dataStoreLBA, ddBuffer, 1);
	for (i=0; i<DATA_BUFFER_SIZE; i+=8) {
		for (j=0; j<8; j++) {
		  PRINTF("%x ", ddBuffer[i+j]);
		}
		PRINTF("\n");
	}
}

// ddFullWrite()
//
// Write the whole saved game data area to the disk.
//
void ddFullWrite(OSMesgQueue* msgQ) {
	OSMesg dummyMesg;
	s32 numBytes;	

	LeoReadWrite(&cmdBlock, OS_WRITE, dataStoreLBA, (void*)ddBuffer, 
			1, &dmaMessageQ);
	osRecvMesg(&dmaMessageQ, &dummyMesg, OS_MESG_BLOCK);
}
