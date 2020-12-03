# Microsoft Developer Studio Generated NMAKE File, Based on XDM.dsp
!IF "$(CFG)" == ""
CFG=XDM - Win32 Profile
!MESSAGE No configuration specified. Defaulting to XDM - Win32 Profile.
!ENDIF 

!IF "$(CFG)" != "XDM - Win32 Debug" && "$(CFG)" != "XDM - Win32 Release" && "$(CFG)" != "XDM - Win32 Profile"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XDM.mak" CFG="XDM - Win32 Profile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XDM - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XDM - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "XDM - Win32 Profile" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "XDM - Win32 Debug"

OUTDIR=.\debug
INTDIR=.\debug/intermediate
# Begin Custom Macros
OutDir=.\debug
# End Custom Macros

ALL : "$(OUTDIR)\XDM_i686.dll" "$(OUTDIR)\XDM.bsc" ".\XDM_i686"


CLEAN :
	-@erase "$(INTDIR)\aflock.obj"
	-@erase "$(INTDIR)\aflock.sbr"
	-@erase "$(INTDIR)\agrenade.obj"
	-@erase "$(INTDIR)\agrenade.sbr"
	-@erase "$(INTDIR)\agrunt.obj"
	-@erase "$(INTDIR)\agrunt.sbr"
	-@erase "$(INTDIR)\ai_basenpc_schedule.obj"
	-@erase "$(INTDIR)\ai_basenpc_schedule.sbr"
	-@erase "$(INTDIR)\airtank.obj"
	-@erase "$(INTDIR)\airtank.sbr"
	-@erase "$(INTDIR)\alauncher.obj"
	-@erase "$(INTDIR)\alauncher.sbr"
	-@erase "$(INTDIR)\ammo.obj"
	-@erase "$(INTDIR)\ammo.sbr"
	-@erase "$(INTDIR)\animating.obj"
	-@erase "$(INTDIR)\animating.sbr"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\animation.sbr"
	-@erase "$(INTDIR)\apache.obj"
	-@erase "$(INTDIR)\apache.sbr"
	-@erase "$(INTDIR)\atomicdevice.obj"
	-@erase "$(INTDIR)\atomicdevice.sbr"
	-@erase "$(INTDIR)\barnacle.obj"
	-@erase "$(INTDIR)\barnacle.sbr"
	-@erase "$(INTDIR)\barney.obj"
	-@erase "$(INTDIR)\barney.sbr"
	-@erase "$(INTDIR)\beamrifle.obj"
	-@erase "$(INTDIR)\beamrifle.sbr"
	-@erase "$(INTDIR)\bigmomma.obj"
	-@erase "$(INTDIR)\bigmomma.sbr"
	-@erase "$(INTDIR)\bloater.obj"
	-@erase "$(INTDIR)\bloater.sbr"
	-@erase "$(INTDIR)\bmodels.obj"
	-@erase "$(INTDIR)\bmodels.sbr"
	-@erase "$(INTDIR)\bullsquid.obj"
	-@erase "$(INTDIR)\bullsquid.sbr"
	-@erase "$(INTDIR)\buttons.obj"
	-@erase "$(INTDIR)\buttons.sbr"
	-@erase "$(INTDIR)\cat.obj"
	-@erase "$(INTDIR)\cat.sbr"
	-@erase "$(INTDIR)\cbase.obj"
	-@erase "$(INTDIR)\cbase.sbr"
	-@erase "$(INTDIR)\chasecam.obj"
	-@erase "$(INTDIR)\chasecam.sbr"
	-@erase "$(INTDIR)\chemgun.obj"
	-@erase "$(INTDIR)\chemgun.sbr"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\client.sbr"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\combat.sbr"
	-@erase "$(INTDIR)\controller.obj"
	-@erase "$(INTDIR)\controller.sbr"
	-@erase "$(INTDIR)\coop_gamerules.obj"
	-@erase "$(INTDIR)\coop_gamerules.sbr"
	-@erase "$(INTDIR)\crossbow.obj"
	-@erase "$(INTDIR)\crossbow.sbr"
	-@erase "$(INTDIR)\crossbowbolt.obj"
	-@erase "$(INTDIR)\crossbowbolt.sbr"
	-@erase "$(INTDIR)\crowbar.obj"
	-@erase "$(INTDIR)\crowbar.sbr"
	-@erase "$(INTDIR)\ctf_gamerules.obj"
	-@erase "$(INTDIR)\ctf_gamerules.sbr"
	-@erase "$(INTDIR)\deadhev.obj"
	-@erase "$(INTDIR)\deadhev.sbr"
	-@erase "$(INTDIR)\defaultai.obj"
	-@erase "$(INTDIR)\defaultai.sbr"
	-@erase "$(INTDIR)\diablo.obj"
	-@erase "$(INTDIR)\diablo.sbr"
	-@erase "$(INTDIR)\displacer.obj"
	-@erase "$(INTDIR)\displacer.sbr"
	-@erase "$(INTDIR)\dom_gamerules.obj"
	-@erase "$(INTDIR)\dom_gamerules.sbr"
	-@erase "$(INTDIR)\doors.obj"
	-@erase "$(INTDIR)\doors.sbr"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\effects.sbr"
	-@erase "$(INTDIR)\effects_new.obj"
	-@erase "$(INTDIR)\effects_new.sbr"
	-@erase "$(INTDIR)\egon.obj"
	-@erase "$(INTDIR)\egon.sbr"
	-@erase "$(INTDIR)\entconfig.obj"
	-@erase "$(INTDIR)\entconfig.sbr"
	-@erase "$(INTDIR)\environment.obj"
	-@erase "$(INTDIR)\environment.sbr"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\explode.sbr"
	-@erase "$(INTDIR)\flamecloud.obj"
	-@erase "$(INTDIR)\flamecloud.sbr"
	-@erase "$(INTDIR)\flamethrower.obj"
	-@erase "$(INTDIR)\flamethrower.sbr"
	-@erase "$(INTDIR)\flyingmonster.obj"
	-@erase "$(INTDIR)\flyingmonster.sbr"
	-@erase "$(INTDIR)\func_break.obj"
	-@erase "$(INTDIR)\func_break.sbr"
	-@erase "$(INTDIR)\func_tank.obj"
	-@erase "$(INTDIR)\func_tank.sbr"
	-@erase "$(INTDIR)\fx.obj"
	-@erase "$(INTDIR)\fx.sbr"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\game.sbr"
	-@erase "$(INTDIR)\gamerules.obj"
	-@erase "$(INTDIR)\gamerules.sbr"
	-@erase "$(INTDIR)\gargantua.obj"
	-@erase "$(INTDIR)\gargantua.sbr"
	-@erase "$(INTDIR)\gauss.obj"
	-@erase "$(INTDIR)\gauss.sbr"
	-@erase "$(INTDIR)\genericmonster.obj"
	-@erase "$(INTDIR)\genericmonster.sbr"
	-@erase "$(INTDIR)\ggrenade.obj"
	-@erase "$(INTDIR)\ggrenade.sbr"
	-@erase "$(INTDIR)\gibs.obj"
	-@erase "$(INTDIR)\gibs.sbr"
	-@erase "$(INTDIR)\glauncher.obj"
	-@erase "$(INTDIR)\glauncher.sbr"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\globals.sbr"
	-@erase "$(INTDIR)\glock.obj"
	-@erase "$(INTDIR)\glock.sbr"
	-@erase "$(INTDIR)\gman.obj"
	-@erase "$(INTDIR)\gman.sbr"
	-@erase "$(INTDIR)\h_ai.obj"
	-@erase "$(INTDIR)\h_ai.sbr"
	-@erase "$(INTDIR)\h_battery.obj"
	-@erase "$(INTDIR)\h_battery.sbr"
	-@erase "$(INTDIR)\h_cycler.obj"
	-@erase "$(INTDIR)\h_cycler.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\handgrenade.obj"
	-@erase "$(INTDIR)\handgrenade.sbr"
	-@erase "$(INTDIR)\hassassin.obj"
	-@erase "$(INTDIR)\hassassin.sbr"
	-@erase "$(INTDIR)\headcrab.obj"
	-@erase "$(INTDIR)\headcrab.sbr"
	-@erase "$(INTDIR)\healthkit.obj"
	-@erase "$(INTDIR)\healthkit.sbr"
	-@erase "$(INTDIR)\hgrunt.obj"
	-@erase "$(INTDIR)\hgrunt.sbr"
	-@erase "$(INTDIR)\hornet.obj"
	-@erase "$(INTDIR)\hornet.sbr"
	-@erase "$(INTDIR)\hornetgun.obj"
	-@erase "$(INTDIR)\hornetgun.sbr"
	-@erase "$(INTDIR)\houndeye.obj"
	-@erase "$(INTDIR)\houndeye.sbr"
	-@erase "$(INTDIR)\ichthyosaur.obj"
	-@erase "$(INTDIR)\ichthyosaur.sbr"
	-@erase "$(INTDIR)\islave.obj"
	-@erase "$(INTDIR)\islave.sbr"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\items.sbr"
	-@erase "$(INTDIR)\items_new.obj"
	-@erase "$(INTDIR)\items_new.sbr"
	-@erase "$(INTDIR)\leech.obj"
	-@erase "$(INTDIR)\leech.sbr"
	-@erase "$(INTDIR)\lgrenade.obj"
	-@erase "$(INTDIR)\lgrenade.sbr"
	-@erase "$(INTDIR)\lightp.obj"
	-@erase "$(INTDIR)\lightp.sbr"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\lights.sbr"
	-@erase "$(INTDIR)\lms_gamerules.obj"
	-@erase "$(INTDIR)\lms_gamerules.sbr"
	-@erase "$(INTDIR)\mapcycle.obj"
	-@erase "$(INTDIR)\mapcycle.sbr"
	-@erase "$(INTDIR)\maprules.obj"
	-@erase "$(INTDIR)\maprules.sbr"
	-@erase "$(INTDIR)\monstermaker.obj"
	-@erase "$(INTDIR)\monstermaker.sbr"
	-@erase "$(INTDIR)\monsters.obj"
	-@erase "$(INTDIR)\monsters.sbr"
	-@erase "$(INTDIR)\monsterstate.obj"
	-@erase "$(INTDIR)\monsterstate.sbr"
	-@erase "$(INTDIR)\mortar.obj"
	-@erase "$(INTDIR)\mortar.sbr"
	-@erase "$(INTDIR)\mp5.obj"
	-@erase "$(INTDIR)\mp5.sbr"
	-@erase "$(INTDIR)\mtarget.obj"
	-@erase "$(INTDIR)\mtarget.sbr"
	-@erase "$(INTDIR)\multiplay_gamerules.obj"
	-@erase "$(INTDIR)\multiplay_gamerules.sbr"
	-@erase "$(INTDIR)\nanosword.obj"
	-@erase "$(INTDIR)\nanosword.sbr"
	-@erase "$(INTDIR)\nihilanth.obj"
	-@erase "$(INTDIR)\nihilanth.sbr"
	-@erase "$(INTDIR)\nodes.obj"
	-@erase "$(INTDIR)\nodes.sbr"
	-@erase "$(INTDIR)\observer.obj"
	-@erase "$(INTDIR)\observer.sbr"
	-@erase "$(INTDIR)\orb.obj"
	-@erase "$(INTDIR)\orb.sbr"
	-@erase "$(INTDIR)\orb_proj.obj"
	-@erase "$(INTDIR)\orb_proj.sbr"
	-@erase "$(INTDIR)\osprey.obj"
	-@erase "$(INTDIR)\osprey.sbr"
	-@erase "$(INTDIR)\pathcorner.obj"
	-@erase "$(INTDIR)\pathcorner.sbr"
	-@erase "$(INTDIR)\plane.obj"
	-@erase "$(INTDIR)\plane.sbr"
	-@erase "$(INTDIR)\plasma.obj"
	-@erase "$(INTDIR)\plasma.sbr"
	-@erase "$(INTDIR)\plasmaball.obj"
	-@erase "$(INTDIR)\plasmaball.sbr"
	-@erase "$(INTDIR)\plats.obj"
	-@erase "$(INTDIR)\plats.sbr"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\player.sbr"
	-@erase "$(INTDIR)\playermonster.obj"
	-@erase "$(INTDIR)\playermonster.sbr"
	-@erase "$(INTDIR)\pm_debug.obj"
	-@erase "$(INTDIR)\pm_debug.sbr"
	-@erase "$(INTDIR)\pm_math.obj"
	-@erase "$(INTDIR)\pm_math.sbr"
	-@erase "$(INTDIR)\pm_shared.obj"
	-@erase "$(INTDIR)\pm_shared.sbr"
	-@erase "$(INTDIR)\python.obj"
	-@erase "$(INTDIR)\python.sbr"
	-@erase "$(INTDIR)\rat.obj"
	-@erase "$(INTDIR)\rat.sbr"
	-@erase "$(INTDIR)\roach.obj"
	-@erase "$(INTDIR)\roach.sbr"
	-@erase "$(INTDIR)\rpg.obj"
	-@erase "$(INTDIR)\rpg.sbr"
	-@erase "$(INTDIR)\rpgrocket.obj"
	-@erase "$(INTDIR)\rpgrocket.sbr"
	-@erase "$(INTDIR)\satchel.obj"
	-@erase "$(INTDIR)\satchel.sbr"
	-@erase "$(INTDIR)\satchelcharge.obj"
	-@erase "$(INTDIR)\satchelcharge.sbr"
	-@erase "$(INTDIR)\saverestore.obj"
	-@erase "$(INTDIR)\saverestore.sbr"
	-@erase "$(INTDIR)\scientist.obj"
	-@erase "$(INTDIR)\scientist.sbr"
	-@erase "$(INTDIR)\scripted.obj"
	-@erase "$(INTDIR)\scripted.sbr"
	-@erase "$(INTDIR)\shotgun.obj"
	-@erase "$(INTDIR)\shotgun.sbr"
	-@erase "$(INTDIR)\singleplay_gamerules.obj"
	-@erase "$(INTDIR)\singleplay_gamerules.sbr"
	-@erase "$(INTDIR)\skill.obj"
	-@erase "$(INTDIR)\skill.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\soundent.obj"
	-@erase "$(INTDIR)\soundent.sbr"
	-@erase "$(INTDIR)\spectator.obj"
	-@erase "$(INTDIR)\spectator.sbr"
	-@erase "$(INTDIR)\sqkgrenade.obj"
	-@erase "$(INTDIR)\sqkgrenade.sbr"
	-@erase "$(INTDIR)\squadmonster.obj"
	-@erase "$(INTDIR)\squadmonster.sbr"
	-@erase "$(INTDIR)\squeakgrenade.obj"
	-@erase "$(INTDIR)\squeakgrenade.sbr"
	-@erase "$(INTDIR)\strtarget.obj"
	-@erase "$(INTDIR)\strtarget.sbr"
	-@erase "$(INTDIR)\subs.obj"
	-@erase "$(INTDIR)\subs.sbr"
	-@erase "$(INTDIR)\sword.obj"
	-@erase "$(INTDIR)\sword.sbr"
	-@erase "$(INTDIR)\talkmonster.obj"
	-@erase "$(INTDIR)\talkmonster.sbr"
	-@erase "$(INTDIR)\teamplay_gamerules.obj"
	-@erase "$(INTDIR)\teamplay_gamerules.sbr"
	-@erase "$(INTDIR)\teleporter.obj"
	-@erase "$(INTDIR)\teleporter.sbr"
	-@erase "$(INTDIR)\tentacle.obj"
	-@erase "$(INTDIR)\tentacle.sbr"
	-@erase "$(INTDIR)\triggers.obj"
	-@erase "$(INTDIR)\triggers.sbr"
	-@erase "$(INTDIR)\triggers_new.obj"
	-@erase "$(INTDIR)\triggers_new.sbr"
	-@erase "$(INTDIR)\tripmine.obj"
	-@erase "$(INTDIR)\tripmine.sbr"
	-@erase "$(INTDIR)\tripminegrenade.obj"
	-@erase "$(INTDIR)\tripminegrenade.sbr"
	-@erase "$(INTDIR)\turret.obj"
	-@erase "$(INTDIR)\turret.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\util_vector.obj"
	-@erase "$(INTDIR)\util_vector.sbr"
	-@erase "$(INTDIR)\util_xhl.obj"
	-@erase "$(INTDIR)\util_xhl.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vector.obj"
	-@erase "$(INTDIR)\vector.sbr"
	-@erase "$(INTDIR)\voice_banmgr.obj"
	-@erase "$(INTDIR)\voice_banmgr.sbr"
	-@erase "$(INTDIR)\voice_gamemgr.obj"
	-@erase "$(INTDIR)\voice_gamemgr.sbr"
	-@erase "$(INTDIR)\weaponbox.obj"
	-@erase "$(INTDIR)\weaponbox.sbr"
	-@erase "$(INTDIR)\weapons.obj"
	-@erase "$(INTDIR)\weapons.sbr"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\world.sbr"
	-@erase "$(INTDIR)\XDM.res"
	-@erase "$(INTDIR)\xen.obj"
	-@erase "$(INTDIR)\xen.sbr"
	-@erase "$(INTDIR)\zombie.obj"
	-@erase "$(INTDIR)\zombie.sbr"
	-@erase "$(OUTDIR)\intermediate\XDM_i686.map"
	-@erase "$(OUTDIR)\XDM.bsc"
	-@erase "$(OUTDIR)\XDM_i686.dll"
	-@erase "$(OUTDIR)\XDM_i686.exp"
	-@erase "$(OUTDIR)\XDM_i686.pdb"
	-@erase "XDM_i686"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "bot" /I "gamerules" /I "projectiles" /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\game_shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\XDM.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\XDM.res" /i ".\res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\XDM.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\pm_debug.sbr" \
	"$(INTDIR)\pm_math.sbr" \
	"$(INTDIR)\pm_shared.sbr" \
	"$(INTDIR)\alauncher.sbr" \
	"$(INTDIR)\beamrifle.sbr" \
	"$(INTDIR)\chemgun.sbr" \
	"$(INTDIR)\crossbow.sbr" \
	"$(INTDIR)\crowbar.sbr" \
	"$(INTDIR)\displacer.sbr" \
	"$(INTDIR)\egon.sbr" \
	"$(INTDIR)\flamethrower.sbr" \
	"$(INTDIR)\gauss.sbr" \
	"$(INTDIR)\glauncher.sbr" \
	"$(INTDIR)\glock.sbr" \
	"$(INTDIR)\handgrenade.sbr" \
	"$(INTDIR)\hornetgun.sbr" \
	"$(INTDIR)\mp5.sbr" \
	"$(INTDIR)\nanosword.sbr" \
	"$(INTDIR)\orb.sbr" \
	"$(INTDIR)\plasma.sbr" \
	"$(INTDIR)\python.sbr" \
	"$(INTDIR)\rpg.sbr" \
	"$(INTDIR)\satchel.sbr" \
	"$(INTDIR)\shotgun.sbr" \
	"$(INTDIR)\squeakgrenade.sbr" \
	"$(INTDIR)\strtarget.sbr" \
	"$(INTDIR)\sword.sbr" \
	"$(INTDIR)\tripmine.sbr" \
	"$(INTDIR)\aflock.sbr" \
	"$(INTDIR)\agrunt.sbr" \
	"$(INTDIR)\apache.sbr" \
	"$(INTDIR)\barnacle.sbr" \
	"$(INTDIR)\barney.sbr" \
	"$(INTDIR)\bigmomma.sbr" \
	"$(INTDIR)\bloater.sbr" \
	"$(INTDIR)\bullsquid.sbr" \
	"$(INTDIR)\cat.sbr" \
	"$(INTDIR)\controller.sbr" \
	"$(INTDIR)\deadhev.sbr" \
	"$(INTDIR)\diablo.sbr" \
	"$(INTDIR)\flyingmonster.sbr" \
	"$(INTDIR)\gargantua.sbr" \
	"$(INTDIR)\genericmonster.sbr" \
	"$(INTDIR)\gman.sbr" \
	"$(INTDIR)\hassassin.sbr" \
	"$(INTDIR)\headcrab.sbr" \
	"$(INTDIR)\hgrunt.sbr" \
	"$(INTDIR)\houndeye.sbr" \
	"$(INTDIR)\ichthyosaur.sbr" \
	"$(INTDIR)\islave.sbr" \
	"$(INTDIR)\leech.sbr" \
	"$(INTDIR)\monsters.sbr" \
	"$(INTDIR)\monsterstate.sbr" \
	"$(INTDIR)\nihilanth.sbr" \
	"$(INTDIR)\osprey.sbr" \
	"$(INTDIR)\playermonster.sbr" \
	"$(INTDIR)\rat.sbr" \
	"$(INTDIR)\roach.sbr" \
	"$(INTDIR)\scientist.sbr" \
	"$(INTDIR)\squadmonster.sbr" \
	"$(INTDIR)\talkmonster.sbr" \
	"$(INTDIR)\tentacle.sbr" \
	"$(INTDIR)\turret.sbr" \
	"$(INTDIR)\zombie.sbr" \
	"$(INTDIR)\airtank.sbr" \
	"$(INTDIR)\healthkit.sbr" \
	"$(INTDIR)\items.sbr" \
	"$(INTDIR)\items_new.sbr" \
	"$(INTDIR)\coop_gamerules.sbr" \
	"$(INTDIR)\ctf_gamerules.sbr" \
	"$(INTDIR)\dom_gamerules.sbr" \
	"$(INTDIR)\gamerules.sbr" \
	"$(INTDIR)\lms_gamerules.sbr" \
	"$(INTDIR)\maprules.sbr" \
	"$(INTDIR)\multiplay_gamerules.sbr" \
	"$(INTDIR)\singleplay_gamerules.sbr" \
	"$(INTDIR)\teamplay_gamerules.sbr" \
	"$(INTDIR)\effects.sbr" \
	"$(INTDIR)\effects_new.sbr" \
	"$(INTDIR)\environment.sbr" \
	"$(INTDIR)\explode.sbr" \
	"$(INTDIR)\fx.sbr" \
	"$(INTDIR)\agrenade.sbr" \
	"$(INTDIR)\atomicdevice.sbr" \
	"$(INTDIR)\crossbowbolt.sbr" \
	"$(INTDIR)\flamecloud.sbr" \
	"$(INTDIR)\ggrenade.sbr" \
	"$(INTDIR)\hornet.sbr" \
	"$(INTDIR)\lgrenade.sbr" \
	"$(INTDIR)\lightp.sbr" \
	"$(INTDIR)\mtarget.sbr" \
	"$(INTDIR)\orb_proj.sbr" \
	"$(INTDIR)\plasmaball.sbr" \
	"$(INTDIR)\rpgrocket.sbr" \
	"$(INTDIR)\satchelcharge.sbr" \
	"$(INTDIR)\sqkgrenade.sbr" \
	"$(INTDIR)\teleporter.sbr" \
	"$(INTDIR)\tripminegrenade.sbr" \
	"$(INTDIR)\voice_banmgr.sbr" \
	"$(INTDIR)\voice_gamemgr.sbr" \
	"$(INTDIR)\ai_basenpc_schedule.sbr" \
	"$(INTDIR)\ammo.sbr" \
	"$(INTDIR)\animating.sbr" \
	"$(INTDIR)\animation.sbr" \
	"$(INTDIR)\bmodels.sbr" \
	"$(INTDIR)\buttons.sbr" \
	"$(INTDIR)\cbase.sbr" \
	"$(INTDIR)\chasecam.sbr" \
	"$(INTDIR)\client.sbr" \
	"$(INTDIR)\combat.sbr" \
	"$(INTDIR)\defaultai.sbr" \
	"$(INTDIR)\doors.sbr" \
	"$(INTDIR)\entconfig.sbr" \
	"$(INTDIR)\func_break.sbr" \
	"$(INTDIR)\func_tank.sbr" \
	"$(INTDIR)\game.sbr" \
	"$(INTDIR)\gibs.sbr" \
	"$(INTDIR)\globals.sbr" \
	"$(INTDIR)\h_ai.sbr" \
	"$(INTDIR)\h_battery.sbr" \
	"$(INTDIR)\h_cycler.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\lights.sbr" \
	"$(INTDIR)\mapcycle.sbr" \
	"$(INTDIR)\monstermaker.sbr" \
	"$(INTDIR)\mortar.sbr" \
	"$(INTDIR)\nodes.sbr" \
	"$(INTDIR)\observer.sbr" \
	"$(INTDIR)\pathcorner.sbr" \
	"$(INTDIR)\plane.sbr" \
	"$(INTDIR)\plats.sbr" \
	"$(INTDIR)\player.sbr" \
	"$(INTDIR)\saverestore.sbr" \
	"$(INTDIR)\scripted.sbr" \
	"$(INTDIR)\skill.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\soundent.sbr" \
	"$(INTDIR)\spectator.sbr" \
	"$(INTDIR)\subs.sbr" \
	"$(INTDIR)\triggers.sbr" \
	"$(INTDIR)\triggers_new.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\util_xhl.sbr" \
	"$(INTDIR)\weaponbox.sbr" \
	"$(INTDIR)\weapons.sbr" \
	"$(INTDIR)\world.sbr" \
	"$(INTDIR)\xen.sbr" \
	"$(INTDIR)\util_vector.sbr" \
	"$(INTDIR)\vector.sbr"

