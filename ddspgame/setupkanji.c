#include<ultra64.h>
#include<PR/leo.h>
#include "main.h"
#include "diskproc.h"

extern OSPiHandle	*handler;
extern unsigned char    kbuffer[];
extern OSMesgQueue	dmaMessageQ;
extern OSMesg		dmaMessageBuf;
extern OSIoMesg		dmaIOMessageBuf;

/* This file has been modified from the original Kanji to an
 *   English character version.  
 *
 * Nintendo Product Support Group, 1/29/98.
 */

#define FONT_NUMBER 1
/* English Font to kbuffer by EPI DMA
 */
void transkan(int code, u8 *ptr)
{
  int 	dx, dy, cy;
  int	offset;

  /* Get char address
   */
  offset = LeoGetAAdr(FONT_NUMBER*192+code,&dx,&dy,&cy);
  
  /* Store letter size info in sizeDB
   */
  sizeDB[code].cy = cy;
  sizeDB[code].dx = dx;
  sizeDB[code].dy = dy;

  dmaIOMessageBuf.hdr.pri 	= OS_MESG_PRI_NORMAL;
  dmaIOMessageBuf.hdr.retQueue 	= &dmaMessageQ;
  dmaIOMessageBuf.dramAddr 	= ptr;
  dmaIOMessageBuf.devAddr 	= DDROM_FONT_START + offset;
  dmaIOMessageBuf.size		= 128;
	
  handler->transferInfo.cmdType	= OS_OTHERS;
  osEPiStartDma(handler, &dmaIOMessageBuf, OS_READ);
  osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);
}

/* Translate English font in a message to buffer 
 */
s32 initkanchr(u8 *str, s32 addr) {
  s16	i = 0;

  while ( str[i] != NULL ) {
    transkan( (int)(str[i])-32, (u8 *)&kbuffer[addr]);
    addr += 128;
    i++;
  }
  return addr;
}

