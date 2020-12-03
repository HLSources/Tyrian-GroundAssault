#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "weapondef.h"

DECLARE_MESSAGE(m_Flash, FlashBat)

int CHudFlashlight::Init(void)
{
	HOOK_MESSAGE(FlashBat);

	m_iFlags |= HUD_ACTIVE;

	gHUD.AddHudElem(this);
	return 1;
}

int CHudFlashlight::VidInit(void)
{
	int HUD_power_full = gHUD.GetSpriteIndex( "power_full" );
	m_hSprite2 = gHUD.GetSprite(HUD_power_full);
	m_prc2 = &gHUD.GetSpriteRect(HUD_power_full);
	m_iWidth = m_prc2->right - m_prc2->left;
	return 1;
};

int CHudFlashlight::MsgFunc_FlashBat(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	m_iBat = READ_SHORT();
	m_iTyrianGun = READ_BYTE();
	END_READ();

	if (m_iBat < POWER_MAX_CHARGE_GUIDED_BOMB)
		m_flBat = ((float)m_iBat)/(POWER_MAX_CHARGE);

	return 1;
}

int CHudFlashlight::Draw(const float &flTime)
{
	if (gHUD.m_iHideHUDDisplay & (HIDEHUD_ALL))
		return 1;

	int r, g, b, x, y, a;
	wrect_t rc;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 1;

	if (m_iBat <= POWER_MAX_CHARGE)
	{
		if (m_iTyrianGun != TYRIANGUN_NONE)	
		{
			a = 225;
			r = (int)(255.0f * (1.0f - m_flBat));
			g = (int)(255.0f * m_flBat);
			b = 0;

			ScaleColors(r, g, b, a);

			y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/1.5;
			x = ScreenWidth - m_iWidth - m_iWidth/5;

			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("power_empty")), r,g,b);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("power_empty")));

			// draw the energy level
			x = ScreenWidth - m_iWidth - m_iWidth/5.5;
			int iOffset = (int)((float)m_iWidth * (1.0f - m_flBat));
			if (iOffset < m_iWidth)
			{
				rc = *m_prc2;
				rc.left += iOffset;
				SPR_Set(m_hSprite2, 150, 150, 150);
				SPR_DrawAdditive(0, x + iOffset, y, &rc);
			}
			y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/2;
			x = ScreenWidth - m_iWidth*1.8;
			x = gHUD.DrawHudNumberLarge(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iBat, r, g, b);
		}
	}
	else 
	{
		if (m_iTyrianGun != TYRIANGUN_NONE)	
		{
			a = 255;
			r = 0;
			g = (int)(255.0f * (1.0f - m_flBat));
			b = (int)(255.0f * m_flBat);

			ScaleColors(r, g, b, a);

			y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/1.5;
			x = ScreenWidth - m_iWidth - m_iWidth/5;

			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("power_empty")), r,g,b);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("power_empty")));

			y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/1.9;

			// draw super weapon icons
			if (m_iBat >= POWER_MAX_CHARGE_SUN_OF_GOD)
			{
				SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("sw_sunofgod")), 0,250,0);
				SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("sw_sunofgod")));
			}
			else if (m_iBat >= POWER_MAX_CHARGE_CHRONOSCEPTRE)
			{
				SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("sw_chronosceptre")), 0,250,0);
				SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("sw_chronosceptre")));
			}
			else if (m_iBat >= POWER_MAX_CHARGE_NUCLEAR_MISSILE)
			{
				SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("sw_nuclearmissile")), 0,250,0);
				SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("sw_nuclearmissile")));
			}
			else if (m_iBat >= POWER_MAX_CHARGE_SHOCK_WAVE)
			{
				SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("sw_lightningball")), 0,250,0);
				SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("sw_lightningball")));
			}
			else if (m_iBat >= POWER_MAX_CHARGE_DISPLACER)
			{
				SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("sw_displacer")), 0,250,0);
				SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("sw_displacer")));
			}
			else if (m_iBat >= POWER_MAX_CHARGE_PROTON_MISSILE)
			{
				SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("sw_protonmissile")), 0,250,0);
				SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("sw_protonmissile")));
			}
			else if (m_iBat >= POWER_MAX_CHARGE_GUIDED_BOMB)
			{
				SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("sw_guidedbomb")), 0,250,0);
				SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("sw_guidedbomb")));
			}
			else if (m_iBat < POWER_MAX_CHARGE_GUIDED_BOMB)
			{
				SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("power_owercharge")), 0,200,0);
				SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("power_owercharge")));
			}

			y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/2;
			x = ScreenWidth - m_iWidth*1.8;
			x = gHUD.DrawHudNumberLarge(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iBat, r, g, b);
		}
	}

	if (m_iTyrianGun == TYRIANGUN_NONE)
	{
		y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/1.5;
		x = ScreenWidth - m_iWidth - m_iWidth/6;
		SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_none")), 255,0,0);
		SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_none")));
		return 1;
	}

	//draw front gun icon
	x = ScreenWidth - m_iWidth - m_iWidth/6;
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight*1.7;

	switch (m_iTyrianGun)
	{
		case TYRIANGUN_MACHINEGUN:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_machinegun")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_machinegun")));
		}
		break;

		case TYRIANGUN_MINIGUN:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_minigun")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_minigun")));
		}
		break;

		case TYRIANGUN_VULCAN_MINIGUN:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_vulcan")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_vulcan")));
		}
		break;

		case TYRIANGUN_HEAVY_TURRET:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_heavyturret")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_heavyturret")));
		}
		break;

		case TYRIANGUN_GAUSS:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_gauss")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_gauss")));
		}
		break;

		case TYRIANGUN_LASER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_laser")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_laser")));
		}
		break;

		case TYRIANGUN_RAILGUN:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_railgun")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_railgun")));
		}
		break;

		case TYRIANGUN_SUPER_RAILGUN:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_heavyrailgun")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_heavyrailgun")));
		}
		break;

		case TYRIANGUN_SHOCK_CANNON:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_shockcannon")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_shockcannon")));
		}
		break;

		case TYRIANGUN_LIGHTNING:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_lightning")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_lightning")));
		}
		break;

		case TYRIANGUN_TWIN_LASER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_twinlaser")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_twinlaser")));
		}
		break;

		case TYRIANGUN_TAU_CANNON:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_taucannon")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_taucannon")));
		}
		break;

		case TYRIANGUN_BEAMSPLITTER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_beamsplitter")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_beamsplitter")));
		}
		break;

		case TYRIANGUN_MEZON_CANNON:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_mezoncannon")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_mezoncannon")));
		}
		break;

		case TYRIANGUN_REPEATER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_repeater")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_repeater")));
		}
		break;

		case TYRIANGUN_PHOTONGUN:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_photongun")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_photongun")));
		}
		break;

		case TYRIANGUN_BFG:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_bfg")), 255,160,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_bfg")));
		}
		break;

		case TYRIANGUN_M203:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_m203")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_m203")));
		}
		break;

		case TYRIANGUN_NEEDLE_LASER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_needle_laser")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_needle_laser")));
		}
		break;

		case TYRIANGUN_SHOCK_LASER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_shock_laser")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_shock_laser")));
		}
		break;

		case TYRIANGUN_HELL_HOUNDER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_hellhounder")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_hellhounder")));
		}
		break;

		case TYRIANGUN_30MMGRENADE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_30mmgrenade")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_30mmgrenade")));
		}
		break;

		case TYRIANGUN_MINI_MISSILE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_mini_missile")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_mini_missile")));
		}
		break;

		case TYRIANGUN_BIOHAZARD_MISSILE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_biohazard_missile")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_biohazard_missile")));
		}
		break;

		case TYRIANGUN_GLUON:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_gluon")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_gluon")));
		}
		break;

		case TYRIANGUN_TRIDENT:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_trident")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_trident")));
		}
		break;

		case TYRIANGUN_HELLFIRE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_hellfire")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_hellfire")));
		}
		break;

		case TYRIANGUN_PLASMA:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_plasma")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_plasma")));
		}
		break;

		case TYRIANGUN_PLASMA_STORM:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_plasma_storm")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_plasma_storm")));
		}
		break;

		case TYRIANGUN_PULSE_CANNON:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_pulse_cannon")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_pulse_cannon")));
		}
		break;

		case TYRIANGUN_MULTI_CANNON:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_multi_cannon")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_multi_cannon")));
		}
		break;

		case TYRIANGUN_SCORCHER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_scorcher")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_scorcher")));
		}
		break;

		case TYRIANGUN_FROSTER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_froster")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_froster")));
		}
		break;

		case TYRIANGUN_SONIC_WAVE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_sonic_wave")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_sonic_wave")));
		}
		break;

		case TYRIANGUN_CHARGE_CANNON:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_charge_cannon")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_charge_cannon")));
		}
		break;

		case TYRIANGUN_DEMOLITION_MISSILE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_demolition_missile")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_demolition_missile")));
		}
		break;

		case TYRIANGUN_GHOST_MISSILE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_ghost_missile")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_ghost_missile")));
		}
		break;

		case TYRIANGUN_MICRO_MISSILE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_micro_missile")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_micro_missile")));
		}
		break;

		case TYRIANGUN_HVR_MISSILE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_hvr_missile")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_hvr_missile")));
		}
		break;

		case TYRIANGUN_RIPPER:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_ripper")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_ripper")));
		}
		break;

		case TYRIANGUN_DISRUPTOR:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_disruptor")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_disruptor")));
		}
		break;

		case TYRIANGUN_CLUSTER_GUN:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_cluster_gun")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_cluster_gun")));
		}
		break;

		case TYRIANGUN_NEYTRON_GUN:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_neytrongun")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_neytrongun")));
		}
		break;

		case TYRIANGUN_FLAK_CANNON:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_flak_cannon")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_flak_cannon")));
		}
		break;

		case TYRIANGUN_STARBURST:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_star_burst")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_star_burst")));
		}
		break;

		case TYRIANGUN_BIORIFLE:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_biorifle")), 255,240,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_biorifle")));
		}
		break;

		case TYRIANGUN_BANANA:
		{
			SPR_Set(gHUD.GetSprite(gHUD.GetSpriteIndex("tg_banana")), 255,125,0);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(gHUD.GetSpriteIndex("tg_banana")));
		}
		break;
	}
	return 1;
}
