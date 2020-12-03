# Microsoft Developer Studio Generated NMAKE File, Based on cl_dll.dsp
!IF "$(CFG)" == ""
CFG=cl_dll - Win32 Release
!MESSAGE No configuration specified. Defaulting to cl_dll - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "cl_dll - Win32 Release" && "$(CFG)" != "cl_dll - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cl_dll.mak" CFG="cl_dll - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cl_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cl_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "cl_dll - Win32 Release"

OUTDIR=.\release
INTDIR=.\release/intermediate
# Begin Custom Macros
OutDir=.\release
# End Custom Macros

ALL : "$(OUTDIR)\client.dll" "$(OUTDIR)\cl_dll.bsc" ".\client"


CLEAN :
	-@erase "$(INTDIR)\alauncher.obj"
	-@erase "$(INTDIR)\alauncher.sbr"
	-@erase "$(INTDIR)\ammo.obj"
	-@erase "$(INTDIR)\ammo.sbr"
	-@erase "$(INTDIR)\ammo_secondary.obj"
	-@erase "$(INTDIR)\ammo_secondary.sbr"
	-@erase "$(INTDIR)\ammohistory.obj"
	-@erase "$(INTDIR)\ammohistory.sbr"
	-@erase "$(INTDIR)\battery.obj"
	-@erase "$(INTDIR)\battery.sbr"
	-@erase "$(INTDIR)\beamrifle.obj"
	-@erase "$(INTDIR)\beamrifle.sbr"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\cdll_int.sbr"
	-@erase "$(INTDIR)\chemgun.obj"
	-@erase "$(INTDIR)\chemgun.sbr"
	-@erase "$(INTDIR)\cl_dll.res"
	-@erase "$(INTDIR)\cl_fx.obj"
	-@erase "$(INTDIR)\cl_fx.sbr"
	-@erase "$(INTDIR)\com_weapons.obj"
	-@erase "$(INTDIR)\com_weapons.sbr"
	-@erase "$(INTDIR)\crossbow.obj"
	-@erase "$(INTDIR)\crossbow.sbr"
	-@erase "$(INTDIR)\crowbar.obj"
	-@erase "$(INTDIR)\crowbar.sbr"
	-@erase "$(INTDIR)\death.obj"
	-@erase "$(INTDIR)\death.sbr"
	-@erase "$(INTDIR)\demo.obj"
	-@erase "$(INTDIR)\demo.sbr"
	-@erase "$(INTDIR)\displacer.obj"
	-@erase "$(INTDIR)\displacer.sbr"
	-@erase "$(INTDIR)\egon.obj"
	-@erase "$(INTDIR)\egon.sbr"
	-@erase "$(INTDIR)\entity.obj"
	-@erase "$(INTDIR)\entity.sbr"
	-@erase "$(INTDIR)\ev_common.obj"
	-@erase "$(INTDIR)\ev_common.sbr"
	-@erase "$(INTDIR)\ev_fx.obj"
	-@erase "$(INTDIR)\ev_fx.sbr"
	-@erase "$(INTDIR)\ev_game.obj"
	-@erase "$(INTDIR)\ev_game.sbr"
	-@erase "$(INTDIR)\ev_hldm.obj"
	-@erase "$(INTDIR)\ev_hldm.sbr"
	-@erase "$(INTDIR)\events.obj"
	-@erase "$(INTDIR)\events.sbr"
	-@erase "$(INTDIR)\flamethrower.obj"
	-@erase "$(INTDIR)\flamethrower.sbr"
	-@erase "$(INTDIR)\flashlight.obj"
	-@erase "$(INTDIR)\flashlight.sbr"
	-@erase "$(INTDIR)\GameStudioModelRenderer.obj"
	-@erase "$(INTDIR)\GameStudioModelRenderer.sbr"
	-@erase "$(INTDIR)\gauss.obj"
	-@erase "$(INTDIR)\gauss.sbr"
	-@erase "$(INTDIR)\geiger.obj"
	-@erase "$(INTDIR)\geiger.sbr"
	-@erase "$(INTDIR)\glauncher.obj"
	-@erase "$(INTDIR)\glauncher.sbr"
	-@erase "$(INTDIR)\glock.obj"
	-@erase "$(INTDIR)\glock.sbr"
	-@erase "$(INTDIR)\handgrenade.obj"
	-@erase "$(INTDIR)\handgrenade.sbr"
	-@erase "$(INTDIR)\health.obj"
	-@erase "$(INTDIR)\health.sbr"
	-@erase "$(INTDIR)\hl_baseentity.obj"
	-@erase "$(INTDIR)\hl_baseentity.sbr"
	-@erase "$(INTDIR)\hl_weapons.obj"
	-@erase "$(INTDIR)\hl_weapons.sbr"
	-@erase "$(INTDIR)\hornetgun.obj"
	-@erase "$(INTDIR)\hornetgun.sbr"
	-@erase "$(INTDIR)\hud.obj"
	-@erase "$(INTDIR)\hud.sbr"
	-@erase "$(INTDIR)\hud_domdisplay.obj"
	-@erase "$(INTDIR)\hud_domdisplay.sbr"
	-@erase "$(INTDIR)\hud_flagdisplay.obj"
	-@erase "$(INTDIR)\hud_flagdisplay.sbr"
	-@erase "$(INTDIR)\hud_msg.obj"
	-@erase "$(INTDIR)\hud_msg.sbr"
	-@erase "$(INTDIR)\hud_redraw.obj"
	-@erase "$(INTDIR)\hud_redraw.sbr"
	-@erase "$(INTDIR)\hud_rocketscrn.obj"
	-@erase "$(INTDIR)\hud_rocketscrn.sbr"
	-@erase "$(INTDIR)\hud_servers.obj"
	-@erase "$(INTDIR)\hud_servers.sbr"
	-@erase "$(INTDIR)\hud_spectator.obj"
	-@erase "$(INTDIR)\hud_spectator.sbr"
	-@erase "$(INTDIR)\hud_update.obj"
	-@erase "$(INTDIR)\hud_update.sbr"
	-@erase "$(INTDIR)\hud_zcrosshair.obj"
	-@erase "$(INTDIR)\hud_zcrosshair.sbr"
	-@erase "$(INTDIR)\in_camera.obj"
	-@erase "$(INTDIR)\in_camera.sbr"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\input.sbr"
	-@erase "$(INTDIR)\inputw32.obj"
	-@erase "$(INTDIR)\inputw32.sbr"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\interface.sbr"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\message.sbr"
	-@erase "$(INTDIR)\mp5.obj"
	-@erase "$(INTDIR)\mp5.sbr"
	-@erase "$(INTDIR)\msg_fx.obj"
	-@erase "$(INTDIR)\msg_fx.sbr"
	-@erase "$(INTDIR)\musicplayer.obj"
	-@erase "$(INTDIR)\musicplayer.sbr"
	-@erase "$(INTDIR)\nanosword.obj"
	-@erase "$(INTDIR)\nanosword.sbr"
	-@erase "$(INTDIR)\orb.obj"
	-@erase "$(INTDIR)\orb.sbr"
	-@erase "$(INTDIR)\parsemsg.obj"
	-@erase "$(INTDIR)\parsemsg.sbr"
	-@erase "$(INTDIR)\Particle.obj"
	-@erase "$(INTDIR)\Particle.sbr"
	-@erase "$(INTDIR)\PartSystem.obj"
	-@erase "$(INTDIR)\PartSystem.sbr"
	-@erase "$(INTDIR)\plasma.obj"
	-@erase "$(INTDIR)\plasma.sbr"
	-@erase "$(INTDIR)\pm_debug.obj"
	-@erase "$(INTDIR)\pm_debug.sbr"
	-@erase "$(INTDIR)\pm_math.obj"
	-@erase "$(INTDIR)\pm_math.sbr"
	-@erase "$(INTDIR)\pm_shared.obj"
	-@erase "$(INTDIR)\pm_shared.sbr"
	-@erase "$(INTDIR)\PSBeam.obj"
	-@erase "$(INTDIR)\PSBeam.sbr"
	-@erase "$(INTDIR)\PSDrips.obj"
	-@erase "$(INTDIR)\PSDrips.sbr"
	-@erase "$(INTDIR)\PSFlameCone.obj"
	-@erase "$(INTDIR)\PSFlameCone.sbr"
	-@erase "$(INTDIR)\PSFlatTrail.obj"
	-@erase "$(INTDIR)\PSFlatTrail.sbr"
	-@erase "$(INTDIR)\PSSparks.obj"
	-@erase "$(INTDIR)\PSSparks.sbr"
	-@erase "$(INTDIR)\PSSpawnEffect.obj"
	-@erase "$(INTDIR)\PSSpawnEffect.sbr"
	-@erase "$(INTDIR)\python.obj"
	-@erase "$(INTDIR)\python.sbr"
	-@erase "$(INTDIR)\RenderManager.obj"
	-@erase "$(INTDIR)\RenderManager.sbr"
	-@erase "$(INTDIR)\RenderSystem.obj"
	-@erase "$(INTDIR)\RenderSystem.sbr"
	-@erase "$(INTDIR)\RotatingSystem.obj"
	-@erase "$(INTDIR)\RotatingSystem.sbr"
	-@erase "$(INTDIR)\rpg.obj"
	-@erase "$(INTDIR)\rpg.sbr"
	-@erase "$(INTDIR)\RSBeam.obj"
	-@erase "$(INTDIR)\RSBeam.sbr"
	-@erase "$(INTDIR)\RSBeamStar.obj"
	-@erase "$(INTDIR)\RSBeamStar.sbr"
	-@erase "$(INTDIR)\RSCylinder.obj"
	-@erase "$(INTDIR)\RSCylinder.sbr"
	-@erase "$(INTDIR)\RSDelayed.obj"
	-@erase "$(INTDIR)\RSDelayed.sbr"
	-@erase "$(INTDIR)\RSDisk.obj"
	-@erase "$(INTDIR)\RSDisk.sbr"
	-@erase "$(INTDIR)\RSLight.obj"
	-@erase "$(INTDIR)\RSLight.sbr"
	-@erase "$(INTDIR)\RSSphere.obj"
	-@erase "$(INTDIR)\RSSphere.sbr"
	-@erase "$(INTDIR)\RSSprite.obj"
	-@erase "$(INTDIR)\RSSprite.sbr"
	-@erase "$(INTDIR)\RSTeleparts.obj"
	-@erase "$(INTDIR)\RSTeleparts.sbr"
	-@erase "$(INTDIR)\satchel.obj"
	-@erase "$(INTDIR)\satchel.sbr"
	-@erase "$(INTDIR)\saytext.obj"
	-@erase "$(INTDIR)\saytext.sbr"
	-@erase "$(INTDIR)\shotgun.obj"
	-@erase "$(INTDIR)\shotgun.sbr"
	-@erase "$(INTDIR)\squeakgrenade.obj"
	-@erase "$(INTDIR)\squeakgrenade.sbr"
	-@erase "$(INTDIR)\status_icons.obj"
	-@erase "$(INTDIR)\status_icons.sbr"
	-@erase "$(INTDIR)\statusbar.obj"
	-@erase "$(INTDIR)\statusbar.sbr"
	-@erase "$(INTDIR)\strtarget.obj"
	-@erase "$(INTDIR)\strtarget.sbr"
	-@erase "$(INTDIR)\studio_util.obj"
	-@erase "$(INTDIR)\studio_util.sbr"
	-@erase "$(INTDIR)\StudioModelRenderer.obj"
	-@erase "$(INTDIR)\StudioModelRenderer.sbr"
	-@erase "$(INTDIR)\sword.obj"
	-@erase "$(INTDIR)\sword.sbr"
	-@erase "$(INTDIR)\text_message.obj"
	-@erase "$(INTDIR)\text_message.sbr"
	-@erase "$(INTDIR)\train.obj"
	-@erase "$(INTDIR)\train.sbr"
	-@erase "$(INTDIR)\tri.obj"
	-@erase "$(INTDIR)\tri.sbr"
	-@erase "$(INTDIR)\tripmine.obj"
	-@erase "$(INTDIR)\tripmine.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\util_vector.obj"
	-@erase "$(INTDIR)\util_vector.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vector.obj"
	-@erase "$(INTDIR)\vector.sbr"
	-@erase "$(INTDIR)\vgui_checkbutton2.obj"
	-@erase "$(INTDIR)\vgui_checkbutton2.sbr"
	-@erase "$(INTDIR)\vgui_CommandMenu.obj"
	-@erase "$(INTDIR)\vgui_CommandMenu.sbr"
	-@erase "$(INTDIR)\vgui_ConsolePanel.obj"
	-@erase "$(INTDIR)\vgui_ConsolePanel.sbr"
	-@erase "$(INTDIR)\vgui_CustomObjects.obj"
	-@erase "$(INTDIR)\vgui_CustomObjects.sbr"
	-@erase "$(INTDIR)\vgui_grid.obj"
	-@erase "$(INTDIR)\vgui_grid.sbr"
	-@erase "$(INTDIR)\vgui_helpers.obj"
	-@erase "$(INTDIR)\vgui_helpers.sbr"
	-@erase "$(INTDIR)\vgui_Int.obj"
	-@erase "$(INTDIR)\vgui_Int.sbr"
	-@erase "$(INTDIR)\vgui_listbox.obj"
	-@erase "$(INTDIR)\vgui_listbox.sbr"
	-@erase "$(INTDIR)\vgui_loadtga.obj"
	-@erase "$(INTDIR)\vgui_loadtga.sbr"
	-@erase "$(INTDIR)\vgui_MOTDWindow.obj"
	-@erase "$(INTDIR)\vgui_MOTDWindow.sbr"
	-@erase "$(INTDIR)\vgui_MusicPlayer.obj"
	-@erase "$(INTDIR)\vgui_MusicPlayer.sbr"
	-@erase "$(INTDIR)\vgui_SchemeManager.obj"
	-@erase "$(INTDIR)\vgui_SchemeManager.sbr"
	-@erase "$(INTDIR)\vgui_ScorePanel.obj"
	-@erase "$(INTDIR)\vgui_ScorePanel.sbr"
	-@erase "$(INTDIR)\vgui_scrollbar2.obj"
	-@erase "$(INTDIR)\vgui_scrollbar2.sbr"
	-@erase "$(INTDIR)\vgui_ServerBrowser.obj"
	-@erase "$(INTDIR)\vgui_ServerBrowser.sbr"
	-@erase "$(INTDIR)\vgui_slider2.obj"
	-@erase "$(INTDIR)\vgui_slider2.sbr"
	-@erase "$(INTDIR)\vgui_SpectatorPanel.obj"
	-@erase "$(INTDIR)\vgui_SpectatorPanel.sbr"
	-@erase "$(INTDIR)\vgui_TeamFortressViewport.obj"
	-@erase "$(INTDIR)\vgui_TeamFortressViewport.sbr"
	-@erase "$(INTDIR)\vgui_TeamMenu.obj"
	-@erase "$(INTDIR)\vgui_TeamMenu.sbr"
	-@erase "$(INTDIR)\view.obj"
	-@erase "$(INTDIR)\view.sbr"
	-@erase "$(INTDIR)\voice_banmgr.obj"
	-@erase "$(INTDIR)\voice_banmgr.sbr"
	-@erase "$(INTDIR)\voice_status.obj"
	-@erase "$(INTDIR)\voice_status.sbr"
	-@erase "$(INTDIR)\voice_vgui_tweakdlg.obj"
	-@erase "$(INTDIR)\voice_vgui_tweakdlg.sbr"
	-@erase "$(OUTDIR)\cl_dll.bsc"
	-@erase "$(OUTDIR)\client.dll"
	-@erase "$(OUTDIR)\client.exp"
	-@erase "$(OUTDIR)\intermediate\client.map"
	-@erase "client"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /I "." /I "rendersystem" /I "..\dlls" /I "..\dlls\gamerules" /I "..\game_shared" /I "..\vgui_include" /I "..\engine" /I "..\common" /I "..\pm_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "CLIENT_DLL" /Fr"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cl_dll.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cl_dll.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\hl_baseentity.sbr" \
	"$(INTDIR)\hl_weapons.sbr" \
	"$(INTDIR)\pm_debug.sbr" \
	"$(INTDIR)\pm_math.sbr" \
	"$(INTDIR)\pm_shared.sbr" \
	"$(INTDIR)\vgui_CommandMenu.sbr" \
	"$(INTDIR)\vgui_ConsolePanel.sbr" \
	"$(INTDIR)\vgui_CustomObjects.sbr" \
	"$(INTDIR)\vgui_Int.sbr" \
	"$(INTDIR)\vgui_MOTDWindow.sbr" \
	"$(INTDIR)\vgui_MusicPlayer.sbr" \
	"$(INTDIR)\vgui_SchemeManager.sbr" \
	"$(INTDIR)\vgui_ScorePanel.sbr" \
	"$(INTDIR)\vgui_ServerBrowser.sbr" \
	"$(INTDIR)\vgui_SpectatorPanel.sbr" \
	"$(INTDIR)\vgui_TeamFortressViewport.sbr" \
	"$(INTDIR)\vgui_TeamMenu.sbr" \
	"$(INTDIR)\voice_banmgr.sbr" \
	"$(INTDIR)\voice_status.sbr" \
	"$(INTDIR)\voice_vgui_tweakdlg.sbr" \
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
	"$(INTDIR)\GameStudioModelRenderer.sbr" \
	"$(INTDIR)\studio_util.sbr" \
	"$(INTDIR)\StudioModelRenderer.sbr" \
	"$(INTDIR)\tri.sbr" \
	"$(INTDIR)\ammo.sbr" \
	"$(INTDIR)\ammo_secondary.sbr" \
	"$(INTDIR)\battery.sbr" \
	"$(INTDIR)\death.sbr" \
	"$(INTDIR)\flashlight.sbr" \
	"$(INTDIR)\geiger.sbr" \
	"$(INTDIR)\health.sbr" \
	"$(INTDIR)\hud.sbr" \
	"$(INTDIR)\hud_domdisplay.sbr" \
	"$(INTDIR)\hud_flagdisplay.sbr" \
	"$(INTDIR)\hud_msg.sbr" \
	"$(INTDIR)\hud_redraw.sbr" \
	"$(INTDIR)\hud_rocketscrn.sbr" \
	"$(INTDIR)\hud_servers.sbr" \
	"$(INTDIR)\hud_spectator.sbr" \
	"$(INTDIR)\hud_update.sbr" \
	"$(INTDIR)\hud_zcrosshair.sbr" \
	"$(INTDIR)\message.sbr" \
	"$(INTDIR)\saytext.sbr" \
	"$(INTDIR)\status_icons.sbr" \
	"$(INTDIR)\statusbar.sbr" \
	"$(INTDIR)\text_message.sbr" \
	"$(INTDIR)\train.sbr" \
	"$(INTDIR)\Particle.sbr" \
	"$(INTDIR)\PartSystem.sbr" \
	"$(INTDIR)\PSBeam.sbr" \
	"$(INTDIR)\PSDrips.sbr" \
	"$(INTDIR)\PSFlameCone.sbr" \
	"$(INTDIR)\PSFlatTrail.sbr" \
	"$(INTDIR)\PSSparks.sbr" \
	"$(INTDIR)\PSSpawnEffect.sbr" \
	"$(INTDIR)\RenderManager.sbr" \
	"$(INTDIR)\RenderSystem.sbr" \
	"$(INTDIR)\RotatingSystem.sbr" \
	"$(INTDIR)\RSBeam.sbr" \
	"$(INTDIR)\RSBeamStar.sbr" \
	"$(INTDIR)\RSCylinder.sbr" \
	"$(INTDIR)\RSDelayed.sbr" \
	"$(INTDIR)\RSDisk.sbr" \
	"$(INTDIR)\RSLight.sbr" \
	"$(INTDIR)\RSSphere.sbr" \
	"$(INTDIR)\RSSprite.sbr" \
	"$(INTDIR)\RSTeleparts.sbr" \
	"$(INTDIR)\ev_common.sbr" \
	"$(INTDIR)\ev_fx.sbr" \
	"$(INTDIR)\ev_game.sbr" \
	"$(INTDIR)\ev_hldm.sbr" \
	"$(INTDIR)\events.sbr" \
	"$(INTDIR)\vgui_checkbutton2.sbr" \
	"$(INTDIR)\vgui_grid.sbr" \
	"$(INTDIR)\vgui_helpers.sbr" \
	"$(INTDIR)\vgui_listbox.sbr" \
	"$(INTDIR)\vgui_loadtga.sbr" \
	"$(INTDIR)\vgui_scrollbar2.sbr" \
	"$(INTDIR)\vgui_slider2.sbr" \
	"$(INTDIR)\util_vector.sbr" \
	"$(INTDIR)\vector.sbr" \
	"$(INTDIR)\ammohistory.sbr" \
	"$(INTDIR)\cdll_int.sbr" \
	"$(INTDIR)\cl_fx.sbr" \
	"$(INTDIR)\com_weapons.sbr" \
	"$(INTDIR)\demo.sbr" \
	"$(INTDIR)\entity.sbr" \
	"$(INTDIR)\in_camera.sbr" \
	"$(INTDIR)\input.sbr" \
	"$(INTDIR)\inputw32.sbr" \
	"$(INTDIR)\interface.sbr" \
	"$(INTDIR)\msg_fx.sbr" \
	"$(INTDIR)\musicplayer.sbr" \
	"$(INTDIR)\parsemsg.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\view.sbr"

