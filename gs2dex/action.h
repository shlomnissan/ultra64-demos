/*---------------------------------------------------------------------
	Copyright (C) 1997, Nintendo.
	
	File		action.h
	Coded    by	Yoshitaka Yasumoto.	Mar 19, 1997.
	Modified by	
	Comments	
	
	$Id: action.h,v 1.5 1997/08/07 03:24:16 yasu Exp $
  ---------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * The controller structure.  	
 *---------------------------------------------------------------------------*/
typedef	struct	{
  OSContPad	*c;
  u16		button;
  u16		push;
  u16		release;
} myContPad;

/*---------------------------------------------------------------------------*
 * The action data structure.  	
 *---------------------------------------------------------------------------*/
typedef	struct	{
  myContPad	pad[MAXCONTROLLERS];
  u16		active;
  s16		imageX;
  s16		imageY;
  s32		imageYorig;
  s16		frameX;
  s16		frameY;
  u16		frameW;
  u16		frameH;
  s16		objX;
  s16		objY;
  u16		scaleW;
  u16		scaleH;
  u16		theta;
  u16		frameScaleW;
  u16		frameScaleH;
} Action;

/*---------------------------------------------------------------------------*
 * The menu message ID.
 *---------------------------------------------------------------------------*/
#define	MENU_CONTROL		0
#define	MENU_RENDERMODE	 	1
#define	MENU_RENDERMODE_2 	2
#define	MENU_OBJ_TX_TYPE	3
#define	MENU_OBJ_TX_LOAD_BY	4
#define	MENU_OBJ_TX_WINDOW	5
#define	MENU_OBJ_SHRINK		6
#define	MENU_OBJ_FLIPS		7
#define	MENU_OBJ_FLIPT		8
#define	MENU_BG_SCALABLE	9
#define	MENU_BG_TX_LOAD_BY	10
#define	MENU_BG_TX_FORMAT	11
#define	MENU_BG_FLIPS		12
#define	NUM_MENU		13

/*---------------------------------------------------------------------------*
 * The declaration of the prototype.  
 *---------------------------------------------------------------------------*/
extern	Action	Ac;
extern	u8	aMenu[NUM_MENU];
extern	void	menuInit(void);
extern	void	menuHandler(myContPad *);
extern	void	actionInit(void);
extern	void	actionUpdate(void);

/*======== End of action.h ========*/
