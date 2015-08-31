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

#include "mulGlobal.h"
#include <fcntl.h>

#define SQRMAT 0		/* for rdMat(), wrMat) */
#define TRIMAT 1
#define COLMAT 2
#define LOWMAT 0		/* for blkMatSolve() */
#define UPPMAT 1
#define LO2TR 3			/* for matXfer() */
#define UP2TR 4
#define L11 0			/*1/4 matrix file names, see getName() */
#define U11 1
#define U12 2
#define L21 3
#define LTIL 4
#define UTIL 5
#define PMODE 0644		/* protection code used with write() */

#define SQDEX(i, j, siz) ((i)*(siz) + (j))
#define LODEX(i, j, siz) (((i)*((i)+1))/2 + (j))
#define UPDEX(i, j, siz) (((i)*(2*(siz) - (i) - 1))/2 + (j))

double *trimat, *sqrmat;	/* flattened triangular and square matrices */
int *real_index;		/* real_index[i] = index in full array
				   (w/dummies) corresponding to entry i in
				   the condensed array used by blk routines */

/* SRW */
int sqrdex(int, int, int);
int lowdex(int, int, int);
void dumpMatCor(double**, double*, int);
char *getName(int, char*);
void transpose(double*, int);
void wrMat(double*, int, int, int);
void rdMat(double*, int, int, int);
void matXfer(double*, double*, int, int);
void blkMatsolve(double*, double*, int, int);
void subInnerProd(double*, double*, int, int, int);
void blkLudecomp(double*, int);
void blkSolve(double*, double*, int, double*, double*);
void blkQ2Pfull(cube*,  int, int, double**, double**, int**, int*);
void blkLUdecomp(double*, double*, int);
void blkAqprod(double*, double*, int, double*);
void blkCompressVector(double*, int, int, int*);
void blkExpandVector(double*, int, int);


/*
  index into a square siz x siz matrix stored linearly
*/
int sqrdex(int i, int j, int siz)
/* int i, j, siz;			row, column and size */
{
  return(i*siz + j);
}

/*
  index into a lower triangular (w/diagonal) siz x siz matrix stored linearly
*/
int lowdex(int i, int j, int siz)
/* int i, j, siz;			row, column and size */
{
  int ret;
  if(j > i || (ret = i*(i+1)/2 + j) > siz*(1+siz)/2) {
    fprintf(stderr, 
	    "lowdex: bad indices for lower triangular i=%d  j=%d siz =%d\n",
	    i, j, siz);
    exit(1);
  }
  else return(ret);
}

/*
  index into an upper triangular (w/diagonal) siz x siz matrix stored linearly
*/
int uppdex(int i, int j, int siz)
/* int i, j, siz;			row, column and size */
{
  int ret;
  
  if(j < i || (ret = i*siz - i*(i-1)/2 + j - i) > siz*(1+siz)/2) {
    fprintf(stderr, 
	    "uppdex: bad indices for upper triangular i=%d  j=%d siz =%d\n",
	    i, j, siz);
    exit(1);
  }
  else return(ret);
}

/*
  for debug only - dumps upper left corner of a matrix
*/
void dumpMatCor(double **mat, double *vec, int fsize)
/* int fsize;			full matrix size for flat storage */
/* double **mat, *vec;		does first one that's not null */
{
  int i, j, size = 5;

  if(mat != NULL) {		/* full matrix */
    fprintf(stdout, "\nUPPER LEFT CORNER - full %dx%d matrix\n", fsize, fsize);
    for(i = 0; i < MIN(size, fsize); i++) {
      for(j = 0; j < MIN(size, fsize); j++) {
	fprintf(stdout, "%.5e ", mat[i][j]);
      }
      fprintf(stdout, "\n");
    }

  }
  else if(vec != NULL){			/* flat matrix as in blkDirect.c */
    fprintf(stdout, "\nUPPER LEFT CORNER - flat %dx%d matrix\n", fsize, fsize);
    for(i = 0; i < MIN(size, fsize); i++) {
      for(j = 0; j < MIN(size, fsize); j++) {
	fprintf(stdout, "%.5e ", vec[SQDEX(i, j, fsize)]);
      }
      fprintf(stdout, "\n");
    }
  }
}

