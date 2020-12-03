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

LINK_ENTITY_TO_CLASS(meteor, CMeteor);

void CMeteor::Spawn(void)
{
	Precache();
	SET_MODEL(ENT(pev), "models/effects/ef_meteor.mdl");
	pev->body = RANDOM_LONG(0, GetEntBodyCount(edict())-1);
	pev->scale = RANDOM_FLOAT(0.5, 3.0);
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	pev->health = RANDOM_LONG(100,200);
	pev->impulse = 0;
	UTIL_SetSize(this, 16.0f);
	pev->avelocity.x = 256.0;
	SetTouch(&CMeteor::ExplodeTouch);
	pev->nextthink = gpGlobals->time+0.5;
	SetThink(&CMeteor::Fly);
}

void CMeteor::Precache(void)
{
	PRECACHE_MODEL("models/effects/ef_meteor.mdl");
	if (pev->skin == 2)
	{
		PRECACHE_SOUND("weapons/meteor_hit_frozen1.wav");
		PRECACHE_SOUND("weapons/meteor_hit_frozen2.wav");
		PRECACHE_SOUND("weapons/meteor_hit_frozen3.wav");
		PRECACHE_SOUND("weapons/meteor_hit_frozen4.wav");
	}
	else
	{
		PRECACHE_SOUND("weapons/meteor_hit1.wav");
		PRECACHE_SOUND("weapons/meteor_hit2.wav");
		PRECACHE_SOUND("weapons/meteor_hit3.wav");
		PRECACHE_SOUND("weapons/meteor_hit4.wav");
	}
}

CMeteor *CMeteor::Shoot(CBaseEntity *pOwner, const Vector &vecSrc, const Vector &vecVel, float spread, float dmg, int type, BOOL UseGravity)
{
	CMeteor *pNew = GetClassPtr((CMeteor *)NULL, "meteor");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecSrc;
		pNew->pev->velocity = vecVel + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = pNew->pev->scale*dmg;
		pNew->pev->skin = type;

		if (UseGravity)
		{
			pNew->pev->movetype = MOVETYPE_TOSS;
			pNew->pev->gravity = 0.25;
		}
		else
		{
			pNew->pev->movetype = MOVETYPE_FLY;
			pNew->pev->gravity = 0;
		}
	}
	return pNew;
}

void CMeteor::Blast(void)
{
	if (pev->skin == 0)
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_METEOR_BLAST);
	else if (pev->skin == 1)
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BURN, FX_METEOR_FIRE_BLAST);
	else
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_FREEZE, FX_METEOR_FROZEN_BLAST);
}

int CMeteor::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (bitsDamageType & DMG_ANNIHILATION)
	{
		pev->velocity = g_vecZero;
		SetTouchNull();
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return 0;
	}

	pev->health -= flDamage;

	if (pev->health <= 0)
		Blast();
	return 1;
}


void CMeteor::ExplodeTouch(CBaseEntity *pOther)
{
	if (FClassnameIs(pOther->pev, "meteor"))
		goto end;

	if (pOther->IsProjectile())
	{
		Blast();
		return;
	}
end:
	if (pev->skin == 0)
		CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*2.5, TRUE, DECAL_GARGSTOMP1, pev->dmg*0.25, DMG_SLASH, pev->dmg, pev->dmg*1.5, DMG_BLAST, 24.0, FX_METEOR_DETONATE);
	else if (pev->skin == 1)
		CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*2.5, TRUE, RANDOM_LONG(DECAL_SCORCH1, DECAL_SCORCH3), pev->dmg*0.25, DMG_SLASH, pev->dmg, pev->dmg*1.5, DMG_BURN, 24.0, FX_METEOR_FIRE_DETONATE);
	else
		CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*2.5, TRUE, RANDOM_LONG(DECAL_FROSTSCORCH1, DECAL_FROSTSCORCH3), pev->dmg*0.25, DMG_SLASH, pev->dmg, pev->dmg*1.5, DMG_FREEZE, 24.0, FX_METEOR_FROZEN_DETONATE);

	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CMeteor::Fly(void)
{
	if (pev->impulse == 0)
	{
		if (pev->skin == 0)
			FX_Trail(pev->origin, entindex(), FX_METEOR);
		else if (pev->skin == 1)
			FX_Trail(pev->origin, entindex(), FX_METEOR_FIRE);
		else
			FX_Trail(pev->origin, entindex(), FX_METEOR_FROZEN);

		pev->impulse = 1;
	}
	pev->origin.z = pev->origin.z + sin( pev->velocity.z + gpGlobals->time * 20 ) * 3;
	pev->nextthink = gpGlobals->time + 0.025;
}