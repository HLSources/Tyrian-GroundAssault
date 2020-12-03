#ifndef CL_ENGINEFUNCS_H
#define CL_ENGINEFUNCS_H
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */

// Already done in "cl_dll.h"
// extern cl_enginefunc_t gEngfuncs;

// The actual engine callbacks

#define SPR_Load			(*gEngfuncs.pfnSPR_Load)
#define SPR_Frames			(*gEngfuncs.pfnSPR_Frames)
#define SPR_Height			(*gEngfuncs.pfnSPR_Height)
#define SPR_Width			(*gEngfuncs.pfnSPR_Width)
#define SPR_Set				(*gEngfuncs.pfnSPR_Set)
// SPR_Draw  draws a the current sprite as solid
#define SPR_Draw			(*gEngfuncs.pfnSPR_Draw)
// SPR_DrawHoles  draws the current sprites,  with color index255 not drawn (transparent)
#define SPR_DrawHoles		(*gEngfuncs.pfnSPR_DrawHoles)
// SPR_DrawAdditive  adds the sprites RGB values to the background  (additive transulency)
#define SPR_DrawAdditive	(*gEngfuncs.pfnSPR_DrawAdditive)
// SPR_EnableScissor  sets a clipping rect for HUD sprites.  (0,0) is the top-left hand corner of the screen.
#define SPR_EnableScissor	(*gEngfuncs.pfnSPR_EnableScissor)
// SPR_DisableScissor  disables the clipping rect
#define SPR_DisableScissor	(*gEngfuncs.pfnSPR_DisableScissor)
#define SPR_GetList			(*gEngfuncs.pfnSPR_GetList)
// Fill a rectangle
#define FillRGBA			(*gEngfuncs.pfnFillRGBA)

#define GetScreenInfo		(*gEngfuncs.pfnGetScreenInfo)
#define SetCrosshair		(*gEngfuncs.pfnSetCrosshair)
#define CVAR_CREATE			(*gEngfuncs.pfnRegisterVariable)
#define CVAR_GET_FLOAT		(*gEngfuncs.pfnGetCvarFloat)
#define CVAR_GET_STRING		(*gEngfuncs.pfnGetCvarString)

#define ADD_DOMMAND			(*gEngfuncs.pfnAddCommand)
#define HOOK_USER_MESSAGE	(*gEngfuncs.pfnHookUserMsg)
#define SERVER_COMMAND		(*gEngfuncs.pfnServerCmd)
#define CLIENT_COMMAND		(*gEngfuncs.pfnClientCmd)

#define GetPlayerInfo		(*gEngfuncs.pfnGetPlayerInfo)

//#define PlaySound			(*gEngfuncs.pfnPlaySoundByName)
//#define PlaySoundIndex		(*gEngfuncs.pfnPlaySoundByIndex)

#define ANGLE_VECTORS		(*gEngfuncs.pfnAngleVectors)
#define TextMessageGet		(*gEngfuncs.pfnTextMessageGet)
#define TextMessageDrawChar	(*gEngfuncs.pfnDrawCharacter)
#define DrawConsoleString	(*gEngfuncs.pfnDrawConsoleString)
#define SET_TEXT_COLOR		(*gEngfuncs.pfnDrawSetTextColor)
#define DrawConsoleStringLen (*gEngfuncs.pfnDrawConsoleStringLen)
#define ConsolePrint		(*gEngfuncs.pfnConsolePrint)// If string[0] == 1, message goes into the notify buffer
#define CenterPrint			(*gEngfuncs.pfnCenterPrint)
/* conflict
#define GetWindowCenterX	(*gEngfuncs.GetWindowCenterX)
#define GetWindowCenterY	(*gEngfuncs.GetWindowCenterY)
*/
#define GET_VIEW_ANGLES		(*gEngfuncs.GetViewAngles)
#define SET_VIEW_ANGLES		(*gEngfuncs.SetViewAngles)
#define GET_MAX_CLIENTS		(*gEngfuncs.GetMaxClients)
#define CVAR_SET_FLOAT		(*gEngfuncs.Cvar_SetValue)
#define CMD_ARGC			(*gEngfuncs.Cmd_Argc)
#define CMD_ARGV			(*gEngfuncs.Cmd_Argv)
#define CON_PRINTF			(*gEngfuncs.Con_Printf)
#define CON_DPRINTF			(*gEngfuncs.Con_DPrintf)
#define CON_NPRINTF			(*gEngfuncs.Con_NPrintf)
#define CON_NXPRINTF		(*gEngfuncs.Con_NXPrintf)
/*
#define GFUNCMACRO			(*gEngfuncs.PhysInfo_ValueForKey)
#define GFUNCMACRO			(*gEngfuncs.ServerInfo_ValueForKey)
#define GFUNCMACRO			(*gEngfuncs.GetClientMaxspeed)
#define GFUNCMACRO			(*gEngfuncs.CheckParm)
#define GFUNCMACRO			(*gEngfuncs.Key_Event)
#define GFUNCMACRO			(*gEngfuncs.GetMousePosition)
#define GFUNCMACRO			(*gEngfuncs.IsNoClipping)
#define GFUNCMACRO			(*gEngfuncs.GetLocalPlayer)
#define GFUNCMACRO			(*gEngfuncs.GetViewModel)
#define GFUNCMACRO			(*gEngfuncs.GetEntityByIndex)
#define GFUNCMACRO			(*gEngfuncs.GetClientTime)
#define GFUNCMACRO			(*gEngfuncs.V_CalcShake)
#define GFUNCMACRO			(*gEngfuncs.V_ApplyShake)
#define GFUNCMACRO			(*gEngfuncs.PM_PointContents)
#define GFUNCMACRO			(*gEngfuncs.PM_WaterEntity)
#define GFUNCMACRO			(*gEngfuncs.PM_TraceLine)
#define LOAD_MODEL			(*gEngfuncs.CL_LoadModel)
#define CREATE_ENTITY		(*gEngfuncs.CL_CreateVisibleEntity)
#define GFUNCMACRO			(*gEngfuncs.GetSpritePointer)
#define GFUNCMACRO			(*gEngfuncs.pfnPlaySoundByNameAtLocation)
#define GFUNCMACRO			(*gEngfuncs.pfnPrecacheEvent)
#define GFUNCMACRO			(*gEngfuncs.pfnPlaybackEvent)
#define GFUNCMACRO			(*gEngfuncs.pfnWeaponAnim)
*/
#define RANDOM_FLOAT		(*gEngfuncs.pfnRandomFloat)
#define RANDOM_LONG			(*gEngfuncs.pfnRandomLong)
#define HOOK_EVENT			(*gEngfuncs.pfnHookEvent)

