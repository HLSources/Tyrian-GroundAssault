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
// Ammo.cpp
//
// implementation of CHudAmmo class
//

#include "hud.h"
#include <ctype.h>
#include "cl_util.h"
#include "parsemsg.h"
#include "pm_shared.h"
#include "ammohistory.h"
#include "vgui_XDMViewport.h"
#include "weapondef.h"

wrect_t nullrc = {0,0,0,0};

// TODO: replace (HUD_WEAPON *)1
/*
HUD_WEAPON wNoSelection =
{
	"",
	-1,
	-1,
	-1,
	-1,
	0,
	0,
	0,
	0,
	-1,
	0,
	0, {0,0,0,0},
	0, {0,0,0,0},
	0, {0,0,0,0},
	0, {0,0,0,0},
	0, {0,0,0,0},
	0, {0,0,0,0},
	0, {0,0,0,0},
	0, {0,0,0,0}
};
*/
HUD_WEAPON *gpActiveSel;	// NULL means off, 1 means just the menu bar, otherwise this points to the active weapon menu item
HUD_WEAPON *gpLastSel;		// Last weapon menu selection 

WeaponsResource gWR;

int g_weaponselect = 0;

void WeaponsResource::LoadAllWeaponSprites(void)
{
	for (int i = 0; i < MAX_WEAPONS; ++i)
	{
		if (rgWeapons[i].iId)
			LoadWeaponSprites(&rgWeapons[i]);
	}
}

int WeaponsResource::CountAmmo(const int &iId) 
{ 
	if (iId < 0)
		return 0;

	return riAmmo[iId];
}

int WeaponsResource::HasAmmo(HUD_WEAPON *p)
{
	if (p == NULL)
		return FALSE;

	// weapons with no max ammo can always be selected
	if (p->iMax1 == -1)
		return TRUE;

	return (p->iAmmoType == -1) || p->iClip > 0 || CountAmmo(p->iAmmoType) || CountAmmo(p->iAmmo2Type) || ( p->iFlags & WEAPON_FLAGS_SELECTONEMPTY );
}

