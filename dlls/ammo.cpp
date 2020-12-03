#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "game.h"
#include "gamerules.h"
#include "globals.h"
#include "explode.h"
#include "items.h"
#include "player.h"
#include "msg_fx.h"

int giAmmoIndex = 0;
DLL_GLOBAL AmmoInfo g_AmmoInfoArray[MAX_AMMO_SLOTS];

//-----------------------------------------------------------------------------
// Purpose: Precaches the ammo and queues the ammo info for sending to clients
// Input  : *szAmmoname - 
// Output : int index or -1
//-----------------------------------------------------------------------------
int AddAmmoToRegistry(const char *szAmmoName, int iMaxCarry)
{
	if (szAmmoName == NULL)
		return -1;

	// make sure it's not already in the registry
	int i = GetAmmoIndexFromRegistry(szAmmoName);
	if (i != -1)
	{
		if (iMaxCarry > g_AmmoInfoArray[i].iMaxCarry)// somebody else says player should carry more ammo of this type
		{
			ALERT(at_console, "Warning: AddAmmoToRegistry(%s) updating iMaxCarry from %d to %d!\n", szAmmoName, g_AmmoInfoArray[i].iMaxCarry, iMaxCarry);
			g_AmmoInfoArray[i].iMaxCarry = iMaxCarry;
		}
		return i;// found
	}

// XDM?	giAmmoIndex++;
	ASSERT(giAmmoIndex < MAX_AMMO_SLOTS);

	if (giAmmoIndex < MAX_AMMO_SLOTS)
	{
		strncpy(g_AmmoInfoArray[giAmmoIndex].name, szAmmoName, MAX_AMMO_NAME_LEN);
		g_AmmoInfoArray[giAmmoIndex].iMaxCarry = iMaxCarry;
//		g_AmmoInfoArray[giAmmoIndex].pszName = szAmmoname;
//		g_AmmoInfoArray[giAmmoIndex].iId = giAmmoIndex;   // yes, this info is redundant
		++giAmmoIndex;
		return giAmmoIndex-1;
	}
//		giAmmoIndex = 0;
	return -1;// 0?
}

//-----------------------------------------------------------------------------
// Purpose: GetAmmoIndex
// Input  : *psz - 
// Output : int
//-----------------------------------------------------------------------------
int GetAmmoIndexFromRegistry(const char *szAmmoName)
{
	if (szAmmoName == NULL)
		return -1;

	int i;
	for (i = 0; i < MAX_AMMO_SLOTS; ++i)// XDM: start from 0
	{
		if (g_AmmoInfoArray[i].name[0] == 0)// 1st char is empty
			continue;

		if (stricmp(szAmmoName, g_AmmoInfoArray[i].name) == 0)// case-insensitive for stupid mappers
			return i;
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: maximum amount of that type of ammunition that a player can carry.
// Input  : ammoID - 
// Output : int
//-----------------------------------------------------------------------------
int MaxAmmoCarry(int ammoID)
{
	if (ammoID >= 0)
		return g_AmmoInfoArray[ammoID].iMaxCarry;

	return 0;// XDM3035c: TESTME!! was -1
}

//-----------------------------------------------------------------------------
// Purpose: MaxAmmoCarry - pass in a name and this function will tell
// you the maximum amount of that type of ammunition that a player can carry.
// Input  : *szName - 
// Output : int
//-----------------------------------------------------------------------------
int MaxAmmoCarry(const char *szName)
{
	int ammoID = GetAmmoIndexFromRegistry(szName);
	int c = MaxAmmoCarry(ammoID);
	if (c <= 0)
		ALERT(at_console, "MaxAmmoCarry() doesn't recognize '%s'!\n", szName);

	return c;
}


//=========================================================
// base ammo class
//=========================================================

TYPEDESCRIPTION	CBasePlayerAmmo::m_SaveData[] =
{
	DEFINE_FIELD(CBasePlayerAmmo, m_iAmmoGive, FIELD_INTEGER),
	DEFINE_FIELD(CBasePlayerAmmo, m_iAmmoMax, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CBasePlayerAmmo, CBaseEntity);

void CBasePlayerAmmo::Precache(void)// XDM
{
	PRECACHE_SOUND("items/9mmclip1.wav");
}

void CBasePlayerAmmo::Spawn(void)
{
	Precache();
	pev->movetype = MOVETYPE_TOSS;
//	pev->solid = SOLID_TRIGGER;
	pev->scale = UTIL_GetWeaponWorldScale();
	pev->animtime = gpGlobals->time + 0.5f;
	pev->framerate = 1;
	pev->health = 250;

	if (g_pGameRules->IsMultiplayer() && mp_wpnboxbrk.value > 0.0f)// XDM
		pev->takedamage = DAMAGE_YES;
	else
		pev->takedamage = DAMAGE_NO;

	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));

	if (pev->spawnflags & SF_NOTREAL)
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time + WEAPONBOX_DESTROY_TIME;// XDM
	}

	Materialize();// XDM3035c: reuse more code
//	UTIL_SetOrigin(pev, pev->origin);
//	SetTouch(&CBasePlayerAmmo::DefaultTouch);
}

BOOL CBasePlayerAmmo::IsPushable(void)// XDM
{
	if (pev->spawnflags & SF_NORESPAWN)
		return TRUE;

	return FALSE;
}

