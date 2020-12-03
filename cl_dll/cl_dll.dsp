# Microsoft Developer Studio Project File - Name="cl_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=cl_dll - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cl_dll.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""cl_dll", HGEBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cl_dll - Win32 Release"

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
# ADD CPP /nologo /MD /W4 /GX /Zi /I "." /I "rendersystem" /I "..\dlls" /I "..\dlls\gamerules" /I "..\game_shared" /I "..\vgui_include" /I "..\engine" /I "..\common" /I "..\pm_shared" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "CLIENT_DLL" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 user32.lib winmm.lib vgui.lib /nologo /version:3.3 /subsystem:windows /dll /map /debug /machine:I386 /out:"release/client.dll" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Copying to cl_dlls
TargetPath=.\release\client.dll
TargetName=client
InputPath=.\release\client.dll
SOURCE="$(InputPath)"

"%HLROOT%\TYRIAN\cl_dlls\$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	signtool sign /a $(TargetPath) 
	copy                                                  $(TargetPath)                                                  %HLROOT%\TYRIAN\cl_dlls\  
	
# End Custom Build

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug/intermediate"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /I "." /I "rendersystem" /I "..\dlls" /I "..\dlls\gamerules" /I "..\game_shared" /I "..\vgui_include" /I "..\engine" /I "..\common" /I "..\pm_shared" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "CLIENT_DLL" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib wsock32.lib winmm.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 user32.lib winmm.lib vgui.lib /nologo /version:3.3 /subsystem:windows /dll /incremental:no /map /debug /machine:I386 /out:"debug/client.dll" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build - Copying to cl_dlls
TargetPath=.\debug\client.dll
TargetName=client
InputPath=.\debug\client.dll
SOURCE="$(InputPath)"

"%HLROOT%\TYRIAN\cl_dlls\$(TargetName)" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                                                  $(TargetPath)                                                  %HLROOT%\TYRIAN\cl_dlls\ 

# End Custom Build

!ENDIF 

# Begin Target

# Name "cl_dll - Win32 Release"
# Name "cl_dll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx"
# Begin Group "hl code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\hl\hl_baseentity.cpp
# End Source File
# Begin Source File

SOURCE=.\hl\hl_weapons.cpp
# End Source File
# End Group
# Begin Group "pm code"

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
# End Group
# Begin Group "vgui code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\vgui_CommandMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ConsolePanel.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\vgui_ControlConfigPanel.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\vgui_CustomObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_EntityEntryPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_Int.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_MenuPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_MOTDWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_MusicPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_PSEditorPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_SchemeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ScorePanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ServerBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_SpectatorPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_TeamMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_XDMViewport.cpp
# End Source File
# End Group
# Begin Group "voice code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=..\game_shared\voice_banmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_status.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_vgui_tweakdlg.cpp
# End Source File
# End Group
# Begin Group "studio code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\GameStudioModelRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\studio_util.cpp
# End Source File
# Begin Source File

SOURCE=.\StudioModelRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\tri.cpp
# End Source File
# End Group
# Begin Group "hud code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\ammo.cpp
# End Source File
# Begin Source File

SOURCE=.\ammo_secondary.cpp
# End Source File
# Begin Source File

SOURCE=.\battery.cpp
# End Source File
# Begin Source File

SOURCE=.\death.cpp
# End Source File
# Begin Source File

SOURCE=.\flashlight.cpp
# End Source File
# Begin Source File

SOURCE=.\geiger.cpp
# End Source File
# Begin Source File

SOURCE=.\hud.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_domdisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_flagdisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_gamerules.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_health.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_accuracy.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_antidote.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_banana.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_fire_supressor.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_haste.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_invisibility.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_invulnerability.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_lightning_field.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_plasma_shield.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_quaddamage.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_radshield.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_rapidfire.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_shield_strength.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_shieldregen.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_item_weapon_power.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_msg.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_redraw.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_rocketscrn.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_servers.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_spectator.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_update.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_wpn_icon.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_zcrosshair.cpp
# End Source File
# Begin Source File

SOURCE=.\message.cpp
# End Source File
# Begin Source File

SOURCE=.\msg_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\saytext.cpp
# End Source File
# Begin Source File

