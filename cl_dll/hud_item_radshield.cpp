#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>
#include "weapondef.h"
#include "pm_defs.h"
#include "pm_shared.h"

int CHudItemRadShield::Init(void) 
{
	m_iTimer = 0;
    gHUD.AddHudElem(this);
    return 1;
};

int CHudItemRadShield::VidInit(void)
{
	m_iSprite = gHUD.GetSpriteIndex("item_radshield");
	return 1;
};

int CHudItemRadShield::MsgFunc_ItemRadShield(const char *pszName,  int iSize, void *pbuf )
{
	m_iTimer = READ_SHORT();

	if (m_iTimer > 0)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudItemRadShield::Draw(const float &flTime)
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) || gEngfuncs.IsSpectateOnly())
		return 0;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 0;

	const char *pkv = pmove->PM_Info_ValueForKey(pmove->physinfo, PHYSKEY_RADSHIELD);
	if (!pkv || *pkv == '0')
		return 0;

	int r,g,b, x,y;
	int Width = gHUD.GetSpriteRect(m_iSprite).right - gHUD.GetSpriteRect(m_iSprite).left;

	x = Width/3;
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight*6.0;

	if (m_iTimer < TIME_RADSHIELD*0.25) 
	{
		r = 255; g = 0;	b = 0;
	}
	if (m_iTimer >= TIME_RADSHIELD*0.25 && m_iTimer <= TIME_RADSHIELD*0.5)
	{
		r = 255; g = 128;	b = 0;
	}
	if (m_iTimer > TIME_RADSHIELD*0.5)
	{
		r = 0; g = 255;	b = 0;
	}
		
	SPR_Set(gHUD.GetSprite(m_iSprite), 0,125,0);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_iSprite));

	x = Width;
	gHUD.DrawHudNumberSmall(x, y, DHN_3DIGITS, m_iTimer, r,g,b);
	return 1;
}