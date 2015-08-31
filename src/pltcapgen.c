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

#define DEFWID 1.0		/* default plate width, meters */
#define DEFEFR 1E-1		/* default edge-cell-width/inner-cell-width */
#define DEFNCL 3		/* default #cells on short side of faces */
#define DEFSEP 1E-1		/* default plate separation, meters */
#define DEFPLT 2		/* default number of || plates */

#define TRUE 1
#define FALSE 0

/*
  generates a parallel (square) plate capacitor example in quickif.c format
  - uses disRect() for discretization plates
*/
main(argc, argv)
int argc;
char *argv[];
{
  char temp[BUFSIZ], **chkp, *chk, name[BUFSIZ];
  int npanels = 0, ncells, cmderr = FALSE, i, cond, numplt, name_given = FALSE;
  int align_on_x = FALSE, no_disc = FALSE;
  double edgefrac, width, strtod(), sep;
  double x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4; /* 4 corners */
  long strtol();
  FILE *fp, *fopen();

  /* load default parameters */
  width = DEFWID;
  edgefrac = DEFEFR;
  ncells = DEFNCL;
  sep = DEFSEP;
  numplt = DEFPLT;

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
		"%s: bad number of panels/plate side `%s'\n", 
		argv[0], &argv[i][2]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 'p') {
      numplt = (int) strtol(&(argv[i][2]), chkp, 10);
      if(*chkp == &(argv[i][2]) || numplt < 1) {
	fprintf(stderr, 
		"%s: bad number of parallel plates `%s'\n", 
		argv[0], &argv[i][2]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 's') {
      sep = strtod(&(argv[i][2]), chkp);
      if(*chkp == &(argv[i][2]) || sep <= 0.0) {
	fprintf(stderr, "%s: bad plate separation `%s'\n", 
		argv[0], &argv[i][2]);
	cmderr = TRUE;
	break;
      }
    }
    else if(argv[i][1] == 'w') {
      width = strtod(&(argv[i][2]), chkp);
      if(*chkp == &(argv[i][2]) || width <= 0.0) {
	fprintf(stderr, "%s: bad plate width `%s'\n", 
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
    else if(argv[i][1] == 'x') {
      align_on_x = TRUE;
    }
    else if(argv[i][1] == 'd') {
      no_disc = TRUE;
    }
    else {
      fprintf(stderr, "%s: illegal option -- %s\n", argv[0], &(argv[i][1]));
      cmderr = TRUE;
      break;
    }
  }

  if(cmderr == TRUE) {
    fprintf(stderr,
	    "Usage: %s [-s<plate sep>] [-w<plate width>] [-p<num plates>]\n              [-n<num panels/plate width>] [-e<rel edge panel width>]\n              [-na<cond name base>] [-d]\n", argv[0]);
    fprintf(stderr, "DEFAULT VALUES:\n");
    fprintf(stderr, "  plate sep = %g\n", DEFSEP);
    fprintf(stderr, "  plate width = %g\n", DEFWID);
    fprintf(stderr, "  num plates = %d\n", DEFPLT);
    fprintf(stderr, "  num panels/plate width = %d\n", DEFNCL);
    fprintf(stderr, "  rel edge panel width = %g\n", DEFEFR);
    fprintf(stderr, "  conductor name base = <none>\n");
    fprintf(stderr, "OPTIONS:\n");
    fprintf(stderr, "  -d = do not discretize faces\n");
    exit(0);
  }

  /* open output file */
  fp = stdout;

  /* write title */
  if(numplt > 1)
      fprintf(fp, 
	      "0 %gmX%gm %d || plate capacitor with %gm separation (n=%d e=%.3g)\n",
	      width, width, numplt, sep, ncells, edgefrac);
  else 
      fprintf(fp, 
	      "0 %gmX%gm single plate capacitor with %gm separation (n=%d e=%.3g)\n",
	      width, width, sep, ncells, edgefrac);


  /* write panels */
  if(align_on_x) {
    if(numplt % 2 != 0) {	/* odd number of plates */
      for(i = -(numplt / 2); i <= numplt / 2; i++) {
	npanels += disRect(fp, i+numplt, edgefrac, ncells, no_disc,
			   sep*(double)i, width/2.0, width/2.0,
			   sep*(double)i, -width/2.0, width/2.0,
			   sep*(double)i, -width/2.0, -width/2.0,
			   sep*(double)i, width/2.0, -width/2.0);
      }
    }
    else {
      for(i = 1, cond = 1; i < numplt; i += 2) {
	npanels += disRect(fp, cond++, edgefrac, ncells, no_disc,
			   -sep*(double)i/2, width/2.0, width/2.0,
			   -sep*(double)i/2, -width/2.0, width/2.0,
			   -sep*(double)i/2, -width/2.0, -width/2.0,
			   -sep*(double)i/2, width/2.0, -width/2.0);
	npanels += disRect(fp, cond++, edgefrac, ncells, no_disc,
			   sep*(double)i/2, width/2.0, width/2.0,
			   sep*(double)i/2, -width/2.0, width/2.0,
			   sep*(double)i/2, -width/2.0, -width/2.0,
			   sep*(double)i/2, width/2.0, -width/2.0);
      }
    }
  }
  else {
    for(i = 0; i < numplt; i++) {
      npanels += disRect(fp, i+1, edgefrac, ncells, no_disc,
			 0.0, 0.0, sep*(double)i, 
			 0.0, width, sep*(double)i, 
			 width, width, sep*(double)i, 
			 width, 0.0, sep*(double)i);
    }
  }

  if(name_given) {
    for(i = 1; i <= numplt; i++) fprintf(fp, "N %d %s%d\n", i, name, i);
  }

}
