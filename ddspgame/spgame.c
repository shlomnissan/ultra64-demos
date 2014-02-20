
/**************************************************************************
 *                                                                        *
 *               Copyright (C) 1994, Silicon Graphics, Inc.               *
 *                                                                        *
 *  These coded instructions, statements, and computer programs  contain  *
 *  unpublished  proprietary  information of Silicon Graphics, Inc., and  *
 *  are protected by Federal copyright  law.  They  may not be disclosed  *
 *  to  third  parties  or copied or duplicated in any form, in whole or  *
 *  in part, without the prior written consent of Silicon Graphics, Inc.  *
 *                                                                        *
 *************************************************************************/

/*
 * File:	sptext.c
 * Creator:	rww@sgi.com
 * Create Date:	Mon Apr 17 11:45:57 PDT 1995
 *
 * VERY simple app, draws a bunch of text.
 *
 */

#include <ultra64.h>

#ifdef _EMULATOR
#   include <em.h> /* Only if using emulator */
#   include <verify.h>
#endif

#include <PR/os.h>
#include <PR/leo.h>
#include "cont.h"
#include "main.h"
#include "diskproc.h"
#include "font_ext.h"

#define	TIK_SCALE	(50.223)
#define	TIK2USEC(x)	((int)(x/TIK_SCALE))

s16		standby = 3, sleep = 1;
LEODiskTime    	RTCdata, RTCdata2;
int		cursor = 0;
int		frmcnt = 0;

static u8	*func_str[] = {
  "LeoCreateLeoManager()",
  "LeoReadWrite()(WRITE)",
  "LeoReadWrite()(READ)",
  "LeoSeek()",
  "LeoRezero()",
  "LeoReadDiskID()",
  "LeoReadRTC()",
  "LeoSetRTC()",
  "LeoModeSelectAsync()",
  "LeoSpdlMotor()(SLEEP)",
  "LeoSpdlMotor()(ACTIVE)",
  "LeoTestUnitReady()",
};

void do_bkg( Gfx ** );
void do_game( Gfx ** );
void test_64dd( Gfx ** );

void
sp_do_work( Gfx **pglistp )
{
    Gfx *gp;

    gp = *pglistp;

    spInit( &gp );			/* Initialize Sprites */

    do_bkg( &gp );

    do_game( &gp );   /* */

    test_64dd( &gp );   /* Test for 64DD */
    
    spFinish( &gp );		

    gp--;

    *pglistp = gp;

    frmcnt ++;
}

void
do_bkg( Gfx **pglistp )
{
    Gfx *gp, *dl;
    extern Sprite lit_sprite;

    gp = *pglistp;

    lit_sprite.rsp_dl_next   = lit_sprite.rsp_dl;

    spMove( &lit_sprite, 0,0 );
    spScale( &lit_sprite, 1.0, 1.0 );
    spSetAttribute( &lit_sprite, SP_FASTCOPY );

    dl = spDraw( &lit_sprite );

    gSPDisplayList(gp++, dl );

    *pglistp = gp;
}


#include "sp_ball.h"

extern Sprite ball_sprite;

typedef struct state_struct {
    Sprite *sp;		/* Sprite   */

    double size;	/* Radius   */

    double  s,vs;	/* Scaling */

    double  x, y;	/* Position */
    double vx,vy;	/* Velocity */

    double  e,ve;	/* Explosion */

    double  a,va;	/* Alpha */
} Sobj;

Sobj ball = {

	&ball_sprite,

	16,

	1.6, 0.00,

	10, 0,
	 -1,  -2,
	
	0, 0,		/* Don't explode yet! */

	255, 0,		/* Don't fade yet! */
    };

#define NUM_BALLS	(7)

Sobj balls[NUM_BALLS];

#define NUM_balls_GFX (NUM_DL(NUM_ball_BMS))

static Gfx balls_gfx[NUM_BALLS][NUM_balls_GFX];

