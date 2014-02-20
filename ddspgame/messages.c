#include <ultra64.h>
#include "diskproc.h"

/* This file has been modified from the original Kanji to an
 *   English character version.
 *
 * Nintendo Product Support Group, 1/29/98.
 */

u8*	message[ERROR_MES_SIZE] = {
	"Normal termination",
	"Abnormal termination",
	"Forced termination",
	"Error number",
	"Please refer to the DD User's Guide.",
	"[Caution] Please do not remove the disk",
	"while the access lamp is blinking.",
	"Disk is not inserted.",
	"Please insert the disk",
	"again.",
	"Is the disk inserted?",
	"The eject button may have been",
	"pushed.",
	"Please remove the disk",
	"and reinsert it.",
	"Wrong disk may have been inserted.",
	"Please confirm and",
	"set the time.",
	"Refer to the DD User's Guide",
	"for details.",
	"Battery for the built-in-clock is dead.",
	"Memory pak is not",
	"inserted.",
	"Currently creating.",
};

s32	kaddr[ERROR_MES_SIZE];

AsciiInfo sizeDB[256];

