//
// XDM: FUCK OFF WITH THIS SHIT!
//
// This code was SO crappy, hacky and disgusting that valve should have died of shame!
// The fact that it somehow did work is a pure miracle!
//
// TODO: get rid of those ugly BOOLs
//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "monsters.h"
#include "weapons.h"
#include "globals.h"// XDM
#include "soundent.h"
#include "decals.h"
#include "gamerules.h"
#include "game.h"
#include "weaponinfo.h"
#include "usercmd.h"

DLL_GLOBAL ItemInfo g_ItemInfoArray[MAX_WEAPONS];

extern int giAmmoIndex;

//-----------------------------------------------------------------------------
// Purpose: Precaches the weapon and queues the weapon info for sending to clients
// Warning: ammo_entities do not represent all possible ammo types, so this cannot be avoided :(
// Input  : *szClassname - 
//-----------------------------------------------------------------------------
void UTIL_PrecacheOtherWeapon(const char *szClassname)
{
	edict_t	*pent = CREATE_NAMED_ENTITY(MAKE_STRING(szClassname));
	if (FNullEnt(pent))
	{
		ALERT(at_console, "ERROR: NULL Ent in UTIL_PrecacheOtherWeapon(%s)\n", szClassname);
		return;
	}
	CBaseEntity *pEntity = CBaseEntity::Instance(VARS(pent));
	if (pEntity)// don't even spawn
	{
		pEntity->Precache();
		if (pEntity->IsPlayerItem())
		{
			ItemInfo II;
			memset(&II, 0, sizeof(II));
			if (((CBasePlayerItem *)pEntity)->GetItemInfo(&II))
			{
				g_ItemInfoArray[II.iId] = II;

				if (II.pszAmmo1 && *II.pszAmmo1)
					AddAmmoToRegistry(II.pszAmmo1, II.iMaxAmmo1);

				if (II.pszAmmo2 && *II.pszAmmo2)
					AddAmmoToRegistry(II.pszAmmo2, II.iMaxAmmo2);
			}
		}
	}
	REMOVE_ENTITY(pent);
}

//-----------------------------------------------------------------------------
// Purpose: Precaches ammo resources and registers ammo type in ammo registry
// Input  : *szClassname - 
//-----------------------------------------------------------------------------
void UTIL_PrecacheAmmo(const char *szClassname)
{
	UTIL_PrecacheOther(szClassname);
}

//-----------------------------------------------------------------------------
// Purpose: called by worldspawn
//-----------------------------------------------------------------------------
void W_Precache(void)
{
	memset(g_ItemInfoArray, 0, sizeof(g_ItemInfoArray));
	memset(g_AmmoInfoArray, 0, sizeof(g_AmmoInfoArray));
	giAmmoIndex = 0;

	SERVER_PRINT(" Precaching items...\n");
	UTIL_PrecacheOtherWeapon("weapon_tyriangun");
	UTIL_PrecacheOther("monster_mortar");// from mtarget.cpp
}




TYPEDESCRIPTION CBasePlayerItem::m_SaveData[] =
{
	DEFINE_FIELD( CBasePlayerItem, m_pPlayer, FIELD_CLASSPTR ),
	DEFINE_FIELD( CBasePlayerItem, m_iId, FIELD_INTEGER ),
};
IMPLEMENT_SAVERESTORE( CBasePlayerItem, CBaseAnimating );


TYPEDESCRIPTION CBasePlayerWeapon::m_SaveData[] =
{
#if defined( CLIENT_WEAPONS )
	DEFINE_FIELD( CBasePlayerWeapon, m_flNextAmmoBurn, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayerWeapon, m_flNextPrimaryAttack, FIELD_FLOAT ),
 	DEFINE_FIELD( CBasePlayerWeapon, m_flNextSecondaryAttack, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayerWeapon, m_flTimeWeaponIdle, FIELD_FLOAT ),
#else
	DEFINE_FIELD( CBasePlayerWeapon, m_flNextAmmoBurn, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayerWeapon, m_flNextPrimaryAttack, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayerWeapon, m_flNextSecondaryAttack, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayerWeapon, m_flTimeWeaponIdle, FIELD_TIME ),
#endif // CLIENT_WEAPONS
	DEFINE_FIELD( CBasePlayerWeapon, m_iPrimaryAmmoType, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayerWeapon, m_iSecondaryAmmoType, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayerWeapon, m_iClip, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayerWeapon, m_fInReload, FIELD_INTEGER ),// XDM
	DEFINE_FIELD( CBasePlayerWeapon, m_fInSpecialReload, FIELD_INTEGER ),// XDM
	DEFINE_FIELD( CBasePlayerWeapon, m_iDefaultAmmo, FIELD_INTEGER ),

//	DEFINE_FIELD( CBasePlayerWeapon, m_iClientClip, FIELD_INTEGER )	 , reset to zero on load so hud gets updated correctly
//  DEFINE_FIELD( CBasePlayerWeapon, m_iClientWeaponState, FIELD_INTEGER ), reset to zero on load so hud gets updated correctly
};
IMPLEMENT_SAVERESTORE(CBasePlayerWeapon, CBasePlayerItem);


//-----------------------------------------------------------------------------
// XDM3034: fire events on spawn
// OBSOLETE: just use ordinary pev->target
//-----------------------------------------------------------------------------
/*void CBasePlayerItem::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "fireonspawn"))
	{
		pev->message = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}*/

//-----------------------------------------------------------------------------
// Purpose: TODO: individual boxes?
//-----------------------------------------------------------------------------
void CBasePlayerItem::SetObjectCollisionBox(void)
{
	pev->absmin = pev->origin + Vector(-24, -24, 0);
	pev->absmax = pev->origin + Vector(24, 24, 16);
}

//-----------------------------------------------------------------------------
// Purpose: Default Spawn, never get here?
//-----------------------------------------------------------------------------
void CBasePlayerItem::Spawn(void)// XDM
{
	Precache();
	pev->takedamage = DAMAGE_NO;
	m_pPlayer = NULL;// XDM3035a
/*	if (!(pev->spawnflags & SF_NORESPAWN) && !(pev->effects & EF_NODRAW))
	{
		pev->origin.z += 2.0f;
		DROP_TO_FLOOR(ENT(pev));
		pev->origin.z += 2.0f;
	}*/
	Initialize();
	pev->scale = UTIL_GetWeaponWorldScale();// XDM3035b
	CBaseAnimating::Spawn();// XDM: starts animation
#ifdef _DEBUG_ITEMS
	ALERT(at_aiconsole, "CBasePlayerItem::Spawn()\n");
#endif
	pev->nextthink = gpGlobals->time + 0.1;
//	Drop();// XDM: ?
}