CBaseEntity *CBasePlayerAmmo::Respawn(void)
{
	pev->effects |= EF_NODRAW;
	SetTouchNull();//NULL);
	UTIL_SetOrigin(pev, pev->origin);// move to wherever I'm supposed to repawn.
	SetThink(&CBasePlayerAmmo::Materialize);
	pev->nextthink = g_pGameRules->FlAmmoRespawnTime(this);
	return this;
}

// TODO: global mechanism
void CBasePlayerAmmo::Materialize(void)
{
	if (pev->effects & EF_NODRAW)
	{
		// changing from invisible state to visible.
		pev->effects &= ~EF_NODRAW;
		pev->effects |= EF_MUZZLEFLASH;
		
		MESSAGE_BEGIN(MSG_PVS, gmsgItemSpawn, pev->origin+Vector(0,0,4));
			WRITE_BYTE(EV_ITEMSPAWN_AMMO);
			WRITE_SHORT(entindex());
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_ANGLE(pev->angles.x);
			WRITE_ANGLE(pev->angles.y);
			WRITE_ANGLE(pev->angles.z);
			WRITE_SHORT(pev->modelindex);
			WRITE_BYTE((int)pev->scale*10);
			WRITE_BYTE(pev->body);
			WRITE_BYTE(pev->skin);
			WRITE_BYTE(pev->sequence);
		MESSAGE_END();

//		PLAYBACK_EVENT_FULL(0, ENT(pev), g_usItemSpawn, 0.0f, (float *)&pev->origin, (float *)&pev->angles, pev->scale, pev->sequence, EV_ITEMSPAWN_AMMO, pev->modelindex, pev->body, pev->skin);
//		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "items/respawn.wav", VOL_NORM, ATTN_NORM, 0, 110);// XDM: 110
	}
	if (g_pGameRules->IsMultiplayer())
		pev->flags |= FL_HIGHLIGHT;// XDM3035c

	pev->solid = SOLID_TRIGGER;
	UTIL_SetOrigin(pev, pev->origin);
	SetTouch(&CBasePlayerAmmo::DefaultTouch);
}

void CBasePlayerAmmo::DefaultTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
		return;

	if (AddAmmo(pOther))
	{
		if (!(pev->spawnflags & (SF_NOTREAL|SF_NORESPAWN)) && g_pGameRules->AmmoShouldRespawn(this))
		{
			Respawn();
		}
		else
		{
			SetTouchNull();
			SetThink(&CBaseEntity::SUB_Remove);
			pev->nextthink = gpGlobals->time + 0.1f;
		}
	}
	else if (gEvilImpulse101)
	{
		SetTouchNull();
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;// XDM + .1;
	}
}

BOOL CBasePlayerAmmo::AddAmmo(CBaseEntity *pOther)// XDM
{
	if (pOther->GiveAmmo(m_iAmmoGive, STRINGV(pev->message), m_iAmmoMax) > 0)
	{
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM);
		return TRUE;
	}
	return FALSE;
}

void CBasePlayerAmmo::InitAmmo(const int &ammo_give, const char *name, const int &ammo_max)
{
	m_iAmmoGive = ammo_give;
	m_iAmmoMax = ammo_max;
	pev->message = MAKE_STRING(name);// must be safe
}

int CBasePlayerAmmo::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pev->takedamage == DAMAGE_NO)
		return 0;

	if (bitsDamageType & DMG_NOSELF)
		return 0;

	if (pev->spawnflags & SF_NORESPAWN)// dropped by somebody
	{
		pev->health -= flDamage;

		if (pev->health <= 0)
		{
			Killed(pInflictor, pAttacker, GIB_NORMAL);
		}
		return 1;
	}
	return 0;
}

void CBasePlayerAmmo::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)// XDM3035c
{
	FX_Trail(pev->origin, NULL, FX_PLAYER_WEAPON_BLAST);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

//=========================================================
// item_superweapon Actually this is ammo for super weapon
//=========================================================

class CSWweaponAmmo : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_items.mdl");
		pev->body = ITEM_BODY_SUPERWEAPON_AMMO;
		CBasePlayerAmmo::Spawn();
	}
	void Precache(void)
	{
		int m_iGiveAmmo;
		switch (RANDOM_LONG(0,6))
		{
			case 0:	m_iGiveAmmo = AMMO_USE_GUIDED_BOMB; break;
			case 1:	m_iGiveAmmo = AMMO_USE_PROTON_MISSILE; break;
			case 2:	m_iGiveAmmo = AMMO_USE_DISPLACER; break;
			case 3:	m_iGiveAmmo = AMMO_USE_SHOCK_WAVE; break;
			case 4:	m_iGiveAmmo = AMMO_USE_NUCLEAR_MISSILE; break;
			case 5:	m_iGiveAmmo = AMMO_USE_CHRONOSCEPTRE; break;
			case 6:	m_iGiveAmmo = AMMO_USE_SUN_OF_GOD; break;
		}
		InitAmmo(m_iGiveAmmo, "energy_core", MAX_AMMO_SUPERWEAPON);
		CBasePlayerAmmo::Precache();
	}
};
LINK_ENTITY_TO_CLASS(item_superweapon, CSWweaponAmmo);