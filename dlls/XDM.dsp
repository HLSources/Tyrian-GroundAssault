# Microsoft Developer Studio Project File - Name="XDM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=XDM - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XDM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XDM.mak" CFG="XDM - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XDM - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XDM - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SDK/SourceCode/dlls", NVGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XDM - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\XDM___Win"
# PROP BASE Intermediate_Dir ".\XDM___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug/intermediate"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "." /I "bot" /I "gamerules" /I "projectiles" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\game_shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "SVDLL_NEWFUNCTIONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i ".\res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 /nologo /version:3.0 /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"debug/TYRIAN_i686.dll"
# SUBTRACT LINK32 /verbose /pdb:none
# Begin Custom Build - Copying to dlls
TargetPath=.\debug\TYRIAN_i686.dll
TargetName=TYRIAN_i686
InputPath=.\debug\TYRIAN_i686.dll
SOURCE="$(InputPath)"

"%HLROOT%\TYRIAN\dlls\$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                                                                                                    $(TargetPath)                                                                                                    %HLROOT%\TYRIAN\dlls\ 

# End Custom Build

!ELSEIF  "$(CFG)" == "XDM - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "XDM___Win32_Release"
# PROP BASE Intermediate_Dir "XDM___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release/intermediate"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\engine" /I "..\common" /I "..\pm_shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR /YX /FD /c
# ADD CPP /nologo /G6 /MD /W4 /GX /Zi /I "." /I "bot" /I "gamerules" /I "projectiles" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\game_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "SVDLL_NEWFUNCTIONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# SUBTRACT MTL /Oicf
# ADD BASE RSC /l 0x409 /i ".\res" /d "_DEBUG"
# ADD RSC /l 0x409 /i ".\res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /version:3.0 /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"release/TYRIAN_i686.dll"
# SUBTRACT LINK32 /verbose /profile /nodefaultlib
# Begin Custom Build - Copying to dlls
TargetPath=.\release\TYRIAN_i686.dll
TargetName=TYRIAN_i686
InputPath=.\release\TYRIAN_i686.dll
SOURCE="$(InputPath)"

"%HLROOT%\TYRIAN\dlls\$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	signtool  sign /a "$(TargetPath)" 
	copy                                                                                                    $(TargetPath)                                                                                                    %HLROOT%\TYRIAN\dlls\  
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "XDM - Win32 Debug"
# Name "XDM - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx"
# Begin Group "physics"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=..\pm_shared\pm_debug.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_math.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_shared.cpp
# End Source File
# Begin Source File

SOURCE=..\common\shared_resources.cpp
# End Source File
# End Group
# Begin Group "weapons"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\weapons\wpn_tyriangun.cpp
# End Source File
# End Group
# Begin Group "monsters"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\ai_basenpc_schedule.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\flyingmonster.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\monsters.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\monsterstate.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\playermonster.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\squadmonster.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\talkmonster.cpp
# End Source File
# Begin Source File

SOURCE=.\monsters\turret.cpp
# End Source File
# End Group
# Begin Group "items"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\items.cpp
# End Source File
# End Group
# Begin Group "gamerules"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\gamerules\coop_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\ctf_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\dom_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\lms_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\maprules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\multiplay_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\round_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\singleplay_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\gamerules\teamplay_gamerules.cpp
# End Source File
# End Group
# Begin Group "entities"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\animating.cpp
# End Source File
# Begin Source File

SOURCE=.\bmodels.cpp
# End Source File
# Begin Source File

SOURCE=.\buttons.cpp
# End Source File
# Begin Source File

SOURCE=.\chasecam.cpp
# End Source File
# Begin Source File

SOURCE=.\doors.cpp
# End Source File
# Begin Source File

SOURCE=.\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\effects_new.cpp
# End Source File
# Begin Source File

SOURCE=.\ent_alias.cpp
# End Source File
# Begin Source File

SOURCE=.\ent_functional.cpp
# End Source File
# Begin Source File

