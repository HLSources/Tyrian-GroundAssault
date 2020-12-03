#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "globals.h"
#include "game.h"
#include "msg_fx.h"
#include "projectiles.h"
#include "decals.h"

LINK_ENTITY_TO_CLASS(StarBurst, CStarBurst);

CStarBurst *CStarBurst::ShootStarBurst(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CStarBurst *pNew = GetClassPtr((CStarBurst *)NULL, "StarBurst");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_STARBURST + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}

void CStarBurst::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->effects |= EF_NODRAW;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->frags = gpGlobals->time;
	UTIL_SetSize(this, 2.0f);

	pev->nextthink = gpGlobals->time;
	SetTouch(&CStarBurst::ExplodeTouch);
	SetThink(&CStarBurst::CheckTarget);
}

void CStarBurst::CheckTarget(void)
{
	TraceResult tr;
	Vector vecSpot = pev->origin;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 8192;
	UTIL_TraceHull(vecSpot, vecEnd, dont_ignore_monsters, large_hull, ENT(pev), &tr);

	if (tr.flFraction < 1.0)
	{
		CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);

		if (pHit->pev->takedamage == DAMAGE_NO)
		goto end;

		if (pHit == this || pHit->pev == pev)
		goto end;

		if (!pHit->IsAlive())
		goto end;

		if (pHit->edict() == pev->owner)
		goto end;

		if (pHit->IsProjectile())
		goto end;

		if (pHit->IsMonster() || pHit->IsPlayer())
		{
			UTIL_SetOrigin(pev, pHit->pev->origin);
			return;
		}
	}
	end:
	pev->nextthink = gpGlobals->time;
}

void CStarBurst::ExplodeTouch(CBaseEntity *pOther)
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}
	if (pOther->IsMonster() || pOther->IsPlayer() && pOther->pev->takedamage != DAMAGE_NO)
	{
		pev->movetype = MOVETYPE_FOLLOW;
		pev->solid = SOLID_NOT;
		pev->aiment = pOther->edict();
		pev->animtime = RANDOM_FLOAT(0.5f, 1.0f);
		SetTouchNull();

		SetThink(&CStarBurst::Follow);
		pev->nextthink = gpGlobals->time;
		return;
	}
	else
	{
		CGrenade::Impact(NULL, TRUE, 0, 0, TRUE, RANDOM_LONG(DECAL_GLUONSCORCH1,DECAL_GLUONSCORCH3), 0, 0, pev->dmg*0.5, pev->dmg*1.5, DMG_ENERGYBLAST, 20.0, FX_STARBURST_DETONATE);
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
	}
}

void CStarBurst::Follow(void)
{
	CBaseEntity *pAiment = CBaseEntity::Instance(pev->aiment);

	if (gpGlobals->time - pev->frags >= pev->animtime || (pAiment != NULL && !pAiment->IsAlive()))
	{
		SetThink(&CStarBurst::CallStrike);
		pev->nextthink = gpGlobals->time;
		return;
	}
	pev->nextthink = gpGlobals->time + 0.1;
}

void CStarBurst::CallStrike(void)
{
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin + Vector(0,0,512), ignore_monsters, edict(), &tr);

	if (tr.flFraction == 1.0)// if there is enough space from the top (no floor)
	{
		//draw "star rain" effect
		FX_Trail( pev->origin + Vector(0,0,384), entindex(), FX_STARBURST_PART1);
	}
	SetThink(&CStarBurst::Blast);
	pev->nextthink = gpGlobals->time+RANDOM_FLOAT(0.2, 0.4);
}

void CStarBurst::Blast(void)
{
	CGrenade::Blast(0, 0, pev->dmg, 384, DMG_DISINTEGRATING | DMG_RADIUS_MAX, FX_STARBURST_PART2);
}
