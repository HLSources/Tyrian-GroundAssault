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
// hud.cpp
//
// implementation of CHud class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "hud_servers.h"
#include "demo.h"
#include "demo_api.h"
#include "vgui_XDMViewport.h"
#include "musicplayer.h"
#include "RenderManager.h"// XDM
#include "cl_fx.h"
#include "pm_defs.h"
#include "pm_shared.h"

extern cl_entity_t *g_pWorld;

CHud::CHud(): m_iSpriteCount(0), m_pHudList(NULL)
{
	m_flTime = 0.0f;
	m_flShotTime = 0.0f;
	m_flNextAnnounceTime = 0.0f;// XDM3035
	m_iTeamNumber = TEAM_NONE;
	m_iPaused = 0;
	m_iGameType = 0;
	m_iGameMode = 0;
	m_iGameSkillLevel = 0;
	m_iGameFlags = 0;
	m_iRevengeMode = 0;
}

// This is called every time the DLL is loaded
void CHud::Init(void)
{
	memset(m_szMessageAward, 0, sizeof(m_szMessageAward));
	memset(m_szMessageCombo, 0, sizeof(m_szMessageCombo));
	memset(m_szMessageTimeLeft, 0, sizeof(m_szMessageTimeLeft));
	memset(m_szMessageAnnouncement, 0, sizeof(m_szMessageAnnouncement));
	memset(&m_MessageAward, 0, sizeof(client_textmessage_t));
	memset(&m_MessageCombo, 0, sizeof(client_textmessage_t));
	memset(&m_MessageTimeLeft, 0, sizeof(client_textmessage_t));
	memset(&m_MessageAnnouncement, 0, sizeof(client_textmessage_t));

	// XDM3036: only HUD-vars here
	m_pCvarColorMain	= CVAR_CREATE("hud_grn",			"160 160 255",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL);// RGB_GREEN
	m_pCvarColorRed		= CVAR_CREATE("hud_red",			"255 0 0",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL);// RGB_RED
	m_pCvarColorBlue	= CVAR_CREATE("hud_blu",			"0 0 255",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL);// RGB_BLUE
	m_pCvarColorCyan	= CVAR_CREATE("hud_cyn",			"0 255 255",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL);// RGB_CYAN
	m_pCvarColorYellow	= CVAR_CREATE("hud_yel",			"255 255 0",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL);// RGB_YELLOW
	m_pCvarStealMouse	= CVAR_CREATE("hud_capturemouse",	"1",			FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_pCvarDraw			= CVAR_CREATE("hud_draw",			"1",			FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_pCvarUseTeamColor	= CVAR_CREATE("hud_useteamcolor",	"0",			FCVAR_ARCHIVE | FCVAR_CLIENTDLL);// XDM
	m_pCvarUsePlayerColor=CVAR_CREATE("hud_useplayercolor",	"1",			FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_pCvarMiniMap		= CVAR_CREATE("hud_minimap",		"0",			FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_pCvarTakeShots	= CVAR_CREATE("hud_takesshots",		"0",			FCVAR_ARCHIVE | FCVAR_CLIENTDLL);// controls whether or not to automatically take screenshots at the end of a round

	m_flTimeLeft = 0.0f;
	m_iScoreLeftLast = 0;// XDM3035
	m_iTimeLeftLast = 0;
	m_flNextAnnounceTime = 0.0f;// XDM3035

	m_pSpriteList = NULL;
	m_iFogMode = 0;// XDM3035
	m_iSkyMode = 0;
	m_iLogo = 0;
	m_iFOV = DEFAULT_FOV;
//	m_iCameraMode = 0;

	m_flFogStart = 0.0f;
	m_flFogEnd = 0.0f;

	// Clear any old HUD list
	if ( m_pHudList )
	{
		HUDLIST *pList;
		while ( m_pHudList )
		{
			pList = m_pHudList;
			m_pHudList = m_pHudList->pNext;
			free( pList );
		}
		m_pHudList = NULL;
	}

	// In case we get messages before the first update -- time will be valid
	m_flTime = 1.0;
	m_ZoomCrosshair.Init();// XDM: must be first!
	m_Ammo.Init();
	m_Health.Init();
	m_SayText.Init();
	m_Spectator.Init();
	m_Geiger.Init();
	m_Train.Init();
	m_Battery.Init();
	m_Flash.Init();
	m_Message.Init();
	m_StatusBar.Init();
	m_DeathNotice.Init();
	m_AmmoSecondary.Init();
	m_TextMessage.Init();
	m_StatusIcons.Init();
//	m_RocketScreen.Init();// XDM
	m_DomDisplay.Init();
	m_FlagDisplay.Init();
//	m_Menu.Init();
	ServersInit();

	m_ItemAccuracy.Init();
	m_ItemBanana.Init();
	m_ItemInvisibility.Init();
	m_ItemInvulnerability.Init();
	m_ItemQuaddamage.Init();
	m_ItemShieldregen.Init();
	m_ItemHaste.Init();
	m_ItemRapidfire.Init();
	m_ItemAntidote.Init();
	m_ItemLightningField.Init();
	m_ItemRadShield.Init();
	m_ItemShieldStrength.Init();
	m_ItemFireSupressor.Init();
	m_ItemPlasmaShield.Init();
	m_WpnIcon.Init();
	m_ItemWeaponPower.Init();

	MsgFunc_ResetHUD(0, 0, NULL);
}

// CHud destructor
// cleans up memory allocated for m_rg* arrays
CHud :: ~CHud()
{
	delete [] m_rghSprites;
	delete [] m_rgrcRects;
	delete [] m_rgszSpriteNames;

	if ( m_pHudList )
	{
		HUDLIST *pList;
		while ( m_pHudList )
		{
			pList = m_pHudList;
			m_pHudList = m_pHudList->pNext;
			free( pList );
		}
		m_pHudList = NULL;
	}

	ServersShutdown();
}

// GetSpriteIndex()
// searches through the sprite list loaded from hud.txt for a name matching SpriteName
// returns an index into the gHUD.m_rghSprites[] array
// returns 0 if sprite not found
int CHud :: GetSpriteIndex( const char *SpriteName )
{
	// look through the loaded sprite name list for SpriteName
	for (int i = 0; i < m_iSpriteCount; ++i)
	{
		if ( strncmp( SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH ) == 0 )
			return i;
	}

	return -1; // invalid sprite
}

void CHud :: VidInit( void )
{
	m_scrinfo.iSize = sizeof(m_scrinfo);
	GetScreenInfo(&m_scrinfo);

	m_iFogMode = 0;// XDM: clear out the fog!
	ResetFog();

	// Load Sprites
//	m_hsprFont = LoadSprite("sprites/%d_font.spr");
	m_hsprLogo = 0;	
	m_hsprCursor = 0;

	if (ScreenWidth < 640)
		m_iRes = 320;
	else
		m_iRes = 640;

	int j = 0;
	// Only load this once
	if ( !m_pSpriteList )
	{
		// we need to load the hud.txt, and all sprites within
		m_pSpriteList = SPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList)
		{
			// count the number of sprites of the appropriate res
			m_iSpriteCount = 0;
			client_sprite_t *p = m_pSpriteList;
			for (j = 0; j < m_iSpriteCountAllRes; ++j)
			{
				if ( p->iRes == m_iRes )
					++m_iSpriteCount;
				p++;
			}

			// allocated memory for sprite handle arrays
 			m_rghSprites = new HSPRITE[m_iSpriteCount];
			m_rgrcRects = new wrect_t[m_iSpriteCount];
			m_rgszSpriteNames = new char[m_iSpriteCount * MAX_SPRITE_NAME_LENGTH];

			p = m_pSpriteList;
			int index = 0;
			for (j = 0; j < m_iSpriteCountAllRes; ++j)
			{
				if ( p->iRes == m_iRes )
				{
					char sz[256];
					sprintf(sz, "sprites/%s.spr", p->szSprite);
					m_rghSprites[index] = SPR_Load(sz);
					m_rgrcRects[index] = p->rc;
					strncpy( &m_rgszSpriteNames[index * MAX_SPRITE_NAME_LENGTH], p->szName, MAX_SPRITE_NAME_LENGTH );
					++index;
				}

				p++;
			}
		}
	}
	else
	{
		// we have already have loaded the sprite reference from hud.txt, but
		// we need to make sure all the sprites have been loaded (we've gone through a transition, or loaded a save game)
		client_sprite_t *p = m_pSpriteList;
		int index = 0;
		for (j = 0; j < m_iSpriteCountAllRes; ++j)
		{
			if ( p->iRes == m_iRes )
			{
				char sz[256];
				sprintf( sz, "sprites/%s.spr", p->szSprite );
				m_rghSprites[index] = SPR_Load(sz);
				index++;
			}
			p++;
		}
	}

	// assumption: number_1, number_2, etc, are all listed and loaded sequentially
	m_HUD_number_0 = GetSpriteIndex( "number_0" );
	m_HUD_sm_number_0 = GetSpriteIndex( "sm_number_0" );

	m_iFontHeight = m_rgrcRects[m_HUD_number_0].bottom - m_rgrcRects[m_HUD_number_0].top;

	m_Ammo.VidInit();
	m_Health.VidInit();
	m_Spectator.VidInit();
	m_Geiger.VidInit();
	m_Train.VidInit();
	m_Battery.VidInit();
	m_Flash.VidInit();
	m_Message.VidInit();
	m_StatusBar.VidInit();
	m_DeathNotice.VidInit();
	m_SayText.VidInit();
//	m_Menu.VidInit();
	m_AmmoSecondary.VidInit();
	m_TextMessage.VidInit();
	m_StatusIcons.VidInit();
	m_ZoomCrosshair.VidInit();// XDM
//	m_RocketScreen.VidInit();
	m_DomDisplay.VidInit();
	m_FlagDisplay.VidInit();

	m_ItemAccuracy.VidInit();
	m_ItemBanana.VidInit();
	m_ItemInvisibility.VidInit();
	m_ItemInvulnerability.VidInit();
	m_ItemQuaddamage.VidInit();
	m_ItemShieldregen.VidInit();
	m_ItemHaste.VidInit();
	m_ItemRapidfire.VidInit();
	m_ItemAntidote.VidInit();
	m_ItemLightningField.VidInit();
	m_ItemRadShield.VidInit();
	m_ItemShieldStrength.VidInit();
	m_ItemFireSupressor.VidInit();
	m_ItemPlasmaShield.VidInit();
	m_ItemWeaponPower.VidInit();

	m_WpnIcon.VidInit();
}

float g_lastFOV = 0.0f;// XDM3035c: TODO: int?

/*
=====================
HUD_GetFOV

Returns last FOV
=====================
*/
float HUD_GetFOV(void)
{
	if (gEngfuncs.pDemoAPI->IsRecording())
	{
		// Write it
		int i = 0;
		unsigned char buf[100];

		// Active
		*( float * )&buf[i] = g_lastFOV;
		i += sizeof(float);

		Demo_WriteBuffer(TYPE_ZOOM, i, buf);
	}

	if (gEngfuncs.pDemoAPI->IsPlayingback())
		g_lastFOV = g_demozoom;

	return g_lastFOV;
}

void CHud::AddHudElem(CHudBase *phudelem)
{
	HUDLIST *pdl, *ptemp;

//phudelem->Think();

	if (!phudelem)
		return;

	pdl = (HUDLIST *)malloc(sizeof(HUDLIST));
	if (!pdl)
		return;

	memset(pdl, 0, sizeof(HUDLIST));
	pdl->p = phudelem;

	if (!m_pHudList)
	{
		m_pHudList = pdl;
		return;
	}

	ptemp = m_pHudList;

	while (ptemp->pNext)
		ptemp = ptemp->pNext;

	ptemp->pNext = pdl;
}

float CHud::GetSensitivity(void)
{
	return m_flMouseSensitivity;
}

float CHud::GetUpdatedDefaultFOV(void)// XDM
{
	if (g_pCvarDefaultFOV->value == 0.0f)
	{
//		g_pCvarDefaultFOV->value = DEFAULT_FOV;
		CVAR_SET_FLOAT(g_pCvarDefaultFOV->name, DEFAULT_FOV);// slow
	}
	return g_pCvarDefaultFOV->value;
}

// XDM: called when game state is changed
void CHud::OnGamePaused(int paused)
{
	m_iPaused = paused;
	BGM_GamePaused(paused);
}

// XDM: called when game state is changed
void CHud::OnGameActivated(int active)
{
	m_iActive = active;
	if (active)
	{
		CL_Precache();
		g_pWorld = gEngfuncs.GetEntityByIndex(0);
	}
	else
	{
		if (g_pRenderManager)// XDM3035a
			g_pRenderManager->DeleteAllSystems();

		g_pWorld = NULL;// XDM3035c
	}
}



//-----------------------------------------------------------------------------
// Purpose: XDM3038a: Is local player alive?
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHud::PlayerIsAlive(void)
{
	if (IsSpectator())
	{
		if (g_iUser2 > 0)
		{
			cl_entity_t *pEnt = gEngfuncs.GetEntityByIndex(g_iUser2);
			if (pEnt && pEnt->curstate.health > 0)
				return true;
		}
		return false;// ?
	}
	else if (pmove)
		return (pmove->dead == false);
	// BAD! CRASH when gmsgDamage is called too early (CO_AI )else if (gHUD.m_pLocalPlayer)
	//	return (gHUD.m_pLocalPlayer->curstate.health > 0);
	else
		return (gHUD.m_Health.m_iHealth > 0);//return (gHUD.m_ClientData.health > 0);
}


bool CHudBase::IsActive(void)
{
	return (m_iFlags & HUD_ACTIVE);
}

void CHudBase::SetActive(bool active)
{
	if (active)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
}
