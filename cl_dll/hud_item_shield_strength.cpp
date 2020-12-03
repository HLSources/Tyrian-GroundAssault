#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>
#include "weapondef.h"
#include "pm_defs.h"
#include "pm_shared.h"

int CHudItemShieldStrength::Init(void) 
{
	m_iAmount = 0;
    gHUD.AddHudElem(this);
    return 1;
};

int CHudItemShieldStrength::VidInit(void)
{
	m_iSprite = gHUD.GetSpriteIndex("upgrade");
	return 1;
};

int CHudItemShieldStrength::MsgFunc_ItemShieldStrength(const char *pszName,  int iSize, void *pbuf )
{
	m_iAmount = READ_SHORT();

	if (m_iAmount > 0)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;
	return 1;
}

int CHudItemShieldStrength::Draw(const float &flTime)
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) || gEngfuncs.IsSpectateOnly())
		return 0;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 0;

	int r,g,b, x,y;
	r = 0; g = 255;	b = 0;

	y = ScreenHeight - gHUD.m_iFontHeight*1.4;
	x = ScreenWidth/5.5;

	SPR_Set(gHUD.GetSprite(m_iSprite), 0,255,0);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_iSprite));

	y = ScreenHeight - gHUD.m_iFontHeight * 1.1;
	x = ScreenWidth/6.2;
	gHUD.DrawHudNumberSmall(x, y, DHN_3DIGITS, m_iAmount, 0,255,0);

	//does not show icon after amount > 3
/*
	if (m_iAmount == 2)
	{
		SPR_DrawAdditive(0, x, y - RectHeight(gHUD.GetSpriteRect(m_iSprite)), &gHUD.GetSpriteRect(m_iSprite));
	}
	if (m_iAmount == 3)
	{
		SPR_DrawAdditive(0, x, y - RectHeight(gHUD.GetSpriteRect(m_iSprite)), &gHUD.GetSpriteRect(m_iSprite));
		SPR_DrawAdditive(0, x, y - RectHeight(gHUD.GetSpriteRect(m_iSprite*2)), &gHUD.GetSpriteRect(m_iSprite));
	}
	if (m_iAmount == 4)
	{
		SPR_DrawAdditive(0, x, y - RectHeight(gHUD.GetSpriteRect(m_iSprite)), &gHUD.GetSpriteRect(m_iSprite));
		SPR_DrawAdditive(0, x, y - RectHeight(gHUD.GetSpriteRect(m_iSprite*2)), &gHUD.GetSpriteRect(m_iSprite));
		SPR_DrawAdditive(0, x, y + RectHeight(gHUD.GetSpriteRect(m_iSprite*3)), &gHUD.GetSpriteRect(m_iSprite));
	}
	if (m_iAmount == 5)
	{
		SPR_DrawAdditive(0, x, y - RectHeight(gHUD.GetSpriteRect(m_iSprite)), &gHUD.GetSpriteRect(m_iSprite));
		SPR_DrawAdditive(0, x, y - RectHeight(gHUD.GetSpriteRect(m_iSprite*2)), &gHUD.GetSpriteRect(m_iSprite));
		SPR_DrawAdditive(0, x, y + RectHeight(gHUD.GetSpriteRect(m_iSprite*3)), &gHUD.GetSpriteRect(m_iSprite));
		SPR_DrawAdditive(0, x, y + RectHeight(gHUD.GetSpriteRect(m_iSprite*4)), &gHUD.GetSpriteRect(m_iSprite));
	}
*/
    return 1;
}