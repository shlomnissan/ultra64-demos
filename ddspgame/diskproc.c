#include <ultra64.h>
#include <leo.h>
#include "diskproc.h"

s32	CheckDiskEject(void);
s32	CheckDiskInsert(void);


DiskStat		disk_stat = {0, 0, 0, 0, 0, 0};
s32			errbuf[ERRBUF_SIZE+1];
static OSMesg		LeoMessages[NUM_LEO_MESGS];

OSMesgQueue		diskQ;
OSMesg         		diskQBuf;
s32			error = -100;
static LEOCmd		cmdBlock;
static LEODiskID	diskID;

void
SetErrorCode(s32 err, s32 flag) {
  s16		i;
  static s32	olderr = -1;

  if ( olderr != err || disk_stat.errptr == 0 ) {
    olderr = err;

    if ( err != 0 && flag != 0) {
      disk_stat.errno = err;
    }

    if ( disk_stat.errptr >= ERRBUF_SIZE ) {
      disk_stat.errptr = ERRBUF_SIZE-1;
      for (i = 0 ; i < ERRBUF_SIZE-1 ; i ++ ) {
	errbuf[i] = errbuf[i+1];
      }
    }
    errbuf[disk_stat.errptr++] = err;
  }
}

s32
DiskOtherErrors(s32 err) {
  disk_stat.errmes = 4;
  SetErrorCode(err, 1);
  return err;
}

s32
ErrSeqDiskChange(s16 flag) {
  s32		ret;
  static s16	mode = 0;

  if (mode == 0) {
    if (flag < 3) {
      mode = 1;
    } else {
      mode = 3;
    }
  }
  switch( mode ) {
  case 1:
    if ( flag == 0 )  {
      disk_stat.errmes = 9;
      mode = 2;
    } else if (flag == 1) {
      disk_stat.errmes = 7;
      mode = 3;
    } else {			/* flag == 2 */
      disk_stat.errmes = 8;
      mode = 3;
    }
    ret = DISK_PROC_CONTINUE;
    break;
  case 2:
    ret = CheckDiskEject();
    if (ret == DISK_NOT_EJECTED) {
      ret = DISK_PROC_CONTINUE;
      mode = 2;
    } else if (ret == DISK_EJECTED) {
      ret = DISK_PROC_CONTINUE;
      mode = 3;
    }
    break;
  case 3:
    ret = CheckDiskInsert();
    if (ret == DISK_NOT_INSERTED) {
      if ( flag == 4 && disk_stat.errmes != 5 && disk_stat.errmes != 10 ) {
	disk_stat.errmes = 14;
      }
      ret = DISK_PROC_CONTINUE;
      mode = 3;
    } else if (ret == DISK_INSERTED) {
      if (bcmp((void *)&leoBootID, (void *)&diskID, sizeof(LEODiskID))
	  != 0) {
	disk_stat.errmes = 10;		/* This isn't correct disk! */
	mode = 2;
	ret = DISK_PROC_CONTINUE;
      } else {
	mode = 0;
	ret = 0;
      }
    }
  }
  return ret;
}

s32
ErrSeqDiskInsert( s16 flag ) {
  static s16	mode = 0;
  s32	ret;

  if (mode == 0) {
    if (flag == 0) {
      disk_stat.errmes = 4;
    } else {
      disk_stat.errmes = 9;
    }
  }
  if ( (ret = CheckDiskEject()) == DISK_EJECTED) {
    ret = 0;
    mode = 0;
  } else {
    ret = DISK_PROC_CONTINUE;
    mode = 1;
  }
  return ret;
}

s32
ReadRTCTest(void) {

  LeoReadRTC(&cmdBlock, &diskQ);
  osRecvMesg(&diskQ, (OSMesg *)&error, OS_MESG_BLOCK);
  bcopy(&cmdBlock.data.time, &RTCdata2, sizeof(cmdBlock.data.time));

  disk_stat.flag = 0;

  switch( error ) {
  case 0:
    break;
  case LEO_ERROR_COMMAND_TERMINATED:
    break;
  case LEO_ERROR_QUEUE_FULL:
    error = DISK_PROC_CONTINUE;
    break;
  case LEO_ERROR_REAL_TIME_CLOCK_FAILURE:
    SetErrorCode( error, 1 );
    disk_stat.errmes = 12;
    disk_stat.flag = LEO_ERROR_REAL_TIME_CLOCK_FAILURE;
    break;
  default:
    DiskOtherErrors(error);
  }
  return error;
}

