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

#define TOL 1e-10                /* tolerance on epsilon (machine precision) */
#define UPPEPS 1e-10              /* should be bigger than epsilon */
#define LOWEPS 1e-25             /* should be smaller than epsilon */

/* SRW */
void getEpsBnds(double*, double*);

/*
  returns upper and lower bounds on machine precision (for doubles)
  - takes into account the limitations of the memory rep of a doubles
    by forcing the compiler to put all doubles in core
*/
void getEpsBnds(double *upper, double *lower)
/* double *upper, *lower;            current upper and lower epsilon bounds */
{
  double dif, tol, mid;
  double temp, one;

  double *difp = &dif;		/* to stop optimizer from putting */
  double *tolp = &tol;		/* variables in registers (not doing so */
  double *midp = &mid;		/* can lead to undully optomistic machine */
  double *tempp = &temp;	/* precision estimate that doesnt take */
  double *onep = &one;		/* memory storage rounding into account) */

  *upper = UPPEPS;
  *lower = LOWEPS;
  *onep = 1.0;

  *difp = *upper - *lower;
  *tolp = *difp/(*lower);
  while(*tolp > TOL) {
    *midp = (*upper + *lower)/2.0;
    *tempp = 1.0 + *midp;
    if(*tempp > *onep) *upper = *midp; 
    else *lower = *midp;
    *difp = *upper - *lower;
    *tolp = *difp/(*lower);
  }
}
