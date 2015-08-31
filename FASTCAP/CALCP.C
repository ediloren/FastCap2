/*
Copyright (C) 2003 by the Board of Trustees of Massachusetts Institute of
Technology, hereafter designated as the Copyright Owners.
 
License to use, copy, modify, sell and/or distribute this software and
its documentation for any purpose is hereby granted without royalty,
subject to the following terms and conditions:
 
1.  The above copyright notice and this permission notice must
appear in all copies of the software and related documentation.
 
2.  The names of the Copyright Owners may not be used in advertising or
publicity pertaining to distribution of the software without the specific,
prior written permission of the Copyright Owners.
 
3.  THE SOFTWARE IS PROVIDED "AS-IS" AND THE COPYRIGHT OWNERS MAKE NO
REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, BY WAY OF EXAMPLE, BUT NOT
LIMITATION.  THE COPYRIGHT OWNERS MAKE NO REPRESENTATIONS OR WARRANTIES OF
MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF THE
SOFTWARE WILL NOT INFRINGE ANY PATENTS, COPYRIGHTS TRADEMARKS OR OTHER
RIGHTS. THE COPYRIGHT OWNERS SHALL NOT BE LIABLE FOR ANY LIABILITY OR DAMAGES
WITH RESPECT TO ANY CLAIM BY LICENSEE OR ANY THIRD PARTY ON ACCOUNT OF, OR
ARISING FROM THE LICENSE, OR ANY SUBLICENSE OR USE OF THE SOFTWARE OR ANY
SERVICE OR SUPPORT.
 
LICENSEE shall indemnify, hold harmless and defend the Copyright Owners and
their trustees, officers, employees, students and agents against any and all
claims arising out of the exercise of any rights under this Agreement,
including, without limiting the generality of the foregoing, against any
damages, losses or liabilities whatsoever with respect to death or injury to
person or damage to property arising from or out of the possession, use, or
operation of Software or Licensed Program(s) by LICENSEE or its customers.

*/

#include "../FCWindow.h" // Enrico
#include "mulGlobal.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#define XI 0
#define YI 1
#define ZI 2
#define EQUIV_TOL 1.0e-9
/*#define PLANAR_TOL 1.0e-3*/
#define PLANAR_TOL 1.0e-5
#define MAX_REAL 1.0e+20;

/* Obvious Constants. */
#define PI 3.1415927
#define TWOPI 6.2831853
#define HALFPI 1.5707963E+00

/* Constants to save typing. */
#define FIVE3 1.666666666667
#define SEVEN3 2.3333333333333
#define ONE6 0.16666666666667
#define ONE3 0.3333333333333
#define FT3 4.66666667


/* Defines breakpoints panel multipoles. */
#define LIMITFOURTH 9.0
#define LIMITSECOND 36.0

/* Constants used for Hart arctan approximation. */
#define B1 0.24091197
#define B2 3.7851122
#define B3 5.6770721
#define B4 5.6772854
#define B5 5.6770747

#define Dot_Product(V1,V2) (V1[XI]*V2[XI]+V1[YI]*V2[YI]+V1[ZI]*V2[ZI])
#define DotP_Product(V1,R,S,T) ((V1[XI])*(R)+(V1[YI])*(S)+(V1[ZI])*(T))


static int num2nd=0, num4th=0, numexact=0;
static int num2ndsav=0, num4thsav=0, numexactsav=0;

// static vars made global - Enrico
static int maxorder = 0;


