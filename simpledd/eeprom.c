#include <ultra64.h>
#include "simple.h"

/**********************************************************************
 This source code created by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Purpose:
	Handles all the EEPROM stuff
 *********************************************************************/

int EepromSystemRunning;
int EepromAccessing;
static OSScMsg eepromMsg;
static OSThread eepromThread;
static u64 eepromThreadStack[STACKSIZEBYTES/sizeof(u64)];

static unsigned char eepromBuffer[DATA_BUFFER_SIZE];

// initEeprom() 
//
// Start eeprom subsystem.
//
int initEeprom(OSMesgQueue* siQ) {
  unsigned int i, j;
  unsigned char value;

  // Check for Eeprom existence
  if (osEepromProbe(siQ)) {
    EepromSystemRunning = 1;
  } else {
    EepromSystemRunning = 0;
  }

  if (EepromSystemRunning) {
    // Fill memory EEPROM buffer
    for (i=0; i<DATA_BUFFER_SIZE; i+=8) {
      value = i>>3;
      for (j=0; j<8; j++) {
        eepromBuffer[i+j] = value;
        value += 0x11;
      }
    }
  }

  EepromAccessing = 0;

  return EepromSystemRunning;
}

// eepromFullRead() 
//
// Read the entire eeprom into the buffer.
//
void eepromFullRead(OSMesgQueue* siQ) {
  int i, j;

  if (EepromSystemRunning == 0)
    return;

  EepromAccessing = 1;
#ifndef _FINALROM
  PRINTF("*** doing full read\n");
#endif

  osEepromLongRead(siQ, 0, eepromBuffer, DATA_BUFFER_SIZE);

  EepromAccessing = 0;
}


// eepromFullWrite() 
//
// Write the entire eeprom from the buffer.
//
void eepromFullWrite(OSMesgQueue* siQ) {
  int i;

  if (EepromSystemRunning == 0)
    return;

  EepromAccessing = 1;
#ifndef _FINALROM
  PRINTF("*** doing full write\n");
#endif

  osEepromLongWrite(siQ, 0, eepromBuffer, DATA_BUFFER_SIZE);
  
  EepromAccessing = 0;
}

