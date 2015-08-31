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

#if OPCNT == ON
static directops=0, upops=0, downops=0, evalops=0;
#endif

/* SRW */
void mulDirect(ssystem*);
void mulPrecond(ssystem*, int);
void mulUp(ssystem*);
void mulEval(ssystem*);
void mulDown(ssystem*);
#if OPCNT == ON
void printops(void);
#endif


/* 
Compute the direct piece. 
*/
void mulDirect(ssystem *sys)
{
int i, j, k, dsize, *is_dummy, *is_dielec;
double pc, *p, *q, *qn, *pn, **mat;
cube *nextc;

/* Assumes the potential vector has been zero'd!!!! */
  for(nextc=sys->directlist; nextc != NULL; nextc = nextc->dnext) {
    dsize = nextc->directnumeles[0];  /* Equals number of charges. */
    q = nextc->directq[0];   
    p = nextc->eval;
    is_dummy = nextc->nbr_is_dummy[0];
    is_dielec = nextc->is_dielec;
  /* Inside Cube piece. */
    mat = nextc->directmats[0];
    for(j = dsize - 1; j >= 0; j--) {
#if NUMDPT == 2
      if(is_dielec[j]) continue;
#endif
      for(k = dsize - 1; k >= 0; k--) {
	if(!is_dummy[k]) p[j] += mat[j][k] * q[k];
#if OPCNT == ON
	directops++;
#endif
      }
    }
  /* Through all nearest nbrs. */
    for(i=nextc->directnumvects - 1; i > 0; i--) {
      mat = nextc->directmats[i];
      qn = nextc->directq[i];
      is_dummy = nextc->nbr_is_dummy[i];
      for(j = dsize - 1; j >= 0; j--) {
#if NUMDPT == 2
	if(is_dielec[j]) continue;
#endif
	for(k = nextc->directnumeles[i] - 1; k >= 0; k--) {
	  if(!is_dummy[k]) p[j] += mat[j][k] * qn[k];
#if OPCNT == ON
	  directops++;
#endif
	}
      }
    }
  }
}

/*
Block diagonal or Overlapped Preconditioner.
*/
void mulPrecond(ssystem *sys, int type)
{
  int i, j, k, dsize, *is_dummy;
  double pc, *p, *q, *qn, *pn, **mat;
  cube *nc;

  if(type == BD) {
    for(nc=sys->precondlist; nc != NULL; nc = nc->pnext) {
      solve(nc->precond, nc->prevectq, nc->prevectq, nc->presize);
    }
  }
  else {
    /* Assumes the potential vector has been zero'd!!!! */
    for(nc=sys->directlist; nc != NULL; nc = nc->dnext) {
      dsize = nc->directnumeles[0];  /* Equals number of charges. */
      q = nc->directq[0];   
      p = nc->eval;
      is_dummy = nc->nbr_is_dummy[0];
      /* Inside Cube piece. */
      mat = nc->precondmats[0];
      for(j = dsize - 1; j >= 0; j--) {
	for(k = dsize - 1; k >= 0; k--) {
	  if(!is_dummy[k]) p[j] += mat[j][k] * q[k];
	}
      }
      /* Through all nearest nbrs. */
      for(i=nc->directnumvects - 1; i > 0; i--) {
	mat = nc->precondmats[i];
	is_dummy = nc->nbr_is_dummy[i];
	if(mat != NULL) {
	  qn = nc->directq[i];
	  for(j = dsize - 1; j >= 0; j--) {
	    for(k = nc->directnumeles[i] - 1; k >= 0; k--) {
	      if(!is_dummy[k]) p[j] += mat[j][k] * qn[k];
	    }
	  }
	}
      }
    }
    /* Copy ps back to qs and zero ps. */
    for(nc=sys->directlist; nc != NULL; nc = nc->dnext) {
      dsize = nc->directnumeles[0];  /* Equals number of charges. */
      q = nc->directq[0];   
      p = nc->eval;
      for(j = dsize - 1; j >= 0; j--) {
	q[j] = p[j];
	p[j] = 0.0;
      }
    }
  }
}
  