s32
SetRTCTest(void) {
  LeoSetRTC(&cmdBlock, &RTCdata, &diskQ);
  osRecvMesg(&diskQ, (OSMesg *)&error, OS_MESG_BLOCK);
 
  switch( error ) {
  case 0:
    break;
  case LEO_ERROR_COMMAND_TERMINATED:
    break;
  case LEO_ERROR_QUEUE_FULL:
    error = DISK_PROC_CONTINUE;
    break;
  default:
    DiskOtherErrors(error);
  }
  return error;
}

s32
ModeSelectTest(void) {
  s32		ret;
  static s16	mode = 0;
  static s32	err_mode = 0;
  
  if ( mode == 0 ) {
#if 1
    LeoModeSelectAsync(&cmdBlock, (u32)standby, (u32)sleep, &diskQ);
    osRecvMesg(&diskQ, (OSMesg *)&error, OS_MESG_BLOCK);
#else
    error = LeoModeSelect((u32)standby, (u32)sleep);
#endif
    SetErrorCode( error, 1 );
    err_mode = error;
  }
  ret = err_mode;
  mode = 0;

  switch( err_mode ) {
  case 0:
  case LEO_ERROR_COMMAND_TERMINATED:
    break;
  case LEO_ERROR_QUEUE_FULL:
    ret = DISK_PROC_CONTINUE;
    break;
  case LEO_ERROR_MEDIUM_MAY_HAVE_CHANGED:
    if ( (ret = ErrSeqDiskChange(4) ) == 0) {
      ret = DISK_PROC_CONTINUE;
    } else if (ret ==  DISK_PROC_CONTINUE) {
      mode = 1;
    }
    break;
  default:
    DiskOtherErrors(ret);
  }
  return ret;
}

s32
SleepTest(void) {
  s32		ret;
  static s16	mode = 0;
  static s32	err_mode = 0;

  if ( mode == 0 ) {
    LeoSpdlMotor(&cmdBlock, LEO_MOTOR_SLEEP, &diskQ);
    osRecvMesg(&diskQ, (OSMesg *)&error, OS_MESG_BLOCK);
    SetErrorCode(error, 1);
    err_mode = error;
  } else {
    mode = 0;
  }

  switch( err_mode) {
  case 0:
  case LEO_ERROR_MEDIUM_NOT_PRESENT:
  case LEO_ERROR_MEDIUM_MAY_HAVE_CHANGED:
    ret = 0;
    break;
  case LEO_ERROR_QUEUE_FULL:
    ret = DISK_PROC_CONTINUE;
    break;
  case LEO_ERROR_DIAGNOSTIC_FAILURE:
    if ( (ret = ErrSeqDiskInsert(1) ) !=  0) mode = 1;
    break;
  case LEO_ERROR_COMMAND_TERMINATED:
    ret = LEO_ERROR_COMMAND_TERMINATED;
    break;
  default:
    ret = err_mode;
    DiskOtherErrors( err_mode );
  }
  return ret;
}