SOURCE=.\ent_locus.cpp
# End Source File
# Begin Source File

SOURCE=.\environment.cpp
# End Source File
# Begin Source File

SOURCE=.\explode.cpp
# End Source File
# Begin Source File

SOURCE=.\func_break.cpp
# End Source File
# Begin Source File

SOURCE=.\func_tank.cpp
# End Source File
# Begin Source File

SOURCE=.\fx.cpp
# End Source File
# Begin Source File

SOURCE=.\gibs.cpp
# End Source File
# Begin Source File

SOURCE=.\h_cycler.cpp
# End Source File
# Begin Source File

SOURCE=.\lights.cpp
# End Source File
# Begin Source File

SOURCE=.\monstermaker.cpp
# End Source File
# Begin Source File

SOURCE=.\mortar.cpp
# End Source File
# Begin Source File

SOURCE=.\pathcorner.cpp
# End Source File
# Begin Source File

SOURCE=.\plats.cpp
# End Source File
# Begin Source File

SOURCE=.\scripted.cpp
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\soundent.cpp
# End Source File
# Begin Source File

SOURCE=.\triggers.cpp
# End Source File
# Begin Source File

SOURCE=.\triggers_new.cpp
# End Source File
# Begin Source File

SOURCE=.\xen.cpp
# End Source File
# End Group
# Begin Group "projectiles"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\projectiles\fx_frozen_cube.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\fx_lightning_field.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\fx_ring_teleport.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\ggrenade.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_30mmgren.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_acidblob.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_airstriketarget.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_antimatherialmissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_atombomb.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_banana.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_biohazardmissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_chargecannon.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_demolitionmissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_disruptorball.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_flashbang.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_frostball.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_ghostmissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_gluonball.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_guidedbomb.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_hellfire.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_hellhounder.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_hvrmissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_lightningball.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_m203gren.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_meteor.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_micromissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_minimissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_multicannon.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_needlelaser.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_nuclearmissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_plasma_shield_charge.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_plasmaball.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_plasmastorm.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_protonmissile.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_pulse.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_ripper.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_scorcher.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_shocklaser.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_shockwave.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_sonicwave.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_spidermine.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_starburst.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_sunofgod.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_teleporter.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_teleporttarget.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_toilet.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_trident.cpp
# End Source File
# Begin Source File

SOURCE=.\projectiles\proj_tripmine.cpp
# End Source File
# End Group
# Begin Group "game shared code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=..\game_shared\voice_banmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_gamemgr.cpp
# End Source File
# End Group
# Begin Group "common code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\airtank.cpp
# End Source File
# Begin Source File

SOURCE=..\common\materials.cpp
# End Source File
# Begin Source File

SOURCE=..\common\util_vector.cpp
# End Source File
# Begin Source File

SOURCE=..\common\vector.cpp
# End Source File
# Begin Source File

SOURCE=.\weaponbox.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ammo.cpp
# End Source File
# Begin Source File

SOURCE=.\animation.cpp
# End Source File
# Begin Source File

SOURCE=.\cbase.cpp
# End Source File
# Begin Source File

SOURCE=.\client.cpp
# End Source File
# Begin Source File

SOURCE=.\combat.cpp
# End Source File
# Begin Source File

SOURCE=.\defaultai.cpp
# End Source File
# Begin Source File

SOURCE=.\entconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\h_ai.cpp
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp
# End Source File
# Begin Source File

SOURCE=.\mapcycle.cpp
# End Source File
# Begin Source File

SOURCE=.\movewith.cpp
# End Source File
# Begin Source File

SOURCE=.\msg_fx.cpp
# End Source File
# Begin Source File

SOURCE=.\nodes.cpp
# End Source File
# Begin Source File

SOURCE=.\observer.cpp
# End Source File
# Begin Source File

SOURCE=.\plane.cpp
# End Source File
# Begin Source File

SOURCE=.\player.cpp
# End Source File
# Begin Source File

SOURCE=.\saverestore.cpp
# End Source File
# Begin Source File

