#!/bin/csh
#
# script to run test problems, one generated using testgen.sh
# - problems are meant to exercise interface, not engine
#
set exam = .
set exe = ../bin
set dest = .
set fastcap = ${exe}/fastcap
set cubegen = ${exe}/cubegen
#
#   run the full test problem
echo "Running full test problem"
echo "  results to test_full.out"
${fastcap} -l${dest}/test.lst > test_full.out
#
#   run the full test problem but don't calculate gnd plane capacitances
echo "Running full test problem, no ground plane solve"
echo "  results to test_rs.out"
${fastcap} -l${dest}/test.lst -rsGND > test_rs.out
#
#   run on bus crossing between planes only (ie remove pyramid, L-shape)
echo "Running reduced test problem, no pyramid or L-shape, no gnd plane solve"
echo "  results to test_rs_ri.out"
${fastcap} -l${dest}/test.lst -rsGND -riPYRAMID,LSHAPE > test_rs_ri.out
#   alternative command, same result, illustrates stdin read, no list file
echo "  results to test_stdin_rs.out, should be identical"
${cubegen} -naGND -p -xh6 -yh5.5 -zh4 -e0 -n10 -xo-1 -yo-1 -zo-1 \
| ${fastcap} - ${dest}/bus.qui -rsGND > test_stdin_rs.out
#
#   generate a line drawing of the structures between dielectric and gnd planes
echo "Generating line drawing of everything but gnd and dielectric interfaces"
echo "  postscript to test_ri.ps, stdout to test_ld1.out"
${fastcap} -l${dest}/test.lst -riGND -rd -m -e75 > test_ld1.out
mv test.ps test_ri.ps
#   alternate command, same effect
echo "  postscript to test.ps, should be identical, stdout to test_ld2.out"
${fastcap} -l${dest}/test.lst -rcGND -rd -m -e75 > test_ld2.out
#
#   generate a plot of charge on only L-shape and pyramid after 
#     figuring charge density for bus-conductor-1-at-1volt problem
#     - full test problem is run, just that only parts are plotted
#       and only after bus-conductor-1-at-1volt problem
#     - the group specification in the -q option is not strictly required
#	since the conductor name `1' is unique in this example;
#       it is included to show the syntax
echo "Running full test problem, no gnd plane solve, plot q on L-shape, pyramid"
echo "  postscript to test2.ps, results to test_rs_q_rc_rd.out"
${fastcap} -l${dest}/test.lst -rsGND -q1%BUS -rcGND,1,2 -rd \
> test_rs_q_rc_rd.out
