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
// hud_redraw.cpp
//
#include <math.h>
#include "hud.h"
#include "cl_util.h"
#include "in_defs.h"
#include "vgui_XDMViewport.h"
#include "vgui_ScorePanel.h"

//-----------------------------------------------------------------------------
// Purpose: step through the local data, placing the appropriate graphics & text as appropriate
// Input  : flTime - 
//			intermission - 
// Output : returns 1 if they've changed, 0 otherwise
//-----------------------------------------------------------------------------
int CHud::Redraw(const float &flTime, const int &intermission)
{
	m_fOldTime = m_flTime;	// save time of previous redraw
	m_flTime = flTime;
	m_flTimeDelta = (double)m_flTime - m_fOldTime;

	// Clock was reset, reset delta
	if (m_flTimeDelta < 0)
		m_flTimeDelta = 0;

	if (m_flTimeLeft > 0 && m_flTimeDelta > 0)
		m_flTimeLeft -= m_flTimeDelta;//only float!!

	// Bring up the scoreboard during intermission
	if (m_iIntermission && !intermission)
	{
		// Have to do this here so the scoreboard goes away
		m_iIntermission = intermission;
		IntermissionEnd();
	}
	else if (!m_iIntermission && intermission)
	{
		m_iIntermission = intermission;
		IntermissionStart();

		// Take a screenshot if the client's got the cvar set
		if (m_pCvarTakeShots->value > 0.0f)
			m_flShotTime = flTime + 1.0f;// Take a screenshot in a second
	}

	if (m_iPaused == 0)// allow in demo && m_iActive > 0)
	{
		if (gViewPort->GetScoreBoard())
			gViewPort->GetScoreBoard()->UpdateCounters();
	}

	m_iIntermission = intermission;

	if (m_flShotTime && m_flShotTime < flTime)
	{
		CLIENT_COMMAND("snapshot\n");
		m_flShotTime = 0;
	}

	// if no redrawing is necessary
	// return 0;

	if (m_pCvarDraw->value)
	{
		HUDLIST *pList = m_pHudList;
		while (pList)
		{
			if (intermission == 0)
			{
				if (pList->p->IsActive())
				{
					if (!(m_iHideHUDDisplay & HIDEHUD_ALL) || pList->p->m_iFlags & HUD_DRAW_ALWAYS)
						pList->p->Draw(flTime);
				}
			}
			else
			{  // it's an intermission,  so only draw hud elements that are set to draw during intermissions
				if (pList->p->m_iFlags & HUD_INTERMISSION)
					pList->p->Draw(flTime);
			}
			pList = pList->pNext;
		}
	}

	// are we in demo mode? do we need to draw the logo in the top corner?
	if (m_iLogo)
	{
		int x, y;
		if (m_hsprLogo == 0)
			m_hsprLogo = LoadSprite("sprites/%d_logo.spr");

		SPR_Set(m_hsprLogo, 255,255,255);

		x = SPR_Width(m_hsprLogo, 0);
		x = ScreenWidth - x;
		y = SPR_Height(m_hsprLogo, 0)/2;

		// Draw the logo at 20 fps
		int iFrame = (int)(flTime * 20) % SPR_Frames(m_hsprLogo);
//		i = grgLogoFrame[iFrame] - 1;

		SPR_DrawAdditive(iFrame, x, y, NULL);
	}

	if (g_iMouseManipulationMode != MMM_NONE)// XDM3036
	{
		char szMMModeMsg[16];
		sprintf(szMMModeMsg, "#MMMODE%d\0", g_iMouseManipulationMode);
		DrawConsoleString(4, ScreenHeight/2, LookupString(szMMModeMsg, NULL));
	}

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : xpos ypos - 
//			iMaxX - 
//			*szIt - 
//			r g b - 
// Output : int
//-----------------------------------------------------------------------------
/*int CHud::DrawHudString(int xpos, int ypos, int iMaxX, char *szIt, int r, int g, int b)
{
	// draw the string until we hit the null character or a newline character
	for (; *szIt != 0 && *szIt != '\n'; szIt++)
	{
		int next = xpos + gHUD.m_scrinfo.charWidths[*szIt]; // variable-width fonts look cool
		if (next > iMaxX)
			return xpos;

		TextMessageDrawChar(xpos, ypos, *szIt, r, g, b);
		xpos = next;		
	}
	return xpos;
}*/

//-----------------------------------------------------------------------------
// Purpose: draws a string from right to left (right-aligned)
// Input  : xpos ypos - 
//			iMinX - 
//			*szString - 
//			r g b - 
// Output : int
//-----------------------------------------------------------------------------
int CHud::DrawHudStringReverse(int xpos, int ypos, int iMinX, char *szString, const int &r, const int &g, const int &b)
{
	// find the end of the string
	for (char *szIt = szString; *szIt != 0; szIt++)
	{ // we should count the length?		
	}
	// iterate throug the string in reverse
	for (szIt--;  szIt != (szString-1);  szIt--)	
	{
		int next = xpos - gHUD.m_scrinfo.charWidths[*szIt]; // variable-width fonts look cool
		if (next < iMinX)
			return xpos;

		xpos = next;
		TextMessageDrawChar(xpos, ypos, *szIt, r, g, b);
	}
	return xpos;
}

//-----------------------------------------------------------------------------
// Purpose: Number as textmessage
// Input  : xpos ypos - 
//			iMinX - 
//			iNumber - 
//			r g b - 
// Output : int
//-----------------------------------------------------------------------------
int CHud::DrawHudNumberString(int xpos, int ypos, int iMinX, int iNumber, const int &r, const int &g, const int &b)
{
	char szString[32];
	sprintf(szString, "%d", iNumber);
	return DrawHudStringReverse(xpos, ypos, iMinX, szString, r, g, b);
}

//-----------------------------------------------------------------------------
// Purpose: LARGE number
// Input  : x y - 
//			iFlags - 
//			iNumber - 
//			r g b - 
// Output : int CHud ::
//-----------------------------------------------------------------------------
int CHud::DrawHudNumber(int x, int y, int iFlags, int iNumber, const int &r, const int &g, const int &b)
{
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	int k;
	
	if (iNumber > 0)
	{
		// SPR_Draw 100's
		if (iNumber >= 100)
		{
			 k = iNumber/100;
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			SPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive(0, x, y, &rc);
			x += iWidth;
		}

		// SPR_Draw 10's
		if (iNumber >= 10)
		{
			k = (iNumber % 100)/10;
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
			SPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive(0, x, y, &rc);
			x += iWidth;
		}

		// SPR_Draw ones
		k = iNumber % 10;
		SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b);
		SPR_DrawAdditive(0,  x, y, &GetSpriteRect(m_HUD_number_0 + k));
		x += iWidth;
	} 
	else if (iFlags & DHN_DRAWZERO) 
	{
		SPR_Set(GetSprite(m_HUD_number_0), r, g, b);

		// SPR_Draw 100's
		if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive(0, x, y, &rc );
			x += iWidth;
		}

		if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive(0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		SPR_DrawAdditive(0,  x, y, &GetSpriteRect(m_HUD_number_0));
		x += iWidth;
	}
	return x;
}

