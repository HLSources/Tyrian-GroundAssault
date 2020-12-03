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
//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
/*

===== explode.cpp ========================================================

  Explosion-related code

*/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "decals.h"
#include "weapons.h"
#include "explode.h"

//-----------------------------------------------------------------------------
// XDM: OBSOLETE! Keep for compatibility
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(spark_shower, CShower);

void CShower::Spawn(void)
{
	pev->velocity = RANDOM_FLOAT(200, 300) * pev->angles;
	pev->velocity.x += RANDOM_FLOAT(-100.f,100.f);
	pev->velocity.y += RANDOM_FLOAT(-100.f,100.f);
	if ( pev->velocity.z >= 0 )
		pev->velocity.z += 200.0f;
	else
		pev->velocity.z -= 200.0f;

	pev->movetype = MOVETYPE_BOUNCE;
	pev->gravity = 0.5;
	pev->solid = SOLID_NOT;
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
	pev->modelindex = g_iModelIndexAnimSpr10;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 160;

	pev->scale = 0.2;
	pev->speed = RANDOM_FLOAT(0.5, 1.5);
	pev->angles = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CShower::Think(void)
{
	UTIL_Sparks(pev->origin);
	pev->speed -= 0.1;

	if (pev->renderamt > 0)// XDM
	{
		pev->renderamt--;
		pev->effects |= EF_MUZZLEFLASH;
	}

	if (pev->speed > 0)
		pev->nextthink = gpGlobals->time + 0.1f;
	else
		UTIL_Remove(this);

	pev->flags &= ~FL_ONGROUND;
}

void CShower::Touch(CBaseEntity *pOther)
{
	if (pev->flags & FL_ONGROUND)
		pev->velocity = pev->velocity * 0.1f;
	else
		pev->velocity = pev->velocity * 0.6f;

	if ((pev->velocity.x*pev->velocity.x+pev->velocity.y*pev->velocity.y) < 10.0f)
		pev->speed = 0;
}



//-----------------------------------------------------------------------------
// XDM: CEnvExplosion now works as CGrenade and has SAME SPAWNFLAGS
//-----------------------------------------------------------------------------
TYPEDESCRIPTION	CEnvExplosion::m_SaveData[] =
{
	DEFINE_FIELD(CEnvExplosion, m_iMagnitude, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CEnvExplosion, CBaseMonster);

LINK_ENTITY_TO_CLASS(env_explosion, CEnvExplosion);

CEnvExplosion *CEnvExplosion::CreateExplosion(const Vector &origin, const Vector &angles, CBaseEntity *pOwner, int magnitude, int flags)
{
	CEnvExplosion *pExp = GetClassPtr((CEnvExplosion *)NULL, "env_explosion");
	if (pExp)
	{
	//	pExp->pev->classname = MAKE_STRING("env_explosion");
		if (pOwner)
			pExp->pev->owner = pOwner->edict();
		pExp->m_iMagnitude = magnitude;
		pExp->pev->spawnflags = flags;

		pExp->Spawn();
		UTIL_SetOrigin(pExp->pev, origin);
		pExp->pev->origin = origin;
		pExp->pev->angles = angles;
	}
	return pExp;
}

void CEnvExplosion::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "iMagnitude"))
	{
		m_iMagnitude = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CEnvExplosion::Spawn(void)
{
	pev->effects = EF_NODRAW;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_NOT;//intangible
	pev->model = iStringNull;//invisible
	SetThinkNull();
	SetTouchNull();
}

void CEnvExplosion::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	TraceResult tr;
	UTIL_TraceLine(pev->origin + Vector(0,0,8), pev->origin - Vector(0,0,8),  ignore_monsters, ENT(pev), &tr);
	m_hActivator = pActivator;// XDM3035b

	if (pev->spawnflags & SF_NUCLEAR)
	{
		pev->oldbuttons = 0;// counter
		SetThink(&CGrenade::NuclearExplodeThink);
		pev->nextthink = gpGlobals->time + 0.1;
	}
	else
	{
		pev->dmg = m_iMagnitude;
		Explode(&tr, DMG_BLAST);
	}
}

CEnvExplosion *ExplosionCreate(const Vector &center, const Vector &angles, CBaseEntity *pOwner, int magnitude, int flags, float delay)
{
	CEnvExplosion *pExplosion = CEnvExplosion::CreateExplosion(center, angles, pOwner, magnitude, flags);

	if (pExplosion)
	{
	//	if (!doDamage)
	//		pExplosion->pev->spawnflags |= SF_ENVEXPLOSION_NODAMAGE;

		if (delay <= 0.0)// XDM: may be faster/less laggy?
		{
			pExplosion->Use(NULL, NULL, USE_TOGGLE, 0);
		}
		else
		{
			pExplosion->SetThink(&CBaseEntity::SUB_CallUseToggle);
			pExplosion->pev->nextthink = gpGlobals->time + delay;
		}
	}
	return pExplosion;
}
