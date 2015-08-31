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

/* SRW */
void compute_electric_fields(ssystem*, charge*);


/*
  converts the voltage vector entries corresponding to panels on dielectric
     interfaces into electric field boundary condition checks: 
     eps_outer*E_outer - eps_inner*E_inner
  this routine might be improved by fixing the way dummy, permittivity and h
     information is stored (more arrays and less pointer chasing)
  also - infinitesimally thin conductors on a dielectric i/f (surface type 
     BOTH) are not supported
*/
void compute_electric_fields(ssystem *sys, charge *chglist)
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
