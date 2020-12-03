#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_XDMViewport.h"
#include "gamedefs.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSLight.h"


DECLARE_MESSAGE(m_FlagDisplay, FlagInfo);

int CHudFlagDisplay::Init(void)
{
	HOOK_MESSAGE(FlagInfo);

	m_iFlags = HUD_INTERMISSION|HUD_DRAW_ALWAYS;// XDM3035
	InitHUDData();

	gHUD.AddHudElem(this);
	return 1;
}

void CHudFlagDisplay::InitHUDData(void)
{
	m_iNumFlags = 0;
	for (int i = 0; i < MAX_CAPTURE_ENTS; ++i)
	{
		m_iFlagEnts[i] = 0;
		m_iFlagTeam[i] = TEAM_NONE;
		m_iFlagState[i] = 0;
	}
}

int CHudFlagDisplay::VidInit(void)
{
	m_iFlagNormal = gHUD.GetSpriteIndex("flag");
	m_iFlagTaken = gHUD.GetSpriteIndex("flag_t");
	m_iFlagDropped = gHUD.GetSpriteIndex("flag_d");
	m_hsprOvwIcon = SPR_Load("sprites/spec_domobject.spr");
	return 1;
}

int CHudFlagDisplay::MsgFunc_FlagInfo(const char *pszName,  int iSize, void *pbuf)
{
	if (m_iNumFlags >= MAX_CAPTURE_ENTS)
		return 1;

//	if (m_iGameType == GT_CTF)
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ(pbuf, iSize);
	m_iFlagEnts[m_iNumFlags] = READ_SHORT();
	m_iFlagTeam[m_iNumFlags] = READ_BYTE();
	END_READ();

	int r,g,b;
	GetTeamColor(m_iFlagTeam[m_iNumFlags], r,g,b);

	if (g_pRenderManager)
		g_pRenderManager->AddSystem(new CRSLight(Vector(0,0,2048), r,g,b, 128, NULL, 0.0, 0.0), 0, m_iFlagEnts[m_iNumFlags], RENDERSYSTEM_FFLAG_ICNF_NODRAW);

// doesn't seem to work
	if (!m_iFlagNormal)
		m_iFlagNormal = gHUD.GetSpriteIndex("flag");

// Useless because StartObserver() sends InitHUD message that forces m_Spectator to reset data
//	if (!gHUD.m_Spectator.AddOverviewEntityToList(gHUD.GetSprite(m_iFlagNormal), gEngfuncs.GetEntityByIndex(m_iFlagEnts[m_iNumFlags]), 0))
//		CON_PRINTF("CL: WARNING: unable to add flag %d to the overview entlist!\n", m_iFlagEnts[m_iNumFlags]);

//	CON_PRINTF(" >>>>>> Registered flag %d for team %d!\n", m_iFlagEnts[m_iNumFlags], m_iFlagTeam[m_iNumFlags]);
	m_iNumFlags ++;
	return 1;
}

int CHudFlagDisplay::Draw(const float &flTime)
{
	int r,g,b;
	int spr = m_iFlagNormal;

	wrect_t *pRect = &gHUD.GetSpriteRect(spr);
	int icon_width = pRect->right - pRect->left;
	int icon_height = pRect->top - pRect->bottom*1.1;

	int dig_width = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;

	int x = ScreenWidth - icon_width;
	int y = (ScreenHeight - icon_height*MAX_CAPTURE_ENTS)/2;

	for (int i=0; i < m_iNumFlags; i++)
	{
		if (m_iFlagState[i] == CTF_EV_TAKEN)// our flag
			spr = m_iFlagTaken;
		else if (m_iFlagState[i] == CTF_EV_DROP)
			spr = m_iFlagDropped;
		else
			spr = m_iFlagNormal;

		GetTeamColor(m_iFlagTeam[i], r,g,b);

		pRect = &gHUD.GetSpriteRect(spr);// !!
		SPR_Set(gHUD.GetSprite(spr), r, g, b);
		SPR_DrawAdditive(0, x, y, pRect);

		cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(m_iFlagEnts[i]);
		if (pEntity)
		{
			pEntity->curstate.team = m_iFlagTeam[i];// XDM3037: force set team because entity network updates may not be available
			gHUD.m_Spectator.AddOverviewEntityToList(pEntity, gEngfuncs.GetSpritePointer(m_hsprOvwIcon), -1);
		}

		gHUD.DrawHudNumber(x-dig_width*2, y+gHUD.m_iFontHeight/2, DHN_2DIGITS | DHN_DRAWZERO, g_TeamInfo[m_iFlagTeam[i]].scores_overriden, r, g, b);
		y += icon_height;
	}
	return 1;
}

void CHudFlagDisplay::SetEntState(int entindex, int team, int state)
{
	for (int i=0; i < m_iNumFlags; i++)
	{
		if (m_iFlagEnts[i] == entindex)
			m_iFlagState[i] = state;
//			CON_PRINTF(" Flag %d state changed to %d\n", entindex, state);
	}
}

void CHudFlagDisplay::Reset(void)
{
//	for (int i=0; i < m_iNumFlags; i++)
// GetEntityByIndex() may return NULL now		gHUD.m_Spectator.AddOverviewEntityToList(m_hsprOvwIcon, gEngfuncs.GetEntityByIndex(m_iFlagEnts[i]), 0);
}
