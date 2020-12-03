#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "weapondef.h"

int CHudBattery::Init(void)
{
	m_iBat = 0;
	m_fFade = 0;
	m_iFlags = 0;
	gHUD.AddHudElem(this);
	return 1;
}

int CHudBattery::VidInit(void)
{
	int HUD_suit_empty = gHUD.GetSpriteIndex("tg_none");
	int HUD_suit_full = gHUD.GetSpriteIndex("suit_full");
	m_hShieldSprite = 0;  // delaying get sprite handles until we know the sprites are loaded
	m_prc1 = &gHUD.GetSpriteRect(HUD_suit_empty);
	m_prc2 = &gHUD.GetSpriteRect(HUD_suit_full);
	m_fFade = 0;
	return 1;
}

int CHudBattery::MsgFunc_Battery(const char *pszName,  int iSize, void *pbuf)
{
	int x = READ_SHORT();

	if (x != m_iBat)
	{
		m_fFade = FADE_TIME;
		m_iBat = x;
	}
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudBattery::Draw(const float &flTime)
{
	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) || gEngfuncs.IsSpectateOnly())
		return 0;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 0;

	byte r,g,b,a;
	int x, y;

	if ( m_iBat > (MAX_NORMAL_BATTERY*2) + (EXTRA_BATTERY_SHIELD_AUG*MAX_SHIELD_AUG)) //if invulnerable
	{
		r = 255;
		g = 0;
		b = 128;
	}
	else
	{
		// convert to byte 0...255
		byte batbyte = (byte)(255.0f * max(0.0f, min(1.0f, (float)m_iBat/MAX_NORMAL_BATTERY)));
		r = 255 - batbyte;
		g = batbyte;
		b = 0;
	}
	// Has health changed? Flash the health #
	if (m_fFade)
	{
		if (m_fFade > FADE_TIME)
			m_fFade = FADE_TIME;

		m_fFade -= (float)(gHUD.m_flTimeDelta * 20);
		if (m_fFade <= 0)
		{
			a = MIN_ALPHA;
			m_fFade = 0;
		}

		// Fade the health number back to dim
		a = MIN_ALPHA + (int)((m_fFade/FADE_TIME)*128.0f);
	}
	else
		a = MIN_ALPHA;

	ScaleColors(r, g, b, a);
	
	int iOffset = (m_prc2->bottom - m_prc2->top)/6;
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
	x = ScreenWidth/6;

	// make sure we have the right sprite handles
	if (!m_hShieldSprite)
		m_hShieldSprite = gHUD.GetSprite(gHUD.GetSpriteIndex("suit_full"));

	if (m_iBat != SHIELD_IS_DEAD)
	{
		SPR_Set(m_hShieldSprite, r, g, b);
		SPR_DrawAdditive(0,  x, y - iOffset, m_prc2);

		if (m_iBat < 999)
			x += (m_prc2->right - m_prc2->left)/2;
		else
			x += (m_prc2->right - m_prc2->left);
		gHUD.DrawHudNumberLarge(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iBat, r, g, b);
	}
	else
	{
		SPR_Set(m_hShieldSprite, 200, 0, 0);
		SPR_DrawAdditive(0,  x, y - iOffset, m_prc1);
	}

	return 1;
}
