/*---------------------------------------------------------------------
	Copyright (C) 1997, Nintendo.
	
	File		action.c
	Coded    by	Yoshitaka Yasumoto.	Mar 19, 1997.
	Modified by	
	Comments	
	
	$Id: action.c,v 1.10 1997/08/07 03:24:16 yasu Exp $
  ---------------------------------------------------------------------*/
#include	<ultra64.h>
#include	<PR/gs2dex.h>
#include	"system.h"
#include	"sprite.h"
#include	"action.h"

void	moveBg(myContPad *);
void	moveObject(myContPad *);
void	moveByPlusKey(u16, s16 *, s16 *, s16);
void	moveByCButtons(u16, s16 *, s16 *, s16);
void	moveBy3DStick(myContPad *, s16 *, s16 *, s16);
void	printMenu(s16, s16);
void	menuInit(void);
void	menuHandler(myContPad *);


/*---------------------------------------------------------------------------*
 * The Action structure.  
---------------------------------------------------------------------------*/
Action	Ac;

/*---------------------------------------------------------------------------*
 * The menu value. 
 *---------------------------------------------------------------------------*/
u8	aMenu[NUM_MENU];

/*---------------------------------------------------------------------------*
 * The initialization of the Action structure.  
 *---------------------------------------------------------------------------*/
void	actionInit(void)
{
  int	i;
  
  Ac.active        = 0;
  Ac.imageX        = 0<<2;
  Ac.imageY        = 0<<2;
  Ac.imageYorig    = 0<<2;
  Ac.frameX        = 0<<2;
  Ac.frameY        = 0<<2;
  Ac.frameW        = 320<<2;
  Ac.frameH        = 240<<2;
  Ac.objX          = (SCREEN_WD/2 - 16)<<2;
  Ac.objY          = (SCREEN_HT/2 - 16)<<2;
  Ac.scaleW        = 1<<10;
  Ac.scaleH        = 1<<10;
  Ac.theta         = 0;
  Ac.frameScaleW   = 1<<10;
  Ac.frameScaleH   = 1<<10;
  
  for (i = 0; i < MAXCONTROLLERS; i ++){
    Ac.pad[i].c = contPad + i;
    Ac.pad[i].button = Ac.pad[i].push = Ac.pad[i].release = 0;
  }
  return;
}

/*---------------------------------------------------------------------------*
 * Set the member of the Action structure from the Menu status.  
 *---------------------------------------------------------------------------*/
void	actionUpdate(void)
{
  int	i;
  u16	button, change;
  
  /*---- The trigger calculation of the controller. ----*/
  for (i = 0; i < MAXCONTROLLERS; i ++){
    button = Ac.pad[i].c->button;
    change = Ac.pad[i].button ^ button;
    Ac.pad[i].release = (Ac.pad[i].push = change & button) ^ change;
    Ac.pad[i].button = button;
  }
  
  /*---- The menu selection process. ----*/
  menuHandler(Ac.pad+0);
  
  /*---- The moving process of Sprite.  ----*/
  if (aMenu[MENU_CONTROL]){
    moveBg    (Ac.pad+0);
    moveObject(Ac.pad+1);
  } else {
    moveObject(Ac.pad+0);  
    moveBg    (Ac.pad+1);
  }
  return;
}

/*---------------------------------------------------------------------------*
 * The operation of Bg.  
 *---------------------------------------------------------------------------*/