#define CON_IS_VISIBLE		(*gEngfuncs.Con_IsVisible)
#define GET_GAME_DIR		(*gEngfuncs.pfnGetGameDirectory)
#define CVAR_GET_POINTER	(*gEngfuncs.pfnGetCvarPointer)

#define KEY_LOOKUP_BINDING	(*gEngfuncs.Key_LookupBinding)
#define GET_LEVEL_NAME		(*gEngfuncs.pfnGetLevelName)
#define GET_SCREEN_FADE		(*gEngfuncs.pfnGetScreenFade)
#define SET_SCREEN_FADE		(*gEngfuncs.pfnSetScreenFade)
#define VGUI_GETPANEL		(*gEngfuncs.VGui_GetPanel)
#define VGUI_VPORT_PAINTBG	(*gEngfuncs.VGui_ViewportPaintBackground)

#define COM_LOAD_FILE		(*gEngfuncs.COM_LoadFile)
#define COM_PARSE_FILE		(*gEngfuncs.COM_ParseFile)
#define COM_FREE_FILE		(*gEngfuncs.COM_FreeFile)
#define IS_SPECTATE_ONLY	(*gEngfuncs.IsSpectateOnly)
#define LOAD_MAP_SPRITE		(*gEngfuncs.LoadMapSprite)
#define GFUNCMACRO			(*gEngfuncs.COM_AddAppDirectoryToSearchPath)
#define COM_EXPAND_FILENAME	(*gEngfuncs.COM_ExpandFilename)
/*
#define GFUNCMACRO			(*gEngfuncs.PlayerInfo_ValueForKey)
#define GFUNCMACRO			(*gEngfuncs.PlayerInfo_SetValueForKey)
#define GFUNCMACRO			(*gEngfuncs.GetPlayerUniqueID)
#define GFUNCMACRO			(*gEngfuncs.GetTrackerIDForPlayer)
#define GFUNCMACRO			(*gEngfuncs.GetPlayerForTrackerID)
#define SERVER_COMMAND_U	(*gEngfuncs.pfnServerCmdUnreliable)
#define GFUNCMACRO			(*gEngfuncs.pfnGetMousePos)
#define GFUNCMACRO			(*gEngfuncs.pfnSetMousePos)
#define GFUNCMACRO			(*gEngfuncs.pfnSetMouseEnable)


#ifdef CLDLL_NEWFUNCTIONS

#define GFUNCMACRO			(*pfnGetCvarList)
#define GFUNCMACRO			(*pfnGetCmdList)
#define GFUNCMACRO			(*pfnCvarNameFromPointer)
#define GFUNCMACRO			(*pfnCmdNameFromPointer)
#define GFUNCMACRO			(*pfnGetCurrentTime)
#define GFUNCMACRO			(*pfnGetGravity)
#define GFUNCMACRO			(*pfnGetModelByIndex)
#define GFUNCMACRO			(*pfnSetGL_TexSort)
#define GFUNCMACRO			(*pfnSetGL_TexSort_Color)
#define GFUNCMACRO			(*pfnSetGL_TexSort_Scale)
#define GFUNCMACRO			(*pfnSequenceGet)
#define GFUNCMACRO			(*pfnDrawSpriteGeneric)
#define GFUNCMACRO			(*pfnSequencePickSentence)
#define GFUNCMACRO			(*pfnUnknownFunction6)
#define GFUNCMACRO			(*pfnUnknownFunction7)
#define GFUNCMACRO			(*pfnUnknownFunction8)
#define GFUNCMACRO			(*pfnUnknownFunction9)
#define GFUNCMACRO			(*pfnUnknownFunction10)
#define GFUNCMACRO			(*pfnGetApproxWavePlayLen)
#define GFUNCMACRO			(*pfnUnknownFunction11)
#define CVAR_SET			(*Cvar_Set)
#define GFUNCMACRO			(*pfnIsCareerMatch)
#define GFUNCMACRO			(*pfnStartDynamicSound)
#define GFUNCMACRO			(*pfnMP3_InitStream)
#define GFUNCMACRO			(*pfnUnknownFunction12)
#define GFUNCMACRO			(*pfnProcessTutorMessageDecayBuffer)
#define GFUNCMACRO			(*pfnConstructTutorMessageDecayBuffer)
#define GFUNCMACRO			(*pfnResetTutorMessageDecayData)
#define GFUNCMACRO			(*pfnStartDynamicSound2)
#define GFUNCMACRO			(*pfnFillRGBA2)

#endif // CLDLL_NEWFUNCTIONS
*/
#endif // CL_ENGINEFUNCS_H
