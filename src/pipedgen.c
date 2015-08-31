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

#define DEFSID 1.0		/* default cube side, meters */
#define DEFEFR 0.1		/* default edge-cell-width/inner-cell-width */
#define DEFNCL 3		/* default #cells on short side of faces */
#define DEFPLT 2		/* default number of || plates */

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
  generates a parallelipiped example in quickif.c format
  - uses disRect() for discretization plates
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
  double x1, y1, z1, x2, y2, z2, x3, y3, z3, xr, yr, zr; /* 4 corners */
  long strtol();
  FILE *fp, *fopen();

  /* load default parameters */
  width = DEFSID;
  edgefrac = DEFEFR;
  ncells = DEFNCL;
  center_on_origin = no_top = no_perimeter = no_bottom = no_discr = FALSE;
  top_cells_given = FALSE;
  xr = X0; yr = Y0; zr = Z0;
  x1 = xr+XH; y1 = yr; z1 = zr;
  x2 = xr; y2 = yr+YH; z2 = zr;
  x3 = xr; y3 = yr; z3 = zr+ZH;
  no_perimeter_front_left = no_perimeter_front_right = name_given = FALSE;
  no_perimeter_back_left = no_perimeter_back_right = FALSE;

  /* parse command line */
  chkp = &chk;			/* pointers for error checking */
  for(i = 1; i < argc && cmderr == FALSE; i++) {
    if(argv[i][0] != '-') {
      fprintf(stderr, "%s: illegal argument -- %s\n", argv[0], argv[i]);
      cmderr = TRUE;
      break;
    }
    else if(!strcmp(argv[i], "-cr")) {
      xr = strtod(argv[i+1], chkp);
      if(*chkp == argv[i+1]) {
	fprintf(stderr, "%s: bad reference corner x coordinate `%s'\n",
		argv[0], argv[i+1]);
	cmderr = TRUE;
	break;
      }
      yr = strtod(argv[i+2], chkp);
      if(*chkp == argv[i+2]) {
	fprintf(stderr, "%s: bad reference corner y coordinate `%s'\n",
		argv[0], argv[i+2]);
	cmderr = TRUE;
	break;
      }
      zr = strtod(argv[i+3], chkp);
      if(*chkp == argv[i+3]) {
	fprintf(stderr, "%s: bad reference corner z coordinate `%s'\n",
		argv[0], argv[i+3]);
	cmderr = TRUE;
	break;
      }
      i = i + 3;
    }
    else if(!strcmp(argv[i], "-c1")) {
      if(sscanf(argv[i+1], "%lf", &x1) != 1) {
	fprintf(stderr, "%s: bad first corner x coordinate `%s'\n",
		argv[0], argv[i+1]);
	cmderr = TRUE;
	break;
      }
      if(sscanf(argv[i+2], "%lf", &y1) != 1) {
	fprintf(stderr, "%s: bad first corner y coordinate `%s'\n",
		argv[0], argv[i+2]);
	cmderr = TRUE;
	break;
      }
      if(sscanf(argv[i+3], "%lf", &z1) != 1) {
	fprintf(stderr, "%s: bad first corner z coordinate `%s'\n",
		argv[0], argv[i+3]);
	cmderr = TRUE;
	break;
      }
      i = i + 3;
    }
    else if(!strcmp(argv[i], "-c2")) {
      if(sscanf(argv[i+1], "%lf", &x2) != 1) {
	fprintf(stderr, "%s: bad second corner x coordinate `%s'\n",
		argv[0], argv[i+1]);
	cmderr = TRUE;
	break;
      }
      if(sscanf(argv[i+2], "%lf", &y2) != 1) {
	fprintf(stderr, "%s: bad second corner y coordinate `%s'\n",
		argv[0], argv[i+2]);
	cmderr = TRUE;
	break;
      }
      if(sscanf(argv[i+3], "%lf", &z2) != 1) {
	fprintf(stderr, "%s: bad second corner z coordinate `%s'\n",
		argv[0], argv[i+3]);
	cmderr = TRUE;
	break;
      }
      i = i + 3;
    }
    else if(!strcmp(argv[i], "-c3")) {
      if(sscanf(argv[i+1], "%lf", &x3) != 1) {
	fprintf(stderr, "%s: bad third corner x coordinate `%s'\n",
		argv[0], argv[i+1]);
	cmderr = TRUE;
	break;
      }
      if(sscanf(argv[i+2], "%lf", &y3) != 1) {
	fprintf(stderr, "%s: bad third corner y coordinate `%s'\n",
		argv[0], argv[i+2]);
	cmderr = TRUE;
	break;
      }
      if(sscanf(argv[i+3], "%lf", &z3) != 1) {
	fprintf(stderr, "%s: bad third corner z coordinate `%s'\n",
		argv[0], argv[i+3]);
	cmderr = TRUE;
	break;
      }
      i = i + 3;
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
    else if(argv[i][1] == 't') {
      no_top = TRUE;
    }
    else if(!strcmp(&(argv[i][1]),"pfl")) {
      no_perimeter_front_left = TRUE;
    }
    else if(!strcmp(&(argv[i][1]),"pfr")) {
      no_perimeter_front_right = TRUE;
    }
    else if(!strcmp(&(argv[i][1]),"pbl")) {
      no_perimeter_back_left = TRUE;
    }
    else if(!strcmp(&(argv[i][1]),"pbr")) {
      no_perimeter_back_right = TRUE;
    }
    else if(!strcmp(&(argv[i][1]),"p")) {
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
	    "Usage: %s [-cr <x y z>] [-c1 <x y z>] [-c2 <x y z>] [-c3 <x y z>] \n                [-n<num panels/side>] [-e<rel edge panel width>] \n                [-na<name>] [-t] [-b] [-p] [-pfl] [-pfr] [-pbl] [-pbr] [-d]\n", 
	    argv[0]);
    fprintf(stdout, "DEFAULT VALUES:\n");
    fprintf(stdout, "  -cr reference corner = (%g %g %g)\n", X0, Y0, Z0);
    fprintf(stdout, "  -c1 corner 1 = (%g %g %g)\n", X0+XH, Y0, Z0);
    fprintf(stdout, "  -c2 corner 2 = (%g %g %g)\n", X0, Y0+YH, Z0);
    fprintf(stdout, "  -c3 corner 3 = (%g %g %g)\n", X0, Y0, Z0+ZH);
    fprintf(stdout, "  num panels/side = %d\n", DEFNCL);
    fprintf(stdout, "  rel edge panel width = %g\n", DEFEFR);
    fprintf(stdout, "  conductor name = `1'\n");
    fprintf(stdout, "OPTIONS:\n");
    fprintf(stdout, "  -t don't include top (c1-cr-c2 plane) face\n");
    fprintf(stdout, "  -b don't include bottom (|| top) face\n");
    fprintf(stdout, "  -p don't include any perimeter (side) faces\n");
    fprintf(stdout, 
	 "  -pfl don't include perimeter front left (c1-cr-c3 plane) face\n");
    fprintf(stdout, 
	 "  -pfr don't include perimeter front right (c2-cr-c3 plane) face\n");
    fprintf(stdout, 
	 "  -pbl don't include perimeter back left (|| front right) face\n");
    fprintf(stdout, 
	 "  -pbr don't include perimeter back right (|| front left) face\n");
    fprintf(stdout, "  -d don't discretize faces\n");
    exit(0);
  }

  /* set up number of cells on top and bottom */
  if(!top_cells_given) top_cells = ncells;

  /* open output file */
  fp = stdout;

  /* write title */
  fprintf(fp, "0 parallelepiped, ref corner (%g %g %g)\n", xr, yr, zr);
  fprintf(fp, "* other corners: (%g %g %g) (%g %g %g) (%g %g %g)\n",
	  x1, y1, z1, x2, y2, z2, x3, y3, z3);
  if(no_top) fprintf(fp, "* top panel omitted\n");
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

  left_cells = right_cells = ncells; /* in case want to set one rel to other */

  /* write panels with outward pointing normals */
  fprintf(fp, "* view looking at cr; c1 to left, c2 to right, c3 down\n");

  if(!no_perimeter) {
    if(!no_perimeter_front_left) {
      fprintf(fp, "* front left\n");
      npanels += disRect(fp, 1, edgefrac, left_cells, no_discr,
			 xr, yr, zr,
			 x1, y1, z1,
			 x1+(x3-xr), y1+(y3-yr), z1+(z3-zr),
			 x3, y3, z3);
    }

    if(!no_perimeter_front_right) {
      fprintf(fp, "* front right\n");
      npanels += disRect(fp, 1, edgefrac, right_cells, no_discr,
			 xr, yr, zr,
			 x3, y3, z3,
			 x2+(x3-xr), y2+(y3-yr), z2+(z3-zr),
			 x2, y2, z2);
    }

    if(!no_perimeter_back_left) {
      fprintf(fp, "* back left\n");
      npanels += disRect(fp, 1, edgefrac, right_cells, no_discr,
			 x1, y1, z1,
			 x1+(x2-xr), y1+(y2-yr), z1+(z2-zr),
			 x1+(x2-xr)+(x3-xr), y1+(y2-yr)+(y3-yr), 
			 z1+(z2-zr)+(z3-zr),
			 x1+(x3-xr), y1+(y3-yr), z1+(z3-zr));
    }

    if(!no_perimeter_back_right) {
      fprintf(fp, "* back right\n");
      npanels += disRect(fp, 1, edgefrac, left_cells, no_discr,
			 x2, y2, z2,
			 x2+(x3-xr), y2+(y3-yr), z2+(z3-zr),
			 x1+(x2-xr)+(x3-xr), y1+(y2-yr)+(y3-yr), 
			 z1+(z2-zr)+(z3-zr),
			 x1+(x2-xr), y1+(y2-yr), z1+(z2-zr));
    }
  }

  if(!no_bottom) {
    fprintf(fp, "* bottom\n");
    npanels += disRect(fp, 1, edgefrac, top_cells, no_discr,
		       x3, y3, z3,
		       x1+(x3-xr), y1+(y3-yr), z1+(z3-zr),
		       x1+(x2-xr)+(x3-xr), y1+(y2-yr)+(y3-yr), 
		       z1+(z2-zr)+(z3-zr),
		       x2+(x3-xr), y2+(y3-yr), z2+(z3-zr));
  }

  if(!no_top) {
    fprintf(fp, "* top\n");
    npanels += disRect(fp, 1, edgefrac, top_cells, no_discr,
		       xr, yr, zr,
		       x2, y2, z2,
		       x1+(x2-xr), y1+(y2-yr), z1+(z2-zr),
		       x1, y1, z1);
  }

  if(name_given) fprintf(fp, "N 1 %s\n", name);

}
