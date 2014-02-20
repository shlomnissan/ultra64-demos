#ifndef _EEPROM_H
#define _EEPROM_H

/**********************************************************************
 This source code created by Mark A. DeLoura, Nintendo of America Inc.
 Last modification: October 10, 1997

 Purpose:
	Handles all the EEPROM stuff.
 *********************************************************************/

extern int EepromFound;
extern int EepromRequest;
extern int EepromSystemRunning;
extern int EepromAccessing;

// Eeprom subsystem
#define EEPROM_FREE 0
#define EEPROM_READ 1
#define EEPROM_WRITE 2

int initEeprom(OSMesgQueue*);
void eepromFullRead(OSMesgQueue*);
void eepromFullWrite(OSMesgQueue*);

#endif
