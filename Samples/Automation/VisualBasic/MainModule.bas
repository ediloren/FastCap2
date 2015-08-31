Attribute VB_Name = "MainModule"
Public mainForm As Object

Public Sub Main()
    ' create and show main window
    Set mainForm = FastCapClient
    mainForm.Show
End Sub

