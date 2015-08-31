/*!\page LICENSE LICENSE
 
Copyright (C) 2003 by the Board of Trustees of Massachusetts Institute of Technology, hereafter designated as the Copyright Owners.
 
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

#include <stdio.h>
#include <math.h>

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(A,B)  ( (A) > (B) ? (B) : (A) )
#endif

void wrQuad(fp, cond, p1, p2, p3, p4)
int cond;
double *p1, *p2, *p3, *p4;
FILE *fp;
{
  char pan_type;
  int i;

  if(p4 == NULL) pan_type = 'T';
  else pan_type = 'Q';

  fprintf(fp, "%c %d  ", pan_type, cond);
  for(i = 0; i < 3; i++) fprintf(fp, "%.5e ", p1[i]);
  fprintf(fp, " ");
  for(i = 0; i < 3; i++) fprintf(fp, "%.5e ", p2[i]);
  fprintf(fp, " ");
  for(i = 0; i < 3; i++) fprintf(fp, "%.5e ", p3[i]);
  if(p4 != NULL) {
    fprintf(fp, " ");
    for(i = 0; i < 3; i++) fprintf(fp, "%.5e ", p4[i]);
  }
  fprintf(fp, "\n");
}


/*
  write a set of panels for a triangle in quickif.c format
  - currently uses a primitive algorithm that won't work for skinny triangles
  - returns number of panels generated
*/
int disTri(fp, cond, edgefrac, ncells, no_discr,
	   x1, y1, z1, x2, y2, z2, x3, y3, z3)