/*
  gets the file name from the flag
*/
char *getName(int file, char *name)
{
  if(file == L11 || file == L21 || file == LTIL) name[0] = 'L';
  else name[0] = 'U';

  if(file == L11 || file == U11 || file == U12) name[1] = '1';
  else if(file == L21) name[1] = '2';
  else name[1] = 'T';

  if(file == L11 || file == U11 || file == L21) name[2] = '1';
  else if(file == U12) name[2] = '2';
  else name[2] = 'I';

  name[3] = '\0';
  return(name);
}

/*
  converts a square matrix to its transpose (used to write columnwise)
*/
void transpose(double *mat, int siz)
{
  int i, j;
  double temp;

  for(i = 0; i < siz; i++) {
    for(j = 0; j < i; j++) {
      temp = mat[SQDEX(i, j, siz)];
      mat[SQDEX(i, j, siz)] = mat[SQDEX(j, i, siz)];
      mat[SQDEX(j, i, siz)] = temp;
    }
  }
}


/*
  writes full or triangular matrices 
*/
void wrMat(double *mat, int siz, int file, int type)
/* int siz, file, type;		siz is #rows and cols */
{
  int i, j, ds = sizeof(double), fdis;
  int realsiz, actsiz;			/* size in chars */
  char name[3];			/* name of file */

  /* figure the real size */
  if(type == TRIMAT) realsiz = ds*siz*(siz+1)/2;
  else if(type == SQRMAT || type == COLMAT) realsiz = sizeof(double)*siz*siz;
  else {
    fprintf(stderr, "wrMat: bad type flag %d\n", type);
    exit(1);
  }

  /* figure name of file and create, open to write */
  if((fdis = creat(getName(file, name), PMODE)) == -1) {
    fprintf(stderr, "wrMat: can't creat '%s'\n", name);
    exit(1);
  }

  fprintf(stderr,"Writing %s...", name);

  /* write the data and close */
  if(type == COLMAT) transpose(mat, siz);	/* store columnwise */
  if((actsiz = write(fdis, (char *)mat, realsiz)) != realsiz) {
    fprintf(stderr, 
	    "wrMat: buffer write error to '%s,' wrote %d of %d dbls\n", 
	    name, actsiz/ds, realsiz/ds);
    exit(1);
  }
  close(fdis);

  fprintf(stderr, "done.\n");
}

/*
  reads full or triangular matrices 
*/
void rdMat(double *mat, int siz, int file, int type)
/* int siz, file, type;		siz is #rows and cols */
{
  int i, j, fdis;
  int realsiz;			/* size in chars */
  char name[3];   	/* name of file */

  /* figure the real size */
  if(type == TRIMAT) realsiz = sizeof(double)*siz*(siz+1)/2;
  else if(type == SQRMAT) realsiz = sizeof(double)*siz*siz;
  else {
    fprintf(stderr, "rdMat: bad type flag %d\n", type);
    exit(1);
  }

  /* figure name of file and open to read */
  if((fdis = open(getName(file, name), 0)) == -1) {
    fprintf(stderr, "rdMat: can't open '%s'\n", name);
    exit(1);
  }

  fprintf(stderr,"Reading %s...", name);

  /* read the data and close */
  if(realsiz != read(fdis, (char *)mat, realsiz)) {
    fprintf(stderr, "rdMat: read error to '%s'\n", name);
    exit(1);
  }
  close(fdis);

  fprintf(stderr, "done.\n");
}

/*
  transfer part of the square matrix to the triangular matrix
*/
void matXfer(double *matsq, double *matri, int siz, int type)
{
  int i, j, temp;

  if(type == UP2TR) {		/* mv upper triangular part */
    for(i = 0; i < siz; i++) {	/* from row zero up */
      for(j = i; j < siz; j++) {
	temp = UPDEX(i, j, siz);
	matri[temp] = matsq[SQDEX(i, j, siz)];
      }
    }
  }
  else if(type == LO2TR) {	/* mv lower triangular part, put 1's on diag */
    for(i = 0; i < siz; i++) {	/* from row zero up */
      for(j = 0; j <= i; j++) {
	if(i == j) matri[LODEX(i, j, siz)] = 1.0;
	else matri[LODEX(i, j, siz)] = matsq[SQDEX(i, j, siz)];
      }
    }
  }
  else {
    fprintf(stderr, "matXfer: bad type %d\n", type);
    exit(1);
  }
}  

