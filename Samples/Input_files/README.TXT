*** 
*
* Edited and converted for Windows by Enrico Di Lorenzo, www.fastfieldsolvers.com
*
* Windows version comments are marked by '*'
*
***

This directory contains example neutral format input files for `fastcap'
that are described in the User's Guide.


 Discretizations of 1 wire crossing 1 wire (all these examples may also be
 generated in generic file format using `busgen'):
  1x1coarse.neu - 28 panel uniform discretization (Figure 1(b) in User's Guide)
  1x1fine.neu - 252 panel uniform discretization (Figure 1(c))
  1x1nonuni.neu - 252 panel nonuniform discretization (Figure 1(d))


***
* 
* Replaced with:
*
*  busgen -c1 -n1 -e0 > _1x1coarse.qui
*  _1x1coarse.qui - 28 panel uniform discretization 
*
*  busgen -c1 -e0 > _1x1fine.qui
*  _1x1fine.qui - 252 panel uniform discretization
*
*  busgen -c1 -e0.4 > _1x1nonuni.qui 
*  _1x1nonuni.qui - 252 panel nonuniform discretization 
*
***

 Disretizations of a 1 meter radius sphere:
  sphere1.neu - 104 panel discretization (Fig. 4, left)
  sphere2.neu - 768 panel discretization (Fig. 4, right)
  sphere3.neu - 1200 panel discretization (not pictured)

***
*
* Replaced with:
*
*  sphere0.qui - 20 panel discretization
*  
*  sphere1.qui - 80 panel discretization
*
*  sphere2.qui - 320 panel discretization
*
*  sphere3.qui - 1280 panel discretization
*
***


Other examples may be generated in generic file format using the input
file generators `capgen' (|| plate capacitors), `cubegen' (cubic capacitors),
`pipedgen' (parallelepiped capacitors), `pyragen' (pyramid capacitors)
and `busgen' (buscrossing problem) -- see the User's Guide for details.



Via through two power planes example - via.neu
 6120 panels, no dielectric interfaces --- produces mulitple panel
 warning due to an error in setting up the neutral file; result is
 not affected

*** 
*
*  Kept via.neu
*
***


Examples with multiple dielectrics

 1. 1x1bus.lst - 664 panel discretization of two crossing wires; the
    lower wire is coated with a conformal dielectric layer
    run using `fastcap -l1x1bus.lst' -- Figure 3 in the Guide

*** 
*
*  Kept 1x1bus.lst
*
***

 2. sixteen-pin backplane connector problem - 9524 panel discretization
    CAUTION: this problem requires around 170MB to run and takes about
	25 CPU minuites on an IBM RS6000/540
    generate input list file `connector43.5-n3.lst' using the command
	`connector4.sh 3.5 -n3'
    here 3.5 is the relative permittivity of the U-shaped connector body
    this requires the input file generators `../bin/pyragen' 
	and `../bin/cubegen'
    run fastcap using the command
	`fastcap -lconnector43.5-n3.lst'
    to generate a coarse discretization for plotting, use
	`connector4.sh 3.5 -d'
    and generate postscript plot file `connector43.5-d.ps' using
	`fastcap -m -lconnector43.5-d.lst'

*** 
*
*  Can generated using batch file 'connector4.bat', same options
*
***

 3. nine-pin backplane connector problem - 5568 panel discretization
    follow same steps as in 2, substituting `connector3' for `connector4'
    CAUTION: this problem requires around 85MB to run and takes about
	6 CPU minuites on an IBM RS6000/540

*** 
*
*  Can generated using batch file 'connector3.bat', same options
*
***

 4. four-pin backplane connector problem - as before with `connector2'

*** 
*
*  Can generated using batch file 'connector2.bat', same options
*
***

 5. test.lst - generate with testgen.sh; example runs in testrun.sh,
    Figure 9 in the manual

*** 
*
*  Can be generated using batch file 'testrun.bat', same options
*  Testrun.sh is replaced by testrun.vbs, also showing automation,
*  see comments in the file for details
*
***

 6. ramcell.lst - generate with ramgen.sh; Figure 11 in the manual;
    output file, ramcell.out, is included

*** 
*
*  Not ported
*
***

 7. 2x2bus.lst - like 1x1bus.lst but instead 2 conductors crossing 2

*** 
*
*  Not present in original FastCap distribution, not added
*
***

 8. coated_sph.lst - coated sphere in Figure 2

*** 
*
*  Kept coated_sph.lst
*
***