int ncells;			/* number of cells on short side, > 2 */
int cond;			/* conductor number */
int no_discr;			/* TRUE => no discr., just wr the three pnts */
double edgefrac;       		/* edge cell widths =edgefrac*(inner widths) */
double x1, y1, z1, x2, y2, z2, x3, y3, z3; /* 3 corners */
FILE *fp;
{
  int lflag, linernum, sinernum, npanels, i;
  double lside, sside, mside, temp, sedgewid, sinerwid, linerwid;
  double side_dirs[3][3], norm, disc_points[12][3];
  static double lepsilon, uepsilon;
  static int fstflag = 1;

  if(fp == NULL) {
    fprintf(stderr, "\ndisTri: bad output file pointer\n");
    exit(0);
  }

  if(no_discr || ncells == 1) {
    fprintf(fp, "T %d %.5e %.5e %.5e  %.5e %.5e %.5e",
	    cond, x1, y1, z1, x2, y2, z2);
    fprintf(fp, " %.5e %.5e %.5e\n", x3, y3, z3);
    return(1);
  }

  /* TEMPORARY until impelmentation is extended */
  if(ncells > 3) {
    fprintf(stderr, "\ndisTri: ncells (= %d) > 3 not implemented\n", ncells);
    exit(0);
  }
  if(ncells == 2) {
    fprintf(stderr, "\ndisTri: ncells = 2 not implemented\n", ncells);
    exit(0);
  }

  /* setup bounds on machine precision on first call */
  if(fstflag == 1) {
    fstflag = 0;
    getEpsBnds(&uepsilon, &lepsilon);
  }

  /* find the minimum side length */
  sside = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
  sside = MIN(sside,
	      sqrt((x3-x2)*(x3-x2) + (y3-y2)*(y3-y2) + (z3-z2)*(z3-z2)));
  sside = MIN(sside,
	      sqrt((x1-x3)*(x1-x3) + (y1-y3)*(y1-y3) + (z1-z3)*(z1-z3)));

  /* find the edge cell width */
  sinernum = 1;			/* hard wired cells per side parameter (3) */
  sinerwid = sside/(2.0*edgefrac + (double)sinernum);
  sedgewid = edgefrac*sinerwid;

  /* figure the triangle side directions using point 1 as origin */
  side_dirs[0][0] = x2 - x1;
  side_dirs[0][1] = y2 - y1;
  side_dirs[0][2] = z2 - z1;
  norm = 0.0;
  for(i = 0; i < 3; i++) norm += side_dirs[0][i]*side_dirs[0][i];
  norm = sqrt(norm);
  for(i = 0; i < 3; i++) side_dirs[0][i] /= norm;
  side_dirs[1][0] = x3 - x1;
  side_dirs[1][1] = y3 - y1;
  side_dirs[1][2] = z3 - z1;
  norm = 0.0;
  for(i = 0; i < 3; i++) norm += side_dirs[1][i]*side_dirs[1][i];
  norm = sqrt(norm);
  for(i = 0; i < 3; i++) side_dirs[1][i] /= norm;
  /*   and point 2 as origin for far side */
  side_dirs[2][0] = x3 - x2;
  side_dirs[2][1] = y3 - y2;
  side_dirs[2][2] = z3 - z2;
  norm = 0.0;
  for(i = 0; i < 3; i++) norm += side_dirs[2][i]*side_dirs[2][i];
  norm = sqrt(norm);
  for(i = 0; i < 3; i++) side_dirs[2][i] /= norm;

  /* load the corner points for the discretization
     - points 0 to 3 are the points along the edge from p1 to p2 
     - points 4 to 7 are the points along the edge from p1 to p2 on
       the other side of the edge panels 
     - points 8 to 11 are the corners of the p3 corner panel */
  /*   points 0 to 3, on edge btwn p1, p2 */
  disc_points[0][0] = x1; disc_points[0][1] = y1; disc_points[0][2] = z1; 
  for(i = 0; i < 3; i++)
      disc_points[1][i] = disc_points[0][i] + side_dirs[0][i]*sedgewid;
  disc_points[3][0] = x2; disc_points[3][1] = y2; disc_points[3][2] = z2; 
  for(i = 0; i < 3; i++)
      disc_points[2][i] = disc_points[3][i] - side_dirs[0][i]*sedgewid;
  /*   points 4 to 7, defined relative to 0 to 3 */
  for(i = 0; i < 3; i++)
      disc_points[4][i] = disc_points[0][i] + side_dirs[1][i]*sedgewid;
  for(i = 0; i < 3; i++)
      disc_points[7][i] = disc_points[3][i] + side_dirs[2][i]*sedgewid;
  for(i = 0; i < 3; i++)
      disc_points[6][i] = disc_points[7][i] - side_dirs[0][i]*sedgewid;
  for(i = 0; i < 3; i++)
      disc_points[5][i] = disc_points[4][i] + side_dirs[0][i]*sedgewid;
  /*   points 8 to 11, defined relative to p3 */
  disc_points[11][0] = x3; disc_points[11][1] = y3; disc_points[11][2] = z3; 
  for(i = 0; i < 3; i++)
      disc_points[9][i] = disc_points[11][i] - side_dirs[1][i]*sedgewid;
  for(i = 0; i < 3; i++)
      disc_points[10][i] = disc_points[11][i] - side_dirs[2][i]*sedgewid;
  for(i = 0; i < 3; i++)
      disc_points[8][i] = disc_points[10][i] - side_dirs[1][i]*sedgewid;

  /* write seven panels one by one - all same twist as 1,2,3 ordering */
  /*   corner panel near point 1 */
  wrQuad(fp, cond, 
	 disc_points[0], disc_points[1], disc_points[5], disc_points[4]);
  /*   edge panel on pt1-2 edge */
  wrQuad(fp, cond,
	 disc_points[1], disc_points[2], disc_points[6], disc_points[5]);
  /*   corner panel near 2 */
  wrQuad(fp, cond,
	 disc_points[2], disc_points[3], disc_points[7], disc_points[6]);
  /*   edge panel on 1-3 edge */
  wrQuad(fp, cond,
	 disc_points[4], disc_points[5], disc_points[8], disc_points[9]);
  /*   central triangle */
  wrQuad(fp, cond,
	 disc_points[5], disc_points[6], disc_points[8], NULL);
  /*   edge panel on 2-3 edge */
  wrQuad(fp, cond,
	 disc_points[6], disc_points[7], disc_points[10], disc_points[8]);
  /*   corner panel near point 3 */
  wrQuad(fp, cond,
	 disc_points[8], disc_points[10], disc_points[11], disc_points[9]);

  return(7);
}