void WeaponsResource::LoadWeaponSprites(HUD_WEAPON *pWeapon)
{
	if (pWeapon == NULL)
		return;

	int i, iRes;

	if (ScreenWidth < 640)
		iRes = 320;
	else
		iRes = 640;

	char sz[64];
	memset(&pWeapon->rcActive, 0, sizeof(wrect_t));
	memset(&pWeapon->rcInactive, 0, sizeof(wrect_t));
	memset(&pWeapon->rcAmmo, 0, sizeof(wrect_t));
	memset(&pWeapon->rcAmmo2, 0, sizeof(wrect_t));
	pWeapon->hInactive = 0;
	pWeapon->hActive = 0;
	pWeapon->hAmmo = 0;
	pWeapon->hAmmo2 = 0;

	sprintf(sz, "sprites/%s.txt", pWeapon->szName);
	client_sprite_t *pList = SPR_GetList(sz, &i);

	if (pList == NULL)// XDM
	{
		CON_DPRINTF("WeaponsResource: loading default sprite set for %s %d\n", pWeapon->szName, pWeapon->iId);
		pList = SPR_GetList("sprites/weapon_unknown.txt", &i);
	}

	if (pList == NULL)
		return;

	client_sprite_t *p;
	p = GetSpriteList(pList, "crosshair", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hCrosshair = SPR_Load(sz);
		pWeapon->rcCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = NULL;

	p = GetSpriteList(pList, "autoaim", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAutoaim = SPR_Load(sz);
		pWeapon->rcAutoaim = p->rc;
	}
	else
		pWeapon->hAutoaim = 0;

	p = GetSpriteList(pList, "zoom", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedCrosshair = SPR_Load(sz);
		pWeapon->rcZoomedCrosshair = p->rc;
	}
	else
	{
		pWeapon->hZoomedCrosshair = pWeapon->hCrosshair; //default to non-zoomed crosshair
		pWeapon->rcZoomedCrosshair = pWeapon->rcCrosshair;
	}

	p = GetSpriteList(pList, "zoom_autoaim", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hZoomedAutoaim = SPR_Load(sz);
		pWeapon->rcZoomedAutoaim = p->rc;
	}
	else
	{
		pWeapon->hZoomedAutoaim = pWeapon->hZoomedCrosshair;  //default to zoomed crosshair
		pWeapon->rcZoomedAutoaim = pWeapon->rcZoomedCrosshair;
	}

	p = GetSpriteList(pList, "weapon", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hInactive = SPR_Load(sz);
		pWeapon->rcInactive = p->rc;
		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hInactive = 0;

	p = GetSpriteList(pList, "weapon_s", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hActive = SPR_Load(sz);
		pWeapon->rcActive = p->rc;
	}
	else
		pWeapon->hActive = 0;

	p = GetSpriteList(pList, "ammo", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo = SPR_Load(sz);
		pWeapon->rcAmmo = p->rc;
		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo = 0;

	p = GetSpriteList(pList, "ammo2", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spr", p->szSprite);
		pWeapon->hAmmo2 = SPR_Load(sz);
		pWeapon->rcAmmo2 = p->rc;
		gHR.iHistoryGap = max(gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top);
	}
	else
		pWeapon->hAmmo2 = 0;
}

// Returns the first weapon for a given slot.
HUD_WEAPON *WeaponsResource::GetFirstPos(const int &iSlot)
{
	HUD_WEAPON *pret = NULL;
	for (int i = 0; i < MAX_WEAPON_POSITIONS; ++i)
	{
		if (rgSlots[iSlot][i] && HasAmmo(rgSlots[iSlot][i]))
		{
			pret = rgSlots[iSlot][i];
			break;
		}
	}
	return pret;
}

HUD_WEAPON *WeaponsResource::GetNextActivePos(const int &iSlot, const int &iSlotPos)
{
	if ( iSlotPos >= MAX_WEAPON_POSITIONS || iSlot >= MAX_WEAPON_SLOTS )
		return NULL;

	HUD_WEAPON *p = gWR.rgSlots[iSlot][iSlotPos+1];
	
	if (p == NULL || !gWR.HasAmmo(p))
		return GetNextActivePos(iSlot, iSlotPos+1);

	return p;
}

// Helper function to return a Ammo pointer from id
HSPRITE *WeaponsResource::GetAmmoPicFromWeapon(const int &iAmmoId, wrect_t &rect)
{
	for (int i = 1; i < MAX_WEAPONS; ++i)// XDM: start from 1 to ignore WEAPON_NONE
	{
//		CON_PRINTF("rgWeapons[%d].iAmmoType = %d\n", i, rgWeapons[i].iAmmoType);
		if (rgWeapons[i].iAmmoType == iAmmoId)
		{
//			CON_PRINTF("GetAmmoPicFromWeapon found rgWeapons[%d].iAmmoType = %d\n", i, rgWeapons[i].iAmmoType);
			rect = rgWeapons[i].rcAmmo;
			return &rgWeapons[i].hAmmo;
		}
		else if (rgWeapons[i].iAmmo2Type == iAmmoId)
		{
			rect = rgWeapons[i].rcAmmo2;
			return &rgWeapons[i].hAmmo2;
		}
	}
//	CON_PRINTF("GetAmmoPicFromWeapon failed!\n");
	return NULL;
}

// Menu Selection Code
void WeaponsResource::SelectSlot(const int &iSlot, const int &fAdvance, const int &iDirection)
{
//	if (iSlot > MAX_WEAPON_SLOTS)
		return;

	if (gHUD.m_fPlayerDead || gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_ALL))
		return;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return;

	if (!(gHUD.m_iWeaponBits & ~(1<<(WEAPON_SUIT))))
		return;

	HUD_WEAPON *p = NULL;
	bool fastSwitch = 0;

	if ( (gpActiveSel == NULL) || (gpActiveSel == (HUD_WEAPON *)1) || (iSlot != gpActiveSel->iSlot) )
	{
//		PlaySound("common/wpn_hudon.wav", VOL_NORM);
		p = GetFirstPos(iSlot);
		if (p && fastSwitch) // check for fast weapon switch mode
		{
			// if fast weapon switch is on, then weapons can be selected in a single keypress
			// but only if there is only one item in the bucket
			HUD_WEAPON *p2 = GetNextActivePos(p->iSlot, p->iSlotPos);
			if (!p2)
			{	// only one active item in bucket, so change directly to weapon
				SERVER_COMMAND(p->szName);
				g_weaponselect = p->iId;
				return;
			}
		}
		PlaySound("common/wpn_hudon.wav", VOL_NORM);
	}
	else
	{
		PlaySound("common/wpn_moveselect.wav", VOL_NORM);
		if (gpActiveSel)
			p = GetNextActivePos(gpActiveSel->iSlot, gpActiveSel->iSlotPos);
		if (!p)
			p = GetFirstPos(iSlot);
	}

	if (!p)  // no selection found
	{
		// just display the weapon list, unless fastswitch is on just ignore it
		if (!fastSwitch)
			gpActiveSel = (HUD_WEAPON *)1;
		else
			gpActiveSel = NULL;
	}
	else 
		gpActiveSel = p;
}


int giBucketHeight, giBucketWidth, giABHeight, giABWidth; // Ammo Bar width and height

HSPRITE ghsprBuckets;					// Sprite for top row of weapons menu

DECLARE_MESSAGE(m_Ammo, CurWeapon);		// Current weapon and clip
DECLARE_MESSAGE(m_Ammo, WeaponList);	// new weapon type
DECLARE_MESSAGE(m_Ammo, AmmoX);			// update known ammo type's count
DECLARE_MESSAGE(m_Ammo, AmmoPickup);	// flashes an ammo pickup record
DECLARE_MESSAGE(m_Ammo, WeapPickup);    // flashes a weapon pickup record
DECLARE_MESSAGE(m_Ammo, HideWeapon);	// hides the weapon, ammo, and crosshair displays temporarily
DECLARE_MESSAGE(m_Ammo, ItemPickup);
DECLARE_MESSAGE(m_Ammo, UpdWeapons);	// XDM: All weapons and clip
DECLARE_MESSAGE(m_Ammo, UpdAmmo);		// XDM: All ammo

/*
DECLARE_COMMAND(m_Ammo, Slot1);
DECLARE_COMMAND(m_Ammo, Slot2);
DECLARE_COMMAND(m_Ammo, Slot3);
DECLARE_COMMAND(m_Ammo, Slot4);
DECLARE_COMMAND(m_Ammo, Slot5);
DECLARE_COMMAND(m_Ammo, Slot6);
DECLARE_COMMAND(m_Ammo, Slot7);
DECLARE_COMMAND(m_Ammo, Slot8);
DECLARE_COMMAND(m_Ammo, Close);
DECLARE_COMMAND(m_Ammo, NextWeapon);
DECLARE_COMMAND(m_Ammo, PrevWeapon);
DECLARE_COMMAND(m_Ammo, InvUp);
DECLARE_COMMAND(m_Ammo, InvDown);
*/

//-----------------------------------------------------------------------------
// Purpose: 
// Output : int
//-----------------------------------------------------------------------------
int CHudAmmo::Init(void)
{
	gHUD.AddHudElem(this);

	HOOK_MESSAGE(CurWeapon);
	HOOK_MESSAGE(WeaponList);
	HOOK_MESSAGE(AmmoPickup);
	HOOK_MESSAGE(WeapPickup);
	HOOK_MESSAGE(ItemPickup);
	HOOK_MESSAGE(HideWeapon);
	HOOK_MESSAGE(AmmoX);
	HOOK_MESSAGE(UpdWeapons);// XDM3035
	HOOK_MESSAGE(UpdAmmo);

	Reset();
	m_iFlags |= HUD_ACTIVE; //!!!

	gWR.Init();
	gHR.Init();
	return 1;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAmmo::Reset(void)
{
	m_fFade = 0;
	m_iFlags |= HUD_ACTIVE; //!!!

	gpActiveSel = NULL;
	gHUD.m_iHideHUDDisplay = 0;

	m_iRecentPickedWeapon = 0;// XDM3035a

	gWR.Reset();
	gHR.Reset();

}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : int
//-----------------------------------------------------------------------------
int CHudAmmo::VidInit(void)
{
	// Load sprites for buckets (top row of weapon menu)
	m_HUD_bucket0 = gHUD.GetSpriteIndex("bucket1");
	m_HUD_selection = gHUD.GetSpriteIndex("selection");

	ghsprBuckets = gHUD.GetSprite(m_HUD_bucket0);
	giBucketWidth = gHUD.GetSpriteRect(m_HUD_bucket0).right - gHUD.GetSpriteRect(m_HUD_bucket0).left;
	giBucketHeight = gHUD.GetSpriteRect(m_HUD_bucket0).bottom - gHUD.GetSpriteRect(m_HUD_bucket0).top;

	gHR.iHistoryGap = max( gHR.iHistoryGap, gHUD.GetSpriteRect(m_HUD_bucket0).bottom - gHUD.GetSpriteRect(m_HUD_bucket0).top);

	int HUD_gen_destr = gHUD.GetSpriteIndex("tg_none");
	m_prc1 = &gHUD.GetSpriteRect(HUD_gen_destr);

	// If we've already loaded weapons, let's get new sprites
	gWR.LoadAllWeaponSprites();

	if (ScreenWidth >= 640)
	{
		giABWidth = 20;
		giABHeight = 4;
	}
	else
	{
		giABWidth = 10;
		giABHeight = 2;
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: ed for selection of weapon menu item.
//-----------------------------------------------------------------------------
void CHudAmmo::Think(void)
{
	if (gHUD.m_fPlayerDead)
		return;

	if ( gHUD.m_iWeaponBits != gWR.iOldWeaponBits )
	{
		gWR.iOldWeaponBits = gHUD.m_iWeaponBits;

		for (int i = MAX_WEAPONS-1; i > 0; --i)
		{
			HUD_WEAPON *p = gWR.GetWeapon(i);
			if (p)
			{
				if (gHUD.m_iWeaponBits & (1 << p->iId))
					gWR.PickupWeapon(p);
				else
					gWR.DropWeapon(p);
			}
		}
	}

	if (!gpActiveSel)
		return;

	// has the player selected one?
	if (gHUD.m_iKeyBits & IN_ATTACK)
	{
		if (gpActiveSel != (HUD_WEAPON *)1)
		{
//			CON_PRINTF("ServerCmd %s\n", gpActiveSel->szName);
//			SERVER_COMMAND(gpActiveSel->szName);
			char cmd[8];// XDM3035
			sprintf(cmd, "_sw %d", gpActiveSel->iId);
			SERVER_COMMAND(cmd);
			g_weaponselect = gpActiveSel->iId;
		}

		gpLastSel = gpActiveSel;
		gpActiveSel = NULL;
		gHUD.m_iKeyBits &= ~IN_ATTACK;

		PlaySound("common/wpn_moveselect.wav", VOL_NORM);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// XDM: mode 0 - clear, 1 - normal, 2 - autoaim
// Input  : &mode - 
//			&zoomed - 
//-----------------------------------------------------------------------------
void CHudAmmo::UpdateCrosshair(const int &mode, const int &zoomed)
{
	if (mode == 0)
	{
		SetCrosshair(0, nullrc, 0, 0, 0);
	}
	else if (m_pWeapon)// XDM3035b: fix
	{
		if (zoomed == 0)
		{ // normal crosshairs
			if (mode == 2 && m_pWeapon->hAutoaim)
				SetCrosshair(m_pWeapon->hAutoaim, m_pWeapon->rcAutoaim, 255, 255, 255);
			else
				SetCrosshair(m_pWeapon->hCrosshair, m_pWeapon->rcCrosshair, 255, 255, 255);
		}
		else
		{ // zoomed crosshairs
			if (mode == 2 && m_pWeapon->hZoomedAutoaim)
				SetCrosshair(m_pWeapon->hZoomedAutoaim, m_pWeapon->rcZoomedAutoaim, 255, 255, 255);
			else
				SetCrosshair(m_pWeapon->hZoomedCrosshair, m_pWeapon->rcZoomedCrosshair, 255, 255, 255);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: XDM3035b: InitWeaponSlots
// Output : int
//-----------------------------------------------------------------------------
int CHudAmmo::InitWeaponSlots(void)
{
	sprintf(m_szWeaponSlotConfig, "%d",	WEAPON_TYRIANGUN );
	return 1;
}

//-----------------------------------------------------------------------------
// "1,19,16; 2,3,5; 4,7,6; 8,17,18; 9,10,22,23; 12,14,13,15,21,20; 24,25,26,11;"
// Purpose: Parse weapon slot config string and find slot and position for ID
// Input  : &iId - 
//			&wslot - 
//			&wpos - 
// Output : int 1 = success
//-----------------------------------------------------------------------------
int CHudAmmo::GetWeaponSlotPos(const int &iId, int &wslot, int &wpos)
{
//	char *string = m_pCvarWeaponSlots->string;
	int len = strlen(m_szWeaponSlotConfig);
	char *c = m_szWeaponSlotConfig;

	int slot = 0;
	int slotpos = 0;

	// 1,2,3;4,5,6;7,8,9;
/*	char *slotstring = string;
	char *slotposstring = NULL;
	while ((slotstring = strtok(slotstring, ";")) != NULL)// NO SPACES!
	{
		slotpos = 0;
		CON_PRINTF(" # %d: slotstring: '%s'\n", slot, slotstring);
		slotposstring = strtok(slotstring, " ,");
		while (slotposstring)
		{
			CON_PRINTF("## %d: slotposstring: '%s'\n", slotpos, slotposstring);
			++slotpos;
			slotposstring = strtok(NULL, " ,");
		}
		CON_PRINTF("END slot %d\n", slot);
		++slot;
	}*/

//	CON_DPRINTF("CHudAmmo::GetWeaponSlotPos(%d)\n", iId);
//	int tokenlen;
	int current_id = 0;
	char *tokenstart = NULL;
	char swapchar;
	for (int i=0; i<len; ++i)
	{
		if (isdigit(*c))
		{
			if (tokenstart == NULL)
			{
				tokenstart = c;
//				tokenlen = 0;
			}
		}
		else if (*c == ';' || *c == ',')// found separator, analyze previously found string
		{
			swapchar = *c;
			*c = NULL;// set as end of string
			current_id = atoi(tokenstart);
			*c = swapchar;// restore original!
//			CON_DPRINTF("#%d # %d: current_id: %d\n", slot, slotpos, current_id);

			if (current_id == iId)// found, write output
			{
				wslot = slot;
				wpos = slotpos;
				return 1;
			}

			if (*c == ';')// Next slot
			{
				++slot;
				slotpos = 0;
			}
			else// Next position
				++slotpos;

			tokenstart = NULL;
		}
		c++;
	}
	CON_PRINTF("CHudAmmo::GetWeaponSlotPos(%d) failed!\n", iId);
	return 0;
}

//-------------------------------------------------------------------------
// Drawing code
//-------------------------------------------------------------------------
int CHudAmmo::Draw(const float &flTime)
{
	if (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL))
		return 1;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))))
		return 1;

	// Draw Weapon Menu
	DrawWList(flTime);

	// Draw ammo pickup history
	gHR.DrawAmmoHistory(flTime);

	if (!(m_iFlags & HUD_ACTIVE))
		return 0;

	if (!m_pWeapon)
		return 0;

	HUD_WEAPON *pw = m_pWeapon; // shorthand

	// SPR_Draw Ammo
	if ((pw->iAmmoType < 0) && (pw->iAmmo2Type < 0))
		return 0;

	int a, x, y, r, g, b;
	int iFlags = DHN_DRAWZERO; // draw 0 values
	int AmmoWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;

	a = (int) max( MIN_ALPHA, m_fFade );

	if (m_fFade > 0)
		m_fFade -= (float)(gHUD.m_flTimeDelta * 20);

	UnpackRGB(r,g,b, RGB_GREEN);
	ScaleColors(r, g, b, a);

	// Does this weapon have a clip?
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight/2;

	// Does weapon have any ammo at all?
	if (m_pWeapon->iAmmoType >= 0)
	{
		int iIconWidth = m_pWeapon->rcAmmo.right - m_pWeapon->rcAmmo.left;
		if (pw->iClip >= 0)// XDM =
		{
			// room for the number and the '|' and the current ammo
			x = ScreenWidth - (8 * AmmoWidth) - iIconWidth;
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, pw->iClip, r, g, b);

			wrect_t rc;
			rc.top = 0;
			rc.left = 0;
			rc.right = AmmoWidth;
			rc.bottom = 100;

			int iBarWidth = AmmoWidth/10;

			x += AmmoWidth/2;

			UnpackRGB(r,g,b, RGB_GREEN);

			// draw the | bar
			FillRGBA(x, y, iBarWidth, gHUD.m_iFontHeight, r, g, b, a);
			x += iBarWidth + AmmoWidth/2;

			// GL Seems to need this
			ScaleColors(r, g, b, a);
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmoType), r, g, b);		
		}
		else
		{
			// SPR_Draw a bullets only line
			x = ScreenWidth - 18 * AmmoWidth - iIconWidth;
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmoType), r, g, b);
		}

		// Draw the ammo Icon
		int iOffset = (m_pWeapon->rcAmmo.bottom - m_pWeapon->rcAmmo.top)/8;
		SPR_Set(m_pWeapon->hAmmo, r, g, b);
		SPR_DrawAdditive(0, x, y - iOffset, &m_pWeapon->rcAmmo);
	}

	int iIconWidth = m_pWeapon->rcAmmo2.right - m_pWeapon->rcAmmo2.left;
	x = ScreenWidth - 23 * AmmoWidth - iIconWidth;

	// Draw the ammo Icon
	if (gWR.CountAmmo(pw->iAmmo2Type) != GENERATOR_IS_DEAD)
	{
		x = gHUD.DrawHudNumber(x, y, iFlags|DHN_3DIGITS, gWR.CountAmmo(pw->iAmmo2Type), r, g, b);

		SPR_Set(m_pWeapon->hAmmo2, r, g, b);
		int iOffset = (m_pWeapon->rcAmmo2.bottom - m_pWeapon->rcAmmo2.top)/8;
		SPR_DrawAdditive(0, x, y - iOffset, &m_pWeapon->rcAmmo2);
	}
	else
	{
		SPR_Set(m_pWeapon->hAmmo2, 200, 0, 0);
		int iOffset = (m_pWeapon->rcAmmo2.bottom - m_pWeapon->rcAmmo2.top)/8;
		SPR_DrawAdditive(0,  x-XRES(12), y - iOffset, m_prc1);
	}

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Draws the ammo bar on the hud
// Input  : x y - 
//			width height - 
//			f - 
// Output : int
//-----------------------------------------------------------------------------
int DrawBar(int x, int y, int width, int height, float f)
{
	byte r, g, b;// XDM3035

	if (f < 0)
		f = 0;
	if (f > 1)
		f = 1;

	if (f)
	{
		int w = (int)(f * (float)width);

		// Always show at least one pixel if we have ammo.
		if (w <= 0)
			w = 1;
		UnpackRGB(r, g, b, RGB_BLUE);
		FillRGBA(x, y, w, height, r, g, b, 255);
		x += w;
		width -= w;
	}

	UnpackRGB(r, g, b, RGB_YELLOW);
	FillRGBA(x, y, width, height, r, g, b, 127);
	return (x + width);
}

//-----------------------------------------------------------------------------
// Purpose: DrawAmmoBar
// Input  : *p - 
//			x y - 
//			width height - 
//-----------------------------------------------------------------------------
void DrawAmmoBar(HUD_WEAPON *p, int x, int y, int width, int height)
{
	if (p == NULL)
		return;

	float f = 0.0;
	if (p->iAmmoType != -1)
	{
//		if (!gWR.CountAmmo(p->iAmmoType))
//			return;

		f = (float)gWR.CountAmmo(p->iAmmoType)/(float)p->iMax1;
		x = DrawBar(x, y, width, height, f);
	}
	// Do we have secondary ammo too?
	if (p->iAmmo2Type != -1)
	{
		f = (float)gWR.CountAmmo(p->iAmmo2Type)/(float)p->iMax2;
		x += 4; // don't forget about some offset !!!
		DrawBar(x, y, width, height, f);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draw Weapon Menu
// Input  : &flTime - 
// Output : int
//-----------------------------------------------------------------------------
int CHudAmmo::DrawWList(const float &flTime)
{
	if (!gpActiveSel)
		return 0;

	byte r,g,b,a;
	int x,y,i;
	int iActiveSlot;

	if (gpActiveSel == (HUD_WEAPON *)1)
		iActiveSlot = -1;	// current slot has no weapons
	else 
		iActiveSlot = gpActiveSel->iSlot;

	x = 10; //!!!
	y = 10; //!!!

	// Ensure that there are available choices in the active slot
	if (iActiveSlot > 0)
	{
		if (!gWR.GetFirstPos(iActiveSlot))
		{
			gpActiveSel = (HUD_WEAPON *)1;
			iActiveSlot = -1;
		}
	}

	// Draw top line
	for (i = 0; i < MAX_WEAPON_SLOTS; ++i)
	{
		UnpackRGB(r,g,b, RGB_GREEN);

		if (iActiveSlot == i)
			a = 255;
		else
			a = 192;

		ScaleColors(r, g, b, a);// XDM3035c: fix?
		SPR_Set(gHUD.GetSprite(m_HUD_bucket0), r, g, b);

		int iWidth;
		// make active slot wide enough to accomodate gun pictures
		if (i == iActiveSlot)
		{
			HUD_WEAPON *p = gWR.GetFirstPos(iActiveSlot);
			if (p)
				iWidth = p->rcActive.right - p->rcActive.left;
			else
				iWidth = giBucketWidth;
		}
		else
			iWidth = giBucketWidth;

		SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_bucket0));
		x += iWidth + 5;
	}

	a = 128; //!!!
	x = 10;

	// Draw all of the buckets
	for (i = 0; i < MAX_WEAPON_SLOTS; ++i)
	{
		y = giBucketHeight + 10;
		// If this is the active slot, draw the bigger pictures,
		// otherwise just draw boxes
		if (i == iActiveSlot)
		{
			HUD_WEAPON *p = gWR.GetFirstPos(i);
			int iWidth = giBucketWidth;
			if (p)
				iWidth = p->rcActive.right - p->rcActive.left;

			for (int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++)
			{
				p = gWR.GetWeaponSlot( i, iPos );

				if ( !p || !p->iId )
					continue;

				// if active, then we must have ammo.
				if ( gpActiveSel == p )
				{
					UnpackRGB( r,g,b, RGB_GREEN );
					SPR_Set(p->hActive, r, g, b );
					SPR_DrawAdditive(0, x, y, &p->rcActive);
					SPR_Set(gHUD.GetSprite(m_HUD_selection), r, g, b );
					SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_selection));
				}
				else
				{
					// XDM: Draw Weapon in YELLOW if no ammo
					if (gWR.HasAmmo(p))
					{
						UnpackRGB(r,g,b, RGB_GREEN);
						ScaleColors(r, g, b, (byte)192);
					}
					else
					{
						UnpackRGB(r,g,b, RGB_YELLOW);
						ScaleColors(r, g, b, (byte)128);
					}
					SPR_Set(p->hInactive, r, g, b);
					SPR_DrawAdditive(0, x, y, &p->rcInactive);
				}

				// Draw Ammo Bar
				DrawAmmoBar(p, x + giABWidth/2, y, giABWidth, giABHeight);
				y += p->rcActive.bottom - p->rcActive.top + 5;
			}
			x += iWidth + 5;
		}
		else
		{
			// Draw Row of weapons.
			UnpackRGB(r,g,b, RGB_GREEN);

			for (int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++)
			{
				HUD_WEAPON *p = gWR.GetWeaponSlot(i, iPos);

				if (p == NULL || !p->iId)
					continue;

				if (gWR.HasAmmo(p))
				{
					UnpackRGB(r,g,b, RGB_GREEN);
					a = 128;
				}
				else
				{
					UnpackRGB(r,g,b, RGB_YELLOW);
					a = 96;
				}
				FillRGBA(x, y, giBucketWidth, giBucketHeight, r, g, b, a);
				y += giBucketHeight + 5;
			}
			x += giBucketWidth + 5;
		}
	}	
	return 1;
}


