#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "game.h"
#include "gamerules.h"
#include "globals.h"

DLL_GLOBAL int g_iWeaponBoxCount = 0;// XDM3035: limit number of dropped boxes in world!


LINK_ENTITY_TO_CLASS(weaponbox, CWeaponBox);

TYPEDESCRIPTION	CWeaponBox::m_SaveData[] =
{
	DEFINE_ARRAY(CWeaponBox, m_rgAmmo, FIELD_INTEGER, MAX_AMMO_SLOTS),
	DEFINE_ARRAY(CWeaponBox, m_rgiszAmmo, FIELD_STRING, MAX_AMMO_SLOTS),
	DEFINE_ARRAY(CWeaponBox, m_rgpWeapons, FIELD_CLASSPTR, MAX_WEAPONS),
//	DEFINE_ARRAY(CWeaponBox, m_rgpWeapons, FIELD_EHANDLE, MAX_WEAPONS),
	DEFINE_FIELD(CWeaponBox, m_cAmmoTypes, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CWeaponBox, CBaseEntity);

const char szCWeaponBoxClassName[] = "weaponbox";

//-----------------------------------------------------------------------------
// Purpose: Remove some dropped weapons/boxes from the world to prevent overflows
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CleanupTemporaryWeapons(void)
{
	// remove some other box or just temporary entity to prevent overflow
	if (g_pGameRules->IsMultiplayer())
	{
		int maxboxes = max(32, gpGlobals->maxClients*2);// 2 dropped boxes per player should be enough
		if (g_iWeaponBoxCount > maxboxes)// || count_all_entities >= gpGlobals->maxEntities-256// XDM3035: remove some other box :P UNDONE: remove the oldest one!
		{
			CBaseEntity *pFound = NULL;
			CBaseEntity *pEntity = NULL;
			edict_t		*pEdict = INDEXENT(1);
//			while ((pEntity = (CWeaponBox *)UTIL_FindEntityByClassname(pEntity, szClassName)) != NULL)
			for (int i = 1; i < gpGlobals->maxEntities; ++i, ++pEdict)
			{
				if (!UTIL_IsValidEntity(pEdict))
					continue;

				if (pEdict->v.spawnflags & (SF_NOTREAL|SF_NORESPAWN))// OR // WARNING! This cycle WILL find attached/carried/packed weapons which are still marked like this! DO NOT DESTROY THEM!!
				{
					if (pEdict->v.impulse > 0 && pEdict->v.aiment == NULL)
					{
						pEntity = CBaseEntity::Instance(pEdict);
						if (pEntity)
						{
							if (pEntity->IsPlayerItem() || FClassnameIs(&pEdict->v, szCWeaponBoxClassName))
							{
								pFound = (CWeaponBox *)pEntity;
								break;
							}
						}
					}
				}
			}
			if (pFound)
			{
#ifdef _DEBUG
				ALERT(at_console, "CleanupTemporaryWeapons(): Removing %s %d to prevent overflow.\n", STRING(pEntity->pev->classname), pEntity->entindex());
#endif
				pFound->pev->health = 1.0f;// don't disintegrate
				pFound->Killed(g_pWorld, g_pWorld, GIB_DISINTEGRATE);// common method for safe removal
				pFound = NULL;
				return true;
			}
		}
	}
	return false;
}



CWeaponBox *CWeaponBox::CreateBox(CBaseEntity *pOwner, const Vector &vecOrigin, const Vector &vecAngles)
{
	CleanupTemporaryWeapons();

	CWeaponBox *pBox = GetClassPtr((CWeaponBox *)NULL, szCWeaponBoxClassName);
	if (pBox)
	{
		pBox->pev->spawnflags |= SF_NOTREAL|SF_NORESPAWN;
		pBox->Clear();// XDM3035b the whole thing for the sake of this
		pBox->pev->origin = vecOrigin;
		pBox->pev->angles = vecAngles;
		pBox->pev->oldorigin = vecOrigin;
		pBox->Spawn();
		pBox->pev->impulse = 1;// not placed by mapper
/*
		pBox->pev->effects = EF_BRIGHTFIELD;
		pBox->pev->rendermode = kRenderNormal;
		pBox->pev->renderamt = 255;
//		pBox->pev->scale = 2.0f;
*/
	//	UTIL_SetOrigin(pGrenade->pev, vecOrigin);
	//	pBox->pev->origin = vecOrigin;
	//	pBox->pev->angles = vecAngles;
//		if (pOwner)
//			pBox->pev->owner = pOwner->edict();

		++g_iWeaponBoxCount;// XDM: register me!
	}
	return pBox;
}

void CWeaponBox::KeyValue(KeyValueData *pkvd)
{
	if (m_cAmmoTypes < MAX_AMMO_SLOTS)
	{
		int count = atoi(pkvd->szValue);
		if (count > 0)
		{
			int iname = ALLOC_STRING(pkvd->szKeyName);
			PackAmmo(STRING(iname), count);
			m_cAmmoTypes++;// count this new ammo type.
		}
		else
			ALERT(at_console, "Warning: WeaponBox has bad amount of %s!\n", pkvd->szKeyName);

		pkvd->fHandled = TRUE;
	}
	else
		ALERT(at_console, "WeaponBox too full! only %d ammotypes allowed\n", MAX_AMMO_SLOTS);
}

void CWeaponBox::Spawn(void)
{
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;
	if (g_pGameRules->IsMultiplayer() && mp_wpnboxbrk.value > 0.0f)// XDM
		pev->takedamage = DAMAGE_YES;
	else
		pev->takedamage = DAMAGE_NO;

	pev->health = 200;
//	pev->scale = 1.0f;
	SET_MODEL(ENT(pev), "models/w_items.mdl");
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 48));// XDM
	UTIL_SetOrigin(pev, pev->origin);

	if (pev->spawnflags & SF_NOTREAL)
	{
		SetThink(&CWeaponBox::Kill);
		pev->nextthink = gpGlobals->time + WEAPONBOX_DESTROY_TIME;// XDM
	}
}

