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

#define HVRM_DETONATE_DELAY	10.0
#define HVRM_MAX_VELOCITY	2000
#define HVRM_MAX_DIST		8192

LINK_ENTITY_TO_CLASS(HVRMissile, CHVRMissile);

CHVRMissile *CHVRMissile::ShootHVRMissile(CBaseEntity *pOwner, const Vector &vecStart, float dmg)
{
	CHVRMissile *pNew = GetClassPtr((CHVRMissile *)NULL, "HVRMissile");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_HVR_MISSILE;
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}


void CHVRMissile::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_HVR_MISSILE;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 3.0f);
	pev->impulse = 0;
	pev->frags = gpGlobals->time;

	pev->nextthink = gpGlobals->time+0.15;
	SetTouch(&CHVRMissile::ExplodeTouch);
	SetThink(&CHVRMissile::Fly);
}

int CHVRMissile::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_HVRMISSILE_BLAST);
	return 1;
}

void CHVRMissile::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.4, pev->dmg*1.75, TRUE, RANDOM_LONG(DECAL_MDSCORCH1,DECAL_MDSCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.2, DMG_BLAST, 28.0, FX_HVRMISSILE_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CHVRMissile::Fly(void)
{
	if (gpGlobals->time - pev->frags >= HVRM_DETONATE_DELAY)
	{
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_HVRMISSILE_BLAST);
		return;
	}

	TraceResult tr;
	Vector a, forward;
	a = pev->angles;
	a.x = -a.x;
	ANGLE_VECTORS(a, forward, NULL, NULL);

	Vector vecTarget = forward;

	if (!FNullEnt(pev->enemy))
	{
		CBaseEntity *pOther = CBaseEntity::Instance(pev->enemy);
		if (pOther)
		{
			Vector vecDir;
			float flDist, flDot;
			float flMax = HVRM_MAX_DIST;
			UTIL_TraceLine(pev->origin, pOther->Center(), dont_ignore_monsters, ignore_glass, ENT(pev), &tr);

			if (tr.flFraction >= 0.90)
			{
				vecDir = pOther->Center() - pev->origin;
				flDist = vecDir.Length();
				vecDir = vecDir.Normalize();
				flDot = DotProduct(forward, vecDir);
				if ((flDot > 0.0f) && (flDist * (1.0f - flDot) < flMax))
				{
					flMax = flDist * (1.0f - flDot);
					vecTarget = vecDir;
				}
			}
			pev->angles = UTIL_VecToAngles(vecTarget);
		}
	}

	float flSpeed = pev->velocity.Length();
	pev->velocity = pev->velocity * 0.2f + vecTarget * (flSpeed * 0.8f + 400.0f);

	if (pev->velocity.Length() > HVRM_MAX_VELOCITY)
		pev->velocity = pev->velocity.Normalize()*HVRM_MAX_VELOCITY;

	if (pev->impulse == 0)
	{
		FX_Trail(pev->origin, entindex(), FX_HVRMISSILE);
		pev->impulse = 1;
	}

	if ( gpGlobals->time >= pev->framerate )
	{
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/fly_hvrmissile.wav", VOL_NORM, ATTN_NORM);
		pev->framerate = gpGlobals->time + 1.3;
	}
	pev->nextthink = gpGlobals->time + 0.05f;
}
