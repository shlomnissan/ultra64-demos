/*====================================================================
 * static.c
 *
 * Synopsis:
 *
 * SGI Logo data translated from Inventor format.
 *
 * Copyright 1993, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics,
 * Inc.; the contents of this file may not be disclosed to third
 * parties, copied or duplicated in any form, in whole or in part,
 * without the prior written permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to
 * restrictions as set forth in subdivision (c)(1)(ii) of the Rights
 * in Technical Data and Computer Software clause at DFARS
 * 252.227-7013, and/or in similar or successor clauses in the FAR,
 * DOD or NASA FAR Supplement. Unpublished - rights reserved under the
 * Copyright Laws of the United States.
 *====================================================================*/

#include <ultra64.h>
#include "gfx.h"

/*
 * To get the most use out of the vertex cache, we have broken
 * up the vertices into groups of loads, and machine-generated
 * the gfx_tri.h file which does the loads and triangle draws
 * for that load below.
 */
#include "gfx_vtx.h"	/* all the vertices */

/*
 * This is the SGI logo display list. It depends on the initialization
 * state elsewhere for some graphics pipeline state assumptions.
 */
Gfx logo_dl[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_SHADE | G_ZBUFFER | G_CULL_BACK),
    gsDPSetCombineMode (G_CC_SHADE, G_CC_SHADE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),

#include "gfx_tri.h"	/* all the vtx loads and triangles */

    gsDPPipeSync(),
    gsSPEndDisplayList(),
};


/*
 * The background is two big triangles.
 */
static Vtx bg_vtx[4] =  {
        {        -30,  30, 0, 0, 0, 0, 0, 0, 0xff, 0xff		},
        {         30,  30, 0, 0, 0, 0, 0, 0, 0xff, 0xff		},
        {         30, -30, 0, 0, 0, 0, 0, 0, 0x10, 0xff		},
        {        -30, -30, 0, 0, 0, 0, 0, 0, 0x10, 0xff		},
};

Gfx bg_dl[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER),
    gsSPSetGeometryMode(G_SHADE),
    gsDPSetCombineMode (G_CC_SHADE, G_CC_SHADE),
    gsDPSetColorDither(G_CD_BAYER),	/* HW 1.0 should use 'G_CD_ENABLE' */
    gsSPVertex(&bg_vtx, 4, 0),
    gsSP1Triangle(0, 2, 1, 0),
    gsSP1Triangle(0, 3, 2, 0),
    gsSPEndDisplayList(),
};



