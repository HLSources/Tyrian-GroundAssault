#ifndef CL_DLL_H
#define CL_DLL_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

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
//  cl_dll.h
//

// 4-23-98  JOHN

//
//  This DLL is linked by the client when they first initialize.
// This DLL is responsible for the following tasks:
//		- Loading the HUD graphics upon initialization
//		- Drawing the HUD graphics every frame
//		- Handling the custum HUD-update packets
//

#include "../common/platform.h"
#include "../common/vector.h"
#include "../common/cdll_dll.h"

typedef int (*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);

//#include "wrect.h"
#include "../engine/cdll_int.h"
#include "../engine/eiface.h"


// XDM
void CL_Precache(void);
void CL_RegisterVariables(void);
void CL_RegisterCommands(void);
void CL_RegisterMessages(void);
void CL_TempEntPlaySound(struct tempent_s *pTemp, float damp);

void EV_HookEvents(void);


extern cl_enginefunc_t gEngfuncs;

extern cvar_t *g_pCvarDeveloper;
extern cvar_t *g_pCvarServerZMax;
extern cvar_t *g_pCvarTimeLeft;
extern cvar_t *g_pCvarScheme;
extern cvar_t *g_pCvarEnableLods;
extern cvar_t *g_pCvarTrailDLight;
extern cvar_t *g_pCvarTrailRS;
extern cvar_t *g_pCvarTFX;
extern cvar_t *g_pCvarDeathView;
extern cvar_t *g_pCvarAnnouncer;
extern cvar_t *g_pCvarViewDistance;
extern cvar_t *g_pCvarFlashLightMode;
extern cvar_t *g_pCvarDefaultFOV;
extern cvar_t *g_pCvarZSR;
extern cvar_t *g_pCvarCameraAngles;
extern cvar_t *g_pCvarTmp;

#endif // CL_DLL_H