"$(OUTDIR)\cl_dll.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib winmm.lib vgui.lib /nologo /version:3.3 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\client.pdb" /map:"$(INTDIR)\client.map" /machine:I386 /out:"$(OUTDIR)\client.dll" /implib:"$(OUTDIR)\client.lib" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\hl_baseentity.obj" \
	"$(INTDIR)\hl_weapons.obj" \
	"$(INTDIR)\pm_debug.obj" \
	"$(INTDIR)\pm_math.obj" \
	"$(INTDIR)\pm_shared.obj" \
	"$(INTDIR)\vgui_CommandMenu.obj" \
	"$(INTDIR)\vgui_ConsolePanel.obj" \
	"$(INTDIR)\vgui_CustomObjects.obj" \
	"$(INTDIR)\vgui_Int.obj" \
	"$(INTDIR)\vgui_MOTDWindow.obj" \
	"$(INTDIR)\vgui_MusicPlayer.obj" \
	"$(INTDIR)\vgui_SchemeManager.obj" \
	"$(INTDIR)\vgui_ScorePanel.obj" \
	"$(INTDIR)\vgui_ServerBrowser.obj" \
	"$(INTDIR)\vgui_SpectatorPanel.obj" \
	"$(INTDIR)\vgui_TeamFortressViewport.obj" \
	"$(INTDIR)\vgui_TeamMenu.obj" \
	"$(INTDIR)\voice_banmgr.obj" \
	"$(INTDIR)\voice_status.obj" \
	"$(INTDIR)\voice_vgui_tweakdlg.obj" \
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
	"$(INTDIR)\GameStudioModelRenderer.obj" \
	"$(INTDIR)\studio_util.obj" \
	"$(INTDIR)\StudioModelRenderer.obj" \
	"$(INTDIR)\tri.obj" \
	"$(INTDIR)\ammo.obj" \
	"$(INTDIR)\ammo_secondary.obj" \
	"$(INTDIR)\battery.obj" \
	"$(INTDIR)\death.obj" \
	"$(INTDIR)\flashlight.obj" \
	"$(INTDIR)\geiger.obj" \
	"$(INTDIR)\health.obj" \
	"$(INTDIR)\hud.obj" \
	"$(INTDIR)\hud_domdisplay.obj" \
	"$(INTDIR)\hud_flagdisplay.obj" \
	"$(INTDIR)\hud_msg.obj" \
	"$(INTDIR)\hud_redraw.obj" \
	"$(INTDIR)\hud_rocketscrn.obj" \
	"$(INTDIR)\hud_servers.obj" \
	"$(INTDIR)\hud_spectator.obj" \
	"$(INTDIR)\hud_update.obj" \
	"$(INTDIR)\hud_zcrosshair.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\saytext.obj" \
	"$(INTDIR)\status_icons.obj" \
	"$(INTDIR)\statusbar.obj" \
	"$(INTDIR)\text_message.obj" \
	"$(INTDIR)\train.obj" \
	"$(INTDIR)\Particle.obj" \
	"$(INTDIR)\PartSystem.obj" \
	"$(INTDIR)\PSBeam.obj" \
	"$(INTDIR)\PSDrips.obj" \
	"$(INTDIR)\PSFlameCone.obj" \
	"$(INTDIR)\PSFlatTrail.obj" \
	"$(INTDIR)\PSSparks.obj" \
	"$(INTDIR)\PSSpawnEffect.obj" \
	"$(INTDIR)\RenderManager.obj" \
	"$(INTDIR)\RenderSystem.obj" \
	"$(INTDIR)\RotatingSystem.obj" \
	"$(INTDIR)\RSBeam.obj" \
	"$(INTDIR)\RSBeamStar.obj" \
	"$(INTDIR)\RSCylinder.obj" \
	"$(INTDIR)\RSDelayed.obj" \
	"$(INTDIR)\RSDisk.obj" \
	"$(INTDIR)\RSLight.obj" \
	"$(INTDIR)\RSSphere.obj" \
	"$(INTDIR)\RSSprite.obj" \
	"$(INTDIR)\RSTeleparts.obj" \
	"$(INTDIR)\ev_common.obj" \
	"$(INTDIR)\ev_fx.obj" \
	"$(INTDIR)\ev_game.obj" \
	"$(INTDIR)\ev_hldm.obj" \
	"$(INTDIR)\events.obj" \
	"$(INTDIR)\vgui_checkbutton2.obj" \
	"$(INTDIR)\vgui_grid.obj" \
	"$(INTDIR)\vgui_helpers.obj" \
	"$(INTDIR)\vgui_listbox.obj" \
	"$(INTDIR)\vgui_loadtga.obj" \
	"$(INTDIR)\vgui_scrollbar2.obj" \
	"$(INTDIR)\vgui_slider2.obj" \
	"$(INTDIR)\util_vector.obj" \
	"$(INTDIR)\vector.obj" \
	"$(INTDIR)\ammohistory.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\cl_fx.obj" \
	"$(INTDIR)\com_weapons.obj" \
	"$(INTDIR)\demo.obj" \
	"$(INTDIR)\entity.obj" \
	"$(INTDIR)\in_camera.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\inputw32.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\msg_fx.obj" \
	"$(INTDIR)\musicplayer.obj" \
	"$(INTDIR)\parsemsg.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\view.obj" \
	"$(INTDIR)\cl_dll.res"

