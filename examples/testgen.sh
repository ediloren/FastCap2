#!/bin/csh
#
# script to generate test problems --- run with testrun.sh
# - problems are meant to exercised interface, not engine
#
#   set up parameters
set exam = .
set exe = ../bin
set dest = .
set cubegen = ${exe}/cubegen
set pyragen = ${exe}/pyragen
set busgen = ${exe}/busgen
set top_perm = 3
set bot_perm = 1
#
#   generate tetrahedron (single conductor in its own file)
#   translate in list file by (0 3 0)
${pyragen} -naPYRAMID > ${dest}/pyramid.qui
#
#   generate 1x1 bus crossing (single file with multiple conductors)
#   no translation in list file
${busgen} -c1 -xo2 -yo1 > ${dest}/bus.qui
#
#   generate L-shape (single object built from 3 separate files)
${cubegen} -naLSHAPE -pfl -pfr > ${dest}/lcntr.qui
#   translate in list file by (1 0 0)
${cubegen} -naLSHAPE -pbr > ${dest}/lleft.qui
#   translate in list file by (0 1 0)
${cubegen} -naLSHAPE -pbl > ${dest}/lright.qui
#
#   generate top an bottom ground planes (one file, called 3x in list file)
#   translate by (-1 -1 -1), (-1 -1 5) and (-1 -1 4)
${cubegen} -naGND -p -t -xh7 -yh6 -e0 -n10 > ${dest}/plane.qui
#
#   generate dielectric interface skirt for high permittivities
#   translate by (-1 -1 4)
${cubegen} -t -b -xh7 -yh6 -e0 -n2 > ${dest}/skirt.qui
#
#   generate list file 
echo "*" > ${dest}/test.lst
echo "* test geometry to show how interface works" >> ${dest}/test.lst
echo "* - a pyramid, an L-shape and a 1x1 bus xing btwn gnd planes" \
>> ${dest}/test.lst
echo "* - two dielectric slabs between ground planes" >> ${dest}/test.lst
echo "*" >> ${dest}/test.lst
echo "**   pyramid (single conductor in its own file)" >> ${dest}/test.lst
echo "C ${dest}/pyramid.qui ${bot_perm}  0 3 0" >> ${dest}/test.lst
echo "**   1x1 bus crossing (single file with multiple conductors)" \
>> ${dest}/test.lst
echo "G BUS" >> ${dest}/test.lst
echo "C ${dest}/bus.qui ${bot_perm}  0 0 0" >> ${dest}/test.lst
echo "**   L-shaped conductor (single object built from 3 separate files)" \
>> ${dest}/test.lst
echo "C ${dest}/lcntr.qui ${bot_perm}  0 0 0 +" >> ${dest}/test.lst
echo "C ${dest}/lleft.qui ${bot_perm}  1 0 0 +" >> ${dest}/test.lst
echo "C ${dest}/lright.qui ${bot_perm}  0 1 0" >> ${dest}/test.lst
echo "**   ground planes (use of same file repeatedly)" >> ${dest}/test.lst
echo "C ${dest}/plane.qui ${bot_perm}  -1 -1 -1 +" >> ${dest}/test.lst
echo "C ${dest}/plane.qui ${top_perm}  -1 -1 5" >> ${dest}/test.lst
#echo "C ${dest}/plane.qui ${bot_perm}  -1 -1 5" >> ${dest}/test.lst
echo "**   dielectric interface (same file as ground planes)" \
>> ${dest}/test.lst
echo "D ${dest}/plane.qui ${top_perm} ${bot_perm}  -1 -1 4  0 0 5" \
>> ${dest}/test.lst
echo "D ${dest}/skirt.qui ${bot_perm} ${top_perm}  -1 -1 4  0 0 4.5 -" \
>> ${dest}/test.lst