initcalcp(panel_list)
  charge *panel_list;
{
  charge *pq, *npq;
  double vtemp[3];
  double length, maxlength, minlength, length20, length31, sum, sum2, delta;
  double normalize();
  int i, j, next;

#if JACDBG == ON
  FILE *foo;

  foo = fopen("corners.txt", "w");
#endif

  for(i=0, pq = panel_list; pq != NULL; pq = pq->next) i++;
#if JACDBG == ON
  viewprintf(stderr, "Initial number of panels = %d\n", i);
#endif

  pq = panel_list; 
  while (pq != NULL) {
    /* Calculate edge lengths. */
    maxlength = 0.0;
    minlength = MAX_REAL;
    for(i=0; i < pq->shape; i++) {    
      if(i == (pq->shape -1)) next = 0;
      else next = i + 1;
      for(sum= 0, j = 0; j < 3; j++) {
	delta = pq->corner[next][j] - pq->corner[i][j];
	sum += delta * delta;
      }
      pq->length[i] = length = sqrt(sum);
      maxlength = MAX(maxlength, length);
      minlength = MIN(minlength, length);
    }
    
    /* Get diags and lengths. */
    for(sum= 0, sum2 = 0, i = 0; i < 3; i++) {     
      pq->X[i] = delta = pq->corner[2][i] - pq->corner[0][i];
      sum += delta * delta;
      if(pq->shape == 3) pq->Y[i] = pq->corner[0][i] - pq->corner[1][i];      
      else {
	pq->Y[i] = delta = pq->corner[3][i] - pq->corner[1][i];      
	sum2 += delta * delta;
      }
    }
    length20 = sqrt(sum);
    length31 = sqrt(sum2);

    /* Check on lengths for quad. */
    if(pq->shape == 3) {
      pq->max_diag = maxlength;
      pq->min_diag = minlength;
    }
    else {
      length = MAX(length20, length31);
      pq->max_diag = length;
      pq->min_diag = MIN(length20, length31);
    }

    /* Z-axis is normal to two diags. */
    Cross_Product(pq->X, pq->Y, pq->Z);
/*#if 1 == 0*/
    if(flip_normal(pq)) {
      for(i = 0; i < 3; i++) {
	pq->Z[i] = -(pq->Z[i]);	/* flip the normal */
	pq->X[i] = -(pq->X[i]);	/* flip the x direction */
	/* interchange points 0 and 2 so that corner order will be
	   consistent with X flip (note this is OK for both quads and tris) */
	vtemp[0] = pq->corner[0][i];
	pq->corner[0][i] = pq->corner[2][i];
	pq->corner[2][i] = vtemp[0];
      }
      /* interchange length entries in length vector */
      vtemp[0] = pq->length[0];
      pq->length[0] = pq->length[1];
      pq->length[1] = vtemp[0];
      if(pq->shape == 4) {
	vtemp[0] = pq->length[2];
	pq->length[2] = pq->length[3];
	pq->length[3] = vtemp[0];
      }
    } 
/*#endif*/
    pq->area = 0.5 * normalize(pq->Z);
    normalize(pq->X);

    /* Real Y-axis is normal to X and Z (resulting system is left-handed). */
    Cross_Product(pq->X, pq->Z, pq->Y);

    /* Project the corner points into the plane defined by edge midpoints. */
    if(planarize(pq) == FALSE) {     
      /* Planarization too drastic, crack into two triangles. */
      CALLOC(npq, 1, charge, ON, AMSC);
      npq->next = pq->next;
      pq->next = npq;
      npq->shape = 3;
      pq->shape = 3;
      npq->cond = pq->cond;
      npq->surf = pq->surf;
      VCOPY(npq->corner[0], pq->corner[0]);
      VCOPY(npq->corner[1], pq->corner[2]);
      VCOPY(npq->corner[2], pq->corner[3]);
      continue;
    }

    /* Calculate the centroid. */
    centroid(pq, length20);      

    /* Put corners in the newly defined coord system. */
    for(i=0; i < pq->shape; i++) {
      pq->corner[i][XI] -= pq->x;
      pq->corner[i][YI] -= pq->y;
      pq->corner[i][ZI] -= pq->z;
    }
    for(i=0; i < pq->shape; i++) {
      vtemp[XI] = Dot_Product(pq->corner[i], pq->X);
      vtemp[YI] = Dot_Product(pq->corner[i], pq->Y);
      vtemp[ZI] = Dot_Product(pq->corner[i], pq->Z);
      pq->corner[i][XI] = vtemp[XI];
      pq->corner[i][YI] = vtemp[YI];
      pq->corner[i][ZI] = vtemp[ZI];
      if(fabs(pq->corner[i][ZI]) > (EQUIV_TOL * pq->min_diag)) {
	viewprintf(stderr, "FATAL PROGRAM ERROR: renormalized z=%g\n", pq->corner[i][ZI]);
	FCExit(FC_GENERIC_ERROR);
      }
      pq->corner[i][ZI] = 0.0;
    }

#if JACDBG == ON			/* dump corners, center to file */
    fprintf(foo, "%g %g %g: ", pq->x, pq->y, pq->z);
    for(i = 0; i < pq->shape; i++) {
      fprintf(foo, "%g %g %g ", pq->corner[i][XI], pq->corner[i][YI],
	      pq->corner[i][ZI]);
    }
    fprintf(foo, "\n");
#endif

    /* Finally, calculate the moments. */
    ComputeMoments(pq);

    /* Iterate for the next panel. */
    pq = pq->next;

  }

}