void	moveBg(myContPad *mc)
{	
  s16	imageW, imageH;
  
  imageW = objBg.b.imageW * 8;
  imageH = objBg.b.imageH * 8;

  /*---- Scroll by the 3D stick. ----*/
  moveBy3DStick(mc, &Ac.imageX, &Ac.imageY, 8);
  
  /*---- Slight moving by the cross key. ----*/
  moveByPlusKey(mc->push, &Ac.imageX, &Ac.imageY, 8);
  
  /*---- The Wrap process of the screen end. ----*/
  if (Ac.imageX < 0){
    Ac.imageX     += imageW;
    Ac.imageY     -= 32;
    Ac.imageYorig -= 32;
  }
  if (Ac.imageX >= imageW){
    Ac.imageX     -= imageW;
    Ac.imageY     += 32;
    Ac.imageYorig += 32;
  }
  if (Ac.imageY < 0){
    Ac.imageY     += imageH;
    Ac.imageYorig += imageH;
  }
  if (Ac.imageY >= imageH){
    Ac.imageY     -= imageH;
    Ac.imageYorig -= imageH;
  }
  
  /*----- Move the frame by the C Button. ----*/
  if (mc->button & Z_TRIG){
    moveByCButtons(mc->button, (s16 *)&Ac.frameW, (s16 *)&Ac.frameH, 8);
  } else {
    moveByCButtons(mc->button, &Ac.frameX, &Ac.frameY, 8);
  }
  
  /*---- Scaling by L R keys. ----*/
  if (mc->button & L_TRIG){
    Ac.frameScaleW -= 32;
    Ac.frameScaleH -= 32;
    if (Ac.frameScaleW <= 0) Ac.frameScaleW = 32;
    if (Ac.frameScaleH <= 0) Ac.frameScaleH = 32;
    Ac.imageYorig = Ac.imageY;
  }
  if (mc->button & R_TRIG){
    Ac.frameScaleW += 32;
    Ac.frameScaleH += 32;
    Ac.imageYorig = (s32)Ac.imageY;
  }

  /*---- To the initialize position with START. ----*/
  if (mc->push & START_BUTTON){
    Ac.imageX = Ac.imageY = 0;
    Ac.frameX = Ac.frameY = 0;    
    Ac.imageYorig = 0L;
    Ac.frameScaleW = Ac.frameScaleH = 1<<10;
  }
  return;
}

/*---------------------------------------------------------------------------*
 * The operation of Obj.
 *---------------------------------------------------------------------------*/
void	moveObject(myContPad *mc)
{	
  /*---- Move with the 3D stick. ----*/
  moveBy3DStick(mc, &Ac.objX, &Ac.objY, 1);
  
  /*----- Slight move with the cross key. ----*/
  moveByPlusKey(mc->push, &Ac.objX, &Ac.objY, 1);
  
  /*---- Scaling by the C Button. ----*/
  moveByCButtons(mc->button, (s16 *)&Ac.scaleW, (s16 *)&Ac.scaleH, -32);
  if (Ac.scaleW <= 0) Ac.scaleW = 32;
  if (Ac.scaleH <= 0) Ac.scaleH = 32;
  
  /*---- The rotation by L R keys. ----*/
  if (mc->button & L_TRIG) Ac.theta -= 64;
  if (mc->button & R_TRIG) Ac.theta += 64;
  
  /*---- To the initialize position by START. ----*/
  if (mc->push & START_BUTTON){
    Ac.objX = (SCREEN_WD/2 - 16)<<2;
    Ac.objY = (SCREEN_HT/2 - 16)<<2;
    Ac.scaleW = Ac.scaleH = 1<<10;
    Ac.theta = 0;
  }
  return;
}

/*---------------------------------------------------------------------------*
  For input of the cross key, return amount of moving with the multiplication.  
 *---------------------------------------------------------------------------*/
void	moveByPlusKey(u16 button, s16 *x, s16 *y, s16 factor)
{
  if (button & U_JPAD) (*y) -= factor;
  if (button & D_JPAD) (*y) += factor;
  if (button & L_JPAD) (*x) -= factor;
  if (button & R_JPAD) (*x) += factor;
}  

/*---------------------------------------------------------------------------*
  For input of the C Buttons, return amount of moving with the multiplication.    
 *---------------------------------------------------------------------------*/
void	moveByCButtons(u16 button, s16 *x, s16 *y, s16 factor)
{
  if (button & U_CBUTTONS) (*y) -= factor;
  if (button & D_CBUTTONS) (*y) += factor;
  if (button & L_CBUTTONS) (*x) -= factor;
  if (button & R_CBUTTONS) (*x) += factor;
} 

