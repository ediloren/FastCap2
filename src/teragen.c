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

#define DEFSID 1.0		/* default side and height, meters */
#define DEFEFR 0.1		/* default edge-cell-width/inner-cell-width */
#define DEFNCL 3		/* default #cells on short side of faces */

#define DEBUG 0

#define X0 0.0
#define Y0 0.0
#define Z0 0.0
#define XH 1.0
#define YH 1.0
#define ZH 1.0

#define TRUE 1
#define FALSE 0

#define MIN(A,B)  ( (A) > (B) ? (B) : (A) )

/*
  generates a pyramid example in quickif.c format
  - uses disRect() and discTri() for discretization of plates
*/
main(argc, argv)
int argc;
char *argv[];
{
  char temp[BUFSIZ], name[BUFSIZ], **chkp, *chk;
  int npanels = 0, ncells, cmderr = FALSE, i, cond, center_on_origin, no_top;
  int top_area, right_side_area, left_side_area, no_perimeter, no_discr;
  int right_cells, left_cells, top_cells, no_bottom, top_cells_given;
  int no_perimeter_front_left, no_perimeter_front_right;
  int no_perimeter_back_left, no_perimeter_back_right, name_given;
  double edgefrac, width, strtod(), pos_end, neg_end;
  double x1, y1, z1, x2, y2, z2, x3, y3, z3, x0, y0, z0; /* 4 corners */
  double xh, yh, zh, x4, y4, z4;
  long strtol();
  FILE *fp, *fopen();

  /* load default parameters */
  width = DEFSID;
  edgefrac = DEFEFR;
  ncells = DEFNCL;
  center_on_origin = no_top = no_perimeter = no_bottom = no_discr = FALSE;
  top_cells_given = FALSE;
  x0 = X0; y0 = Y0; z0 = Z0;
  xh = XH; yh = YH; zh = ZH;
  no_perimeter_front_left = no_perimeter_front_right = FALSE;
  no_perimeter_back_left = no_perimeter_back_right = FALSE;
  name_given = FALSE;

  /* parse command line */
  chkp = &chk;			/* pointers for error checking */
  for(i = 1; i < argc && cmderr == FALSE; i++) {
    if(argv[i][0] != '-') {
      fprintf(stderr, "%s: illegal argument -- %s\n", argv[0], argv[i]);
      cmderr = TRUE;
      break;
    }
    else if(argv[i][1] == 'n' && argv[i][2] == 'a') {
      if(sscanf(&(argv[i][3]), "%s", name) != 1) {
	fprintf(stderr, "%s: bad name `%s'\n", argv[0], &argv[i][3]);
	cmderr = TRUE;
	break;
      }
      name_given = TRUE;
    }
    else if(argv[i][1] == 'n') {
      ncells = (int) strtol(&(argv[i][2]), chkp, 10);
      if(*chkp == &(argv[i][2]) || ncells < 1) {
	fprintf(stderr, 
		"%s: bad number of panels/side `%s'\n", 
		argv[0], &argv[i][2]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 'e') {
      edgefrac = strtod(&(argv[i][2]), chkp);
      if(*chkp == &(argv[i][2]) || edgefrac < 0.0) {
	fprintf(stderr, "%s: bad edge panel fraction `%s'\n", 
		argv[0], &argv[i][2]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 'x' && argv[i][2] == 'o') {
      x0 = strtod(&(argv[i][3]), chkp);
      if(*chkp == &(argv[i][3])) {
	fprintf(stderr, "%s: bad x origin value `%s'\n", 
		argv[0], &argv[i][3]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 'y' && argv[i][2] == 'o') {
      y0 = strtod(&(argv[i][3]), chkp);
      if(*chkp == &(argv[i][3])) {
	fprintf(stderr, "%s: bad y origin value `%s'\n", 
		argv[0], &argv[i][3]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 'z' && argv[i][2] == 'o') {
      z0 = strtod(&(argv[i][3]), chkp);
      if(*chkp == &(argv[i][3])) {
	fprintf(stderr, "%s: bad z origin value `%s'\n", 
		argv[0], &argv[i][3]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 'x' && argv[i][2] == 'h') {
      xh = strtod(&(argv[i][3]), chkp);
      if(*chkp == &(argv[i][3])) {
	fprintf(stderr, "%s: bad x height value `%s'\n", 
		argv[0], &argv[i][3]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 'y' && argv[i][2] == 'h') {
      yh = strtod(&(argv[i][3]), chkp);
      if(*chkp == &(argv[i][3])) {
	fprintf(stderr, "%s: bad y height value `%s'\n", 
		argv[0], &argv[i][3]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 'z' && argv[i][2] == 'h') {
      zh = strtod(&(argv[i][3]), chkp);
      if(*chkp == &(argv[i][3])) {
	fprintf(stderr, "%s: bad z height value `%s'\n", 
		argv[0], &argv[i][3]);
	cmderr = TRUE;
	break;
      }
    }
    else if(!strcmp(&(argv[i][1]), "pfl")) {
      no_perimeter_front_left = TRUE;
    }
    else if(!strcmp(&(argv[i][1]), "pfr")) {
      no_perimeter_front_right = TRUE;
    }
    else if(!strcmp(&(argv[i][1]), "pbl")) {
      no_perimeter_back_left = TRUE;
    }
    else if(!strcmp(&(argv[i][1]), "pbr")) {
      no_perimeter_back_right = TRUE;
    }
    else if(!strcmp(&(argv[i][i]), "p")
) {
      no_perimeter = TRUE;
    }
    else if(argv[i][1] == 'b') {
      no_bottom = TRUE;
    }
    else if(argv[i][1] == 'd') {
      no_discr = TRUE;
    }
    else {
      fprintf(stderr, "%s: illegal option -- %s\n", argv[0], &(argv[i][1]));
      cmderr = TRUE;
      break;
    }
  }

  if(cmderr == TRUE) {
    fprintf(stderr,
	    "Usage: %s [-xo<originx>] [-yo<originy>] [-zo<originz>] \n               [-xh<heightx>] [-yh<heighty>] [-zh<heightz>] \n               [-n<num panels/side>] [-e<rel edge panel width>] \n               [-na<name>] [-b] [-p] [-pfl] [-pfr] [-pbl] [-pbr] [-d]\n", 
	    argv[0]);
    fprintf(stdout, "DEFAULT VALUES:\n");
    fprintf(stderr, "  origin = (xo yo zo) = (%g %g %g)\n", X0, Y0, Z0);
    fprintf(stderr, "  side heights = (xh yh zh) = (%g %g %g)\n", XH, YH, ZH);
    fprintf(stdout, "  num panels/side = %d\n", DEFNCL);
    fprintf(stdout, "  rel edge panel width = %g\n", DEFEFR);
    fprintf(stdout, "  conductor name = `1'\n");
    fprintf(stdout, "OPTIONS:\n");
    fprintf(stdout, "  -b don't include bottom (rectangular) face\n");
    fprintf(stdout, "  -p don't include any perimeter (side) faces\n");
    fprintf(stdout, "  -pfl don't include perimeter front left face (view from positive orthant)\n");
    fprintf(stdout, "  -pfr don't include perimeter front right face\n");
    fprintf(stdout, "  -pbl don't include perimeter back left face\n");
    fprintf(stdout, "  -pbr don't include perimeter back right face\n");
    fprintf(stdout, "  -d don't discretize faces\n");
    exit(0);
  }

  /* open output file */
  fp = stdout;

  /* write title */
  if(center_on_origin) {
    fprintf(fp, 
	    "0 %gmX%gmX%gm pyramid centered on origin (n=%d e=%.3g)\n",
	    xh, yh, zh, ncells, edgefrac);
    x0 = -xh/2.0; y0 = -yh/2.0; z0 = -zh/2.0;
  }
  else {
    fprintf(fp, 
	    "0 %gmX%gmX%gm pyramid (n=%d e=%.3g)\n",
	    xh, yh, zh, ncells, edgefrac);
    fprintf(fp, "* xo = %g, yo = %g, zo = %g\n", x0, y0, z0);
  }
  if(no_bottom) fprintf(fp, "* bottom panel omitted\n");
  if(no_perimeter) fprintf(fp, "* all side panels omitted\n");
  else {
    if(no_perimeter_front_left)
	fprintf(fp, "* front left side panel omitted\n");
    if(no_perimeter_front_right)
	fprintf(fp, "* front right side panel omitted\n");
    if(no_perimeter_back_left)
	fprintf(fp, "* back left side panel omitted\n");
    if(no_perimeter_back_right)
	fprintf(fp, "* back right side panel omitted\n");
  }

  /* set up corners */
  x1 = x0 + xh; y1 = y0; z1 = z0;
  x2 = x0; y2 = y0 + yh; z2 = z0;
  x3 = x0 + xh; y3 = y0 + yh; z3 = z0;
  x4 = x0 + xh/2.0; y4 = y0 + yh/2.0; z4 = z0 + zh;

  left_cells = right_cells = ncells; /* in case want to set one rel to other */
  top_cells = ncells;

  /* write panels with outward pointing normals */
  fprintf(stdout, "* view from -x, -y, +z\n");

  if(!no_perimeter) {
    if(!no_perimeter_front_right) {
      fprintf(stdout, "* front right\n");
      npanels += disTri(fp, 1, edgefrac, right_cells, no_discr,
			x3, y3, z3,
			x4, y4, z4,
			x2, y2, z2);
    }
#if DEBUG == 0
    if(!no_perimeter_front_left) {
      fprintf(stdout, "* front left\n");
      npanels += disTri(fp, 1, edgefrac, left_cells, no_discr,
			x1, y1, z1,
			x4, y4, z4,
			x3, y3, z3);
    }

    if(!no_perimeter_back_left) {
      fprintf(stdout, "* back left\n");
#if DEBUG == 1
      npanels += disTri(fp, 1, edgefrac, right_cells, no_discr,
			x0, y0, z0,
			x2, y2, z2,
			x1, y1, z1);
#else
      npanels += disTri(fp, 1, edgefrac, right_cells, no_discr,
			x0, y0, z0,
			x4, y4, z4,
			x1, y1, z1);
#endif
    }

    if(!no_perimeter_back_right) {
      fprintf(stdout, "* back right\n");
      npanels += disTri(fp, 1, edgefrac, left_cells, no_discr,
			x2, y2, z2,
			x4, y4, z4,
			x0, y0, z0);
    }
#endif
  }

  if(!no_bottom) {
    fprintf(stdout, "* bottom\n");
    npanels += disRect(fp, 1, edgefrac, top_cells, no_discr,
		       x0, y0, z0,
		       x2, y2, z2,
		       x3, y3, z3,
		       x1, y1, z1);
  }

  if(name_given) fprintf(fp, "N 1 %s\n", name);

}
