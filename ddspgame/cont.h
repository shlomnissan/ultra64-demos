/*=====================================================================
  File  : cont.h

  Created   by Koji Mitsunari.   Jun,19 1997.
  Copyright by Nintendo, Co., Ltd.      1997.
  =====================================================================*/
#ifndef	_cont_h_
#define	_cont_h_

extern	void	initcont(OSMesgQueue *);
extern	void	readcont(void);

typedef struct{
  int	flag, stat;
  int	nowcon, oldcon, nowtrg;
  int	repeat, repcnt;
  int	sx, sy;
}Conts;

extern	Conts	conts[];

#define	LcdSetCtrl(x,y,z) __osContRamWrite(x, y, LCD_CTRL_BANK, z, 1);
#define	LcdSetData(x,y,z) __osContRamWrite(x, y, LCD_DATA_BANK, z, 1);

#endif /* _cont_h_ */