"$(OUTDIR)\XDM.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /version:3.0 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\XDM_i686.pdb" /map:"$(INTDIR)\XDM_i686.map" /debug /machine:I386 /def:".\XDM.def" /out:"$(OUTDIR)\XDM_i686.dll" /implib:"$(OUTDIR)\XDM_i686.lib" 
DEF_FILE= \
	".\XDM.def"
LINK32_OBJS= \
	"$(INTDIR)\pm_debug.obj" \
	"$(INTDIR)\pm_math.obj" \
	"$(INTDIR)\pm_shared.obj" \
	"$(INTDIR)\alauncher.obj" \
	"$(INTDIR)\beamrifle.obj" \
	"$(INTDIR)\chemgun.obj" \
	"$(INTDIR)\crossbow.obj" \
	"$(INTDIR)\crowbar.obj" \
	"$(INTDIR)\displacer.obj" \
	"$(INTDIR)\egon.obj" \
	"$(INTDIR)\flamethrower.obj" \
	"$(INTDIR)\gauss.obj" \
	"$(INTDIR)\glauncher.obj" \
	"$(INTDIR)\glock.obj" \
	"$(INTDIR)\handgrenade.obj" \
	"$(INTDIR)\hornetgun.obj" \
	"$(INTDIR)\mp5.obj" \
	"$(INTDIR)\nanosword.obj" \
	"$(INTDIR)\orb.obj" \
	"$(INTDIR)\plasma.obj" \
	"$(INTDIR)\python.obj" \
	"$(INTDIR)\rpg.obj" \
	"$(INTDIR)\satchel.obj" \
	"$(INTDIR)\shotgun.obj" \
	"$(INTDIR)\squeakgrenade.obj" \
	"$(INTDIR)\strtarget.obj" \
	"$(INTDIR)\sword.obj" \
	"$(INTDIR)\tripmine.obj" \
	"$(INTDIR)\aflock.obj" \
	"$(INTDIR)\agrunt.obj" \
	"$(INTDIR)\apache.obj" \
	"$(INTDIR)\barnacle.obj" \
	"$(INTDIR)\barney.obj" \
	"$(INTDIR)\bigmomma.obj" \
	"$(INTDIR)\bloater.obj" \
	"$(INTDIR)\bullsquid.obj" \
	"$(INTDIR)\cat.obj" \
	"$(INTDIR)\controller.obj" \
	"$(INTDIR)\deadhev.obj" \
	"$(INTDIR)\diablo.obj" \
	"$(INTDIR)\flyingmonster.obj" \
	"$(INTDIR)\gargantua.obj" \
	"$(INTDIR)\genericmonster.obj" \
	"$(INTDIR)\gman.obj" \
	"$(INTDIR)\hassassin.obj" \
	"$(INTDIR)\headcrab.obj" \
	"$(INTDIR)\hgrunt.obj" \
	"$(INTDIR)\houndeye.obj" \
	"$(INTDIR)\ichthyosaur.obj" \
	"$(INTDIR)\islave.obj" \
	"$(INTDIR)\leech.obj" \
	"$(INTDIR)\monsters.obj" \
	"$(INTDIR)\monsterstate.obj" \
	"$(INTDIR)\nihilanth.obj" \
	"$(INTDIR)\osprey.obj" \
	"$(INTDIR)\playermonster.obj" \
	"$(INTDIR)\rat.obj" \
	"$(INTDIR)\roach.obj" \
	"$(INTDIR)\scientist.obj" \
	"$(INTDIR)\squadmonster.obj" \
	"$(INTDIR)\talkmonster.obj" \
	"$(INTDIR)\tentacle.obj" \
	"$(INTDIR)\turret.obj" \
	"$(INTDIR)\zombie.obj" \
	"$(INTDIR)\airtank.obj" \
	"$(INTDIR)\healthkit.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\items_new.obj" \
	"$(INTDIR)\coop_gamerules.obj" \
	"$(INTDIR)\ctf_gamerules.obj" \
	"$(INTDIR)\dom_gamerules.obj" \
	"$(INTDIR)\gamerules.obj" \
	"$(INTDIR)\lms_gamerules.obj" \
	"$(INTDIR)\maprules.obj" \
	"$(INTDIR)\multiplay_gamerules.obj" \
	"$(INTDIR)\singleplay_gamerules.obj" \
	"$(INTDIR)\teamplay_gamerules.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\effects_new.obj" \
	"$(INTDIR)\environment.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\fx.obj" \
	"$(INTDIR)\agrenade.obj" \
	"$(INTDIR)\atomicdevice.obj" \
	"$(INTDIR)\crossbowbolt.obj" \
	"$(INTDIR)\flamecloud.obj" \
	"$(INTDIR)\ggrenade.obj" \
	"$(INTDIR)\hornet.obj" \
	"$(INTDIR)\lgrenade.obj" \
	"$(INTDIR)\lightp.obj" \
	"$(INTDIR)\mtarget.obj" \
	"$(INTDIR)\orb_proj.obj" \
	"$(INTDIR)\plasmaball.obj" \
	"$(INTDIR)\rpgrocket.obj" \
	"$(INTDIR)\satchelcharge.obj" \
	"$(INTDIR)\sqkgrenade.obj" \
	"$(INTDIR)\teleporter.obj" \
	"$(INTDIR)\tripminegrenade.obj" \
	"$(INTDIR)\voice_banmgr.obj" \
	"$(INTDIR)\voice_gamemgr.obj" \
	"$(INTDIR)\ai_basenpc_schedule.obj" \
	"$(INTDIR)\ammo.obj" \
	"$(INTDIR)\animating.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\bmodels.obj" \
	"$(INTDIR)\buttons.obj" \
	"$(INTDIR)\cbase.obj" \
	"$(INTDIR)\chasecam.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\defaultai.obj" \
	"$(INTDIR)\doors.obj" \
	"$(INTDIR)\entconfig.obj" \
	"$(INTDIR)\func_break.obj" \
	"$(INTDIR)\func_tank.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\gibs.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\h_ai.obj" \
	"$(INTDIR)\h_battery.obj" \
	"$(INTDIR)\h_cycler.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\mapcycle.obj" \
	"$(INTDIR)\monstermaker.obj" \
	"$(INTDIR)\mortar.obj" \
	"$(INTDIR)\nodes.obj" \
	"$(INTDIR)\observer.obj" \
	"$(INTDIR)\pathcorner.obj" \
	"$(INTDIR)\plane.obj" \
	"$(INTDIR)\plats.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\saverestore.obj" \
	"$(INTDIR)\scripted.obj" \
	"$(INTDIR)\skill.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\soundent.obj" \
	"$(INTDIR)\spectator.obj" \
	"$(INTDIR)\subs.obj" \
	"$(INTDIR)\triggers.obj" \
	"$(INTDIR)\triggers_new.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\util_xhl.obj" \
	"$(INTDIR)\weaponbox.obj" \
	"$(INTDIR)\weapons.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\xen.obj" \
	"$(INTDIR)\XDM.res" \
	"$(INTDIR)\util_vector.obj" \
	"$(INTDIR)\vector.obj"