/*
  does many rhs, triangular problem to get L21 and U12 for blk factorization
*/
void blkMatsolve(double *matsq, double *matri, int siz, int type)
{
  int i, j, k;
  extern int fulldirops;

  if(type == LOWMAT) {
    /* a row in the result matrix is a linear comb of the previous rows */
    for(i = 1; i < siz; i++) {	/* loop on rows */
      fprintf(stderr, "%d ", i);
      for(j = 0; j < siz; j++) { /* loop on columns */
	for(k = 0; k < i; k++) {	/* loop on previous rows */
	  matsq[SQDEX(i, j, siz)]
	      -= (matri[LODEX(i, k, siz)]*matsq[SQDEX(k, j, siz)]);
	  fulldirops++;
	}
      }
    }
    fprintf(stderr, "\n");
  }
  else if(type == UPPMAT) {
    /* a column in the result matrix is a linear comb of the prev columns */
    for(j = 0; j < siz; j++) {	/* loop on columns */
      fprintf(stderr, "%d ", j);
      for(i = 0; i < siz; i++) { /* loop on rows */
	for(k = 0; k < j; k++) {	/* loop on previous columns */
	  matsq[SQDEX(i, j, siz)]
	      -= (matri[UPDEX(k, j, siz)]*matsq[SQDEX(i, k, siz)]);
	  fulldirops++;
	}
	matsq[SQDEX(i, j, siz)] /= matri[UPDEX(j, j, siz)];
	fulldirops++;
      }
    }
    fprintf(stderr, "\n");
  }
  else {
    fprintf(stderr, "blkMatsolve: bad type %d\n", type);
    exit(1);
  }

}

/*
  figures the difference A11-(L21)(U12) - part of block factorization
*/
void subInnerProd(double *matsq, double *matri, int siz, int matl, int matu)
/* int siz, matl, matu;		size in doubles; matrices to multiply */
{
  int i, j, k, matrisiz, rowlim, rowliml, colimu, fdl, fdu;
  int froml, fromu, ds = sizeof(double), readl, readu;
  char name[3];
  double *matriu, temp;
  extern int fulldirops;
  extern double lutime;

  /* figure how many doubles will fit in matrix
  matrisiz = siz*(siz + 1)/2; */

  /* figure max number of square matrix rows that can fit (truncate) */
  rowlim = (siz+1)/2;	/* total #rows */
  rowliml = rowlim/2;		/* number of rows in first chunk */
  colimu = rowlim - rowliml;	/* number of rows in second chunk */
  matriu = matri + rowliml*siz; /* pointer to second block of rows */

  /* matri always holds rows from matl; matri1 holds columns from matu */
  /* open the relvant files */
  if((fdl = open(getName(matl, name), 0)) == -1) {
    fprintf(stderr, "subInnerProd: can't open '%s'\n", name);
    exit(1);
  }

  /* for each matl chunk, read in all chunks of matu and do inner products */
  for(froml = 0; froml < siz; froml += rowliml) { /* loop on rows in l part */
    readl = read(fdl, (char *)matri, rowliml*siz*ds);
    if(readl % (siz*ds) != 0) {	/* must read in row size chunks */
      fprintf(stderr, "subInnerProd: read error from '%s'\n", 
	      getName(matl, name));
      exit(1);
    }
    readl /= (siz*ds);
    if((fdu = open(getName(matu, name), 0)) == -1) { /* (re)open u part */
      fprintf(stderr, "subInnerProd: can't open '%s'\n", name);
      exit(1);
    }
    for(fromu = 0; fromu < siz; fromu += colimu) { /* loop on cols in u part */
      fprintf(stderr, "%d-%d ", froml, fromu);
      readu = read(fdu, (char *)matriu, colimu*siz*ds);
      if(readu % (siz*ds) != 0) {	/* must read in col size chunks */
	fprintf(stderr, "subInnerProd: read error from '%s'\n", 
		getName(matu, name));
	exit(1);
      }
      readu /= (siz*ds);
      /* do the inner product/subtractions possible with these chunks */
      starttimer;
      for(i = 0; i < readl; i++) { /* loop on rows */
	for(j = 0; j < readu; j++) {	/* loop on columns */
	  for(k = 0, temp = 0.0; k < siz; k++) { /* inner product loop */
	    /* matriu indices are flipped since it was stored as columns */
	    temp += matri[SQDEX(i, k, siz)] * matriu[SQDEX(j, k, siz)];
	    fulldirops++;
	  }
	  /* indices must be offset to get right square matrix entry */
	  matsq[SQDEX(i + froml, j + fromu, siz)] -= temp;
	  fulldirops++;
	}
      }
      stoptimer;
      lutime += dtime;
    }

    /* close the u file so it can be reread for next set of l rows */
    close(fdu);
  }
  fprintf(stderr, "\n");
  close(fdl);
}


