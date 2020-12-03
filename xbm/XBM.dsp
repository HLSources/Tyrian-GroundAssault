# Microsoft Developer Studio Project File - Name="XBM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=XBM - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XBM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XBM.mak" CFG="XBM - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XBM - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XBM - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SDKSrc/Public/dlls", NVGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XBM - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release/intermediate"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W4 /GX /Zi /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "SVDLL_NEWFUNCTIONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /version:3.0 /subsystem:windows /dll /map /debug /machine:I386
# SUBTRACT LINK32 /profile /incremental:yes
# Begin Custom Build - Copying to dlls
TargetPath=.\release\XBM.dll
TargetName=XBM
InputPath=.\release\XBM.dll
SOURCE="$(InputPath)"

"%HLROOT%\TYRIAN\dlls\$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(TargetPath)    %HLROOT%\TYRIAN\dlls\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "XBM - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\XBM___Win"
# PROP BASE Intermediate_Dir ".\XBM___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug/intermediate"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "SVDLL_NEWFUNCTIONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /version:3.0 /subsystem:windows /dll /incremental:no /map /debug /machine:I386
# SUBTRACT LINK32 /profile
# Begin Custom Build - Copying to dlls
TargetPath=.\debug\XBM.dll
TargetName=XBM
InputPath=.\debug\XBM.dll
SOURCE="$(InputPath)"

"%HLROOT%\TYRIAN\dlls\$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy    $(TargetPath)    %HLROOT%\TYRIAN\dlls\ 

# End Custom Build

!ENDIF 

# Begin Target

# Name "XBM - Win32 Release"
# Name "XBM - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\bot.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_chat.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_client.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_combat.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_commands.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_navigate.cpp
# End Source File
# Begin Source File

SOURCE=.\dll.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp
# End Source File
# Begin Source File

SOURCE=.\linkfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\namefunc.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\waypoint.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx"
# Begin Group "dlls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dlls\activity.h
# End Source File
# Begin Source File

SOURCE=..\dlls\basemonster.h
# End Source File
# Begin Source File

SOURCE=..\dlls\cbase.h
# End Source File
# Begin Source File

SOURCE=..\dlls\extdll.h
# End Source File
# Begin Source File

SOURCE=..\dlls\monsterevent.h
# End Source File
# Begin Source File

SOURCE=..\dlls\saverestore.h
# End Source File
# Begin Source File

SOURCE=..\dlls\schedule.h
# End Source File
# Begin Source File

SOURCE=..\dlls\vector.h
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\cdll_dll.h
# End Source File
# Begin Source File

SOURCE=..\common\const.h
# End Source File
# Begin Source File

SOURCE=..\common\crc.h
# End Source File
# Begin Source File

SOURCE=..\common\cvardef.h
# End Source File
# Begin Source File

SOURCE=..\common\damage.h
# End Source File
# Begin Source File

SOURCE=..\common\entity_state.h
# End Source File
# Begin Source File

SOURCE=..\common\event_flags.h
# End Source File
# Begin Source File

SOURCE=..\common\gamedefs.h
# End Source File
# Begin Source File

SOURCE=..\common\in_buttons.h
# End Source File
# Begin Source File

SOURCE=..\common\platform.h
# End Source File
# Begin Source File

SOURCE=..\dlls\sound.h
# End Source File
# Begin Source File

SOURCE=..\common\studio_event.h
# End Source File
# Begin Source File

SOURCE=..\common\usercmd.h
# End Source File
# Begin Source File

SOURCE=..\common\vector.h
# End Source File
# Begin Source File

SOURCE=..\common\weapondef.h
# End Source File
# Begin Source File

SOURCE=..\common\weaponinfo.h
# End Source File
# End Group
# Begin Group "engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\engine\archtypes.h
# End Source File
# Begin Source File

SOURCE=..\engine\custom.h
# End Source File
# Begin Source File

SOURCE=..\engine\edict.h
# End Source File
# Begin Source File

SOURCE=..\engine\eiface.h
# End Source File
# Begin Source File

SOURCE=..\engine\progdefs.h
# End Source File
# Begin Source File

SOURCE=..\engine\Sequence.h
# End Source File
# Begin Source File

SOURCE=..\engine\studio.h
# End Source File
# End Group
# Begin Group "pm_shared"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\pm_shared\pm_info.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_materials.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_shared.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\common\basetsd.h
# End Source File
# Begin Source File

SOURCE=.\bot.h
# End Source File
# Begin Source File

SOURCE=.\bot_client.h
# End Source File
# Begin Source File

SOURCE=.\bot_cvar.h
# End Source File
# Begin Source File

SOURCE=.\bot_func.h
# End Source File
# Begin Source File

SOURCE=.\bot_weapons.h
# End Source File
# Begin Source File

SOURCE=.\engine.h
# End Source File
# Begin Source File

SOURCE=.\enginecallback.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\waypoint.h
# End Source File
# End Group
# Begin Group "Other Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XBM.def
# End Source File
# End Group
# End Target
# End Project