SOURCE=.\status_icons.cpp
# End Source File
# Begin Source File

SOURCE=.\statusbar.cpp
# End Source File
# Begin Source File

SOURCE=.\text_message.cpp
# End Source File
# Begin Source File

SOURCE=.\train.cpp
# End Source File
# End Group
# Begin Group "render system code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\rendersystem\Particle.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\ParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSAurora.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBeam.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBlastCone.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBubbles.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSDrips.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSFlameCone.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSFlatTrail.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSparks.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSparkShower.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSpawnEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RenderManager.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RenderSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RotatingSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSBeam.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSBeamStar.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSCylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSDelayed.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSLight.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSModel.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSTeleparts.cpp
# End Source File
# End Group
# Begin Group "event code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\ev_common.cpp
# End Source File
# Begin Source File

SOURCE=.\ev_game.cpp
# End Source File
# Begin Source File

SOURCE=.\ev_hldm.cpp
# End Source File
# Begin Source File

SOURCE=.\events.cpp
# End Source File
# Begin Source File

SOURCE=.\fx_game.cpp
# End Source File
# End Group
# Begin Group "game shared code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=..\game_shared\vgui_checkbutton2.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_grid.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_loadtga.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_scrollbar2.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_slider2.cpp
# End Source File
# End Group
# Begin Group "common code"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=..\common\materials.cpp
# End Source File
# Begin Source File

SOURCE=..\common\matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\common\shared_resources.cpp
# End Source File
# Begin Source File

SOURCE=..\common\util_vector.cpp
# End Source File
# Begin Source File

SOURCE=..\common\vector.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ammohistory.cpp
# End Source File
# Begin Source File

SOURCE=.\bsputil.cpp
# End Source File
# Begin Source File

SOURCE=.\cdll_int.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_cmd.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_fx.cpp
# End Source File
# Begin Source File

SOURCE=.\cl_msg.cpp
# End Source File
# Begin Source File

SOURCE=.\com_weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\demo.cpp
# End Source File
# Begin Source File

SOURCE=.\entity.cpp
# End Source File
# Begin Source File

SOURCE=.\in_camera.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\inputw32.cpp
# End Source File
# Begin Source File

SOURCE=..\common\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\musicplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\parsemsg.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\view.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx"
# Begin Group "pm headers"

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
# Begin Group "vgui headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\vgui_include\VGUI.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ActionSignal.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_App.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Bitmap.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_BitmapTGA.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Border.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_BorderLayout.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_BorderPair.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_BuildGroup.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Button.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ButtonController.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ButtonGroup.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ChangeSignal.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_CheckButton.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Color.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ComboKey.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ConfigWizard.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Cursor.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Dar.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_DataInputStream.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Desktop.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_DesktopIcon.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_EditPanel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_EtchedBorder.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_FileInputStream.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_FlowLayout.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_FocusChangeSignal.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_FocusNavGroup.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Font.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Frame.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_FrameSignal.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_GridLayout.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_HeaderPanel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Image.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ImagePanel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_InputSignal.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_InputStream.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_IntChangeSignal.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_IntLabel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_KeyCode.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Label.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Layout.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_LayoutInfo.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_LineBorder.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ListPanel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_LoweredBorder.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Menu.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_MenuItem.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_MenuSeparator.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_MessageBox.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_MiniApp.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_MouseCode.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Panel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_PopupMenu.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ProgressBar.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_RadioButton.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_RaisedBorder.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_RepaintSignal.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Scheme.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ScrollBar.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ScrollPanel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Slider.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_StackLayout.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_String.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_Surface.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_SurfaceBase.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_SurfaceGL.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_TablePanel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_TabPanel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_TaskBar.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_TextEntry.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_TextGrid.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_TextImage.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_TextPanel.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_TickSignal.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_ToggleButton.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_TreeFolder.h
# End Source File
# Begin Source File

SOURCE=..\vgui_include\VGUI_WizardPanel.h
# End Source File
# End Group
# Begin Group "event headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=.\eventscripts.h
# End Source File
# End Group
# Begin Group "studio headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=.\GameStudioModelRenderer.h
# End Source File
# Begin Source File

SOURCE=.\studio_util.h
# End Source File
# Begin Source File