// CWeaponBox - Kill - the think function that removes the box from the world.
void CWeaponBox::Kill(void)
{
	SetTouchNull();
//	SetThinkNull();
	Clear();
	// remove the box
	pev->mins = Vector(-2,-2,-2);// XDM3035: for Disintegrate()
	pev->maxs = Vector(2,2,2);
	if (pev->health <= 0.0f)// box was destroyed
		Disintegrate();// XDM3035: allow box to be removed by common code ONLY HERE!!
	else
		UTIL_Remove(this);
}

// CWeaponBox - Touch: try to add my contents to the toucher if the toucher is a player.
void CWeaponBox::Touch(CBaseEntity *pOther)
{
	if (!(pev->flags & FL_ONGROUND))// ENT_IS_ON_FLOOR()?
	{
		if (pev->owner && (pOther->edict() == pev->owner))// XDM
			return;// Don't touch the player that has just dropped this box
	}

	if (!pOther->IsPlayer())// only players may touch a weaponbox.
	{
		// XDM3035 FL_ONGROUND is not set when touching sky
//		ALERT ( at_notice, "CWeaponBox::Touch() !pOther->IsPlayer()\n");
		Vector end = pev->origin + pev->velocity.Normalize() * 4.0f;// don't trace too far!
		//Vector end = pev->origin + Vector(0.0f, 0.0f, -8.0f);
		int pc = POINT_CONTENTS(end);// XDM
		if (pc == CONTENTS_SLIME || pc == CONTENTS_LAVA || pc == CONTENTS_SKY)
		{
			Kill();
//			SetTouchNull();
//			Disintegrate();// XDM3035
//			UTIL_Remove(this);
			return;
		}
/*		if (pOther->IsBSPModel() && pOther->pev->solid != SOLID_NOT)// FL_ONGROUND and pev->groundentity are not set yet
		{
			if (pev->origin != pev->oldorigin)//(pev->velocity != g_vecZero)
			{
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "items/weapondrop1.wav", VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(90,100));
	//			pev->velocity = g_vecZero;
			}
		}*/
		return;
	}

	if (!pOther->IsAlive())// no dead guys.
		return;

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;
	int i;

// dole out ammo
	for (i = 0; i < MAX_AMMO_SLOTS; ++i)
	{
		if (!FStringNull(m_rgiszAmmo[i]))
		{
			// there's some ammo of this type.
			pPlayer->GiveAmmo(m_rgAmmo[i], STRINGV(m_rgiszAmmo[i]), MaxAmmoCarry(STRING(m_rgiszAmmo[i])));
			//ALERT ( at_console, "Gave %d rounds of %s\n", m_rgAmmo[i], STRING(m_rgiszAmmo[i]) );
			// now empty the ammo from the weaponbox since we just gave it to the player
			m_rgiszAmmo[i] = iStringNull;
			m_rgAmmo[i] = 0;
		}
	}

// go through my weapons and try to give the usable ones to the player.
// it's important for the player to be given ammo first, so the weapons code doesn't refuse
// to deploy a better weapon that the player may pick up because he has no ammo for it.
	for (i = 0; i < MAX_WEAPONS; ++i)
	{
		if (m_rgpWeapons[i])
		{
			CBasePlayerItem *pItem = (CBasePlayerItem *)(CBaseEntity *)m_rgpWeapons[i];
/*			if (pItem->iFlags() & ITEM_FLAG_EXHAUSTIBLE)// XDM: don't add 'fake' weapons
			{
UNDONE			if (m_rgAmmo[GetAmmoIndex(pItem->pszAmmo1())] <= 0 &&
					m_rgAmmo[GetAmmoIndex(pItem->pszAmmo2())] <= 0)
					return 0;
			}*/

			if (g_pGameRules->CanHavePlayerItem(pPlayer, pItem))
			{
				if (pPlayer->AddPlayerItem(pItem))
					pItem->AttachToPlayer(pPlayer);
				else
					pItem->DestroyItem();
			}
			else
				pItem->DestroyItem();

			m_rgpWeapons[i] = NULL;// unlink this weapon from the box
		}
	}

	EMIT_SOUND(pOther->edict(), CHAN_ITEM, "items/gunpickup4.wav", VOL_NORM, ATTN_NORM);
	SetTouchNull();
//	--g_iWeaponBoxCount;// XDM3035: unregister me
	UTIL_Remove(this);// XDM3035: don't Kill()
}

