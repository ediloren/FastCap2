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
#include "zbufGlobal.h"

/* SRW */
int capsolve(double***, ssystem*, charge*, int, int, int, Name*);
int oldgcr(ssystem*, double*, double*, double*, double*, double**, double**,
    int, int, int, double, charge*);
void oldcomputePsi(ssystem*, charge*);
int gcr(ssystem*, double*, double*, double*, double*, double**, double**,
    int, int, double, charge*);
int gmres(ssystem*, double*, double*, double*, double*, double**, double**,
    int, int, double, charge*);
void computePsi(ssystem*, double*, double*, int, charge*);


/* This routine takes the cube data struct and computes capacitances. */
int capsolve(double ***capmat, ssystem *sys, charge *chglist, int size,
    int real_size, int numconds, Name *name_list)
/* double ***capmat;		pointer to capacitance matrix */
/* int size, numconds, real_size;	real_size = total #panels, incl dummies */
{
  int i, cond, iter, maxiter = MAXITER, ttliter = 0;
  charge *nq;
  double *q, *p, *r, *ap;
  double **bp, **bap;
  extern double fullsoltime;
  surface *surf;
  extern ITER *kill_num_list, *kinp_num_list;
  extern double iter_tol;

#if CAPVEW == ON
  extern ITER *qpic_num_list;
  extern int q_;
  extern int dd_;
#endif

#if DIRSOL == ON
  extern double *trimat, *sqrmat; /* globals in blkDirect.c */
#endif

  /* Allocate space for the capacitance matrix. */
  CALLOC((*capmat), numconds+1, double*, ON, AMSC);
  for(i=1; i <= numconds; i++)  {
    CALLOC((*capmat)[i], numconds+1, double, ON, AMSC);
  }

  /* Allocate space for cg vectors , r=residual and p=projection, ap = Ap. */
  CALLOC(q, size+1, double, ON, AMSC);
  CALLOC(r, size+1, double, ON, AMSC);

#if DIRSOL != ON		/* too much to allocate if not used */
  /* allocate for gcr accumulated basis vectors (moved out of loop 30Apr90) */
  fflush(stdout);		/* so header will be saved if crash occurs */

  CALLOC(bp, maxiter+1, double*, ON, AMSC);
  CALLOC(bap, maxiter+1, double*, ON, AMSC);

  /* moved inside of gcr to save memory 22OCT90
  for(i=0; i <= maxiter; i++) {
    CALLOC(bp[i], size+1, double, ON, AMSC);
    CALLOC(bap[i], size+1, double, ON, AMSC);
  } */
#endif

  /* ualloc_verify(); */

  /* P is the "psuedo-charge" for multipole. Ap is the "psuedo-potential". */
  p = sys->q;
  ap = sys->p;

  /* Loop through all the conductors. */
  for(cond=1; cond <= numconds; cond++) {
    
    /* skip conductors in the -rs and the -ri kill list */
    if(want_this_iter(kill_num_list, cond)
/* SRW -- error
       || want_this_iter(kinp_num_list)) continue; */
       || want_this_iter(kinp_num_list, cond)) continue;
/* end fix */

    fprintf(stdout, "\nStarting on column %d (%s)\n", cond, 
	    getConductorName(cond, &name_list));
    fflush(stdout);

    /* Set up the initial residue vector and charge guess. */
    for(i=1; i <= size; i++) r[i] = q[i] = 0.0;
    i = 0;
    for(nq = chglist; nq != NULL; nq = nq->next) {
      if(nq->cond == cond && !nq->dummy 
	 && (nq->surf->type == CONDTR || nq->surf->type == BOTH)) 
	  r[nq->index] = 1.0;
    }

#if DIRSOL == ON
    /* do a direct forward elimination/back solve for the charge vector */
    if(size > MAXSIZ) {		/* index from 1 here, from 0 in solvers */
      blkCompressVector(r+1, size, real_size, sys->is_dummy+1);
      blkSolve(q+1, r+1, real_size, trimat, sqrmat);
      blkExpandVector(q+1, size, real_size);
    }
    else {
      starttimer;
      solve(sys->directlist->directmats[0], q+1, r+1, size);
      stoptimer;
      fullsoltime += dtime;
    }
    iter = 0;
#else
    /* Do gcr. First allocate space for back vectors. */
    /* allocation moved out of loop 30Apr90 */
#if ITRTYP == GMRES			       
    if((iter = gmres(sys,q,p,r,ap,bp,bap,size,maxiter,iter_tol,chglist)) 
       > maxiter) {
      fprintf(stderr, "NONCONVERGENCE AFTER %d ITERATIONS\n", maxiter);
      exit(1);
    }
#else
    if((iter = gcr(sys,q,p,r,ap,bp,bap,size,maxiter,iter_tol,chglist)) 
       > maxiter) {
      fprintf(stderr, "NONCONVERGENCE AFTER %d ITERATIONS\n", maxiter);
      exit(1);
    }
#endif				/* ITRTYP == GMRES */
    ttliter += iter;
#endif				/* DIRSOL == ON */

#if DMPCHG == LAST
    fprintf(stdout, "\nPanel charges, iteration %d\n", iter);
    dumpChgDen(stdout, q, chglist);
    fprintf(stdout, "End panel charges\n");
#endif

#if CAPVEW == ON
    /* dump shaded geometry file if only if this column picture wanted */
    /* (variable names are messed up - iter list now is list of columns) */
    if(want_this_iter(qpic_num_list, cond) || (q_ && qpic_num_list == NULL)) {
      dump_ps_geometry(chglist, q, cond, dd_);    
    }
#endif

    /* Calc cap matrix entries by summing up charges over each conductor. */
    /* use the permittivity ratio to get the real surface charge */
    /* NOT IMPLEMENTED: fancy stuff for infinitessimally thin conductors */
    /* (once again, permittivity data is poorly organized, lots of pointing) */
    for(i=1; i <= numconds; i++) (*capmat)[i][cond] = 0.0;
    for(nq = chglist; nq != NULL; nq = nq->next) {
      if(nq->dummy || (surf = nq->surf)->type != CONDTR) continue;
      (*capmat)[nq->cond][cond] += surf->outer_perm * q[nq->index];
    }

#if RAWDAT == ON
    if(ITRDAT == OFF) fprintf(stdout, "\n");
    fprintf(stdout, "cond=%d iters=%d\n", cond, iter);

    for(i=1; i <= numconds; i++) {
      fprintf(stdout, "c%d%d=%g  ", i, cond, (*capmat)[i][cond]);
      if(i % 4 == 0) fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n\n");
#endif

#if ITRDAT == ON && RAWDAT == OFF
    fprintf(stdout, "%d iterations\n", iter);
#endif

  }
  fflush(stdout);
  return(ttliter);
}


/* 
Unpreconditioned Generalized Conjugate Residuals.
*/
int oldgcr(ssystem *sys, double *q, double *p, double *r, double *ap,
    double **bp, double **bap, int size, int real_size, int maxiter,
    double tol, charge *chglist)
{
  int iter, i, j;
  double norm, beta, alpha, maxnorm;
  extern double conjtime;
#if EXPGCR == ON
  extern double *sqrmat;
#endif

  /* NOTE ON EFFICIENCY: all the loops of length "size" could have */
  /*   if(sys->is_dummy[i]) continue; as their first line to save some ops */
  /* currently the entries corresponding to dummy panels are set to zero */

  /* The multipole algorithm takes charges from p and puts them in ap. */
  /* Here it is more helpful if r is used. */
  for(i=1; i <= size; i++) {
    p[i] = r[i];
  }
  r = p;

  for(iter = 0; iter < maxiter; iter++) {

    /* allocate the back vectors if they haven't been already (22OCT90) */
    if(bp[iter] == NULL) {
      CALLOC(bp[iter], size+1, double, ON, AMSC);
      CALLOC(bap[iter], size+1, double, ON, AMSC);
    }

    /* Form the Ar product. */
    for(i=1; i <= size; i++) ap[i] = 0.0;

#if EXPGCR == ON
    blkCompressVector(r+1, size, real_size, sys->is_dummy+1);
    blkAqprod(ap+1, r+1, real_size, sqrmat);	/* offset since index from 1 */
    blkExpandVector(ap+1, size, real_size);
    blkExpandVector(r+1, size, real_size);
#else
/* SRW -- error
    computePsi(sys, chglist); */
    oldcomputePsi(sys, chglist);
/* end fix */
#endif

    starttimer;
    for(i=1; i <= size; i++) {
      bp[iter][i] = r[i];
      bap[iter][i] = ap[i];
    }

    /* Subtract the backward projections from p and ap. */
    for(j=0; j < iter; j++) {
      INNER(beta, ap, bap[j], size);
      for(i=1; i <= size; i++) {
	bp[iter][i] -= beta * bp[j][i];
	bap[iter][i] -= beta * bap[j][i];
      }
    }

    /* Normalize the p and ap vectors so that ap*ap = 1. */
    INNER(norm, bap[iter], bap[iter], size);
    norm = sqrt(norm);
    for(i=1; i <= size; i++) {
      bap[iter][i] /= norm;
      bp[iter][i] /= norm;
    }

    /* Compute the projection in the p direction and get the next p. */
    INNER(alpha, r, bap[iter], size);
    for(i=1; i <= size; i++) {
      q[i] += alpha * bp[iter][i];
      r[i] -= alpha * bap[iter][i];
    }

#if DMPCHG == ON
    fprintf(stdout, "\nPanel charges, iteration %d\n", iter+1);
    dumpChgDen(stdout, q, chglist);
    fprintf(stdout, "End panel charges\n");
#endif

    /* Check convergence. */
    for(maxnorm = 0.0, i=1; i <= size; i++) maxnorm = MAX(ABS(r[i]),maxnorm);
#if ITRDAT == ON
    INNER(norm, r, r, size);
    fprintf(stdout, "max res = %g ||res|| = %g\n", maxnorm, sqrt(norm));
#else
    fprintf(stdout, "%d ", iter+1);
    if((iter+1) % 15 == 0) fprintf(stdout, "\n");
#endif

    fflush(stdout);
    stoptimer;
    conjtime += dtime;
    if(maxnorm < tol) {      
      break;
    }
  }
  if(maxnorm >= tol) {
    fprintf(stdout, "\ngcr: WARNING exiting without converging\n");
  }
  return(iter+1);
}

/*
ComputePsi computes the potential from the charge vector.  It is 
assumed that the vectors for the charge and potential have already been 
set up and that the potential vector has been zeroed.  ARBITRARY
VECTORS CAN NOT BE USED!
*/
/* ultimately should not need to pass in chglist after E field rtn is fixed */
void oldcomputePsi(ssystem *sys, charge *chglist)
{
  extern double dirtime, uptime, downtime, evaltime;

  starttimer;
  mulDirect(sys);
  stoptimer;
  dirtime += dtime;

  starttimer;
  mulUp(sys);
  stoptimer;
  uptime += dtime;

#if DUPVEC == ON
  dumpLevOneUpVecs(sys);
#endif

  starttimer;
#if DNTYPE == NOSHFT
  mulDown(sys);		/* do downward pass without local exp shifts */
#endif

#if DNTYPE == GRENGD
  mulDown(sys);	       	/* do heirarchical local shift dwnwd pass */
#endif
  stoptimer;
  downtime += dtime;

  starttimer;
#if MULTI == ON
  mulEval(sys);		/* evaluate either locals or multis or both */
#endif
  stoptimer;
  evaltime += dtime;

#if CHKDUM == ON
  chkDummy(sys->p, sys->is_dummy, sys->up_size);
#endif

  /* convert the voltage vec entries on dielectric i/f's into eps1E1-eps2E2 */
  compute_electric_fields(sys, chglist);

#if CHKDUM == ON
  chkDummy(sys->p, sys->is_dummy, sys->up_size);
#endif

#if OPCNT == ON
  fprintf(stderr, "compile option OPCNT not implemented\n");
  printops();
  exit(1);
#endif
}



/* 
Preconditioned(possibly) Generalized Conjugate Residuals.
*/
int gcr(ssystem *sys, double *q, double *p, double *r, double *ap,
    double **bp, double **bap, int size, int maxiter, double tol,
    charge *chglist)
{
  int iter, i, j;
  double norm, beta, alpha, maxnorm;
  extern double conjtime;
#if EXPGCR == ON
  extern double *sqrmat;
#endif

  /* NOTE ON EFFICIENCY: all the loops of length "size" could have */
  /*   if(sys->is_dummy[i]) continue; as their first line to save some ops */
  /* currently the entries corresponding to dummy panels are set to zero */

  for(iter = 0; iter < maxiter; iter++) {

    /* allocate the back vectors if they haven't been already (22OCT90) */
    if(bp[iter] == NULL) {
      CALLOC(bp[iter], size+1, double, ON, AMSC);
      CALLOC(bap[iter], size+1, double, ON, AMSC);
    }

    for(i=1; i <= size; i++) {
      bp[iter][i] = p[i] = r[i];
    }

    computePsi(sys, p, ap, size, chglist);
    
    starttimer;
    for(i=1; i <= size; i++) {
      bap[iter][i] = ap[i];
    }
    
    /* Subtract the backward projections from p and ap. */
    for(j= 0; j < iter; j++) {
      INNER(beta, ap, bap[j], size);
      for(i=1; i <= size; i++) {
	bp[iter][i] -= beta * bp[j][i];
	bap[iter][i] -= beta * bap[j][i];
      }
    }
    
    /* Normalize the p and ap vectors so that ap*ap = 1. */
    INNER(norm, bap[iter], bap[iter], size);
    norm = sqrt(norm);
    for(i=1; i <= size; i++) {
      bap[iter][i] /= norm;
      bp[iter][i] /= norm;
    }
    
    /* Compute the projection in the p direction and get the next p. */
    INNER(alpha, r, bap[iter], size);
    for(i=1; i <= size; i++) {
      q[i] += alpha * bp[iter][i];
      r[i] -= alpha * bap[iter][i];
    }

    /* Check convergence. */
    for(maxnorm = 0.0, i=1; i <= size; i++) maxnorm = MAX(ABS(r[i]),maxnorm);
#if ITRDAT == ON
    INNER(norm, r, r, size);
    fprintf(stdout, "max res = %g ||res|| = %g\n", maxnorm, sqrt(norm));
#else
    fprintf(stdout, "%d ", iter+1);
    if((iter+1) % 15 == 0) fprintf(stdout, "\n");
#endif
    fflush(stdout);
    stoptimer;
    conjtime += dtime;
    if(maxnorm < tol) break;
  }
/*  
#if ITRDAT == ON
  printf("Total iterative loop iterations = %d\n", iter); 
#endif 
*/
  
#if PRECOND != NONE
  /* Undo the preconditioning to get the real q. */
  for(i=1; i <= size; i++) {
    p[i] = q[i];
    ap[i] = 0.0;
  }
  mulPrecond(sys, PRECOND);
  for(i=1; i <= size; i++) {
    q[i] = p[i];
  }
#endif
  
  if(maxnorm >= tol) {
    fprintf(stdout, "\ngcr: WARNING exiting without converging\n");
  }
  return(iter+1);
}


/* 
  Preconditioned(possibly) Generalized Minimum Residual. 
  */
int gmres(ssystem *sys, double *q, double *p, double *r, double *ap,
    double **bv, double **bh, int size, int maxiter, double tol,
    charge *chglist)
{
  int iter, i, j;
  double rnorm, norm, maxnorm=10.0;
  double beta, hi, hip1, length;
  extern double conjtime, prectime;
#if EXPGCR == ON
  extern double *sqrmat;
#endif

  static double *c=NULL, *s=NULL, *g=NULL, *y=NULL;
  
  starttimer;

  /* Allocation first time through. */
  if(c == NULL) {
    CALLOC(c, size+1, double, ON, AMSC);
    CALLOC(s, size+1, double, ON, AMSC);
    CALLOC(g, size+1, double, ON, AMSC);
    CALLOC(y, size+1, double, ON, AMSC);
  }
  
  /* Set up v^1 and g^0. */
  INNER(rnorm, r, r, size);
  rnorm = sqrt(rnorm);
  for(i=1; i <= size; i++) {
    p[i] = r[i] / rnorm;
    g[i] = 0.0;
  }
  g[1] = rnorm;

  stoptimer;
  conjtime += dtime;

#if ITRDAT == ON
  fprintf(stdout, "||res|| = %g\n", rnorm); /* initial guess residual norm */
#endif
  
  for(iter = 1; (iter <= maxiter) && (rnorm > tol); iter++) {
    
    starttimer;
    /* allocate the back vectors if they haven't been already */
    if(bv[iter] == NULL) {
      CALLOC(bv[iter], size+1, double, ON, AMSC);
      CALLOC(bh[iter], iter+2, double, ON, AMSC);
    }
    
    /* Save p as the v{iter}. */
    for(i=1; i <= size; i++) bv[iter][i] = p[i];
    
    stoptimer;
    conjtime += dtime;

    /* Form Av{iter}. */
    computePsi(sys, p, ap, size, chglist);

    starttimer;
    
    /* Initialize v^{iter+1} to Av^{iter}. */
    for(i=1; i <= size; i++) p[i] = ap[i];
    
    /* Make v^{iter+1} orthogonal to v^{i}, i <= iter. */
    for(j=1; j <= iter; j++) {
      INNER(hi, ap, bv[j], size);
      for(i=1; i <= size; i++) p[i] -= hi * bv[j][i];
      bh[iter][j] = hi;
    }
    
    /* Normalize v^{iter+1}. */
    INNER(norm, p, p, size);    
    norm = sqrt(norm);
    for(i=1; i <= size; i++) p[i] /= norm;
    bh[iter][iter+1] = norm;
    
    /* Apply rotations to new h column. */
    for(i=1; i < iter; i++) {
      hi = bh[iter][i];
      hip1 = bh[iter][i+1];
      bh[iter][i] = c[i] * hi - s[i] * hip1;
      bh[iter][i+1] = c[i] * hip1 + s[i] * hi;
    }
    
    /* Compute new rotations. */
    hi = bh[iter][iter];
    hip1 = bh[iter][iter+1];
    length = sqrt(hi * hi + hip1 * hip1);
    c[iter] = hi/length;
    s[iter] = -hip1/length;
    
    /* Apply new rotations. */
    bh[iter][iter] = c[iter] * hi - s[iter] * hip1;
    bh[iter][iter+1] = c[iter] * hip1 + s[iter] * hi;
    /* ASSERT(g[iter+1] == 0); WHY IS THIS HERE ??? */
    hi = g[iter];
    g[iter] = c[iter] * hi;
    g[iter+1] = s[iter] * hi;    
    
    rnorm = ABS(g[iter+1]);

    stoptimer;
    conjtime += dtime;
    
#if ITRDAT == ON
    fprintf(stdout, "||res|| = %g\n", rnorm);
#else
    fprintf(stdout, "%d ", iter);
    if((iter) % 15 == 0 && iter != 0) fprintf(stdout, "\n");
#endif
    fflush(stdout);
  }
  /* Decrement from the last increment. */
  iter--;

/*  
#if ITRDAT == ON
  printf("Total iterative loop iterations = %d\n", iter); 
#endif 
*/
  
  starttimer;
  
  /* Compute solution, note, bh is bh[col][row]. */
  for(i=1; i <= iter; i++) y[i] = g[i];
  for(i = iter; i > 0; i--) {
    y[i] /=  bh[i][i];
    for(j = i-1; j > 0; j--) {
      y[j] -= bh[i][j]*y[i];
    }
  }
  for(i=1; i <= size; i++) {
    q[i] = 0.0;
    for(j=1; j <= iter; j++) {
      q[i] += y[j] * bv[j][i];
    }
  }

  stoptimer;
  conjtime += dtime;
  
#if PRECOND != NONE
  /* Undo the preconditioning to get the real q. */
  starttimer;
  for(i=1; i <= size; i++) {
    p[i] = q[i];
    ap[i] = 0.0;
  }
  mulPrecond(sys, PRECOND);
  for(i=1; i <= size; i++) {
    q[i] = p[i];
  }
  stoptimer;
  prectime += dtime;
#endif

  if(rnorm > tol) {
    fprintf(stdout, "\ngmres: WARNING exiting without converging\n");
  }
  return(iter);
}

/* 
ComputePsi computes the potential from the charge vector, or may
include a preconditioner.  It is assumed that the vectors for the
charge and potential have already been set up and that the potential
vector has been zeroed.  ARBITRARY VECTORS CAN NOT BE USED.
*/

void computePsi(ssystem *sys, double *q, double *p, int size, charge *chglist)
{
  extern double dirtime, uptime, downtime, evaltime, prectime;
  extern int real_size;
  int i;

  ASSERT(p == sys->p);
  ASSERT(q == sys->q);

  for(i=1; i <= size; i++) p[i] = 0;

#if PRECOND != NONE
  starttimer;
  mulPrecond(sys, PRECOND);
  stoptimer;
  prectime += dtime;
#endif

#if EXPGCR == ON
  blkCompressVector(q+1, size, real_size, sys->is_dummy+1);
  blkAqprod(p+1, q+1, real_size, sqrmat);	/* offset since index from 1 */
  blkExpandVector(p+1, size, real_size); /* ap changed to p, r chged to q */
  blkExpandVector(q+1, size, real_size); /*    7 Oct 91 */
#else
  starttimer;
  mulDirect(sys);
  stoptimer;
  dirtime += dtime;

  starttimer;
  mulUp(sys);
  stoptimer;
  uptime += dtime;

#if DUPVEC == ON
  dumpLevOneUpVecs(sys);
#endif

#if DNTYPE == NOSHFT
  mulDown(sys);		/* do downward pass without local exp shifts */
#endif

#if DNTYPE == GRENGD
  mulDown(sys);	       	/* do heirarchical local shift dwnwd pass */
#endif
  stoptimer;
  downtime += dtime;

  starttimer;
#if MULTI == ON
  mulEval(sys);		/* evaluate either locals or multis or both */
#endif
  stoptimer;
  evaltime += dtime;

#if DMPCHG == LAST
  fprintf(stdout, "\nPanel potentials divided by areas\n");
  dumpChgDen(stdout, p, chglist);
  fprintf(stdout, "End panel potentials\n");
#endif

  /* convert the voltage vec entries on dielectric i/f's into eps1E1-eps2E2 */
  compute_electric_fields(sys, chglist);

#if OPCNT == ON
  printops();
  exit(0);
#endif				/* OPCNT == ON */
#endif				/* EXPGCR == ON */
}