SOURCE=.\StudioModelRenderer.h
# End Source File
# End Group
# Begin Group "game shared headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\game_shared\bitvec.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_checkbutton2.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_defaultinputsignal.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_grid.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_helpers.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_listbox.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_loadtga.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_scrollbar2.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_slider2.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_banmgr.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_common.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_status.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_vgui_tweakdlg.h
# End Source File
# End Group
# Begin Group "hud headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=.\hud.h
# End Source File
# Begin Source File

SOURCE=.\hud_iface.h
# End Source File
# Begin Source File

SOURCE=.\hud_servers.h
# End Source File
# Begin Source File

SOURCE=.\hud_servers_priv.h
# End Source File
# Begin Source File

SOURCE=.\hud_spectator.h
# End Source File
# End Group
# Begin Group "server dll headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\dlls\activity.h
# End Source File
# Begin Source File

SOURCE=..\dlls\animation.h
# End Source File
# Begin Source File

SOURCE=..\dlls\basemonster.h
# End Source File
# Begin Source File

SOURCE=..\dlls\cbase.h
# End Source File
# Begin Source File

SOURCE=..\dlls\ctf_objects.h
# End Source File
# Begin Source File

SOURCE=..\dlls\decals.h
# End Source File
# Begin Source File

SOURCE=..\dlls\effects.h
# End Source File
# Begin Source File

SOURCE=..\dlls\enginecallback.h
# End Source File
# Begin Source File

SOURCE=..\dlls\explode.h
# End Source File
# Begin Source File

SOURCE=..\dlls\extdll.h
# End Source File
# Begin Source File

SOURCE=..\dlls\game.h
# End Source File
# Begin Source File

SOURCE=..\dlls\gamerules\gamerules.h
# End Source File
# Begin Source File

SOURCE=..\dlls\globals.h
# End Source File
# Begin Source File

SOURCE=..\dlls\monsterevent.h
# End Source File
# Begin Source File

SOURCE=..\dlls\monsters.h
# End Source File
# Begin Source File

SOURCE=..\dlls\nodes.h
# End Source File
# Begin Source File

SOURCE=..\dlls\player.h
# End Source File
# Begin Source File

SOURCE=..\dlls\saverestore.h
# End Source File
# Begin Source File

SOURCE=..\dlls\schedule.h
# End Source File
# Begin Source File

SOURCE=..\dlls\skill.h
# End Source File
# Begin Source File

SOURCE=..\dlls\sound.h
# End Source File
# Begin Source File

SOURCE=..\dlls\soundent.h
# End Source File
# Begin Source File

SOURCE=..\dlls\weapons.h
# End Source File
# End Group
# Begin Group "common headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\common\basetsd.h
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

SOURCE=..\common\cl_entity.h
# End Source File
# Begin Source File

SOURCE=..\common\color.h
# End Source File
# Begin Source File

SOURCE=..\common\colors.h
# End Source File
# Begin Source File

SOURCE=..\common\com_model.h
# End Source File
# Begin Source File

SOURCE=..\common\con_nprint.h
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

SOURCE=..\common\fmod.h
# End Source File
# Begin Source File

SOURCE=..\common\fmod_dyn.h
# End Source File
# Begin Source File

SOURCE=..\common\fmod_errors.h
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

SOURCE=..\common\interface.h
# End Source File
# Begin Source File

SOURCE=..\common\ivoicetweak.h
# End Source File
# Begin Source File

SOURCE=..\common\mathlib.h
# End Source File
# Begin Source File

SOURCE=..\common\matrix.h
# End Source File
# Begin Source File

SOURCE=..\common\msg_fx.h
# End Source File
# Begin Source File

SOURCE=..\common\net_api.h
# End Source File
# Begin Source File

SOURCE=..\common\netadr.h
# End Source File
# Begin Source File

SOURCE=..\common\particledef.h
# End Source File
# Begin Source File

SOURCE=..\common\platform.h
# End Source File
# Begin Source File

SOURCE=..\common\pmtrace.h
# End Source File
# Begin Source File

SOURCE=..\common\protocol.h
# End Source File
# Begin Source File

SOURCE=..\common\r_efx.h
# End Source File
# Begin Source File

SOURCE=..\common\r_studioint.h
# End Source File
# Begin Source File