/*
  determine if normal needs to be flipped to get dielectric bdry cond right
*/
oldflip_normal(panel)
charge *panel;
{
  int i;
  double x, y, z;
  double ctr_minus_n[3], ctr_plus_n[3], norm_minus, norm_plus, norm, norm_sq;
  surface *surf = panel->surf;
  int ref_inside = surf->ref_inside, flip_normal;
  double *ref = surf->ref, *normal;
  char *surf_name = surf->name;

  if(surf->type != DIELEC && surf->type != BOTH) return(FALSE);

  /* get panel corner (relative to reference point) and normal */
  x = panel->corner[0][0] - ref[0]; 
  y = panel->corner[0][1] - ref[1]; 
  z = panel->corner[0][2] - ref[2];
  norm_sq = x*x + y*y + z*z;
  norm = sqrt(norm_sq);
  normal = panel->Z;

  /* add the (scaled) normal and negative normal to the panel corner */
  /* negative normal result should be closer to ref point if(ref_inside) */
  ctr_minus_n[0] = x - 0.1*norm*normal[0];
  ctr_minus_n[1] = y - 0.1*norm*normal[1];
  ctr_minus_n[2] = z - 0.1*norm*normal[2];
  ctr_plus_n[0] = x + 0.1*norm*normal[0];
  ctr_plus_n[1] = y + 0.1*norm*normal[1];
  ctr_plus_n[2] = z + 0.1*norm*normal[2];

  /* get norms of test points, one inside (minus) other out (plus) */
  norm_minus = ctr_minus_n[0]*ctr_minus_n[0];
  norm_plus = ctr_plus_n[0]*ctr_plus_n[0];
  for(i = 1; i < 3; i++) {
    norm_minus += ctr_minus_n[i]*ctr_minus_n[i];
    norm_plus += ctr_plus_n[i]*ctr_plus_n[i];
  }

  flip_normal = FALSE;
  if(norm_minus > norm_sq) {
    if(norm_plus > norm_sq) {
      viewprintf(stderr, 
	      "flip_normal: both test points on non-reference side\n");
      viewprintf(stderr, "  Surface: %s\n", hack_path(surf_name));
      viewprintf(stderr, "  Translation: (%g %g %g)\n", surf->trans[0],
	      surf->trans[1], surf->trans[2]);
      viewprintf(stderr, "  Reference point: (%g %g %g)\n",
	      ref[0], ref[1], ref[2]);
      viewprintf(stderr, "  Panel corner: (%g %g %g)\n",
	      panel->corner[0][0], panel->corner[0][1], panel->corner[0][2]);
      viewprintf(stderr, "  Normal: (%g %g %g)\n",
	      normal[0], normal[1], normal[2]);
      FCExit(FC_GENERIC_ERROR);
    }
    if(ref_inside) flip_normal = TRUE;
    }
  else if(norm_plus < norm_sq) {
    if(norm_minus < norm_sq) {
      viewprintf(stderr, 
	      "flip_normal: both test points on reference point side\n");
      viewprintf(stderr, "  Surface: %s\n", hack_path(surf_name));
      viewprintf(stderr, "  Translation: (%g %g %g)\n", surf->trans[0],
	      surf->trans[1], surf->trans[2]);
      viewprintf(stderr, "  Reference point: (%g %g %g)\n",
	      ref[0], ref[1], ref[2]);
      viewprintf(stderr, "  Panel corner: (%g %g %g)\n",
	      panel->corner[0][0], panel->corner[0][1], panel->corner[0][2]);
      viewprintf(stderr, "  Normal: (%g %g %g)\n",
	      normal[0], normal[1], normal[2]);
      FCExit(FC_GENERIC_ERROR);
    }	
    if(!ref_inside) flip_normal = TRUE;
  }

  return(flip_normal);
}

/*
  determine if normal needs to be flipped to get dielectric bdry cond right
  - this function uses 0.0 as a breakpoint when really machine precision
    weighted checks should be done (really not an issue if ref point far)
*/
flip_normal(panel)
charge *panel;
{
  int i;
  double x, y, z;
  double ctr_minus_n[3], ctr_plus_n[3], norm_minus, norm_plus, norm, norm_sq;
  surface *surf = panel->surf;
  int ref_inside = surf->ref_inside, flip_normal;
  double *ref = surf->ref, *normal, angle, norm_n;
  char *surf_name = surf->name;

  if(surf->type != DIELEC && surf->type != BOTH) return(FALSE);

  /* get panel corner (relative to reference point) and normal */
  x = panel->corner[0][0] - ref[0]; 
  y = panel->corner[0][1] - ref[1]; 
  z = panel->corner[0][2] - ref[2];
  norm_sq = x*x + y*y + z*z;
  norm = sqrt(norm_sq);
  normal = panel->Z;
  norm_n = 
      sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);

  /* figure the dot product between the normal and the corner-ref point
     - ref_inside and angle <= 90 => flip
     - ref_inside and angle  > 90 => no flip
     - !ref_inside and angle <= 90 => no flip 
     - !ref_inside and angle > 90 => flip */
  /*    figure angle */
  angle = (x*normal[0] + y*normal[1] + z*normal[2])/(norm*norm_n);
  if(ref_inside && angle <= 0.0) flip_normal = TRUE;
  else if(ref_inside && angle > 0.0) flip_normal = FALSE;
  else if(!ref_inside && angle <= 0.0) flip_normal = FALSE;
  else if(!ref_inside && angle > 0.0) flip_normal = TRUE;
  else {
    viewprintf(stderr, 
	    "flip_normal: inconclusive test for normal flipping\n");
    viewprintf(stderr, "  Surface: %s\n", hack_path(surf_name));
    viewprintf(stderr, "  Translation: (%g %g %g)\n", surf->trans[0],
	    surf->trans[1], surf->trans[2]);
    viewprintf(stderr, "  Reference point: (%g %g %g)\n",
	    ref[0], ref[1], ref[2]);
    viewprintf(stderr, "  Panel corner: (%g %g %g)\n",
	    panel->corner[0][0], panel->corner[0][1], panel->corner[0][2]);
    viewprintf(stderr, "  Normal: (%g %g %g)\n",
	    normal[0], normal[1], normal[2]);
    FCExit(FC_GENERIC_ERROR);
  }
    
  return(flip_normal);
}
  