SOURCE=.\skill.cpp
# End Source File
# Begin Source File

SOURCE=.\spectator.cpp
# End Source File
# Begin Source File

SOURCE=.\subs.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\util_xhl.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\world.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx"
# Begin Group "physics headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\pm_shared\pm_debug.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_defs.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_info.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_materials.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_movevars.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_shared.h
# End Source File
# End Group
# Begin Group "common headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\engine\archtypes.h
# End Source File
# Begin Source File

SOURCE=..\common\beamdef.h
# End Source File
# Begin Source File

SOURCE=..\common\bspfile.h
# End Source File
# Begin Source File

SOURCE=..\common\cdll_dll.h
# End Source File
# Begin Source File

SOURCE=..\common\colors.h
# End Source File
# Begin Source File

SOURCE=..\common\com_model.h
# End Source File
# Begin Source File

SOURCE=..\common\const.h
# End Source File
# Begin Source File

SOURCE=..\common\crc.h
# End Source File
# Begin Source File

SOURCE=..\engine\custom.h
# End Source File
# Begin Source File

SOURCE=..\engine\customentity.h
# End Source File
# Begin Source File

SOURCE=..\common\cvardef.h
# End Source File
# Begin Source File

SOURCE=..\common\damage.h
# End Source File
# Begin Source File

SOURCE=..\common\decals.h
# End Source File
# Begin Source File

SOURCE=..\common\demo_api.h
# End Source File
# Begin Source File

SOURCE=..\common\dlight.h
# End Source File
# Begin Source File

SOURCE=..\common\entity_state.h
# End Source File
# Begin Source File

SOURCE=..\common\entity_types.h
# End Source File
# Begin Source File

SOURCE=..\common\event_api.h
# End Source File
# Begin Source File

SOURCE=..\common\event_args.h
# End Source File
# Begin Source File

SOURCE=..\common\event_flags.h
# End Source File
# Begin Source File

SOURCE=..\common\gamedefs.h
# End Source File
# Begin Source File

SOURCE=..\common\hltv.h
# End Source File
# Begin Source File

SOURCE=..\common\in_buttons.h
# End Source File
# Begin Source File

SOURCE=..\common\msg_fx.h
# End Source File
# Begin Source File

SOURCE=..\common\netadr.h
# End Source File
# Begin Source File

SOURCE=..\common\nowin.h
# End Source File
# Begin Source File

SOURCE=..\common\platform.h
# End Source File
# Begin Source File

SOURCE=..\common\pmtrace.h
# End Source File
# Begin Source File

SOURCE=..\engine\progdefs.h
# End Source File
# Begin Source File

SOURCE=..\common\protocol.h
# End Source File
# Begin Source File

SOURCE=..\common\r_efx.h
# End Source File
# Begin Source File

SOURCE=..\common\ref_params.h
# End Source File
# Begin Source File

SOURCE=..\engine\Sequence.h
# End Source File
# Begin Source File

SOURCE=..\engine\shake.h
# End Source File
# Begin Source File

SOURCE=..\common\shared_resources.h
# End Source File
# Begin Source File

SOURCE=..\engine\studio.h
# End Source File
# Begin Source File

SOURCE=..\common\studio_event.h
# End Source File
# Begin Source File

SOURCE=..\common\usercmd.h
# End Source File
# Begin Source File

SOURCE=..\common\util_vector.h
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
# Begin Group "engine headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\engine\edict.h
# End Source File
# Begin Source File

SOURCE=..\engine\eiface.h
# End Source File
# End Group
# Begin Group "vc headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\common\basetsd.h
# End Source File
# End Group
# Begin Group "effects headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=.\effects.h
# End Source File
# Begin Source File

SOURCE=.\environment.h
# End Source File
# Begin Source File

SOURCE=.\explode.h
# End Source File
# End Group
# Begin Group "gamerules headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=.\gamerules\coop_gamerules.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\ctf_gamerules.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\dom_gamerules.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\gamerules.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\lms_gamerules.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\maprules.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\round_gamerules.h
# End Source File
# Begin Source File

