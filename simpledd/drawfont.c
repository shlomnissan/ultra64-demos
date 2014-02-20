#include <ultra64.h>
#include "simple.h"

/**********************************************************************
 This source code created by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Purpose:
	Draw fonts to the current frame.
 *********************************************************************/

#define	G_CC_INT4	0,0,0,PRIMITIVE,0,0,0,TEXEL0

#define WELCOME_MESG 0
#define EEPROM_MESG 1
#define DD_MESG 2

#define DM_READ_MESG 3
#define DM_WRITE_MESG 4
#define DM_GOAWAY_MESG 5

char *message[] = {
	"Welcome to the simple demo.",
	"EEPROM",
	"DD",
	"Data Read",
	"Data Write",
	"Exit Menu"
};

#ifdef DD64

OSPiHandle* epiHandler;
static unsigned char* fontBuffer = (unsigned char*)0x80500000; // For DD font temp storage
static unsigned char* currBufferPtr; // For DD font temp storage

// printString()
//
// print string s at x,y using font z
// 
void printString(int x,int y,int z,unsigned char *s) {
	OSMesg dummyMessage;
	int ofs,i,sx,dx,dy,cy;

	for(i=0; s[i]; i++){
		ofs=LeoGetAAdr((int)s[i]-32+z*192,&dx,&dy,&cy);

		ddGetFontChar(ofs, currBufferPtr, &dmaMessageQ);

		sx = (dx+1)&254;
		gDPLoadTextureTile_4b(glistp++,
                   currBufferPtr, G_IM_FMT_I, sx, dy, 0, 0,
                   (sx-1)<<G_TEXTURE_IMAGE_FRAC, (dy-1)<<G_TEXTURE_IMAGE_FRAC,
                   0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, 
                   G_TX_NOLOD, G_TX_NOLOD);
		gSPTextureRectangle(glistp++,
                   x<<2, (y-cy)<<2, (x+dx)<<2, (y-cy+dy)<<2,
                   0, 0, 0, 0x400, 0x400);
		currBufferPtr+=128;
                x += dx+1;
		osRecvMesg(&dmaMessageQ,&dummyMessage,OS_MESG_BLOCK);
	}
}

#else

// printString()
//
// print string s at x,y using font z
// 
void printString(int x,int y,int z,unsigned char *s)
{
	static int fontLoaded = 0;

	int i,cnum,sx,dx,dy,cy;

	if (fontLoaded == 0) {
		cartCopy(_fontSegmentRomStart, _fontSegmentStart,
			_fontSegmentRomEnd - _fontSegmentRomStart);
		fontLoaded = 1;
	}

	for(i=0;s[i];i++){
		cnum = s[i]-32;
		dx = myFontDx[cnum];
		dy = myFontDy[cnum];
		cy = myFontCy[cnum];

		sx = (dx+1)&254;
		gDPLoadTextureTile_4b(glistp++,
                   myFont[cnum], G_IM_FMT_I, sx, dy, 0, 0,
                   (sx-1)<<G_TEXTURE_IMAGE_FRAC, (dy-1)<<G_TEXTURE_IMAGE_FRAC,
                   0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, 
                   G_TX_NOLOD, G_TX_NOLOD);
		gSPTextureRectangle(glistp++,
                   x<<2, (y-cy)<<2, (x+dx)<<2, (y-cy+dy)<<2,
                   0, 0, 0, 0x400, 0x400);
                x += dx+1;
	}
}

#endif

#define COLOR_DELTA 10
// doFont()
//
// Draw all fonts for this frame.
//
void doFont(void) {
	static int color = 255;

#ifdef DD64
	currBufferPtr = fontBuffer;
#endif

	/* Settings */
	gDPPipeSync(glistp++);
	gDPSetCycleType(glistp++,G_CYC_1CYCLE);
	gDPSetTexturePersp(glistp++,G_TP_NONE);
	gDPSetTextureLUT(glistp++,G_TT_NONE);
	gDPSetTextureLOD(glistp++,G_TL_LOD);
	gDPSetAlphaCompare(glistp++,G_AC_THRESHOLD);

	gDPSetRenderMode(glistp++,G_RM_XLU_SURF,G_RM_XLU_SURF2);
	gDPSetCombineMode(glistp++,G_CC_INT4,G_CC_INT4);
	gDPSetPrimColor(glistp++,0,0,255,255,255,255);

	// Print welcome message
	printString(40, 30, 3, message[WELCOME_MESG]);

#ifdef DD64
	// Assume no EEPROM if DD game
	gDPSetPrimColor(glistp++,0,0,255,0,0,255);
#else
	// Print EEPROM message if EEPROM exists
	if (EepromFound == 1) {
	  gDPSetPrimColor(glistp++,0,0,0,255,0,255);
	} else {
	  gDPSetPrimColor(glistp++,0,0,255,0,0,255);
	}
#endif
	printString(30, 52, 3, message[EEPROM_MESG]);

	// Print DD message
#ifdef DD64
	gDPSetPrimColor(glistp++,0,0,0,255,0,255);
#else
	gDPSetPrimColor(glistp++,0,0,255,0,0,255);
#endif
	printString(250, 52, 3, message[DD_MESG]);
	
#ifdef DD64
	// Print Data menu
	if (DataMenu != DM_INVISIBLE) {
	  gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 255);
	  printString(200, 170, 3, message[DM_READ_MESG]);
	  printString(200, 182, 3, message[DM_WRITE_MESG]);
	  printString(200, 194, 3, message[DM_GOAWAY_MESG]);
	  gDPSetPrimColor(glistp++, 0, 0, color, 200, 200, 255);
	  color += COLOR_DELTA;
	  color = color%256;
	  printString(200, 170 + 12*DataMenuPos, 3, message[DataMenuPos+3]); 
	}
#else
	// Print Data menu
	if ((EepromFound == 1) && (DataMenu != DM_INVISIBLE)) {
	  gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 255);
	  printString(30, 170, 3, message[DM_READ_MESG]);
	  printString(30, 182, 3, message[DM_WRITE_MESG]);
	  printString(30, 194, 3, message[DM_GOAWAY_MESG]);
	  gDPSetPrimColor(glistp++, 0, 0, color, 200, 200, 255);
	  color += COLOR_DELTA;
	  color = color%256;
	  printString(30, 170 + 12*DataMenuPos, 3, message[DataMenuPos+3]); 
	}
#endif

#ifdef DD64
	osInvalDCache(fontBuffer,currBufferPtr-fontBuffer);
#else
	osInvalDCache(myFont,sizeof(myFont));
#endif
}