/*
Changes the corner points so that they lie in the plane defined by the
panel diagonals and any midpoint of an edge.
*/
planarize(pq)
charge *pq;
{
  double origin[3], corner[3], delta[4][3], px, py, dx, dy, dz;
  int i, j, numcorners = pq->shape;
  double tolsq = PLANAR_TOL * pq->min_diag; 

  tolsq *= tolsq;

  /* Triangular panels are just fine already. */
  if(numcorners != 4) return(TRUE);

  /* Pick edge midpoint as origin. */
  for(i=0; i < 3; i++) origin[i] = 0.5 * (pq->corner[1][i] + pq->corner[0][i]);

  for(i=0; i < numcorners; i++) {
    for(j=0; j < 3; j++) corner[j] = pq->corner[i][j] - origin[j];
    px = Dot_Product(corner, pq->X);
    py = Dot_Product(corner, pq->Y);

    dx = px * pq->X[XI] + py * pq->Y[XI] + origin[XI] - pq->corner[i][XI];
    dy = px * pq->X[YI] + py * pq->Y[YI] + origin[YI] - pq->corner[i][YI];
    dz = px * pq->X[ZI] + py * pq->Y[ZI] + origin[ZI] - pq->corner[i][ZI];

    delta[i][XI] = dx;
    delta[i][YI] = dy;
    delta[i][ZI] = dz;
    
    /* Moved too much, crack the panel. */
    if((dx * dx + dy * dy + dz * dz) > tolsq) return(FALSE);
  }

  /* Still Here? Must be planarizing. */
  for(i=0; i < numcorners; i++) {
    for(j=0; j < 3; j++) {
      pq->corner[i][j] += delta[i][j];
    }
  }
  return(TRUE);
}


/* 
Determines centroid of a panel (defined as the point which make the
first moments vanish.  Calculation begins by projection into the
coordinate system defined by the panel normal as the z-axis and
edge02 as the x-axis.
*/
centroid(pp, x2)
charge *pp;
double x2;
{
  double vertex1[3], vertex3[3];
  double sum, dl, x1, y1, x3, y3, xc, yc;
  int i;

  /* Use vertex 0 as the origin. */
  for(i=0; i< 3; i++) {
    vertex1[i] = pp->corner[1][i] - pp->corner[0][i];
    if(pp->shape == 4) vertex3[i] = pp->corner[3][i] - pp->corner[0][i];
    else vertex3[i] = pp->corner[2][i] - pp->corner[0][i];
  }

  /* Project into the panel axes. */
  y1 = Dot_Product(vertex1, pp->Y);
  y3 = Dot_Product(vertex3, pp->Y);
  x1 = Dot_Product(vertex1, pp->X);
  x3 = Dot_Product(vertex3, pp->X);

  yc = ONE3 * (y1 + y3);
  xc = ONE3 * (x2 + ((x1 * y1 - x3 * y3)/(y1 - y3)));

  pp->x = pp->corner[0][XI] + xc * pp->X[XI] + yc * pp->Y[XI];
  pp->y = pp->corner[0][YI] + xc * pp->X[YI] + yc * pp->Y[YI];
  pp->z = pp->corner[0][ZI] + xc * pp->X[ZI] + yc * pp->Y[ZI];

}

double normalize(vector)
  double vector[3];
{
  double length;
  int i;

  length = sqrt( vector[0]*vector[0] 
		+ vector[1]*vector[1] 
		+ vector[2]*vector[2]);
    
  for (i=0; i<3; i++) vector[i] = vector[i] / length;

  return length;
}

