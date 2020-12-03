/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
//  cdll_int.c
//
// this implementation handles the linking of the engine to the DLL
//

#include "hud.h"
#include "cl_util.h"
#include "netadr.h"
#include "vgui_schememanager.h"
#include "hud_servers.h"
#include "vgui_int.h"
//#include "interface.h"
#include "in_defs.h"
#include "musicplayer.h"
#include "RenderManager.h"
#include "pm_defs.h"
#include "pm_shared.h"
#include "r_studioint.h"
#include "bsputil.h"
#include "studio_util.h"
#include "vgui_XDMViewport.h"
#include "vgui_scorepanel.h"
//#include "studio.h"
//#include "StudioModelRenderer.h"
//#include "GameStudioModelRenderer.h"
#include "..\game_shared\voice_status.h"

#ifdef _WIN32
//#define RC_INVOKED
#undef MAX_PATH
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "gl_dynamic.h"

void initMsgManager();

cl_enginefunc_t gEngfuncs;
CHud gHUD;

cvar_t *g_pCvarDeveloper = NULL;
cvar_t *g_pCvarServerZMax = NULL;
cvar_t *g_pCvarTimeLeft = NULL;
cvar_t *g_pCvarScheme = NULL;
cvar_t *g_pCvarEnableLods = NULL;
cvar_t *g_pCvarTFX = NULL;
cvar_t *g_pCvarTrailDLight = NULL;
cvar_t *g_pCvarTrailRS = NULL;
cvar_t *g_pCvarDeathView = NULL;
cvar_t *g_pCvarAnnouncer = NULL;
cvar_t *g_pCvarViewDistance = NULL;
cvar_t *g_pCvarFlashLightMode = NULL;
cvar_t *g_pCvarDefaultFOV = NULL;
cvar_t *g_pCvarZSR = NULL;
cvar_t *g_pCvarCameraAngles = NULL;
cvar_t *g_pCvarTmp = NULL;


int g_iWaterLevel = 0;// XDM: FOG
HMODULE hOpenGLDLL = NULL;

//#ifdef CL_GL_FOG
GLAPI_glEnable GL_glEnable = NULL;
GLAPI_glDisable GL_glDisable;
GLAPI_glFogi GL_glFogi;
GLAPI_glFogf GL_glFogf;
GLAPI_glFogfv GL_glFogfv;
GLAPI_glHint GL_glHint;
//#endif

//extern "C"
//{
unsigned short g_usPM_Fall;// XDM3035a
//}

class CHLVoiceStatusHelper : public IVoiceStatusHelper
{
public:
	virtual void GetPlayerTextColor(int entindex, int color[3])
	{
		byte r,g,b;
		if (GetPlayerColor(entindex, r,g,b))
		{
			color[0] = r; color[1] = g; color[2] = b;
		}
		else
		{
			color[0] = color[1] = color[2] = 255;
		}
	}

	virtual void UpdateCursorState()
	{
		gViewPort->UpdateCursorState();
	}

	virtual int	GetAckIconHeight()
	{
		return ScreenHeight - gHUD.m_iFontHeight*3 - 6;
	}

	virtual bool CanShowSpeakerLabels()
	{
		if (gViewPort && gViewPort->GetScoreBoard())
			return !gViewPort->GetScoreBoard()->isVisible();
		else
			return false;
	}
};

CHLVoiceStatusHelper g_VoiceStatusHelper;


