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
//  hud_msg.cpp
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "r_efx.h"
#include "event_api.h"
#include "vgui_XDMViewport.h"
#include "vgui_ScorePanel.h"
#include "cl_fx.h"

cl_entity_t *g_pWorld = NULL;

int CHud::MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf)
{
	ASSERT( iSize == 0 );

#ifdef _DEBUG
	CON_DPRINTF("MsgFunc_ResetHUD()\n");
#endif
	g_pWorld = gEngfuncs.GetEntityByIndex(0);

	// clear all hud data
	HUDLIST *pList = m_pHudList;
	while (pList)
	{
		if (pList->p)
			pList->p->Reset();
		pList = pList->pNext;
	}

	byte r = 0;
	byte g = 0;
	byte b = 0;
	if (StringToRGB(m_pCvarColorMain->string, r,g,b))
		m_iDrawColorMain = RGB2INT(r,g,b);
	else
		m_iDrawColorMain = 0x7F7F7F7F;//RGB2INT(127,127,127);// 0x7F7F7F7F

	if (StringToRGB(m_pCvarColorRed->string, r,g,b))
		m_iDrawColorRed = RGB2INT(r,g,b);
	else
		m_iDrawColorRed = 0x7F7F7F7F;

	if (StringToRGB(m_pCvarColorBlue->string, r,g,b))
		m_iDrawColorBlue = RGB2INT(r,g,b);
	else
		m_iDrawColorBlue = 0x7F7F7F7F;

	if (StringToRGB(m_pCvarColorCyan->string, r,g,b))
		m_iDrawColorCyan = RGB2INT(r,g,b);
	else
		m_iDrawColorCyan = 0x7F7F7F7F;

	if (StringToRGB(m_pCvarColorYellow->string, r,g,b))
		m_iDrawColorYellow = RGB2INT(r,g,b);
	else
		m_iDrawColorYellow = 0x7F7F7F7F;

// NONONO!	m_iTimeLeftLast = 0;
//NO!			m_iScoreLeftLast = 0;
//NO!	m_flNextAnnounceTime = 0.0f;
//NO!!	m_flMouseSensitivity = 0.0f;
//	m_iConcussionEffect = 0;
//	m_iFogMode = 0;
//	ResetFog();// XDM: don't clear the fog here.
//	if (m_iGameType == GT_SINGLE)// XDM3035b: single in trouble
//		CL_Precache();

	return 1;
}

int CHud::MsgFunc_ViewMode(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_iCameraMode = READ_BYTE();
	END_READ();
	return 1;
}

int CHud::MsgFunc_InitHUD(const char *pszName, int iSize, void *pbuf)
{
	m_iSkyMode = 0;
	// prepare all hud data
	HUDLIST *pList = m_pHudList;

	while (pList)
	{
		if (pList->p)
			pList->p->InitHUDData();
		pList = pList->pNext;
	}

	m_Ammo.UpdateCrosshair(0,0);// XDM
	m_iFogMode = 0;
	ResetFog();// XDM: clear out the fog!

	m_flNextAnnounceTime = 0.0f;// XDM3035
	//Probably not a good place to put this.
// XDM	pBeam = pBeam2 = NULL;
	return 1;
}

extern float g_lastFOV;

int CHud::MsgFunc_SetFOV(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int newfov = READ_BYTE();
	END_READ();

	g_lastFOV = (float)newfov;

	if (newfov == 0)
		m_iFOV = gHUD.GetUpdatedDefaultFOV();// XDM: a problematic int-float conversion
	else
		m_iFOV = newfov;

	// the clients fov is actually set in the client data update section of the hud
	m_flMouseSensitivity = GetSensitivityByFOV(m_iFOV);// XDM
	return 1;
}

int CHud::MsgFunc_Logo(const char *pszName,  int iSize, void *pbuf)
{
	m_iLogo = READ_SHORT();
	return 1;
}

int CHud::MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_iGameType			= READ_BYTE();
	m_iGameMode			= READ_BYTE();
	m_iGameSkillLevel	= READ_BYTE();
	m_iGameFlags		= READ_BYTE();// "gamedefs.h"
	m_iRevengeMode		= READ_BYTE();
	m_iFragLimit		= READ_SHORT();// cvar
	m_iScoreLimit		= READ_SHORT();// unique per-rule vaule
	m_iRoundsLimit		= READ_BYTE();
	m_iRoundsPlayed		= READ_BYTE();
	m_iPlayerMaxHealth	= READ_BYTE();
	END_READ();

	g_pCvarTimeLeft = CVAR_GET_POINTER("mp_timeleft");// XDM: now it's ready
//	g_pCvarScoreLeft = CVAR_GET_POINTER("mp_fragsleft");

	if (gViewPort)
	{
		gViewPort->UpdateOnPlayerInfo();
//		gViewPort->m_pScoreBoard->UpdateTitle();// XDM3035a: check this: movet into UpdateOnPlayerInfo()
	//	gViewPort->m_iAllowSpectators = (m_iGameFlags & GAME_FLAG_ALLOW_SPECTATORS)?:1:0;
	}
	return 1;
}

// optimize as much as possible
int CHud::MsgFunc_GRInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int bestplayer = READ_BYTE();
	int bestteam = READ_BYTE();
	m_iScoreLeft = READ_SHORT();
	m_flTimeLeft = (float)READ_SHORT();
	END_READ();

//#ifdef _DEBUG
//	CON_DPRINTF("cl: MsgFunc_GRInfo(%d %d %d %g)\n", bestplayer, bestteam, m_iScoreLeft, m_flTimeLeft);
//#endif
	CheckRemainingScoreAnnouncements();
	return 1;
}

int CHud::MsgFunc_GREvent(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int gameevent = READ_BYTE();
	short data1 = READ_SHORT();
	short data2 = READ_SHORT();
	END_READ();

//#ifdef _DEBUG
//	CON_DPRINTF("cl: MsgFunc_GREvent(%d %d %d)\n", gameevent, data1, data2);
//#endif
	GameRulesEvent(gameevent, data1, data2);
	return 1;
}

/*
int CHud::MsgFunc_Concuss( const char *pszName, int iSize, void *pbuf )
{
	int r,g,b;
	UnpackRGB(r,g,b, RGB_YELLOW);

	BEGIN_READ( pbuf, iSize );
	m_iConcussionEffect = READ_BYTE();
	END_READ();
	if (m_iConcussionEffect)
		this->m_StatusIcons.EnableIcon("dmg_concuss",r,g,b);
	else
		this->m_StatusIcons.DisableIcon("dmg_concuss");
	return 1;
}
*/
