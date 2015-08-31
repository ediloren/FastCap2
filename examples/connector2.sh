#!/bin/csh -f
#
# shell script for generating a connector problem
# usage: connector2.sh <plastic_permittivity> <passed_directly>
#  all args must be given always (interface does no error checking)
#  use passed directly arg as "-d" to turn off all discretization (for plots)
#
#echo "THIS SCRIPT SHOULD BE CHECKED OVER"
set cubegen = "../bin/cubegen -e0"
set ex = $1$2
set offset = "0.0 0.0 0.0"
set op = $2
#### build the pins 
echo "* 2 pin connector, plastic permittivity = $1, options: ${op}" >connector2${ex}.lst
pin 0.0 0.0 0.0 $1 2 $2
pin 6.5 0.0 0.0 $1 2 $2
pin 0.0 6.5 0.0 $1 2 $2
pin 6.5 6.5 0.0 $1 2 $2
#### build the sides
${cubegen} $3 -xo-7.8 -yo-2.6 -zo5.2 -xh5.2 -yh13 -zh18.8 -b ${op} -n4 > side_l2${ex}.qui
${cubegen} $3 -xo10.4 -yo-2.6 -zo5.2 -xh5.2 -yh13 -zh18.8 -b ${op} -n4 > side_r2${ex}.qui
${cubegen} $3 -xo-7.8 -yo-2.6 -xh5.2 -yh13 -zh5.2 -t -pfl ${op} -n4 > corner_l2${ex}.qui
${cubegen} $3 -xo10.4 -yo-2.6 -xh5.2 -yh13 -zh5.2 -t -pbr ${op} -n4 > corner_r2${ex}.qui
${cubegen} $3 -xo-2.6 -yo-2.6 -xh13 -yh13 -zh5.2 -t -b -pbr -pfl ${op} -n4 > back2${ex}.qui
#### add sides, side caps and back caps to .lst file
echo "* body2${ex}.lst" > body2${ex}.lst
echo "D side_l2${ex}.qui 1.0 $1 ${offset} -3.5 0.0 7.0 -" >> body2${ex}.lst
echo "D side_r2${ex}.qui 1.0 $1 ${offset} 11.0 0.0 7.0 -" >> body2${ex}.lst
echo "D corner_l2${ex}.qui 1.0 $1 ${offset} -3.5 0.0 2.5 -" >> body2${ex}.lst
echo "D corner_r2${ex}.qui 1.0 $1 ${offset} 11.0 0.0 2.5 -" >> body2${ex}.lst
echo "D back2${ex}.qui 1.0 $1 ${offset} 0.0 0.0 2.5 -" >> body2${ex}.lst
cat body2${ex}.lst >> connector2${ex}.lst