/* Assumes the vectors are normalized. */
int If_Equal(vector1, vector2)
  double vector1[3], vector2[3];
{
  int i;

  for (i=0; i<3; i++)
    if (fabs(vector1[i] - vector2[i]) > EQUIV_TOL) return FALSE;   
  return TRUE;
}

/* Calculates result_vector = vector1 X vector2. */
Cross_Product(vector1, vector2, result_vector)
  double vector1[], vector2[], result_vector[];
{
  result_vector[XI] = vector1[YI]*vector2[ZI] - vector1[ZI]*vector2[YI];
  result_vector[YI] = vector1[ZI]*vector2[XI] - vector1[XI]*vector2[ZI];
  result_vector[ZI] = vector1[XI]*vector2[YI] - vector1[YI]*vector2[XI];
}

/*
Computes the potential at x, y, z due to a unit source on panel
and due to a dipole is returned in pfd.

Note, the code is subtle because there are 5 cases depending on
the placement of the collocation point

    CASE1: evaluation point projection strictly outside of panel (happens most
      of the time).
    CASE2: eval pnt proj. strictly inside panel (happens >= #panels times,
      at least for the self terms)
    CASE3: eval pnt proj. on a panel side, not on a corner (happens
      when paneled faces meet at right angles, also possible other ways)
    CASE4: eval pnt proj. on a panel corner (happens rarely to never)
    CASE5: eval pnt proj. on side extension (happens when paneled 
      faces meet at right angles, also possible other ways).
*/
double calcp(panel, x, y, z, pfd)
charge *panel;
double x, y, z, *pfd;
{
  double r[4], fe[4], xmxv[4], ymyv[4];
  double xc, yc, zc, zsq, xn, yn, zn, znabs, xsq, ysq, rsq, diagsq, dtol;
  double v, arg, st, ct, length, s1, c1, s2, c2, s12, c12, val;
  double *s;
  double rInv, r2Inv, r3Inv, r5Inv, r7Inv, r9Inv, zr2Inv;
  double ss1, ss3, ss5, ss7, ss9;
  double s914, s813, s411, s512, s1215;
  double fs, fd, fdsum;
  int okay, i, next;
  struct edge *edge;
  double *corner;

  /* Put the evaluation point into this panel's coordinates. */
  xc = x - panel->x;
  yc = y - panel->y;
  zc = z - panel->z;

  xn = DotP_Product(panel->X, xc, yc, zc);
  yn = DotP_Product(panel->Y, xc, yc, zc);
  zn = DotP_Product(panel->Z, xc, yc, zc);

  zsq = zn * zn;
  xsq = xn * xn;
  ysq = yn * yn;
  rsq = zsq + xsq + ysq;
  diagsq = panel->max_diag * panel->max_diag;

  if(rsq > (LIMITFOURTH * diagsq)) {
    fs = 0.0; fd = 0.0;
    s = panel->moments;
    /* First, second moments. */
    r2Inv = 1.0 / rsq;
    rInv = sqrt(r2Inv);
    r3Inv = r2Inv * rInv;
    r5Inv = r3Inv * r2Inv;
    zr2Inv = zn * r2Inv;
    ss1 = s[1] * rInv;
    ss3 = -(s[3] + s[10]) * r3Inv;
    ss5 = (xsq * s[10] + (xn * yn * s[7]) + ysq * s[3]) * r5Inv;
    fs = ss1 + ONE3 * ss3 + ss5;
    fdsum = ss1 + ss3 + 5.0 * ss5;
    fd = zr2Inv * fdsum;
    if(rsq < (LIMITSECOND * diagsq)) {
    /* Third and fourth moments added for diagsq/r2 between 40 and 150. */
      s914 = s[9] + s[14];
      s813 = s[8] + s[13];
      s411 = s[4] + s[11];
      s512 = s[5] + s[12];
      s1215 = s[12] + s[15];
      r7Inv = r5Inv * r2Inv;
      r9Inv = r7Inv * r2Inv;
      ss5 = (-xn * s813 - yn * s411 + 0.1 * (s512 + s1215)) * r5Inv;

      ss7 = (FIVE3 *((xn * xsq * s[13] + yn * ysq * s[4]) 
		     + 3.0 * xn * yn * (xn * s[11]  +  yn * s[8]))
		     - xsq * s1215 - ysq * s512 - xn * yn * s914) * r7Inv;

      ss9 = (7.0 * (ONE6 * (xsq * xsq * s[15] + ysq * ysq * s[5])
		    + xsq * ysq * s[12])
	     + SEVEN3 * xn * yn * (xsq * s[14] + ysq * s[9])) * r9Inv;

      fs += ss5 + ss7 + ss9;
      fdsum = 5.0 * ss5 + 7.0 * ss7 + 9.0 * ss9;
      fd += zr2Inv * fdsum;
      num4th++;
    }
    else num2nd++;
  }
  else {
    dtol = EQUIV_TOL * panel->min_diag;
    znabs = fabs(zn);

    /* Always move the evaluation point a little bit off the panel. */
    if(znabs < dtol) { 
      zn = 0.5 * dtol;  /* Half of dtol insures detection for zero dipole. */
      znabs = 0.5 * dtol;
    }

    /* Once per corner computations. */
    for(okay = TRUE, i=0; i < panel->shape; i++) {
      corner = panel->corner[i]; 
      xmxv[i] = xc = xn - corner[XI];
      ymyv[i] = yc = yn - corner[YI];
      zc = zn - corner[ZI];
      fe[i] = xc * xc + zc * zc;
      r[i] = sqrt(yc * yc + fe[i]);
      if(r[i] < (1.005 * znabs)) {  /* If r almost z, on vertex normal. */
	okay = FALSE;
      }
    }

    /* Once per edge computations. */
    fs = 0.0; fd = 0.0;
    for(i=0; i < panel->shape; i++) {
      if(i == (panel->shape - 1)) next = 0;
      else next = i + 1;

      /* Now calculate the edge contributions to a panel. */
      length = panel->length[i];
      ct = (panel->corner[next][XI] - panel->corner[i][XI]) / length;
      st = (panel->corner[next][YI] - panel->corner[i][YI]) / length;

      /* v is projection of eval-i edge onto perpend to next-i edge. */
      /* Exploits the fact that corner points in panel coordinates. */
      v = xmxv[i] * st - ymyv[i] * ct;

      /* arg == zero if eval on next-i edge, but then v = 0. */
      arg = (r[i] + r[next] - length)/(r[i] + r[next] + length);
      if(arg > 0.0) fs -= v * log(arg);

      /* Okay means eval not near a vertex normal, Use Hess-Smith. */
      if(okay) {
	s1 = v * r[i];
	c1 = znabs * (xmxv[i] * ct + ymyv[i] * st);
	s2 = v * r[next];
	c2 = znabs * (xmxv[next] * ct + ymyv[next] * st);
      }
      /* Near a vertex normal, use Newman. */
      else {
	s1 = (fe[i] * st) - (xmxv[i] * ymyv[i] * ct);
	c1 = znabs * r[i] * ct;
	s2 = (fe[next] * st) - (xmxv[next] * ymyv[next] * ct);
	c2 = znabs * r[next] * ct;
      }    

      s12 = (s1 * c2) - (s2 * c1);
      c12 = (c1 * c2) + (s1 * s2);
      val = atan2(s12, c12);
      fd += val;
    }
    /* Adjust the computed values. */

    if(fd < 0.0) fd += TWOPI;
    if(zn < 0.0) fd *= -1.0;
    if(znabs < dtol) fd = 0.0;

    fs -= zn * fd;
    numexact++;
  }

  /* Return values of the source and dipole, normalized by area. */
  fs /= panel->area;
  fd /= panel->area;
  if(pfd != NULL) *pfd = fd;


  if(fs < 0.0) {
    viewprintf(stderr, 
	    "\ncalcp: panel potential coeff. less than zero = %g\n", fs);
    viewprintf(stderr, "Okay = %d Evaluation Point = %g %g %g\n", okay, x, y, z);
    viewprintf(stderr, "Evaluation Point in local coords = %g %g %g\n",xn,yn, zn);
    viewprintf(stderr, "Panel Description Follows\n");
    dp(panel);
    /*FCExit(FC_GENERIC_ERROR);*/
  }


  return (fs);
}