//------------------------------------------------------------------------
// Message Handlers
//------------------------------------------------------------------------

// AmmoX  -- Update the count of a known type of ammo
int CHudAmmo::MsgFunc_AmmoX(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();
	END_READ();
	gWR.SetAmmo(iIndex, abs(iCount));
	return 1;
}

int CHudAmmo::MsgFunc_AmmoPickup(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();
	END_READ();
	// Add ammo to the history
	gHR.AddToHistory( HISTSLOT_AMMO, iIndex, abs(iCount) );
	return 1;
}

int CHudAmmo::MsgFunc_WeapPickup(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	END_READ();
	// Add the weapon to the history
	gHR.AddToHistory(HISTSLOT_WEAP, iIndex);
	m_iRecentPickedWeapon = iIndex;// XDM3035a
	return 1;
}

int CHudAmmo::MsgFunc_ItemPickup(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int count = READ_SHORT();
	const char *szName = READ_STRING();
	END_READ();
	// Add the weapon to the history
	gHR.AddToHistory(HISTSLOT_ITEM, szName, count);

	return 1;
}

int CHudAmmo::MsgFunc_HideWeapon(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	gHUD.m_iHideHUDDisplay = READ_BYTE();
	END_READ();

	if (gEngfuncs.IsSpectateOnly())
		return 1;

	if (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL))
	{
		static wrect_t nullrc;
		gpActiveSel = NULL;
		SetCrosshair(0, nullrc, 0, 0, 0);
	}
	else
	{
		if (m_pWeapon)
			SetCrosshair(m_pWeapon->hCrosshair, m_pWeapon->rcCrosshair, 255, 255, 255);
	}
	return 1;
}