"$(OUTDIR)\client.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\release\client.dll
TargetName=client
InputPath=.\release\client.dll
SOURCE="$(InputPath)"

".\client" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	signtool  sign /a $(TargetPath) 
	copy       $(TargetPath)       \Half-Life\XDM\cl_dlls\  
	copy       $(TargetPath)       F:\games\HL1121\XDM\cl_dlls\  
	copy      $(TargetPath)      W:\dev\Half-Life\XDM\cl_dlls\
<< 
	

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

OUTDIR=.\debug
INTDIR=.\debug/intermediate
# Begin Custom Macros
OutDir=.\debug
# End Custom Macros

ALL : "$(OUTDIR)\client.dll" "$(OUTDIR)\cl_dll.bsc" ".\client"


CLEAN :
	-@erase "$(INTDIR)\alauncher.obj"
	-@erase "$(INTDIR)\alauncher.sbr"
	-@erase "$(INTDIR)\ammo.obj"
	-@erase "$(INTDIR)\ammo.sbr"
	-@erase "$(INTDIR)\ammo_secondary.obj"
	-@erase "$(INTDIR)\ammo_secondary.sbr"
	-@erase "$(INTDIR)\ammohistory.obj"
	-@erase "$(INTDIR)\ammohistory.sbr"
	-@erase "$(INTDIR)\battery.obj"
	-@erase "$(INTDIR)\battery.sbr"
	-@erase "$(INTDIR)\beamrifle.obj"
	-@erase "$(INTDIR)\beamrifle.sbr"
	-@erase "$(INTDIR)\cdll_int.obj"
	-@erase "$(INTDIR)\cdll_int.sbr"
	-@erase "$(INTDIR)\chemgun.obj"
	-@erase "$(INTDIR)\chemgun.sbr"
	-@erase "$(INTDIR)\cl_dll.res"
	-@erase "$(INTDIR)\cl_fx.obj"
	-@erase "$(INTDIR)\cl_fx.sbr"
	-@erase "$(INTDIR)\com_weapons.obj"
	-@erase "$(INTDIR)\com_weapons.sbr"
	-@erase "$(INTDIR)\crossbow.obj"
	-@erase "$(INTDIR)\crossbow.sbr"
	-@erase "$(INTDIR)\crowbar.obj"
	-@erase "$(INTDIR)\crowbar.sbr"
	-@erase "$(INTDIR)\death.obj"
	-@erase "$(INTDIR)\death.sbr"
	-@erase "$(INTDIR)\demo.obj"
	-@erase "$(INTDIR)\demo.sbr"
	-@erase "$(INTDIR)\displacer.obj"
	-@erase "$(INTDIR)\displacer.sbr"
	-@erase "$(INTDIR)\egon.obj"
	-@erase "$(INTDIR)\egon.sbr"
	-@erase "$(INTDIR)\entity.obj"
	-@erase "$(INTDIR)\entity.sbr"
	-@erase "$(INTDIR)\ev_common.obj"
	-@erase "$(INTDIR)\ev_common.sbr"
	-@erase "$(INTDIR)\ev_fx.obj"
	-@erase "$(INTDIR)\ev_fx.sbr"
	-@erase "$(INTDIR)\ev_game.obj"
	-@erase "$(INTDIR)\ev_game.sbr"
	-@erase "$(INTDIR)\ev_hldm.obj"
	-@erase "$(INTDIR)\ev_hldm.sbr"
	-@erase "$(INTDIR)\events.obj"
	-@erase "$(INTDIR)\events.sbr"
	-@erase "$(INTDIR)\flamethrower.obj"
	-@erase "$(INTDIR)\flamethrower.sbr"
	-@erase "$(INTDIR)\flashlight.obj"
	-@erase "$(INTDIR)\flashlight.sbr"
	-@erase "$(INTDIR)\GameStudioModelRenderer.obj"
	-@erase "$(INTDIR)\GameStudioModelRenderer.sbr"
	-@erase "$(INTDIR)\gauss.obj"
	-@erase "$(INTDIR)\gauss.sbr"
	-@erase "$(INTDIR)\geiger.obj"
	-@erase "$(INTDIR)\geiger.sbr"
	-@erase "$(INTDIR)\glauncher.obj"
	-@erase "$(INTDIR)\glauncher.sbr"
	-@erase "$(INTDIR)\glock.obj"
	-@erase "$(INTDIR)\glock.sbr"
	-@erase "$(INTDIR)\handgrenade.obj"
	-@erase "$(INTDIR)\handgrenade.sbr"
	-@erase "$(INTDIR)\health.obj"
	-@erase "$(INTDIR)\health.sbr"
	-@erase "$(INTDIR)\hl_baseentity.obj"
	-@erase "$(INTDIR)\hl_baseentity.sbr"
	-@erase "$(INTDIR)\hl_weapons.obj"
	-@erase "$(INTDIR)\hl_weapons.sbr"
	-@erase "$(INTDIR)\hornetgun.obj"
	-@erase "$(INTDIR)\hornetgun.sbr"
	-@erase "$(INTDIR)\hud.obj"
	-@erase "$(INTDIR)\hud.sbr"
	-@erase "$(INTDIR)\hud_domdisplay.obj"
	-@erase "$(INTDIR)\hud_domdisplay.sbr"
	-@erase "$(INTDIR)\hud_flagdisplay.obj"
	-@erase "$(INTDIR)\hud_flagdisplay.sbr"
	-@erase "$(INTDIR)\hud_msg.obj"
	-@erase "$(INTDIR)\hud_msg.sbr"
	-@erase "$(INTDIR)\hud_redraw.obj"
	-@erase "$(INTDIR)\hud_redraw.sbr"
	-@erase "$(INTDIR)\hud_rocketscrn.obj"
	-@erase "$(INTDIR)\hud_rocketscrn.sbr"
	-@erase "$(INTDIR)\hud_servers.obj"
	-@erase "$(INTDIR)\hud_servers.sbr"
	-@erase "$(INTDIR)\hud_spectator.obj"
	-@erase "$(INTDIR)\hud_spectator.sbr"
	-@erase "$(INTDIR)\hud_update.obj"
	-@erase "$(INTDIR)\hud_update.sbr"
	-@erase "$(INTDIR)\hud_zcrosshair.obj"
	-@erase "$(INTDIR)\hud_zcrosshair.sbr"
	-@erase "$(INTDIR)\in_camera.obj"
	-@erase "$(INTDIR)\in_camera.sbr"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\input.sbr"
	-@erase "$(INTDIR)\inputw32.obj"
	-@erase "$(INTDIR)\inputw32.sbr"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\interface.sbr"
	-@erase "$(INTDIR)\message.obj"
	-@erase "$(INTDIR)\message.sbr"
	-@erase "$(INTDIR)\mp5.obj"
	-@erase "$(INTDIR)\mp5.sbr"
	-@erase "$(INTDIR)\msg_fx.obj"
	-@erase "$(INTDIR)\msg_fx.sbr"
	-@erase "$(INTDIR)\musicplayer.obj"
	-@erase "$(INTDIR)\musicplayer.sbr"
	-@erase "$(INTDIR)\nanosword.obj"
	-@erase "$(INTDIR)\nanosword.sbr"
	-@erase "$(INTDIR)\orb.obj"
	-@erase "$(INTDIR)\orb.sbr"
	-@erase "$(INTDIR)\parsemsg.obj"
	-@erase "$(INTDIR)\parsemsg.sbr"
	-@erase "$(INTDIR)\Particle.obj"
	-@erase "$(INTDIR)\Particle.sbr"
	-@erase "$(INTDIR)\PartSystem.obj"
	-@erase "$(INTDIR)\PartSystem.sbr"
	-@erase "$(INTDIR)\plasma.obj"
	-@erase "$(INTDIR)\plasma.sbr"
	-@erase "$(INTDIR)\pm_debug.obj"
	-@erase "$(INTDIR)\pm_debug.sbr"
	-@erase "$(INTDIR)\pm_math.obj"
	-@erase "$(INTDIR)\pm_math.sbr"
	-@erase "$(INTDIR)\pm_shared.obj"
	-@erase "$(INTDIR)\pm_shared.sbr"
	-@erase "$(INTDIR)\PSBeam.obj"
	-@erase "$(INTDIR)\PSBeam.sbr"
	-@erase "$(INTDIR)\PSDrips.obj"
	-@erase "$(INTDIR)\PSDrips.sbr"
	-@erase "$(INTDIR)\PSFlameCone.obj"
	-@erase "$(INTDIR)\PSFlameCone.sbr"
	-@erase "$(INTDIR)\PSFlatTrail.obj"
	-@erase "$(INTDIR)\PSFlatTrail.sbr"
	-@erase "$(INTDIR)\PSSparks.obj"
	-@erase "$(INTDIR)\PSSparks.sbr"
	-@erase "$(INTDIR)\PSSpawnEffect.obj"
	-@erase "$(INTDIR)\PSSpawnEffect.sbr"
	-@erase "$(INTDIR)\python.obj"
	-@erase "$(INTDIR)\python.sbr"
	-@erase "$(INTDIR)\RenderManager.obj"
	-@erase "$(INTDIR)\RenderManager.sbr"
	-@erase "$(INTDIR)\RenderSystem.obj"
	-@erase "$(INTDIR)\RenderSystem.sbr"
	-@erase "$(INTDIR)\RotatingSystem.obj"
	-@erase "$(INTDIR)\RotatingSystem.sbr"
	-@erase "$(INTDIR)\rpg.obj"
	-@erase "$(INTDIR)\rpg.sbr"
	-@erase "$(INTDIR)\RSBeam.obj"
	-@erase "$(INTDIR)\RSBeam.sbr"
	-@erase "$(INTDIR)\RSBeamStar.obj"
	-@erase "$(INTDIR)\RSBeamStar.sbr"
	-@erase "$(INTDIR)\RSCylinder.obj"
	-@erase "$(INTDIR)\RSCylinder.sbr"
	-@erase "$(INTDIR)\RSDelayed.obj"
	-@erase "$(INTDIR)\RSDelayed.sbr"
	-@erase "$(INTDIR)\RSDisk.obj"
	-@erase "$(INTDIR)\RSDisk.sbr"
	-@erase "$(INTDIR)\RSLight.obj"
	-@erase "$(INTDIR)\RSLight.sbr"
	-@erase "$(INTDIR)\RSSphere.obj"
	-@erase "$(INTDIR)\RSSphere.sbr"
	-@erase "$(INTDIR)\RSSprite.obj"
	-@erase "$(INTDIR)\RSSprite.sbr"
	-@erase "$(INTDIR)\RSTeleparts.obj"
	-@erase "$(INTDIR)\RSTeleparts.sbr"
	-@erase "$(INTDIR)\satchel.obj"
	-@erase "$(INTDIR)\satchel.sbr"
	-@erase "$(INTDIR)\saytext.obj"
	-@erase "$(INTDIR)\saytext.sbr"
	-@erase "$(INTDIR)\shotgun.obj"
	-@erase "$(INTDIR)\shotgun.sbr"
	-@erase "$(INTDIR)\squeakgrenade.obj"
	-@erase "$(INTDIR)\squeakgrenade.sbr"
	-@erase "$(INTDIR)\status_icons.obj"
	-@erase "$(INTDIR)\status_icons.sbr"
	-@erase "$(INTDIR)\statusbar.obj"
	-@erase "$(INTDIR)\statusbar.sbr"
	-@erase "$(INTDIR)\strtarget.obj"
	-@erase "$(INTDIR)\strtarget.sbr"
	-@erase "$(INTDIR)\studio_util.obj"
	-@erase "$(INTDIR)\studio_util.sbr"
	-@erase "$(INTDIR)\StudioModelRenderer.obj"
	-@erase "$(INTDIR)\StudioModelRenderer.sbr"
	-@erase "$(INTDIR)\sword.obj"
	-@erase "$(INTDIR)\sword.sbr"
	-@erase "$(INTDIR)\text_message.obj"
	-@erase "$(INTDIR)\text_message.sbr"
	-@erase "$(INTDIR)\train.obj"
	-@erase "$(INTDIR)\train.sbr"
	-@erase "$(INTDIR)\tri.obj"
	-@erase "$(INTDIR)\tri.sbr"
	-@erase "$(INTDIR)\tripmine.obj"
	-@erase "$(INTDIR)\tripmine.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\util_vector.obj"
	-@erase "$(INTDIR)\util_vector.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vector.obj"
	-@erase "$(INTDIR)\vector.sbr"
	-@erase "$(INTDIR)\vgui_checkbutton2.obj"
	-@erase "$(INTDIR)\vgui_checkbutton2.sbr"
	-@erase "$(INTDIR)\vgui_CommandMenu.obj"
	-@erase "$(INTDIR)\vgui_CommandMenu.sbr"
	-@erase "$(INTDIR)\vgui_ConsolePanel.obj"
	-@erase "$(INTDIR)\vgui_ConsolePanel.sbr"
	-@erase "$(INTDIR)\vgui_CustomObjects.obj"
	-@erase "$(INTDIR)\vgui_CustomObjects.sbr"
	-@erase "$(INTDIR)\vgui_grid.obj"
	-@erase "$(INTDIR)\vgui_grid.sbr"
	-@erase "$(INTDIR)\vgui_helpers.obj"
	-@erase "$(INTDIR)\vgui_helpers.sbr"
	-@erase "$(INTDIR)\vgui_Int.obj"
	-@erase "$(INTDIR)\vgui_Int.sbr"
	-@erase "$(INTDIR)\vgui_listbox.obj"
	-@erase "$(INTDIR)\vgui_listbox.sbr"
	-@erase "$(INTDIR)\vgui_loadtga.obj"
	-@erase "$(INTDIR)\vgui_loadtga.sbr"
	-@erase "$(INTDIR)\vgui_MOTDWindow.obj"
	-@erase "$(INTDIR)\vgui_MOTDWindow.sbr"
	-@erase "$(INTDIR)\vgui_MusicPlayer.obj"
	-@erase "$(INTDIR)\vgui_MusicPlayer.sbr"
	-@erase "$(INTDIR)\vgui_SchemeManager.obj"
	-@erase "$(INTDIR)\vgui_SchemeManager.sbr"
	-@erase "$(INTDIR)\vgui_ScorePanel.obj"
	-@erase "$(INTDIR)\vgui_ScorePanel.sbr"
	-@erase "$(INTDIR)\vgui_scrollbar2.obj"
	-@erase "$(INTDIR)\vgui_scrollbar2.sbr"
	-@erase "$(INTDIR)\vgui_ServerBrowser.obj"
	-@erase "$(INTDIR)\vgui_ServerBrowser.sbr"
	-@erase "$(INTDIR)\vgui_slider2.obj"
	-@erase "$(INTDIR)\vgui_slider2.sbr"
	-@erase "$(INTDIR)\vgui_SpectatorPanel.obj"
	-@erase "$(INTDIR)\vgui_SpectatorPanel.sbr"
	-@erase "$(INTDIR)\vgui_TeamFortressViewport.obj"
	-@erase "$(INTDIR)\vgui_TeamFortressViewport.sbr"
	-@erase "$(INTDIR)\vgui_TeamMenu.obj"
	-@erase "$(INTDIR)\vgui_TeamMenu.sbr"
	-@erase "$(INTDIR)\view.obj"
	-@erase "$(INTDIR)\view.sbr"
	-@erase "$(INTDIR)\voice_banmgr.obj"
	-@erase "$(INTDIR)\voice_banmgr.sbr"
	-@erase "$(INTDIR)\voice_status.obj"
	-@erase "$(INTDIR)\voice_status.sbr"
	-@erase "$(INTDIR)\voice_vgui_tweakdlg.obj"
	-@erase "$(INTDIR)\voice_vgui_tweakdlg.sbr"
	-@erase "$(OUTDIR)\cl_dll.bsc"
	-@erase "$(OUTDIR)\client.dll"
	-@erase "$(OUTDIR)\client.exp"
	-@erase "$(OUTDIR)\client.pdb"
	-@erase "$(OUTDIR)\intermediate\client.map"
	-@erase "client"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "." /I "rendersystem" /I "..\dlls" /I "..\dlls\gamerules" /I "..\game_shared" /I "..\vgui_include" /I "..\engine" /I "..\common" /I "..\pm_shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "CLIENT_DLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\cl_dll.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cl_dll.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\hl_baseentity.sbr" \
	"$(INTDIR)\hl_weapons.sbr" \
	"$(INTDIR)\pm_debug.sbr" \
	"$(INTDIR)\pm_math.sbr" \
	"$(INTDIR)\pm_shared.sbr" \
	"$(INTDIR)\vgui_CommandMenu.sbr" \
	"$(INTDIR)\vgui_ConsolePanel.sbr" \
	"$(INTDIR)\vgui_CustomObjects.sbr" \
	"$(INTDIR)\vgui_Int.sbr" \
	"$(INTDIR)\vgui_MOTDWindow.sbr" \
	"$(INTDIR)\vgui_MusicPlayer.sbr" \
	"$(INTDIR)\vgui_SchemeManager.sbr" \
	"$(INTDIR)\vgui_ScorePanel.sbr" \
	"$(INTDIR)\vgui_ServerBrowser.sbr" \
	"$(INTDIR)\vgui_SpectatorPanel.sbr" \
	"$(INTDIR)\vgui_TeamFortressViewport.sbr" \
	"$(INTDIR)\vgui_TeamMenu.sbr" \
	"$(INTDIR)\voice_banmgr.sbr" \
	"$(INTDIR)\voice_status.sbr" \
	"$(INTDIR)\voice_vgui_tweakdlg.sbr" \
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
	"$(INTDIR)\GameStudioModelRenderer.sbr" \
	"$(INTDIR)\studio_util.sbr" \
	"$(INTDIR)\StudioModelRenderer.sbr" \
	"$(INTDIR)\tri.sbr" \
	"$(INTDIR)\ammo.sbr" \
	"$(INTDIR)\ammo_secondary.sbr" \
	"$(INTDIR)\battery.sbr" \
	"$(INTDIR)\death.sbr" \
	"$(INTDIR)\flashlight.sbr" \
	"$(INTDIR)\geiger.sbr" \
	"$(INTDIR)\health.sbr" \
	"$(INTDIR)\hud.sbr" \
	"$(INTDIR)\hud_domdisplay.sbr" \
	"$(INTDIR)\hud_flagdisplay.sbr" \
	"$(INTDIR)\hud_msg.sbr" \
	"$(INTDIR)\hud_redraw.sbr" \
	"$(INTDIR)\hud_rocketscrn.sbr" \
	"$(INTDIR)\hud_servers.sbr" \
	"$(INTDIR)\hud_spectator.sbr" \
	"$(INTDIR)\hud_update.sbr" \
	"$(INTDIR)\hud_zcrosshair.sbr" \
	"$(INTDIR)\message.sbr" \
	"$(INTDIR)\saytext.sbr" \
	"$(INTDIR)\status_icons.sbr" \
	"$(INTDIR)\statusbar.sbr" \
	"$(INTDIR)\text_message.sbr" \
	"$(INTDIR)\train.sbr" \
	"$(INTDIR)\Particle.sbr" \
	"$(INTDIR)\PartSystem.sbr" \
	"$(INTDIR)\PSBeam.sbr" \
	"$(INTDIR)\PSDrips.sbr" \
	"$(INTDIR)\PSFlameCone.sbr" \
	"$(INTDIR)\PSFlatTrail.sbr" \
	"$(INTDIR)\PSSparks.sbr" \
	"$(INTDIR)\PSSpawnEffect.sbr" \
	"$(INTDIR)\RenderManager.sbr" \
	"$(INTDIR)\RenderSystem.sbr" \
	"$(INTDIR)\RotatingSystem.sbr" \
	"$(INTDIR)\RSBeam.sbr" \
	"$(INTDIR)\RSBeamStar.sbr" \
	"$(INTDIR)\RSCylinder.sbr" \
	"$(INTDIR)\RSDelayed.sbr" \
	"$(INTDIR)\RSDisk.sbr" \
	"$(INTDIR)\RSLight.sbr" \
	"$(INTDIR)\RSSphere.sbr" \
	"$(INTDIR)\RSSprite.sbr" \
	"$(INTDIR)\RSTeleparts.sbr" \
	"$(INTDIR)\ev_common.sbr" \
	"$(INTDIR)\ev_fx.sbr" \
	"$(INTDIR)\ev_game.sbr" \
	"$(INTDIR)\ev_hldm.sbr" \
	"$(INTDIR)\events.sbr" \
	"$(INTDIR)\vgui_checkbutton2.sbr" \
	"$(INTDIR)\vgui_grid.sbr" \
	"$(INTDIR)\vgui_helpers.sbr" \
	"$(INTDIR)\vgui_listbox.sbr" \
	"$(INTDIR)\vgui_loadtga.sbr" \
	"$(INTDIR)\vgui_scrollbar2.sbr" \
	"$(INTDIR)\vgui_slider2.sbr" \
	"$(INTDIR)\util_vector.sbr" \
	"$(INTDIR)\vector.sbr" \
	"$(INTDIR)\ammohistory.sbr" \
	"$(INTDIR)\cdll_int.sbr" \
	"$(INTDIR)\cl_fx.sbr" \
	"$(INTDIR)\com_weapons.sbr" \
	"$(INTDIR)\demo.sbr" \
	"$(INTDIR)\entity.sbr" \
	"$(INTDIR)\in_camera.sbr" \
	"$(INTDIR)\input.sbr" \
	"$(INTDIR)\inputw32.sbr" \
	"$(INTDIR)\interface.sbr" \
	"$(INTDIR)\msg_fx.sbr" \
	"$(INTDIR)\musicplayer.sbr" \
	"$(INTDIR)\parsemsg.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\view.sbr"

