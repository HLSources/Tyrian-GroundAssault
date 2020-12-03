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
// ammohistory.h
//
#ifndef	AMMOHISTORY_H
#define	AMMOHISTORY_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include "protocol.h"


// this is the max number of items in each bucket
#define MAX_WEAPON_POSITIONS		MAX_WEAPON_SLOTS

class WeaponsResource
{
private:
	// Information about weapons & ammo
	HUD_WEAPON	rgWeapons[MAX_WEAPONS];	// Weapons Array

	// counts of weapons * ammo
	HUD_WEAPON *rgSlots[MAX_WEAPON_SLOTS+1][MAX_WEAPON_POSITIONS+1];	// The slots currently in use by weapons.  The value is a pointer to the weapon;  if it's NULL, no weapon is there
	int			riAmmo[MAX_AMMO_SLOTS];							// count of each ammo type

public:
	void Init(void)
	{
		Reset();
		memset(rgWeapons, 0, sizeof rgWeapons);
		for (int i=0; i<MAX_WEAPONS; ++i)// XDM
		{
			rgWeapons[i].iAmmoType = -1;
			rgWeapons[i].iAmmo2Type = -1;
			rgWeapons[i].iMax1 = -1;
			rgWeapons[i].iMax2 = -1;
		}
	}

	void Reset(void)
	{
		iOldWeaponBits = 0;
		memset(rgSlots, 0, sizeof rgSlots);
		memset(riAmmo, 0, sizeof riAmmo);
	}

///// WEAPON /////
	int			iOldWeaponBits;

	HUD_WEAPON *GetWeapon(int iId) { return &rgWeapons[iId]; }
	void AddWeapon(HUD_WEAPON *wp) 
	{ 
		rgWeapons[wp->iId] = *wp;	
		LoadWeaponSprites(&rgWeapons[wp->iId]);
	}

	void PickupWeapon(HUD_WEAPON *wp)
	{
		rgSlots[wp->iSlot][wp->iSlotPos] = wp;
	}

	void DropWeapon(HUD_WEAPON *wp)
	{
		rgSlots[wp->iSlot][wp->iSlotPos] = NULL;
	}
/*	void DropAllWeapons(void)
	{
		for ( int i = 0; i < MAX_WEAPONS; i++ )
		{
			if ( rgWeapons[i].iId )
				DropWeapon( &rgWeapons[i] );
		}
	}*/
	void LoadWeaponSprites(HUD_WEAPON* wp);
	void LoadAllWeaponSprites(void);

	HUD_WEAPON *GetWeaponSlot(const int &slot, const int &pos) { return rgSlots[slot][pos]; }
	HUD_WEAPON *GetFirstPos(const int &iSlot);
	HUD_WEAPON *GetNextActivePos(const int &iSlot, const int &iSlotPos);

	void SelectSlot(const int &iSlot, const int &fAdvance, const int &iDirection);
	int HasAmmo(HUD_WEAPON *p);

///// AMMO /////
//	AMMO GetAmmo(int iId) { return iId; }
	void SetAmmo(const int &iId, int iCount) { riAmmo[iId] = iCount; }
	int CountAmmo(const int &iId);

	HSPRITE *GetAmmoPicFromWeapon(const int &iAmmoId, wrect_t &rect);
};

extern WeaponsResource gWR;


#define MAX_HISTORY 12

enum
{
	HISTSLOT_EMPTY,
	HISTSLOT_AMMO,
	HISTSLOT_WEAP,
	HISTSLOT_ITEM,
};


class HistoryResource
{
private:
	struct HIST_ITEM
	{
		int type;
		float DisplayTime;  // the time at which this item should be removed from the history
		int iCount;
		int iId;
	};

	HIST_ITEM rgAmmoHistory[MAX_HISTORY];

public:
	void Init(void)
	{
		Reset();
	}
	void Reset(void)
	{
		memset(rgAmmoHistory, 0, sizeof(rgAmmoHistory));
	}
	void AddToHistory(const int &iType, const int &iId, const int &iCount = 0);
	void AddToHistory(const int &iType, const char *szName, const int &iCount = 0);
	void CheckClearHistory(void);
	int DrawAmmoHistory(const float &flTime);

	int iHistoryGap;
	int iCurrentHistorySlot;
};

extern HistoryResource gHR;


#endif // AMMOHISTORY_H
