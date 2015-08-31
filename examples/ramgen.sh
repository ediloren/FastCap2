#!/bin/csh -f
#
# generates the conductor parts of the ramcell example
#  usage: ramgen.sh [<option>]  [<option>]  [<option>]
#  options are passed directly to cubegen and pipedgen; useful ones:
#  -d (no discretization)
#  -nX (set number of panels per edge to at least X (default = 3))
#
set exe = ../bin
set dest = .
set cgen = ${exe}/cubegen
set pgen = ${exe}/pipedgen
set px = ${dest}/ramcell
set tox = 0.01
set sio2 = 3.9
set sini = 7
set tpol = 0.3
set tAl = 0.9
set pol2 = 0.7
set op = "$1 $2 $3"
# add this to remove ends
#########set re1 = -pfl
#########set re2 = -pbr
#########set re3 = -pbl
set re1 = ""
set re2 = ""
set re3 = ""
#
#  transistor-controlling polycide word line
set xp = "${op} -naWD_LINE_GATE"
echo "** transistor-controlling polycide word line" > ${px}$1$2$3.lst
${cgen} ${xp} -xo-4.6 -yo2.5 -xh1.2 -zh${tpol} -pbr ${re1} >${px}-4.6$1$2$3.qui
echo "C ${px}-4.6$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-3.4 -yo2.5 -zh${tpol} -pfl -pbr > ${px}-3.4$1$2$3.qui
echo "C ${px}-3.4$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-2.4 -yo2.5 -xh2.4 -zh${tpol} -pfl -pbr > ${px}-2.4$1$2$3.qui
echo "C ${px}-2.4$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -yo2.5 -zh${tpol} -pfl -pbr > ${px}0$1$2$3.qui
echo "C ${px}0$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo1 -yo2.5 -xh2.4 -zh${tpol} -pfl -pbr > ${px}1$1$2$3.qui
echo "C ${px}1$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo3.4 -yo2.5 -zh${tpol} -pfl -pbr > ${px}3.4$1$2$3.qui
echo "C ${px}3.4$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo4.4 -yo2.5 -xh1.2 -zh${tpol} -pfl ${re2} > ${px}4.4$1$2$3.qui
echo "C ${px}4.4$1$2$3.qui ${sio2} 0.0 0.0 ${tox}" >> ${px}$1$2$3.lst
#
#  other polycide word line (translate above line by (0,3,pol2))
set xp = "${op} -naWD_LINE_PLATE"
echo "** other polycide word line" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-4.6 -yo5.5 -zo${pol2} -xh1.2 -zh${tpol} -pbr ${re1} >${px}o-4.6$1$2$3.qui
echo "C ${px}o-4.6$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-3.4 -yo5.5 -zo${pol2} -zh${tpol} -pfl -pbr > ${px}o-3.4$1$2$3.qui
echo "C ${px}o-3.4$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-2.4 -yo5.5 -zo${pol2} -xh2.4 -zh${tpol} -pfl -pbr > ${px}o-2.4$1$2$3.qui
echo "C ${px}o-2.4$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -yo5.5 -zo${pol2} -zh${tpol} -pfl -pbr > ${px}o0$1$2$3.qui
echo "C ${px}o0$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo1 -yo5.5 -zo${pol2} -xh2.4 -zh${tpol} -pfl -pbr > ${px}o1$1$2$3.qui
echo "C ${px}o1$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo3.4 -yo5.5 -zo${pol2} -zh${tpol} -pfl -pbr > ${px}o3.4$1$2$3.qui
echo "C ${px}o3.4$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo4.4 -yo5.5 -zo${pol2} -xh1.2 -zh${tpol} -pfl ${re2} > ${px}o4.4$1$2$3.qui
echo "C ${px}o4.4$1$2$3.qui ${sio2} 0.0 0.0 ${tox}" >> ${px}$1$2$3.lst
#
#  storage node plate
echo "** storage node plate" >> ${px}$1$2$3.lst
set xp = "${op} -naPLATE"
${cgen} ${xp} -xo-4.6 -yo4.5 -xh10.2 -zh0.3 -pbl > ${px}p4.5$1$2$3.qui
echo "C ${px}p4.5$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-4.6 -yo5.5 -xh10.2 -zh0.3 -pbl -pfr > ${px}p5.5$1$2$3.qui
echo "C ${px}p5.5$1$2$3.qui ${sio2} 0.0 0.0 ${tox} +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-4.6 -yo6.5 -xh10.2 -yh2.5 -zh0.3 ${re3} -pfr > ${px}p6.6$1$2$3.qui
echo "C ${px}p6.6$1$2$3.qui ${sio2} 0.0 0.0 ${tox}" >> ${px}$1$2$3.lst
#
###  left Al word line
set xp = "${op} -naAL_WD_LINE"
echo "** left Al word line (view from +x towards xz plane)" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-4.6 -yo2.5 -xh1.2 -zh${tAl} -pbr -b ${re1} >${px}w-4.6$1$2$3.qui
echo "C ${px}w-4.6$1$2$3.qui ${sini} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-4.6 -yo2.5 -xh1.2 -zh${tAl} -p -t ${re1} >${px}wb-4.6$1$2$3.qui
echo "C ${px}wb-4.6$1$2$3.qui ${sio2} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
#
${cgen} ${xp} -xo-3.4 -yo2.5 -zh${tAl} -pfl -pbr -b > ${px}w-3.4$1$2$3.qui
echo "C ${px}w-3.4$1$2$3.qui ${sini} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-3.4 -yo2.5 -zh${tAl} -p -t > ${px}wb-3.4$1$2$3.qui
echo "C ${px}wb-3.4$1$2$3.qui ${sio2} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
#
${cgen} ${xp} -xo-2.4 -yo2.5 -xh2.4 -zh${tAl} -pfl -pbr -b > ${px}w-2.4$1$2$3.qui
echo "C ${px}w-2.4$1$2$3.qui ${sini} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo-2.4 -yo2.5 -xh2.4 -zh${tAl} -p -t > ${px}wb-2.4$1$2$3.qui
echo "C ${px}wb-2.4$1$2$3.qui ${sio2} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
#
${cgen} ${xp} -yo2.5 -zh${tAl} -pfl -pbr -b > ${px}w0$1$2$3.qui
echo "C ${px}w0$1$2$3.qui ${sini} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
${cgen} ${xp} -yo2.5 -zh${tAl} -p -t > ${px}wb0$1$2$3.qui
echo "C ${px}wb0$1$2$3.qui ${sio2} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
#
${cgen} ${xp} -xo1 -yo2.5 -xh2.4 -zh${tAl} -pfl -pbr -b > ${px}w1$1$2$3.qui
echo "C ${px}w1$1$2$3.qui ${sini} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo1 -yo2.5 -xh2.4 -zh${tAl} -p -t > ${px}wb1$1$2$3.qui
echo "C ${px}wb1$1$2$3.qui ${sio2} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
#
${cgen} ${xp} -xo3.4 -yo2.5 -zh${tAl} -pfl -pbr -b > ${px}w3.4$1$2$3.qui
echo "C ${px}w3.4$1$2$3.qui ${sini} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo3.4 -yo2.5 -zh${tAl} -p -t > ${px}wb3.4$1$2$3.qui
echo "C ${px}wb3.4$1$2$3.qui ${sio2} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
#
${cgen} ${xp} -xo4.4 -yo2.5 -xh1.2 -zh${tAl} -pfl ${re2} -b > ${px}w4.4$1$2$3.qui
echo "C ${px}w4.4$1$2$3.qui ${sini} 0.0 0.0 1.8 +" >> ${px}$1$2$3.lst
${cgen} ${xp} -xo4.4 -yo2.5 -xh1.2 -zh${tAl} -p -t > ${px}wb4.4$1$2$3.qui
echo "C ${px}wb4.4$1$2$3.qui ${sio2} 0.0 0.0 1.8" >> ${px}$1$2$3.lst
#
###  right Al word line
echo "** right Al word line (view from +x towards xz plane)" >> ${px}$1$2$3.lst
echo "C ${px}w-4.6$1$2$3.qui ${sini} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}wb-4.6$1$2$3.qui ${sio2} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}w-3.4$1$2$3.qui ${sini} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}wb-3.4$1$2$3.qui ${sio2} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}w-2.4$1$2$3.qui ${sini} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}wb-2.4$1$2$3.qui ${sio2} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}w0$1$2$3.qui ${sini} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}wb0$1$2$3.qui ${sio2} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}w1$1$2$3.qui ${sini} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}wb1$1$2$3.qui ${sio2} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}w3.4$1$2$3.qui ${sini} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}wb3.4$1$2$3.qui ${sio2} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}w4.4$1$2$3.qui ${sini} 0.0 3.0 2.2 +" >> ${px}$1$2$3.lst
echo "C ${px}wb4.4$1$2$3.qui ${sio2} 0.0 3.0 2.2" >> ${px}$1$2$3.lst
#
#  center polycide bit line
set xp = "${op} -naBIT_LINE"
echo "** center polycide bit line" >> ${px}$1$2$3.lst
echo "G CNTR_BL" >>  ${px}$1$2$3.lst
${cgen} ${xp} -yo2.5 -zo1 -zh0.3 -pfr -pbl > ${px}b2.5$1$2$3.qui
echo "C ${px}b2.5$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst 
${pgen} ${xp} -cr 0 3.5 1.3 -c1 0 5.5 1.7 -c2 1 3.5 1.3 -c3 0 3.5 1 -pfr -pbl > ${px}b3.5$1$2$3.qui
echo "C ${px}b3.5$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${xp} -yo5.5 -zo1.4 -zh0.3 -pfr -pbl > ${px}b5.5$1$2$3.qui
echo "C ${px}b5.5$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst 
${cgen} ${xp} -yo6.5 -zo1.4 -yh2.5 -zh0.3 -pfr ${re3} > ${px}b6.5$1$2$3.qui
echo "C ${px}b6.5$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst 
echo "C ${dest}/ramcell_bitl.neu ${sio2} 0.0 0.0 0.0" >> ${px}$1$2$3.lst
#
#  left polycide bit line
echo "** left polycide bit line (view from -x, -y, +z)" >> ${px}$1$2$3.lst
echo "G LEFT_BL" >>  ${px}$1$2$3.lst
echo "C ${px}b2.5$1$2$3.qui ${sio2} -3.4 0.0 0.0 +" >> ${px}$1$2$3.lst 
echo "C ${px}b3.5$1$2$3.qui ${sio2} -3.4 0.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}b5.5$1$2$3.qui ${sio2} -3.4 0.0 0.0 +" >> ${px}$1$2$3.lst 
echo "C ${px}b6.5$1$2$3.qui ${sio2} -3.4 0.0 0.0 +" >> ${px}$1$2$3.lst 
echo "C ${dest}/ramcell_bitl.neu ${sio2} -3.4 0.0 0.0" >> ${px}$1$2$3.lst
#
#  right polycide bit line
echo "** right polycide bit line (view from -x, -y, +z)" >> ${px}$1$2$3.lst
echo "G RGHT_BL" >>  ${px}$1$2$3.lst
echo "C ${px}b2.5$1$2$3.qui ${sio2} 3.4 0.0 0.0 +" >> ${px}$1$2$3.lst 
echo "C ${px}b3.5$1$2$3.qui ${sio2} 3.4 0.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}b5.5$1$2$3.qui ${sio2} 3.4 0.0 0.0 +" >> ${px}$1$2$3.lst 
echo "C ${px}b6.5$1$2$3.qui ${sio2} 3.4 0.0 0.0 +" >> ${px}$1$2$3.lst 
echo "C ${dest}/ramcell_bitl.neu ${sio2} 3.4 0.0 0.0" >> ${px}$1$2$3.lst
#
### PATRAN FILES (also a few above for via ends of bit lines) ##############
#
echo "** top Si3Ni4/air interface" >> ${px}$1$2$3.lst
echo "D ${dest}/ramcell_topd.neu 1.0 ${sini} 0.0 0.0 0.0 2.2 4.5 -5.0 -" >> ${px}$1$2$3.lst
echo "D ${dest}/ramcell_topd.neu 1.0 ${sini} -3.4 0.0 0.0 2.2 4.5 -5.0 -" >> ${px}$1$2$3.lst
echo "D ${dest}/ramcell_topd.neu 1.0 ${sini} 3.4 0.0 0.0 2.2 4.5 -5.0 -" >> ${px}$1$2$3.lst
echo "** top Si3Ni4/SiO2 interface" >> ${px}$1$2$3.lst
echo "D ${dest}/ramcell_botd.neu ${sio2} ${sini} 0.0 0.0 0.0 2.2 4.5 5.0 -" >> ${px}$1$2$3.lst
echo "D ${dest}/ramcell_botd.neu ${sio2} ${sini} -3.4 0.0 0.0 2.2 4.5 5.0 -" >> ${px}$1$2$3.lst
echo "D ${dest}/ramcell_botd.neu ${sio2} ${sini} 3.4 0.0 0.0 2.2 4.5 5.0 -" >> ${px}$1$2$3.lst
####### END PATRAN FILES ###################################################
#
#  flat part of ground plane, nearest pits
set np = "${op} -naGND_PLANE"
echo "** flat part of ground plane, nearest pits" >> ${px}$1$2$3.lst
${cgen} ${np} -xo-4.6 -yo2.5 -xh1.2 -zh${tpol} -p -t ${re1} >${px}g-4.6$1$2$3.qui
echo "C ${px}g-4.6$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo-3.4 -yo2.5 -zh${tpol} -p -t > ${px}g-3.4$1$2$3.qui
echo "C ${px}g-3.4$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo-2.4 -yo2.5 -xh2.4 -zh${tpol} -p -t > ${px}g-2.4$1$2$3.qui
echo "C ${px}g-2.4$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -yo2.5 -zh${tpol} -p -t > ${px}g0$1$2$3.qui
echo "C ${px}g0$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo1 -yo2.5 -xh2.4 -zh${tpol} -p -t > ${px}g1$1$2$3.qui
echo "C ${px}g1$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo3.4 -yo2.5 -zh${tpol} -p -t > ${px}g3.4$1$2$3.qui
echo "C ${px}g3.4$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo4.4 -yo2.5 -xh1.2 -zh${tpol} -p -t > ${px}g4.4$1$2$3.qui
echo "C ${px}g4.4$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
#
#  flat part of ground plane, 2nd from pits
echo "** flat part of ground plane, 2nd from pits" >> ${px}$1$2$3.lst
echo "C ${px}g-4.6$1$2$3.qui ${sio2} 0.0 1.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g-3.4$1$2$3.qui ${sio2} 0.0 1.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g-2.4$1$2$3.qui ${sio2} 0.0 1.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g0$1$2$3.qui ${sio2} 0.0 1.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g1$1$2$3.qui ${sio2} 0.0 1.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g3.4$1$2$3.qui ${sio2} 0.0 1.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g4.4$1$2$3.qui ${sio2} 0.0 1.0 0.0 +" >> ${px}$1$2$3.lst
#
#  flat part of ground plane, 3rd from pits
echo "** flat part of ground plane, 3rd from pits" >> ${px}$1$2$3.lst
echo "C ${px}g-4.6$1$2$3.qui ${sio2} 0.0 2.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g-3.4$1$2$3.qui ${sio2} 0.0 2.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g-2.4$1$2$3.qui ${sio2} 0.0 2.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g0$1$2$3.qui ${sio2} 0.0 2.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g1$1$2$3.qui ${sio2} 0.0 2.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g3.4$1$2$3.qui ${sio2} 0.0 2.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g4.4$1$2$3.qui ${sio2} 0.0 2.0 0.0 +" >> ${px}$1$2$3.lst
#
#  flat part of ground plane, 4th from pits
echo "** flat part of ground plane, 4th from pits" >> ${px}$1$2$3.lst
echo "C ${px}g-4.6$1$2$3.qui ${sio2} 0.0 3.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g-3.4$1$2$3.qui ${sio2} 0.0 3.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g-2.4$1$2$3.qui ${sio2} 0.0 3.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g0$1$2$3.qui ${sio2} 0.0 3.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g1$1$2$3.qui ${sio2} 0.0 3.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g3.4$1$2$3.qui ${sio2} 0.0 3.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${px}g4.4$1$2$3.qui ${sio2} 0.0 3.0 0.0 +" >> ${px}$1$2$3.lst
#
#  flat part of ground plane, wide strip
echo "** flat part of ground plane, wide strip" >> ${px}$1$2$3.lst
${cgen} ${np} -xo-4.6 -yo6.5 -xh1.2 -yh2.5 -zh${tpol} -p -t ${re1} >${px}gw-4.6$1$2$3.qui
echo "C ${px}gw-4.6$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo-3.4 -yo6.5 -yh2.5 -zh${tpol} -p -t > ${px}gw-3.4$1$2$3.qui
echo "C ${px}gw-3.4$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo-2.4 -yo6.5 -xh2.4 -yh2.5 -zh${tpol} -p -t > ${px}gw-2.4$1$2$3.qui
echo "C ${px}gw-2.4$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -yo6.5 -yh2.5 -zh${tpol} -p -t > ${px}gw0$1$2$3.qui
echo "C ${px}gw0$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo1 -yo6.5 -xh2.4 -yh2.5 -zh${tpol} -p -t > ${px}gw1$1$2$3.qui
echo "C ${px}gw1$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo3.4 -yo6.5 -yh2.5 -zh${tpol} -p -t > ${px}gw3.4$1$2$3.qui
echo "C ${px}gw3.4$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
${cgen} ${np} -xo4.4 -yo6.5 -xh1.2 -yh2.5 -zh${tpol} -p -t > ${px}gw4.4$1$2$3.qui
echo "C ${px}gw4.4$1$2$3.qui ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
########### FINAL PATRAN FILES ###########################################
#
#  ground plane pits
echo "** ground plane pits" >> ${px}$1$2$3.lst
echo "C ${dest}/ramcell_gnd.neu ${sio2} 0.0 0.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${dest}/ramcell_gnd.neu ${sio2} 3.4 0.0 0.0 +" >> ${px}$1$2$3.lst
echo "C ${dest}/ramcell_gnd.neu ${sio2} -3.4 0.0 0.0" >> ${px}$1$2$3.lst