// CWeaponBox - PackWeapon: Add this weapon to the box
BOOL CWeaponBox::PackWeapon(CBasePlayerItem *pWeapon)
{
	if (pWeapon == NULL || pWeapon->pev == NULL)
		return FALSE;

	// is one of these weapons already packed in this box?
	if (HasWeapon(pWeapon->GetID()))
	{
//		ALERT(at_aiconsole, "CWeaponBox::PackWeapon(%s) refused.\n", STRING(pWeapon->pev->classname));
		return FALSE;// box can only hold one of each weapon type
	}

	m_rgpWeapons[pWeapon->GetID()] = pWeapon;
	pWeapon->pev->spawnflags |= SF_NORESPAWN;// never respawn
	pWeapon->pev->origin = pev->origin;
	pWeapon->pev->movetype = MOVETYPE_FOLLOW;// TESTME
	pWeapon->pev->solid = SOLID_NOT;
	pWeapon->pev->effects = EF_NODRAW;
	pWeapon->pev->modelindex = 0;
//?	pWeapon->pev->model = iStringNull;
	pWeapon->pev->owner = edict();
	pWeapon->pev->aiment = edict();// NULL?
	pWeapon->SetThinkNull();// crowbar may be trying to swing again, etc.
	pWeapon->SetTouchNull();
	pWeapon->SetOwner(NULL);

//	ALERT(at_console, "CWeaponBox::PackWeapon(%s) ok\n", STRING(pWeapon->pev->classname));
	return TRUE;
}

// CWeaponBox - PackAmmo
bool CWeaponBox::PackAmmo(const char *szName, const int &iCount)
{
	int iMaxCarry;
	if (szName == NULL)
	{
		// error here
		ALERT(at_console, "CWeaponBox::PackAmmo(): NULL String in PackAmmo!\n");
		return false;
	}
	iMaxCarry = MaxAmmoCarry(szName);
	if (iMaxCarry != -1 && iCount > 0)
	{
		//ALERT(at_console, "CWeaponBox::PackAmmo(): Packed %d rounds of %s\n", iCount, STRING(iszName));
		StoreAmmo(iCount, szName, iMaxCarry);// XDM30345b: miraclous misleading call!
		return true;
	}
	return false;
}