"$(OUTDIR)\XDM_i686.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\debug\XDM_i686.dll
InputPath=.\debug\XDM_i686.dll
InputName=XDM_i686
SOURCE="$(InputPath)"

".\XDM_i686" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy          $(TargetPath)          \Half-Life\XDM\dlls\  
	copy          $(TargetPath)          F:\games\HL1121\XDM\dlls\  
	copy $(TargetPath) W:\dev\Half-Life\XDM\dlls\
<< 
	

!ELSEIF  "$(CFG)" == "XDM - Win32 Release"

OUTDIR=.\release
INTDIR=.\release/intermediate
# Begin Custom Macros
OutDir=.\release
# End Custom Macros

ALL : "$(OUTDIR)\XDM_i686.dll" "$(OUTDIR)\XDM.bsc" ".\XDM_i686"


CLEAN :
	-@erase "$(INTDIR)\aflock.obj"
	-@erase "$(INTDIR)\aflock.sbr"
	-@erase "$(INTDIR)\agrenade.obj"
	-@erase "$(INTDIR)\agrenade.sbr"
	-@erase "$(INTDIR)\agrunt.obj"
	-@erase "$(INTDIR)\agrunt.sbr"
	-@erase "$(INTDIR)\ai_basenpc_schedule.obj"
	-@erase "$(INTDIR)\ai_basenpc_schedule.sbr"
	-@erase "$(INTDIR)\airtank.obj"
	-@erase "$(INTDIR)\airtank.sbr"
	-@erase "$(INTDIR)\alauncher.obj"
	-@erase "$(INTDIR)\alauncher.sbr"
	-@erase "$(INTDIR)\ammo.obj"
	-@erase "$(INTDIR)\ammo.sbr"
	-@erase "$(INTDIR)\animating.obj"
	-@erase "$(INTDIR)\animating.sbr"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\animation.sbr"
	-@erase "$(INTDIR)\apache.obj"
	-@erase "$(INTDIR)\apache.sbr"
	-@erase "$(INTDIR)\atomicdevice.obj"
	-@erase "$(INTDIR)\atomicdevice.sbr"
	-@erase "$(INTDIR)\barnacle.obj"
	-@erase "$(INTDIR)\barnacle.sbr"
	-@erase "$(INTDIR)\barney.obj"
	-@erase "$(INTDIR)\barney.sbr"
	-@erase "$(INTDIR)\beamrifle.obj"
	-@erase "$(INTDIR)\beamrifle.sbr"
	-@erase "$(INTDIR)\bigmomma.obj"
	-@erase "$(INTDIR)\bigmomma.sbr"
	-@erase "$(INTDIR)\bloater.obj"
	-@erase "$(INTDIR)\bloater.sbr"
	-@erase "$(INTDIR)\bmodels.obj"
	-@erase "$(INTDIR)\bmodels.sbr"
	-@erase "$(INTDIR)\bullsquid.obj"
	-@erase "$(INTDIR)\bullsquid.sbr"
	-@erase "$(INTDIR)\buttons.obj"
	-@erase "$(INTDIR)\buttons.sbr"
	-@erase "$(INTDIR)\cat.obj"
	-@erase "$(INTDIR)\cat.sbr"
	-@erase "$(INTDIR)\cbase.obj"
	-@erase "$(INTDIR)\cbase.sbr"
	-@erase "$(INTDIR)\chasecam.obj"
	-@erase "$(INTDIR)\chasecam.sbr"
	-@erase "$(INTDIR)\chemgun.obj"
	-@erase "$(INTDIR)\chemgun.sbr"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\client.sbr"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\combat.sbr"
	-@erase "$(INTDIR)\controller.obj"
	-@erase "$(INTDIR)\controller.sbr"
	-@erase "$(INTDIR)\coop_gamerules.obj"
	-@erase "$(INTDIR)\coop_gamerules.sbr"
	-@erase "$(INTDIR)\crossbow.obj"
	-@erase "$(INTDIR)\crossbow.sbr"
	-@erase "$(INTDIR)\crossbowbolt.obj"
	-@erase "$(INTDIR)\crossbowbolt.sbr"
	-@erase "$(INTDIR)\crowbar.obj"
	-@erase "$(INTDIR)\crowbar.sbr"
	-@erase "$(INTDIR)\ctf_gamerules.obj"
	-@erase "$(INTDIR)\ctf_gamerules.sbr"
	-@erase "$(INTDIR)\deadhev.obj"
	-@erase "$(INTDIR)\deadhev.sbr"
	-@erase "$(INTDIR)\defaultai.obj"
	-@erase "$(INTDIR)\defaultai.sbr"
	-@erase "$(INTDIR)\diablo.obj"
	-@erase "$(INTDIR)\diablo.sbr"
	-@erase "$(INTDIR)\displacer.obj"
	-@erase "$(INTDIR)\displacer.sbr"
	-@erase "$(INTDIR)\dom_gamerules.obj"
	-@erase "$(INTDIR)\dom_gamerules.sbr"
	-@erase "$(INTDIR)\doors.obj"
	-@erase "$(INTDIR)\doors.sbr"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\effects.sbr"
	-@erase "$(INTDIR)\effects_new.obj"
	-@erase "$(INTDIR)\effects_new.sbr"
	-@erase "$(INTDIR)\egon.obj"
	-@erase "$(INTDIR)\egon.sbr"
	-@erase "$(INTDIR)\entconfig.obj"
	-@erase "$(INTDIR)\entconfig.sbr"
	-@erase "$(INTDIR)\environment.obj"
	-@erase "$(INTDIR)\environment.sbr"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\explode.sbr"
	-@erase "$(INTDIR)\flamecloud.obj"
	-@erase "$(INTDIR)\flamecloud.sbr"
	-@erase "$(INTDIR)\flamethrower.obj"
	-@erase "$(INTDIR)\flamethrower.sbr"
	-@erase "$(INTDIR)\flyingmonster.obj"
	-@erase "$(INTDIR)\flyingmonster.sbr"
	-@erase "$(INTDIR)\func_break.obj"
	-@erase "$(INTDIR)\func_break.sbr"
	-@erase "$(INTDIR)\func_tank.obj"
	-@erase "$(INTDIR)\func_tank.sbr"
	-@erase "$(INTDIR)\fx.obj"
	-@erase "$(INTDIR)\fx.sbr"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\game.sbr"
	-@erase "$(INTDIR)\gamerules.obj"
	-@erase "$(INTDIR)\gamerules.sbr"
	-@erase "$(INTDIR)\gargantua.obj"
	-@erase "$(INTDIR)\gargantua.sbr"
	-@erase "$(INTDIR)\gauss.obj"
	-@erase "$(INTDIR)\gauss.sbr"
	-@erase "$(INTDIR)\genericmonster.obj"
	-@erase "$(INTDIR)\genericmonster.sbr"
	-@erase "$(INTDIR)\ggrenade.obj"
	-@erase "$(INTDIR)\ggrenade.sbr"
	-@erase "$(INTDIR)\gibs.obj"
	-@erase "$(INTDIR)\gibs.sbr"
	-@erase "$(INTDIR)\glauncher.obj"
	-@erase "$(INTDIR)\glauncher.sbr"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\globals.sbr"
	-@erase "$(INTDIR)\glock.obj"
	-@erase "$(INTDIR)\glock.sbr"
	-@erase "$(INTDIR)\gman.obj"
	-@erase "$(INTDIR)\gman.sbr"
	-@erase "$(INTDIR)\h_ai.obj"
	-@erase "$(INTDIR)\h_ai.sbr"
	-@erase "$(INTDIR)\h_battery.obj"
	-@erase "$(INTDIR)\h_battery.sbr"
	-@erase "$(INTDIR)\h_cycler.obj"
	-@erase "$(INTDIR)\h_cycler.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\handgrenade.obj"
	-@erase "$(INTDIR)\handgrenade.sbr"
	-@erase "$(INTDIR)\hassassin.obj"
	-@erase "$(INTDIR)\hassassin.sbr"
	-@erase "$(INTDIR)\headcrab.obj"
	-@erase "$(INTDIR)\headcrab.sbr"
	-@erase "$(INTDIR)\healthkit.obj"
	-@erase "$(INTDIR)\healthkit.sbr"
	-@erase "$(INTDIR)\hgrunt.obj"
	-@erase "$(INTDIR)\hgrunt.sbr"
	-@erase "$(INTDIR)\hornet.obj"
	-@erase "$(INTDIR)\hornet.sbr"
	-@erase "$(INTDIR)\hornetgun.obj"
	-@erase "$(INTDIR)\hornetgun.sbr"
	-@erase "$(INTDIR)\houndeye.obj"
	-@erase "$(INTDIR)\houndeye.sbr"
	-@erase "$(INTDIR)\ichthyosaur.obj"
	-@erase "$(INTDIR)\ichthyosaur.sbr"
	-@erase "$(INTDIR)\islave.obj"
	-@erase "$(INTDIR)\islave.sbr"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\items.sbr"
	-@erase "$(INTDIR)\items_new.obj"
	-@erase "$(INTDIR)\items_new.sbr"
	-@erase "$(INTDIR)\leech.obj"
	-@erase "$(INTDIR)\leech.sbr"
	-@erase "$(INTDIR)\lgrenade.obj"
	-@erase "$(INTDIR)\lgrenade.sbr"
	-@erase "$(INTDIR)\lightp.obj"
	-@erase "$(INTDIR)\lightp.sbr"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\lights.sbr"
	-@erase "$(INTDIR)\lms_gamerules.obj"
	-@erase "$(INTDIR)\lms_gamerules.sbr"
	-@erase "$(INTDIR)\mapcycle.obj"
	-@erase "$(INTDIR)\mapcycle.sbr"
	-@erase "$(INTDIR)\maprules.obj"
	-@erase "$(INTDIR)\maprules.sbr"
	-@erase "$(INTDIR)\monstermaker.obj"
	-@erase "$(INTDIR)\monstermaker.sbr"
	-@erase "$(INTDIR)\monsters.obj"
	-@erase "$(INTDIR)\monsters.sbr"
	-@erase "$(INTDIR)\monsterstate.obj"
	-@erase "$(INTDIR)\monsterstate.sbr"
	-@erase "$(INTDIR)\mortar.obj"
	-@erase "$(INTDIR)\mortar.sbr"
	-@erase "$(INTDIR)\mp5.obj"
	-@erase "$(INTDIR)\mp5.sbr"
	-@erase "$(INTDIR)\mtarget.obj"
	-@erase "$(INTDIR)\mtarget.sbr"
	-@erase "$(INTDIR)\multiplay_gamerules.obj"
	-@erase "$(INTDIR)\multiplay_gamerules.sbr"
	-@erase "$(INTDIR)\nanosword.obj"
	-@erase "$(INTDIR)\nanosword.sbr"
	-@erase "$(INTDIR)\nihilanth.obj"
	-@erase "$(INTDIR)\nihilanth.sbr"
	-@erase "$(INTDIR)\nodes.obj"
	-@erase "$(INTDIR)\nodes.sbr"
	-@erase "$(INTDIR)\observer.obj"
	-@erase "$(INTDIR)\observer.sbr"
	-@erase "$(INTDIR)\orb.obj"
	-@erase "$(INTDIR)\orb.sbr"
	-@erase "$(INTDIR)\orb_proj.obj"
	-@erase "$(INTDIR)\orb_proj.sbr"
	-@erase "$(INTDIR)\osprey.obj"
	-@erase "$(INTDIR)\osprey.sbr"
	-@erase "$(INTDIR)\pathcorner.obj"
	-@erase "$(INTDIR)\pathcorner.sbr"
	-@erase "$(INTDIR)\plane.obj"
	-@erase "$(INTDIR)\plane.sbr"
	-@erase "$(INTDIR)\plasma.obj"
	-@erase "$(INTDIR)\plasma.sbr"
	-@erase "$(INTDIR)\plasmaball.obj"
	-@erase "$(INTDIR)\plasmaball.sbr"
	-@erase "$(INTDIR)\plats.obj"
	-@erase "$(INTDIR)\plats.sbr"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\player.sbr"
	-@erase "$(INTDIR)\playermonster.obj"
	-@erase "$(INTDIR)\playermonster.sbr"
	-@erase "$(INTDIR)\pm_debug.obj"
	-@erase "$(INTDIR)\pm_debug.sbr"
	-@erase "$(INTDIR)\pm_math.obj"
	-@erase "$(INTDIR)\pm_math.sbr"
	-@erase "$(INTDIR)\pm_shared.obj"
	-@erase "$(INTDIR)\pm_shared.sbr"
	-@erase "$(INTDIR)\python.obj"
	-@erase "$(INTDIR)\python.sbr"
	-@erase "$(INTDIR)\rat.obj"
	-@erase "$(INTDIR)\rat.sbr"
	-@erase "$(INTDIR)\roach.obj"
	-@erase "$(INTDIR)\roach.sbr"
	-@erase "$(INTDIR)\rpg.obj"
	-@erase "$(INTDIR)\rpg.sbr"
	-@erase "$(INTDIR)\rpgrocket.obj"
	-@erase "$(INTDIR)\rpgrocket.sbr"
	-@erase "$(INTDIR)\satchel.obj"
	-@erase "$(INTDIR)\satchel.sbr"
	-@erase "$(INTDIR)\satchelcharge.obj"
	-@erase "$(INTDIR)\satchelcharge.sbr"
	-@erase "$(INTDIR)\saverestore.obj"
	-@erase "$(INTDIR)\saverestore.sbr"
	-@erase "$(INTDIR)\scientist.obj"
	-@erase "$(INTDIR)\scientist.sbr"
	-@erase "$(INTDIR)\scripted.obj"
	-@erase "$(INTDIR)\scripted.sbr"
	-@erase "$(INTDIR)\shotgun.obj"
	-@erase "$(INTDIR)\shotgun.sbr"
	-@erase "$(INTDIR)\singleplay_gamerules.obj"
	-@erase "$(INTDIR)\singleplay_gamerules.sbr"
	-@erase "$(INTDIR)\skill.obj"
	-@erase "$(INTDIR)\skill.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\soundent.obj"
	-@erase "$(INTDIR)\soundent.sbr"
	-@erase "$(INTDIR)\spectator.obj"
	-@erase "$(INTDIR)\spectator.sbr"
	-@erase "$(INTDIR)\sqkgrenade.obj"
	-@erase "$(INTDIR)\sqkgrenade.sbr"
	-@erase "$(INTDIR)\squadmonster.obj"
	-@erase "$(INTDIR)\squadmonster.sbr"
	-@erase "$(INTDIR)\squeakgrenade.obj"
	-@erase "$(INTDIR)\squeakgrenade.sbr"
	-@erase "$(INTDIR)\strtarget.obj"
	-@erase "$(INTDIR)\strtarget.sbr"
	-@erase "$(INTDIR)\subs.obj"
	-@erase "$(INTDIR)\subs.sbr"
	-@erase "$(INTDIR)\sword.obj"
	-@erase "$(INTDIR)\sword.sbr"
	-@erase "$(INTDIR)\talkmonster.obj"
	-@erase "$(INTDIR)\talkmonster.sbr"
	-@erase "$(INTDIR)\teamplay_gamerules.obj"
	-@erase "$(INTDIR)\teamplay_gamerules.sbr"
	-@erase "$(INTDIR)\teleporter.obj"
	-@erase "$(INTDIR)\teleporter.sbr"
	-@erase "$(INTDIR)\tentacle.obj"
	-@erase "$(INTDIR)\tentacle.sbr"
	-@erase "$(INTDIR)\triggers.obj"
	-@erase "$(INTDIR)\triggers.sbr"
	-@erase "$(INTDIR)\triggers_new.obj"
	-@erase "$(INTDIR)\triggers_new.sbr"
	-@erase "$(INTDIR)\tripmine.obj"
	-@erase "$(INTDIR)\tripmine.sbr"
	-@erase "$(INTDIR)\tripminegrenade.obj"
	-@erase "$(INTDIR)\tripminegrenade.sbr"
	-@erase "$(INTDIR)\turret.obj"
	-@erase "$(INTDIR)\turret.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\util_vector.obj"
	-@erase "$(INTDIR)\util_vector.sbr"
	-@erase "$(INTDIR)\util_xhl.obj"
	-@erase "$(INTDIR)\util_xhl.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vector.obj"
	-@erase "$(INTDIR)\vector.sbr"
	-@erase "$(INTDIR)\voice_banmgr.obj"
	-@erase "$(INTDIR)\voice_banmgr.sbr"
	-@erase "$(INTDIR)\voice_gamemgr.obj"
	-@erase "$(INTDIR)\voice_gamemgr.sbr"
	-@erase "$(INTDIR)\weaponbox.obj"
	-@erase "$(INTDIR)\weaponbox.sbr"
	-@erase "$(INTDIR)\weapons.obj"
	-@erase "$(INTDIR)\weapons.sbr"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\world.sbr"
	-@erase "$(INTDIR)\XDM.res"
	-@erase "$(INTDIR)\xen.obj"
	-@erase "$(INTDIR)\xen.sbr"
	-@erase "$(INTDIR)\zombie.obj"
	-@erase "$(INTDIR)\zombie.sbr"
	-@erase "$(OUTDIR)\intermediate\XDM_i686.map"
	-@erase "$(OUTDIR)\XDM.bsc"
	-@erase "$(OUTDIR)\XDM_i686.dll"
	-@erase "$(OUTDIR)\XDM_i686.exp"
	-@erase "$(OUTDIR)\XDM_i686.ilk"
	-@erase "XDM_i686"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /Zi /I "bot" /I "gamerules" /I "projectiles" /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\game_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\XDM.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\XDM.res" /i ".\res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\XDM.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\pm_debug.sbr" \
	"$(INTDIR)\pm_math.sbr" \
	"$(INTDIR)\pm_shared.sbr" \
	"$(INTDIR)\alauncher.sbr" \
	"$(INTDIR)\beamrifle.sbr" \
	"$(INTDIR)\chemgun.sbr" \
	"$(INTDIR)\crossbow.sbr" \
	"$(INTDIR)\crowbar.sbr" \
	"$(INTDIR)\displacer.sbr" \
	"$(INTDIR)\egon.sbr" \
	"$(INTDIR)\flamethrower.sbr" \
	"$(INTDIR)\gauss.sbr" \
	"$(INTDIR)\glauncher.sbr" \
	"$(INTDIR)\glock.sbr" \
	"$(INTDIR)\handgrenade.sbr" \
	"$(INTDIR)\hornetgun.sbr" \
	"$(INTDIR)\mp5.sbr" \
	"$(INTDIR)\nanosword.sbr" \
	"$(INTDIR)\orb.sbr" \
	"$(INTDIR)\plasma.sbr" \
	"$(INTDIR)\python.sbr" \
	"$(INTDIR)\rpg.sbr" \
	"$(INTDIR)\satchel.sbr" \
	"$(INTDIR)\shotgun.sbr" \
	"$(INTDIR)\squeakgrenade.sbr" \
	"$(INTDIR)\strtarget.sbr" \
	"$(INTDIR)\sword.sbr" \
	"$(INTDIR)\tripmine.sbr" \
	"$(INTDIR)\aflock.sbr" \
	"$(INTDIR)\agrunt.sbr" \
	"$(INTDIR)\apache.sbr" \
	"$(INTDIR)\barnacle.sbr" \
	"$(INTDIR)\barney.sbr" \
	"$(INTDIR)\bigmomma.sbr" \
	"$(INTDIR)\bloater.sbr" \
	"$(INTDIR)\bullsquid.sbr" \
	"$(INTDIR)\cat.sbr" \
	"$(INTDIR)\controller.sbr" \
	"$(INTDIR)\deadhev.sbr" \
	"$(INTDIR)\diablo.sbr" \
	"$(INTDIR)\flyingmonster.sbr" \
	"$(INTDIR)\gargantua.sbr" \
	"$(INTDIR)\genericmonster.sbr" \
	"$(INTDIR)\gman.sbr" \
	"$(INTDIR)\hassassin.sbr" \
	"$(INTDIR)\headcrab.sbr" \
	"$(INTDIR)\hgrunt.sbr" \
	"$(INTDIR)\houndeye.sbr" \
	"$(INTDIR)\ichthyosaur.sbr" \
	"$(INTDIR)\islave.sbr" \
	"$(INTDIR)\leech.sbr" \
	"$(INTDIR)\monsters.sbr" \
	"$(INTDIR)\monsterstate.sbr" \
	"$(INTDIR)\nihilanth.sbr" \
	"$(INTDIR)\osprey.sbr" \
	"$(INTDIR)\playermonster.sbr" \
	"$(INTDIR)\rat.sbr" \
	"$(INTDIR)\roach.sbr" \
	"$(INTDIR)\scientist.sbr" \
	"$(INTDIR)\squadmonster.sbr" \
	"$(INTDIR)\talkmonster.sbr" \
	"$(INTDIR)\tentacle.sbr" \
	"$(INTDIR)\turret.sbr" \
	"$(INTDIR)\zombie.sbr" \
	"$(INTDIR)\airtank.sbr" \
	"$(INTDIR)\healthkit.sbr" \
	"$(INTDIR)\items.sbr" \
	"$(INTDIR)\items_new.sbr" \
	"$(INTDIR)\coop_gamerules.sbr" \
	"$(INTDIR)\ctf_gamerules.sbr" \
	"$(INTDIR)\dom_gamerules.sbr" \
	"$(INTDIR)\gamerules.sbr" \
	"$(INTDIR)\lms_gamerules.sbr" \
	"$(INTDIR)\maprules.sbr" \
	"$(INTDIR)\multiplay_gamerules.sbr" \
	"$(INTDIR)\singleplay_gamerules.sbr" \
	"$(INTDIR)\teamplay_gamerules.sbr" \
	"$(INTDIR)\effects.sbr" \
	"$(INTDIR)\effects_new.sbr" \
	"$(INTDIR)\environment.sbr" \
	"$(INTDIR)\explode.sbr" \
	"$(INTDIR)\fx.sbr" \
	"$(INTDIR)\agrenade.sbr" \
	"$(INTDIR)\atomicdevice.sbr" \
	"$(INTDIR)\crossbowbolt.sbr" \
	"$(INTDIR)\flamecloud.sbr" \
	"$(INTDIR)\ggrenade.sbr" \
	"$(INTDIR)\hornet.sbr" \
	"$(INTDIR)\lgrenade.sbr" \
	"$(INTDIR)\lightp.sbr" \
	"$(INTDIR)\mtarget.sbr" \
	"$(INTDIR)\orb_proj.sbr" \
	"$(INTDIR)\plasmaball.sbr" \
	"$(INTDIR)\rpgrocket.sbr" \
	"$(INTDIR)\satchelcharge.sbr" \
	"$(INTDIR)\sqkgrenade.sbr" \
	"$(INTDIR)\teleporter.sbr" \
	"$(INTDIR)\tripminegrenade.sbr" \
	"$(INTDIR)\voice_banmgr.sbr" \
	"$(INTDIR)\voice_gamemgr.sbr" \
	"$(INTDIR)\ai_basenpc_schedule.sbr" \
	"$(INTDIR)\ammo.sbr" \
	"$(INTDIR)\animating.sbr" \
	"$(INTDIR)\animation.sbr" \
	"$(INTDIR)\bmodels.sbr" \
	"$(INTDIR)\buttons.sbr" \
	"$(INTDIR)\cbase.sbr" \
	"$(INTDIR)\chasecam.sbr" \
	"$(INTDIR)\client.sbr" \
	"$(INTDIR)\combat.sbr" \
	"$(INTDIR)\defaultai.sbr" \
	"$(INTDIR)\doors.sbr" \
	"$(INTDIR)\entconfig.sbr" \
	"$(INTDIR)\func_break.sbr" \
	"$(INTDIR)\func_tank.sbr" \
	"$(INTDIR)\game.sbr" \
	"$(INTDIR)\gibs.sbr" \
	"$(INTDIR)\globals.sbr" \
	"$(INTDIR)\h_ai.sbr" \
	"$(INTDIR)\h_battery.sbr" \
	"$(INTDIR)\h_cycler.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\lights.sbr" \
	"$(INTDIR)\mapcycle.sbr" \
	"$(INTDIR)\monstermaker.sbr" \
	"$(INTDIR)\mortar.sbr" \
	"$(INTDIR)\nodes.sbr" \
	"$(INTDIR)\observer.sbr" \
	"$(INTDIR)\pathcorner.sbr" \
	"$(INTDIR)\plane.sbr" \
	"$(INTDIR)\plats.sbr" \
	"$(INTDIR)\player.sbr" \
	"$(INTDIR)\saverestore.sbr" \
	"$(INTDIR)\scripted.sbr" \
	"$(INTDIR)\skill.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\soundent.sbr" \
	"$(INTDIR)\spectator.sbr" \
	"$(INTDIR)\subs.sbr" \
	"$(INTDIR)\triggers.sbr" \
	"$(INTDIR)\triggers_new.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\util_xhl.sbr" \
	"$(INTDIR)\weaponbox.sbr" \
	"$(INTDIR)\weapons.sbr" \
	"$(INTDIR)\world.sbr" \
	"$(INTDIR)\xen.sbr" \
	"$(INTDIR)\util_vector.sbr" \
	"$(INTDIR)\vector.sbr"

