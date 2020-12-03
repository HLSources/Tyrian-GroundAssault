# Microsoft Developer Studio Generated NMAKE File, Based on XBM.dsp
!IF "$(CFG)" == ""
CFG=XBM - Win32 Release
!MESSAGE No configuration specified. Defaulting to XBM - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "XBM - Win32 Release" && "$(CFG)" != "XBM - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XBM - Win32 Release"

OUTDIR=.\release
INTDIR=.\release/intermediate
# Begin Custom Macros
OutDir=.\release
# End Custom Macros

ALL : "$(OUTDIR)\XBM.dll" "$(OUTDIR)\XBM.bsc" ".\XBM"


CLEAN :
	-@erase "$(INTDIR)\bot.obj"
	-@erase "$(INTDIR)\bot.sbr"
	-@erase "$(INTDIR)\bot_chat.obj"
	-@erase "$(INTDIR)\bot_chat.sbr"
	-@erase "$(INTDIR)\bot_client.obj"
	-@erase "$(INTDIR)\bot_client.sbr"
	-@erase "$(INTDIR)\bot_combat.obj"
	-@erase "$(INTDIR)\bot_combat.sbr"
	-@erase "$(INTDIR)\bot_commands.obj"
	-@erase "$(INTDIR)\bot_commands.sbr"
	-@erase "$(INTDIR)\bot_navigate.obj"
	-@erase "$(INTDIR)\bot_navigate.sbr"
	-@erase "$(INTDIR)\dll.obj"
	-@erase "$(INTDIR)\dll.sbr"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\engine.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\linkfunc.obj"
	-@erase "$(INTDIR)\linkfunc.sbr"
	-@erase "$(INTDIR)\namefunc.obj"
	-@erase "$(INTDIR)\namefunc.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\waypoint.obj"
	-@erase "$(INTDIR)\waypoint.sbr"
	-@erase "$(OUTDIR)\intermediate\XBM.map"
	-@erase "$(OUTDIR)\XBM.bsc"
	-@erase "$(OUTDIR)\XBM.dll"
	-@erase "$(OUTDIR)\XBM.exp"
	-@erase "$(OUTDIR)\XBM.ilk"
	-@erase "$(OUTDIR)\XBM.lib"
	-@erase "XBM"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\XBM.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\XBM.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\bot.sbr" \
	"$(INTDIR)\bot_chat.sbr" \
	"$(INTDIR)\bot_client.sbr" \
	"$(INTDIR)\bot_combat.sbr" \
	"$(INTDIR)\bot_commands.sbr" \
	"$(INTDIR)\bot_navigate.sbr" \
	"$(INTDIR)\dll.sbr" \
	"$(INTDIR)\engine.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\linkfunc.sbr" \
	"$(INTDIR)\namefunc.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\waypoint.sbr"

"$(OUTDIR)\XBM.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /version:3.0 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\XBM.pdb" /map:"$(INTDIR)\XBM.map" /machine:I386 /def:".\XBM.def" /out:"$(OUTDIR)\XBM.dll" /implib:"$(OUTDIR)\XBM.lib" 
DEF_FILE= \
	".\XBM.def"
LINK32_OBJS= \
	"$(INTDIR)\bot.obj" \
	"$(INTDIR)\bot_chat.obj" \
	"$(INTDIR)\bot_client.obj" \
	"$(INTDIR)\bot_combat.obj" \
	"$(INTDIR)\bot_commands.obj" \
	"$(INTDIR)\bot_navigate.obj" \
	"$(INTDIR)\dll.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\linkfunc.obj" \
	"$(INTDIR)\namefunc.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\waypoint.obj"

"$(OUTDIR)\XBM.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\release\XBM.dll
TargetName=XBM
InputPath=.\release\XBM.dll
SOURCE="$(InputPath)"

".\XBM" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy        $(TargetPath)        \Half-Life\XDM\dlls\  
	copy        $(TargetPath)        F:\games\HL1121\XDM\dlls\  
	copy $(TargetPath) W:\dev\Half-Life\XDM\dlls\
<< 
	

!ELSEIF  "$(CFG)" == "XBM - Win32 Debug"

OUTDIR=.\debug
INTDIR=.\debug/intermediate
# Begin Custom Macros
OutDir=.\debug
# End Custom Macros

ALL : "$(OUTDIR)\XBM.dll" "$(OUTDIR)\XBM.bsc" ".\XBM"


