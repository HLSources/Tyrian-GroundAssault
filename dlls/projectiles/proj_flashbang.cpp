//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "globals.h"
#include "game.h"
#include "projectiles.h"
#include "msg_fx.h"
#include "decals.h"

LINK_ENTITY_TO_CLASS(FlashBang, CFlashBang);

#define FLASHBANG_DETONATE_DELAY	3.5

CFlashBang *CFlashBang::ShootFlashBang(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CFlashBang *pNew = GetClassPtr((CFlashBang *)NULL, "FlashBang");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->dmg = dmg;
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_FLASHBANG + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
	}
	return pNew;
}

void CFlashBang::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_FLASHBANG;
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_SLIDEBOX;
	pev->gravity = 0.5;
	pev->friction = 0.8;
	pev->takedamage = DAMAGE_YES;
	pev->frags = gpGlobals->time;

	UTIL_SetSize(this, 2.0f);
	UTIL_SetOrigin(pev, pev->origin);
	pev->avelocity.y = RANDOM_FLOAT(-240.0, -320.0);

	SetThink(&CFlashBang::TimeThink);
	SetTouch(&CFlashBang::BounceTouch);
	pev->nextthink = gpGlobals->time;
}

void CFlashBang::TimeThink(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}
	if (gpGlobals->time - pev->frags >= FLASHBANG_DETONATE_DELAY)
	{
		Detonate();
		return;
	}
	pev->nextthink = gpGlobals->time;
}

void CFlashBang::BounceTouch(CBaseEntity *pOther)
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}
	if (pOther->edict() == pev->owner)
		return;

	Vector vecTestVelocity(pev->velocity);
	vecTestVelocity.z *= 0.45f;

	if (pev->flags & FL_ONGROUND)
		pev->velocity = pev->velocity * 0.85f;
	else
		CGrenade::BounceSound();
}

int CFlashBang::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg*4.0, DMG_BLIND | DMG_IGNOREARMOR, FX_FLASHBANG_BLAST);
	return 1;
}

void CFlashBang::Detonate(void)
{
	TraceResult tr;
	Vector v(0,0,0);
	if (pev->velocity == g_vecZero)
		v.z = -1.0f;
	else
		v = pev->velocity.Normalize()*16.0f;

	CGrenade::Blast(0, 0, pev->dmg, pev->dmg*8.0, DMG_BLIND | DMG_IGNOREARMOR, FX_FLASHBANG_DETONATE);
}