"$(OUTDIR)\XDM.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /version:3.0 /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\XDM_i686.pdb" /map:"$(INTDIR)\XDM_i686.map" /machine:I386 /def:".\XDM.def" /out:"$(OUTDIR)\XDM_i686.dll" /implib:"$(OUTDIR)\XDM_i686.lib" 
DEF_FILE= \
	".\XDM.def"
LINK32_OBJS= \
	"$(INTDIR)\pm_debug.obj" \
	"$(INTDIR)\pm_math.obj" \
	"$(INTDIR)\pm_shared.obj" \
	"$(INTDIR)\alauncher.obj" \
	"$(INTDIR)\beamrifle.obj" \
	"$(INTDIR)\chemgun.obj" \
	"$(INTDIR)\crossbow.obj" \
	"$(INTDIR)\crowbar.obj" \
	"$(INTDIR)\displacer.obj" \
	"$(INTDIR)\egon.obj" \
	"$(INTDIR)\flamethrower.obj" \
	"$(INTDIR)\gauss.obj" \
	"$(INTDIR)\glauncher.obj" \
	"$(INTDIR)\glock.obj" \
	"$(INTDIR)\handgrenade.obj" \
	"$(INTDIR)\hornetgun.obj" \
	"$(INTDIR)\mp5.obj" \
	"$(INTDIR)\nanosword.obj" \
	"$(INTDIR)\orb.obj" \
	"$(INTDIR)\plasma.obj" \
	"$(INTDIR)\python.obj" \
	"$(INTDIR)\rpg.obj" \
	"$(INTDIR)\satchel.obj" \
	"$(INTDIR)\shotgun.obj" \
	"$(INTDIR)\squeakgrenade.obj" \
	"$(INTDIR)\strtarget.obj" \
	"$(INTDIR)\sword.obj" \
	"$(INTDIR)\tripmine.obj" \
	"$(INTDIR)\aflock.obj" \
	"$(INTDIR)\agrunt.obj" \
	"$(INTDIR)\apache.obj" \
	"$(INTDIR)\barnacle.obj" \
	"$(INTDIR)\barney.obj" \
	"$(INTDIR)\bigmomma.obj" \
	"$(INTDIR)\bloater.obj" \
	"$(INTDIR)\bullsquid.obj" \
	"$(INTDIR)\cat.obj" \
	"$(INTDIR)\controller.obj" \
	"$(INTDIR)\deadhev.obj" \
	"$(INTDIR)\diablo.obj" \
	"$(INTDIR)\flyingmonster.obj" \
	"$(INTDIR)\gargantua.obj" \
	"$(INTDIR)\genericmonster.obj" \
	"$(INTDIR)\gman.obj" \
	"$(INTDIR)\hassassin.obj" \
	"$(INTDIR)\headcrab.obj" \
	"$(INTDIR)\hgrunt.obj" \
	"$(INTDIR)\houndeye.obj" \
	"$(INTDIR)\ichthyosaur.obj" \
	"$(INTDIR)\islave.obj" \
	"$(INTDIR)\leech.obj" \
	"$(INTDIR)\monsters.obj" \
	"$(INTDIR)\monsterstate.obj" \
	"$(INTDIR)\nihilanth.obj" \
	"$(INTDIR)\osprey.obj" \
	"$(INTDIR)\playermonster.obj" \
	"$(INTDIR)\rat.obj" \
	"$(INTDIR)\roach.obj" \
	"$(INTDIR)\scientist.obj" \
	"$(INTDIR)\squadmonster.obj" \
	"$(INTDIR)\talkmonster.obj" \
	"$(INTDIR)\tentacle.obj" \
	"$(INTDIR)\turret.obj" \
	"$(INTDIR)\zombie.obj" \
	"$(INTDIR)\airtank.obj" \
	"$(INTDIR)\healthkit.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\items_new.obj" \
	"$(INTDIR)\coop_gamerules.obj" \
	"$(INTDIR)\ctf_gamerules.obj" \
	"$(INTDIR)\dom_gamerules.obj" \
	"$(INTDIR)\gamerules.obj" \
	"$(INTDIR)\lms_gamerules.obj" \
	"$(INTDIR)\maprules.obj" \
	"$(INTDIR)\multiplay_gamerules.obj" \
	"$(INTDIR)\singleplay_gamerules.obj" \
	"$(INTDIR)\teamplay_gamerules.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\effects_new.obj" \
	"$(INTDIR)\environment.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\fx.obj" \
	"$(INTDIR)\agrenade.obj" \
	"$(INTDIR)\atomicdevice.obj" \
	"$(INTDIR)\crossbowbolt.obj" \
	"$(INTDIR)\flamecloud.obj" \
	"$(INTDIR)\ggrenade.obj" \
	"$(INTDIR)\hornet.obj" \
	"$(INTDIR)\lgrenade.obj" \
	"$(INTDIR)\lightp.obj" \
	"$(INTDIR)\mtarget.obj" \
	"$(INTDIR)\orb_proj.obj" \
	"$(INTDIR)\plasmaball.obj" \
	"$(INTDIR)\rpgrocket.obj" \
	"$(INTDIR)\satchelcharge.obj" \
	"$(INTDIR)\sqkgrenade.obj" \
	"$(INTDIR)\teleporter.obj" \
	"$(INTDIR)\tripminegrenade.obj" \
	"$(INTDIR)\voice_banmgr.obj" \
	"$(INTDIR)\voice_gamemgr.obj" \
	"$(INTDIR)\ai_basenpc_schedule.obj" \
	"$(INTDIR)\ammo.obj" \
	"$(INTDIR)\animating.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\bmodels.obj" \
	"$(INTDIR)\buttons.obj" \
	"$(INTDIR)\cbase.obj" \
	"$(INTDIR)\chasecam.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\defaultai.obj" \
	"$(INTDIR)\doors.obj" \
	"$(INTDIR)\entconfig.obj" \
	"$(INTDIR)\func_break.obj" \
	"$(INTDIR)\func_tank.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\gibs.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\h_ai.obj" \
	"$(INTDIR)\h_battery.obj" \
	"$(INTDIR)\h_cycler.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\mapcycle.obj" \
	"$(INTDIR)\monstermaker.obj" \
	"$(INTDIR)\mortar.obj" \
	"$(INTDIR)\nodes.obj" \
	"$(INTDIR)\observer.obj" \
	"$(INTDIR)\pathcorner.obj" \
	"$(INTDIR)\plane.obj" \
	"$(INTDIR)\plats.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\saverestore.obj" \
	"$(INTDIR)\scripted.obj" \
	"$(INTDIR)\skill.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\soundent.obj" \
	"$(INTDIR)\spectator.obj" \
	"$(INTDIR)\subs.obj" \
	"$(INTDIR)\triggers.obj" \
	"$(INTDIR)\triggers_new.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\util_xhl.obj" \
	"$(INTDIR)\weaponbox.obj" \
	"$(INTDIR)\weapons.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\xen.obj" \
	"$(INTDIR)\XDM.res" \
	"$(INTDIR)\util_vector.obj" \
	"$(INTDIR)\vector.obj"