"$(OUTDIR)\cl_dll.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib winmm.lib vgui.lib /nologo /version:3.3 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\client.pdb" /map:"$(INTDIR)\client.map" /debug /machine:I386 /out:"$(OUTDIR)\client.dll" /implib:"$(OUTDIR)\client.lib" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\hl_baseentity.obj" \
	"$(INTDIR)\hl_weapons.obj" \
	"$(INTDIR)\pm_debug.obj" \
	"$(INTDIR)\pm_math.obj" \
	"$(INTDIR)\pm_shared.obj" \
	"$(INTDIR)\vgui_CommandMenu.obj" \
	"$(INTDIR)\vgui_ConsolePanel.obj" \
	"$(INTDIR)\vgui_CustomObjects.obj" \
	"$(INTDIR)\vgui_Int.obj" \
	"$(INTDIR)\vgui_MOTDWindow.obj" \
	"$(INTDIR)\vgui_MusicPlayer.obj" \
	"$(INTDIR)\vgui_SchemeManager.obj" \
	"$(INTDIR)\vgui_ScorePanel.obj" \
	"$(INTDIR)\vgui_ServerBrowser.obj" \
	"$(INTDIR)\vgui_SpectatorPanel.obj" \
	"$(INTDIR)\vgui_TeamFortressViewport.obj" \
	"$(INTDIR)\vgui_TeamMenu.obj" \
	"$(INTDIR)\voice_banmgr.obj" \
	"$(INTDIR)\voice_status.obj" \
	"$(INTDIR)\voice_vgui_tweakdlg.obj" \
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
	"$(INTDIR)\GameStudioModelRenderer.obj" \
	"$(INTDIR)\studio_util.obj" \
	"$(INTDIR)\StudioModelRenderer.obj" \
	"$(INTDIR)\tri.obj" \
	"$(INTDIR)\ammo.obj" \
	"$(INTDIR)\ammo_secondary.obj" \
	"$(INTDIR)\battery.obj" \
	"$(INTDIR)\death.obj" \
	"$(INTDIR)\flashlight.obj" \
	"$(INTDIR)\geiger.obj" \
	"$(INTDIR)\health.obj" \
	"$(INTDIR)\hud.obj" \
	"$(INTDIR)\hud_domdisplay.obj" \
	"$(INTDIR)\hud_flagdisplay.obj" \
	"$(INTDIR)\hud_msg.obj" \
	"$(INTDIR)\hud_redraw.obj" \
	"$(INTDIR)\hud_rocketscrn.obj" \
	"$(INTDIR)\hud_servers.obj" \
	"$(INTDIR)\hud_spectator.obj" \
	"$(INTDIR)\hud_update.obj" \
	"$(INTDIR)\hud_zcrosshair.obj" \
	"$(INTDIR)\message.obj" \
	"$(INTDIR)\saytext.obj" \
	"$(INTDIR)\status_icons.obj" \
	"$(INTDIR)\statusbar.obj" \
	"$(INTDIR)\text_message.obj" \
	"$(INTDIR)\train.obj" \
	"$(INTDIR)\Particle.obj" \
	"$(INTDIR)\PartSystem.obj" \
	"$(INTDIR)\PSBeam.obj" \
	"$(INTDIR)\PSDrips.obj" \
	"$(INTDIR)\PSFlameCone.obj" \
	"$(INTDIR)\PSFlatTrail.obj" \
	"$(INTDIR)\PSSparks.obj" \
	"$(INTDIR)\PSSpawnEffect.obj" \
	"$(INTDIR)\RenderManager.obj" \
	"$(INTDIR)\RenderSystem.obj" \
	"$(INTDIR)\RotatingSystem.obj" \
	"$(INTDIR)\RSBeam.obj" \
	"$(INTDIR)\RSBeamStar.obj" \
	"$(INTDIR)\RSCylinder.obj" \
	"$(INTDIR)\RSDelayed.obj" \
	"$(INTDIR)\RSDisk.obj" \
	"$(INTDIR)\RSLight.obj" \
	"$(INTDIR)\RSSphere.obj" \
	"$(INTDIR)\RSSprite.obj" \
	"$(INTDIR)\RSTeleparts.obj" \
	"$(INTDIR)\ev_common.obj" \
	"$(INTDIR)\ev_fx.obj" \
	"$(INTDIR)\ev_game.obj" \
	"$(INTDIR)\ev_hldm.obj" \
	"$(INTDIR)\events.obj" \
	"$(INTDIR)\vgui_checkbutton2.obj" \
	"$(INTDIR)\vgui_grid.obj" \
	"$(INTDIR)\vgui_helpers.obj" \
	"$(INTDIR)\vgui_listbox.obj" \
	"$(INTDIR)\vgui_loadtga.obj" \
	"$(INTDIR)\vgui_scrollbar2.obj" \
	"$(INTDIR)\vgui_slider2.obj" \
	"$(INTDIR)\util_vector.obj" \
	"$(INTDIR)\vector.obj" \
	"$(INTDIR)\ammohistory.obj" \
	"$(INTDIR)\cdll_int.obj" \
	"$(INTDIR)\cl_fx.obj" \
	"$(INTDIR)\com_weapons.obj" \
	"$(INTDIR)\demo.obj" \
	"$(INTDIR)\entity.obj" \
	"$(INTDIR)\in_camera.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\inputw32.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\msg_fx.obj" \
	"$(INTDIR)\musicplayer.obj" \
	"$(INTDIR)\parsemsg.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\view.obj" \
	"$(INTDIR)\cl_dll.res"