/*
  used only with DIRSOL == ON to do a direct LU and count the ops
  - returned matrix has L below the diagonal, U above (GVL1 pg 58)
  - meant to be used with 2x2 block matrix factorization
*/
void blkLudecomp(double *mat, int size)
{
  extern int fulldirops;
  double factor;
  int i, j, k;

  for(k = 0; k < size-1; k++) {	/* loop on rows */
    if(mat[SQDEX(k, k, size)] == 0.0) {
      fprintf(stderr, "blkLudecomp: zero piovt\n");
      exit(1);
    }
    fprintf(stderr,"%d ", k);
    for(i = k+1; i < size; i++) { /* loop on remaining rows */
      factor = (mat[SQDEX(i, k, size)] /= mat[SQDEX(k, k, size)]);
      fulldirops++;
      for(j = k+1; j < size; j++) { /* loop on remaining columns */
	mat[SQDEX(i, j, size)] -= (factor*mat[SQDEX(k, j, size)]);
	fulldirops++;
      }
    }
  }
  fprintf(stderr, "\n");
}

/*
  solves using factored matrix on disc
*/
void blkSolve(double *x, double *b, int siz, double *matri, double *matsq)
/* double *x, *b, *matri, *matsq;			solution, rhs */
{
  int i, j, k;
  extern int fulldirops;
  extern double fullsoltime;

  fprintf(stdout, "blkSolve: fwd elimination...");
  fflush(stdout);

  /* forward elimination, solve Ly = b (x becomes y) */
  for(i = 0; i < siz; i++) x[i] = b[i];	/* copy rhs */

  rdMat(matri, siz/2, L11, TRIMAT);
  /* a row in the result vector is a linear comb of the previous rows */
  /* do first (lower triangular only) part */
  starttimer;
  for(i = 1; i < siz/2; i++) {	/* loop on rows */
    for(k = 0; k < i; k++) {	/* loop on previous rows */
      x[i] -= (matri[LODEX(i, k, siz/2)]*x[k]);
      fulldirops++;
    }
  }
  stoptimer;
  fullsoltime += dtime;

  /* load L21 and LTIL */
  rdMat(matri, siz/2, LTIL, TRIMAT);
  rdMat(matsq, siz/2, L21, SQRMAT);

  /* do second (square and lower triangular) part */
  starttimer;
  for(; i < siz; i++) {		/* loop on rows of entire matrix */
    for(k = 0; k < siz/2; k++) { /* loop on first half of rows (L21) */
      x[i] -= (matsq[SQDEX(i-siz/2, k, siz/2)]*x[k]);
      fulldirops++;
    }
    for(; k < i; k++) {	/* loop on 2nd half of rows (LTIL) */
      x[i] -= (matri[LODEX(i-siz/2, k-siz/2, siz/2)]*x[k]);
      fulldirops++;
    }
  }
  stoptimer;
  fullsoltime += dtime;

  fprintf(stdout, "back substitution...");
  fflush(stdout);

  /* back substitute, solve Ux = y (x converted in place from y to x) */
  rdMat(matri, siz/2, UTIL, TRIMAT); /* load lower right U factor */
  starttimer;
  for(i = siz-1; i >= siz/2; i--) {	/* loop on rows */
    for(k = siz-1; k > i; k--) {	/* loop on rows (of x) already done */
      x[i] -= (matri[UPDEX(i-siz/2, k-siz/2, siz/2)]*x[k]);
      fulldirops++;
    }
    x[i] /= matri[UPDEX(i-siz/2, i-siz/2, siz/2)]; /* divide by u_{ii} */
    fulldirops++;
  }
  stoptimer;
  fullsoltime += dtime;

  /* load U11, U12 to do triangle plus square part of back solve */
  rdMat(matri, siz/2, U11, TRIMAT);
  rdMat(matsq, siz/2, U12, SQRMAT); /* U12 is stored columnwise */

  starttimer;
  for(; i >= 0; i--) {		/* loop on rows */
    for(k = siz-1; k >= siz/2; k--) { /* loop on rows corresponding to U12 */
      /* note flipped index because U12 stored as columns */
      x[i] -= (matsq[SQDEX(k-siz/2, i, siz/2)]*x[k]);
      fulldirops++;
    }
    for(; k > i; k--) {		/* loop on rows corresponding to cols of U11 */
      x[i] -= (matri[UPDEX(i, k, siz/2)]*x[k]);
      fulldirops++;
    }
    x[i] /= matri[UPDEX(i, i, siz/2)];
    fulldirops++;
  }
  stoptimer;
  fullsoltime += dtime;

  fprintf(stdout, "done.\n\n");
  fflush(stdout);
}