int CHud :: DrawHudNumberLarge(int x, int y, int iFlags, int iNumber, const int &r, const int &g, const int &b)
{
	int iWidth = GetSpriteRect(m_HUD_number_0).right - GetSpriteRect(m_HUD_number_0).left;
	int k;
	
	if (iNumber > 0)
	{

	// SPR_Draw 1000's
	if (iNumber >= 1000)
        {
	k = (iNumber % 10000)/1000;
            SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
            SPR_DrawAdditive( 0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
            x += iWidth;
        }
        else if (iFlags & (DHN_3DIGITS))
        {
            x += iWidth;
        }

		// SPR_Draw 100's
if (iNumber >= 100)
        {
             k = (iNumber % 1000)/100;
            SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
            SPR_DrawAdditive( 0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
            x += iWidth;
        }
		else if (iFlags & (DHN_3DIGITS))
		{
			x += iWidth;
		}

		// SPR_Draw 10's
		if (iNumber >= 10)
		{
			k = (iNumber % 100)/10;
			SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect(m_HUD_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		k = iNumber % 10;
		SPR_Set(GetSprite(m_HUD_number_0 + k), r, g, b );
		SPR_DrawAdditive(0,  x, y, &GetSpriteRect(m_HUD_number_0 + k));
		x += iWidth;
	} 
	else if (iFlags & DHN_DRAWZERO) 
	{
		SPR_Set(GetSprite(m_HUD_number_0), r, g, b );

		// SPR_Draw 100's
		if (iFlags & (DHN_3DIGITS))
		{
			x += iWidth;
		}

		if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			x += iWidth;
		}

		SPR_DrawAdditive( 0,  x, y, &GetSpriteRect(m_HUD_number_0));
		x += iWidth;
	}

	return x;
}

//-----------------------------------------------------------------------------
// Purpose: Count number of digits in a number, which may be overridden by flags
// Input  : iNumber - 
//			iFlags - 
// Output : int
//-----------------------------------------------------------------------------
int CHud::GetNumWidth(const int &iNumber, const int &iFlags)
{
	if (iFlags & (DHN_3DIGITS))
		return 3;

	if (iFlags & (DHN_2DIGITS))
		return 2;

	if (iNumber <= 0)
	{
		if (iFlags & (DHN_DRAWZERO))
			return 1;
		else
			return 0;
	}

	if (iNumber < 10)
		return 1;

	if (iNumber < 100)
		return 2;

	return 3;
}

//-----------------------------------------------------------------------------
// Purpose: Y all bottom text should be centered around
// Output : int Y
//-----------------------------------------------------------------------------
int CHud::GetHUDBottomLine(void)
{
	return ScreenHeight - (gHUD.m_iFontHeight * 1.5);
}

//-----------------------------------------------------------------------------
// Purpose: small sized number
// Input  : x y - 
//			iFlags - 
//			iNumber - 
//			r g b - 
// Output : int CHud ::
//-----------------------------------------------------------------------------
int CHud::DrawHudNumberSmall(int x, int y, int iFlags, int iNumber, const int &r, const int &g, const int &b)
{
	int iWidth = GetSpriteRect(m_HUD_sm_number_0).right - GetSpriteRect(m_HUD_sm_number_0).left;
	int k;
	
	if (iNumber > 0)
	{
		// SPR_Draw 100's
		if (iNumber >= 100)
		{
			 k = iNumber/100;
			SPR_Set(GetSprite(m_HUD_sm_number_0 + k), r, g, b);
			SPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_sm_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive(0, x, y, &rc);
			x += iWidth;
		}

		// SPR_Draw 10's
		if (iNumber >= 10)
		{
			k = (iNumber % 100)/10;
			SPR_Set(GetSprite(m_HUD_sm_number_0 + k), r, g, b);
			SPR_DrawAdditive(0, x, y, &GetSpriteRect(m_HUD_sm_number_0 + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive(0, x, y, &rc);
			x += iWidth;
		}

		// SPR_Draw ones
		k = iNumber % 10;
		SPR_Set(GetSprite(m_HUD_sm_number_0 + k), r, g, b);
		SPR_DrawAdditive(0,  x, y, &GetSpriteRect(m_HUD_sm_number_0 + k));
		x += iWidth;
	} 
	else if (iFlags & DHN_DRAWZERO) 
	{
		SPR_Set(GetSprite(m_HUD_sm_number_0), r, g, b);

		// SPR_Draw 100's
		if (iFlags & (DHN_3DIGITS))
		{
			//SPR_DrawAdditive(0, x, y, &rc );
			x += iWidth;
		}

		if (iFlags & (DHN_3DIGITS | DHN_2DIGITS))
		{
			//SPR_DrawAdditive(0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		SPR_DrawAdditive(0,  x, y, &GetSpriteRect(m_HUD_sm_number_0));
		x += iWidth;
	}
	return x;
}
	
