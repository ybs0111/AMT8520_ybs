# Microsoft Developer Studio Project File - Name="ManagerSocket" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ManagerSocket - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ManagerSocket.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ManagerSocket.mak" CFG="ManagerSocket - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ManagerSocket - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ManagerSocket - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ManagerSocket - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 KeyBoard.lib libmysql.lib Spr32d70.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "ManagerSocket - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 KeyBoard.lib libmysql.lib Spr32d70.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ManagerSocket - Win32 Release"
# Name "ManagerSocket - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Ctrl\Alg_Mysql.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\BCMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogAdd.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\GradientStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCell.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellDateTime.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellNumeric.cpp
# End Source File
# Begin Source File

SOURCE=.\GridControlAlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ManagerSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\ManagerSocket.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\TitleTip.cpp
# End Source File
# Begin Source File

SOURCE=.\Ctrl\XPGroupBox.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Ctrl\Alg_Mysql.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\BCMenu.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\DialogAdd.h
# End Source File
# Begin Source File

SOURCE=.\DialogManager.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\GradientStatic.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCell.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellBase.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellCheck.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellCombo.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellDateTime.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCellNumeric.h
# End Source File
# Begin Source File

SOURCE=.\GridControlAlg.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\GridDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\ManagerSocket.def
# End Source File
# Begin Source File

SOURCE=.\ManagerSocket.h
# End Source File
# Begin Source File

SOURCE=.\mysql.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\Grid\TitleTip.h
# End Source File
# Begin Source File

SOURCE=.\SOCKET\Variable.h
# End Source File
# Begin Source File

SOURCE=.\Ctrl\XPGroupBox.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ManagerSocket.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