CLEAN :
	-@erase "$(INTDIR)\bot.obj"
	-@erase "$(INTDIR)\bot.sbr"
	-@erase "$(INTDIR)\bot_chat.obj"
	-@erase "$(INTDIR)\bot_chat.sbr"
	-@erase "$(INTDIR)\bot_client.obj"
	-@erase "$(INTDIR)\bot_client.sbr"
	-@erase "$(INTDIR)\bot_combat.obj"
	-@erase "$(INTDIR)\bot_combat.sbr"
	-@erase "$(INTDIR)\bot_commands.obj"
	-@erase "$(INTDIR)\bot_commands.sbr"
	-@erase "$(INTDIR)\bot_navigate.obj"
	-@erase "$(INTDIR)\bot_navigate.sbr"
	-@erase "$(INTDIR)\dll.obj"
	-@erase "$(INTDIR)\dll.sbr"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\engine.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\linkfunc.obj"
	-@erase "$(INTDIR)\linkfunc.sbr"
	-@erase "$(INTDIR)\namefunc.obj"
	-@erase "$(INTDIR)\namefunc.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\waypoint.obj"
	-@erase "$(INTDIR)\waypoint.sbr"
	-@erase "$(OUTDIR)\intermediate\XBM.map"
	-@erase "$(OUTDIR)\XBM.bsc"
	-@erase "$(OUTDIR)\XBM.dll"
	-@erase "$(OUTDIR)\XBM.exp"
	-@erase "$(OUTDIR)\XBM.pdb"
	-@erase "XBM"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\XBM.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\XBM.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\bot.sbr" \
	"$(INTDIR)\bot_chat.sbr" \
	"$(INTDIR)\bot_client.sbr" \
	"$(INTDIR)\bot_combat.sbr" \
	"$(INTDIR)\bot_commands.sbr" \
	"$(INTDIR)\bot_navigate.sbr" \
	"$(INTDIR)\dll.sbr" \
	"$(INTDIR)\engine.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\linkfunc.sbr" \
	"$(INTDIR)\namefunc.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\waypoint.sbr"

"$(OUTDIR)\XBM.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /version:3.0 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\XBM.pdb" /map:"$(INTDIR)\XBM.map" /debug /machine:I386 /def:".\XBM.def" /out:"$(OUTDIR)\XBM.dll" /implib:"$(OUTDIR)\XBM.lib" 
DEF_FILE= \
	".\XBM.def"
LINK32_OBJS= \
	"$(INTDIR)\bot.obj" \
	"$(INTDIR)\bot_chat.obj" \
	"$(INTDIR)\bot_client.obj" \
	"$(INTDIR)\bot_combat.obj" \
	"$(INTDIR)\bot_commands.obj" \
	"$(INTDIR)\bot_navigate.obj" \
	"$(INTDIR)\dll.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\linkfunc.obj" \
	"$(INTDIR)\namefunc.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\waypoint.obj"

"$(OUTDIR)\XBM.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\debug\XBM.dll
TargetName=XBM
InputPath=.\debug\XBM.dll
SOURCE="$(InputPath)"

".\XBM" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy        $(TargetPath)        \Half-Life\XDM\dlls\  
	copy        $(TargetPath)        F:\games\HL1121\XDM\dlls\  
	copy $(TargetPath) W:\dev\Half-Life\XDM\dlls\
<< 
	

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("XBM.dep")
!INCLUDE "XBM.dep"
!ELSE 
!MESSAGE Warning: cannot find "XBM.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "XBM - Win32 Release" || "$(CFG)" == "XBM - Win32 Debug"
SOURCE=.\bot.cpp

"$(INTDIR)\bot.obj"	"$(INTDIR)\bot.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bot_chat.cpp

"$(INTDIR)\bot_chat.obj"	"$(INTDIR)\bot_chat.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bot_client.cpp

"$(INTDIR)\bot_client.obj"	"$(INTDIR)\bot_client.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bot_combat.cpp

"$(INTDIR)\bot_combat.obj"	"$(INTDIR)\bot_combat.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bot_commands.cpp

"$(INTDIR)\bot_commands.obj"	"$(INTDIR)\bot_commands.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bot_navigate.cpp

"$(INTDIR)\bot_navigate.obj"	"$(INTDIR)\bot_navigate.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dll.cpp

"$(INTDIR)\dll.obj"	"$(INTDIR)\dll.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\engine.cpp

"$(INTDIR)\engine.obj"	"$(INTDIR)\engine.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\h_export.cpp

"$(INTDIR)\h_export.obj"	"$(INTDIR)\h_export.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\linkfunc.cpp

"$(INTDIR)\linkfunc.obj"	"$(INTDIR)\linkfunc.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\namefunc.cpp

"$(INTDIR)\namefunc.obj"	"$(INTDIR)\namefunc.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\util.cpp

"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\waypoint.cpp

"$(INTDIR)\waypoint.obj"	"$(INTDIR)\waypoint.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

