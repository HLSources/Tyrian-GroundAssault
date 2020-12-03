#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>
#include "weapondef.h"
#include "pm_defs.h"
#include "pm_shared.h"

int CHudItemWeaponPower::Init(void) 
{
	m_iAmount = 0;
	m_iFlags |= HUD_ACTIVE;
    gHUD.AddHudElem(this);
    return 1;
};

int CHudItemWeaponPower::VidInit(void)
{
	m_iSprite = gHUD.GetSpriteIndex("wpn_power");
	return 1;
};

int CHudItemWeaponPower::MsgFunc_ItemWeaponPower(const char *pszName,  int iSize, void *pbuf )
{
	m_iAmount = READ_SHORT();

	if (m_iAmount >= WEAPON_IS_DEAD)
		m_iFlags &= ~HUD_ACTIVE;
	else
		m_iFlags |= HUD_ACTIVE;

	return 1;
}

int CHudItemWeaponPower::Draw(const float &flTime)
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) || gEngfuncs.IsSpectateOnly())
		return 0;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 0;

	if (m_iAmount == WEAPON_IS_DEAD)
		return 0;

	int r,g,b, x,y;
	int Width = gHUD.GetSpriteRect(m_iSprite).right - gHUD.GetSpriteRect(m_iSprite).left;

	r = 255; g = 128; b = 0;

	x = ScreenHeight - gHUD.m_iFontHeight/3.5;
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight*1.5;

	SPR_Set(gHUD.GetSprite(m_iSprite), r,g,b);
	SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_iSprite));

	gHUD.DrawHudNumberSmall(x+Width/2.8, y, DHN_3DIGITS | DHN_DRAWZERO, 100+(m_iAmount*10), r,g,b);
	return 1;
}