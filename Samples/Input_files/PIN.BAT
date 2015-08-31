:: Unix shell script converted to batch file for MSDOS
:: by Enrico Di Lorenzo
::
:: shell script for generating a pin for the connector problem
:: usage: pin <xo> <yo> <zo> <relperm_at_sleeve> <id> <passed_directly>
::  all args must be given always (interface does no error checking)
::  use passed directly arg as "-d" to turn off all discretization (for plots)
::  called by script `connector4' 
::  requires input file generators `pyragen' and `cubegen'
::
setlocal
set xh=1.3
set yh=1.3
set xyh=-xh%xh% -yh%yh%
set pnth=1.0
set offset=%1 %2 %3
set ex=%1%2%3%4%5%6
set refpnt=0.0 0.0 2.5
::
:: top end
call pyragen -b -zo20.2 %xyh% -zh%pnth% %6 > %7/pin_end1%ex%.qui
:: long tube
call cubegen -t -b -zo5.2 %xyh% -zh15.0 %6 > %7/pin_long%ex%.qui
:: sleeve
call cubegen -t -b %xyh% -zh5.2 %6 > %7/pin_sleeve%ex%.qui
:: short tube
call cubegen -t -b -zo-7.0 %xyh% -zh7.0 %6 > %7/pin_short%ex%.qui
:: bottom end
call pyragen -b -zo-7.0 %xyh% -zh-%pnth% %6 > %7/pin_end2%ex%.qui
:::::::: dielectric panels on back of conductor
:: small panel, above pin
call cubegen -p -yo1.3 -xh1.3 -yh2.6 -zh5.2 -e0 -n2 %6 > %7/pin_sm_a%ex%.qui
:: small panel, below pin
call cubegen -p -yo-2.6 -xh1.3 -yh2.6 -zh5.2 -e0 -n2 %6 > %7/pin_sm_b%ex%.qui
:: big panel, left of pin
call cubegen -p -yo-2.6 -xo-2.6 -xh2.6 -yh6.5 -zh5.2 -e0 -n4 %6 > %7/pin_big_l%ex%.qui
:: big panel, right of pin
call cubegen -p -yo-2.6 -xo1.3 -xh2.6 -yh6.5 -zh5.2 -e0 -n4 %6 > %7/pin_big_r%ex%.qui
::
:: write list file
::
echo * pin%ex%.lst > pin%ex%.lst
echo C %7/pin_end1%ex%.qui 1.0 %offset% + >> pin%ex%.lst
echo C %7/pin_long%ex%.qui 1.0 %offset% + >> pin%ex%.lst
echo C %7/pin_sleeve%ex%.qui %4 %offset% + >> pin%ex%.lst
echo C %7/pin_short%ex%.qui 1.0 %offset% + >> pin%ex%.lst
echo C %7/pin_end2%ex%.qui 1.0 %offset% >> pin%ex%.lst
echo D %7/pin_sm_a%ex%.qui 1.0 %4 %offset% %refpnt% - >> pin%ex%.lst
echo D %7/pin_sm_b%ex%.qui 1.0 %4 %offset% %refpnt% - >> pin%ex%.lst
echo D %7/pin_big_l%ex%.qui 1.0 %4 %offset% %refpnt% - >> pin%ex%.lst
echo D %7/pin_big_r%ex%.qui 1.0 %4 %offset% %refpnt% - >> pin%ex%.lst
::
:: update final .lst file 
::
type pin%ex%.lst >> connector%5%4%6.lst
:: remove temp files
del pin%ex%.lst
endlocal