"$(OUTDIR)\XDM_i686.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\release\XDM_i686.dll
InputPath=.\release\XDM_i686.dll
InputName=XDM_i686
SOURCE="$(InputPath)"

".\XDM_i686" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	signtool  sign /a "$(TargetPath)" 
	copy          $(TargetPath)          \Half-Life\XDM\dlls\  
	copy          $(TargetPath)          F:\games\HL1121\XDM\dlls\  
	copy  $(TargetPath)  W:\dev\Half-Life\XDM\dlls\  
	
<< 
	

!ELSEIF  "$(CFG)" == "XDM - Win32 Profile"

OUTDIR=.\profile
INTDIR=.\profile/intermediate
# Begin Custom Macros
OutDir=.\profile
# End Custom Macros

ALL : ".\release\XDM_i686.dll" "$(OUTDIR)\XDM.bsc" ".\XDM_i686"


CLEAN :
	-@erase "$(INTDIR)\aflock.obj"
	-@erase "$(INTDIR)\aflock.sbr"
	-@erase "$(INTDIR)\agrenade.obj"
	-@erase "$(INTDIR)\agrenade.sbr"
	-@erase "$(INTDIR)\agrunt.obj"
	-@erase "$(INTDIR)\agrunt.sbr"
	-@erase "$(INTDIR)\ai_basenpc_schedule.obj"
	-@erase "$(INTDIR)\ai_basenpc_schedule.sbr"
	-@erase "$(INTDIR)\airtank.obj"
	-@erase "$(INTDIR)\airtank.sbr"
	-@erase "$(INTDIR)\alauncher.obj"
	-@erase "$(INTDIR)\alauncher.sbr"
	-@erase "$(INTDIR)\ammo.obj"
	-@erase "$(INTDIR)\ammo.sbr"
	-@erase "$(INTDIR)\animating.obj"
	-@erase "$(INTDIR)\animating.sbr"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\animation.sbr"
	-@erase "$(INTDIR)\apache.obj"
	-@erase "$(INTDIR)\apache.sbr"
	-@erase "$(INTDIR)\atomicdevice.obj"
	-@erase "$(INTDIR)\atomicdevice.sbr"
	-@erase "$(INTDIR)\barnacle.obj"
	-@erase "$(INTDIR)\barnacle.sbr"
	-@erase "$(INTDIR)\barney.obj"
	-@erase "$(INTDIR)\barney.sbr"
	-@erase "$(INTDIR)\beamrifle.obj"
	-@erase "$(INTDIR)\beamrifle.sbr"
	-@erase "$(INTDIR)\bigmomma.obj"
	-@erase "$(INTDIR)\bigmomma.sbr"
	-@erase "$(INTDIR)\bloater.obj"
	-@erase "$(INTDIR)\bloater.sbr"
	-@erase "$(INTDIR)\bmodels.obj"
	-@erase "$(INTDIR)\bmodels.sbr"
	-@erase "$(INTDIR)\bullsquid.obj"
	-@erase "$(INTDIR)\bullsquid.sbr"
	-@erase "$(INTDIR)\buttons.obj"
	-@erase "$(INTDIR)\buttons.sbr"
	-@erase "$(INTDIR)\cat.obj"
	-@erase "$(INTDIR)\cat.sbr"
	-@erase "$(INTDIR)\cbase.obj"
	-@erase "$(INTDIR)\cbase.sbr"
	-@erase "$(INTDIR)\chasecam.obj"
	-@erase "$(INTDIR)\chasecam.sbr"
	-@erase "$(INTDIR)\chemgun.obj"
	-@erase "$(INTDIR)\chemgun.sbr"
	-@erase "$(INTDIR)\client.obj"
	-@erase "$(INTDIR)\client.sbr"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\combat.sbr"
	-@erase "$(INTDIR)\controller.obj"
	-@erase "$(INTDIR)\controller.sbr"
	-@erase "$(INTDIR)\coop_gamerules.obj"
	-@erase "$(INTDIR)\coop_gamerules.sbr"
	-@erase "$(INTDIR)\crossbow.obj"
	-@erase "$(INTDIR)\crossbow.sbr"
	-@erase "$(INTDIR)\crossbowbolt.obj"
	-@erase "$(INTDIR)\crossbowbolt.sbr"
	-@erase "$(INTDIR)\crowbar.obj"
	-@erase "$(INTDIR)\crowbar.sbr"
	-@erase "$(INTDIR)\ctf_gamerules.obj"
	-@erase "$(INTDIR)\ctf_gamerules.sbr"
	-@erase "$(INTDIR)\deadhev.obj"
	-@erase "$(INTDIR)\deadhev.sbr"
	-@erase "$(INTDIR)\defaultai.obj"
	-@erase "$(INTDIR)\defaultai.sbr"
	-@erase "$(INTDIR)\diablo.obj"
	-@erase "$(INTDIR)\diablo.sbr"
	-@erase "$(INTDIR)\displacer.obj"
	-@erase "$(INTDIR)\displacer.sbr"
	-@erase "$(INTDIR)\dom_gamerules.obj"
	-@erase "$(INTDIR)\dom_gamerules.sbr"
	-@erase "$(INTDIR)\doors.obj"
	-@erase "$(INTDIR)\doors.sbr"
	-@erase "$(INTDIR)\effects.obj"
	-@erase "$(INTDIR)\effects.sbr"
	-@erase "$(INTDIR)\effects_new.obj"
	-@erase "$(INTDIR)\effects_new.sbr"
	-@erase "$(INTDIR)\egon.obj"
	-@erase "$(INTDIR)\egon.sbr"
	-@erase "$(INTDIR)\entconfig.obj"
	-@erase "$(INTDIR)\entconfig.sbr"
	-@erase "$(INTDIR)\environment.obj"
	-@erase "$(INTDIR)\environment.sbr"
	-@erase "$(INTDIR)\explode.obj"
	-@erase "$(INTDIR)\explode.sbr"
	-@erase "$(INTDIR)\flamecloud.obj"
	-@erase "$(INTDIR)\flamecloud.sbr"
	-@erase "$(INTDIR)\flamethrower.obj"
	-@erase "$(INTDIR)\flamethrower.sbr"
	-@erase "$(INTDIR)\flyingmonster.obj"
	-@erase "$(INTDIR)\flyingmonster.sbr"
	-@erase "$(INTDIR)\func_break.obj"
	-@erase "$(INTDIR)\func_break.sbr"
	-@erase "$(INTDIR)\func_tank.obj"
	-@erase "$(INTDIR)\func_tank.sbr"
	-@erase "$(INTDIR)\fx.obj"
	-@erase "$(INTDIR)\fx.sbr"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\game.sbr"
	-@erase "$(INTDIR)\gamerules.obj"
	-@erase "$(INTDIR)\gamerules.sbr"
	-@erase "$(INTDIR)\gargantua.obj"
	-@erase "$(INTDIR)\gargantua.sbr"
	-@erase "$(INTDIR)\gauss.obj"
	-@erase "$(INTDIR)\gauss.sbr"
	-@erase "$(INTDIR)\genericmonster.obj"
	-@erase "$(INTDIR)\genericmonster.sbr"
	-@erase "$(INTDIR)\ggrenade.obj"
	-@erase "$(INTDIR)\ggrenade.sbr"
	-@erase "$(INTDIR)\gibs.obj"
	-@erase "$(INTDIR)\gibs.sbr"
	-@erase "$(INTDIR)\glauncher.obj"
	-@erase "$(INTDIR)\glauncher.sbr"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\globals.sbr"
	-@erase "$(INTDIR)\glock.obj"
	-@erase "$(INTDIR)\glock.sbr"
	-@erase "$(INTDIR)\gman.obj"
	-@erase "$(INTDIR)\gman.sbr"
	-@erase "$(INTDIR)\h_ai.obj"
	-@erase "$(INTDIR)\h_ai.sbr"
	-@erase "$(INTDIR)\h_battery.obj"
	-@erase "$(INTDIR)\h_battery.sbr"
	-@erase "$(INTDIR)\h_cycler.obj"
	-@erase "$(INTDIR)\h_cycler.sbr"
	-@erase "$(INTDIR)\h_export.obj"
	-@erase "$(INTDIR)\h_export.sbr"
	-@erase "$(INTDIR)\handgrenade.obj"
	-@erase "$(INTDIR)\handgrenade.sbr"
	-@erase "$(INTDIR)\hassassin.obj"
	-@erase "$(INTDIR)\hassassin.sbr"
	-@erase "$(INTDIR)\headcrab.obj"
	-@erase "$(INTDIR)\headcrab.sbr"
	-@erase "$(INTDIR)\healthkit.obj"
	-@erase "$(INTDIR)\healthkit.sbr"
	-@erase "$(INTDIR)\hgrunt.obj"
	-@erase "$(INTDIR)\hgrunt.sbr"
	-@erase "$(INTDIR)\hornet.obj"
	-@erase "$(INTDIR)\hornet.sbr"
	-@erase "$(INTDIR)\hornetgun.obj"
	-@erase "$(INTDIR)\hornetgun.sbr"
	-@erase "$(INTDIR)\houndeye.obj"
	-@erase "$(INTDIR)\houndeye.sbr"
	-@erase "$(INTDIR)\ichthyosaur.obj"
	-@erase "$(INTDIR)\ichthyosaur.sbr"
	-@erase "$(INTDIR)\islave.obj"
	-@erase "$(INTDIR)\islave.sbr"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\items.sbr"
	-@erase "$(INTDIR)\items_new.obj"
	-@erase "$(INTDIR)\items_new.sbr"
	-@erase "$(INTDIR)\leech.obj"
	-@erase "$(INTDIR)\leech.sbr"
	-@erase "$(INTDIR)\lgrenade.obj"
	-@erase "$(INTDIR)\lgrenade.sbr"
	-@erase "$(INTDIR)\lightp.obj"
	-@erase "$(INTDIR)\lightp.sbr"
	-@erase "$(INTDIR)\lights.obj"
	-@erase "$(INTDIR)\lights.sbr"
	-@erase "$(INTDIR)\lms_gamerules.obj"
	-@erase "$(INTDIR)\lms_gamerules.sbr"
	-@erase "$(INTDIR)\mapcycle.obj"
	-@erase "$(INTDIR)\mapcycle.sbr"
	-@erase "$(INTDIR)\maprules.obj"
	-@erase "$(INTDIR)\maprules.sbr"
	-@erase "$(INTDIR)\monstermaker.obj"
	-@erase "$(INTDIR)\monstermaker.sbr"
	-@erase "$(INTDIR)\monsters.obj"
	-@erase "$(INTDIR)\monsters.sbr"
	-@erase "$(INTDIR)\monsterstate.obj"
	-@erase "$(INTDIR)\monsterstate.sbr"
	-@erase "$(INTDIR)\mortar.obj"
	-@erase "$(INTDIR)\mortar.sbr"
	-@erase "$(INTDIR)\mp5.obj"
	-@erase "$(INTDIR)\mp5.sbr"
	-@erase "$(INTDIR)\mtarget.obj"
	-@erase "$(INTDIR)\mtarget.sbr"
	-@erase "$(INTDIR)\multiplay_gamerules.obj"
	-@erase "$(INTDIR)\multiplay_gamerules.sbr"
	-@erase "$(INTDIR)\nanosword.obj"
	-@erase "$(INTDIR)\nanosword.sbr"
	-@erase "$(INTDIR)\nihilanth.obj"
	-@erase "$(INTDIR)\nihilanth.sbr"
	-@erase "$(INTDIR)\nodes.obj"
	-@erase "$(INTDIR)\nodes.sbr"
	-@erase "$(INTDIR)\observer.obj"
	-@erase "$(INTDIR)\observer.sbr"
	-@erase "$(INTDIR)\orb.obj"
	-@erase "$(INTDIR)\orb.sbr"
	-@erase "$(INTDIR)\orb_proj.obj"
	-@erase "$(INTDIR)\orb_proj.sbr"
	-@erase "$(INTDIR)\osprey.obj"
	-@erase "$(INTDIR)\osprey.sbr"
	-@erase "$(INTDIR)\pathcorner.obj"
	-@erase "$(INTDIR)\pathcorner.sbr"
	-@erase "$(INTDIR)\plane.obj"
	-@erase "$(INTDIR)\plane.sbr"
	-@erase "$(INTDIR)\plasma.obj"
	-@erase "$(INTDIR)\plasma.sbr"
	-@erase "$(INTDIR)\plasmaball.obj"
	-@erase "$(INTDIR)\plasmaball.sbr"
	-@erase "$(INTDIR)\plats.obj"
	-@erase "$(INTDIR)\plats.sbr"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\player.sbr"
	-@erase "$(INTDIR)\playermonster.obj"
	-@erase "$(INTDIR)\playermonster.sbr"
	-@erase "$(INTDIR)\pm_debug.obj"
	-@erase "$(INTDIR)\pm_debug.sbr"
	-@erase "$(INTDIR)\pm_math.obj"
	-@erase "$(INTDIR)\pm_math.sbr"
	-@erase "$(INTDIR)\pm_shared.obj"
	-@erase "$(INTDIR)\pm_shared.sbr"
	-@erase "$(INTDIR)\python.obj"
	-@erase "$(INTDIR)\python.sbr"
	-@erase "$(INTDIR)\rat.obj"
	-@erase "$(INTDIR)\rat.sbr"
	-@erase "$(INTDIR)\roach.obj"
	-@erase "$(INTDIR)\roach.sbr"
	-@erase "$(INTDIR)\rpg.obj"
	-@erase "$(INTDIR)\rpg.sbr"
	-@erase "$(INTDIR)\rpgrocket.obj"
	-@erase "$(INTDIR)\rpgrocket.sbr"
	-@erase "$(INTDIR)\satchel.obj"
	-@erase "$(INTDIR)\satchel.sbr"
	-@erase "$(INTDIR)\satchelcharge.obj"
	-@erase "$(INTDIR)\satchelcharge.sbr"
	-@erase "$(INTDIR)\saverestore.obj"
	-@erase "$(INTDIR)\saverestore.sbr"
	-@erase "$(INTDIR)\scientist.obj"
	-@erase "$(INTDIR)\scientist.sbr"
	-@erase "$(INTDIR)\scripted.obj"
	-@erase "$(INTDIR)\scripted.sbr"
	-@erase "$(INTDIR)\shotgun.obj"
	-@erase "$(INTDIR)\shotgun.sbr"
	-@erase "$(INTDIR)\singleplay_gamerules.obj"
	-@erase "$(INTDIR)\singleplay_gamerules.sbr"
	-@erase "$(INTDIR)\skill.obj"
	-@erase "$(INTDIR)\skill.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\soundent.obj"
	-@erase "$(INTDIR)\soundent.sbr"
	-@erase "$(INTDIR)\spectator.obj"
	-@erase "$(INTDIR)\spectator.sbr"
	-@erase "$(INTDIR)\sqkgrenade.obj"
	-@erase "$(INTDIR)\sqkgrenade.sbr"
	-@erase "$(INTDIR)\squadmonster.obj"
	-@erase "$(INTDIR)\squadmonster.sbr"
	-@erase "$(INTDIR)\squeakgrenade.obj"
	-@erase "$(INTDIR)\squeakgrenade.sbr"
	-@erase "$(INTDIR)\strtarget.obj"
	-@erase "$(INTDIR)\strtarget.sbr"
	-@erase "$(INTDIR)\subs.obj"
	-@erase "$(INTDIR)\subs.sbr"
	-@erase "$(INTDIR)\sword.obj"
	-@erase "$(INTDIR)\sword.sbr"
	-@erase "$(INTDIR)\talkmonster.obj"
	-@erase "$(INTDIR)\talkmonster.sbr"
	-@erase "$(INTDIR)\teamplay_gamerules.obj"
	-@erase "$(INTDIR)\teamplay_gamerules.sbr"
	-@erase "$(INTDIR)\teleporter.obj"
	-@erase "$(INTDIR)\teleporter.sbr"
	-@erase "$(INTDIR)\tentacle.obj"
	-@erase "$(INTDIR)\tentacle.sbr"
	-@erase "$(INTDIR)\triggers.obj"
	-@erase "$(INTDIR)\triggers.sbr"
	-@erase "$(INTDIR)\triggers_new.obj"
	-@erase "$(INTDIR)\triggers_new.sbr"
	-@erase "$(INTDIR)\tripmine.obj"
	-@erase "$(INTDIR)\tripmine.sbr"
	-@erase "$(INTDIR)\tripminegrenade.obj"
	-@erase "$(INTDIR)\tripminegrenade.sbr"
	-@erase "$(INTDIR)\turret.obj"
	-@erase "$(INTDIR)\turret.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\util_vector.obj"
	-@erase "$(INTDIR)\util_vector.sbr"
	-@erase "$(INTDIR)\util_xhl.obj"
	-@erase "$(INTDIR)\util_xhl.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vector.obj"
	-@erase "$(INTDIR)\vector.sbr"
	-@erase "$(INTDIR)\voice_banmgr.obj"
	-@erase "$(INTDIR)\voice_banmgr.sbr"
	-@erase "$(INTDIR)\voice_gamemgr.obj"
	-@erase "$(INTDIR)\voice_gamemgr.sbr"
	-@erase "$(INTDIR)\weaponbox.obj"
	-@erase "$(INTDIR)\weaponbox.sbr"
	-@erase "$(INTDIR)\weapons.obj"
	-@erase "$(INTDIR)\weapons.sbr"
	-@erase "$(INTDIR)\world.obj"
	-@erase "$(INTDIR)\world.sbr"
	-@erase "$(INTDIR)\XDM.res"
	-@erase "$(INTDIR)\xen.obj"
	-@erase "$(INTDIR)\xen.sbr"
	-@erase "$(INTDIR)\zombie.obj"
	-@erase "$(INTDIR)\zombie.sbr"
	-@erase "$(OUTDIR)\intermediate\XDM_i686.map"
	-@erase "$(OUTDIR)\XDM.bsc"
	-@erase "$(OUTDIR)\XDM_i686.exp"
	-@erase ".\release\XDM_i686.dll"
	-@erase "XDM_i686"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /GX /Zi /I "bot" /I "gamerules" /I "projectiles" /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\game_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\XDM.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\XDM.res" /i ".\res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\XDM.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\pm_debug.sbr" \
	"$(INTDIR)\pm_math.sbr" \
	"$(INTDIR)\pm_shared.sbr" \
	"$(INTDIR)\alauncher.sbr" \
	"$(INTDIR)\beamrifle.sbr" \
	"$(INTDIR)\chemgun.sbr" \
	"$(INTDIR)\crossbow.sbr" \
	"$(INTDIR)\crowbar.sbr" \
	"$(INTDIR)\displacer.sbr" \
	"$(INTDIR)\egon.sbr" \
	"$(INTDIR)\flamethrower.sbr" \
	"$(INTDIR)\gauss.sbr" \
	"$(INTDIR)\glauncher.sbr" \
	"$(INTDIR)\glock.sbr" \
	"$(INTDIR)\handgrenade.sbr" \
	"$(INTDIR)\hornetgun.sbr" \
	"$(INTDIR)\mp5.sbr" \
	"$(INTDIR)\nanosword.sbr" \
	"$(INTDIR)\orb.sbr" \
	"$(INTDIR)\plasma.sbr" \
	"$(INTDIR)\python.sbr" \
	"$(INTDIR)\rpg.sbr" \
	"$(INTDIR)\satchel.sbr" \
	"$(INTDIR)\shotgun.sbr" \
	"$(INTDIR)\squeakgrenade.sbr" \
	"$(INTDIR)\strtarget.sbr" \
	"$(INTDIR)\sword.sbr" \
	"$(INTDIR)\tripmine.sbr" \
	"$(INTDIR)\aflock.sbr" \
	"$(INTDIR)\agrunt.sbr" \
	"$(INTDIR)\apache.sbr" \
	"$(INTDIR)\barnacle.sbr" \
	"$(INTDIR)\barney.sbr" \
	"$(INTDIR)\bigmomma.sbr" \
	"$(INTDIR)\bloater.sbr" \
	"$(INTDIR)\bullsquid.sbr" \
	"$(INTDIR)\cat.sbr" \
	"$(INTDIR)\controller.sbr" \
	"$(INTDIR)\deadhev.sbr" \
	"$(INTDIR)\diablo.sbr" \
	"$(INTDIR)\flyingmonster.sbr" \
	"$(INTDIR)\gargantua.sbr" \
	"$(INTDIR)\genericmonster.sbr" \
	"$(INTDIR)\gman.sbr" \
	"$(INTDIR)\hassassin.sbr" \
	"$(INTDIR)\headcrab.sbr" \
	"$(INTDIR)\hgrunt.sbr" \
	"$(INTDIR)\houndeye.sbr" \
	"$(INTDIR)\ichthyosaur.sbr" \
	"$(INTDIR)\islave.sbr" \
	"$(INTDIR)\leech.sbr" \
	"$(INTDIR)\monsters.sbr" \
	"$(INTDIR)\monsterstate.sbr" \
	"$(INTDIR)\nihilanth.sbr" \
	"$(INTDIR)\osprey.sbr" \
	"$(INTDIR)\playermonster.sbr" \
	"$(INTDIR)\rat.sbr" \
	"$(INTDIR)\roach.sbr" \
	"$(INTDIR)\scientist.sbr" \
	"$(INTDIR)\squadmonster.sbr" \
	"$(INTDIR)\talkmonster.sbr" \
	"$(INTDIR)\tentacle.sbr" \
	"$(INTDIR)\turret.sbr" \
	"$(INTDIR)\zombie.sbr" \
	"$(INTDIR)\airtank.sbr" \
	"$(INTDIR)\healthkit.sbr" \
	"$(INTDIR)\items.sbr" \
	"$(INTDIR)\items_new.sbr" \
	"$(INTDIR)\coop_gamerules.sbr" \
	"$(INTDIR)\ctf_gamerules.sbr" \
	"$(INTDIR)\dom_gamerules.sbr" \
	"$(INTDIR)\gamerules.sbr" \
	"$(INTDIR)\lms_gamerules.sbr" \
	"$(INTDIR)\maprules.sbr" \
	"$(INTDIR)\multiplay_gamerules.sbr" \
	"$(INTDIR)\singleplay_gamerules.sbr" \
	"$(INTDIR)\teamplay_gamerules.sbr" \
	"$(INTDIR)\effects.sbr" \
	"$(INTDIR)\effects_new.sbr" \
	"$(INTDIR)\environment.sbr" \
	"$(INTDIR)\explode.sbr" \
	"$(INTDIR)\fx.sbr" \
	"$(INTDIR)\agrenade.sbr" \
	"$(INTDIR)\atomicdevice.sbr" \
	"$(INTDIR)\crossbowbolt.sbr" \
	"$(INTDIR)\flamecloud.sbr" \
	"$(INTDIR)\ggrenade.sbr" \
	"$(INTDIR)\hornet.sbr" \
	"$(INTDIR)\lgrenade.sbr" \
	"$(INTDIR)\lightp.sbr" \
	"$(INTDIR)\mtarget.sbr" \
	"$(INTDIR)\orb_proj.sbr" \
	"$(INTDIR)\plasmaball.sbr" \
	"$(INTDIR)\rpgrocket.sbr" \
	"$(INTDIR)\satchelcharge.sbr" \
	"$(INTDIR)\sqkgrenade.sbr" \
	"$(INTDIR)\teleporter.sbr" \
	"$(INTDIR)\tripminegrenade.sbr" \
	"$(INTDIR)\voice_banmgr.sbr" \
	"$(INTDIR)\voice_gamemgr.sbr" \
	"$(INTDIR)\ai_basenpc_schedule.sbr" \
	"$(INTDIR)\ammo.sbr" \
	"$(INTDIR)\animating.sbr" \
	"$(INTDIR)\animation.sbr" \
	"$(INTDIR)\bmodels.sbr" \
	"$(INTDIR)\buttons.sbr" \
	"$(INTDIR)\cbase.sbr" \
	"$(INTDIR)\chasecam.sbr" \
	"$(INTDIR)\client.sbr" \
	"$(INTDIR)\combat.sbr" \
	"$(INTDIR)\defaultai.sbr" \
	"$(INTDIR)\doors.sbr" \
	"$(INTDIR)\entconfig.sbr" \
	"$(INTDIR)\func_break.sbr" \
	"$(INTDIR)\func_tank.sbr" \
	"$(INTDIR)\game.sbr" \
	"$(INTDIR)\gibs.sbr" \
	"$(INTDIR)\globals.sbr" \
	"$(INTDIR)\h_ai.sbr" \
	"$(INTDIR)\h_battery.sbr" \
	"$(INTDIR)\h_cycler.sbr" \
	"$(INTDIR)\h_export.sbr" \
	"$(INTDIR)\lights.sbr" \
	"$(INTDIR)\mapcycle.sbr" \
	"$(INTDIR)\monstermaker.sbr" \
	"$(INTDIR)\mortar.sbr" \
	"$(INTDIR)\nodes.sbr" \
	"$(INTDIR)\observer.sbr" \
	"$(INTDIR)\pathcorner.sbr" \
	"$(INTDIR)\plane.sbr" \
	"$(INTDIR)\plats.sbr" \
	"$(INTDIR)\player.sbr" \
	"$(INTDIR)\saverestore.sbr" \
	"$(INTDIR)\scripted.sbr" \
	"$(INTDIR)\skill.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\soundent.sbr" \
	"$(INTDIR)\spectator.sbr" \
	"$(INTDIR)\subs.sbr" \
	"$(INTDIR)\triggers.sbr" \
	"$(INTDIR)\triggers_new.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\util_xhl.sbr" \
	"$(INTDIR)\weaponbox.sbr" \
	"$(INTDIR)\weapons.sbr" \
	"$(INTDIR)\world.sbr" \
	"$(INTDIR)\xen.sbr" \
	"$(INTDIR)\util_vector.sbr" \
	"$(INTDIR)\vector.sbr"