// OBSOLETE! DO NOT USE!
//  CurWeapon: Update hud state with the current weapon and clip count. Ammo
//  counts are updated with AmmoX. Server assures that the Weapon ammo type 
//  numbers match a real ammo type.
int CHudAmmo::MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf)
{
	static wrect_t nullrc;
	int fOnTarget = FALSE;

	BEGIN_READ(pbuf, iSize);
	int iState = READ_BYTE();
	int iId = READ_CHAR();
	int iClip = READ_CHAR();
	END_READ();

	// detect if we're also on target
	if (iState > 1)
		fOnTarget = TRUE;

	if (iId < 1)
	{
		SetCrosshair(0, nullrc, 0, 0, 0);
		m_iFlags &= ~HUD_ACTIVE;// XDM3035c: don't draw ammo indicators too
//		WEAPON *p = gWR.GetWeapon(lol?);
//		if (p)
//			gWR.DropWeapon(p);
		return 0;
	}

	if (g_iUser1 != OBS_IN_EYE)
	{
		// Is player dead???
		if ((iId == -1) && (iClip == -1))
		{
			gHUD.m_fPlayerDead = TRUE;
			gpActiveSel = NULL;
			return 1;
		}
		gHUD.m_fPlayerDead = FALSE;
	}

	HUD_WEAPON *pWeapon = gWR.GetWeapon(iId);

	if (!pWeapon)
		return 0;

	if (iClip < -1)
		pWeapon->iClip = abs(iClip);
	else
		pWeapon->iClip = iClip;

	if (iState == 0)	// we're not the current weapon, so update no more
		return 1;

	m_pWeapon = pWeapon;

	if (!(gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)) && pWeapon)// XDM: check weapon
		UpdateCrosshair(fOnTarget?2:1, (gHUD.m_iFOV >= 90)?0:1);// XDM

	m_fFade = 200.0f; //!!!
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