void
init_game( void )
{
    int i;

    for( i=0; i<NUM_BALLS; i++ ) {
	balls[i] = ball;
	balls[i].x  += (200.0* i)/NUM_BALLS;
	balls[i].vx += (2.0  * i)/NUM_BALLS;
	balls[i].y  += (100.0*i)/NUM_BALLS;
	balls[i].vy += (4.0* i)/NUM_BALLS;

	balls[i].s  -= (0.5*i)/NUM_BALLS;
	balls[i].vs += (0.05* i+0.02)/NUM_BALLS;
/*	
*/
    };

}

void draw_sobj( Gfx **, Sobj * );
void move_sobj( Sobj *, int );

void
do_game( Gfx **pglistp )
{
    Gfx *gp;
    static int first = 1;
    int i;

    if( first ) {
	first = 0;
	init_game();
    };

    gp = *pglistp;

    for( i=0; i<NUM_BALLS; i++ ) {
	balls[i].sp->rsp_dl = &(balls_gfx[i][0]);
        spSetAttribute( balls[i].sp, SP_TRANSPARENT | SP_CUTOUT );
	draw_sobj( &gp, &(balls[i]) );
	move_sobj( &(balls[i]), i );
    };


    *pglistp = gp;
}

void
draw_sobj( Gfx **pglistp, Sobj *s )
{
    Sprite *sp;
    Gfx *gp, *dl;

    gp = *pglistp;

    sp = s->sp;

    sp->rsp_dl_next   = sp->rsp_dl;

    spMove( sp, (int)(s->x),(int)(s->y) );
    if( s->a == 255 )
	spScale( sp, 1.0/s->s, 1.0/s->s );
    else
	spScale( sp, s->s, s->s );

    sp->expx = (int) s->e;
    sp->expy = (int) s->e;
    sp->alpha = (int) s->a;

    dl = spDraw( sp );

    gSPDisplayList(gp++, dl );

    *pglistp = gp;
}

#define ACCEL (9.81/60)

void
move_sobj( Sobj *s , int i)
{

    s->s += s->vs;

    if( s->a == 255 )
	if( ((s->s < 0.5)       && (s->vs < 0)) ||
	    ((s->s > 5.0) && (s->vs > 0)) )
	    s->vs = -s->vs;

    s->x += s->vx;

    if( ((s->x < 20)       && (s->vx < 0)) ||
        ((s->x > (300-64)) && (s->vx > 0)) )
	s->vx = -s->vx;

    s->y += s->vy;

    if( ((s->y < 20)       && (s->vy < 0)) ||
        ((s->y > (220-64)) && (s->vy > 0)) )
	s->vy = -s->vy;

    s->vy += ACCEL;

    if( (s->y > (220-64)) && ( s->vy > -2 ) ) {	/* Jump! */
	s->vy -= 5;
	s->s   = 1.0/s->s;

	s->va = -1;
	s->ve = 1/8.0;
	s->vs = -s->s/270;
    };

    s->a += s->va;
    s->e += s->ve;

/*    s->s += s->vs; */

    if( (s->a <= 0) || (s->s <= 0) ) {	/* Explosion is over */
	s->ve = 0;
	s->va = 0;
	s->vs = ball.vs + (0.05* i + 0.03)/NUM_BALLS;

	s->e  = 0;
	s->s  = 1.6;
	s->a  = 255;
    };
}

#ifdef WALLS

void
check_walls( void)
{
    int i;
    double dt = 1.0/60;
    double tcur = cur->t;

    for(i=0; i<num_walls; i++) {
	if( collision( walls[i], cur ) ) {
	    i = (-1);
	    update_force( cur );

	};
    };
}

#endif /* WALLS */

u8
fromBCD(u8 data){
  return ((data >> 4) & 15)*10 + (data & 15);
}

u8
toBCD(u8 data){
  return ((u8)(data/10))*16 + (data % 10);
}