"$(OUTDIR)\XDM.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /version:3.0 /subsystem:windows /dll /profile /map:"$(INTDIR)\XDM_i686.map" /debug /machine:I386 /def:".\XDM.def" /out:"release/XDM_i686.dll" /implib:"$(OUTDIR)\XDM_i686.lib" 
DEF_FILE= \
	".\XDM.def"
LINK32_OBJS= \
	"$(INTDIR)\pm_debug.obj" \
	"$(INTDIR)\pm_math.obj" \
	"$(INTDIR)\pm_shared.obj" \
	"$(INTDIR)\alauncher.obj" \
	"$(INTDIR)\beamrifle.obj" \
	"$(INTDIR)\chemgun.obj" \
	"$(INTDIR)\crossbow.obj" \
	"$(INTDIR)\crowbar.obj" \
	"$(INTDIR)\displacer.obj" \
	"$(INTDIR)\egon.obj" \
	"$(INTDIR)\flamethrower.obj" \
	"$(INTDIR)\gauss.obj" \
	"$(INTDIR)\glauncher.obj" \
	"$(INTDIR)\glock.obj" \
	"$(INTDIR)\handgrenade.obj" \
	"$(INTDIR)\hornetgun.obj" \
	"$(INTDIR)\mp5.obj" \
	"$(INTDIR)\nanosword.obj" \
	"$(INTDIR)\orb.obj" \
	"$(INTDIR)\plasma.obj" \
	"$(INTDIR)\python.obj" \
	"$(INTDIR)\rpg.obj" \
	"$(INTDIR)\satchel.obj" \
	"$(INTDIR)\shotgun.obj" \
	"$(INTDIR)\squeakgrenade.obj" \
	"$(INTDIR)\strtarget.obj" \
	"$(INTDIR)\sword.obj" \
	"$(INTDIR)\tripmine.obj" \
	"$(INTDIR)\aflock.obj" \
	"$(INTDIR)\agrunt.obj" \
	"$(INTDIR)\apache.obj" \
	"$(INTDIR)\barnacle.obj" \
	"$(INTDIR)\barney.obj" \
	"$(INTDIR)\bigmomma.obj" \
	"$(INTDIR)\bloater.obj" \
	"$(INTDIR)\bullsquid.obj" \
	"$(INTDIR)\cat.obj" \
	"$(INTDIR)\controller.obj" \
	"$(INTDIR)\deadhev.obj" \
	"$(INTDIR)\diablo.obj" \
	"$(INTDIR)\flyingmonster.obj" \
	"$(INTDIR)\gargantua.obj" \
	"$(INTDIR)\genericmonster.obj" \
	"$(INTDIR)\gman.obj" \
	"$(INTDIR)\hassassin.obj" \
	"$(INTDIR)\headcrab.obj" \
	"$(INTDIR)\hgrunt.obj" \
	"$(INTDIR)\houndeye.obj" \
	"$(INTDIR)\ichthyosaur.obj" \
	"$(INTDIR)\islave.obj" \
	"$(INTDIR)\leech.obj" \
	"$(INTDIR)\monsters.obj" \
	"$(INTDIR)\monsterstate.obj" \
	"$(INTDIR)\nihilanth.obj" \
	"$(INTDIR)\osprey.obj" \
	"$(INTDIR)\playermonster.obj" \
	"$(INTDIR)\rat.obj" \
	"$(INTDIR)\roach.obj" \
	"$(INTDIR)\scientist.obj" \
	"$(INTDIR)\squadmonster.obj" \
	"$(INTDIR)\talkmonster.obj" \
	"$(INTDIR)\tentacle.obj" \
	"$(INTDIR)\turret.obj" \
	"$(INTDIR)\zombie.obj" \
	"$(INTDIR)\airtank.obj" \
	"$(INTDIR)\healthkit.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\items_new.obj" \
	"$(INTDIR)\coop_gamerules.obj" \
	"$(INTDIR)\ctf_gamerules.obj" \
	"$(INTDIR)\dom_gamerules.obj" \
	"$(INTDIR)\gamerules.obj" \
	"$(INTDIR)\lms_gamerules.obj" \
	"$(INTDIR)\maprules.obj" \
	"$(INTDIR)\multiplay_gamerules.obj" \
	"$(INTDIR)\singleplay_gamerules.obj" \
	"$(INTDIR)\teamplay_gamerules.obj" \
	"$(INTDIR)\effects.obj" \
	"$(INTDIR)\effects_new.obj" \
	"$(INTDIR)\environment.obj" \
	"$(INTDIR)\explode.obj" \
	"$(INTDIR)\fx.obj" \
	"$(INTDIR)\agrenade.obj" \
	"$(INTDIR)\atomicdevice.obj" \
	"$(INTDIR)\crossbowbolt.obj" \
	"$(INTDIR)\flamecloud.obj" \
	"$(INTDIR)\ggrenade.obj" \
	"$(INTDIR)\hornet.obj" \
	"$(INTDIR)\lgrenade.obj" \
	"$(INTDIR)\lightp.obj" \
	"$(INTDIR)\mtarget.obj" \
	"$(INTDIR)\orb_proj.obj" \
	"$(INTDIR)\plasmaball.obj" \
	"$(INTDIR)\rpgrocket.obj" \
	"$(INTDIR)\satchelcharge.obj" \
	"$(INTDIR)\sqkgrenade.obj" \
	"$(INTDIR)\teleporter.obj" \
	"$(INTDIR)\tripminegrenade.obj" \
	"$(INTDIR)\voice_banmgr.obj" \
	"$(INTDIR)\voice_gamemgr.obj" \
	"$(INTDIR)\ai_basenpc_schedule.obj" \
	"$(INTDIR)\ammo.obj" \
	"$(INTDIR)\animating.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\bmodels.obj" \
	"$(INTDIR)\buttons.obj" \
	"$(INTDIR)\cbase.obj" \
	"$(INTDIR)\chasecam.obj" \
	"$(INTDIR)\client.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\defaultai.obj" \
	"$(INTDIR)\doors.obj" \
	"$(INTDIR)\entconfig.obj" \
	"$(INTDIR)\func_break.obj" \
	"$(INTDIR)\func_tank.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\gibs.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\h_ai.obj" \
	"$(INTDIR)\h_battery.obj" \
	"$(INTDIR)\h_cycler.obj" \
	"$(INTDIR)\h_export.obj" \
	"$(INTDIR)\lights.obj" \
	"$(INTDIR)\mapcycle.obj" \
	"$(INTDIR)\monstermaker.obj" \
	"$(INTDIR)\mortar.obj" \
	"$(INTDIR)\nodes.obj" \
	"$(INTDIR)\observer.obj" \
	"$(INTDIR)\pathcorner.obj" \
	"$(INTDIR)\plane.obj" \
	"$(INTDIR)\plats.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\saverestore.obj" \
	"$(INTDIR)\scripted.obj" \
	"$(INTDIR)\skill.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\soundent.obj" \
	"$(INTDIR)\spectator.obj" \
	"$(INTDIR)\subs.obj" \
	"$(INTDIR)\triggers.obj" \
	"$(INTDIR)\triggers_new.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\util_xhl.obj" \
	"$(INTDIR)\weaponbox.obj" \
	"$(INTDIR)\weapons.obj" \
	"$(INTDIR)\world.obj" \
	"$(INTDIR)\xen.obj" \
	"$(INTDIR)\XDM.res" \
	"$(INTDIR)\util_vector.obj" \
	"$(INTDIR)\vector.obj"