dumpnums(flag, size)
int flag, size;
{
  double total;

  if(flag == ON) {		/* if first call */
    num2ndsav = num2nd;
    num4thsav = num4th;
    numexactsav = numexact;
  }
  else {
    total = num2ndsav + num4thsav + numexactsav;
#if MULDAT == ON
    viewprintf(stdout, "Potential coefficient counts\n multipole only:\n");
    viewprintf(stdout, 
	    "  2nd order: %d %.3g%%; 4th: %d %.3g%%; Integral: %d %.3g%%\n",
	    num2nd, 100*(num2ndsav/total), num4th, 100*(num4thsav/total), 
	    numexact, 100*(numexactsav/total));
#endif
    total = num2nd + num4th + numexact;
#if MULDAT == ON
    viewprintf(stdout, " multipole plus adaptive:\n");
    viewprintf(stdout, 
	    "  2nd order: %d %.3g%%; 4th: %d %.3g%%; Integral: %d %.3g%%\n",
	    num2nd, 100*(num2nd/total), num4th, 100*(num4th/total), 
	    numexact, 100*(numexact/total));
#endif
    viewprintf(stdout, "Percentage of multiplies done by multipole: %.3g%%\n",
	    100*(size*size - total)/(size*size));
    if(size*size == total) 
	viewprintf(stdout, "Warning: no multipole acceleration\n");
  }
}