// WeaponList -- Tells the hud about a new weapon type.
int CHudAmmo::MsgFunc_WeaponList(const char *pszName, int iSize, void *pbuf)
{
	HUD_WEAPON Weapon;

	BEGIN_READ(pbuf, iSize);
	strcpy(Weapon.szName, READ_STRING());
	Weapon.iAmmoType = (int)READ_CHAR();	

	Weapon.iMax1 = READ_BYTE();
	if (Weapon.iMax1 == 255)
		Weapon.iMax1 = -1;

	Weapon.iAmmo2Type = (int)READ_CHAR();
	Weapon.iMax2 = READ_BYTE();
	if (Weapon.iMax2 == 255)
		Weapon.iMax2 = -1;

#ifdef SERVER_WEAPON_SLOTS
	Weapon.iSlot = READ_CHAR();
	Weapon.iSlotPos = READ_CHAR();
#endif
	Weapon.iId = READ_CHAR();
	Weapon.iFlags = READ_BYTE();
	Weapon.iClip = 0;
	END_READ();

#ifndef SERVER_WEAPON_SLOTS
	if (GetWeaponSlotPos(Weapon.iId, Weapon.iSlot, Weapon.iSlotPos) == 0)
	{
		CON_PRINTF("CHudAmmo::MsgFunc_WeaponList() error! Cannot find slot information for weapon %d (%s)!\n", Weapon.iId, Weapon.szName);
		Weapon.iSlot = 0;
		Weapon.iSlotPos = 0;
	}
#endif
/*
#ifdef _DEBUG
	CON_DPRINTF("HUDAmmo: Adding %s (at1 %d max %d) (at2 %d max %d) sl %d, pos %d, ID %d, flags %d\n",
		Weapon.szName, Weapon.iAmmoType, Weapon.iMax1, Weapon.iAmmo2Type, Weapon.iMax2,
		Weapon.iSlot, Weapon.iSlotPos, Weapon.iId, Weapon.iFlags);
#endif*/
	gWR.AddWeapon(&Weapon);// <- LoadWeaponSprites() already there
	return 1;
}