void CL_RegisterVariables(void)
{
	// search by name is faster when there are less items so search now, then register new cvars
	g_pCvarDeveloper		= CVAR_GET_POINTER("developer");
	g_pCvarServerZMax		= CVAR_GET_POINTER("sv_zmax");
	g_pCvarEnableLods		= CVAR_CREATE("cl_enable_lods",			"1",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL);

	g_pCvarTrailDLight		= CVAR_CREATE("cl_trail_dlight",		"1",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	g_pCvarTrailRS			= CVAR_CREATE("cl_trail_rs",			"1",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL);

	g_pCvarTimeLeft			= NULL;// XDM: it does not extst yet
	g_pCvarScheme			= CVAR_CREATE("cl_scheme",			"",			FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	g_pCvarTFX				= CVAR_CREATE("cl_tournamentfx",	"1",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	g_pCvarDeathView		= CVAR_CREATE("cl_death_view",		"0",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	g_pCvarAnnouncer		= CVAR_CREATE("cl_announcer_voice",	"1",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	g_pCvarViewDistance		= CVAR_CREATE("cl_viewdist"			,"2048",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL);// XDM3035c
	g_pCvarFlashLightMode	= CVAR_CREATE("cl_flashlightmode"	,"1",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL);// XDM3037
	g_pCvarDefaultFOV		= CVAR_CREATE("default_fov",		"90",		FCVAR_CLIENTDLL);
	g_pCvarZSR				= CVAR_CREATE("zoom_sensitivity_ratio",	"1.0",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
//	g_pCvarCameraAngles		= CVAR_CREATE("cam_angles",			"0 0 0",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	g_pCvarTmp				= CVAR_CREATE("tmpcvar",			"1",		FCVAR_UNLOGGED | FCVAR_CLIENTDLL);
}


/*
========================== 
    Initialize

Called when the DLL is first loaded.
==========================
*/
extern "C" 
{
int		DLLEXPORT Initialize(cl_enginefunc_t *pEnginefuncs, int iVersion);
int		DLLEXPORT HUD_VidInit(void);
void	DLLEXPORT HUD_Init(void);
int		DLLEXPORT HUD_Redraw(float flTime, int intermission);
int		DLLEXPORT HUD_UpdateClientData(client_data_t *cdata, float flTime);
void	DLLEXPORT HUD_Reset(void);
void	DLLEXPORT HUD_PlayerMove(struct playermove_s *ppmove, int server);
void	DLLEXPORT HUD_PlayerMoveInit(struct playermove_s *ppmove);
char	DLLEXPORT HUD_PlayerMoveTexture(char *name);
int		DLLEXPORT HUD_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
int		DLLEXPORT HUD_GetHullBounds(int hullnumber, float *mins, float *maxs);
void	DLLEXPORT HUD_Frame(double time);
void	DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking);
void	DLLEXPORT HUD_DirectorMessage(int iSize, void *pbuf);
void	DLLEXPORT HUD_Shutdown(void);// XDM: moved here from input.cpp
}

/*
================================
HUD_GetHullBounds

  Engine calls this to enumerate player collision hulls, for prediction.  Return 0 if the hullnumber doesn't exist.
================================
*/
int DLLEXPORT HUD_GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	int iret = 0;

	switch (hullnumber)
	{
	case 0:				// Normal player
		mins = Vector(-16, -16, -36);
		maxs = Vector(16, 16, 36);
		iret = 1;
		break;
	case 1:				// Crouched player
		mins = Vector(-16, -16, -18);
		maxs = Vector(16, 16, 18);
		iret = 1;
		break;
	case 2:				// Point based hull
		mins = Vector(0, 0, 0);
		maxs = Vector(0, 0, 0);
		iret = 1;
		break;
	}

	return iret;
}


/*
================================
HUD_ConnectionlessPacket

 Return 1 if the packet is valid.  Set response_buffer_size if you want to send a response packet.  Incoming, it holds the max
  size of the response_buffer, so you must zero it out if you choose not to respond.
================================
*/
int	DLLEXPORT HUD_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
	// Parse stuff from args
//	int max_buffer_size = *response_buffer_size;

	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;

	CON_DPRINTF("Connectionless packet from %d.%d.%d.%d: \"%s\"\n", net_from->ip[0], net_from->ip[1], net_from->ip[2], net_from->ip[3], args);

	// Since we don't listen for anything here, just respond that it's a bogus message
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

// XDM: NOTE: called once by the engine (at game startup)
void DLLEXPORT HUD_PlayerMoveInit(struct playermove_s *ppmove)
{
	PM_Init(ppmove);
}

char DLLEXPORT HUD_PlayerMoveTexture(char *name)
{
	return PM_FindTextureType(name);
}

void DLLEXPORT HUD_PlayerMove(struct playermove_s *ppmove, int server)
{
	PM_Move(ppmove, server);
	g_iWaterLevel = ppmove->waterlevel;// XDM
}

int DLLEXPORT Initialize(cl_enginefunc_t *pEnginefuncs, int iVersion)
{
	if (iVersion != CLDLL_INTERFACE_VERSION)
		return 0;

	gEngfuncs = *pEnginefuncs;
// XDM: TODO: find a reliable way to get game/interface/protocol version
//	(*pEnginefuncs->pfnGetCvarString)("sv_version");
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

//	if (CVAR_GET_POINTER("host_clientloaded") != NULL)
//		g_fXashEngine = TRUE;

//	CON_PRINTF("Client DLL build %s, interface version %d\n", __DATE__, CLDLL_INTERFACE_VERSION);
	EV_HookEvents();
	return 1;
}


/*
==========================
	HUD_VidInit

Called when the game initializes
and whenever the vid_mode is changed
so the HUD can reinitialize itself.
==========================
*/

int DLLEXPORT HUD_VidInit(void)
{
	if (g_pRenderManager != NULL)// XDM
		g_pRenderManager->DeleteAllSystems();

	gHUD.VidInit();

	VGui_Startup();

	try
	{
		GetClientVoiceMgr()->VidInit();// XDM3035c: this will (probably) work better after VGui_Startup
	}
	catch (...)
	{
		CON_PRINTF("GetClientVoiceMgr()->VidInit() exception!\n");
	}

//#ifdef CL_GL_FOG
	gHUD.m_iHardwareMode = IEngineStudio.IsHardware();
	if (gHUD.m_iHardwareMode == 1)// XDM3035: OpenGL
	{
		if (hOpenGLDLL == NULL)
		{
			hOpenGLDLL = GetModuleHandle("opengl32.dll");// XDM3035c: forcibly reuse loaded module, if present (not OpenGL otherwise).
			if (hOpenGLDLL)
			{
				CON_PRINTF("CL: OpenGL library loaded.\n");
				GL_glEnable = (GLAPI_glEnable)GetProcAddress(hOpenGLDLL, "glEnable");
				GL_glDisable = (GLAPI_glDisable)GetProcAddress(hOpenGLDLL, "glDisable");
				GL_glFogi = (GLAPI_glFogi)GetProcAddress(hOpenGLDLL, "glFogi");
				GL_glFogf = (GLAPI_glFogf)GetProcAddress(hOpenGLDLL, "glFogf");
				GL_glFogfv = (GLAPI_glFogfv)GetProcAddress(hOpenGLDLL, "glFogfv");
				GL_glHint = (GLAPI_glHint)GetProcAddress(hOpenGLDLL, "glHint");

				if (GL_glEnable)
					CON_PRINTF("CL: OpenGL mode initialized.\n");
			}
			else
				CON_PRINTF("CL: OpenGL mode failed to initialize!\n");
		}
	}
//#endif

	return 1;
}


/*
==========================
	HUD_Init

Called whenever the client connects
to a server.  Reinitializes all 
the hud variables.
==========================
*/

void DLLEXPORT HUD_Init(void)
{
	CON_DPRINTF("HUD_Init()\n");
	InitInput();

	CL_RegisterVariables();// XDM
	CL_RegisterCommands();// XDM
	CL_RegisterMessages();// XDM
	CL_Precache();

	g_usPM_Fall = gEngfuncs.pfnPrecacheEvent(1, "events/pm/fall.sc");

	gHUD.Init();
initMsgManager();
	Scheme_Init();

	GetClientVoiceMgr()->Init(&g_VoiceStatusHelper, (vgui::Panel**)&gViewPort);// XDM3035c

	BGM_Init();// XDM

	if (g_pRenderManager == NULL)
		g_pRenderManager = new CRenderManager();// XDM
}


/*
==========================
	HUD_Redraw

called every screen frame to
redraw the HUD.
===========================
*/

int DLLEXPORT HUD_Redraw(float time, int intermission)
{
	try
	{
		VIS_Frame(time);// XDM3035c: HUD_Frame() could be a proper place for this, but engine keeps calling it after server is stopped (critical/precache/network error) which causes crashes (g_pWorld cannot be set to NULL beforehand).
	}
	catch(...)
	{
		CON_PRINTF("CL: VIS_Frame exception!\n");
		g_pWorld = NULL;// XDM3035c: TESTME!!!!
	}

	gHUD.Redraw(time, intermission);

//	CON_PRINTF("HUD_Redraw(%f, %d)\n", time, intermission);
	return 1;
}


/*
==========================
	HUD_UpdateClientData

called every time shared client
dll/engine data gets changed,
and gives the cdll a chance
to modify the data.

returns 1 if anything has been changed, 0 otherwise.
==========================
*/
int DLLEXPORT HUD_UpdateClientData(client_data_t *pcldata, float flTime)
{
	IN_Commands();

	return gHUD.UpdateClientData(pcldata, flTime);
}

/*
==========================
	HUD_Reset

Called at start and end of demos to restore to "non"HUD state.
==========================
*/
void DLLEXPORT HUD_Reset(void)
{
//	if (g_pRenderManager)// XDM
//		g_pRenderManager->KillAllSystems();

	gHUD.VidInit();
}


float g_fEntityHighlightEffect = 0;
/*
==========================
HUD_Frame

Called by engine every frame that client .dll is loaded
==========================
*/
void DLLEXPORT HUD_Frame(double time)
{
/*	try
	{
		VIS_Frame(time);
	}
	catch(...)
	{
		CON_PRINTF("CL: VIS_Frame exception!\n");
	}*/
//	if (g_iModelIndexSmkball <= 0)// XDM
//		CL_PrecacheModels();

	// XDM3035: this works well even in steam versions
	if (g_pRenderManager && gHUD.m_iActive/* && gHUD.m_iIntermission == 0*/)// XDM: call this AFTER gHUD.Redraw()! // XDM3035c: finally m_iActive!
		g_pRenderManager->Update(gEngfuncs.GetClientTime(), time);

	if (gViewPort && gHUD.m_iActive)
		gViewPort->GetAllPlayersInfo();

	ServersThink(time);

	if (gHUD.m_iActive)
	{
		try
		{
			GetClientVoiceMgr()->Frame(time);// XDM3035c
		}
		catch (...)
		{
			CON_PRINTF("GetClientVoiceMgr()->Frame() exception!\n");
		}
/*
		if (gHUD.m_iPaused == 0)
		{
			if (g_fEntityHighlightEffect > 1)
				g_fEntityHighlightEffect -= time;//*CVAR_GET_FLOAT("test1");
			else
				g_fEntityHighlightEffect = 2;//CVAR_GET_FLOAT("test2");

//			CON_PRINTF("EHE: %g\n", g_fEntityHighlightEffect);
		}*/
	}

	BGM_StartFrame();
}

/*
==========================
HUD_VoiceStatus

Called when a player starts or stops talking.
==========================
*/

void DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	GetClientVoiceMgr()->UpdateSpeakerStatus(entindex, bTalking);
}

/*
==========================
HUD_DirectorEvent

Called when a director event message was received
==========================
*/

void DLLEXPORT HUD_DirectorMessage(int iSize, void *pbuf)
{
	 gHUD.m_Spectator.DirectorMessage(iSize, pbuf);
}

/*
==========================
HUD_Shutdown

Called whenever the client disconnects from a server.
==========================
*/

void DLLEXPORT HUD_Shutdown(void)
{
	g_pWorld = NULL;

	if (g_pRenderManager)
	{
		g_pRenderManager->DeleteAllSystems();
		delete g_pRenderManager;
		g_pRenderManager = NULL;
	}

	hOpenGLDLL = NULL;// XDM3035c: NEVER free this!

	ShutdownInput();

	BGM_Shutdown();// XDM
}

// just a guess
/*void DLLEXPORT HUD_ChatInputPosition(int *x, int *y)
{
	*x = 100;
	*y = 100;
}*/

int	DLLEXPORT HUD_GetPlayerTeam(int playerIndex)
{
	if (IsValidPlayerIndex(playerIndex))
		return g_PlayerExtraInfo[playerIndex].teamnumber;

	return TEAM_NONE;
}

/*void DLLEXPORT HUD_ClipMoveToEntity(physent_t *pe, const vec3_t start, vec3_t mins, vec3_t maxs, const vec3_t end, pmtrace_t *tr)
{
}*/