s32
ActiveTest(void) {
  s32		ret;
  static s16	mode = 0;
  static s32	err_mode = 0;

  if ( mode == 0 ) {
    LeoSpdlMotor(&cmdBlock, LEO_MOTOR_ACTIVE, &diskQ);
    osRecvMesg(&diskQ, (OSMesg *)&error, OS_MESG_BLOCK);
    SetErrorCode(error, 1);
    err_mode = error;
  } else {
    mode = 0;
  }
 
  switch( err_mode ) {
  case 0:
    ret = 0;
    break;
  case LEO_ERROR_MEDIUM_NOT_PRESENT:
    if ( (ret = ErrSeqDiskChange(2) ) == 0) {
      ret = DISK_PROC_CONTINUE;
    } else if (ret ==  DISK_PROC_CONTINUE) {
      mode = 1;
    }
    break;
  case LEO_ERROR_MEDIUM_MAY_HAVE_CHANGED:
    if ( (ret = ErrSeqDiskChange(4) ) == 0) {
      ret = DISK_PROC_CONTINUE;
    } else if (ret ==  DISK_PROC_CONTINUE) {
      mode = 1;
    }
    break;
  case LEO_ERROR_QUEUE_FULL:
    ret = DISK_PROC_CONTINUE;
    break;
  case LEO_ERROR_DIAGNOSTIC_FAILURE:
    if ( (ret = ErrSeqDiskChange(0) ) == 0) {
      ret = DISK_PROC_CONTINUE;
    } else if (ret ==  DISK_PROC_CONTINUE) {
      mode = 1;
    }
    break;
  case LEO_ERROR_EJECTED_ILLEGALLY_RESUME:
    if ( (ret = ErrSeqDiskChange(1) ) == 0) {
      ret = DISK_PROC_CONTINUE;
    } else if (ret ==  DISK_PROC_CONTINUE) {
      mode = 1;
    }
    break;
  case LEO_ERROR_COMMAND_TERMINATED:
    ret = LEO_ERROR_COMMAND_TERMINATED;
    break;
  default:
    ret = err_mode;
    DiskOtherErrors(error);
  }
  return ret;
}

s32
UnitReadyTest(void) {
  s32		ret;
  LEOStatus	status;

  ret = LeoTestUnitReady(&status);
  SetErrorCode(ret, 1);

  switch( ret ) {
  case 0:
  case LEO_ERROR_MEDIUM_NOT_PRESENT:
  case LEO_ERROR_MEDIUM_MAY_HAVE_CHANGED:
    ret = 0;
    break;
  case LEO_ERROR_QUEUE_FULL:
  case LEO_ERROR_BUSY:
    ret = DISK_PROC_CONTINUE;
    break;
  case LEO_ERROR_COMMAND_TERMINATED:
    ret = LEO_ERROR_COMMAND_TERMINATED;
    break;
  default:
    DiskOtherErrors(ret);
  }
  return ret;
}

s32
DiskAccessTest( s16 flag ) {
  s32		ret;
  s32		lbas;
  static s16	mode = 0;
  static s32	err_mode = 0;

  switch( mode ) {
  case 0:
    if (flag == DISK_WRITE_MODE || flag == DISK_READ_MODE ) {
      LeoByteToLBA((s32)TEST_START_LBA, (u32)TEST_BYTE_SIZE, &lbas); 
    }
  case 1:
    switch ( flag ) {
    case DISK_READ_MODE:
      LeoReadWrite(&cmdBlock, OS_READ, (u32)TEST_START_LBA,
		   readwritebuf, lbas, &diskQ);
      break;
    case DISK_WRITE_MODE:
      LeoReadWrite(&cmdBlock, OS_WRITE, (u32)TEST_START_LBA,
		   readwritebuf, lbas, &diskQ);
      break;
    case DISK_SEEK_MODE:
      LeoSeek(&cmdBlock, (u32)TEST_START_LBA, &diskQ);
      break;
    case DISK_REZERO_MODE:
      LeoRezero(&cmdBlock, &diskQ);
      break;
    default:
      mode = 0;
      return -1;
    }
    osRecvMesg(&diskQ, (OSMesg *)&error, OS_MESG_BLOCK);
    err_mode = error;
    SetErrorCode(error, 1);
  case 2:
    ret = err_mode;
    mode = 0;
    switch( err_mode ) {
    case 0:
      break;
    case LEO_ERROR_QUEUE_FULL:
      mode = 1;
      ret = DISK_PROC_CONTINUE;
      break;
    case LEO_ERROR_MEDIUM_NOT_PRESENT:
      if ( (ret = ErrSeqDiskChange(2) ) == 0) {
	ret = DISK_PROC_CONTINUE;
      } else if (ret ==  DISK_PROC_CONTINUE) {
	mode = 2;
      }
      break;
    case LEO_ERROR_MEDIUM_MAY_HAVE_CHANGED:
      if ( (ret = ErrSeqDiskChange(4) ) == 0) {
	ret = DISK_PROC_CONTINUE;
      } else if (ret ==  DISK_PROC_CONTINUE) {
	mode = 2;
      }
      break;
    case LEO_ERROR_EJECTED_ILLEGALLY_RESUME:
      if ( (ret = ErrSeqDiskChange(1) ) == 0) {
	ret = DISK_PROC_CONTINUE;
      } else if (ret ==  DISK_PROC_CONTINUE) {
	mode = 2;
      }
      break;
    case LEO_ERROR_COMMAND_TERMINATED:
      ret = LEO_ERROR_COMMAND_TERMINATED;
      break;
    case LEO_ERROR_DIAGNOSTIC_FAILURE:
      if ( (ret = ErrSeqDiskChange(0) ) == 0) {
	ret = DISK_PROC_CONTINUE;
      } else if (ret ==  DISK_PROC_CONTINUE) {
	mode = 2;
      }
      break;
    default:
      DiskOtherErrors(error);
    }
  }
  return ret;
}

