' FastCap2 Automation example
' Enrico Di Lorenzo, 2004/04/15
'
' Launch from a DOS shell, typing 'cscript <filename>',
' where <filename> is the name of this file
' (can also double click on the file, but you only get
' message boxes in this case)

Dim FastCap2
' Create FastCap2 object
Set FastCap2 = CreateObject("FastCap2.Document")

' Extract script path from ScriptFullName Property
pathPos = InstrRev(Wscript.ScriptFullName, Wscript.ScriptName)
path = left(Wscript.ScriptFullName, pathPos-1)

For  i= 0 to 3
  ' Input files are already existing in this simple example;
  ' could also generate them on the fly and stop loop when
  ' desired refinement accuracy is reached (e.g. <1% difference
  ' between FastCap2 results) or optimization result is achieved
  
  ' Try to run FastCap2
  ' Remark: the run path must be surrounded by quotas '"' to support
  ' also paths containing spaces (quotas in VisualBasic are escaped by
  ' doubling the symbol, i.e., "" )
  couldRun = FastCap2.Run("""" + path + "sphere"+CStr(i)+".qui""")
  ' Wait for end of operation, using polling; could also use callback function,
  ' see Windows Script Components help
  Do While FastCap2.IsRunning = True
    Wscript.Sleep 500
  Loop
  ' retrieve capacitance matrix
  capacitance = FastCap2.GetCapacitance()
  WScript.Echo "sphere"+CStr(i)+" capacitance is " + CStr(capacitance(0, 0))
Next

' Quit FastCap2
FastCap2.Quit
' Destroy FastCap2 object
Set FastCap2 = Nothing
