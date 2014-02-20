#ifndef	_diskproc_h_
#define	_diskproc_h_

#include <ultra64.h>
#include <PR/leo.h>

/* This file has been modified from the original Kanji to an
 *   English character version.
 *
 * Nintendo Product Support Group, 1/29/98.
 */

#define	ERROR_MES_SIZE	24

#define	DISK_BUSY	1
#define	DISK_NO_BUSY	0

#define	DISK_PROC_CONTINUE	100

#define	DISK_EJECTED		LEO_ERROR_MEDIUM_NOT_PRESENT
#define	DISK_NOT_EJECTED	0
#define DISK_INSERTED		0
#define DISK_NOT_INSERTED	LEO_ERROR_MEDIUM_NOT_PRESENT

#define	DISK_READ_MODE		0
#define	DISK_WRITE_MODE		1
#define	DISK_SEEK_MODE		2
#define	DISK_REZERO_MODE	3

#define	NUM_LEO_MESGS	8
#define TEST_START_LBA	1418
#define	TEST_BYTE_SIZE	(1024*1024)

#define ERRBUF_SIZE	7

#define	MES_CMD_X	30
#define	MES_CMD_Y	28
#define	MES_CMD_DY	18
#define	MES_DATE_X	80
#define	MES_DATE_Y	96
#define MES_END_X	120
#define MES_END_Y	120

#define abs(x)                  ( (x >= 0) ? (x) : (-(x)))

enum ERROR_MESSAGE {
  ERRMES_SEIJO_SHURYO,
  ERRMES_IJO_SHURYO,
  ERRMES_KYOSEI_SHURYO,
  ERRMES_ERROR_NUMBER,
  ERRMES_TORIATUKAI_SETUMEISHO,
  ERRMES_ACCESS_LAMP,
  ERRMES_DISK_WO_NUKANAIDE,
  ERRMES_DISK_GA_SOUNYUUSARETE,
  ERRMES_MOITIDO_DISK_WO,
  ERRMES_KUDASAI,
  ERRMES_DISK_GA_HAITTEMASUKA,
  ERRMES_DISK_GA_CHUTO_HANPA,
  ERRMES_KOMARETEIRU_KANOSEI,
  ERRMES_ICHIDO_DISK_WO_NUITE,
  ERRMES_SASHIKONNDE,
  ERRMES_MACHIGATTA_DISK,
  ERRMES_KANOSEI_GA_ARIMASU,
  ERRMES_JIKOKU_WO_SETTEI,
  ERRMES_KUWASIKU_HA_SETUMEISHO,
  ERRMES_OYOMIKUDASAI,
  ERRMES_TOKEI_NO_DENCHI,
  ERRMES_KAKCHO_RAM_PAK,
  ERRMES_IMASEN,
  ERRMES_GENZAI_SAKUSEICHU
};

typedef struct {
  s16	mode;
  s16	flag;
  u16	errptr;
  s16   errno;
  s16	errmes;
  s16	counter;
} DiskStat;

typedef struct {
  int cy;
  int dx;
  int dy;
} AsciiInfo;

extern	s32		initkanchr(u8 *, s32);
extern	void		printkanji(int, int, u16);
extern  s32		LeoCJCreateLeoManager(OSPri, OSPri, OSMesg *, s32);
extern 	OSPiHandle 	*osDriveCRomInit(void);
extern	void		diskproc(void);

extern	OSMesgQueue     diskMsgQ;
extern	DiskStat	disk_stat;
extern	s16		resetflag;
extern	int		cursor;
extern	s16		standby;
extern	s16		sleep;

extern	LEODiskTime	RTCdata;
extern	LEODiskTime	RTCdata2;
extern	s32		errbuf[];
extern	int		counter;
extern	u8		readwritebuf[];
extern	u8		*message[];
extern	s32		kaddr[];
extern	AsciiInfo	sizeDB[];

#endif /* _diskproc_h_ */







