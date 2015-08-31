/*
Copyright (c) 1990 Massachusetts Institute of Technology, Cambridge, MA.
All rights reserved.

This Agreement gives you, the LICENSEE, certain rights and obligations.
By using the software, you indicate that you have read, understood, and
will comply with the terms.

Permission to use, copy and modify for internal, noncommercial purposes
is hereby granted.  Any distribution of this program or any part thereof
is strictly prohibited without prior written consent of M.I.T.

Title to copyright to this software and to any associated documentation
shall at all times remain with M.I.T. and LICENSEE agrees to preserve
same.  LICENSEE agrees not to make any copies except for LICENSEE'S
internal noncommercial use, or to use separately any portion of this
software without prior written consent of M.I.T.  LICENSEE agrees to
place the appropriate copyright notice on any such copies.

Nothing in this Agreement shall be construed as conferring rights to use
in advertising, publicity or otherwise any trademark or the name of
"Massachusetts Institute of Technology" or "M.I.T."

M.I.T. MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.  By
way of example, but not limitation, M.I.T. MAKES NO REPRESENTATIONS OR
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR
THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS OR DOCUMENTATION WILL
NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.
M.I.T. shall not be held liable for any liability nor for any direct,
indirect or consequential damages with respect to any claim by LICENSEE
or any third party on account of or arising from this Agreement or use
of this software.
*/

/*
 * savemat - C language routine to save a matrix in a MAT-file.
 *
 * We recommend that you use this routine, and its companion loadmat.c,
 * for all writing and reading of MAT-files.  These routines implement
 * "access methods" for MAT-files.  By using these routines, instead
 * of writing your own code to directly access the MAT-file format,
 * you will be unaffected by any changes that may be made to the MAT-file
 * structure at a future date.
 *
 * Here is an example that uses 'savemat' to save two matrices to disk,
 * the second of which is complex:
 *
 *	FILE *fp;
 *	double xyz[1000], ar[1000], ai[1000];
 *	fp = fopen("foo.mat","wb");
 *	savemat(fp, 2000, "xyz", 2, 3, 0, xyz, (double *)0);
 *	savemat(fp, 2000, "a", 5, 5, 1, ar, ai);
 *      fclose(fp);
 *
 * Author J.N. Little 11-3-86
 * Revised 7-23-91 to support ANSI-C
 */
#include <stdio.h>
#include <string.h>

typedef struct {
     long type;   /* type */
     long mrows;  /* row dimension */
     long ncols;  /* column dimension */
     long imagf;  /* flag indicating imag part */
     long namlen; /* name length (including NULL) */
} Fmatrix;

#ifdef __STDC__
void savemat(FILE *fp, int type, char *pname, int mrows, int ncols, 
             int imagf, double *preal, double *pimag)
#else
void savemat(fp, type, pname, mrows, ncols, imagf, preal, pimag)
FILE *fp;       /* File pointer */
int type;       /* Type flag: Normally 0 for PC, 1000 for Sun, Mac, */
		/* Apollo, and other Motorola format, */
		/* 2000 for VAX D-float, 3000 for VAX G-float, and */
		/* 4000 for CRAY */
		/* Add 1 for text variables, 2 for sparse matrices */
		/* See LOAD in reference section of guide for more info.*/
char *pname;    /* pointer to matrix name */
int mrows;      /* row dimension */
int ncols;      /* column dimension */
int imagf;	/* imaginary flag */
double *preal;  /* pointer to real data */
double *pimag;  /* pointer to imag data */
#endif
{
	Fmatrix x;
	int mn;
	
	x.type = type;
	x.mrows = mrows;
	x.ncols = ncols;
	x.imagf = imagf;
	x.namlen = strlen(pname) + 1;
	mn = x.mrows * x.ncols;

	fwrite(&x, sizeof(Fmatrix), 1, fp);
	fwrite(pname, sizeof(char), (int)x.namlen, fp);
	fwrite(preal, sizeof(double), mn, fp);
	if (imagf) {
	     fwrite(pimag, sizeof(double), mn, fp);
	}
}

/*
  MODIFIED version of above: added wr_flag to allow multiple writes 
  to same matrix 
  wr_flag = 0 => open, print header (like old matlab setup)
  wr_flag = 1 => update, print without header
*/
#ifdef __STDC__
void savemat_mod(FILE *fp, int type, char *pname, int mrows, int ncols, 
		 int imagf, double *preal, double *pimag, int wr_flag, int mn)
#else
void savemat_mod(fp, type, pname, mrows, ncols, imagf, preal, pimag, 
		 wr_flag, mn)
FILE *fp;       /* File pointer */
int type;       /* Type flag: Normally 0 for PC, 1000 for Sun, Mac, */
		/* Apollo, and other Motorola format, */
		/* 2000 for VAX D-float, 3000 for VAX G-float, and */
		/* 4000 for CRAY */
		/* Add 1 for text variables, 2 for sparse matrices */
		/* See LOAD in reference section of guide for more info.*/
char *pname;    /* pointer to matrix name */
int mrows;      /* row dimension */
int ncols;      /* column dimension */
int imagf;	/* imaginary flag */
double *preal;  /* pointer to real data */
double *pimag;  /* pointer to imag data */
int wr_flag;			/* 0 for open, 1 to add to matrix */
int mn;				/* real #entries, this dump only */
#endif
{
	Fmatrix x;
	
	if(wr_flag == 0) {
	  x.type = type;
	  x.mrows = mrows;
	  x.ncols = ncols;
	  x.imagf = imagf;
	  x.namlen = strlen(pname) + 1;
	
	  fwrite(&x, sizeof(Fmatrix), 1, fp);
	  fwrite(pname, sizeof(char), (int)x.namlen, fp);
	}
	fwrite(preal, sizeof(double), mn, fp);
	if (imagf) {
	     fwrite(pimag, sizeof(double), mn, fp);
	}
}