"$(OUTDIR)\client.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=.\debug\client.dll
TargetName=client
InputPath=.\debug\client.dll
SOURCE="$(InputPath)"

".\client" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy       $(TargetPath)       \Half-Life\XDM\cl_dlls\  
	copy       $(TargetPath)       F:\games\HL1121\XDM\cl_dlls\  
	copy      $(TargetPath)      W:\dev\Half-Life\XDM\cl_dlls\
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
!IF EXISTS("cl_dll.dep")
!INCLUDE "cl_dll.dep"
!ELSE 
!MESSAGE Warning: cannot find "cl_dll.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cl_dll - Win32 Release" || "$(CFG)" == "cl_dll - Win32 Debug"
SOURCE=.\hl\hl_baseentity.cpp

"$(INTDIR)\hl_baseentity.obj"	"$(INTDIR)\hl_baseentity.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\hl\hl_weapons.cpp

"$(INTDIR)\hl_weapons.obj"	"$(INTDIR)\hl_weapons.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\pm_shared\pm_debug.c

"$(INTDIR)\pm_debug.obj"	"$(INTDIR)\pm_debug.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\pm_shared\pm_math.c

"$(INTDIR)\pm_math.obj"	"$(INTDIR)\pm_math.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\pm_shared\pm_shared.c

