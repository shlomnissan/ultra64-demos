/**************************************************************************
 *                                                                        *
 *               Copyright (C) 1995, Silicon Graphics, Inc.               *
 *                                                                        *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright  law.  They  may not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *                                                                        *
 *************************************************************************/

#define	STACKSIZE	0x2000

#if defined(_LANGUAGE_C)

#define MY_MAX_BLOCK_SIZE  0x8000

#define CMD_PRINT_SOMETHING      1
#define CMD_WRITE_HOST           2
#define CMD_READ_GAME            CMD_WRITE_HOST
#define CMD_READ_HOST            3
#define CMD_WRITE_GAME           CMD_READ_HOST

typedef struct {
    int         command;
    int         blkSize;
} cmdBlk;



#endif	/* _LANGUAGE_C */
