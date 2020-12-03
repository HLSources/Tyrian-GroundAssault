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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "pm_defs.h"
#include "event_api.h"
#include "r_efx.h"
#include "com_weapons.h"
#include "cl_dll.h"
#include "hud.h"

// XDM: right now this code is outdated and useless

//extern globalvars_t *gpGlobals;
//extern int g_iUser1;
// Pool of client side entities/entvars_t
static entvars_t ev[PLAYER_INVENTORY_SIZE];
static int num_ents = 0;
// The entity we'll use to represent the local client
static CBasePlayer player;
// Local version of game .dll global variables ( time, etc. )
static globalvars_t	Globals; 
static CBasePlayerWeapon *g_pWpns[PLAYER_INVENTORY_SIZE];

static vec3_t previousorigin;

cvar_t *g_psv_zmax = NULL;// XDM

/*
=====================
HUD_PrepEntity

Links the raw entity to an entvars_s holder.  If a player is passed in as the owner, then
we set up the m_pPlayer field.
=====================
*/
void HUD_PrepEntity(CBaseEntity *pEntity, CBasePlayer *pWeaponOwner)
{
	memset(&ev[num_ents], 0, sizeof(entvars_t));
	CBasePlayerWeapon *pWeapon = (CBasePlayerWeapon *)pEntity;
	pWeapon->pev = &ev[num_ents++];
	pWeapon->Precache();
	pWeapon->Spawn();
	if (pWeaponOwner)
	{
//		ItemInfo info;
		pWeapon->SetOwner(pWeaponOwner);
//		pWeapon->GetItemInfo(&info);
		g_pWpns[pWeapon->GetID()] = pWeapon;
//		g_pWpns[info.iId] = (CBasePlayerWeapon *)pEntity;
	}
}

//Just loads a v_ model.
void LoadVModel(char *szViewModel, CBasePlayer *m_pPlayer)
{
	/*model_t *pModel = */gEngfuncs.CL_LoadModel(szViewModel, &m_pPlayer->pev->viewmodel);
//	pModel->
}


int AddAmmoToRegistry(const char *szAmmoName, int iMaxCarry)
{
	return 0;
}







//-----------------------------------------------------------------------------
// Purpose: Lots of CBasePlayerItem stubs
//-----------------------------------------------------------------------------


void CBasePlayerItem::SetOwner(CBasePlayer *pPlayer)
{
/*	if (pPlayer == NULL)
	{
		ALERT(at_aiconsole, "CBasePlayerItem::SetOwner(NULL) %d\n", m_iId);
	}*/
	m_pPlayer = pPlayer;
}
/*
void CBasePlayerItem::SetID(const int &ID)
{
	m_iId = ID;
}
*/
CBasePlayer	*CBasePlayerItem::GetOwner(void) const
{
	return m_pPlayer;
}
/*
const int &CBasePlayerItem::GetID(void) const
{
	return m_iId;
}*/

//-----------------------------------------------------------------------------
// Purpose: Default Spawn, never get here?
//-----------------------------------------------------------------------------
void CBasePlayerItem::Spawn(void)// XDM
{
	Precache();
	pev->takedamage = DAMAGE_NO;
	pev->nextthink = gpGlobals->time + 0.1;
	m_pPlayer = NULL;// XDM3035a
	Initialize();
	pev->scale = UTIL_GetWeaponWorldScale();// XDM3035b
	CBaseAnimating::Spawn();// XDM: starts animation
#ifdef _DEBUG
	ALERT(at_aiconsole, "CBasePlayerItem::Spawn()\n");
#endif
//	Drop();// XDM: ?
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayerItem::Precache(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: A must
//-----------------------------------------------------------------------------
void CBasePlayerItem::Initialize(void)
{
	m_iModelIndexView = 0;
	m_iModelIndexWorld = pev->modelindex;
	FallInit();
}

int CBasePlayerItem::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType) { return 0; }
void CBasePlayerItem::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)// XDM3035c
{
	Kill();
}
int CBasePlayerItem::ShouldCollide(CBaseEntity *pOther) { return 1; }
int CBasePlayerItem::Restore(class CRestore &) { return 1; }
int CBasePlayerItem::Save(class CSave &) { return 1; }
void CBasePlayerItem::SetObjectCollisionBox(void) { }
void CBasePlayerItem::FallInit(void) { }
void CBasePlayerItem::FallThink(void) { }
void CBasePlayerItem::Materialize(void) { }
void CBasePlayerItem::AttemptToMaterialize(void) { }
CBaseEntity *CBasePlayerItem::StartRespawn(void) { return NULL; }