s32
CheckDiskEject( void ) {
  s32		ret;

  LeoSpdlMotor(&cmdBlock, LEO_MOTOR_SLEEP, &diskQ);
  osRecvMesg(&diskQ, (OSMesg *)&error, OS_MESG_BLOCK);

  switch( error ) {
  case 0:
  case LEO_ERROR_DIAGNOSTIC_FAILURE:
    SetErrorCode(error, 0);
    ret = DISK_NOT_EJECTED;
    break;
  case LEO_ERROR_QUEUE_FULL:
    SetErrorCode(error, 0);
    ret = DISK_PROC_CONTINUE;
    break;
  case LEO_ERROR_MEDIUM_NOT_PRESENT:
  case LEO_ERROR_MEDIUM_MAY_HAVE_CHANGED:
  case LEO_ERROR_EJECTED_ILLEGALLY_RESUME:
    SetErrorCode(error, 0);
    ret = DISK_EJECTED;
    break;
  case LEO_ERROR_COMMAND_TERMINATED:
    SetErrorCode(error, 0);
    ret = LEO_ERROR_COMMAND_TERMINATED;
    break;
  default:
    DiskOtherErrors(error);
    ret = error;
  }
  return ret;
}

s32
CheckDiskInsert( void ) {
  s32		ret;
  static s16	mode = 0;
  static s32	err_mode = 0;

  if ( mode == 0 ) {
    LeoReadDiskID(&cmdBlock, &diskID, &diskQ);
    osRecvMesg(&diskQ, (OSMesg *)&error, OS_MESG_BLOCK);
    err_mode = error;
  } else {
    mode = 0;
  }

  switch( err_mode ) {
  case 0:
    SetErrorCode(error, 0);
    ret = DISK_INSERTED;
    break;
  case LEO_ERROR_QUEUE_FULL:
    SetErrorCode(error, 0);
    ret = DISK_PROC_CONTINUE;
    break;
  case LEO_ERROR_EJECTED_ILLEGALLY_RESUME:
    disk_stat.errmes = 5;
    SetErrorCode(error, 1);
    ret = DISK_NOT_INSERTED;
    break;
  case LEO_ERROR_MEDIUM_NOT_PRESENT:
    SetErrorCode(error, 0);
    ret = DISK_NOT_INSERTED;
    break;
  case LEO_ERROR_COMMAND_TERMINATED:
    SetErrorCode(error, 0);
    ret = LEO_ERROR_COMMAND_TERMINATED;
    break;
  case LEO_ERROR_INCOMPATIBLE_MEDIUM_INSTALLED:
    SetErrorCode(error, 1);
    if ((ret = ErrSeqDiskInsert(0)) != 0) {
      ret = DISK_PROC_CONTINUE;
      mode = 1;
    }
    break;
  case LEO_ERROR_DIAGNOSTIC_FAILURE:
    SetErrorCode(error, 1);
    if ( (ret = ErrSeqDiskInsert(1)) != 0) {
      ret = DISK_PROC_CONTINUE;
      mode = 1;
    }
    break;
#ifdef	START_FROM_CASSETTE
  case LEO_ERROR_POWERONRESET_DEVICERESET_OCCURED:
    disk_stat.counter ++;
    SetErrorCode(error, 1);
    if (cursor == 0 && disk_stat.counter < 30) {
      LeoResetClear();
    } else {
      DiskOtherErrors(error);
    }
    ret = LEO_ERROR_POWERONRESET_DEVICERESET_OCCURED;
    break;
#endif
  default:
    DiskOtherErrors(error);
    ret = err_mode;
  }
  return ret;
}