/*
  used only in conjunction with EXPGCR == ON  and DIRSOL == ON
  to dump full P to disc in four square chunks - N MUST BE EVEN
  - matrix stored in factored form on exit
  - used to get around memory restrictions
  - only 3/8 of the entire matrix is in core at any given time, rest on disc
    in L11, U11, U12, L21, Ltil and Util
*/
void blkQ2Pfull(cube *directlist,  int numchgs, int numchgs_wdummy, 
    double **triArray, double **sqrArray, int **real_index, int *is_dummy)
/* double **triArray, **sqrArray;	LINEAR arrays: 1 triangular, 1 square mat */
{
  int i, j, fromp, fromq, top, toq, matsize;
  int k, l, i_real, j_real;
  cube *pq, *pp;
  charge **pchgs, **qchgs, *ppan, *qpan;
  double pos_fact, neg_fact;

  /* allocate room for one 1/4 size square full P matrix, one 1/4 size tri.
     - linear arrays are used to cut down on memory overhead */
  /* allocate for read index array too */
  if(numchgs % 2 == 0) {	/* if numchgs is even */
    matsize = numchgs*numchgs/4;
    CALLOC(*sqrArray, matsize, double, ON, AMSC);
    matsize = ((numchgs/2)*(numchgs/2 + 1))/2;
    CALLOC(*triArray, matsize, double, ON, AMSC);
    CALLOC(*real_index, numchgs, int, ON, AMSC);
  }
  else {
    fprintf(stderr, "blkQ2Pfull: can't handle an odd number of panels\n");
    exit(1);
  }

  /* load the matrix in the style of Q2P() - no attempt to exploit symmetry */
  /* calculate interaction between every direct list entry and entire dlist */
  /* the block implementation MUST have all the charges in 1st dlist entry */
  pp = pq = directlist;
  if(pp == NULL || pp->dnext != NULL || pp->upnumeles[0] != numchgs_wdummy) {
    fprintf(stderr, "blkQ2Pfull: bad directlist, must run with depth 0\n");
    exit(1);
  }

  pchgs = qchgs = pp->chgs;

  /* get the real index array - indices of non-dummy panels */
  j = 0;
  for(i = 0; i < numchgs_wdummy; i++) {
    /* should be that pchgs[i]->index = i + 1 */
    /*  note COULD BE STRANGE DUE TO INDEXING FROM 1 */
    ASSERT(i == pchgs[i]->index - 1);
    if(!pchgs[i]->dummy) (*real_index)[j++] = i;
  }
  if(j != numchgs) {
    fprintf(stderr, "blkQ2Pfull: panel count and given #panels don't match\n");
    exit(1);
  }

  /* dump the four matrix sections */
  /* - if a charge panel is a dummy panel, skip the interaction calculation */
  /* - if a potential panel is on a BOTH or DIELEC surf, 
       include its dummy panels by converting the entry into a divided diff */
  for(fromp = 0, k = 0; k < 2; k++, fromp += numchgs/2) {
    for(fromq = 0, l = 0; l < 2; l++, fromq += numchgs/2) {

      for(i = 0; i < numchgs/2; i++) { /* loop on collocation points */
	i_real = (*real_index)[fromp+i];
	ASSERT(!(ppan = pchgs[i_real])->dummy);

	for(j = 0; j < numchgs/2; j++) { /* loop on charge panels */

	  /* real_index should eliminate all direct refs to dummy panels */
	  j_real = (*real_index)[fromq+j];
	  ASSERT(!(qpan = qchgs[j_real])->dummy);

	  (*sqrArray)[SQDEX(i, j, numchgs/2)] = calcp(qpan, ppan->x, ppan->y, 
						      ppan->z, NULL);
	  /* old: qchgs[from+j], pchgs[fromp+i] */
	  /* older: pchgs[fromp+i], qchgs[fromq+j] */

	  if(ppan->surf->type == DIELEC || ppan->surf->type == BOTH) {
	    /* add off-panel evaluation contributions to divided diffs */
	    /* see also dumpQ2PDiag() in mulDisplay.c */
	    pos_fact = ppan->surf->outer_perm/ppan->pos_dummy->area;
	    neg_fact = ppan->surf->inner_perm/ppan->neg_dummy->area;

	    /* effectively do one columns worth of two row ops, get div dif */
	    (*sqrArray)[SQDEX(i, j, numchgs/2)]
		= (pos_fact*calcp(qpan, ppan->pos_dummy->x, ppan->pos_dummy->y,
				  ppan->pos_dummy->z, NULL)
		   - (pos_fact + neg_fact)*(*sqrArray)[SQDEX(i, j, numchgs/2)]
		   + neg_fact*calcp(qpan, ppan->neg_dummy->x, 
				    ppan->neg_dummy->y, ppan->neg_dummy->z,
				    NULL));
	  }
	}
      }
      
      /* dump the 1/4 matrix to a file */
      if(k == 0 && l == 0) {
	wrMat(*sqrArray, numchgs/2, L11, SQRMAT);
	/* dumpMatCor((double **)NULL, *sqrArray, numchgs/2); */ /* for debug */
      }
      else if(k == 0 && l == 1) wrMat(*sqrArray, numchgs/2, U12, SQRMAT);
      else if(k == 1 && l == 0) wrMat(*sqrArray, numchgs/2, L21, SQRMAT);
      else wrMat(*sqrArray, numchgs/2, LTIL, SQRMAT);
    }
  }
  fprintf(stderr, "Initial dump to disk complete\n\n");
  fprintf(stdout, "Initial dump to disk complete\n\n");
  fflush(stdout);

}