"$(INTDIR)\pm_shared.obj"	"$(INTDIR)\pm_shared.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\vgui_CommandMenu.cpp

"$(INTDIR)\vgui_CommandMenu.obj"	"$(INTDIR)\vgui_CommandMenu.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_ConsolePanel.cpp

"$(INTDIR)\vgui_ConsolePanel.obj"	"$(INTDIR)\vgui_ConsolePanel.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_ControlConfigPanel.cpp
SOURCE=.\vgui_CustomObjects.cpp

"$(INTDIR)\vgui_CustomObjects.obj"	"$(INTDIR)\vgui_CustomObjects.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_Int.cpp

"$(INTDIR)\vgui_Int.obj"	"$(INTDIR)\vgui_Int.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_MOTDWindow.cpp

"$(INTDIR)\vgui_MOTDWindow.obj"	"$(INTDIR)\vgui_MOTDWindow.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_MusicPlayer.cpp

"$(INTDIR)\vgui_MusicPlayer.obj"	"$(INTDIR)\vgui_MusicPlayer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_SchemeManager.cpp

"$(INTDIR)\vgui_SchemeManager.obj"	"$(INTDIR)\vgui_SchemeManager.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_ScorePanel.cpp

"$(INTDIR)\vgui_ScorePanel.obj"	"$(INTDIR)\vgui_ScorePanel.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_ServerBrowser.cpp