".\release\XDM_i686.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\release\XDM_i686.dll
InputPath=.\release\XDM_i686.dll
InputName=XDM_i686
SOURCE="$(InputPath)"

".\XDM_i686" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy       $(TargetPath)       \Half-Life\XDM\dlls\  
	copy       $(TargetPath)       F:\games\HL1121\XDM\dlls\  
	copy  $(TargetPath)  W:\dev\Half-Life\XDM\dlls\  
	
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
!IF EXISTS("XDM.dep")
!INCLUDE "XDM.dep"
!ELSE 
!MESSAGE Warning: cannot find "XDM.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "XDM - Win32 Debug" || "$(CFG)" == "XDM - Win32 Release" || "$(CFG)" == "XDM - Win32 Profile"
SOURCE=..\pm_shared\pm_debug.c

"$(INTDIR)\pm_debug.obj"	"$(INTDIR)\pm_debug.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\pm_shared\pm_math.c

"$(INTDIR)\pm_math.obj"	"$(INTDIR)\pm_math.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\pm_shared\pm_shared.cpp

"$(INTDIR)\pm_shared.obj"	"$(INTDIR)\pm_shared.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\alauncher.cpp

"$(INTDIR)\alauncher.obj"	"$(INTDIR)\alauncher.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\beamrifle.cpp

"$(INTDIR)\beamrifle.obj"	"$(INTDIR)\beamrifle.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\chemgun.cpp

"$(INTDIR)\chemgun.obj"	"$(INTDIR)\chemgun.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\crossbow.cpp

"$(INTDIR)\crossbow.obj"	"$(INTDIR)\crossbow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\crowbar.cpp

"$(INTDIR)\crowbar.obj"	"$(INTDIR)\crowbar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\displacer.cpp

"$(INTDIR)\displacer.obj"	"$(INTDIR)\displacer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\egon.cpp

"$(INTDIR)\egon.obj"	"$(INTDIR)\egon.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\flamethrower.cpp

"$(INTDIR)\flamethrower.obj"	"$(INTDIR)\flamethrower.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\gauss.cpp

"$(INTDIR)\gauss.obj"	"$(INTDIR)\gauss.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\glauncher.cpp

"$(INTDIR)\glauncher.obj"	"$(INTDIR)\glauncher.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\glock.cpp

"$(INTDIR)\glock.obj"	"$(INTDIR)\glock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\handgrenade.cpp

"$(INTDIR)\handgrenade.obj"	"$(INTDIR)\handgrenade.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\hornetgun.cpp

"$(INTDIR)\hornetgun.obj"	"$(INTDIR)\hornetgun.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\mp5.cpp

"$(INTDIR)\mp5.obj"	"$(INTDIR)\mp5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\nanosword.cpp