//-----------------------------------------------------------------------------
// Purpose: Think that calls Kill()
//-----------------------------------------------------------------------------
void CBasePlayerItem::DestroyItem(void)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayerItem(%d)::DestroyItem()(id %d)\n", entindex(), GetID());
#endif

/*	try
	{
	if (m_pPlayer)// if attached to a player, remove.
		m_pPlayer->RemovePlayerItem(this);
	}
	catch (...)
	{
		printf("*** CBasePlayerItem(%d)::DestroyItem() exception!\n", GetID());
		DBG_FORCEBREAK
	}*/
	SetOwner(NULL);
	Kill();
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

//void CBasePlayerItem::Drop(void) { }
void CBasePlayerItem::Kill(void)
{
	pev->flags = FL_KILLME;
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
//#ifdef _DEBUG_ITEMS
	else
		ALERT(at_console, "CBasePlayerItem(%d)::AttachToPlayer(%d %s) (id %d)\n", entindex(), pPlayer->entindex(), STRING(pPlayer->pev->netname), GetID());
//#endif

	SetTouchNull();
	SetOwner(pPlayer);// XDM3035a: WTF?!?!?!
	pev->origin = pPlayer->pev->origin;
	pev->movetype = MOVETYPE_FOLLOW;
//	pev->flags |= FL_DORMANT;// XDM3035b: test!
	pev->solid = SOLID_NOT;
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
	if ((pev->spawnflags & SF_NORESPAWN) && !(pev->effects & EF_NODRAW))// dropped by monster AND not picked up and carried
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

int CBasePlayerItem::PrimaryAmmoIndex(void)
{
	return GetAmmoIndexFromRegistry(this->pszAmmo1());
}

int CBasePlayerItem::SecondaryAmmoIndex(void)
{
	return GetAmmoIndexFromRegistry(this->pszAmmo2());
}

void CBasePlayerItem::OnFreePrivateData(void)// XDM3035
{
//	m_iId = 0;// TEST
//	m_pPlayer = NULL;
}






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

int CBasePlayerWeapon::Restore(class CRestore &) { return 1; }
int CBasePlayerWeapon::Save(class CSave &) { return 1; }
int CBasePlayerWeapon::AddDuplicate(CBasePlayerItem *pOriginal) { return 0; }
int CBasePlayerWeapon::AddToPlayer(CBasePlayer *pPlayer)
{
	return CBasePlayerItem::AddToPlayer(pPlayer);
}
int CBasePlayerWeapon::UpdateClientData(char *pBuffer) { return 0; }

//-----------------------------------------------------------------------------
// Purpose: 
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
// Purpose: Can be deployed at this moment
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::CanDeploy(void)
{
	if (m_pPlayer == NULL)
		return FALSE;

	if (m_pPlayer->m_fFrozen)
		return FALSE;

	if (pszAmmo1() == NULL && pszAmmo2() == NULL)
		return TRUE;// this weapon doesn't use ammo, can always deploy.

	return HasAmmo(AMMO_ANYTYPE);// now it is safe and usable
}

BOOL CBasePlayerWeapon::DefaultDeploy(const char *szViewModel, const char *szWeaponModel, int iViewAnim, const char *szAnimExt, int body)
{
	if (!CanDeploy())
		return FALSE;

	// remember default models
	m_iModelIndexView = MODEL_INDEX(szViewModel);
	m_iModelIndexWorld = MODEL_INDEX(szWeaponModel);

	m_pPlayer->TabulateAmmo();
	gEngfuncs.CL_LoadModel(szViewModel, &m_pPlayer->pev->viewmodel);
	m_pPlayer->pev->viewmodel = MAKE_STRING(szViewModel);// XDM: WTF IS THIS?! These should be model indexes, not string pointers!!!
	m_pPlayer->pev->weaponmodel = MAKE_STRING(szWeaponModel);

	m_pPlayer->SetWeaponAnimType(szAnimExt);
	SendWeaponAnim(iViewAnim, body);

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5f;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0f;
	return TRUE;
}

BOOL CBasePlayerWeapon::PlayEmptySound(void)
{
	if (m_iPlayEmptySound)
	{
		HUD_PlaySound("weapons/dryfire1.wav", 0.8);
		m_iPlayEmptySound = 0;
		return 0;
	}
	return 0;
}

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

	if (m_pPlayer)
	{
//		if (m_pPlayer->pev->deadflag == DEAD_NO)
//			m_pPlayer->SetAnimation(PLAYER_DISARM);// unused, let it be

// allow the animation to finish	m_pPlayer->pev->viewmodel = 0;
		m_pPlayer->pev->weaponmodel = 0;// XDM3035: we use this to determine weapon IsHolstered state
	}
/* TODO: TESTME
	if (iFlags() & ITEM_FLAG_EXHAUSTIBLE)
		if (m_pPlayer->AmmoInventory(m_iPrimaryAmmoType) == 0)
				DestroyItem();
*/
}