void
test_64dd( Gfx **pglistp)
{
  static int	menu_mode = 0, submode = 0;
  static s16	xpos = 0;
  static u16	year = 1997;
  static u8	month = 8;
  static u8	day = 18;
  static u8	hour = 12;
  static u8	minute = 30;
  static u8	second = 30;
  Gfx		*gp;
  int		i, j;
  u8		temp[32];

  gp = *pglistp;
  
  font_init( &gp );

  if ( disk_stat.mode == DISK_PROC_CONTINUE ) {
    osSendMesg(&diskMsgQ, (OSMesg)NULL, OS_MESG_BLOCK);
  }

  switch( menu_mode ) {
  case 0:
    switch ( submode ) {
    case 0:
      osSendMesg(&diskMsgQ, (OSMesg)NULL, OS_MESG_BLOCK);
      submode ++;
      break;
    case 1:
#ifndef START_FROM_CASSETTE
      if ( disk_stat.mode == DISK_NO_BUSY && disk_stat.errno == 0 ) {
	disk_stat.errno = 0;
	disk_stat.errptr = 0;
	disk_stat.errmes = 0;
	cursor = 6;
	osSendMesg(&diskMsgQ, (OSMesg)NULL, OS_MESG_BLOCK);
	submode ++;
      }
#else
      if ( disk_stat.mode == DISK_NO_BUSY && disk_stat.errno == 0 ) {
	disk_stat.errptr = 0;
	if ( osGetMemSize() < 0x00800000L ) {
	  disk_stat.errno = LEO_ERROR_RAMPACK_NOT_CONNECTED;
	  disk_stat.errmes = 18;
	  submode = 6;
	} else {
	  disk_stat.errno = 0;
	  disk_stat.errmes = 0;
	  cursor = 6;
	  osSendMesg(&diskMsgQ, (OSMesg)NULL, OS_MESG_BLOCK);
	  submode ++;
	}
      }
#endif
      break;
    case 2:
      if ( disk_stat.mode == DISK_NO_BUSY && 
	  (conts[0].nowtrg & A_BUTTON) != 0 ) {
	disk_stat.errno = 0;
	disk_stat.errptr = 0;
	disk_stat.errmes = 0;
	submode = 0;
	if ( disk_stat.flag == LEO_ERROR_REAL_TIME_CLOCK_FAILURE ) {
	  cursor = 7;
	  menu_mode = 3;
	} else {
	  cursor = 1;
	  menu_mode = 1;
	}
      }
    }
    font_set_color(255, 255, 255, 255 );
    font_set_pos( MES_CMD_X, MES_CMD_Y );
    font_show_string( &gp, func_str[cursor] );
    break;
  case 1:
    switch ( conts[0].nowtrg ) {
    case A_BUTTON:
      if ( cursor == 7  ) {
	menu_mode = 3;
	xpos = 0;
      } else if ( cursor == 8 ) {
	menu_mode = 4;
	xpos = 0;
      } else if (disk_stat.mode == DISK_NO_BUSY ) {
	menu_mode = 2;
	osSendMesg(&diskMsgQ, (OSMesg)NULL, OS_MESG_BLOCK);
      }
      break;
    case D_JPAD:
      if (++cursor >= sizeof(func_str)/sizeof(u8 *)) cursor = 1;
      break;
    case U_JPAD:
      if (--cursor < 1) cursor =  sizeof(func_str)/sizeof(u8 *) - 1;
      break;
    }
    
    font_set_transparent( 1 );
    font_set_win( 32, 1 );
    
    for ( i = 1 ; i < sizeof(func_str)/sizeof(u8 *) ; i++) {
      if (i == cursor) {
	font_set_color(180, 255, 220, 255 );
      } else {
	font_set_color(120, 160, 130, 255 );
      }
      font_set_pos( MES_CMD_X, MES_CMD_Y + MES_CMD_DY*(i-1) );
      font_show_string( &gp, func_str[i] );
    }
    break;
  case 2:
    if ( disk_stat.mode == DISK_NO_BUSY && 
	(conts[0].nowtrg & A_BUTTON) != 0 ) {
      disk_stat.errno = 0;
      disk_stat.errptr = 0;
      disk_stat.errmes = 0;
      if ( disk_stat.flag == LEO_ERROR_REAL_TIME_CLOCK_FAILURE ) {
	if ( cursor == 6 ) {
	  menu_mode = 3;
	} else {
	  cursor = 6;
	  osSendMesg(&diskMsgQ, (OSMesg)NULL, OS_MESG_BLOCK);
	}
      } else {
	menu_mode = 1;
      }
    } else {
      font_set_color(255, 255, 255, 255 );
      font_set_pos( MES_CMD_X, MES_CMD_Y );
      font_show_string( &gp, func_str[cursor] );
    }
    break;
  case 3:			/* SetRTC */
    font_set_color(255, 255, 255, 255 );
    font_set_pos( MES_CMD_X, MES_CMD_Y );
    font_show_string( &gp, func_str[cursor] );
    
    switch ( conts[0].nowtrg ) {
    case A_BUTTON:
      if (disk_stat.mode == DISK_NO_BUSY ) {
	menu_mode = 2;
	RTCdata.yearhi = toBCD( (u8)(year / 100));
	RTCdata.yearlo = toBCD( (u8)(year % 100));
	RTCdata.month = toBCD(month);
	RTCdata.day = toBCD(day);
	RTCdata.hour = toBCD(hour);
	RTCdata.minute = toBCD(minute);
	RTCdata.second = toBCD(second);
	osSendMesg(&diskMsgQ, (OSMesg)NULL, OS_MESG_BLOCK);
      }
      break;
    case B_BUTTON:
      menu_mode = 1;
      break;
    case R_JPAD:
      if ( xpos < 5) xpos ++;
      break;
    case L_JPAD:
      if ( xpos > 0) xpos --;
      break;
    }
    switch (xpos) {
    case 0:
      i = MES_DATE_X + 2*8;
      if ( (conts[0].nowtrg | conts[0].repeat) & U_JPAD ) {
	if (++year > 2095 ) year = 2095;
      } else if ( (conts[0].nowtrg | conts[0].repeat) & D_JPAD ) {
	if (--year < 1996 ) year = 1996;
      }
      break;
    case 1:
      i = MES_DATE_X + 5*8;
      if ( (conts[0].nowtrg | conts[0].repeat) & U_JPAD ) {
	if (month < 12 ) ++month;
      } else if ( (conts[0].nowtrg | conts[0].repeat) & D_JPAD ) {
	if (month > 1 ) --month;
      }
      break;	
    case 2:
      i = MES_DATE_X + 8*8;
      if ( (conts[0].nowtrg | conts[0].repeat) & U_JPAD ) {
	if (day < 31 ) ++day;
      } else if ( (conts[0].nowtrg | conts[0].repeat) & D_JPAD ) {
	if (day > 1 ) --day;
      }
      break;	
    case 3:
      i = MES_DATE_X + 11*8;
      if ( (conts[0].nowtrg | conts[0].repeat) & U_JPAD ) {
	if (hour < 23 ) ++hour;
      } else if ( (conts[0].nowtrg | conts[0].repeat) & D_JPAD ) {
	if (hour > 0 ) --hour;
      }
      break;	
    case 4:
      i = MES_DATE_X + 14*8;
      if ( (conts[0].nowtrg | conts[0].repeat) & U_JPAD ) {
	if (minute < 59 ) ++minute;
      } else if ( (conts[0].nowtrg | conts[0].repeat) & D_JPAD ) {
	if (minute > 0 ) --minute;
      }
      break;	
    case 5:
      i = MES_DATE_X + 17*8;
      if ( (conts[0].nowtrg | conts[0].repeat) & U_JPAD ) {
	if (second < 59 ) ++second;
      } else if ( (conts[0].nowtrg | conts[0].repeat) & D_JPAD ) {
	if (second > 0 ) --second;
      }
      break;	
    }
    font_set_color(255, 255, 255, 255 );
    font_set_pos( MES_DATE_X + 0, MES_DATE_Y  );
    sprintf(temp, "%04d/%02d/%02d %02d:%02d:%02d",
	    year, month, day,
	    hour, minute, second);
    font_show_string( &gp, temp );
    font_set_color(255, 255, 0, 255 );
    font_set_pos( i, MES_DATE_Y+8  );
    font_show_string( &gp, "--" );
    
    break;
  case 4:			/* LeoModeSelect */
    font_set_color(255, 255, 255, 255 );
    font_set_pos( MES_CMD_X, MES_CMD_Y );
    font_show_string( &gp, func_str[cursor] );
    
    switch ( conts[0].nowtrg ) {
    case A_BUTTON:
      if (disk_stat.mode == DISK_NO_BUSY ) {
	osSendMesg(&diskMsgQ, (OSMesg)NULL, OS_MESG_BLOCK);
	menu_mode = 2;
      }
      break;
    case B_BUTTON:
      menu_mode = 1;
      break;
    case U_JPAD:
      if (xpos == 0) {
	if (++standby > 10) {
	  standby = 10;
	}
      } else {
	if (++sleep > 10) {
	  sleep = 10;
	}
      }
      break;
    case D_JPAD:
      if (xpos == 0) {
	if (--standby < 0) {
	  standby = 0;
	}
      } else {
	if (--sleep < 0) {
	  sleep = 0;
	}
      }
      break;
    case R_JPAD:
    case L_JPAD:
      xpos ^= 1;
      break;
    }
    if ( xpos == 0 ) {
      i = 255;
      j = 0;
    } else {
      i = 0;
      j = 255;
    }
    font_set_color(255, 255, j, 255 );
    font_set_pos( MES_CMD_X + 16, MES_CMD_Y + 52 );
    font_show_string( &gp, "To STANDBY" );
    font_set_pos( MES_CMD_X + 36, MES_CMD_Y + 72 );
    sprintf(temp, "%2d", standby);
    font_show_string( &gp, temp );
    
    font_set_color(255, 255, i, 255 );
    font_set_pos( MES_CMD_X + 128, MES_CMD_Y + 52 );
    font_show_string( &gp, "To SLEEP" );
    font_set_pos( MES_CMD_X + 144, MES_CMD_Y + 72 );
    sprintf(temp, "%2d", sleep);
    font_show_string( &gp, temp );
  }

  /* Display Error No */
  font_set_color(255, 255, 255, 255 );
  
  if (disk_stat.errptr != 0 ) {
    for (i = 0 ; i < disk_stat.errptr ; i ++) {
      font_set_pos( 32 + i*28, 200 );
      sprintf(temp, "%02d", errbuf[i] );
      font_show_string( &gp, temp );
      if ( errbuf[i] == LEO_ERROR_POWERONRESET_DEVICERESET_OCCURED ) {
	font_set_pos( 32 + i*28, 180 );
	sprintf(temp, "%02d", disk_stat.counter );
	font_show_string( &gp, temp );
      }
    }
  }

  /* Display Error Messages */
  font_set_transparent( 0 );
  font_set_color(255, 255, 255, 255 );
  switch (disk_stat.errmes) {
  case 1:
    font_set_pos( MES_END_X, MES_END_Y );
    jfont_show_string( &gp, ERRMES_SEIJO_SHURYO );
    break;
  case 2:
    font_set_pos( MES_END_X, MES_END_Y );
    jfont_show_string( &gp, ERRMES_IJO_SHURYO );
    break;
  case 3:
    font_set_pos( MES_END_X, MES_END_Y );
    jfont_show_string( &gp, ERRMES_KYOSEI_SHURYO );
    break;
  case 4:
    font_set_pos( 96, 80 );
    jfont_show_string( &gp, ERRMES_ERROR_NUMBER );
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_TORIATUKAI_SETUMEISHO );

    font_set_pos( 178, 82 );
    sprintf(temp, "%02d", disk_stat.errno);
    font_show_string( &gp, temp );
    break;
  case 5:
    font_set_pos( 40, 88 );
    jfont_show_string( &gp, ERRMES_ACCESS_LAMP );
    font_set_pos( 40, 108 );
    jfont_show_string( &gp, ERRMES_DISK_WO_NUKANAIDE );
    break;
  case 6:
    font_set_pos( 40, 100 );
    jfont_show_string( &gp, ERRMES_DISK_GA_SOUNYUUSARETE );
    break;
  case 7:
    font_set_pos( 96, 80 );
    jfont_show_string( &gp, ERRMES_ERROR_NUMBER );
    font_set_pos( 40, 100 );
    jfont_show_string( &gp, ERRMES_ACCESS_LAMP );
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_DISK_WO_NUKANAIDE );
    font_set_pos( 40, 140 );
    jfont_show_string( &gp, ERRMES_MOITIDO_DISK_WO );
    font_set_pos( 40, 160 );
    jfont_show_string( &gp, ERRMES_KUDASAI );
    font_set_pos( 178, 82 );
    sprintf(temp, "%02d", disk_stat.errno);
    font_show_string( &gp, temp );
    break;
  case 8:
    font_set_pos( 96, 80 );
    jfont_show_string( &gp, ERRMES_ERROR_NUMBER );
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_DISK_GA_HAITTEMASUKA );
    font_set_pos( 178, 82 );
    sprintf(temp, "%02d", disk_stat.errno);
    font_show_string( &gp, temp );
    break;
  case 9:
    font_set_pos( 96, 80 );
    jfont_show_string( &gp, ERRMES_ERROR_NUMBER );
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_DISK_GA_CHUTO_HANPA );
    font_set_pos( 40, 140 );
    jfont_show_string( &gp, ERRMES_KOMARETEIRU_KANOSEI );
    font_set_pos( 40, 160 );
    jfont_show_string( &gp, ERRMES_ICHIDO_DISK_WO_NUITE );
    font_set_pos( 40, 180 );
    jfont_show_string( &gp, ERRMES_SASHIKONNDE );
    font_set_pos( 178, 82 );
    sprintf(temp, "%02d", disk_stat.errno);
    font_show_string( &gp, temp );
    break;
  case 10:
    font_set_pos( 40, 100 );
    jfont_show_string( &gp, ERRMES_MACHIGATTA_DISK );
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_KANOSEI_GA_ARIMASU );
    font_set_pos( 40, 140 );
    jfont_show_string( &gp, ERRMES_KUDASAI );
    break;
  case 11:
    font_set_pos( 40, 100 );
    jfont_show_string( &gp, ERRMES_GENZAI_SAKUSEICHU );
    break;
  case 12:
    font_set_pos( 40, 100 );
    jfont_show_string( &gp, ERRMES_JIKOKU_WO_SETTEI );
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_KUWASIKU_HA_SETUMEISHO );
    font_set_pos( 40, 140 );
    jfont_show_string( &gp, ERRMES_OYOMIKUDASAI );
    break;
  case 13:
    font_set_color(255, 255, 255, 255 );
    font_set_pos( MES_DATE_X + 0, MES_DATE_Y  );
    if ( RTCdata2.yearlo < 96 ) {
      i = 20;
    } else {
      i = 19;
    }
    sprintf(temp, "%02d%02d/%02d/%02d %02d:%02d:%02d",
	    i,
	    fromBCD(RTCdata2.yearlo),
	    fromBCD(RTCdata2.month),
	    fromBCD(RTCdata2.day),
	    fromBCD(RTCdata2.hour),
	    fromBCD(RTCdata2.minute),
	    fromBCD(RTCdata2.second) );
    font_show_string( &gp, temp );

    font_set_pos( MES_END_X, MES_END_Y );
    jfont_show_string( &gp, ERRMES_SEIJO_SHURYO );
    break;
  case 14:
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_DISK_GA_HAITTEMASUKA );
    break;
  case 15:
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_DISK_GA_SOUNYUUSARETE );
    break;
  case 16:
    font_set_pos( 80, 120 );
    sprintf(temp, "%s", "64DD DRIVE NOT READY!");
    font_show_string( &gp, temp );

    font_set_pos( 130, 82 );
    sprintf(temp, "ERROR %02d", disk_stat.errno);
    font_show_string( &gp, temp );
    break;
  case 17:
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_TOKEI_NO_DENCHI );
    break;
  case 18:
    font_set_pos( 96, 80 );
    jfont_show_string( &gp, ERRMES_ERROR_NUMBER );
    font_set_pos( 40, 120 );
    jfont_show_string( &gp, ERRMES_KAKCHO_RAM_PAK );
    font_set_pos( 40, 140 );
    jfont_show_string( &gp, ERRMES_IMASEN );

    font_set_pos( 178, 82 );
    sprintf(temp, "%02d", disk_stat.errno);
    font_show_string( &gp, temp );
    break;
  }
  
  font_finish( &gp );
  *pglistp = gp;
}
