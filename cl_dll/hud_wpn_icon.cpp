#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>
#include "weapondef.h"

DECLARE_MESSAGE(m_WpnIcon, WpnIcon)

int CHudWpnIcon::Init(void) 
{
	HOOK_MESSAGE(WpnIcon);

	m_iFlags |= HUD_ACTIVE;

	m_iAmmoFB = 0;
	m_iAmmoTrip = 0;
    gHUD.AddHudElem(this);
    return 1;
};

int CHudWpnIcon::VidInit(void)
{
	m_iSpriteFB = gHUD.GetSpriteIndex("wpn_flashbang");
	m_iSpriteTrip = gHUD.GetSpriteIndex("wpn_tripmine");
	m_iSpriteSpider = gHUD.GetSpriteIndex("wpn_spidermine");
	m_iSpriteMortar = gHUD.GetSpriteIndex("wpn_mortar");
	m_iSpriteSat = gHUD.GetSpriteIndex("wpn_satellite");
	m_iSpriteAtom = gHUD.GetSpriteIndex("wpn_atombomb");
	return 1;
};

int CHudWpnIcon::MsgFunc_WpnIcon(const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ(pbuf, iSize);
	m_iAmmoFB = READ_BYTE();
	m_iAmmoTrip = READ_BYTE();
	m_iAmmoSpider = READ_BYTE();
	m_iAmmoMortar = READ_BYTE();
	m_iAmmoSat = READ_BYTE();
	m_iAmmoAtom = READ_BYTE();
	END_READ();
	return 1;
}

int CHudWpnIcon::Draw(const float &flTime)
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) || gEngfuncs.IsSpectateOnly())
		return 0;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 0;

	//icons
	int Width = gHUD.GetSpriteRect(m_iSpriteFB).right - gHUD.GetSpriteRect(m_iSpriteFB).left;

	//1st icon
	SPR_Set(gHUD.GetSprite(m_iSpriteFB), 255, m_iAmmoFB?128:0,0);
	SPR_DrawAdditive(0, Width*2, 0, &gHUD.GetSpriteRect(m_iSpriteFB));
	gHUD.DrawHudNumberSmall(Width*2+XRES(2), YRES(1), DHN_DRAWZERO, m_iAmmoFB, m_iAmmoFB?0:255, m_iAmmoFB?255:0, 0);

	//2nd icon
	SPR_Set(gHUD.GetSprite(m_iSpriteTrip), 255,m_iAmmoTrip?128:0,0);
	SPR_DrawAdditive(0, Width*3, 0, &gHUD.GetSpriteRect(m_iSpriteTrip));
	gHUD.DrawHudNumberSmall(Width*3+XRES(2), YRES(1), DHN_DRAWZERO, m_iAmmoTrip, m_iAmmoTrip?0:255, m_iAmmoTrip?255:0, 0);

	//3rd icon
	SPR_Set(gHUD.GetSprite(m_iSpriteSpider), 255,m_iAmmoSpider?128:0,0);
	SPR_DrawAdditive(0, Width*4, 0, &gHUD.GetSpriteRect(m_iSpriteSpider));
	gHUD.DrawHudNumberSmall(Width*4+XRES(2), YRES(1), DHN_DRAWZERO, m_iAmmoSpider, m_iAmmoSpider?0:255, m_iAmmoSpider?255:0, 0);

	//4th icon
	SPR_Set(gHUD.GetSprite(m_iSpriteMortar), 255,m_iAmmoMortar?128:0,0);
	SPR_DrawAdditive(0, Width*5, 0, &gHUD.GetSpriteRect(m_iSpriteMortar));
	gHUD.DrawHudNumberSmall(Width*5+XRES(2), YRES(1), DHN_DRAWZERO, m_iAmmoMortar, m_iAmmoMortar?0:255, m_iAmmoMortar?255:0, 0);

	//5th icon
	SPR_Set(gHUD.GetSprite(m_iSpriteSat), 255,m_iAmmoSat?128:0,0);
	SPR_DrawAdditive(0, Width*6, 0, &gHUD.GetSpriteRect(m_iSpriteSat));
	gHUD.DrawHudNumberSmall(Width*6+XRES(2), YRES(1), DHN_DRAWZERO, m_iAmmoSat, m_iAmmoSat?0:255, m_iAmmoSat?255:0, 0);

	//6th icon
	SPR_Set(gHUD.GetSprite(m_iSpriteAtom), 255,(m_iAmmoAtom >= MAX_ENERGY_CUBE_CARRY)?128:0,0);
	SPR_DrawAdditive(0, Width*7, 0, &gHUD.GetSpriteRect(m_iSpriteAtom));
	gHUD.DrawHudNumberSmall(Width*7+XRES(2), YRES(1), DHN_DRAWZERO, m_iAmmoAtom, (m_iAmmoAtom >= MAX_ENERGY_CUBE_CARRY)?0:255, (m_iAmmoAtom >= MAX_ENERGY_CUBE_CARRY)?255:0, 0);

	return 1;
}