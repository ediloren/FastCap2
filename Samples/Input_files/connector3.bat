:: Unix shell script converted to batch file for MSDOS
:: by Enrico Di Lorenzo
::
:: shell script for generating a connector problem
:: usage: connector3.sh <plastic_permittivity> <passed_directly>
::  all args must be given always (interface does no error checking)
::  use passed directly arg as "-d" to turn off all discretization (for plots)
::
@echo off 
setlocal
set ex=%1%2
set offset=0.0 0.0 0.0
set op=%2
set files=connector3%ex%_files
:: make the folder
mkdir %files%
:::::::: build the pins 
echo * 9 pin connector, plastic permittivity = %1, options: %op% > connector3%ex%.lst
call pin 13.0 13.0 0.0 %1 3 %2 %files%
call pin 0.0 13.0 0.0 %1 3 %2 %files%
call pin 6.5 13.0 0.0 %1 3 %2 %files%
call pin 0.0 6.5 0.0 %1 3 %2 %files%
call pin 6.5 6.5 0.0 %1 3 %2 %files%
call pin 13.0 6.5 0.0 %1 3 %2 %files%
call pin 0.0 0.0 0.0 %1 3 %2 %files%
call pin 6.5 0.0 0.0 %1 3 %2 %files%
call pin 13.0 0.0 0.0 %1 3 %2 %files%
:::::::: build the sides
call cubegen -e0 %3 -xo-7.8 -yo-2.6 -zo5.2 -xh5.2 -yh19.5 -zh18.8 -b %op% -n4 > %files%/side_l3%ex%.qui
call cubegen -e0 %3 -xo16.9 -yo-2.6 -zo5.2 -xh5.2 -yh19.5 -zh18.8 -b %op% -n4 > %files%/side_r3%ex%.qui
call cubegen -e0 %3 -xo-7.8 -yo-2.6 -xh5.2 -yh19.5 -zh5.2 -t -pfl %op% -n4 > %files%/corner_l3%ex%.qui
call cubegen -e0 %3 -xo16.9 -yo-2.6 -xh5.2 -yh19.5 -zh5.2 -t -pbr %op% -n4 > %files%/corner_r3%ex%.qui
call cubegen -e0 %3 -xo-2.6 -yo-2.6 -xh19.5 -yh19.5 -zh5.2 -t -b -pfl -pbr %op% -n4 > %files%/back3%ex%.qui
:::::::: add sides, side caps and back caps to .lst file
echo * body3%ex%.lst > body3%ex%.lst
echo D %files%/side_l3%ex%.qui 1.0 %1 %offset% -3.5 0.0 7.0 - >> body3%ex%.lst
echo D %files%/side_r3%ex%.qui 1.0 %1 %offset% 17.5 0.0 7.0 - >> body3%ex%.lst
echo D %files%/corner_l3%ex%.qui 1.0 %1 %offset% -3.5 0.0 2.5 - >> body3%ex%.lst
echo D %files%/corner_r3%ex%.qui 1.0 %1 %offset% 17.5 0.0 2.5 - >> body3%ex%.lst
echo D %files%/back3%ex%.qui 1.0 %1 %offset% 0.0 0.0 2.5 - >> body3%ex%.lst
type body3%ex%.lst >> connector3%ex%.lst
:: remove temp files
del body3%ex%.lst
endlocal