// XDM: replaces CurWeapon message, handles multiple weapons at once
int CHudAmmo::MsgFunc_UpdWeapons(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
//	CON_PRINTF("CHudAmmo::MsgFunc_UpdWeapons(size: %d)\n", iSize);
	byte iState;
	byte iId;
	signed char iClip;
	int num = 0;
	while (READ_REMAINING())
	{
		iState = READ_BYTE();
		iId = READ_CHAR();
		if (READ_REMAINING() < 1)
		{
			CON_DPRINTF("MsgFunc_UpdWeapons(): bad message!\n");
			return 0;
		}
		iClip = READ_CHAR();
		++num;

		if (iId == WEAPON_NONE)
			continue;

		if (g_iUser1 != OBS_IN_EYE)
		{
			// Is player dead???
			if ((iId == -1) && (iClip == -1))
			{
				gHUD.m_fPlayerDead = TRUE;
				gpActiveSel = NULL;
				END_READ();
				return 1;
			}
			gHUD.m_fPlayerDead = FALSE;
		}

		HUD_WEAPON *pWeapon = gWR.GetWeapon(iId);

		if (pWeapon == NULL)
			continue;

		if (iClip == 255)// XDM3035c: -1 TODO: TESTME: if something goes wrong with clips - check this and server code!
			pWeapon->iClip = -1;
		else if (iClip < -1)
			pWeapon->iClip = abs(iClip);
		else
			pWeapon->iClip = iClip;

		if (iState > 0)
		{
			m_pWeapon = pWeapon;// make weapon current

			if (!(gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)) && pWeapon)// XDM: check weapon
				UpdateCrosshair(iState, (gHUD.m_iFOV >= 90)?0:1);// XDM

//			CON_DPRINTF("CHudAmmo::MsgFunc_UpdWeapons() weapon %d state is %d\n", iId, iState);
		}
		m_fFade = 200.0f; //!!!
		m_iFlags |= HUD_ACTIVE;
	}
	END_READ();
