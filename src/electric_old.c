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

#include "mulGlobal.h"

/*
  converts the voltage vector entries corresponding to panels on dielectric
     interfaces into electric field boundary condition checks: 
     eps_outer*E_outer - eps_inner*E_inner
  this routine might be improved by fixing the way dummy, permittivity and h
     information is stored (more arrays and less pointer chasing)
  also - infinitesimally thin conductors on a dielectric i/f (surface type 
     BOTH) are not supported
*/
void compute_electric_fields(sys, chglist)
ssystem *sys;
charge *chglist;
{
  charge *cp, *dummy;
  double h, flux_density, *panel_voltages;
  surface *surf;

  /* for each dielectric panel, do two divided differences to get the */
  /*    gradient of the potential in the normal and anti-normal directions */
  /* store the divided difference where the real panel's voltage was */
  /* zero the dummy panel voltage entries so that iterative loop will be OK */
  /* - the zeros can be skipped in the iterative loop calculations */
  panel_voltages = sys->p;
  for(cp = chglist; cp != NULL; cp = cp->next) {
    if(cp->dummy) continue;

    if((surf = cp->surf)->type == DIELEC) {
      dummy = cp->pos_dummy;
      /* area field is divided difference step h for dummy panels */
      flux_density = surf->outer_perm * 
       (panel_voltages[dummy->index] - panel_voltages[cp->index])/dummy->area;

#if DMPELE == ON
      fprintf(stdout, 
	      "Electric flux density evaluation at (%g %g %g), panel %d\n",
	      cp->x, cp->y, cp->z, cp->index);
      fprintf(stdout, "  pos_dummy at (%g %g %g), potential = %g\n",
	      dummy->x, dummy->y, dummy->z, panel_voltages[dummy->index]);
      fprintf(stdout, "  normal deriv on + side = %g(%g - %g)/%g = %g\n",
	      surf->outer_perm,
	      panel_voltages[dummy->index], panel_voltages[cp->index],
	      dummy->area, flux_density);
#endif

      panel_voltages[dummy->index] = 0.0;

      dummy = cp->neg_dummy;

#if DMPELE == ON
      fprintf(stdout, "  neg_dummy at (%g %g %g), potential = %g\n",
	      dummy->x, dummy->y, dummy->z, panel_voltages[dummy->index]);
      fprintf(stdout, "  normal deriv on - side = %g(%g - %g)/%g = %g\n",
	      surf->inner_perm,
	      panel_voltages[cp->index], panel_voltages[dummy->index],
	      dummy->area, surf->inner_perm * 
       (panel_voltages[cp->index] - panel_voltages[dummy->index])/dummy->area);
#endif

      /* area field is divided difference step h for dummy panels */
      flux_density -= (surf->inner_perm * 
       (panel_voltages[cp->index] - panel_voltages[dummy->index])/dummy->area);
      panel_voltages[dummy->index] = 0.0;

      /* store the normal flux density difference */
      panel_voltages[cp->index] = flux_density;

#if DMPELE == ON
      fprintf(stdout, 
	      "  flux density difference (pos side - neg side) = %g\n", 
	      flux_density);
#endif
    }
  }
}
