#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_XDMViewport.h"


DECLARE_MESSAGE(m_DomDisplay, DomInfo);

int CHudDomDisplay::Init(void)
{
	HOOK_MESSAGE(DomInfo);

	m_iFlags = HUD_INTERMISSION|HUD_DRAW_ALWAYS;// XDM3035
	InitHUDData();

	gHUD.AddHudElem(this);
	return 1;
}

// Don't reset precached resource indexes here!
void CHudDomDisplay::InitHUDData(void)
{
	m_iNumDomPoints = 0;
	for (int i = 0; i < MAX_DOM_POINTS; ++i)
	{
		m_iDomPointEnts[i] = 0;
		m_iDomPointTeam[i] = TEAM_NONE;
		m_szDomPointNames[i][0] = 0;
//		memset(m_szDomPointNames[i], 0, sizeof(m_szDomPointNames));
	}
	memset(m_szMessage, 0, sizeof(m_szMessage));
	memset(&m_Message, 0, sizeof(client_textmessage_t));
}

int CHudDomDisplay::VidInit(void)
{
	m_iSprite = gHUD.GetSpriteIndex("dompoint");
	m_hsprOvwIcon = SPR_Load("sprites/spec_domobject.spr");
	return 1;
}

int CHudDomDisplay::MsgFunc_DomInfo(const char *pszName,  int iSize, void *pbuf)
{
	if (m_iNumDomPoints >= MAX_DOM_POINTS)
		return 1;

//	if (m_iGameType == GT_DOMINATION)
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ(pbuf, iSize);

	int entindex = READ_SHORT();
	int team = READ_BYTE();// XDM3033

	for (int i=0; i < m_iNumDomPoints; ++i)// don't add existing points
	{
		if (m_iDomPointEnts[i] == entindex)
			return 1;
	}
	m_iDomPointEnts[m_iNumDomPoints] = entindex;
	m_iDomPointTeam[m_iNumDomPoints] = team;// XDM3033
//ok	strncpy(m_szDomPointNames[m_iNumDomPoints], READ_STRING(), DOM_NAME_LENGTH);
//buggy!	LocaliseTextString(READ_STRING(), m_szDomPointNames[m_iNumDomPoints], DOM_NAME_LENGTH);

	char *pName = m_szDomPointNames[m_iNumDomPoints];
	memset(pName, 0, DOM_NAME_LENGTH);
	strncpy(pName, LookupString(READ_STRING()), DOM_NAME_LENGTH);
	StripEndNewlineFromString(pName);
	pName[DOM_NAME_LENGTH-1] = 0;
//	CON_DPRINTF(" CL  MsgFunc_DomInfo: %d %s\n", m_iDomPointEnts[m_iNumDomPoints], m_szDomPointNames[m_iNumDomPoints]);
	END_READ();
	m_iNumDomPoints ++;
	return 1;
}

int CHudDomDisplay::Draw(const float &flTime)
{
	wrect_t *pRect = &gHUD.GetSpriteRect(m_iSprite);
	int r,g,b, sw,sh;
	int w = pRect->right - pRect->left;
	int h = pRect->bottom - pRect->top;
	int x = ScreenWidth - w;
	int y = (ScreenHeight - h*MAX_DOM_POINTS)/2;

	for (int i=0; i < m_iNumDomPoints; ++i)
	{
		GetTeamColor(m_iDomPointTeam[i], r,g,b);
		SPR_Set(gHUD.GetSprite(m_iSprite), r, g, b);
		SPR_DrawAdditive(0,  x, y, pRect);

		if (gViewPort && gViewPort->AllowedToPrintText())
		{
			DrawSetTextColor(r,g,b);
			gEngfuncs.pfnDrawConsoleStringLen(m_szDomPointNames[i], &sw, &sh);
//			CON_PRINTF("w %d  h %d  sw %d  sh %d\n", w,h,sw,sh);
			DrawConsoleString(sw<=w?x:(ScreenWidth-sw), y+h, m_szDomPointNames[i]);
			y += sh;
		}

		cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(m_iDomPointEnts[i]);
		if (pEntity)
		{
			pEntity->curstate.team = m_iDomPointTeam[i];// XDM3037: force set team because entity network updates may not be available
			gHUD.m_Spectator.AddOverviewEntityToList(pEntity, gEngfuncs.GetSpritePointer(m_hsprOvwIcon), -1);
		}

		y += h;
	}
	return 1;
}

void CHudDomDisplay::SetEntTeam(int entindex, int teamindex)
{
	for (int i=0; i < m_iNumDomPoints; ++i)
	{
		if (m_iDomPointEnts[i] == entindex)
		{
			m_iDomPointTeam[i] = teamindex;
			if (gViewPort)
			{
				// get text FORMAT from localized string and store it in m_szMessage
				client_textmessage_t *msg = TextMessageGet("DOM_SETPOINT");
				if (msg)
				{
					byte r,g,b;
					GetTeamColor(teamindex, r,g,b);
					sprintf(m_szMessage, msg->pMessage, m_szDomPointNames[i], gViewPort->GetTeamName(teamindex));
					m_szMessage[DOM_MSG_LENGTH-1] = 0;

					memcpy(&m_Message, msg, sizeof(client_textmessage_t));
//					m_Message = *msg;// copy localized message
					m_Message.x = -1;// override some parameters
//					m_Message.y = 0.9;
					m_Message.r1 = r;
					m_Message.g1 = g;
					m_Message.b1 = b;
					m_Message.a1 = 220;
					m_Message.holdtime = 3.0;
					m_Message.pName = "DOM_MSG";
					m_Message.pMessage = m_szMessage;
					gHUD.m_Message.MessageAdd(&m_Message);
				}
			}
		}
	}
}
