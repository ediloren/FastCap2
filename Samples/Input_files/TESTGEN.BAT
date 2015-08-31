:: Unix shell script converted to batch file for MSDOS
:: by Enrico Di Lorenzo
::
:: script to generate test problems --- run with testrun.sh
:: - problems are meant to exercised interface, not engine
::
::   set up parameters
@echo off 
setlocal
set top_perm=3
set bot_perm=1
set files=testgen_files
:: make the folder
mkdir %files%
::
::   generate tetrahedron (single conductor in its own file)
::   translate in list file by (0 3 0)
call pyragen -naPYRAMID > %files%/pyramid.qui
::
::   generate 1x1 bus crossing (single file with multiple conductors)
::   no translation in list file
call busgen -c1 -xo2 -yo1 > %files%/bus.qui
::
::   generate L-shape (single object built from 3 separate files)
call cubegen -naLSHAPE -pfl -pfr > %files%/lcntr.qui
::   translate in list file by (1 0 0)
call cubegen -naLSHAPE -pbr > %files%/lleft.qui
::   translate in list file by (0 1 0)
call cubegen -naLSHAPE -pbl > %files%/lright.qui
::
::   generate top an bottom ground planes (one file, called 3x in list file)
::   translate by (-1 -1 -1), (-1 -1 5) and (-1 -1 4)
call cubegen -naGND -p -t -xh7 -yh6 -e0 -n10 > %files%/plane.qui
::
::   generate dielectric interface skirt for high permittivities
::   translate by (-1 -1 4)
call cubegen -t -b -xh7 -yh6 -e0 -n2 > %files%/skirt.qui
::
::   generate list file 
echo * > testgen.lst
echo * test geometry to show how interface works >> testgen.lst
echo * - a pyramid, an L-shape and a 1x1 bus xing btwn gnd planes >> testgen.lst
echo * - two dielectric slabs between ground planes >> testgen.lst
echo * >> testgen.lst
echo **   pyramid (single conductor in its own file) >> testgen.lst
echo C %files%/pyramid.qui %bot_perm%  0 3 0 >> testgen.lst
echo **   1x1 bus crossing (single file with multiple conductors) >> testgen.lst
echo G BUS >> testgen.lst
echo C %files%/bus.qui %bot_perm%  0 0 0 >> testgen.lst
echo **   L-shaped conductor (single object built from 3 separate files) >> testgen.lst
echo C %files%/lcntr.qui %bot_perm%  0 0 0 + >> testgen.lst
echo C %files%/lleft.qui %bot_perm%  1 0 0 + >> testgen.lst
echo C %files%/lright.qui %bot_perm%  0 1 0 >> testgen.lst
echo **   ground planes (use of same file repeatedly) >> testgen.lst
echo C %files%/plane.qui %bot_perm%  -1 -1 -1 + >> testgen.lst
echo C %files%/plane.qui %top_perm%  -1 -1 5 >> testgen.lst
::echo C %files%/plane.qui %bot_perm%  -1 -1 5 >> testgen.lst
echo **   dielectric interface (same file as ground planes) >> testgen.lst
echo D %files%/plane.qui %top_perm% %bot_perm%  -1 -1 4  0 0 5 >> testgen.lst
echo D %files%/skirt.qui %bot_perm% %top_perm%  -1 -1 4  0 0 4.5 - >> testgen.lst
endlocal

