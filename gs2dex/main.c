/*---------------------------------------------------------------------
	Copyright (C) 1997, Nintendo.
	
	File		main.c
	Coded    by	Yoshitaka Yasumoto.	Feb  6, 1997.
	Modified by	
	Comments	
	
	$Id: main.c,v 1.20 1997/08/07 03:24:17 yasu Exp $
  ---------------------------------------------------------------------*/
#include	<ultra64.h>
#include	<PR/gs2dex.h>
#include	"system.h"
#include	"texture.h"
#include	"action.h"
#include	"uc_assert.h"
#include	"sprite.h"

extern	Gfx	rdpInit_dl[];
extern	Gfx	clearCfb_dl[];
extern	Gfx	spriteInit_dl[];

void	setBg(void);
void	setObject(void);
void	loadSegment(u32, u32);

/*---------------------------------------------------------------------------*
 *	The setting of outputting the DL process log. 
 *---------------------------------------------------------------------------*/
#ifdef	RSP_DEBUG
# define	S2DEX_UCODE(x)	gspS2DEX_fifo_d##x##Start
# define	GFXLOGSIZE	1024
u64		GfxLog[GFXLOGSIZE];
#else
# define	S2DEX_UCODE(x)	gspS2DEX_fifo##x##Start
# define	GfxLog		0
#endif

/*---------------------------------------------------------------------------*
 * The definition of the task data.  
*---------------------------------------------------------------------------*/
OSTask	tlist = {
  M_GFXTASK,					/* task type                */
  OS_TASK_DP_WAIT|OS_TASK_LOADABLE,		/* task flags               */
  (u64 *)&rspbootTextStart,			/* boot ucode ptr           */
  SP_BOOT_UCODE_SIZE,				/* boot ucode size          */
  (u64 *)&S2DEX_UCODE(Text),			/* ucode ptr                */
  SP_UCODE_SIZE,				/* ucode size               */
  (u64 *)&S2DEX_UCODE(Data),			/* ucode data ptr           */
  SP_UCODE_DATA_SIZE,				/* ucode data size          */
  NULL,						/* dram stack      (Unused.)*/
  0,						/* dram stack size (Unused.)*/
  (u64 *)system_rdpfifo,			/* fifo buffer top          */
  (u64 *)system_rdpfifo+RDPFIFO_SIZE,		/* fifo buffer bottom       */
  NULL,						/* data ptr  (Set it later.)*/
  (u32)GfxLog,	/* Specify dl log ptr in S2DEX. data size  (No setting required.) */
  (u64 *)system_rspyield,			/* yield data ptr           */
  OS_YIELD_DATA_SIZE,				/* yield data size          */
};

/*---------------------------------------------------------------------------*
 * The area of the graphics list.  
 *---------------------------------------------------------------------------*/
Gfx	glist[2][GLIST_LEN];

/*---------------------------------------------------------------------------*
 * The drawing mode.  The parameter table.  
 *---------------------------------------------------------------------------*/
static	struct	{
  u32	antiAlias;
  u32	txtrFilter;
  u32	cycleType;
  u32	objRMode;
} RMmodeTable[] = {
{ 0,G_TF_POINT, G_CYC_1CYCLE,0 },                      /* NonAA+Point+1cycle */
{ 0,G_TF_POINT, G_CYC_COPY,  0 },                      /* NonAA+Point+Copy   */
{ 0,G_TF_BILERP,G_CYC_1CYCLE,G_OBJRM_BILERP},          /* NonAA+Bilerp       */
{ 1,G_TF_BILERP,G_CYC_1CYCLE,G_OBJRM_BILERP}	       /* AA+Bilerp          */
};

u32	ShrinkTable[] = { 0, G_OBJRM_SHRINKSIZE_1, G_OBJRM_SHRINKSIZE_2 };
u32	bg16seg, bg8seg;

/*---------------------------------------------------------------------------*
 * M A I N 
 *---------------------------------------------------------------------------*/
