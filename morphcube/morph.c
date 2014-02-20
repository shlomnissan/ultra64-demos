/*
 * morph.c -- R4300 Morph Routines
 */

#include <ultra64.h>

/*
 * Define DO_NORMALS if it's desirable to treat cn[]'s of verticies as
 * normals instead of as colors.
 */
#include "morphdemo.h"   /* Needed only for (possible) #def of 'DO_LIGHTING' */
#ifdef DO_LIGHTING
#define DO_NORMALS
#endif


/*
 * morph -- morph vertex positions
 *
 * Input:
 *        Vtx **valist   = list of input vertex arrays
 *        float *weights = weight for each array.  Should sum to 1.0.
 *        int vacnt      = number of vertex arrays (>= 2)
 *        Vtx *vout      = output vertex array (preallocated)
 *        int vcnt       = number of verticies per list
 *
 * Comments:
 *        1) Texture coordinates are copied from the first set of verticies
 *           For texture blending to work properly, texture coordinates for
 *           corresponding verticies should be the same.
 *        2) The flag value is copied from the first set of verticies.
 *        3) The routine is not necessarily perfectly optimized for speed,
 *           although it's pretty good.
 */

#if 1
/*
 * Reasonably fast version -- does all key arrays for a single vertex at a
 * time.
 */

/*
 * Note that simple averaging and renormalization of normals is NOT strictly
 * correct.  It is however a reasonably good approximation, especially if
 * the normals being averaged are of similar directions.
 * If the normals are in greatly different directions
 * (ex: opposite directions), the quality of the calculations can get
 * arbitrarily crummy.
 */

void morph(Vtx **valist, float *weights, int vacnt, Vtx *vout, int vcnt) {
  float w0, wj;
  float ob0, ob1, ob2, tc0, tc1, cn0, cn1, cn2, cn3;
  float n0f, n1f, n2f, mag, mag1;
  int i, j, k;
  Vtx *a0, *aj;  /* Current input array */
  OSTime mystart, myend, diff; /* For performance timing */
  float fac, fixup;
  int entry, sumsq;
  static setup = 0;
  static float table[435];
  int tbent, cn0i, cn1i, cn2i;

  mystart = osGetTime();
  w0 = weights[0];
  a0 = valist[0];
  for (i=0; i<vcnt; i++) {
    ob0 = w0 * a0[i].v.ob[0]; /* x */
    ob1 = w0 * a0[i].v.ob[1]; /* y */
    ob2 = w0 * a0[i].v.ob[2]; /* z */
    tc0 =      a0[i].v.tc[0]; /* texture u */
    tc1 =      a0[i].v.tc[1]; /* texture v */
#ifdef DO_NORMALS
    cn0 = w0 * a0[i].n.n[0]; /* Normal X */
    cn1 = w0 * a0[i].n.n[1]; /* Normal Y */
    cn2 = w0 * a0[i].n.n[2]; /* Normal Z */
#else
    cn0 = w0 * a0[i].v.cn[0]; /* Color R */
    cn1 = w0 * a0[i].v.cn[1]; /* Color G */
    cn2 = w0 * a0[i].v.cn[2]; /* Color B */
#endif
    cn3 = w0 * a0[i].v.cn[3]; /* Alpha */

    for (j=1; j<vacnt; j++) {
      wj = weights[j];
      aj = valist[j];
      ob0 += wj * aj[i].v.ob[0]; /* x */
      ob1 += wj * aj[i].v.ob[1]; /* y */
      ob2 += wj * aj[i].v.ob[2]; /* z */
#ifdef DO_NORMALS
      cn0 += wj * aj[i].n.n[0]; /* normal X */
      cn1 += wj * aj[i].n.n[1]; /* normal Y */
      cn2 += wj * aj[i].n.n[2]; /* normal Z */
#else
      cn0 += wj * aj[i].v.cn[0]; /* Color R */
      cn1 += wj * aj[i].v.cn[1]; /* Color G */
      cn2 += wj * aj[i].v.cn[2]; /* Color B */
#endif
      cn3 += wj * aj[i].v.cn[3]; /* Alpha */
    } /* for j */

#ifdef DO_NORMALS
    /* Normalize the normal
     * Note: If artifacts appear in lighting, it might be fixable by
     * tweaking this code to produce slightly smaller normals.
     */
#if 0
    /* Slower, more accurate renormalization */
    n0f = cn0 / 128.0;
    n1f = cn1 / 128.0;
    n2f = cn2 / 128.0;
    mag = sqrtf(n0f*n0f + n1f*n1f + n2f*n2f);
    mag1= 1.0/mag; 
    n0f *= mag1 * 128.0;
    n1f *= mag1 * 128.0;
    n2f *= mag1 * 128.0;
    if (n0f > 127.0) n0f = 127.0;
    if (n1f > 127.0) n1f = 127.0;
    if (n2f > 127.0) n2f = 127.0;
    cn0 = n0f;
    cn1 = n1f;
    cn2 = n2f;
#else
    /* Faster, slightly less accurate renormalization */
    if (!setup) {
      /* set up table */
      setup = 1;
      for (tbent = 0; tbent < 435; tbent++) {
	table[tbent] = (1.0/sqrtf((float)(tbent*64)))*128.0;
      }
    }
    sumsq = cn0*cn0 + cn1*cn1 + cn2*cn2;
    /* Range reduction to improve accuracy */
    if (sumsq < 1730) {
      if (sumsq < 108) {
	sumsq *= 256;
	fixup = 0.0625;
      } else {
	sumsq *= 16;
	fixup = 0.25;
      }
    } else {
      fixup = 1.0;
    }
    if (sumsq > 27712) {
      /* Out of range -- should never hit this */
      osSyncPrintf("SQRT out of range!\n");
      sumsq = 27712;
    }
    entry = (sumsq + 32) >> 6;
    /*
     * Table lookup.  Table has entries from 0-434.
     * function represented is: table[entry] = 128/sqrt(sumsq)
     */
    fac = table[entry] * fixup;
    cn0 = cn0*fac + 0.5;
    cn1 = cn1*fac + 0.5;
    cn2 = cn2*fac + 0.5;
    /* Clamp to allowed range */
    if (cn0 > 127.0) cn0 = 127.0;
    if (cn1 > 127.0) cn1 = 127.0;
    if (cn2 > 127.0) cn2 = 127.0;
    if (cn0 < -128.0) cn0 = -128.0;
    if (cn1 < -128.0) cn1 = -128.0;
    if (cn2 < -128.0) cn2 = -128.0;
#endif
#endif

    vout[i].v.ob[0] = ob0;
    vout[i].v.ob[1] = ob1;
    vout[i].v.ob[2] = ob2;
    vout[i].v.flag  = 0;
    vout[i].v.tc[0] = tc0;
    vout[i].v.tc[1] = tc1;
#ifdef DO_NORMALS
    vout[i].n.n[0] = cn0;
    vout[i].n.n[1] = cn1;
    vout[i].n.n[2] = cn2;
#else
    vout[i].v.cn[0] = cn0;
    vout[i].v.cn[1] = cn1;
    vout[i].v.cn[2] = cn2;
#endif
    vout[i].v.cn[3] = cn3;
    
  } /* for i */

  myend = osGetTime();
  diff = myend-mystart;
#if 0
  osSyncPrintf("morph =%lli nsec (%lli cycles)\n", CYCLES_TO_NSEC(diff), diff);
#endif

} /* morph */