/*---------------------------------------------------------------------------*
  For the slope amount of 3D stick, return moving amount with the multiplication.    
 *---------------------------------------------------------------------------*/
void	moveBy3DStick(myContPad *p, s16 *x, s16 *y, s16 factor)
{
  static s16 MoveFactor[] = { -8,-8,-4,-2,-1, 0,0, +1,+2,+4,+8,+8 };
  s16	nx, ny;
  
  nx = (p->c->stick_x + 96) >> 4;
  ny = (p->c->stick_y + 96) >> 4;
  
  (*x) += MoveFactor[nx] * factor;
  (*y) -= MoveFactor[ny] * factor;
} 

/*---------------------------------------------------------------------------*
  The menu definition.  
 *---------------------------------------------------------------------------*/
char	*menu_mesg[NUM_MENU][8] = {
{ "Control Pad 1      => ", "Object            ", "Bg                ", 0 },
{ "OBJ Render Mode    => ", "NonAA+Point+1cycle", "NonAA+Point+Copy  ",
                            "NonAA+Bilerp      ", "AA+Bilerp         ", 0 },
{ "OBJ Render Mode 2  => ", "Opaque            ", "Translucent       ", 0 },
{ "OBJ Texture Type   => ", "RGBA16            ", "CI4               ", 0 },
{ "OBJ TextureLoad by => ", "LoadBlock         ", "LoadTile          ", 0 },
{ "OBJ Texture Window => ", "on                ", "off               ", 0 },
{ "OBJ Shrink panel   => ", "off               ", "-1                ",
                            "-2                ", 0 },
{ "OBJ Flip S         => ", "off               ", "on                ", 0 },
{ "OBJ Flip T         => ", "off               ", "on                ", 0 },

{ "BG Scalable        => ", "off               ", "on  (Emulated)    ",
                            "on  (GBI)         ", 0 },
{ "BG TextureLoad by  => ", "LoadBlock         ", "LoadTile          ", 0 },
{ "BG Texture Format  => ", "RGBA16            ", "CI8               ", 0 },
{ "BG Flip S          => ", "off               ", "on                ", 0 },
};

/*---------------------------------------------------------------------------*
The item-display of the menu.  
------------------------------------------------------------------------*/
void	printMenu(s16 id, s16 sw)
{
  /*---- Highlight if the display of menu line is sw=1. ----*/
  osSyncPrintf("\033[%d;1H%s %s%s\033[%d;1H\033[0m",
	       id+1, ((sw)? "\033[33;5m>>" : "  "),
	       menu_mesg[id][0], menu_mesg[id][1+aMenu[id]], NUM_MENU+1);
}

/*---------------------------------------------------------------------------*
  The initialization of the menu.  
 *---------------------------------------------------------------------------*/
void	menuInit(void)
{
  int	i;
  
  osSyncPrintf("\033[2J");	/* Clear the screen. */
  for (i = 1; i < NUM_MENU; i ++){
    aMenu[i] = 0;
    printMenu(i, 0);
  }
  printMenu(0, 1);
}

/*---------------------------------------------------------------------------*
  The menu operation by the controller.  
 *---------------------------------------------------------------------------*/
void	menuHandler(myContPad *mc)
{
  static s16 active = 0;
  
  /*---- Switch each option by A. ----*/
  if (mc->push & A_BUTTON){
    aMenu[active] ++;
    if (!menu_mesg[active][aMenu[active]+1]) aMenu[active] = 0;
    printMenu(active, 1);
  }
  
  /*---- Select the item by B. ----*/
  if (mc->push & B_BUTTON){
    printMenu(active, 0);
    active += (active < NUM_MENU-1)? 1 : 1-NUM_MENU;
    printMenu(active, 1);
  }
  return;
}

/*======== End of action.c ========*/