double tilelength(nq)
charge *nq;
{
  return nq->max_diag;
}



/*
Evaluation of moments of quadrilateral surface relative to
local system, array S(15).  First initialize array
Note that S(2)=S(6)=0 due to transfer above
*/

ComputeMoments(pp)
charge *pp;
{
  int order=MAXORDER;
  int i, j, nside,  N, M, N1, M1, M2, MN1, MN2;
  double dx, dy, dxdy, dydx, x, y, z, SI, *xp, *yp, *xpn, *ypn;
  double ypp[3];
  static double *XP[4], *YP[4], **I; 
  static double CS[16] = { 0.0, 1.0, 1.0, 1.5, 1.5, 3.75, 1.0, 3.0, 
			   1.5, 7.5, 1.5, 1.5, 3.75, 1.5, 7.5, 3.75 };
  double *multi, sumc, sums, sign, **createBinom(), **createPmn();
  int m, n, r, halfn, flrm, ceilm, numterms, rterms;
  // local var made global - Enrico
  //static int maxorder = 0;

  /* Allocate temporary storage and initialize arrays. */
  if(order > maxorder) {
    for(i = 0; i < 4; i++) {
      CALLOC(XP[i], (order+3), double, ON, AQ2P);
      CALLOC(YP[i], (order+3), double, ON, AQ2P);
    }
    /* Allocate the euclidean moments matrix, Imn. */
    CALLOC(I, (order+1), double*, ON, AQ2P);
    for(i = 0; i <= order; i++) CALLOC(I[i], (order+1), double, ON, AQ2P);

    maxorder = order;
  }

  /* First zero out the Moments matrix. */
  for(i = 0; i <= order; i++) {
    for(j = 0; j <= order; j++) {
      I[i][j] = 0.0;
    }
  }
    
  /* Compute powers of x and y at corner pts. */
  for(i = 0; i < pp->shape; i++) {
    xp = XP[i];
    yp = YP[i];
    xp[1] = pp->corner[i][XI];
    yp[1] = pp->corner[i][YI];
    for(j = 2; j <= order+2; j++) {
      xp[j] = xp[j-1] * xp[1];
      yp[j] = yp[j-1] * yp[1];
    }
  }

  /* First moment, easy, just the panel area. */
  I[0][0] = pp->area;

  /* By using centroid, (1,0) and (0,1) are zero, so begin with (2,0). */
  for(nside = 0; nside < pp->shape; nside++) {
    xp = XP[nside];
    yp = YP[nside];
    if(nside == (pp->shape - 1)) {
      xpn = XP[0];
      ypn = YP[0];  
    }
    else {
      xpn = XP[nside + 1];
      ypn = YP[nside + 1];
    }

    dx = xpn[1] - xp[1];
    dy = ypn[1] - yp[1];

    if(fabs(dx) >= fabs(dy)) {
      dydx = dy/dx;
      for(M = 2; M <= order; M++) {
	M1 = M + 1;
	M2 = M + 2;

	SI = ((xpn[M1] * ypn[1]) - (xp[M1] * yp[1])) / M1
	     + dydx * (xp[M2] - xpn[M2]) / (M1 * M2);
	I[M][0] += SI;

	for(N = 1; N <= M; N++) {
	  N1 = N + 1;
	  MN1 = M - N + 1;
	  SI = (xpn[MN1] * ypn[N1] - xp[MN1] * yp[N1]) / (MN1 * N1)
		     - (dydx * N * SI) / MN1;
	  I[M-N][N] += SI;
	}
      }
    }
    else {
      dxdy = dx/dy;
      for(M = 2; M <= order; M++) {
	M1 = M + 1;
	M2 = M + 2;
	SI = (dxdy / (M1 * M2)) * (ypn[M2] - yp[M2]);
	I[0][M] += SI;
	for(N = 1; N <= M; N++) {
	  MN1 = M - N + 1;
	  MN2 = MN1 + 1;
          SI = dxdy * ((xpn[N] * ypn[MN2] - xp[N] * yp[MN2]) / (MN1 * MN2) 
			- (N * SI / MN1));
	  I[N][M-N] += SI;
	}
      }
    }
  }

  /* Now Create the S vector for calcp. */
  for(i = 0, M = 0; M <= 4; M++) {
    for(N = 0; N <= (4 - M); N++) {
      i++;
      pp->moments[i] = I[M][N] * CS[i];
    }
  }

}