#endif

#if 0
/* Faster version -- assumes vacnt == 2, doesn't do normals */

void morph(Vtx **valist, float *weights, int vacnt, Vtx *vout, int vcnt) {
  float w0, wj;
  float ob0, ob1, ob2, tc0, tc1, cn0, cn1, cn2, cn3;
  int i, j, k;
  Vtx *a0, *aj;  /* Current input array */
  OSTime mystart, myend, diff; /* For performance timing */

  if (vacnt != 2) {
    osSyncPrintf("Uh oh\n");
    return;
  }

  mystart = osGetTime();
  w0 = weights[0];
  a0 = valist[0];
  for (i=0; i<vcnt; i++) {
    ob0 = w0 * a0[i].v.ob[0]; /* x */
    ob1 = w0 * a0[i].v.ob[1]; /* y */
    ob2 = w0 * a0[i].v.ob[2]; /* z */
    tc0 =      a0[i].v.tc[0]; /* texture u */
    tc1 =      a0[i].v.tc[1]; /* texture v */
    cn0 = w0 * a0[i].v.cn[0]; /* Color R */
    cn1 = w0 * a0[i].v.cn[1]; /* Color G */
    cn2 = w0 * a0[i].v.cn[2]; /* Color B */
    cn3 = w0 * a0[i].v.cn[3]; /* Color A */

    wj = weights[1];
    aj = valist[1];
    ob0 += wj * aj[i].v.ob[0]; /* x */
    ob1 += wj * aj[i].v.ob[1]; /* y */
    ob2 += wj * aj[i].v.ob[2]; /* z */
    cn0 += wj * aj[i].v.cn[0]; /* Color R */
    cn1 += wj * aj[i].v.cn[1]; /* Color G */
    cn2 += wj * aj[i].v.cn[2]; /* Color B */
    cn3 += wj * aj[i].v.cn[3]; /* Color A */

    vout[i].v.ob[0] = ob0;
    vout[i].v.ob[1] = ob1;
    vout[i].v.ob[2] = ob2;
    vout[i].v.flag  = 0;
    vout[i].v.tc[0] = tc0;
    vout[i].v.tc[1] = tc1;
    vout[i].v.cn[0] = cn0;
    vout[i].v.cn[1] = cn1;
    vout[i].v.cn[2] = cn2;
    vout[i].v.cn[3] = cn3;
    
  } /* for i */

  myend = osGetTime();
  diff = myend-mystart;
#if 0
  osSyncPrintf("morph =%lli nsec (%lli cycles)\n", CYCLES_TO_NSEC(diff), diff);
#endif

} /* morph */

#endif



