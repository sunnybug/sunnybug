# Microsoft Developer Studio Project File - Name="Common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Common - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak" CFG="Common - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Common - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../tmp/rel/Common"
# PROP Intermediate_Dir "../tmp/rel/Common"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./BaseSupport" /I "../ANALYST" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /YX"mystdafx.h" /FD /EHa /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/rel/Common.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=time /T
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../tmp/debug/Common"
# PROP Intermediate_Dir "../tmp/debug/Common"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./BaseSupport" /I "../ANALYST" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /D "XUSHW_DEBUG" /YX"mystdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"release/Common.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/debug/Common_d.lib"

!ENDIF 

# Begin Target

# Name "Common - Win32 Release"
# Name "Common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Analyst_Common.h
# End Source File
# Begin Source File

SOURCE=.\AutoProxy.h
# End Source File
# Begin Source File

SOURCE=.\AutoPtrC.h
# End Source File
# Begin Source File

SOURCE=.\AutoPtrF.h
# End Source File
# Begin Source File

SOURCE=.\BaseFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\common.cpp
# End Source File
# Begin Source File

SOURCE=.\Console.cpp
# End Source File
# Begin Source File

SOURCE=.\DbgFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\DbgFunc.h
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=.\EventPack.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\list.cpp
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\logfile.cpp
# End Source File
# Begin Source File

SOURCE=.\LoopCount.cpp
# End Source File
# Begin Source File

SOURCE=.\MyClassFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\MyClassFactory.h
# End Source File
# Begin Source File

SOURCE=.\MyCriticalSection.cpp
# End Source File
# Begin Source File

SOURCE=.\MyCriticalSection.h
# End Source File
# Begin Source File

SOURCE=.\MyHeap.cpp
# End Source File
# Begin Source File

SOURCE=.\MyRand.cpp
# End Source File
# Begin Source File

SOURCE=.\MyStdAfx.cpp
# ADD CPP /Yc"mystdafx.h"
# End Source File
# Begin Source File

SOURCE=.\NetStringPacker.cpp
# End Source File
# Begin Source File

SOURCE=.\SafeLink.cpp
# End Source File
# Begin Source File

SOURCE=.\SafeLink.h
# End Source File
# Begin Source File

SOURCE=.\SafeLinkMap.h
# End Source File
# Begin Source File

SOURCE=.\SafeLinkSet.h
# End Source File
# Begin Source File

SOURCE=.\SgiAlloc.cpp
# End Source File
# Begin Source File

SOURCE=.\SgiAlloc.h
# End Source File
# Begin Source File

SOURCE=.\slab.cpp
# End Source File
# Begin Source File

SOURCE=.\slab.h
# End Source File
# Begin Source File

SOURCE=.\ThreadBase.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeOut.cpp
# End Source File
# Begin Source File

SOURCE=.\VarType.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AgileMsg.h
# End Source File
# Begin Source File

SOURCE=.\Array.h
# End Source File
# Begin Source File

SOURCE=.\AutoIter.h
# End Source File
# Begin Source File

SOURCE=.\AutoLink.h
# End Source File
# Begin Source File

SOURCE=.\AutoLinkSet.h
# End Source File
# Begin Source File

SOURCE=.\AutoPtr.h
# End Source File
# Begin Source File

SOURCE=.\BaseFunc.h
# End Source File
# Begin Source File

SOURCE=.\basetype.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\Common_AllFiles.h
# End Source File
# Begin Source File

SOURCE=.\Console.h
# End Source File
# Begin Source File

SOURCE=.\CritSect.h
# End Source File
# Begin Source File

SOURCE=.\EventPack.h
# End Source File
# Begin Source File

SOURCE=.\GameObjArray.h
# End Source File
# Begin Source File

SOURCE=.\GameObjSet.h
# End Source File
# Begin Source File

SOURCE=.\GameObjSetEx.h
# End Source File
# Begin Source File

SOURCE=.\GameTimer.h
# End Source File
# Begin Source File

SOURCE=.\Index.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\logfile.h
# End Source File
# Begin Source File

SOURCE=.\LoopCount.h
# End Source File
# Begin Source File

SOURCE=.\ModDump.h
# End Source File
# Begin Source File

SOURCE=.\MsgBuf.h
# End Source File
# Begin Source File

SOURCE=.\MyArray.h
# End Source File
# Begin Source File

SOURCE=.\mycom.h
# End Source File
# Begin Source File

SOURCE=.\MyHeap.h
# End Source File
# Begin Source File

SOURCE=.\MyRand.h
# End Source File
# Begin Source File

SOURCE=.\MyStack.h
# End Source File
# Begin Source File

SOURCE=.\MyStdAfx.h
# End Source File
# Begin Source File

SOURCE=.\NetStringPacker.h
# End Source File
# Begin Source File

SOURCE=.\QiPtr.h
# End Source File
# Begin Source File

SOURCE=.\Status.h
# End Source File
# Begin Source File

SOURCE=.\T_MyQueue.h
# End Source File
# Begin Source File

SOURCE=.\T_SafePtr.h
# End Source File
# Begin Source File

SOURCE=.\T_SingleMap.h
# End Source File
# Begin Source File

SOURCE=.\ThreadBase.h
# End Source File
# Begin Source File

SOURCE=.\TickOver.h
# End Source File
# Begin Source File

SOURCE=.\TimeOut.h
# End Source File
# Begin Source File

SOURCE=.\VarType.h
# End Source File
# End Group
# End Target
# End Project