//-----------------------------------------------------------------------------
// Purpose: Precache
//-----------------------------------------------------------------------------
void CBasePlayerItem::Precache(void)
{
	CBaseAnimating::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Detect weapons dropped in lava or hit by explosions
// Input  : *pInflictor - 
//			*pAttacker - 
//			flDamage - 
//			bitsDamageType - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerItem::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pev->takedamage == DAMAGE_NO)
		return 0;
	if (GetOwner())
		return 0;

	if ((pev->spawnflags & SF_NORESPAWN) && pev->impulse > 0)// dropped by somebody
	{
		pev->health -= flDamage;

		if (pev->health <= 0)
		{
			Killed(pInflictor, pAttacker, GIB_NORMAL);
		}
		else if (g_pGameRules->FAllowEffects())
		{
			if (RANDOM_LONG(0,3) == 0)
				UTIL_Sparks(Center());
		}
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: A common method that can be used outside
// Input  : *pInflictor - 
//			*pAttacker - 
//			iGib - 
//-----------------------------------------------------------------------------
void CBasePlayerItem::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)// XDM3035c
{
	Kill();
}

//-----------------------------------------------------------------------------
// Purpose: A must
//-----------------------------------------------------------------------------
void CBasePlayerItem::Initialize(void)
{
	if (!IsCarried())
	{
		m_iModelIndexView = 0;
		m_iModelIndexWorld = pev->modelindex;
		FallInit();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sets up movetype, size, solidtype for a new weapon.
//-----------------------------------------------------------------------------
void CBasePlayerItem::FallInit(void)
{
//	pev->flags &= ~FL_DORMANT;
	pev->sequence = LookupSequence("stayput");
	if (pev->sequence < 0)
		pev->sequence = 1;// XDM: default "stayput" goes after "idle" which is used for player hands

	pev->scale = UTIL_GetWeaponWorldScale();
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;
	pev->health = 100;
	if ((pev->spawnflags & SF_NORESPAWN) && pev->impulse > 0)
	{
		ClearBits(pev->effects, EF_NODRAW);
		pev->takedamage = DAMAGE_YES;

	}
	ClearBits(pev->flags, FL_ONGROUND);
	UTIL_SetOrigin(pev, pev->origin);
	UTIL_SetSize(pev, g_vecZero, g_vecZero);//pointsize until it lands on the ground.
	SetTouch(&CBasePlayerItem::DefaultTouch);
	SetThink(&CBasePlayerItem::FallThink);
	pev->nextthink = gpGlobals->time + 0.1;
}

//-----------------------------------------------------------------------------
// Purpose: Items that have just spawned run this think
// to catch them when they hit the ground. Once we're sure
// that the object is grounded, we change its solid type
// to trigger and set it in a large box that helps the
// player get it.
//-----------------------------------------------------------------------------
void CBasePlayerItem::FallThink(void)
{
#ifdef _DEBUG
	if (IsCarried())
	{
		SetThinkNull();
		return;
	}
#endif
	pev->nextthink = gpGlobals->time + 0.1;

	if (pev->flags & FL_ONGROUND)
	{
		// clatter if we have an owner (i.e., dropped by someone)
		// don't clatter if the gun is waiting to respawn (if it's waiting, it is invisible!)
		if (pev->spawnflags & SF_NORESPAWN)//(!FNullEnt(pev->owner))
		{
			int pc = POINT_CONTENTS(pev->origin);// XDM: prevent flood
			if (pc == CONTENTS_SLIME || pc == CONTENTS_LAVA || pc == CONTENTS_SKY)
			{
				Kill();
				return;
			}
			if (g_pGameRules->IsMultiplayer())// XDM3035a: don't flood the world, remove after some time, but keep pickupable!
			{
				SetThink(&CBaseEntity::SUB_Remove);
				pev->nextthink = gpGlobals->time + ITEM_RESPAWN_TIME;
			}
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "items/weapondrop1.wav", VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(100,120));
//			SetModelCollisionBox();// XDM3035b: TODO
			AlignToFloor();// XDM
		}
		// lie flat
//		pev->angles.x = 0;
//		pev->angles.z = 0;
		Materialize();
		// Item should probably not stop thinking in case it gets pushed or falls from breakable, etc.
	}
}

//-----------------------------------------------------------------------------
// Purpose: make a CBasePlayerItem visible and tangible
//-----------------------------------------------------------------------------
void CBasePlayerItem::Materialize(void)
{
#ifdef _DEBUG_ITEMS
//	ALERT(at_aiconsole, "CBasePlayerItem::Materialize()\n");
	ASSERT(IsCarried() == FALSE);
#endif
	if (pev->effects & EF_NODRAW)
	{
		pev->effects &= ~EF_NODRAW;
		if (g_pGameRules->IsMultiplayer())
			pev->flags |= FL_HIGHLIGHT;// XDM3035c

		if (!(pev->spawnflags & SF_NORESPAWN))// XDM: ALL items spawn for the first time, so don't play sound!
		{
			pev->effects |= EF_MUZZLEFLASH;

			MESSAGE_BEGIN(MSG_PVS, gmsgItemSpawn, pev->origin+Vector(0,0,4));
				WRITE_BYTE(EV_ITEMSPAWN_WEAPON);
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
		
// event does not work for badly placed items!			PLAYBACK_EVENT_FULL(0, ENT(pev), g_usItemSpawn, 0.0f, (float *)&pev->origin, (float *)&pev->angles, pev->scale, pev->sequence, EV_ITEMSPAWN_WEAPON, pev->modelindex, pev->body, pev->skin);
//			EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "items/respawn.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		}
	}
	pev->solid = SOLID_TRIGGER;
	UTIL_SetOrigin(pev, pev->origin);// link into world.
	SetTouch(&CBasePlayerItem::DefaultTouch);
	SetThinkNull();
}


//-----------------------------------------------------------------------------
// Purpose: the item is trying to rematerialize,
// should it do so now or wait longer?
//-----------------------------------------------------------------------------
void CBasePlayerItem::AttemptToMaterialize(void)
{
	float time = g_pGameRules->FlWeaponTryRespawn(this);
	if (time == 0)
	{
		pev->nextthink = 0;
		Materialize();
		return;
	}
	pev->nextthink = gpGlobals->time + time;
}

//-----------------------------------------------------------------------------
// Purpose: this item is already in the world, but it is
// invisible and intangible. Make it visible and tangible.
// TODO: revisit, use customized version of global respawn mechanism
// DO NOT REMOVE OR EVEN DO ANYTHING WITH <this>!
// Output : CBaseEntity - duplicated item
//-----------------------------------------------------------------------------
CBaseEntity *CBasePlayerItem::StartRespawn(void)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerItem(%d)::StartRespawn() (id %d)\n", entindex(), GetID());
#endif
	if (!g_pGameRules->WeaponShouldRespawn(this))
		return NULL;

// this is completely different	if (!ShouldRespawn())
	if (pev->spawnflags & (SF_NOTREAL|SF_NORESPAWN))// XDM3035b
		return NULL;

	// make a copy of this weapon that is invisible and inaccessible to players (no touch function).
	// The weapon spawn/respawn code will decide when to make the weapon visible and touchable.
	CBaseEntity *pNewWeapon = CBaseEntity::Create(STRINGV(pev->classname), g_pGameRules->VecWeaponRespawnSpot(this), pev->angles, pev->owner);
	if (pNewWeapon)
	{
		pNewWeapon->pev->effects |= EF_NODRAW;// invisible for now
		pNewWeapon->SetTouchNull();// no touch
		pNewWeapon->SetThink(&CBasePlayerItem::AttemptToMaterialize);
		DROP_TO_FLOOR(ENT(pev));
		// not a typo! We want to know when the weapon the player just picked up should respawn!
		// This new entity we created is the replacement, but when it should respawn is based on conditions belonging to the weapon that was taken.
		pNewWeapon->pev->nextthink = g_pGameRules->FlWeaponRespawnTime(this);
	}
	else
		ALERT(at_console, "CBasePlayerItem(%d)::StartRespawn() failed to create %s!\n", entindex(), STRING(pev->classname));

//	SUB_UseTargets(this, USE_SET, 1.0f);// XDM3034
//	if (!FStringNull(pev->target))
//		FireTargets(STRING(pev->target), this, this, USE_SET, 1.0f);// XDM3034

	return pNewWeapon;
}

//-----------------------------------------------------------------------------
// Purpose: Don't collide with the player that has just dropped me
// Input  : *pOther - 
//-----------------------------------------------------------------------------
int CBasePlayerItem::ShouldCollide(CBaseEntity *pOther)
{
	if ((pev->spawnflags & SF_NORESPAWN) && pev->impulse > 0)
	{
		if (!(pev->flags & FL_ONGROUND) && pOther->IsPlayer())
		{
			if (m_hLastOwner.Get() && (m_hLastOwner.Get() == pOther->edict()))
				return 0;// Don't touch the player that has just dropped this item
		}
	}

	return CBaseEntity::ShouldCollide(pOther);
}

//-----------------------------------------------------------------------------
// Purpose: World item touched by someone
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CBasePlayerItem::DefaultTouch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())// if it's not a player, ignore (FUTURE: allow monsters to pick up and use items?)
		return;

	if (pev->flags & FL_KILLME)
	{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerItem(%d)::DefaultTouch(%d %s) Error: id %d marked for removal!\n", entindex(), pOther->entindex(), STRING(pOther->pev->netname), GetID());
#endif
		REMOVE_ENTITY(ENT(pev));
		return;
	}
	if ((pev->spawnflags & SF_NORESPAWN) && pev->impulse > 0)
	{
		if (!(pev->flags & FL_ONGROUND))
		{
			if (m_hLastOwner.Get() && (m_hLastOwner.Get() == pOther->edict()))// XDM
				return;// Don't touch the player that has just dropped this box
		}
	}

#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerItem(%d)::DefaultTouch(%d %s) id %d\n", entindex(), pOther->entindex(), STRING(pOther->pev->netname), GetID());
#endif

	if (GetOwner())
	{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerItem(%d)::DefaultTouch(%d %s) Error: id %d already has an owner (%d %s), exiting.\n", entindex(), pOther->entindex(), STRING(pOther->pev->netname), GetID(), GetOwner()->entindex(), STRING(GetOwner()->pev->netname));
#endif
		return;
	}

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;
	if (!g_pGameRules->CanHavePlayerItem(pPlayer, this))
	{
		if (gEvilImpulse101)
			UTIL_Remove(this);

		return;
	}

	int result = pOther->AddPlayerItem(this);
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerItem(%d)::AddPlayerItem(%d %s) result: %d\n", entindex(), pOther->entindex(), STRING(pOther->pev->netname), result);
#endif
	if (result != ITEM_ADDRESULT_NONE)
	{
		if (!gEvilImpulse101)
		{
		//	m_flDelay = 0.1f;
			CBaseDelay::SUB_UseTargets(pOther, USE_TOGGLE, 0); // UNDONE: when should this happen?
//			if (!FStringNull(pev->target))// special anonymous version which does not use 'this'
//				FireTargets(STRING(pev->target), pOther, g_pWorld/*hack!*/, USE_TOGGLE, 0);

			StartRespawn();// create respawning duplicate before origin is modified
		}

		AttachToPlayer(pPlayer);// this changes our origin!

		if (result != ITEM_ADDRESULT_EXTRACTED)// "ammo picked" sound is played instead
			EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/gunpickup2.wav", VOL_NORM, ATTN_NORM);

		if (m_pfnTouch != NULL)
		{
			ALERT(at_console, "CBasePlayerItem(%d)::DefaultTouch(%d %s) %d TOUCH STILL ACTIVE! 2\n", entindex(), pOther->entindex(), STRING(pOther->pev->netname), GetID());
			DBG_FORCEBREAK
		}
	}

	// if this item was not moved to player's inventory, remove it
	if (result == ITEM_ADDRESULT_EXTRACTED || 
		(result == ITEM_ADDRESULT_NONE && gEvilImpulse101))// extracted, or was created temporarily
	{
		SetOwner(NULL);
		DestroyItem();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Selected, but put down
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerItem::IsHolstered(void)// XDM3035
{
	if (m_pPlayer)
	{
		if (/*m_pPlayer->pev->viewmodel == 0 && */m_pPlayer->pev->weaponmodel == 0)
			return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Affected by forces only if dropped by someone
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerItem::IsPushable(void)// XDM
{
	if ((pev->spawnflags & SF_NORESPAWN) && !IsCarried())// dropped by monster AND not picked up and carried
		return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Is carried by owner (in inventory)
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerItem::IsCarried(void)// XDM
{
	if ((pev->effects & EF_NODRAW) && pev->aiment)
		return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Has any usable ammo?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
/*BOOL CBasePlayerItem::HasAmmo(void)// XDM
{
	if (m_pPlayer)
	{
		if (pszAmmo1() && m_pPlayer->AmmoInventory(this->PrimaryAmmoIndex()) > 0)// abstract PrimaryAmmoIndex() allows to call THIS HasAmmo() from CBasePlayerWeapon code
			return TRUE;

		if (pszAmmo2() && m_pPlayer->AmmoInventory(this->SecondaryAmmoIndex()) > 0)
			return TRUE;
	}
	return FALSE;
}*/

//-----------------------------------------------------------------------------
// Purpose: Caller may explicitly specify:
// Input  : type - primary/secondary/any type to check (flags)
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerItem::HasAmmo(byte type)
{
	if (m_pPlayer)
	{
		// AMMO_ANYTYPE is 0!
		if (type == AMMO_ANYTYPE || (type & AMMO_PRIMARY))
		{
			if (pszAmmo1() && m_pPlayer->AmmoInventory(this->PrimaryAmmoIndex()) > 0)// abstract PrimaryAmmoIndex() allows to call THIS HasAmmo() from CBasePlayerWeapon code
				return TRUE;
		}
		if (type == AMMO_ANYTYPE || (type & AMMO_SECONDARY))
		{
			if (pszAmmo2() && m_pPlayer->AmmoInventory(this->SecondaryAmmoIndex()) > 0)
				return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerItem::PrimaryAmmoIndex(void)
{
	return GetAmmoIndexFromRegistry(this->pszAmmo1());
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerItem::SecondaryAmmoIndex(void)
{
	return GetAmmoIndexFromRegistry(this->pszAmmo2());
}

//-----------------------------------------------------------------------------
// Purpose: Put away, don't select anything else from here
// Input  : skiplocal - 1 if client is predicting weapon animations
//-----------------------------------------------------------------------------
void CBasePlayerItem::Holster(int skiplocal /* = 0 */)
{
	if (m_pPlayer)
	{
		m_pPlayer->pev->viewmodel = 0;
		m_pPlayer->pev->weaponmodel = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Obsolete?
//-----------------------------------------------------------------------------
void CBasePlayerItem::UpdateItemInfo(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: AddToPlayer
// Input  : *pPlayer - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerItem::AddToPlayer(CBasePlayer *pPlayer)
{
	if (pPlayer == NULL)
	{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerItem(%d)::AddToPlayer(NULL) %d\n", entindex(), GetID());
		DBG_FORCEBREAK
#endif
		return FALSE;
	}
	SetOwner(pPlayer);
	return TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: Called through the newly-touched weapon's instance. The existing player weapon is pOriginal
// Input  : *pOriginal - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerItem::AddDuplicate(CBasePlayerItem *pOriginal)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerItem(%d)::AddDuplicate(%d) id %d\n", entindex(), pOriginal?pOriginal->entindex():0, GetID());
#endif
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: AttachToPlayer physically
// Warning: no calls to player functions from here!
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CBasePlayerItem::AttachToPlayer(CBasePlayer *pPlayer)
{
	if (pPlayer == NULL)
	{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerItem(%d)::AttachToPlayer(NULL) %d\n", entindex(), GetID());
		DBG_FORCEBREAK
#endif
		return;
	}
#ifdef _DEBUG_ITEMS
	else
		ALERT(at_console, "CBasePlayerItem(%d)::AttachToPlayer(%d %s) (id %d)\n", entindex(), pPlayer->entindex(), STRING(pPlayer->pev->netname), GetID());
#endif

	SetTouchNull();
	SetThinkNull();// XDM3035c: should we???
	SetOwner(pPlayer);// XDM3035a: WTF?!?!?!
	pev->origin = pPlayer->pev->origin;
	pev->movetype = MOVETYPE_FOLLOW;
//	pev->flags |= FL_DORMANT;// XDM3035b: test!
	pev->solid = SOLID_NOT;
	pev->impulse = 0;
	pev->aiment = pPlayer->edict();
	pev->effects = EF_NODRAW; // ??
	pev->modelindex = 0;// server won't send down to clients if modelindex == 0
	pev->model = iStringNull;
	pev->owner = pev->aiment;// same pPlayer->edict();
	pev->takedamage = DAMAGE_NO;// XDM3035: RadiusDamage still affects weapons!!
	pev->nextthink = gpGlobals->time + 0.1f;
	pev->playerclass = 1;// XDM3035a: mark as glass so trace won't hit it?
}

//-----------------------------------------------------------------------------
// Purpose: DetachFromPlayer physically
// Warning: no calls to player functions from here!
//-----------------------------------------------------------------------------
void CBasePlayerItem::DetachFromPlayer(void)
{
	if (m_pPlayer)
	{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerItem(%d)::DetachFromPlayer(%d) %d\n", entindex(), m_pPlayer->entindex(), GetID());
#endif
		pev->aiment = NULL;
		pev->owner = NULL;
		SetOwner(NULL);
		pev->modelindex = m_iModelIndexWorld;
//		pev->model = ???
//		pev->effects = 0;
//		pev->solid = SOLID_TRIGGER;
		pev->movetype = MOVETYPE_TOSS;
		pev->takedamage = DAMAGE_NO;
		pev->nextthink = gpGlobals->time + 0.01f;
		pev->playerclass = 0;
		pev->spawnflags |= SF_NORESPAWN;// don't respawn detached items!
//NO! Don't let players touch it here!		SetTouch(&CBasePlayerItem::DefaultTouch);
//		FallInit();?
	}
	else
	{
		ALERT(at_console, "CBasePlayerItem(%d)::DetachFromPlayer(NULL) %d NULL player!\n", entindex(), GetID());
		DBG_FORCEBREAK
	}
}

//-----------------------------------------------------------------------------
// Purpose: destroy and remove from world
//-----------------------------------------------------------------------------
void CBasePlayerItem::Kill(void)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerItem(%d)::Kill()(id %d)\n", entindex(), GetID());
#endif
	pev->targetname = iStringNull;
	pev->target = iStringNull;
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->nextthink = 0;
	pev->owner = NULL;
//	SetID(WEAPON_NONE);// XDM3035: mark as erased
	m_iId = WEAPON_NONE;
	SetTouchNull();
	SetThinkNull();
	SetUseNull();
	SetOwner(NULL);// XDM3035a

//	REMOVE_ENTITY(ENT(pev));// XDM3035b: TESTME
	if (pev->takedamage != DAMAGE_NO && pev->health <= 0.0f)
	{
		Disintegrate();
	}
	else
	{
		pev->health = 0;
		UTIL_Remove(this);
	}
//	SetThink(&CBaseEntity::SUB_Remove);
//	pev->nextthink = gpGlobals->time;// + 0.05;
}

//-----------------------------------------------------------------------------
// Purpose: Think that calls Kill()
//-----------------------------------------------------------------------------
void CBasePlayerItem::DestroyItem(void)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerItem(%d)::DestroyItem()(id %d)\n", entindex(), GetID());
#endif

	try
	{
	if (m_pPlayer)// if attached to a player, remove.
		m_pPlayer->RemovePlayerItem(this);
	}
	catch (...)
	{
		printf("*** CBasePlayerItem(%d)::DestroyItem() exception!\n", GetID());
		DBG_FORCEBREAK
	}
	Kill();
}

//-----------------------------------------------------------------------------
// Purpose: entity is about to be erased from world and server memory
//-----------------------------------------------------------------------------
void CBasePlayerItem::OnFreePrivateData(void)// XDM3035
{
	CBaseAnimating::OnFreePrivateData();
//	m_iId = 0;// TEST
//	m_pPlayer = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Safe method
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CBasePlayerItem::SetOwner(CBasePlayer *pPlayer)
{
#ifdef _DEBUG_ITEMS
	if (pPlayer == NULL)
	{
		ALERT(at_console, "CBasePlayerItem(%d)::SetOwner(NULL) %d\n", entindex(), GetID());
	}
#endif
	m_pPlayer = pPlayer;
	if (pPlayer)// remember only real players
		m_hLastOwner = pPlayer;
}

//-----------------------------------------------------------------------------
// Purpose: Safe method
// really contradicts game logic, but may be used to mark item for removal
// Input  : &ID - 
//-----------------------------------------------------------------------------
/*void CBasePlayerItem::SetID(const int &ID)
{
#ifdef _DEBUG_ITEMS
	if (ID <= 0)
		ALERT(at_console, "CBasePlayerItem(%d)::SetID(0) %d\n", entindex(), GetID());
#endif

	m_iId = ID;
}*/

//-----------------------------------------------------------------------------
// Purpose: Safe method
//-----------------------------------------------------------------------------
CBasePlayer	*CBasePlayerItem::GetOwner(void) const
{
	return m_pPlayer;
}

//-----------------------------------------------------------------------------
// Purpose: Safe method
// Output : const int
//-----------------------------------------------------------------------------
/*const int &CBasePlayerItem::GetID(void) const
{
	return m_iId;
}*/









//-----------------------------------------------------------------------------
// Purpose: Default Spawn, should never be used?
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::Spawn(void)
{
	Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: Every subclassed weapon must call this in Spawn()!
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::Initialize(void)
{
	m_iPrimaryAmmoType = GetAmmoIndexFromRegistry(pszAmmo1());
	m_iSecondaryAmmoType = GetAmmoIndexFromRegistry(pszAmmo2());
	CBasePlayerItem::Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: Called through the newly-touched weapon's instance. The existing player weapon is pOriginal
// Input  : *pOriginal - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::AddDuplicate(CBasePlayerItem *pOriginal)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerWeapon(%d)::AddDuplicate(%d) id %d\n", entindex(), pOriginal?pOriginal->entindex():0, GetID());
#endif
	// extract ammo from weapon to inventory
	if (m_iDefaultAmmo)
		return ExtractAmmo(pOriginal->GetWeaponPtr());
	else// a dead player dropped this.
		return ExtractClipAmmo(pOriginal->GetWeaponPtr());
}

//-----------------------------------------------------------------------------
// Purpose: Called from player to check&add this weapon to inventory. this moved to inventory, no duplication.
// Input  : *pPlayer - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::AddToPlayer(CBasePlayer *pPlayer)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerWeapon(%d)::AddToPlayer(%d %s) (id %d)\n", entindex(), pPlayer->entindex(), STRING(pPlayer->pev->netname), GetID());
	if (GetOwner() != NULL)
	{
		ALERT(at_console, "CBasePlayerWeapon(%d)::AddToPlayer(%d %s) (id %d) owned by %d %s!\n", entindex(), pPlayer->entindex(), STRING(pPlayer->pev->netname), GetID(), GetOwner()->entindex(), STRING(GetOwner()->pev->netname));
	}
#endif

	int bResult = CBasePlayerItem::AddToPlayer(pPlayer);
	if (bResult)
	{
		// update ammo indexes
		m_iPrimaryAmmoType = GetAmmoIndexFromRegistry(pszAmmo1());
		m_iSecondaryAmmoType = GetAmmoIndexFromRegistry(pszAmmo2());

		// extract ammo from weapon to inventory
		if (m_iDefaultAmmo)
			/*return */ ExtractAmmo(this);
		else// a dead player dropped this.
			/*return */ ExtractClipAmmo(this);

		return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Can this weapon perform attack? (based on many restrictions)
// Warning: Overloaded CanAttack in all derived classes must call this one!
// Input  : &attack_time - time to check - m_flNextSecondaryAttack, etc.
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::CanAttack(const float &attack_time)
{
	if (GetOwner())
	{
		if (m_pPlayer->CanAttack() == FALSE)
			return FALSE;

		if (IsHolstered())// XDM3035: new
			return FALSE;

		if (g_pGameRules->IsMultiplayer())
			if (mp_noshooting.value > 0.0f)
				return FALSE;
	}
	else// XDM3035a: if monsters will be able to hold weapons, this code should be revisited
		return FALSE;

	return (attack_time <= UTIL_WeaponTimeBase()) ? TRUE : FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: called each frame by the player PostThink
// XDM3035: I'm amazed it works
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::ItemPostFrame(void)
{
//	ALERT(at_console, "CBasePlayerWeapon(%d id %d)::ItemPostFrame()\n", entindex(), GetID());
	ASSERT(m_pPlayer != NULL);
	if (GetOwner() == NULL)
	{
		ALERT(at_console, "CBasePlayerWeapon(%d)::ItemPostFrame(): weapon %d %s has no owner!\n", entindex(), GetID(), STRING(pev->classname));
		return;
	}
	if (pev->flags & FL_KILLME)
	{
		ALERT(at_console, "CBasePlayerWeapon(%d)::ItemPostFrame(): weapon %d %s was removed!\n", entindex(), GetID(), STRING(pev->classname));
		return;
	}
#ifdef _DEBUG_ITEMS
	if (m_pPlayer->m_pActiveItem != this)
	{
		ALERT(at_console, "CBasePlayerWeapon(%d)::ItemPostFrame(): weapon %d %s is not active!\n", entindex(), GetID(), STRING(pev->classname));
	}
#endif

	if (IsHolstered())// XDM3035: TESTME
	{
		if (m_pPlayer->pev->button & (IN_ATTACK|IN_ATTACK2))// XDM3035b: if the weapon is holstered, player may try to draw it again
		{
			if (CanDeploy())
				Deploy();
		}
		return;
	}

	if (m_fInReload > 0 && (m_pPlayer->m_flNextAttack <= UTIL_WeaponTimeBase()))
	{
		if (iMaxClip() != -1)// complete the reload
		{
			int loadammo = min(iMaxClip() - m_iClip, m_pPlayer->AmmoInventory(m_iPrimaryAmmoType));
			// Add them to the clip
			m_iClip += loadammo;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= loadammo;
			m_pPlayer->TabulateAmmo();
		}
		m_fInReload = 0;
	}

	if ((m_pPlayer->pev->button & IN_ATTACK2) && CanAttack(m_flNextSecondaryAttack))// Player can do SecondaryAttack
	{
		if (pszAmmo2() && m_pPlayer->AmmoInventory(SecondaryAmmoIndex()) <= 0)// dont' !HasAmmo(AMMO_SECONDARY)
			m_fFireOnEmpty = TRUE;

		m_pPlayer->TabulateAmmo();
		SecondaryAttack();
//		if (GetOwner())// XDM3035a: m_pPlayer may be NULL at this point!
//			m_pPlayer->pev->button &= ~IN_ATTACK2;// XDM3035: TODO CHECK (do we need this line?)
	}
	else if ((m_pPlayer->pev->button & IN_ATTACK) && CanAttack(m_flNextPrimaryAttack))// Player can do PrimaryAttack
	{
		if ((m_iClip == 0 && pszAmmo1()) || (iMaxClip() == -1 && m_pPlayer->AmmoInventory(PrimaryAmmoIndex()) <= 0))
			m_fFireOnEmpty = TRUE;

		m_pPlayer->TabulateAmmo();
		PrimaryAttack();
	}
	else if (m_pPlayer->pev->button & IN_RELOAD && iMaxClip() != -1 && m_fInReload == 0)// Player can do Reload
	{
		Reload();// reload when reload is pressed, or if no buttons are down and weapon is empty.
	}
	else if (!(m_pPlayer->pev->button & (IN_ATTACK|IN_ATTACK2)))// XDM3035: can omit this condition, but it's better this way
	{
		m_fFireOnEmpty = FALSE;// no fire buttons down

		if (m_fInReload == FALSE)// XDM3035
		{
			if (!IsUseable() && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())// weapon isn't useable, switch.
			{
				if (!(iFlags() & ITEM_FLAG_NOAUTOSWITCHEMPTY) && m_pPlayer->SelectNextBestItem(this))
				{
					m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3f;
					return;
				}
			}
			else// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			{
				if (iMaxClip() != -1)// XDM3035b
				{
					if (m_iClip == 0 && !(iFlags() & ITEM_FLAG_NOAUTORELOAD) && m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
					{
						Reload();
						return;
					}
				}
			}
		}
		WeaponIdle();
		return;
	}
	// catch all
	if (GetOwner() && ShouldWeaponIdle())
		WeaponIdle();
}

//-----------------------------------------------------------------------------
// Purpose: Client-side ONLY
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::ClientPostFrame(local_state_s *from, local_state_s *to, usercmd_t *cmd, const double &time, const unsigned int &random_seed)
{
}

//-----------------------------------------------------------------------------
// Purpose: Server-side ONLY. Pack data.
// Input  : *player - 
//			*info - 
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::ClientPackData(struct edict_s *player, struct weapon_data_s *weapondata)
{
	weapondata->m_iId					= GetID();
	weapondata->m_iClip					= m_iClip;
	weapondata->m_flTimeWeaponIdle		= max(m_flTimeWeaponIdle, -0.001);
	weapondata->m_flNextPrimaryAttack	= max(m_flNextPrimaryAttack, -0.001);
	weapondata->m_flNextSecondaryAttack	= max(m_flNextSecondaryAttack, -0.001);
	weapondata->m_fInReload				= m_fInReload;
	weapondata->m_fInSpecialReload		= m_fInSpecialReload;
	weapondata->fuser1					= max(pev->fuser1, -0.001);
//	weapondata->iuser3					= m_fireState;
	weapondata->m_flPumpTime			= max(m_flPumpTime, -0.001);
}

//-----------------------------------------------------------------------------
// Purpose: sends hud info to client dll, if things have changed
// Input  : *pBuffer - common buffer for all updating weapons
// Output : int - buffer length generated by this function
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::UpdateClientData(char *pBuffer)
{
	if (m_pPlayer == NULL)
	{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerWeapon(%d)::UpdateClientData(%s) (id %d) has no owner!\n", entindex(), pBuffer, GetID());
#endif
		return 0;
	}
	int iSize = 0;
	bool bSend = false;
	byte state = 0;
	if (m_pPlayer->m_pActiveItem == this)
	{
		if (m_pPlayer->m_fOnTarget)
			state = 2;
		else
			state = 1;
	}
	// Forcing send of all data!
	if (!m_pPlayer->m_fWeapon)
		bSend = true;

	// This is the current or last weapon, so the state will need to be updated
	if (this == m_pPlayer->m_pActiveItem || this == m_pPlayer->m_pClientActiveItem)
	{
		if (m_pPlayer->m_pActiveItem != m_pPlayer->m_pClientActiveItem)
			bSend = true;
	}

	// If the ammo, state, or fov has changed, update the weapon
	if (m_iClip != m_iClientClip || state != m_iClientWeaponState || m_pPlayer->m_iFOV != m_pPlayer->m_iClientFOV)
		bSend = true;

	if (bSend)
	{
		if (pBuffer)// XDM3035: new method: single buffer for all updates
		{
			byte value = state;
			memcpy(pBuffer+iSize, &value, sizeof(byte)); ++iSize;// +=sizeof(byte)
			value = m_iId;
			memcpy(pBuffer+iSize, &value, sizeof(byte)); ++iSize;

			if (iMaxClip() == -1 && m_iClip == 0)// XDM3034: HACK?
				value = 255;
			else
				value = m_iClip;

			memcpy(pBuffer+iSize, &value, sizeof(byte)); ++iSize;
		}
		else// old method: separate messages
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgCurWeapon, NULL, m_pPlayer->edict());
				WRITE_BYTE(state);
				WRITE_BYTE(m_iId);
			if (iMaxClip() == -1 && m_iClip == 0)// XDM3034: HACK?
				WRITE_BYTE(-1);// absurd?
			else
				WRITE_BYTE(m_iClip);

			MESSAGE_END();
			iSize = 3;
		}
		m_iClientClip = m_iClip;
		m_iClientWeaponState = state;
		m_pPlayer->m_fWeapon = TRUE;
	}
	return iSize;
}

//-----------------------------------------------------------------------------
// Purpose: Send weapon animation to client
// Input  : &iAnim - 
//			&body - 
//			skiplocal - 1 if client is predicting weapon animations
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::SendWeaponAnim(const int &iAnim, const int &body, bool skiplocal)
{
	if (m_pPlayer == NULL)// XDM3035
		return;

	if (UseDecrement())
		skiplocal = 1;
	else
		skiplocal = 0;

	m_pPlayer->pev->weaponanim = iAnim;

	MESSAGE_BEGIN(MSG_ONE, SVC_WEAPONANIM, NULL, m_pPlayer->edict());
		WRITE_BYTE(iAnim);		// sequence number
		WRITE_BYTE(pev->body);	// weaponmodel bodygroup.
	MESSAGE_END();
}

//-----------------------------------------------------------------------------
// Purpose: Add primary ammo to me/my owner (primary ammo has clip)
// Input  : &iCount - 
//			&iAmmoIndex - 
// Output : Returns the amount of ammo actually added.
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::AddPrimaryAmmo(const int &iCount, const int &iAmmoIndex)//, const int &iMaxClip, const int &iMaxCarry)
{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerWeapon(%d id %d)::AddPrimaryAmmo(%d %d %d %d)\n", entindex(), GetID(), iCount, iAmmoIndex, iMaxClip(), iMaxAmmo1());
#endif
	if (GetOwner() == NULL)
		return 0;
	if (iAmmoIndex != PrimaryAmmoIndex())
	{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerWeapon(%d id %d)::AddPrimaryAmmo() ammmo ID mismatch! %d %d\n", entindex(), GetID(), iAmmoIndex, PrimaryAmmoIndex());
#endif
		return 0;
	}

	int added = 0;
	int addtoinventory = iCount;

	if ((iMaxClip() != -1) && m_iClip == 0)// fill the clip first, if it's empty
	{
		if (!(m_pPlayer->pev->button & (IN_ATTACK|IN_ATTACK2)))// XDM3035b: things get ugly on weapons that use clip for charging
		{
			int addtoclip = min(iMaxClip() - m_iClip, addtoinventory);// fill the clip to MAX or take what iCount can offer
			if (addtoclip > 0)
			{
				m_iClip += addtoclip;
				added += addtoclip;
				addtoinventory -= addtoclip;
			}
		}
	}

	added += m_pPlayer->GiveAmmo(addtoinventory, iAmmoIndex, iMaxAmmo1());// now fill player's inventory

/*	if (added > 0)// ?? >= 0)
	{
//WTF?!		m_iPrimaryAmmoType = iIdAmmo;
		if (m_pPlayer->GetInventoryItem(GetID()))
		{
			// play the "got ammo" sound only if we gave some ammo to a player that already had this gun.
			// if the player is just getting this gun for the first time, DefaultTouch will play the "picked up gun" sound for us.
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM);
		}
	}*/
	return added;
}

//-----------------------------------------------------------------------------
// Purpose: Add primary ammo to me/my owner
// Input  : &iCount - 
//			&iAmmoIndex - 
// Output : Returns the amount of ammo actually added
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::AddSecondaryAmmo(const int &iCount, const int &iAmmoIndex)//, const int &iMaxCarry)
{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerWeapon(%d id %d)::AddSecondaryAmmo(%d %d %d)\n", entindex(), GetID(), iCount, iAmmoIndex, iMaxAmmo2());
#endif
	if (GetOwner() == NULL)// XDM3035
		return FALSE;
	if (iAmmoIndex != SecondaryAmmoIndex())
	{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerWeapon(%d id %d)::AddSecondaryAmmo() ammmo ID mismatch! %d %d\n", entindex(), GetID(), iAmmoIndex, SecondaryAmmoIndex());
#endif
		return FALSE;
	}

	if (iCount <= 0)// XDM: prevent player from picking up MP5 when he has full primary and SOME secondary ammo
		return FALSE;

	int added = m_pPlayer->GiveAmmo(iCount, iAmmoIndex, iMaxAmmo2());
/*	if (added > 0)
	{
//WTF?!		m_iSecondaryAmmoType = iIdAmmo;
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM);
	}*/
	return added;// XDM3035a: can be 0!
}

//-----------------------------------------------------------------------------
// Purpose: Can be used, hypotethically
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::IsUseable(void)// XDM
{
	if (pszAmmo1() == NULL && pszAmmo2() == NULL)
		return TRUE;// this weapon doesn't use ammo, can always deploy.

	return HasAmmo(AMMO_ANYTYPE);// now it is safe and usable
}

//-----------------------------------------------------------------------------
// Purpose: Can be deployed at this moment
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::CanDeploy(void)
{
	if (m_pPlayer == NULL)
		return FALSE;

	if (mp_laddershooting.value <= 0.0f)// XDM3035: TESTME
	{
		if (m_pPlayer->IsOnLadder())
			return FALSE;
	}

	if (m_pPlayer->m_fFrozen)
		return FALSE;

	return IsUseable();// is this a good thing to do?
}

//-----------------------------------------------------------------------------
// Purpose: DefaultDeploy, used by all weapons everytime they are selected
// Input  : *szViewModel - 
//			*szWeaponModel - 
//			iAnim - 
//			*szAnimExt - 
//			skiplocal - 1 if client is predicting weapon animations
//			body - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::DefaultDeploy(const char *szViewModel, const char *szWeaponModel, int iViewAnim, const char *szAnimExt, int body)
{
	if (!CanDeploy())
		return FALSE;

	// remember default models
	m_iModelIndexView = MODEL_INDEX(szViewModel);
	m_iModelIndexWorld = MODEL_INDEX(szWeaponModel);

	m_pPlayer->TabulateAmmo();
	m_pPlayer->pev->viewmodel = MAKE_STRING(szViewModel);// XDM: WTF IS THIS?! These should be model indexes, not string pointers!!!
	m_pPlayer->pev->weaponmodel = MAKE_STRING(szWeaponModel);

	m_pPlayer->SetWeaponAnimType(szAnimExt);
	SendWeaponAnim(iViewAnim, body);
	m_pPlayer->SetAnimation(PLAYER_ARM);

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.25f;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;
	return TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: Start reload process
// Input  : &iClipSize - 
//			&iAnim - 
//			&fDelay - 
//			&body - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::DefaultReload(const int &iClipSize, const int &iAnim, const float &fDelay, const int &body)
{
	if (m_pPlayer == NULL)// XDM3035
		return FALSE;

	int hasammo = m_pPlayer->AmmoInventory(m_iPrimaryAmmoType);
	if (hasammo <= 0 || iMaxClip() == -1)// XDM3034: TESTME
		return FALSE;

	int loadammo = min(iClipSize - m_iClip, hasammo);
	if (loadammo <= 0)
		return FALSE;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + fDelay;

	//!!UNDONE -- reload sound goes here !!!
	SendWeaponAnim(iAnim, body/*, UseDecrement()*/);


	m_fInReload = TRUE;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + fDelay;// XDM3035: WAS + 3;
	return TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: ?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::PlayEmptySound(void)
{
	if (m_iPlayEmptySound)
	{
//		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dryfire1.wav", 0.8, ATTN_NORM);
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/dryfire1.wav", VOL_NORM, ATTN_STATIC);// XDM
		m_iPlayEmptySound = 0;
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: ?
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::ResetEmptySound(void)
{
	m_iPlayEmptySound = 1;
}

//-----------------------------------------------------------------------------
// Purpose: Overloaded access to ammo index
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::PrimaryAmmoIndex(void)// const
{
	return m_iPrimaryAmmoType;
}

//-----------------------------------------------------------------------------
// Purpose: Overloaded access to ammo index
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::SecondaryAmmoIndex(void)// const
{
	return m_iSecondaryAmmoType;// XDM: was -1
}

//-----------------------------------------------------------------------------
// Purpose: Holster, put away. Also called when selecting another weapon.
// Input  : skiplocal - 1 if client is predicting weapon animations
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::Holster(int skiplocal /* = 0 */)
{
	m_fInReload = FALSE;// cancel any reload in progress.
	m_fInSpecialReload = FALSE;
	m_flPumpTime = 0;
	m_flNextAmmoBurn = 0;
// ?	m_flNextPrimaryAttack = 0;
//	m_flNextSecondaryAttack = 0;
	m_iPlayEmptySound = 0;// XDM3035


	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0f;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;
	SendWeaponAnim(TYRIAN_WEAPON_ANIM_HOLSTER);

	m_pPlayer->SetAnimation(PLAYER_DISARM);

	if (m_pPlayer)
	{

// allow the animation to finish	m_pPlayer->pev->viewmodel = 0;
		m_pPlayer->pev->weaponmodel = 0;// XDM3035: we use this to determine weapon IsHolstered state
	}
/* TODO: TESTME
	if (iFlags() & ITEM_FLAG_EXHAUSTIBLE)
		if (m_pPlayer->AmmoInventory(m_iPrimaryAmmoType) == 0)
				DestroyItem();
*/
}

//-----------------------------------------------------------------------------
// Purpose: Extract ammo from pWeapon to this
// called by the new item with the existing item as parameter
//
// if we call ExtractAmmo(), it's because the player is picking up this type of weapon for
// the first time. If it is spawned by the world, m_iDefaultAmmo will have a default ammo amount in it.
// if this is a weapon dropped by a dying player, has 0 m_iDefaultAmmo, which means only the ammo in
// the weapon clip comes along.
//
// Input  : *pWeapon - extract ammo from
// Output : int amount of ammo extracted
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::ExtractAmmo(CBasePlayerWeapon *pWeapon)
{
	int iReturn = 0;
	if (pWeapon)
	{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerWeapon(%d id %d)::ExtractAmmo(%d id %d)\n", entindex(), GetID(), pWeapon->entindex(), pWeapon->GetID());
#endif
		if (pszAmmo1())
		{
			// Blindly call with m_iDefaultAmmo. It's either going to be a value or zero.
			// If it is zero, we only get the ammo in the weapon's clip, which is what we want.
			iReturn = pWeapon->AddPrimaryAmmo(m_iDefaultAmmo/*>0?m_iDefaultAmmo:m_iClip*/, PrimaryAmmoIndex());// depends on Initialize()!
//			iReturn = pWeapon->AddPrimaryAmmo(m_iDefaultAmmo, /*won't work!PrimaryAmmoIndex()*/GetAmmoIndexFromRegistry(pszAmmo1()));//, iMaxClip(), iMaxAmmo1());
			// ammo_left = m_iDefaultAmmo - iReturn;
			// WTF IS THIS?! m_iDefaultAmmo = 0;
		}
		if (pszAmmo2())// since THIS weapon has no ammo2 in it, why is this needed anyway?
			iReturn += pWeapon->AddSecondaryAmmo(0, SecondaryAmmoIndex());// depends on Initialize()!
//			iReturn += pWeapon->AddSecondaryAmmo(0, /*won't work!SecondaryAmmoIndex()*/GetAmmoIndexFromRegistry(pszAmmo2()));//, iMaxAmmo2());

		if (iReturn > 0)
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM);
	}
#ifdef _DEBUG_ITEMS
	else
	{
	ALERT(at_console, "CBasePlayerWeapon(%d id %d)::ExtractAmmo(NULL)!\n", entindex(), GetID());
	}
#endif
	return iReturn;
}

//-----------------------------------------------------------------------------
// Purpose: Extract ammo from pWeapon to this
// called by the new item's class with the existing item as parameter
// Input  : *pWeapon - extract ammo from
// Output : int amount of ammo extracted
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::ExtractClipAmmo(CBasePlayerWeapon *pWeapon)
{
	int iReturn = 0;
	if (pWeapon)
	{
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayerWeapon(%d id %d)::ExtractClipAmmo(%d id %d)\n", entindex(), GetID(), pWeapon->entindex(), pWeapon->GetID());
#endif
		int iAmmo = 0;
		if (iMaxAmmo1() > 0)// pszAmmo1() // weapon actually (theoretically) uses ammo
		{
			iAmmo = m_iClip;
			m_iClip = 0;// XDM3035b: TESTME!
		}

		if (iAmmo <= 0)
			return 1;// don't flood the world, pick up this useless weapon

		iReturn = pWeapon->m_pPlayer->GiveAmmo(iAmmo, (char *)pszAmmo1(), iMaxAmmo1());// pass only ammo name so the recieving player will determine his own ammo index
//TEST		return pWeapon->m_pPlayer->GiveAmmo(iAmmo, GetAmmoIndex(pszAmmo1()), iMaxAmmo1()); // , &m_iPrimaryAmmoType

		if (iReturn > 0)
		{
			if (iReturn < iAmmo)// inventory accepted less ammo than we offered
			{
				if (iMaxClip() != -1)// try to add resto into the clip
					m_iClip += min(iMaxClip(), iAmmo - iReturn);
			}
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", VOL_NORM, ATTN_NORM);
		}
	}
	return iReturn;
}

//-----------------------------------------------------------------------------
// Purpose: no more ammo for this gun, put it away.
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::RetireWeapon(void)
{
//	ALERT(at_aiconsole, "RetireWeapon();\n");
	Holster();// XDM!!! IMPORTANT: CPlasma depends on this

	// first, no viewmodel at all.
	if (m_pPlayer)
	{
		m_pPlayer->pev->viewmodel = 0;
		m_pPlayer->pev->weaponmodel = 0;
		m_pPlayer->SelectNextBestItem(this);
	}

	if (iFlags() & ITEM_FLAG_EXHAUSTIBLE)
		DestroyItem();// this clears m_pPlayer, so call it after
}

//-----------------------------------------------------------------------------
// Purpose: Caller may explicitly specify:
// Input  : type - primary/secondary/any type to check (flags)
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::HasAmmo(byte type)
{
	if (m_pPlayer)
	{
//AMMO_ANYTYPE is 0		if (type & (AMMO_CLIP | AMMO_ANYTYPE))
		if (type == AMMO_ANYTYPE || (type & AMMO_CLIP))
		{
			if (iMaxClip() != -1 && m_iClip > 0)// m_iClip may indicate something else!
				return TRUE;
		}
	}
	return CBasePlayerItem::HasAmmo(type);
}

//-----------------------------------------------------------------------------
// Purpose: Use some ammo
// Input  : type - AMMO_PRIMARY or AMMO_SECONDARY only!
// Input  : count - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayerWeapon::UseAmmo(byte type, int count)
{
	if (m_pPlayer == NULL)
		return 0;

	int iAmmo;
	if (type == AMMO_PRIMARY)
		iAmmo = m_iPrimaryAmmoType;
	else if (type == AMMO_SECONDARY)
		iAmmo = m_iSecondaryAmmoType;
	else
		return 0;

	if (m_pPlayer->AmmoInventory(iAmmo) >= count)
	{
		m_pPlayer->m_rgAmmo[iAmmo] -= count;
		return count;
	}
	else
		m_pPlayer->m_rgAmmo[iAmmo] = 0;

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Debugging
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::PrintState(void)
{
	ALERT(at_console, "ID %d %s: inreload: %d, ammo1(%d): %d, ammo2(%d): %d, clip: %d, default: %d\n",
		m_iId, STRING(pev->classname), m_fInReload,
		PrimaryAmmoIndex(), m_pPlayer->AmmoInventory(PrimaryAmmoIndex()),
		SecondaryAmmoIndex(), m_pPlayer->AmmoInventory(SecondaryAmmoIndex()),
		m_iClip, m_iDefaultAmmo);
/*	ALERT(at_console, "%d %s  inreload: %d\n ammo1 %s(%d): %d, ammo2 %s(%d): %d, clip: %d\n nextpri: %f nextsec: %f nextidle: %f\n",
		m_iId, STRING(pev->classname), m_fInReload,
		CBasePlayerItem::AmmoInfoArray[-1].pszName, PrimaryAmmoIndex(), m_pPlayer->AmmoInventory(PrimaryAmmoIndex()),
		CBasePlayerItem::AmmoInfoArray[-1].pszName, SecondaryAmmoIndex(), m_pPlayer->AmmoInventory(SecondaryAmmoIndex()),
		m_iClip, m_flNextPrimaryAttack, m_flNextSecondaryAttack, m_flTimeWeaponIdle);*/
}

//-----------------------------------------------------------------------------
// Purpose: CLIENT WEAPONS
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::UseDecrement(void)
{
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: XDM3038: distance at which a projectile must be created
// Output : float - forward distance
//-----------------------------------------------------------------------------
float CBasePlayerWeapon::GetBarrelLength(void) const
{
	return (HULL_RADIUS-1.0f);// this should NEVER be far enough to stick through a thin wall!
}