SOURCE=.\gamerules\teamplay_gamerules.h
# End Source File
# End Group
# Begin Group "utils headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\utils\common\bspfile.h
# End Source File
# Begin Source File

SOURCE=..\utils\common\cmdlib.h
# End Source File
# Begin Source File

SOURCE=..\utils\common\mathlib.h
# End Source File
# End Group
# Begin Group "game shared headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\game_shared\bitvec.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_banmgr.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_common.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_gamemgr.h
# End Source File
# End Group
# Begin Group "projectiles headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=.\projectiles\projectiles.h
# End Source File
# End Group
# Begin Group "bot headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=.\bot\bot.h
# End Source File
# Begin Source File

SOURCE=.\bot\bot_main.h
# End Source File
# Begin Source File

SOURCE=.\bot\bot_memory.h
# End Source File
# Begin Source File

SOURCE=.\bot\bot_misc.h
# End Source File
# Begin Source File

SOURCE=.\bot\bot_stats.h
# End Source File
# End Group
# Begin Group "entities headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=.\doors.h
# End Source File
# Begin Source File

SOURCE=.\ent_alias.h
# End Source File
# Begin Source File

SOURCE=.\ent_functional.h
# End Source File
# Begin Source File

SOURCE=.\ent_locus.h
# End Source File
# Begin Source File

SOURCE=.\func_break.h
# End Source File
# Begin Source File

SOURCE=.\scripted.h
# End Source File
# Begin Source File

SOURCE=.\soundent.h
# End Source File
# Begin Source File

SOURCE=.\trains.h
# End Source File
# Begin Source File

SOURCE=.\triggers.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\activity.h
# End Source File
# Begin Source File

SOURCE=.\activitymap.h
# End Source File
# Begin Source File

SOURCE=.\animation.h
# End Source File
# Begin Source File

SOURCE=.\basemonster.h
# End Source File
# Begin Source File

SOURCE=.\cbase.h
# End Source File
# Begin Source File

SOURCE=.\client.h
# End Source File
# Begin Source File

SOURCE=.\defaultai.h
# End Source File
# Begin Source File

SOURCE=.\enginecallback.h
# End Source File
# Begin Source File

SOURCE=.\entconfig.h
# End Source File
# Begin Source File

SOURCE=.\extdll.h
# End Source File
# Begin Source File

SOURCE=.\flyingmonster.h
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\items.h
# End Source File
# Begin Source File

SOURCE=.\mapcycle.h
# End Source File
# Begin Source File

SOURCE=.\maprules.h
# End Source File
# Begin Source File

SOURCE=.\monsterevent.h
# End Source File
# Begin Source File

SOURCE=.\monsters.h
# End Source File
# Begin Source File

SOURCE=.\movewith.h
# End Source File
# Begin Source File

SOURCE=.\nodes.h
# End Source File
# Begin Source File

SOURCE=.\plane.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=.\saverestore.h
# End Source File
# Begin Source File

SOURCE=.\schedule.h
# End Source File
# Begin Source File

SOURCE=.\scriptevent.h
# End Source File
# Begin Source File

SOURCE=.\skill.h
# End Source File
# Begin Source File

SOURCE=.\sound.h
# End Source File
# Begin Source File

SOURCE=.\spectator.h
# End Source File
# Begin Source File

SOURCE=.\squad.h
# End Source File
# Begin Source File

SOURCE=.\squadmonster.h
# End Source File
# Begin Source File

SOURCE=.\talkmonster.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\weapons.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;rc;rc2"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\XDM.rc
# End Source File
# Begin Source File

SOURCE=..\common\res\xhl.ico
# End Source File
# End Group
# Begin Group "Other Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\comments.txt
# End Source File
# Begin Source File

SOURCE=.\makefile
# End Source File
# Begin Source File

SOURCE=.\XDM.def
# End Source File
# End Group
# End Target
# End Project
