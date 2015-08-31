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

#define TOL 1e-10                /* tolerance on epsilon (machine precision) */
#define UPPEPS 1e-10              /* should be bigger than epsilon */
#define LOWEPS 1e-25             /* should be smaller than epsilon */

/*
  returns upper and lower bounds on machine precision (for doubles)
  - takes into account the limitations of the memory rep of a doubles
    by forcing the compiler to put all doubles in core
*/
void getEpsBnds(upper, lower)
double *upper, *lower;            /* current upper and lower epsilon bounds */
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