/*
  does a block factorization into
  L11  0    U11 U12
  L21 LTI    0  UTI
  using four sections of A stored on disk as 
  A11 = L11, A12 = U12, A21 = L21, A22 = LTI
*/
void blkLUdecomp(double *sqrArray, double *triArray, int numchgs)
/* double *sqrArray, *triArray;	previously allocated flattened matrices */
/* int numchgs;			A is numchgsxnumchgs */
{
  extern double lutime;

  /* factor the stored matrices to give an overall stored factorization */
  /* load the A11 part */
  rdMat(sqrArray, numchgs/2, L11, SQRMAT);

  /* factor it in place */
  starttimer;
  blkLudecomp(sqrArray, numchgs/2);
  stoptimer;
  lutime += dtime;

  /* write out factors to different files */
  matXfer(sqrArray, triArray, numchgs/2, UP2TR); /* upper part to triArr */
  wrMat(triArray, numchgs/2, U11, TRIMAT);
  matXfer(sqrArray, triArray, numchgs/2, LO2TR); /* lower part to triArr */
  wrMat(triArray, numchgs/2, L11, TRIMAT);

  fprintf(stderr, "A11 factorization complete\n\n");
  fprintf(stdout, "\nblkLUdecomp: A11 factored...");
  fflush(stdout);

  /* load A12 and solve in place for U12 and write (L11 in position alrdy) */
  rdMat(sqrArray, numchgs/2, U12, SQRMAT);

  starttimer;
  blkMatsolve(sqrArray, triArray, numchgs/2, LOWMAT);
  stoptimer;
  lutime += dtime;

  wrMat(sqrArray, numchgs/2, U12, COLMAT); /* store as columns */

  fprintf(stderr, "A12 factorization complete\n\n");
  fprintf(stdout, "A12 factored...");
  fflush(stdout);

  /* load A21 and U11, solve in place for L21 and write */
  rdMat(triArray, numchgs/2, U11, TRIMAT);
  rdMat(sqrArray, numchgs/2, L21, SQRMAT);

  starttimer;
  blkMatsolve(sqrArray, triArray, numchgs/2, UPPMAT);
  stoptimer;
  lutime += dtime;

  wrMat(sqrArray, numchgs/2, L21, SQRMAT); /* store as rows */

  fprintf(stderr, "A21 factorization complete\n\n");
  fprintf(stdout, "A21 factored...");
  fflush(stdout);

  /* load A22 and subtract off (L21)(U12) product 1/4 matrix at a time */
  rdMat(sqrArray, numchgs/2, LTIL, SQRMAT);
  subInnerProd(sqrArray, triArray, numchgs/2, L21, U12); /* timed internally */

  /* factor Atilde and write Ltilde, Utilde */
  starttimer;
  blkLudecomp(sqrArray, numchgs/2);
  stoptimer;
  lutime += dtime;

  matXfer(sqrArray, triArray, numchgs/2, UP2TR); /* upper part to triArr */
  wrMat(triArray, numchgs/2, UTIL, TRIMAT);
  matXfer(sqrArray, triArray, numchgs/2, LO2TR); /* lower part to triArr */
  wrMat(triArray, numchgs/2, LTIL, TRIMAT);

  fprintf(stderr, "Block factorization complete\n\n");
  fprintf(stdout, "done.\n");
  fflush(stdout);
}