"$(INTDIR)\vgui_ServerBrowser.obj"	"$(INTDIR)\vgui_ServerBrowser.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_SpectatorPanel.cpp

"$(INTDIR)\vgui_SpectatorPanel.obj"	"$(INTDIR)\vgui_SpectatorPanel.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_TeamFortressViewport.cpp

"$(INTDIR)\vgui_TeamFortressViewport.obj"	"$(INTDIR)\vgui_TeamFortressViewport.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vgui_TeamMenu.cpp

"$(INTDIR)\vgui_TeamMenu.obj"	"$(INTDIR)\vgui_TeamMenu.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=..\game_shared\voice_banmgr.cpp

"$(INTDIR)\voice_banmgr.obj"	"$(INTDIR)\voice_banmgr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\voice_status.cpp

"$(INTDIR)\voice_status.obj"	"$(INTDIR)\voice_status.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\voice_vgui_tweakdlg.cpp

"$(INTDIR)\voice_vgui_tweakdlg.obj"	"$(INTDIR)\voice_vgui_tweakdlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\alauncher.cpp

"$(INTDIR)\alauncher.obj"	"$(INTDIR)\alauncher.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\beamrifle.cpp

"$(INTDIR)\beamrifle.obj"	"$(INTDIR)\beamrifle.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\chemgun.cpp

"$(INTDIR)\chemgun.obj"	"$(INTDIR)\chemgun.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\crossbow.cpp

"$(INTDIR)\crossbow.obj"	"$(INTDIR)\crossbow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\crowbar.cpp

"$(INTDIR)\crowbar.obj"	"$(INTDIR)\crowbar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\displacer.cpp

"$(INTDIR)\displacer.obj"	"$(INTDIR)\displacer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\egon.cpp

"$(INTDIR)\egon.obj"	"$(INTDIR)\egon.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\flamethrower.cpp

"$(INTDIR)\flamethrower.obj"	"$(INTDIR)\flamethrower.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\gauss.cpp

"$(INTDIR)\gauss.obj"	"$(INTDIR)\gauss.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\glauncher.cpp

"$(INTDIR)\glauncher.obj"	"$(INTDIR)\glauncher.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\glock.cpp

"$(INTDIR)\glock.obj"	"$(INTDIR)\glock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\handgrenade.cpp

"$(INTDIR)\handgrenade.obj"	"$(INTDIR)\handgrenade.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\hornetgun.cpp

"$(INTDIR)\hornetgun.obj"	"$(INTDIR)\hornetgun.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\mp5.cpp

"$(INTDIR)\mp5.obj"	"$(INTDIR)\mp5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\nanosword.cpp

"$(INTDIR)\nanosword.obj"	"$(INTDIR)\nanosword.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\orb.cpp

"$(INTDIR)\orb.obj"	"$(INTDIR)\orb.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\plasma.cpp

"$(INTDIR)\plasma.obj"	"$(INTDIR)\plasma.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\python.cpp

"$(INTDIR)\python.obj"	"$(INTDIR)\python.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\rpg.cpp

"$(INTDIR)\rpg.obj"	"$(INTDIR)\rpg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\satchel.cpp

"$(INTDIR)\satchel.obj"	"$(INTDIR)\satchel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\shotgun.cpp

"$(INTDIR)\shotgun.obj"	"$(INTDIR)\shotgun.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\squeakgrenade.cpp

"$(INTDIR)\squeakgrenade.obj"	"$(INTDIR)\squeakgrenade.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\strtarget.cpp

"$(INTDIR)\strtarget.obj"	"$(INTDIR)\strtarget.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\sword.cpp

"$(INTDIR)\sword.obj"	"$(INTDIR)\sword.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\dlls\weapons\tripmine.cpp

"$(INTDIR)\tripmine.obj"	"$(INTDIR)\tripmine.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\GameStudioModelRenderer.cpp

"$(INTDIR)\GameStudioModelRenderer.obj"	"$(INTDIR)\GameStudioModelRenderer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\studio_util.cpp