SOURCE=..\common\randomrange.h
# End Source File
# Begin Source File

SOURCE=..\common\ref_params.h
# End Source File
# Begin Source File

SOURCE=..\common\screenfade.h
# End Source File
# Begin Source File

SOURCE=..\common\shared_resources.h
# End Source File
# Begin Source File

SOURCE=..\common\studio_event.h
# End Source File
# Begin Source File

SOURCE=..\common\triangleapi.h
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
# Begin Source File

SOURCE=..\common\weaponslots.h
# End Source File
# End Group
# Begin Group "engine headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=..\engine\anorms.h
# End Source File
# Begin Source File

SOURCE=..\engine\archtypes.h
# End Source File
# Begin Source File

SOURCE=..\engine\cdll_int.h
# End Source File
# Begin Source File

SOURCE=..\engine\custom.h
# End Source File
# Begin Source File

SOURCE=..\engine\customentity.h
# End Source File
# Begin Source File

SOURCE=..\engine\edict.h
# End Source File
# Begin Source File

SOURCE=..\engine\eiface.h
# End Source File
# Begin Source File

SOURCE=..\engine\keydefs.h
# End Source File
# Begin Source File

SOURCE=..\engine\progdefs.h
# End Source File
# Begin Source File

SOURCE=..\engine\progs.h
# End Source File
# Begin Source File

SOURCE=..\engine\Sequence.h
# End Source File
# Begin Source File

SOURCE=..\engine\shake.h
# End Source File
# Begin Source File

SOURCE=..\engine\studio.h
# End Source File
# End Group
# Begin Group "render system headers"

# PROP Default_Filter "h;hpp;hxx"
# Begin Source File

SOURCE=.\rendersystem\Particle.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\ParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PartSystem.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSAurora.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBeam.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBlastCone.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSBubbles.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSDrips.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSFlameCone.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSFlatTrail.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSparks.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSparkShower.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSSpawnEffect.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\PSStreaks.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RenderManager.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RenderSystem.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RotatingSystem.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSBeam.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSBeamStar.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSCylinder.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSDelayed.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSLight.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSModel.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSSprite.h
# End Source File
# Begin Source File

SOURCE=.\rendersystem\RSTeleparts.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ammo.h
# End Source File
# Begin Source File

SOURCE=.\ammohistory.h
# End Source File
# Begin Source File

SOURCE=.\bsputil.h
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\cl_dll.h
# End Source File
# Begin Source File

SOURCE=.\cl_enginefuncs.h
# End Source File
# Begin Source File

SOURCE=.\cl_fx.h
# End Source File
# Begin Source File

SOURCE=.\cl_util.h
# End Source File
# Begin Source File

SOURCE=.\com_weapons.h
# End Source File
# Begin Source File

SOURCE=.\demo.h
# End Source File
# Begin Source File

SOURCE=.\game_fx.h
# End Source File
# Begin Source File

SOURCE=.\gl_dynamic.h
# End Source File
# Begin Source File

SOURCE=.\health.h
# End Source File
# Begin Source File

SOURCE=.\in_defs.h
# End Source File
# Begin Source File

SOURCE=.\kbutton.h
# End Source File
# Begin Source File

SOURCE=.\musicplayer.h
# End Source File
# Begin Source File

SOURCE=.\overview.h
# End Source File
# Begin Source File

SOURCE=.\parsemsg.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ConsolePanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ControlConfigPanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_CustomObjects.h
# End Source File
# Begin Source File

SOURCE=.\vgui_EntityEntryPanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_int.h
# End Source File
# Begin Source File

SOURCE=.\vgui_MOTDWindow.h
# End Source File
# Begin Source File

SOURCE=.\vgui_MusicPlayer.h
# End Source File
# Begin Source File

SOURCE=.\vgui_PSEditorPanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_SchemeManager.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ScorePanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ServerBrowser.h
# End Source File
# Begin Source File

SOURCE=.\vgui_SpectatorPanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_TeamMenu.h
# End Source File
# Begin Source File

SOURCE=.\vgui_XDMViewport.h
# End Source File
# Begin Source File

SOURCE=.\view.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;rc;rc2"
# Begin Source File

SOURCE=.\cl_dll.rc
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
# End Group
# End Target
# End Project