// CWeaponBox - XDM3035b: revisited! Was this actually called?
int CWeaponBox::StoreAmmo(const int &iCount, const char *szName, const int &iMax, int *pIndex/* = NULL*/)
{
	if (szName == NULL)
		return -1;

	int i;
	for (i = 0; (i < MAX_AMMO_SLOTS) && !FStringNull(m_rgiszAmmo[i]); ++i)// XDM: start from 0
	{
		if (stricmp(szName, STRING(m_rgiszAmmo[i])) == 0)
		{
			if (pIndex)
				*pIndex = i;

			int iAdd = min(iCount, iMax - m_rgAmmo[i]);
			if (iCount == 0 || iAdd > 0)
			{
				m_rgAmmo[i] += iAdd;
				return i;
			}
			return -1;
		}
	}
	if (i < MAX_AMMO_SLOTS)
	{
		if (pIndex)
			*pIndex = i;

		m_rgiszAmmo[i] = MAKE_STRING(szName);// probably safe?
		m_rgAmmo[i] = iCount;
		return i;
	}
//#ifdef _DEBUG// XDM
//	ALERT(at_console, "CWeaponBox: out of named ammo slots\n");
//#endif
	return i;
}

// XDM
int CWeaponBox::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pev->takedamage == DAMAGE_NO)
		return 0;

	if ((pev->spawnflags & SF_NORESPAWN) && pev->impulse > 0)// dropped by somebody
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

void CWeaponBox::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)// XDM3035c
{
	Kill();
}


/*
void CWeaponBox::SetObjectCollisionBox( void )
{
	pev->absmin = pev->origin + Vector(-16, -16, 0);
	pev->absmax = pev->origin + Vector(16, 16, 16);
}
*/

// XDM3035: allow players to grab weapon boxes (in case box is stuck somewhere visually reachable)
void CWeaponBox::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (pActivator->IsPlayer())
	{
		if (FVisible(pActivator))
			this->Touch(pActivator);
	}
}

// CWeaponBox::HasWeapon - is a weapon of this type already packed in this box?
bool CWeaponBox::HasWeapon(const int &iID)// XDM3035b: explicitly check by ID and NOT by pointer!
{
	return (m_rgpWeapons[iID] != NULL);
}

// CWeaponBox::IsEmpty - is there anything in this box?
bool CWeaponBox::IsEmpty(void)
{
	int i;
	for (i = 0; i < MAX_WEAPONS; ++i)
	{
		if (m_rgpWeapons[i])
			return false;
	}
	for (i = 0; i < MAX_AMMO_SLOTS; ++i)
	{
		if (!FStringNull(m_rgiszAmmo[i]))
			return false;// still have a bit of this type of ammo
	}
	return true;
}

// XDM3035b: more safety?
void CWeaponBox::Clear(void)
{
	int i;
	for (i = 0; i < MAX_AMMO_SLOTS; ++i)
	{
		m_rgiszAmmo[i] = 0;
		m_rgAmmo[i] = 0;
	}
	m_cAmmoTypes = 0;
	CBasePlayerItem *pWeapon = NULL;
	for (i = 0; i < MAX_WEAPONS; ++i)
	{
		pWeapon = (CBasePlayerItem *)(CBaseEntity *)m_rgpWeapons[i];
		if (pWeapon && /*pWeapon->IsPlayerItem() && */pWeapon->pev)// XDM3035b: catch all possible memory violations??
		{
//			pWeapon->SetOwner(NULL);
//			pWeapon->SetThinkNull();
//			pWeapon->SetTouchNull();
// already			pWeapon->pev->effects = EF_NODRAW;
			pWeapon->pev->movetype = MOVETYPE_NONE;
			pWeapon->pev->owner = NULL;
			pWeapon->pev->aiment = NULL;
			pWeapon->pev->targetname = 0;
			pWeapon->pev->target = 0;
			pWeapon->pev->flags = FL_KILLME;
			pWeapon->pev->health = 0.0f;
//same			UTIL_Remove(
//			REMOVE_ENTITY(ENT(pWeapon->pev));// CRASH
		}
		m_rgpWeapons[i] = NULL;// XDM3035b: !
	}
}

void CWeaponBox::OnFreePrivateData(void)// XDM3035b
{
	// destroy the weapons
//try this commented
	if (g_pGameRules && !g_pGameRules->IsGameOver())
		Clear();

	--g_iWeaponBoxCount;// XDM3035: unregister me (safe to use global variable here)
}