/* 
Loop through upward pass. 
*/
void mulUp(ssystem *sys)
{
int i, j, k, l;
int msize;
double *multi, *rhs, **mat;
cube *nextc;

  if(sys->depth < 2) return;	/* ret if upward pass not possible/worth it */

/* Through all the depths, starting from the bottom and not doing top. */
  for(i = sys->depth; i > 0; i--) {  
  /* Through all the cubes at depth. */
    for(nextc=sys->multilist[i]; nextc != NULL; nextc = nextc->mnext) {
      msize = nextc->multisize;
      multi = nextc->multi;
      for(j=0; j < msize; j++) multi[j] = 0;
    /* Through all the nonempty children of cube. */
      for(j=nextc->upnumvects - 1; j >= 0; j--) {
	mat = nextc->upmats[j];
	rhs = nextc->upvects[j];
	for(k = nextc->upnumeles[j] - 1; k >= 0; k--) {
	  for(l = msize - 1; l >= 0; l--) {
	    multi[l] += mat[l][k] * rhs[k];
#if OPCNT == ON
	    upops++;
#endif
	  }
	}
      }
    }
  }
}


/*
  evaluation pass - use after mulDown or alone. 
*/
void mulEval(ssystem *sys)
{
  int i, j, k, size, *is_dielec;
  cube *nc;
  double *eval, **mat, *vec;

  if(sys->depth < 2) return;	/* ret if upward pass not possible/worth it */

  for(nc = sys->directlist; nc != NULL; nc = nc->dnext) {
    size = nc->upnumeles[0];	/* number of eval pnts (chgs) in cube */
    eval = nc->eval;		/* vector of evaluation pnt potentials */
    is_dielec = nc->is_dielec;	/* vector of DIELEC/BOTH panel flags */

    /* do the evaluations */
    for(i = nc->evalnumvects - 1; i >= 0; i--) {
      mat = nc->evalmats[i];
      vec = nc->evalvects[i];
      for(j = size - 1; j >= 0; j--) {
#if NUMDPT == 2
	if(is_dielec[j]) continue;
#endif
	for(k = nc->evalnumeles[i] - 1; k >= 0; k--) {
	  eval[j] += mat[j][k] * vec[k];
#if OPCNT == ON
	  evalops++;
#endif
	}
      }
    }
  }
}

/* 
Loop through downward pass. 
*/
void mulDown(ssystem *sys)
{
  cube *nc;
  int depth, i, j, k, lsize;
  double **mat, *rhs, *local;

  if(sys->depth < 2) return;	/* ret if upward pass not possible/worth it */

  for(depth=2; depth <= sys->depth; depth++) {
    for(nc=sys->locallist[depth]; nc != NULL; nc = nc->lnext) {
      lsize = nc->localsize;
      local = nc->local;
      for(j=0; j < lsize; j++) local[j] = 0;
  /* Through all the locals for the cube. */
      for(i=nc->downnumvects - 1; i >= 0; i--) {
	mat = nc->downmats[i];
	rhs = nc->downvects[i];
	for(j = lsize - 1; j >= 0; j--) {
	  for(k = nc->downnumeles[i] - 1; k >= 0; k--) {
	    local[j] += mat[j][k] * rhs[k];
#if OPCNT == ON
	    downops++;
#endif
	  }
	}
      }
    }
  }
}


#if OPCNT == ON
printops()
{
  printf("Number of Direct Multi-Adds = %d\n", directops);
  printf("Number of Upward Pass Multi-Adds = %d\n", upops);
  printf("Number of Downward Pass Multi-Adds = %d\n", downops);
  printf("Number of Evaluation Pass Multi-Adds = %d\n", evalops);
  printf("Total Number of Multi-Adds = %d\n", directops+upops+downops+evalops);
}
#endif