"$(INTDIR)\studio_util.obj"	"$(INTDIR)\studio_util.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StudioModelRenderer.cpp

"$(INTDIR)\StudioModelRenderer.obj"	"$(INTDIR)\StudioModelRenderer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tri.cpp

"$(INTDIR)\tri.obj"	"$(INTDIR)\tri.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ammo.cpp

"$(INTDIR)\ammo.obj"	"$(INTDIR)\ammo.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ammo_secondary.cpp

"$(INTDIR)\ammo_secondary.obj"	"$(INTDIR)\ammo_secondary.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\battery.cpp

"$(INTDIR)\battery.obj"	"$(INTDIR)\battery.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\death.cpp

"$(INTDIR)\death.obj"	"$(INTDIR)\death.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\flashlight.cpp

"$(INTDIR)\flashlight.obj"	"$(INTDIR)\flashlight.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\geiger.cpp

"$(INTDIR)\geiger.obj"	"$(INTDIR)\geiger.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\health.cpp

"$(INTDIR)\health.obj"	"$(INTDIR)\health.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud.cpp

"$(INTDIR)\hud.obj"	"$(INTDIR)\hud.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud_domdisplay.cpp

"$(INTDIR)\hud_domdisplay.obj"	"$(INTDIR)\hud_domdisplay.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud_flagdisplay.cpp

"$(INTDIR)\hud_flagdisplay.obj"	"$(INTDIR)\hud_flagdisplay.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud_msg.cpp

"$(INTDIR)\hud_msg.obj"	"$(INTDIR)\hud_msg.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud_redraw.cpp

"$(INTDIR)\hud_redraw.obj"	"$(INTDIR)\hud_redraw.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud_rocketscrn.cpp

"$(INTDIR)\hud_rocketscrn.obj"	"$(INTDIR)\hud_rocketscrn.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud_servers.cpp

"$(INTDIR)\hud_servers.obj"	"$(INTDIR)\hud_servers.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud_spectator.cpp

"$(INTDIR)\hud_spectator.obj"	"$(INTDIR)\hud_spectator.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud_update.cpp

"$(INTDIR)\hud_update.obj"	"$(INTDIR)\hud_update.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud_zcrosshair.cpp

"$(INTDIR)\hud_zcrosshair.obj"	"$(INTDIR)\hud_zcrosshair.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\message.cpp

"$(INTDIR)\message.obj"	"$(INTDIR)\message.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\saytext.cpp

"$(INTDIR)\saytext.obj"	"$(INTDIR)\saytext.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\status_icons.cpp

"$(INTDIR)\status_icons.obj"	"$(INTDIR)\status_icons.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\statusbar.cpp

"$(INTDIR)\statusbar.obj"	"$(INTDIR)\statusbar.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\text_message.cpp

"$(INTDIR)\text_message.obj"	"$(INTDIR)\text_message.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\train.cpp

"$(INTDIR)\train.obj"	"$(INTDIR)\train.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rendersystem\Particle.cpp

"$(INTDIR)\Particle.obj"	"$(INTDIR)\Particle.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\PartSystem.cpp

"$(INTDIR)\PartSystem.obj"	"$(INTDIR)\PartSystem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\PSBeam.cpp

"$(INTDIR)\PSBeam.obj"	"$(INTDIR)\PSBeam.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\PSDrips.cpp

"$(INTDIR)\PSDrips.obj"	"$(INTDIR)\PSDrips.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\PSFlameCone.cpp

"$(INTDIR)\PSFlameCone.obj"	"$(INTDIR)\PSFlameCone.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\PSFlatTrail.cpp

"$(INTDIR)\PSFlatTrail.obj"	"$(INTDIR)\PSFlatTrail.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\PSSparks.cpp

"$(INTDIR)\PSSparks.obj"	"$(INTDIR)\PSSparks.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\PSSpawnEffect.cpp

"$(INTDIR)\PSSpawnEffect.obj"	"$(INTDIR)\PSSpawnEffect.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RenderManager.cpp

"$(INTDIR)\RenderManager.obj"	"$(INTDIR)\RenderManager.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RenderSystem.cpp

"$(INTDIR)\RenderSystem.obj"	"$(INTDIR)\RenderSystem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RotatingSystem.cpp

"$(INTDIR)\RotatingSystem.obj"	"$(INTDIR)\RotatingSystem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RSBeam.cpp

"$(INTDIR)\RSBeam.obj"	"$(INTDIR)\RSBeam.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RSBeamStar.cpp

"$(INTDIR)\RSBeamStar.obj"	"$(INTDIR)\RSBeamStar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RSCylinder.cpp

"$(INTDIR)\RSCylinder.obj"	"$(INTDIR)\RSCylinder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RSDelayed.cpp

"$(INTDIR)\RSDelayed.obj"	"$(INTDIR)\RSDelayed.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RSDisk.cpp

"$(INTDIR)\RSDisk.obj"	"$(INTDIR)\RSDisk.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RSLight.cpp

"$(INTDIR)\RSLight.obj"	"$(INTDIR)\RSLight.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RSSphere.cpp

"$(INTDIR)\RSSphere.obj"	"$(INTDIR)\RSSphere.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RSSprite.cpp

"$(INTDIR)\RSSprite.obj"	"$(INTDIR)\RSSprite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\rendersystem\RSTeleparts.cpp

"$(INTDIR)\RSTeleparts.obj"	"$(INTDIR)\RSTeleparts.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ev_common.cpp

"$(INTDIR)\ev_common.obj"	"$(INTDIR)\ev_common.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ev_fx.cpp

"$(INTDIR)\ev_fx.obj"	"$(INTDIR)\ev_fx.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ev_game.cpp

"$(INTDIR)\ev_game.obj"	"$(INTDIR)\ev_game.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ev_hldm.cpp

"$(INTDIR)\ev_hldm.obj"	"$(INTDIR)\ev_hldm.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\events.cpp

"$(INTDIR)\events.obj"	"$(INTDIR)\events.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=..\game_shared\vgui_checkbutton2.cpp

"$(INTDIR)\vgui_checkbutton2.obj"	"$(INTDIR)\vgui_checkbutton2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\vgui_grid.cpp

"$(INTDIR)\vgui_grid.obj"	"$(INTDIR)\vgui_grid.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\vgui_helpers.cpp

"$(INTDIR)\vgui_helpers.obj"	"$(INTDIR)\vgui_helpers.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\vgui_listbox.cpp

"$(INTDIR)\vgui_listbox.obj"	"$(INTDIR)\vgui_listbox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\vgui_loadtga.cpp

"$(INTDIR)\vgui_loadtga.obj"	"$(INTDIR)\vgui_loadtga.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\vgui_scrollbar2.cpp

"$(INTDIR)\vgui_scrollbar2.obj"	"$(INTDIR)\vgui_scrollbar2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\game_shared\vgui_slider2.cpp

"$(INTDIR)\vgui_slider2.obj"	"$(INTDIR)\vgui_slider2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\util_vector.cpp

"$(INTDIR)\util_vector.obj"	"$(INTDIR)\util_vector.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\vector.cpp

"$(INTDIR)\vector.obj"	"$(INTDIR)\vector.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ammohistory.cpp

"$(INTDIR)\ammohistory.obj"	"$(INTDIR)\ammohistory.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cdll_int.cpp

"$(INTDIR)\cdll_int.obj"	"$(INTDIR)\cdll_int.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cl_fx.cpp

"$(INTDIR)\cl_fx.obj"	"$(INTDIR)\cl_fx.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\com_weapons.cpp

"$(INTDIR)\com_weapons.obj"	"$(INTDIR)\com_weapons.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\demo.cpp

"$(INTDIR)\demo.obj"	"$(INTDIR)\demo.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\entity.cpp

"$(INTDIR)\entity.obj"	"$(INTDIR)\entity.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\in_camera.cpp

"$(INTDIR)\in_camera.obj"	"$(INTDIR)\in_camera.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\input.cpp

"$(INTDIR)\input.obj"	"$(INTDIR)\input.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\inputw32.cpp

"$(INTDIR)\inputw32.obj"	"$(INTDIR)\inputw32.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=..\common\interface.cpp

"$(INTDIR)\interface.obj"	"$(INTDIR)\interface.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\msg_fx.cpp

"$(INTDIR)\msg_fx.obj"	"$(INTDIR)\msg_fx.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\musicplayer.cpp

"$(INTDIR)\musicplayer.obj"	"$(INTDIR)\musicplayer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\parsemsg.cpp

"$(INTDIR)\parsemsg.obj"	"$(INTDIR)\parsemsg.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\util.cpp

"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\view.cpp

"$(INTDIR)\view.obj"	"$(INTDIR)\view.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cl_dll.rc

"$(INTDIR)\cl_dll.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