/*
  does a matrix vector multiply with the matrix stored on disk in 4 blocks:
  L11 U12
  L21 LTI
*/
void blkAqprod(double *p, double *q, int size, double *sqmat)
/* int size;			A is size by size */
/* double *p, *q;			p = Aq is calculated */
/* double *sqmat;			flat storage space for 1/4 of A */
{
  int i, j, k, l, fromp, fromq;
  extern int fullPqops;
  extern double dirtime;

  for(fromp = 0, k = 0; k < 2; k++, fromp += size/2) {
    for(fromq = 0, l = 0; l < 2; l++, fromq += size/2) {
      
      /* read in the correct 1/4 matrix to a file */
      if(k == 0 && l == 0) rdMat(sqmat, size/2, L11, SQRMAT);
      else if(k == 0 && l == 1) rdMat(sqmat, size/2, U12, SQRMAT);
      else if(k == 1 && l == 0) rdMat(sqmat, size/2, L21, SQRMAT);
      else rdMat(sqmat, size/2, LTIL, SQRMAT);

      /* do the product for this section of the matrix */
      starttimer;
      for(i = 0; i < size/2; i++) {
	for(j = 0; j < size/2; j++) {
	  p[fromp+i] += sqmat[SQDEX(i, j, size/2)]*q[fromq+j];
	  fullPqops++;
	}
      }
      stoptimer;
      dirtime += dtime;

    }
  }
}

/*
  used to reduce a vector eval_size long to one up_size long
  - used w/block direct routines since they work with the condensed matrix
    (all zero columns removed and row ops done for divided differences)
  - should ultimately convert multipole over to condensed form, wont need this
*/
void blkCompressVector(double *vec, int num_panels, int real_size,
    int *is_dummy)
{
  int i, j;

  /* copy the entries of the vector corresponding to the real panels
     into the first up_size entries - is_dummy must be indexed from zero */
  for(i = j = 0; i < num_panels; i++) {
    if(!is_dummy[i]) vec[j++] = vec[i];
  }

  if(j != real_size) {
    fprintf(stderr, "blkCompressVector: number of real panels not right, %d\n",
	    j);
    exit(1);
  }
}

/*
  the inverse of the above function
*/
void blkExpandVector(double *vec, int num_panels, int real_size)
{
  int i, j, from, to, cur_real;
  extern int *real_index;	/* this index set relies on indexing from 0 */

  /* transfer to vector */
  for(i = real_size - 1; i >= 0; i--) {
    vec[real_index[i]] = vec[i];
  }

  /* zero out in between valid entries */
  from = 0;
  for(i = 0; i < real_size; i++) {
    for(j = from; j < real_index[i]; j++) {
      vec[j] = 0.0;
    }
    from = j + 1;
  }
}