//	CON_DPRINTF("CHudAmmo::MsgFunc_UpdWeapons() updated %d weapons\n", num);
	return 1;
}

// XDM: replaces MsgFunc_AmmoX message, handles multiple weapons at once
int CHudAmmo::MsgFunc_UpdAmmo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
//	CON_PRINTF("CHudAmmo::MsgFunc_UpdAmmo(size: %d)\n", iSize);
	int num = 0;
	int iIndex;
	int iCount;
	while (READ_REMAINING())
	{
		iIndex = READ_BYTE();
		if (READ_REMAINING() < 1)
		{
			CON_DPRINTF("MsgFunc_UpdAmmo(): bad message!\n");
			END_READ();
			return 0;
		}
		iCount = READ_BYTE();
		++num;

		gWR.SetAmmo(iIndex, abs(iCount));
	}
	END_READ();
//	CON_DPRINTF("CHudAmmo::MsgFunc_UpdAmmo() updated %d ammo\n", num);
	return 1;
}





//------------------------------------------------------------------------
// Command Handlers
//------------------------------------------------------------------------
// Slot button pressed
void CHudAmmo::SlotInput(int iSlot)
{
	// Let the Viewport use it first, for menus
	if (gViewPort && gViewPort->SlotInput(iSlot))
		return;

	gWR.SelectSlot(iSlot, FALSE, 1);
}

