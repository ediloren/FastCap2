'  Unix shell script converted to batch file for MSDOS
'  by Enrico Di Lorenzo
'    
' Launch from a DOS shell, typing 'cscript <filename>',
' where <filename> is the name of this file
' (can also double click on the file, but you only get
' message boxes in this case)
'
'  Original Unix comment:
'
' script to run test problems, one generated using testgen.sh
' - problems are meant to exercise interface, not engine

Dim FastCap2
' Create FastCap2 object
Set FastCap2 = CreateObject("FastCap2.Document")

' Extract script path from ScriptFullName Property
pathPos = InstrRev(Wscript.ScriptFullName, Wscript.ScriptName)
path = left(Wscript.ScriptFullName, pathPos-1)

' Create shell object to launch scripts 
Set wshShell = WScript.CreateObject("WScript.Shell")

'
' Generate the test geometry 'testgen' with the testgen.bat batch file
'
wshShell.Run "testgen.bat", , True 

'
' Run the full test problem
'
WScript.Echo "Running full test problem"
RunFastCap "-l" + path + "testgen.lst"
WScript.Echo

'
' Run the full test problem but don't calculate gnd plane capacitances
'
WScript.Echo "Running full test problem, no ground plane solve"
RunFastCap "-l" + path + "testgen.lst -rsGND"
WScript.Echo

'
' Run on bus crossing between planes only (ie remove pyramid, L-shape)
'
WScript.Echo "Running reduced test problem, no pyramid or L-shape, no gnd plane solve"
RunFastCap "-l" + path + "testgen.lst -rsGND -riPYRAMID,LSHAPE"
WScript.Echo

'
' Run on bus crossing, generates input file on the spot, no list file
'
WScript.Echo "Running only bus"
' must use %comspec% /c to launch a console and pass it the arguments,
' otherwise 'Run' can run cubegen but not redirect the output 
' (does not interpret the '>' which is simply passed as argument)
wshShell.Run "%comspec% /c busgen -c1 -xo2 -yo1 > testrun.qui", , True 
RunFastCap path + "testrun.qui"
WScript.Echo

'
' Generate a PostScript line drawing of the structures 
' between dielectric and gnd planes; use GhostView to see
' the file (or any other graphic manipulation program which
' supports PostScript images)
'
WScript.Echo "Generating line drawing of everything but gnd and dielectric interfaces"
WScript.Echo "Postscript file is testgen_1.ps"
RunFastCap "-l" + path + "testgen.lst -riGND -rd -m -e75"
' rename file
wshShell.Run "%comspec% /c rename testgen.ps testgen_1.ps"
WScript.Echo

'
'   Alternate command, same effect
'
WScript.Echo "Image should be identical to previous one"
WScript.Echo "Postscript file is testgen_2.ps"
RunFastCap "-l" + path + "testgen.lst -rcGND -rd -m -e75"
' rename file
wshShell.Run "%comspec% /c rename testgen.ps testgen_2.ps"
WScript.Echo

'
' Generate a plot of charge on only L-shape and pyramid after 
' figuring charge density for bus-conductor-1-at-1volt problem
' - full test problem is run, just that only parts are plotted
'   and only after bus-conductor-1-at-1volt problem
' - the group specification in the -q option is not strictly required
'   since the conductor name `1' is unique in this example;
'   it is included to show the syntax
'
WScript.Echo "Generating charge drawing"
WScript.Echo "Running full test problem, no gnd plane solve, plot q on L-shape, pyramid"
RunFastCap "-l" + path + "testgen.lst -rsGND -q1%BUS -rcGND,1,2 -rd"

' Quit FastCap2
FastCap2.Quit
' Destroy FastCap2 object
Set FastCap2 = Nothing


' Subroutine used to launch FastCap2, retrieve results
' and print them on the screen
Sub RunFastCap(launchString)

  ' Try to run FastCap2
  couldRun = FastCap2.Run(launchString)
  ' Wait for end of operation, using polling; could also use callback function,
  ' see Windows Script Components help
  Do While FastCap2.IsRunning = True
    Wscript.Sleep 500
  Loop

' retrieve capacitance matrix
  capacitance = FastCap2.GetCapacitance()
  ' retrieve conductor names
  condNames = FastCap2.GetCondNames()
  ' get matrix dimension
  mtxDim = UBound(capacitance, 1)

  ' output results
  For i = 0 To mtxDim
    resultLine = condNames(i) + " "
    For j = 0 To mtxDim
        resultLine = resultLine + FormatNumber(capacitance(i, j)*1E12,2) + "E-12 "
    Next
    WScript.Echo resultLine
  Next
  
End Sub