s32
DiskInitialize( void ) {
  s32		ret;
  static s16	mode = 0;

  if ( mode == 0 ) {
    osCreateMesgQueue(&diskQ, &diskQBuf, 1);
#ifndef	START_FROM_CASSETTE
    ret = LeoCreateLeoManager((OSPri)OS_PRIORITY_LEOMGR - 1,
				(OSPri)OS_PRIORITY_LEOMGR,
				LeoMessages, NUM_LEO_MESGS);
#else
    ret = LeoCJCreateLeoManager((OSPri)OS_PRIORITY_LEOMGR - 1,
				(OSPri)OS_PRIORITY_LEOMGR,
				LeoMessages, NUM_LEO_MESGS);
#endif
    if ( ret == LEO_ERROR_DEVICE_COMMUNICATION_FAILURE ) {
      disk_stat.errmes = 16;
      SetErrorCode(ret, 1);
    } else if ( ret != LEO_ERROR_GOOD ) {
      DiskOtherErrors(ret);
      return ret;
    }
  } else {
    mode = 0;
  }

  if ( (ret = CheckDiskInsert()) == 0) {
#ifndef START_FROM_CASSETTE
    if (bcmp((void *)&leoBootID, (void *)&diskID, sizeof(LEODiskID))
	!= 0) {
      disk_stat.errmes = 2;
      ret = -1;			/* ID is different */
    } else {
      ret = 0;
    }
#else
    bcopy(&diskID, &leoBootID, sizeof(LEODiskID));
    ret = 0;
#endif
  } else {
    ret = DISK_PROC_CONTINUE;
    mode = 1;
  }

  return ret;
}

void
diskproc( void )
{
  s32		ret;
  static s16	mode = 0;

  disk_stat.mode = DISK_NO_BUSY;

  while (1){
    osRecvMesg(&diskMsgQ, (OSMesg *)NULL, OS_MESG_BLOCK);
    if ( disk_stat.mode == DISK_NO_BUSY ) {
      mode = cursor;
    }
    disk_stat.mode = DISK_BUSY;
    
    switch ( mode ) {
    case 0:
      ret = DiskInitialize();
      break;
    case 1:
      ret = DiskAccessTest(DISK_WRITE_MODE);
      break;
    case 2:
      ret = DiskAccessTest(DISK_READ_MODE);
      break;
    case 3:
      ret = DiskAccessTest(DISK_SEEK_MODE);
      break;
    case 4:
      ret = DiskAccessTest(DISK_REZERO_MODE);
      break;
    case 5:
      ret = CheckDiskInsert();
      break;
    case 6:
      ret = ReadRTCTest();
      break;
    case 7:
      ret = SetRTCTest();
      break;
    case 8:
      ret = ModeSelectTest();
      break;
    case 9:
      ret = SleepTest();
      break;
    case 10:
      ret = ActiveTest();
      break;
    case 11:
      ret = UnitReadyTest();
      break;
    }
    if ( ret == 0 ) {
      disk_stat.errno = 0;
      if ( mode == 6 ) {
	disk_stat.errmes = 13;	/* with TIME */
      } else {
	disk_stat.errmes = 1;
      }
      disk_stat.mode = DISK_NO_BUSY;
    } else if (ret == DISK_PROC_CONTINUE) {
      disk_stat.mode = DISK_PROC_CONTINUE;
    } else if (ret == LEO_ERROR_COMMAND_TERMINATED) {
      disk_stat.errmes = 3;
      disk_stat.mode = DISK_NO_BUSY;
    } else {
      if ( mode == 5 && ret == DISK_NOT_INSERTED &&
	  disk_stat.errno == 0) {
	disk_stat.errmes = 15;
      }
      disk_stat.mode = DISK_NO_BUSY;
    }
  }
}