void	Main(void *arg)
{
  u8	draw_frame = 0;
  u32	objRM;
  Gfx	*gp, *gtop;
  OSTime rspstart;
  u32	 rsptime, rdptime;
  
  bg16seg = (u32)_codeSegmentEnd 
          + (u32)_staticSegmentEnd - (u32)_staticSegmentStart;
  bg8seg  = bg16seg + 
          + (u32)_bg_rgbaSegmentRomEnd - (u32)_bg_rgbaSegmentRomStart;
  
  loadSegment(bg16seg, bg8seg);
  menuInit();
  actionInit();
  rsptime = rdptime = 0;
  
  while(1){

    /*------ Start to read the controller. ------*/
    osContStartReadData(&siMessageQ);
    
    /*------ Wait for the retrace. ------*/
    osRecvMesg(&retraceMessageQ, NULL, OS_MESG_BLOCK);

    /*------ Setting the Bg structure. ------*/
    setBg();
    
    /*------ Setting the Object structure. ------*/
    setObject();
    
    /*------ Create the Gfx list. ------*/
    gtop = gp = glist[draw_frame];

    /*------ RSP initialization setting. ------*/
    gSPSegment(gp ++, 0, 0x0);
    gSPSegment(gp ++, STATIC_SEGMENT, _codeSegmentEnd);
    if (aMenu[MENU_BG_TX_FORMAT]){
      gSPSegment(gp ++, BG_SEGMENT, bg8seg);
    }
    gDPSetColorImage(gp ++, G_IM_FMT_RGBA,
		     G_IM_SIZ_16b, SCREEN_WD, system_cfb[draw_frame]);
    gSPDisplayList(gp ++, rdpInit_dl);
    gSPDisplayList(gp ++, clearCfb_dl);
    gSPDisplayList(gp ++, spriteInit_dl);

    /*------ Bg output. ------*/
    if (aMenu[MENU_BG_TX_FORMAT]){
      gDPSetTextureLUT(gp ++, G_TT_RGBA16);
      gSPObjLoadTxtr(gp ++, &objBgTLUT);
    }
    if (aMenu[MENU_BG_SCALABLE] == 0){

      /* Unscalable BG plane */
      gDPSetRenderMode(gp ++, G_RM_NOOP, G_RM_NOOP2);
      gDPSetCycleType(gp ++, G_CYC_COPY);
      gSPBgRectCopy(gp ++, &objBg);

    } else {
      /* Scalable BG plane */
      gDPSetRenderMode(gp ++, G_RM_SPRITE, G_RM_SPRITE2);
      gDPSetCycleType(gp ++, G_CYC_1CYCLE);
      gDPSetTextureFilter(gp ++,
			  RMmodeTable[aMenu[MENU_RENDERMODE]].txtrFilter);
      if (aMenu[MENU_BG_SCALABLE] == 1){
	/* Emulated by CPU */
	guS2DEmuSetScissor(0, 0, SCREEN_WD, SCREEN_HT, 
			   (RMmodeTable[aMenu[MENU_RENDERMODE]].txtrFilter
			    == G_TF_BILERP));
	guS2DEmuBgRect1Cyc(&gp, &objBg);
      } else {
	/* GBI command */
	gSPObjRenderMode(gp ++, RMmodeTable[aMenu[MENU_RENDERMODE]].objRMode);
	gSPBgRect1Cyc(gp ++, &objBg);
      }
    }
    gDPPipeSync(gp ++);

    /*------ Setting the Render Mode. ------*/
    objRM = RMmodeTable[aMenu[MENU_RENDERMODE]].objRMode;
    if (RMmodeTable[aMenu[MENU_RENDERMODE]].cycleType != G_CYC_COPY){
      if (!aMenu[MENU_RENDERMODE_2]){
	/* Opaque */
	if (RMmodeTable[aMenu[MENU_RENDERMODE]].antiAlias){
	  gDPSetRenderMode(gp ++, G_RM_AA_SPRITE, G_RM_AA_SPRITE2);
	} else {
	  gDPSetRenderMode(gp ++, G_RM_SPRITE, G_RM_SPRITE2);
	}
      } else {
	/* Translucent */
	if (RMmodeTable[aMenu[MENU_RENDERMODE]].antiAlias){
	  gDPSetRenderMode(gp ++, G_RM_AA_XLU_SPRITE, G_RM_AA_XLU_SPRITE2);
	} else {
	  gDPSetRenderMode(gp ++, G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2);
	}
	gDPSetCombineMode(gp ++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
	gDPSetPrimColor(gp ++, 0, 0, 255, 255, 255, 128);
      }
    }
    
    /*------ Setting the Texture Filter and CycleType. ------*/
    gDPSetTextureFilter(gp ++, RMmodeTable[aMenu[MENU_RENDERMODE]].txtrFilter);
    gDPSetCycleType    (gp ++, RMmodeTable[aMenu[MENU_RENDERMODE]].cycleType );
    
    /*------ Setting the Texture Window. ------*/
    if (aMenu[MENU_OBJ_TX_WINDOW]) objRM |= G_OBJRM_NOTXCLAMP;

    /*------ Setting the Shrink. ------*/
    objRM |= ShrinkTable[aMenu[MENU_OBJ_SHRINK]];
    
    /*------ Setting the Object Render Mode. ------*/    
    gSPObjRenderMode(gp ++, objRM);
    
    /*------ Load setting of the Texture. -----*/
    if (!aMenu[MENU_OBJ_TX_TYPE]){
      gDPSetTextureLUT(gp ++, G_TT_NONE);
      gSPObjLoadTxtr(gp ++, (aMenu[MENU_OBJ_TX_LOAD_BY]
			     ? &objTxtrTile_RGBA16 : &objTxtrBlock_RGBA16));
    } else {
      gDPSetTextureLUT(gp ++, G_TT_RGBA16);
      gSPObjLoadTxtr(gp ++, (aMenu[MENU_OBJ_TX_LOAD_BY]
			     ? &objTxtrTile_CI4 : &objTxtrBlock_CI4));
      gSPObjLoadTxtr(gp ++, &objTLUT_CI4);
    }
    
    /*------ Output of Object:Rectangle1. ------*/
    gSPObjRectangle(gp ++, &(objRect[0]));

    if (RMmodeTable[aMenu[MENU_RENDERMODE]].cycleType != G_CYC_COPY){

      /*------ Output of Object:Rectangle2. ------*/
      gSPObjMatrix(gp ++, &(objMtx[0]));
      gSPObjSprite(gp ++, &(objRect[1]));

      /*------ Output of Object:Ball. ------*/      
      /* 
	 Ball is displayed by combining two sprite pieces.  Because of this, you need to change
	 the processing method by setting the Texture Filter.  

	 If the Texture Filter is PointSample, you don't have to specify SHRINKSIZE; but, If it
	 is Bilerp, it must be SHRINKSIZE_1.  When you specify SHRINKSIZE_1, the circumference
	 of Sprite for 0.5 texel is clamped.  The area excepted by this clamp becomes a part
	 that the Bilerp process gives no effect.     

	 Because you need to load the part of adjoining of Sprite twice in Bilerp, Ball draws
	 only for 64x63 texels.  It is important to understand the differences between
	 objBall[1] and objBall[2] well. 
      */
      if (!aMenu[MENU_RENDERMODE_2]){
	/* Draw one size larger to hide the joining part.  (Only opaque.)
	   It became unnecessary after S2DEX 1.05.   */
	/* objRM |= G_OBJRM_WIDEN; */
      }
      gDPPipeSync(gp ++);
      gDPSetTextureLUT(gp ++, G_TT_RGBA16);
      gSPObjLoadTxtr(gp ++, &objBallTLUT);
      if (RMmodeTable[aMenu[MENU_RENDERMODE]].txtrFilter == G_TF_POINT){
	gSPObjRenderMode(gp ++, objRM);
	gSPObjMatrix(gp ++, &(objMtx[2]));
	gSPObjLoadTxRectR(gp ++, &(objBall[0]));
	gSPObjLoadTxRectR(gp ++, &(objBall[1]));
	gSPObjMatrix(gp ++, &(objMtx[1]));
	gSPObjLoadTxSprite(gp ++, &(objBall[0]));
	gSPObjLoadTxSprite(gp ++, &(objBall[1]));
      } else {
	gSPObjRenderMode(gp ++, objRM|G_OBJRM_SHRINKSIZE_1);
	gSPObjMatrix(gp ++, &(objMtx[2]));
	gSPObjLoadTxRectR(gp ++, &(objBall[0]));
	gSPObjLoadTxRectR(gp ++, &(objBall[2]));
	gSPObjMatrix(gp ++, &(objMtx[1]));
	gSPObjLoadTxSprite(gp ++, &(objBall[0]));
	gSPObjLoadTxSprite(gp ++, &(objBall[2]));
      }
    }
    
    /*------ Output the processing meter. ------*/
    if (rsptime){
      gDPPipeSync(gp ++);
      gDPSetCycleType(gp ++, G_CYC_FILL);
      gDPSetRenderMode(gp ++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
      gDPSetFillColor(gp ++, (GPACK_RGBA5551(128,128,255,1) << 16 | 
			      GPACK_RGBA5551(128,128,255,1)));
      gDPFillRectangle(gp ++, 30, 201, 30+rsptime/100, 202);
      gDPFillRectangle(gp ++, 30, 205, 30+rdptime/100, 206);
      gDPSetFillColor(gp ++, (GPACK_RGBA5551(255,255,255,1) << 16 | 
			      GPACK_RGBA5551(255,255,255,1)));
      gDPFillRectangle(gp ++, 30,     200, 30,     207);
      gDPFillRectangle(gp ++, 30+166, 200, 30+166, 207);
    }
    gDPFullSync(gp ++);
    gSPEndDisplayList(gp ++);
    
    /*------ Execute the Gfx task. ------*/
    tlist.t.data_ptr = (u64 *)gtop;
    osWritebackDCache(gtop, ((u32)gp)-((u32)gtop));
    rspstart = osGetTime();
    osSpTaskStart(&tlist);
    
    /*------ Wait for the end. ------*/
    osRecvMesg(&rspMessageQ, NULL, OS_MESG_BLOCK);
    rsptime = OS_CYCLES_TO_NSEC(osGetTime() - rspstart) / 1000;

#ifdef	RSP_DEBUG
    /*------ The ASSERT process of the micro-code. ------*/
    if (ucCheckAssert()){
      ucDebugGfxLogPrint(&tlist);  	/* Output the process log of RSP, Gfx and DL. */
//	ucDebugRdpFifoPrint(&tlist); 	/* Output the RDP and fifo buffers.           */
//	ucDebugIMEMPrint(); 		/* Output IMEM.                     	      */
//      ucDebugDMEMPrint(); 		/* Output DMEM.                     	      */
      ucDebugAssertPrint();		/* Output Assert stop location, etc.          */
      ucDebugInfoPrint();		/* Output the work area for Debugging.        */
      while(1);
    }
#endif

#if 0
    /*------ Output the DEBUG information. ------*/
    if (Ac.pad[0].push & Z_TRIG){
      ucDebugRdpFifoPrint(&tlist); 	/* Output the RDP and fifo buffers.    */
      ucDebugInfoPrint();		/* Output the work area for Debugging. */
    }
#endif
    osRecvMesg(&rdpMessageQ, NULL, OS_MESG_BLOCK);
    rdptime = OS_CYCLES_TO_NSEC(osGetTime() - rspstart) / 1000;
    
    /* Switching the Frame. */
    osViSwapBuffer(system_cfb[draw_frame]);
    draw_frame ^= 1;

    /* Accept the controller data. */
    osRecvMesg(&siMessageQ, NULL, OS_MESG_BLOCK);
    osContGetReadData(contPad);
    
    /* The input process. */
    actionUpdate();
  }
}

/*---------------------------------------------------------------------------*
 * Setting the BG structure.  
 *---------------------------------------------------------------------------*/
void	setBg(void)
{
  /* Setting the BG structure. */
  objBg.b.imageX = Ac.imageX;
  objBg.b.imageY = Ac.imageY;
  objBg.b.frameX = Ac.frameX;
  objBg.b.frameY = Ac.frameY;
  objBg.b.frameW = Ac.frameW;
  objBg.b.frameH = Ac.frameH; 
  objBg.b.imageFlip = (aMenu[MENU_BG_FLIPS] ? G_BG_FLAG_FLIPS : 0);
  objBg.b.imageLoad =
    (aMenu[MENU_BG_TX_LOAD_BY] ? G_BGLT_LOADTILE : G_BGLT_LOADBLOCK);
  if (aMenu[MENU_BG_TX_FORMAT]){
    objBg.b.imagePtr = (u64 *)L2_TV;
    objBg.b.imageFmt = G_IM_FMT_CI;
    objBg.b.imageSiz = G_IM_SIZ_8b;
  } else {
    objBg.b.imagePtr = (u64 *)bg16seg;
    objBg.b.imageFmt = G_IM_FMT_RGBA;
    objBg.b.imageSiz = G_IM_SIZ_16b;
  }

  if (!aMenu[MENU_BG_SCALABLE]){
    /*
     *	If you call guS2DInitBg once at the initialization, you don't have to call it again
     *  except when changing any of imageFmt,imageSiz, imageLoad, imageW and frameW.  
     *	Here, you should call it each time because it is possible to change imageLoad.  
     */
    guS2DInitBg(&objBg);
  } else {
    objBg.s.imageYorig = Ac.imageYorig;
    objBg.s.scaleW     = Ac.frameScaleW;
    objBg.s.scaleH     = Ac.frameScaleH; 
  }
  osWritebackDCache(&objBg, sizeof(uObjBg));

  return;
}


/*---------------------------------------------------------------------------*
 * Setting of the Object structure.  
 *---------------------------------------------------------------------------*/
void	setObject(void)
{
  s32	Sin, Cos;
  u8	flags = ((aMenu[MENU_OBJ_FLIPS] ? G_OBJ_FLAG_FLIPS : 0)|
		 (aMenu[MENU_OBJ_FLIPT] ? G_OBJ_FLAG_FLIPT : 0));

  objRect[0].s.objX   = Ac.objX;
  objRect[0].s.objY   = Ac.objY;
  objRect[0].s.scaleW = Ac.scaleW;
  objRect[0].s.scaleH = Ac.scaleH;
  objRect[0].s.imageFlags = flags;
  
  objMtx[0].m.X = objMtx[1].m.X = Ac.objX + (64<<2);
  objMtx[0].m.Y = objMtx[1].m.Y = Ac.objY;
  objRect[1].s.scaleW = Ac.scaleW;
  objRect[1].s.scaleH = Ac.scaleH;
  objRect[1].s.imageFlags = flags;
  
  Cos = (s32)coss(Ac.theta)*2;
  Sin = (s32)sins(Ac.theta)*2;
  if (Cos == 0xfffe) Cos = 0x10000;
  if (Sin == 0xfffe) Sin = 0x10000;
  if (Cos ==-0xfffe) Cos =-0x10000;
  if (Sin ==-0xfffe) Sin =-0x10000;
  objMtx[0].m.A =    objMtx[0].m.D = Cos;
  objMtx[0].m.B = - (objMtx[0].m.C = Sin);
  
  objMtx[1].m.A =  Cos * 1024 / Ac.scaleW;
  objMtx[1].m.B = -Sin * 1024 / Ac.scaleH;
  objMtx[1].m.C =  Sin * 1024 / Ac.scaleW;
  objMtx[1].m.D =  Cos * 1024 / Ac.scaleH;  
  
  objMtx[2].m.X = Ac.objX;
  objMtx[2].m.Y = Ac.objY;
  objMtx[2].m.BaseScaleX = Ac.scaleW;
  objMtx[2].m.BaseScaleY = Ac.scaleH;

  if (!aMenu[MENU_OBJ_TX_TYPE]){
    objRect[0].s.imageFmt = objRect[1].s.imageFmt = G_IM_FMT_RGBA;
    objRect[0].s.imageSiz = objRect[1].s.imageSiz = G_IM_SIZ_16b;
    objRect[0].s.imageStride =
      objRect[1].s.imageStride = GS_PIX2TMEM(32, G_IM_SIZ_16b);
  } else {
    objRect[0].s.imageFmt = objRect[1].s.imageFmt = G_IM_FMT_CI;
    objRect[0].s.imageSiz = objRect[1].s.imageSiz = G_IM_SIZ_4b;
    objRect[0].s.imageStride =
      objRect[1].s.imageStride = GS_PIX2TMEM(32, G_IM_SIZ_4b);
  }
  
  osWritebackDCache(objRect, sizeof(uObjSprite)*2);
  osWritebackDCache(objMtx,  sizeof(uObjMtx)*3);
  
  return;
}

/*---------------------------------------------------------------------------*
 * The DMA load of the segment.  
 *---------------------------------------------------------------------------*/
void	loadSegment(u32 bg16seg, u32 bg8seg)
{
  /* Load the Static segment. */
  LoadSegment(static, _codeSegmentEnd);
  osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);  
  
  /* Loading of Bg and RGBA segments. */
  LoadSegment(bg_rgba, (char *)bg16seg);
  osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);  
  
  /* Loading of  Bg and CI segments. */
  LoadSegment(bg_ci, (char *)bg8seg);
  osRecvMesg(&dmaMessageQ, NULL, OS_MESG_BLOCK);  
}

/*======== End of main.c ========*/
