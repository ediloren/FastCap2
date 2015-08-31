# Microsoft Developer Studio Project File - Name="FastCap2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FastCap2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FastCap2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FastCap2.mak" CFG="FastCap2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FastCap2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FastCap2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FastCap2 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "FastCap2.midl.log" /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Htmlhelp.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "FastCap2 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "FastCap2.midl.log" /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Htmlhelp.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FastCap2 - Win32 Release"
# Name "FastCap2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\FastCap2.cpp
# End Source File
# Begin Source File

SOURCE=.\FastCap2.odl
# End Source File
# Begin Source File

SOURCE=.\FastCap2.rc
# End Source File
# Begin Source File

SOURCE=.\FastCapDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\FastCapView.cpp
# End Source File
# Begin Source File

SOURCE=.\FastCapWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\RunDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\UnicodeString.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\FastCap2.h
# End Source File
# Begin Source File

SOURCE=.\FastCapDoc.h
# End Source File
# Begin Source File

SOURCE=.\FastCapView.h
# End Source File
# Begin Source File

SOURCE=.\FastCapWindow.h
# End Source File
# Begin Source File

SOURCE=.\FCStructs.h
# End Source File
# Begin Source File

SOURCE=.\FCWindow.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RunDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UnicodeString.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\FastCap2.ico
# End Source File
# Begin Source File

SOURCE=.\res\FastCap2.rc2
# End Source File
# Begin Source File

SOURCE=.\res\FastCap2Doc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "FastCap"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fastcap\blkDirect.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\calcp.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\capsolve.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\direct.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\electric.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\fastcap.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\input.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\mulDisplay.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\mulDo.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\mulGlobal.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\mulGlobal.h
# End Source File
# Begin Source File

SOURCE=.\fastcap\mulLocal.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\mulMats.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\mulMulti.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\mulSetup.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\mulStruct.h
# End Source File
# Begin Source File

SOURCE=.\fastcap\patran.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\patran.h
# End Source File
# Begin Source File

SOURCE=.\fastcap\psMatDisplay.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\quickif.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\quickif.h
# End Source File
# Begin Source File

SOURCE=.\fastcap\resusage.h
# End Source File
# Begin Source File

SOURCE=.\fastcap\savemat_mod.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\uglyalloc.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\zbuf2fastcap.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\zbufGlobal.h
# End Source File
# Begin Source File

SOURCE=.\fastcap\zbufInOut.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\zbufProj.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\zbufSort.c
# End Source File
# Begin Source File

SOURCE=.\fastcap\zbufStruct.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\hlp\fastcap2.hhp

!IF  "$(CFG)" == "FastCap2 - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Release
InputPath=.\hlp\fastcap2.hhp
InputName=fastcap2

"$(OutDir)\$(InputName).chm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hhc.exe .\hlp\$(InputName).hhp 
	if exist "hlp\$(InputName).chm" copy "hlp\$(InputName).chm" $(OutDir) 
	echo off 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FastCap2 - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\hlp\fastcap2.hhp
InputName=fastcap2

"$(OutDir)\$(InputName).chm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hhc.exe .\hlp\$(InputName).hhp 
	if exist "hlp\$(InputName).chm" copy "hlp\$(InputName).chm" $(OutDir) 
	echo off 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FastCap2.reg
# End Source File
# Begin Source File

SOURCE=.\HISTORY\History.txt
# End Source File
# Begin Source File

SOURCE=.\HISTORY\History_detailed.txt
# End Source File
# Begin Source File

SOURCE=.\License_and_history\license.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
