
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

#include <ultra64.h>

#include "block.h"
#include "static.h"
#include "timer.h"

int TimerMinX, TimerMaxX;

void SelfScaleTimerBar(void)
{
  /* self scaling counter stuff, take out if it's too obnoxious */
  
  if ((1.0/TIMERUNDERFLOW) * TimePerFrame[0] / TIMERINTERVAL + 1.0 < TimerTicks)
    while (((1.0/TIMERUNDERFLOW) * TimePerFrame[0] / TIMERINTERVAL + 1.0 < TimerTicks) &&
	   (TimerTicks != MINTIMERTICKS))
      TimerTicks-=2;
  else if ((1.0/TIMEROVERFLOW) * TimePerFrame[0] / TIMERINTERVAL + 1.0 > TimerTicks)
    while (((1.0/TIMEROVERFLOW) * TimePerFrame[0] / TIMERINTERVAL + 1.0 > TimerTicks) &&
	   (TimerTicks != MAXTIMERTICKS))
      TimerTicks+=2;
}

void WriteTimerBar(void)
{
  float TickSpacing, sBarPercent, BarPercent, dBarPercent;
  int i;

  if (SelfScaleTimer) SelfScaleTimerBar();

  TimerMinX =  40.0 / 320.0 * (float) ScreenWidth;
  TimerMaxX = 280.0 / 320.0 * (float) ScreenWidth;

  TickSpacing = (TimerMaxX - TimerMinX) / (TimerTicks - 1.0);
  
  gDPPipeSync(glistp++);
  gDPSetCycleType(glistp++, G_CYC_FILL);

  BarPercent = TimePerFrame[0] / (TIMERINTERVAL * (TimerTicks - 1));
  sBarPercent = TimePerFrameSp[0] / (TIMERINTERVAL * (TimerTicks - 1));
  dBarPercent = TimePerFrameDp[0] / (TIMERINTERVAL * (TimerTicks - 1));

  if (BarPercent < 1.0)
    {
      /* Within bar counter */

      if (numtasks==1) {

        gDPSetFillColor(glistp++, 
		      GPACK_RGBA5551(0,127,0,1) << 16 | 
		      GPACK_RGBA5551(0,127,0,1));
      
        /* RDP time */
        if (UseUCode == 1) {  /* dram ucode */
          gDPFillRectangle(glistp++, 
		       TimerMinX, 
		       TIMERMINY, 
		       TimerMinX + (int) ((TimerMaxX - TimerMinX) * dBarPercent), 
		       (TIMERMAXY + TIMERMINY) /2);      
          gDPPipeSync(glistp++);
          gDPSetFillColor(glistp++, 
		       GPACK_RGBA5551(0,64,0,1) << 16 | 
		       GPACK_RGBA5551(0,64,0,1));
          gDPFillRectangle(glistp++, 
		       TimerMinX + (int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       (TIMERMAXY + TIMERMINY) /2, 
		       TimerMinX + (int) ((TimerMaxX - TimerMinX) * dBarPercent) +
				(int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       TIMERMAXY);      
        } else {
          gDPFillRectangle(glistp++, 
		       TimerMinX, 
		       TIMERMINY, 
		       TimerMinX + (int) ((TimerMaxX - TimerMinX) * BarPercent), 
		       TIMERMAXY);      
        }

        gDPPipeSync(glistp++);
        gDPSetFillColor(glistp++, 
		      GPACK_RGBA5551(0,0,127,1) << 16 | 
		      GPACK_RGBA5551(0,0,127,1));
      
        /* RSP time */
        gDPFillRectangle(glistp++, 
		       TimerMinX,
		       TIMERMINY + 2, 
		       TimerMinX + (int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       TIMERMAXY - 2);      


      } else { /* multiple tasks */
	int i;
	int barminxsp, barminxdp, barminx;
	
	barminxsp = barminxdp = barminx = TimerMinX;

        for (i=1; i<=numtasks;i++) {
          BarPercent = TimePerFrame[i] / (TIMERINTERVAL * (TimerTicks - 1));
          sBarPercent = TimePerFrameSp[i] / (TIMERINTERVAL * (TimerTicks - 1));
          dBarPercent = TimePerFrameDp[i] / (TIMERINTERVAL * (TimerTicks - 1));

          gDPPipeSync(glistp++);
          gDPSetFillColor(glistp++, 
		      GPACK_RGBA5551(0,127,0,1) << 16 | 
		      GPACK_RGBA5551(0,127,0,1));
          switch (UseUCode) {
	    case 1: /* dram */
              /* RDP time */
              gDPFillRectangle(glistp++, 
		       barminxdp, 
		       TIMERMINY, 
		       barminxdp + (int) ((TimerMaxX - TimerMinX) * dBarPercent), 
		       (TIMERMAXY + TIMERMINY) /2);      
              gDPPipeSync(glistp++);
              gDPSetFillColor(glistp++, 
		       GPACK_RGBA5551(0,64,0,1) << 16 | 
		       GPACK_RGBA5551(0,64,0,1));
              gDPFillRectangle(glistp++, 
		       barminx + (int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       (TIMERMAXY + TIMERMINY) /2, 
		       barminx + (int) ((TimerMaxX - TimerMinX) * dBarPercent) +
				(int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       TIMERMAXY);      
              gDPPipeSync(glistp++);
              gDPSetFillColor(glistp++, 
		      GPACK_RGBA5551(0,0,127,1) << 16 | 
		      GPACK_RGBA5551(0,0,127,1));

              /* RSP time */
              gDPFillRectangle(glistp++, 
		       barminx,
		       (TIMERMAXY+TIMERMINY)/2, 
		       barminx + (int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       TIMERMAXY - 2);      
              gDPFillRectangle(glistp++, 
		       barminxsp,
		       TIMERMINY + 2,
		       barminxsp + (int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       (TIMERMAXY+TIMERMINY)/2);      

	      barminx = barminx + (int) ((TimerMaxX - TimerMinX) * BarPercent);
	      barminxsp = barminxsp + (int) ((TimerMaxX - TimerMinX) * sBarPercent);
	      barminxdp = barminxdp + (int) ((TimerMaxX - TimerMinX) * dBarPercent); 
	      break;
	    case 0: /* xbus */
	    case 2: /* fifo */
              /* RDP time */
              gDPFillRectangle(glistp++, 
		       barminxdp, 
		       TIMERMINY, 
		       barminxdp + (int) ((TimerMaxX - TimerMinX) * dBarPercent), 
		       TIMERMAXY);      
              gDPPipeSync(glistp++);
              gDPSetFillColor(glistp++, 
		      GPACK_RGBA5551(0,0,127,1) << 16 | 
		      GPACK_RGBA5551(0,0,127,1));

              /* RSP time */
              gDPFillRectangle(glistp++, 
		       barminx,
		       TIMERMINY + 2, 
		       barminx + (int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       TIMERMAXY - 2);      

	      barminx = barminx + (int) ((TimerMaxX - TimerMinX) * sBarPercent);
	      barminxdp = barminxdp + (int) ((TimerMaxX - TimerMinX) * dBarPercent); 
	      break;
	  }

      
        }
	barminxsp = barminxdp = barminx = TimerMinX;
        for (i=1; i<numtasks;i++) {
          BarPercent = TimePerFrame[i] / (TIMERINTERVAL * (TimerTicks - 1));
          sBarPercent = TimePerFrameSp[i] / (TIMERINTERVAL * (TimerTicks - 1));
          dBarPercent = TimePerFrameDp[i] / (TIMERINTERVAL * (TimerTicks - 1));

          gDPPipeSync(glistp++);
          gDPSetFillColor(glistp++, 
		      GPACK_RGBA5551(127,0,0,1) << 16 | 
		      GPACK_RGBA5551(127,0,0,1));
          switch (UseUCode) {
	    case 1: /* dram */
              gDPFillRectangle(glistp++, 
		       barminxdp + (int) ((TimerMaxX - TimerMinX) * dBarPercent)-1, 
		       TIMERMINY, 
		       barminxdp + (int) ((TimerMaxX - TimerMinX) * dBarPercent), 
		       (TIMERMINY+TIMERMAXY)/2);      
              gDPFillRectangle(glistp++, 
		       barminxsp + (int) ((TimerMaxX - TimerMinX) * sBarPercent)-1,
		       TIMERMINY+2, 
		       barminxsp + (int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       (TIMERMAXY+TIMERMINY)/2);      
	      barminxsp = barminxsp + (int) ((TimerMaxX - TimerMinX) * sBarPercent);
	      barminxdp = barminxdp + (int) ((TimerMaxX - TimerMinX) * dBarPercent);
	      break;
	    case 0: /* xbus */
	    case 2: /* fifo */
              gDPFillRectangle(glistp++, 
		       barminx + (int) ((TimerMaxX - TimerMinX) * sBarPercent)-1, 
		       TIMERMINY+2, 
		       barminx + (int) ((TimerMaxX - TimerMinX) * sBarPercent), 
		       TIMERMAXY-2);      
	      barminx = barminx + (int) ((TimerMaxX - TimerMinX) * sBarPercent);
	      break;
	  }
        }
      }
    }
  else
    {
      /* Overflowed bar counter range */
      
      gDPPipeSync(glistp++);
      gDPSetFillColor(glistp++, 
		      GPACK_RGBA5551(127,0,0,1) << 16 | 
		      GPACK_RGBA5551(127,0,0,1));
      
      gDPFillRectangle(glistp++, 
		       TimerMinX, TIMERMINY, 
		       TimerMaxX, TIMERMAXY);
    }
  
 
  gDPPipeSync(glistp++);

  gDPSetFillColor(glistp++, 
		  GPACK_RGBA5551(0,0,0,1) << 16 | 
		  GPACK_RGBA5551(0,0,0,1));
  
  for (i=0; i<TimerTicks; i++)
    {      
      gDPFillRectangle(glistp++, 
		       TimerMinX + (int) TickSpacing*i, TIMERMINY, 
		       TimerMinX + (int) TickSpacing*i, TIMERMAXY);
    }

  gDPPipeSync(glistp++);
}