/* Debugging Print Routines follow. */

dp(panel)
charge *panel;
{
  int i;
  double c[4][3];

  viewprintf(stderr, "shape=%d maxdiag=%g mindiag=%g area=%g\n", 
	 panel->shape, 
	 panel->max_diag, panel->min_diag, panel->area);

  viewprintf(stdout, "surface: `%s'\n", panel->surf->name);

  viewprintf(stderr, "x=%g y=%g z=%g\n", panel->x, panel->y, panel->z);
  viewprintf(stderr, "X= %g %g %g\n", panel->X[0], panel->X[1], panel->X[2]);
  viewprintf(stderr, "Y= %g %g %g\n", panel->Y[0], panel->Y[1], panel->Y[2]);
  viewprintf(stderr, "Z= %g %g %g\n", panel->Z[0], panel->Z[1], panel->Z[2]);

  for(i=0; i < panel->shape; i++)
      viewprintf(stderr, "corner%d = %g %g %g\n", 
	     i, panel->corner[i][0], panel->corner[i][1], panel->corner[i][2]);

  for(i = 0; i < panel->shape; i++) {
    c[i][0] = panel->x + panel->corner[i][0]*panel->X[0] 
	+ panel->corner[i][0]*panel->X[1] + panel->corner[i][0]*panel->X[2];
    c[i][1] = panel->y + panel->corner[i][1]*panel->Y[0] 
	+ panel->corner[i][1]*panel->Y[1] + panel->corner[i][1]*panel->Y[2];
    c[i][2] = panel->z + panel->corner[i][2]*panel->Z[0] 
	+ panel->corner[i][2]*panel->Z[1] + panel->corner[i][2]*panel->Z[2];
    viewprintf(stderr, "absolute corner%d = %g %g %g\n", i, c[i][0], c[i][1], c[i][2]);
  }

  for(i=0; i < panel->shape; i++)
      viewprintf(stderr, "length%d = %g\n", i, panel->length[i]);

  viewprintf(stderr, "multipole coeffs:  ");
  for(i=0; i < 16; i++) {
    viewprintf(stderr, "%g  ", panel->moments[i]);
    if( (i % 6) == 0) viewprintf(stderr, "\n");
  }
  viewprintf(stderr, "\n");
}



#define DIS 2
#define SCALE 5

testCalcp(pp)
charge *pp;
{

  double offx, offy, offz, x, y, z, mult;
  int i, j, k;

  offx = pp->x;
  offy = pp->y;
  offz = pp->z;

  mult = 0.5 * pp->min_diag;

  viewprintf(stderr, "\n\nCenter Point %g %g %g\n", offx, offy, offz);
  for(i=0; i < DIS; i++) {
    for(j=0; j < DIS; j++) {
      for(k=0; k < DIS; k++) {
	x = offx + i * mult * SCALE;
	y = offy + j * mult * SCALE;
	z = offz + k * mult * SCALE;
	printf("Eval pt = %g %g %g\n", x, y, z);
        calcp(pp, x, y, z, NULL);
      }
    }
  }
}


fileCorners(pp, f)
FILE *f;
charge *pp;

{
  int i;

  for(i=0; i < pp->shape; i++)
      fprintf(f, "%g %g\n", pp->corner[i][0], pp->corner[i][1]);
}


/* Test the moment code. */
calcpm(multi, x, y, z, origorder, order)
double *multi, x, y, z;
int order;
{
  charge panel, *ppanel;
  double **mat, **mulMulti2P(), potential;
  int i, numterms;

  /* Create a temporary panel which has evaluation point as centroid. */
  panel.x = x;
  panel.y = y;
  panel.z = z;
  ppanel = &panel;

  mat = mulMulti2P(0.0, 0.0, 0.0, &ppanel, 1, origorder);
  numterms = multerms(origorder);

  /* Calculate the potential. */
  for(potential = 0.0, i = 0; i < numterms; i++) {
    potential += mat[0][i] * multi[i];
  }
}



/*

dismat(mat, size)
double **mat;
int size;
{
  int i, j;

  for(i=0; i <= size; i++) {
    printf("\nrow i = ");
    for(j=0; j <= size; j++) {
      printf("%g  ", mat[i][j]);
    }
  }
  printf("\n");
}
*/

void InitCalcpVars(void)
{
	num2nd=0;
	num4th=0;
	numexact=0;
	num2ndsav=0;
	num4thsav=0;
	numexactsav=0;

	maxorder = 0;
}