void CHudAmmo::UserCmd_Slot1(void)
{
	SlotInput(0);
}

void CHudAmmo::UserCmd_Slot2(void)
{
	SlotInput(1);
}

void CHudAmmo::UserCmd_Slot3(void)
{
	SlotInput(2);
}

void CHudAmmo::UserCmd_Slot4(void)
{
	SlotInput(3);
}

void CHudAmmo::UserCmd_Slot5(void)
{
	SlotInput(4);
}

void CHudAmmo::UserCmd_Slot6(void)
{
	SlotInput(5);
}

void CHudAmmo::UserCmd_Slot7(void)
{
	SlotInput(6);
}

void CHudAmmo::UserCmd_Slot8(void)
{
	SlotInput(7);
}

void CHudAmmo::UserCmd_Close(void)
{
	if (gpActiveSel)
	{
		gpLastSel = gpActiveSel;
		gpActiveSel = NULL;
		PlaySound("common/wpn_hudoff.wav", VOL_NORM);
	}
	else
		CLIENT_COMMAND("escape");
}

// Selects the next item in the weapon menu
void CHudAmmo::UserCmd_NextWeapon(void)
{
	if (gHUD.m_fPlayerDead || (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)))
		return;

	if (!gpActiveSel || gpActiveSel == (HUD_WEAPON *)1)
		gpActiveSel = m_pWeapon;

	int pos = 0;
	int slot = 0;
	if (gpActiveSel)
	{
		pos = gpActiveSel->iSlotPos + 1;
		slot = gpActiveSel->iSlot;
	}

	for (int loop = 0; loop <= 1; ++loop)
	{
		for ( ; slot < MAX_WEAPON_SLOTS; ++slot)
		{
			for ( ; pos < MAX_WEAPON_POSITIONS; ++pos)
			{
				HUD_WEAPON *wsp = gWR.GetWeaponSlot(slot, pos);
				if (wsp && gWR.HasAmmo(wsp))
				{
					gpActiveSel = wsp;
					return;
				}
			}
			pos = 0;
		}
		slot = 0;  // start looking from the first slot again
	}
	gpActiveSel = NULL;
}

// Selects the previous item in the menu
void CHudAmmo::UserCmd_PrevWeapon(void)
{
	if (gHUD.m_fPlayerDead || (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)))
		return;

	if (!gpActiveSel || gpActiveSel == (HUD_WEAPON *)1)
		gpActiveSel = m_pWeapon;

	int pos = MAX_WEAPON_POSITIONS-1;
	int slot = MAX_WEAPON_SLOTS-1;
	if (gpActiveSel)
	{
		pos = gpActiveSel->iSlotPos - 1;
		slot = gpActiveSel->iSlot;
	}

	for (int loop = 0; loop <= 1; loop++)
	{
		for ( ; slot >= 0; --slot)
		{
			for ( ; pos >= 0; --pos)
			{
				HUD_WEAPON *wsp = gWR.GetWeaponSlot(slot, pos);
				if (wsp && gWR.HasAmmo(wsp))
				{
					gpActiveSel = wsp;
					return;
				}
			}
			pos = MAX_WEAPON_POSITIONS-1;
		}
		slot = MAX_WEAPON_SLOTS-1;
	}
	gpActiveSel = NULL;
}

void CHudAmmo::UserCmd_InvUp(void)
{
	if (gpActiveSel && gpActiveSel != (HUD_WEAPON *)1)
	{
		int pos = gpActiveSel->iSlotPos - 1;
		int slot = gpActiveSel->iSlot;

		while (pos >= 0 && pos < MAX_WEAPON_POSITIONS)
		{
			HUD_WEAPON *wsp = gWR.GetWeaponSlot(slot, pos);
			if (wsp && gWR.HasAmmo(wsp))
			{
				PlaySound("common/wpn_moveselect.wav", VOL_NORM);
				gpActiveSel = wsp;
				return;
			}
			pos--;
		}
	}
}

void CHudAmmo::UserCmd_InvDown(void)
{
	if (gpActiveSel && gpActiveSel != (HUD_WEAPON *)1)
	{
		int pos = gpActiveSel->iSlotPos + 1;
		int slot = gpActiveSel->iSlot;

		while (pos >= 0 && pos < MAX_WEAPON_POSITIONS)
		{
			HUD_WEAPON *wsp = gWR.GetWeaponSlot(slot, pos);
			if (wsp && gWR.HasAmmo(wsp))
			{
				PlaySound("common/wpn_moveselect.wav", VOL_NORM);
				gpActiveSel = wsp;
				return;
			}
			pos++;
		}
	}
}