"$(INTDIR)\nanosword.obj"	"$(INTDIR)\nanosword.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\orb.cpp

"$(INTDIR)\orb.obj"	"$(INTDIR)\orb.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\plasma.cpp

"$(INTDIR)\plasma.obj"	"$(INTDIR)\plasma.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\python.cpp

"$(INTDIR)\python.obj"	"$(INTDIR)\python.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\rpg.cpp

"$(INTDIR)\rpg.obj"	"$(INTDIR)\rpg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\satchel.cpp

"$(INTDIR)\satchel.obj"	"$(INTDIR)\satchel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\shotgun.cpp

"$(INTDIR)\shotgun.obj"	"$(INTDIR)\shotgun.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\squeakgrenade.cpp

"$(INTDIR)\squeakgrenade.obj"	"$(INTDIR)\squeakgrenade.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\strtarget.cpp

"$(INTDIR)\strtarget.obj"	"$(INTDIR)\strtarget.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\sword.cpp

"$(INTDIR)\sword.obj"	"$(INTDIR)\sword.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\weapons\tripmine.cpp

"$(INTDIR)\tripmine.obj"	"$(INTDIR)\tripmine.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\aflock.cpp

"$(INTDIR)\aflock.obj"	"$(INTDIR)\aflock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\agrunt.cpp

"$(INTDIR)\agrunt.obj"	"$(INTDIR)\agrunt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\apache.cpp

"$(INTDIR)\apache.obj"	"$(INTDIR)\apache.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\barnacle.cpp

"$(INTDIR)\barnacle.obj"	"$(INTDIR)\barnacle.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\barney.cpp

"$(INTDIR)\barney.obj"	"$(INTDIR)\barney.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\bigmomma.cpp

"$(INTDIR)\bigmomma.obj"	"$(INTDIR)\bigmomma.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\bloater.cpp

"$(INTDIR)\bloater.obj"	"$(INTDIR)\bloater.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\bullsquid.cpp

"$(INTDIR)\bullsquid.obj"	"$(INTDIR)\bullsquid.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\cat.cpp

"$(INTDIR)\cat.obj"	"$(INTDIR)\cat.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\controller.cpp

"$(INTDIR)\controller.obj"	"$(INTDIR)\controller.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\deadhev.cpp

"$(INTDIR)\deadhev.obj"	"$(INTDIR)\deadhev.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\diablo.cpp

"$(INTDIR)\diablo.obj"	"$(INTDIR)\diablo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\flyingmonster.cpp

"$(INTDIR)\flyingmonster.obj"	"$(INTDIR)\flyingmonster.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\gargantua.cpp

"$(INTDIR)\gargantua.obj"	"$(INTDIR)\gargantua.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\genericmonster.cpp

"$(INTDIR)\genericmonster.obj"	"$(INTDIR)\genericmonster.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\gman.cpp

"$(INTDIR)\gman.obj"	"$(INTDIR)\gman.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\hassassin.cpp

"$(INTDIR)\hassassin.obj"	"$(INTDIR)\hassassin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\headcrab.cpp

"$(INTDIR)\headcrab.obj"	"$(INTDIR)\headcrab.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\hgrunt.cpp

"$(INTDIR)\hgrunt.obj"	"$(INTDIR)\hgrunt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\houndeye.cpp

"$(INTDIR)\houndeye.obj"	"$(INTDIR)\houndeye.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\ichthyosaur.cpp

"$(INTDIR)\ichthyosaur.obj"	"$(INTDIR)\ichthyosaur.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\islave.cpp

"$(INTDIR)\islave.obj"	"$(INTDIR)\islave.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\leech.cpp

"$(INTDIR)\leech.obj"	"$(INTDIR)\leech.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\monsters.cpp

"$(INTDIR)\monsters.obj"	"$(INTDIR)\monsters.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\monsterstate.cpp

"$(INTDIR)\monsterstate.obj"	"$(INTDIR)\monsterstate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\nihilanth.cpp

"$(INTDIR)\nihilanth.obj"	"$(INTDIR)\nihilanth.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\osprey.cpp

"$(INTDIR)\osprey.obj"	"$(INTDIR)\osprey.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\playermonster.cpp

"$(INTDIR)\playermonster.obj"	"$(INTDIR)\playermonster.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\rat.cpp

"$(INTDIR)\rat.obj"	"$(INTDIR)\rat.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\roach.cpp

"$(INTDIR)\roach.obj"	"$(INTDIR)\roach.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\scientist.cpp

"$(INTDIR)\scientist.obj"	"$(INTDIR)\scientist.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\squadmonster.cpp

"$(INTDIR)\squadmonster.obj"	"$(INTDIR)\squadmonster.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\talkmonster.cpp

"$(INTDIR)\talkmonster.obj"	"$(INTDIR)\talkmonster.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\tentacle.cpp

"$(INTDIR)\tentacle.obj"	"$(INTDIR)\tentacle.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\turret.cpp

"$(INTDIR)\turret.obj"	"$(INTDIR)\turret.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\monsters\zombie.cpp

"$(INTDIR)\zombie.obj"	"$(INTDIR)\zombie.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\airtank.cpp

"$(INTDIR)\airtank.obj"	"$(INTDIR)\airtank.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\healthkit.cpp

"$(INTDIR)\healthkit.obj"	"$(INTDIR)\healthkit.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\items.cpp

"$(INTDIR)\items.obj"	"$(INTDIR)\items.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\items_new.cpp

"$(INTDIR)\items_new.obj"	"$(INTDIR)\items_new.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gamerules\coop_gamerules.cpp

"$(INTDIR)\coop_gamerules.obj"	"$(INTDIR)\coop_gamerules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\gamerules\ctf_gamerules.cpp

"$(INTDIR)\ctf_gamerules.obj"	"$(INTDIR)\ctf_gamerules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\gamerules\dom_gamerules.cpp

"$(INTDIR)\dom_gamerules.obj"	"$(INTDIR)\dom_gamerules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\gamerules\gamerules.cpp

"$(INTDIR)\gamerules.obj"	"$(INTDIR)\gamerules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\gamerules\lms_gamerules.cpp

"$(INTDIR)\lms_gamerules.obj"	"$(INTDIR)\lms_gamerules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\gamerules\maprules.cpp

"$(INTDIR)\maprules.obj"	"$(INTDIR)\maprules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\gamerules\multiplay_gamerules.cpp

"$(INTDIR)\multiplay_gamerules.obj"	"$(INTDIR)\multiplay_gamerules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\gamerules\singleplay_gamerules.cpp

"$(INTDIR)\singleplay_gamerules.obj"	"$(INTDIR)\singleplay_gamerules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\gamerules\teamplay_gamerules.cpp

"$(INTDIR)\teamplay_gamerules.obj"	"$(INTDIR)\teamplay_gamerules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\effects.cpp

"$(INTDIR)\effects.obj"	"$(INTDIR)\effects.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\effects_new.cpp

"$(INTDIR)\effects_new.obj"	"$(INTDIR)\effects_new.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\environment.cpp

"$(INTDIR)\environment.obj"	"$(INTDIR)\environment.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\explode.cpp

"$(INTDIR)\explode.obj"	"$(INTDIR)\explode.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\fx.cpp

"$(INTDIR)\fx.obj"	"$(INTDIR)\fx.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\projectiles\agrenade.cpp

"$(INTDIR)\agrenade.obj"	"$(INTDIR)\agrenade.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\atomicdevice.cpp

"$(INTDIR)\atomicdevice.obj"	"$(INTDIR)\atomicdevice.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\crossbowbolt.cpp

"$(INTDIR)\crossbowbolt.obj"	"$(INTDIR)\crossbowbolt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\flamecloud.cpp

"$(INTDIR)\flamecloud.obj"	"$(INTDIR)\flamecloud.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\ggrenade.cpp

"$(INTDIR)\ggrenade.obj"	"$(INTDIR)\ggrenade.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\hornet.cpp

"$(INTDIR)\hornet.obj"	"$(INTDIR)\hornet.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\lgrenade.cpp

"$(INTDIR)\lgrenade.obj"	"$(INTDIR)\lgrenade.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\lightp.cpp

"$(INTDIR)\lightp.obj"	"$(INTDIR)\lightp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\mtarget.cpp

"$(INTDIR)\mtarget.obj"	"$(INTDIR)\mtarget.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\orb_proj.cpp

"$(INTDIR)\orb_proj.obj"	"$(INTDIR)\orb_proj.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\plasmaball.cpp

"$(INTDIR)\plasmaball.obj"	"$(INTDIR)\plasmaball.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\rpgrocket.cpp

"$(INTDIR)\rpgrocket.obj"	"$(INTDIR)\rpgrocket.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\satchelcharge.cpp

"$(INTDIR)\satchelcharge.obj"	"$(INTDIR)\satchelcharge.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\sqkgrenade.cpp

"$(INTDIR)\sqkgrenade.obj"	"$(INTDIR)\sqkgrenade.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\teleporter.cpp

"$(INTDIR)\teleporter.obj"	"$(INTDIR)\teleporter.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\projectiles\tripminegrenade.cpp

"$(INTDIR)\tripminegrenade.obj"	"$(INTDIR)\tripminegrenade.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\voice_banmgr.cpp

"$(INTDIR)\voice_banmgr.obj"	"$(INTDIR)\voice_banmgr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\voice_gamemgr.cpp

"$(INTDIR)\voice_gamemgr.obj"	"$(INTDIR)\voice_gamemgr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\util_vector.cpp

"$(INTDIR)\util_vector.obj"	"$(INTDIR)\util_vector.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\vector.cpp

"$(INTDIR)\vector.obj"	"$(INTDIR)\vector.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ai_basenpc_schedule.cpp

"$(INTDIR)\ai_basenpc_schedule.obj"	"$(INTDIR)\ai_basenpc_schedule.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ammo.cpp

"$(INTDIR)\ammo.obj"	"$(INTDIR)\ammo.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\animating.cpp

"$(INTDIR)\animating.obj"	"$(INTDIR)\animating.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\animation.cpp

"$(INTDIR)\animation.obj"	"$(INTDIR)\animation.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bmodels.cpp

"$(INTDIR)\bmodels.obj"	"$(INTDIR)\bmodels.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\buttons.cpp

"$(INTDIR)\buttons.obj"	"$(INTDIR)\buttons.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cbase.cpp

"$(INTDIR)\cbase.obj"	"$(INTDIR)\cbase.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\chasecam.cpp

"$(INTDIR)\chasecam.obj"	"$(INTDIR)\chasecam.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\client.cpp

"$(INTDIR)\client.obj"	"$(INTDIR)\client.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\combat.cpp

"$(INTDIR)\combat.obj"	"$(INTDIR)\combat.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\defaultai.cpp

"$(INTDIR)\defaultai.obj"	"$(INTDIR)\defaultai.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\doors.cpp

"$(INTDIR)\doors.obj"	"$(INTDIR)\doors.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\entconfig.cpp

"$(INTDIR)\entconfig.obj"	"$(INTDIR)\entconfig.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\func_break.cpp

"$(INTDIR)\func_break.obj"	"$(INTDIR)\func_break.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\func_tank.cpp

"$(INTDIR)\func_tank.obj"	"$(INTDIR)\func_tank.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\game.cpp

"$(INTDIR)\game.obj"	"$(INTDIR)\game.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gibs.cpp

"$(INTDIR)\gibs.obj"	"$(INTDIR)\gibs.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\globals.cpp

"$(INTDIR)\globals.obj"	"$(INTDIR)\globals.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\h_ai.cpp

"$(INTDIR)\h_ai.obj"	"$(INTDIR)\h_ai.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\h_battery.cpp

"$(INTDIR)\h_battery.obj"	"$(INTDIR)\h_battery.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\h_cycler.cpp

"$(INTDIR)\h_cycler.obj"	"$(INTDIR)\h_cycler.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\h_export.cpp

"$(INTDIR)\h_export.obj"	"$(INTDIR)\h_export.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\lights.cpp

"$(INTDIR)\lights.obj"	"$(INTDIR)\lights.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mapcycle.cpp

"$(INTDIR)\mapcycle.obj"	"$(INTDIR)\mapcycle.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\monstermaker.cpp

"$(INTDIR)\monstermaker.obj"	"$(INTDIR)\monstermaker.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mortar.cpp

"$(INTDIR)\mortar.obj"	"$(INTDIR)\mortar.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\nodes.cpp

"$(INTDIR)\nodes.obj"	"$(INTDIR)\nodes.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\observer.cpp

"$(INTDIR)\observer.obj"	"$(INTDIR)\observer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pathcorner.cpp

"$(INTDIR)\pathcorner.obj"	"$(INTDIR)\pathcorner.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plane.cpp

"$(INTDIR)\plane.obj"	"$(INTDIR)\plane.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plats.cpp

"$(INTDIR)\plats.obj"	"$(INTDIR)\plats.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\player.cpp

"$(INTDIR)\player.obj"	"$(INTDIR)\player.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\saverestore.cpp

"$(INTDIR)\saverestore.obj"	"$(INTDIR)\saverestore.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\scripted.cpp

"$(INTDIR)\scripted.obj"	"$(INTDIR)\scripted.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\skill.cpp

"$(INTDIR)\skill.obj"	"$(INTDIR)\skill.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sound.cpp

"$(INTDIR)\sound.obj"	"$(INTDIR)\sound.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\soundent.cpp

"$(INTDIR)\soundent.obj"	"$(INTDIR)\soundent.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\spectator.cpp

"$(INTDIR)\spectator.obj"	"$(INTDIR)\spectator.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\subs.cpp

"$(INTDIR)\subs.obj"	"$(INTDIR)\subs.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\triggers.cpp

"$(INTDIR)\triggers.obj"	"$(INTDIR)\triggers.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\triggers_new.cpp

"$(INTDIR)\triggers_new.obj"	"$(INTDIR)\triggers_new.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\util.cpp

"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\util_xhl.cpp

"$(INTDIR)\util_xhl.obj"	"$(INTDIR)\util_xhl.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\weaponbox.cpp

"$(INTDIR)\weaponbox.obj"	"$(INTDIR)\weaponbox.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\weapons.cpp

"$(INTDIR)\weapons.obj"	"$(INTDIR)\weapons.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\world.cpp

"$(INTDIR)\world.obj"	"$(INTDIR)\world.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\xen.cpp

"$(INTDIR)\xen.obj"	"$(INTDIR)\xen.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\XDM.rc

"$(INTDIR)\XDM.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

