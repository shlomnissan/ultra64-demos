#ifndef _DD_H
#define _DD_H

/**********************************************************************
 This source code created by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Purpose:
	DD-specific functions.
 *********************************************************************/

#define NUM_LEO_MESGS 8
#define MAX_LBA_BYTE_SIZE 19720

void ddInit(void);
void ddCopy(s32 src, const char* dest, s32 length);
void ddInitEPI(void);
void ddGetFontChar(int, unsigned char*, OSMesgQueue*); 
void ddFullRead(OSMesgQueue*);
void ddFullWrite(OSMesgQueue*);

#endif