void CBasePlayerWeapon::SendWeaponAnim(const int &iAnim, const int &body, bool skiplocal)
{
/*	if (m_pPlayer == NULL)// XDM3035
		return;

	if (UseDecrement())
		skiplocal = 1;
	else
		skiplocal = 0;
*/
	m_pPlayer->pev->weaponanim = iAnim;
	HUD_SendWeaponAnim(iAnim, body, 0);
}


void CBasePlayerWeapon::ItemPostFrame(void)
{
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
#if 0 // FIXME, need ammo on client to make this work right
			int loadammo = min(iMaxClip() - m_iClip, m_pPlayer->AmmoInventory(m_iPrimaryAmmoType));
			// Add them to the clip
			m_iClip += loadammo;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= loadammo;
			m_pPlayer->TabulateAmmo();
#else	
			m_iClip += 10;
#endif
		}
		m_fInReload = FALSE;
	}

	if ((m_pPlayer->pev->button & IN_ATTACK2) && (m_flNextSecondaryAttack <= UTIL_WeaponTimeBase()))
	{
		if (pszAmmo2() && m_pPlayer->AmmoInventory(SecondaryAmmoIndex()) <= 0)
			m_fFireOnEmpty = TRUE;

		m_pPlayer->TabulateAmmo();
		SecondaryAttack();
//		if (GetOwner())
//			m_pPlayer->pev->button &= ~IN_ATTACK2;
	}
	else if ((m_pPlayer->pev->button & IN_ATTACK) && (m_flNextPrimaryAttack <= UTIL_WeaponTimeBase()))
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
// Purpose: Individual client-side state processing function
// Input  : *from - 
//			*to - 
//			*cmd - 
//			&time - 
//			&random_seed - 
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::ClientPostFrame(local_state_s *from, local_state_s *to, usercmd_t *cmd, const double &time, const unsigned int &random_seed)
{
	weapon_data_t *pfrom = &from->weapondata[GetID()];

	m_fInReload				= pfrom->m_fInReload;
	m_fInSpecialReload		= pfrom->m_fInSpecialReload;
	m_flPumpTime			= pfrom->m_flPumpTime;
	m_iClip					= pfrom->m_iClip;
	m_flNextPrimaryAttack	= pfrom->m_flNextPrimaryAttack;
	m_flNextSecondaryAttack = pfrom->m_flNextSecondaryAttack;
	m_flTimeWeaponIdle		= pfrom->m_flTimeWeaponIdle;
	pev->fuser1				= pfrom->fuser1;
//	m_flStartThrow			= pfrom->fuser2;
//	m_flReleaseThrow		= pfrom->fuser3;
//	m_chargeReady			= pfrom->iuser1;
//	m_fInAttack				= pfrom->iuser2;
//	m_fireState				= pfrom->iuser3;
	m_iSecondaryAmmoType	= (int)from->client.vuser3[2];
	m_iPrimaryAmmoType		= (int)from->client.vuser4[0];
	player.m_rgAmmo[PrimaryAmmoIndex()] = (int)from->client.vuser4[1];
	player.m_rgAmmo[SecondaryAmmoIndex()] = (int)from->client.vuser4[2];
	m_flNextAmmoBurn		= from->client.fuser2;
//	m_flStartCharge			= from->client.fuser3;
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

int CBasePlayerWeapon::AddPrimaryAmmo(const int &iCount, const int &iAmmoIndex) { return iCount; }
int CBasePlayerWeapon::AddSecondaryAmmo(const int &iCount, const int &iAmmoIndex) { return iCount; }
BOOL CBasePlayerWeapon::IsUseable(void) { return TRUE; }
int CBasePlayerWeapon::ExtractAmmo(CBasePlayerWeapon *pWeapon) { return 0; }
int CBasePlayerWeapon::ExtractClipAmmo(CBasePlayerWeapon *pWeapon) { return 0; }	

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
//		g_pGameRules->GetNextBestWeapon(m_pPlayer, this);
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
// Purpose: 
// Input  : &attack_time - 
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

		if (bIsMultiplayer())
			if (gHUD.m_iGameFlags & GAME_FLAG_NOSHOOTING)
				return FALSE;
	}
	else// XDM3035a: if monsters will be able to hold weapons, this code should be revisited
		return FALSE;

		return ( attack_time <= gpGlobals->time ) ? TRUE : FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayerWeapon::UseDecrement(void)
{
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: For debugging, print out state variables to log file
//-----------------------------------------------------------------------------
void CBasePlayerWeapon::PrintState(void)
{
	COM_Log(NULL, "Weapon %d", m_iId);
	COM_Log(NULL, "%.4f ", gpGlobals->time );
	COM_Log(NULL, "%.4f ", m_pPlayer->m_flNextAttack );
	COM_Log(NULL, "%.4f ", m_flNextPrimaryAttack );
	COM_Log(NULL, "%.4f ", m_flTimeWeaponIdle - gpGlobals->time);
	COM_Log(NULL, "%i ", m_iClip );
}











//-----------------------------------------------------------------------------
// Purpose: Does the player already have EXACTLY THIS INSTANCE of item?
// Input  : *pCheckItem - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::HasPlayerItem(CBasePlayerItem *pCheckItem)
{
	if (pCheckItem == NULL)
		return FALSE;

	if (m_rgpWeapons[pCheckItem->GetID()] == pCheckItem)
		return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Safe way to access player's inventory
// Input  : &iID - 
// Output : CBasePlayerItem
//-----------------------------------------------------------------------------
CBasePlayerItem *CBasePlayer::GetInventoryItem(const int &iItemId)
{
	if (iItemId >= 0 && iItemId < PLAYER_INVENTORY_SIZE)
	{
		if (m_rgpWeapons[iItemId])
		{
			if (m_rgpWeapons[iItemId]->GetOwner() != this)
			{
				ALERT(at_console, "CBasePlayer(%d)::GetInventoryItem(%d)(ei %d id %d) bad item owner!\n", entindex(), iItemId, m_rgpWeapons[iItemId]->entindex(), m_rgpWeapons[iItemId]->GetID());
#if defined(_DEBUG_ITEMS)
				DBG_FORCEBREAK
#endif
				m_rgpWeapons[iItemId]->SetOwner(this);
			}

#ifdef _DEBUG
			if (m_rgpWeapons[iItemId]->pev && m_rgpWeapons[iItemId]->GetID() > WEAPON_NONE)
#endif
				return m_rgpWeapons[iItemId];
#if defined(_DEBUG) && defined(_DEBUG_ITEMS)
			else
				ALERT(at_console, "CBasePlayer(%d)::GetInventoryItem(%d)(ei %d id %d) got bad item!\n", entindex(), iItemId, m_rgpWeapons[iItemId]->entindex(), m_rgpWeapons[iItemId]->GetID());
#endif
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::DeployActiveItem(void)// XDM
{
	if (m_pActiveItem)
	{
		m_pActiveItem->Deploy();
// XDM3035b: obsolete		m_pActiveItem->UpdateItemInfo();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pItem - 
//-----------------------------------------------------------------------------
void CBasePlayer::QueueItem(CBasePlayerItem *pItem)// XDM
{
//	if (pItem && pItem->m_iId == WEAPON_NONE)
	if (pItem == NULL || pItem->GetID() == WEAPON_NONE)// XDM3035: wtf?
		return;

	if (!m_pActiveItem)// no active weapon
	{
		m_pActiveItem = pItem;
		return;// just set this item as active
	}
    else// remember active item
	{
		m_pLastItem = m_pActiveItem;
		m_pActiveItem = NULL;// clear current
	}
	m_pNextItem = pItem;// add item to queue
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pItem - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::SelectItem(CBasePlayerItem *pItem)
{
	if (pItem == NULL)
		return FALSE;

	if (!pItem->CanDeploy())// XDM: TESTME!!
		return FALSE;

	if (m_pActiveItem)
	{
		if (pItem == m_pActiveItem)
			return TRUE;

		if (!m_pActiveItem->CanHolster())// XDM
			return FALSE;
	}
	ResetAutoaim();

	if (m_pActiveItem)
		m_pActiveItem->Holster();

	QueueItem(pItem);// XDM
	DeployActiveItem();// XDM
	return TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &iID - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::SelectItem(const int &iID)
{
	CBasePlayerItem *pItem = NULL;
	for (int i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
	{
		if (m_rgpWeapons[i])
		{
			pItem = m_rgpWeapons[i];
			if (pItem->GetID() == iID)
				break;
		}
	}
	return SelectItem(pItem);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pstr - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::SelectItem(const char *pstr)
{
	if (!pstr)
		return FALSE;

	CBasePlayerItem *pItem = NULL;

	for (int i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
	{
		if (m_rgpWeapons[i])
		{
			pItem = m_rgpWeapons[i];
			if (FClassnameIs(pItem->pev, pstr))
				break;
		}
	}
	return SelectItem(pItem);

}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pItem - 
// Output : CBasePlayerItem
//-----------------------------------------------------------------------------
CBasePlayerItem *CBasePlayer::SelectNextBestItem(CBasePlayerItem *pItem)
{
	CBasePlayerItem *pNewItem = NULL;
	if (pItem)
	{
		int id = pItem->GetID();
		int newid;
		for (newid = PLAYER_INVENTORY_SIZE-1; newid >= 0; --newid)
		{
			pNewItem = GetInventoryItem(newid);
			if (pNewItem && newid != id)
				break;
		}
/*		do
		{
			++newid;
			if (newid >= PLAYER_INVENTORY_SIZE)
				newid = 0;

			pNewItem = GetInventoryItem(newid);
		}
		while (pNewItem == NULL && newid != id);*/
	}
	return pNewItem;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::SelectLastItem(void)
{
	if (!m_pLastItem || !m_pLastItem->CanDeploy())// XDM
		return;

	if (m_pActiveItem && !m_pActiveItem->CanHolster())
		return;

	ResetAutoaim();

	if (m_pActiveItem)
		m_pActiveItem->Holster();

	QueueItem(m_pLastItem);// XDM
	DeployActiveItem();// XDM
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pInflictor - 
//			*pAttacker - 
//			iGib - 
//-----------------------------------------------------------------------------
void CBasePlayer::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)
{
	// Holster weapon immediately, to allow it to cleanup
	if (m_pActiveItem)
		m_pActiveItem->Holster();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : restore - 
//-----------------------------------------------------------------------------
void CBasePlayer::Spawn(byte restore)
{
	Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::Spawn(void)
{
	if (m_pActiveItem)
		m_pActiveItem->Deploy();
}



/*
=====================
HUD_InitClientWeapons

Set up weapons, player and functions needed to run weapons code client-side.
=====================
*/
void HUD_InitClientWeapons( void )
{
	static int initialized = 0;
	if ( initialized )
		return;

	initialized = 1;
	g_psv_zmax = CVAR_GET_POINTER("sv_zmax");// XDM

	// Set up pointer ( dummy object )
	gpGlobals = &Globals;

	// Fill in current time ( probably not needed )
	gpGlobals->time = gEngfuncs.GetClientTime();

	// Fake functions
	g_engfuncs.pfnPrecacheModel		= stub_PrecacheModel;
	g_engfuncs.pfnPrecacheSound		= stub_PrecacheSound;
//	g_engfuncs.pfnPrecacheEvent		= stub_PrecacheEvent;
	g_engfuncs.pfnNameForFunction	= stub_NameForFunction;
	g_engfuncs.pfnSetModel			= stub_SetModel;
	g_engfuncs.pfnSetClientMaxspeed = HUD_SetMaxSpeed;

	// Handled locally
	g_engfuncs.pfnPlaybackEvent		= HUD_PlaybackEvent;
	g_engfuncs.pfnAlertMessage		= AlertMessage;

	// Pass through to engine
	g_engfuncs.pfnPrecacheEvent		= gEngfuncs.pfnPrecacheEvent;
	g_engfuncs.pfnRandomFloat		= gEngfuncs.pfnRandomFloat;
	g_engfuncs.pfnRandomLong		= stub_RandomLong;// gEngfuncs.pfnRandomLong;

	// Allocate a slot for the local player
	HUD_PrepEntity( &player		, NULL );
}

/*
=====================
HUD_GetLastOrg

Retruns the last position that we stored for egon beam endpoint.
=====================
*/
void HUD_GetLastOrg( float *org )
{
	int i;
	// Return last origin
	for (i = 0; i < 3; ++i)
		org[i] = previousorigin[i];
}

/*
=====================
HUD_SetLastOrg

Remember our exact predicted origin so we can draw the egon to the right position.
=====================
*/
void HUD_SetLastOrg( void )
{
	int i;
	// Offset final origin by view_offset
	for (i = 0; i < 3; ++i)
		previousorigin[i] = g_finalstate->playerstate.origin[i] + g_finalstate->client.view_ofs[i];
}

/*
=====================
HUD_WeaponsPostThink

Run Weapon firing code on client
=====================
*/
void HUD_WeaponsPostThink(local_state_s *from, local_state_s *to, usercmd_t *cmd, const double &time, const unsigned int &random_seed)
{
	int buttonsChanged;
	CBasePlayerWeapon *pWeapon = NULL;
	CBasePlayerWeapon *pCurrent = NULL;
//	weapon_data_t nulldata, *pto;
	static int lasthealth;

//	memset( &nulldata, 0, sizeof( nulldata ) );

	HUD_InitClientWeapons();	

	// Get current clock
	gpGlobals->time = time;

	// Store pointer to our destination entity_state_t so we can get our origin, etc. from it
	//  for setting up events on the client
	g_finalstate = to;

	// If we are running events/etc. go ahead and see if we
	//  managed to die between last frame and this one
	// If so, run the appropriate player killed or spawn function
	if ( g_runfuncs )
	{
		if ( to->client.health <= 0 && lasthealth > 0 )
			player.Killed( NULL, NULL, 0 );
		else if ( to->client.health > 0 && lasthealth <= 0 )
			player.Spawn();

		lasthealth = (int)to->client.health;
	}

	// We are not predicting the current weapon, just bow out here.
	if (pWeapon == NULL)
		return;

	int i;

	// For random weapon events, use this seed to seed random # generator
	player.random_seed = random_seed;

	// Get old buttons from previous state.
	player.m_afButtonLast = from->playerstate.oldbuttons;

	// Which buttsons chave changed
	buttonsChanged = (player.m_afButtonLast ^ cmd->buttons);	// These buttons have changed this frame
	
	// Debounced button codes for pressed/released
	// The changed ones still down are "pressed"
	player.m_afButtonPressed =  buttonsChanged & cmd->buttons;	
	// The ones not down are "released"
	player.m_afButtonReleased = buttonsChanged & (~cmd->buttons);

	// Set player variables that weapons code might check/alter
	player.pev->button = cmd->buttons;

	player.pev->velocity = from->client.velocity;
	player.pev->flags = from->client.flags;

	player.pev->deadflag = from->client.deadflag;
	player.pev->waterlevel = from->client.waterlevel;
	player.pev->maxspeed    = from->client.maxspeed;
	player.pev->fov = from->client.fov;
	player.pev->weaponanim = from->client.weaponanim;
	player.pev->viewmodel = from->client.viewmodel;
	player.m_flNextAttack = from->client.m_flNextAttack;

	// Point to current weapon object
	if ( from->client.m_iId )
		player.m_pActiveItem = g_pWpns[ from->client.m_iId ];


	
	// Don't go firing anything if we have died.
	// Or if we don't have a weapon model deployed
	if ( ( player.pev->deadflag != ( DEAD_DISCARDBODY + 1 ) ) && !CL_IsDead() && player.pev->viewmodel && !g_iUser1 )
	{
		if (player.m_flNextAttack <= 0)
			pWeapon->ItemPostFrame();
	}

	// Assume that we are not going to switch weapons
	to->client.m_iId					= from->client.m_iId;

	// Now see if we issued a changeweapon command ( and we're not dead )
	if ( cmd->weaponselect && ( player.pev->deadflag != ( DEAD_DISCARDBODY + 1 ) ) )
	{
		// Switched to a different weapon?
		if ( from->weapondata[ cmd->weaponselect ].m_iId == cmd->weaponselect )
		{
			CBasePlayerWeapon *pNew = g_pWpns[ cmd->weaponselect ];
			if ( pNew && ( pNew != pWeapon ) )
			{
				// Put away old weapon
				if (player.m_pActiveItem)
					player.m_pActiveItem->Holster();
				
				player.m_pLastItem = player.m_pActiveItem;
				player.m_pActiveItem = pNew;

				// Deploy new weapon
				if (player.m_pActiveItem)
					player.m_pActiveItem->Deploy();

				// Update weapon id so we can predict things correctly.
				to->client.m_iId = cmd->weaponselect;
			}
		}
	}

	// Copy in results of prediction code
	to->client.viewmodel				= player.pev->viewmodel;
	to->client.fov						= player.pev->fov;
	to->client.weaponanim				= player.pev->weaponanim;
	to->client.m_flNextAttack			= player.m_flNextAttack;
	to->client.fuser2					= pWeapon->m_flNextAmmoBurn;
	to->client.maxspeed					= player.pev->maxspeed;

	

	// Make sure that weapon animation matches what the game .dll is telling us
	//  over the wire ( fixes some animation glitches )
	if ( g_runfuncs && ( HUD_GetWeaponAnim() != to->client.weaponanim ) )
	{
		int body = 2;

		// Force a fixed anim down to viewmodel
		HUD_SendWeaponAnim( to->client.weaponanim, body, 1 );
	}

	weapon_data_t *pto;
	for (i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
	{
		pCurrent = g_pWpns[i];
		pto = &to->weapondata[i];
		if (!pCurrent)
		{
			memset(pto, 0, sizeof(weapon_data_t));
			continue;
		}
	
		pto->m_fInReload				= pCurrent->m_fInReload;
		pto->m_fInSpecialReload			= pCurrent->m_fInSpecialReload;
		pto->m_flPumpTime				= pCurrent->m_flPumpTime;
		pto->m_iClip					= pCurrent->m_iClip; 
		pto->m_flNextPrimaryAttack		= pCurrent->m_flNextPrimaryAttack;
		pto->m_flNextSecondaryAttack	= pCurrent->m_flNextSecondaryAttack;
		pto->m_flTimeWeaponIdle			= pCurrent->m_flTimeWeaponIdle;
		pto->fuser1						= pCurrent->pev->fuser1;
/*		pto->fuser2						= pCurrent->m_flStartThrow;
XDM		pto->fuser3						= pCurrent->m_flReleaseThrow;
		pto->iuser1						= pCurrent->m_chargeReady;
		pto->iuser2						= pCurrent->m_fInAttack;
		pto->iuser3						= pCurrent->m_fireState;*/
		// Decrement weapon counters, server does this at same time ( during post think, after doing everything else )
		pto->m_flNextReload				-= cmd->msec / 1000.0f;
		pto->m_fNextAimBonus			-= cmd->msec / 1000.0f;
		pto->m_flNextPrimaryAttack		-= cmd->msec / 1000.0f;
		pto->m_flNextSecondaryAttack	-= cmd->msec / 1000.0f;
		pto->m_flTimeWeaponIdle			-= cmd->msec / 1000.0f;
		pto->fuser1						-= cmd->msec / 1000.0f;

		to->client.vuser3[2]				= pCurrent->PrimaryAmmoIndex();
		to->client.vuser4[0]				= pCurrent->SecondaryAmmoIndex();
		to->client.vuser4[1]				= player.AmmoInventory(pCurrent->PrimaryAmmoIndex());
		to->client.vuser4[2]				= player.AmmoInventory(pCurrent->SecondaryAmmoIndex());

		if (pto->m_flPumpTime != -9999)
		{
			pto->m_flPumpTime -= cmd->msec / 1000.0f;
			if (pto->m_flPumpTime < -0.001)
				pto->m_flPumpTime = -0.001;
		}

		if (pto->m_fNextAimBonus < -1.0)
			pto->m_fNextAimBonus = -1.0;

		if (pto->m_flNextPrimaryAttack < -1.0)
			pto->m_flNextPrimaryAttack = -1.0;

		if (pto->m_flNextSecondaryAttack < -0.001)
			pto->m_flNextSecondaryAttack = -0.001;

		if (pto->m_flTimeWeaponIdle < -0.001)
			pto->m_flTimeWeaponIdle = -0.001;

		if (pto->m_flNextReload < -0.001)
			pto->m_flNextReload = -0.001;

		if (pto->fuser1 < -0.001)
			pto->fuser1 = -0.001;
	}

	// m_flNextAttack is now part of the weapons, but is part of the player instead
	to->client.m_flNextAttack -= cmd->msec / 1000.0f;
	if (to->client.m_flNextAttack < -0.001)
		to->client.m_flNextAttack = -0.001;

	to->client.fuser2 -= cmd->msec / 1000.0f;
	if (to->client.fuser2 < -0.001)
		to->client.fuser2 = -0.001;
	
	to->client.fuser3 -= cmd->msec / 1000.0f;
	if (to->client.fuser3 < -0.001)
		to->client.fuser3 = -0.001;

	// Store off the last position from the predicted state.
	HUD_SetLastOrg();

	// Wipe it so we can't use it after this frame
	g_finalstate = NULL;
}

/*
=====================
HUD_PostRunCmd

Client calls this during prediction, after it has moved the player and updated any info changed into to->
time is the current client clock based on prediction
cmd is the command that caused the movement, etc
runfuncs is 1 if this is the first time we've predicted this command.  If so, sounds and effects should play, otherwise, they should
be ignored
=====================
*/
void EXPORT HUD_PostRunCmd( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed )
{
	g_runfuncs = (runfuncs > 0);
	to->client.fov = g_lastFOV;

	// All games can use FOV state
	g_lastFOV = to->client.fov;
}
