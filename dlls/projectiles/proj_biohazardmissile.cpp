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

LINK_ENTITY_TO_CLASS(BioMissile, CBioMissile);

void CBioMissile::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_BIO_MISSILE;
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	pev->health = 10;
	pev->frags = 0;
	pev->impulse = 0;
	pev->gravity = 0.01;
	pev->friction = 0.5;
	UTIL_SetSize(this, 3.0f);

	SetThink(&CBioMissile::Fly);
	SetTouch(&CBioMissile::ExplodeTouch);
	pev->nextthink = gpGlobals->time + 0.2;
}

CBioMissile *CBioMissile::ShootBioMissile(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CBioMissile *pNew = GetClassPtr((CBioMissile *)NULL, "BioMissile");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_BIOHAZARD_MISSILE + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}


int CBioMissile::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.75, pev->dmg, DMG_POISON, FX_BIOHAZARDMISSILE_BLAST);
	return 1;
}

void CBioMissile::ExplodeTouch(CBaseEntity *pOther)
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pOther->IsMonster() || pOther->IsPlayer())
	{
		switch (RANDOM_LONG(0,1))
		{
			case 0:	CGrenade::Impact(pOther, TRUE, pev->dmg*0.25, pev->dmg*1.5, TRUE, RANDOM_LONG(DECAL_MDSCORCH1,DECAL_MDSCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_POISON, 12.0, FX_BIOHAZARDMISSILE_DETONATE); break;
			case 1:	CGrenade::Impact(pOther, TRUE, pev->dmg*0.25, pev->dmg*1.5, TRUE, RANDOM_LONG(DECAL_MDSCORCH1,DECAL_MDSCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 12.0, FX_BIOHAZARDMISSILE_DETONATE); break;
		}
		SetThink(&CBaseEntity::SUB_Remove);
	}
	else if (pOther->IsProjectile())
	{
		CGrenade::Impact(pOther, TRUE, 0, 0, FALSE, 0, pev->dmg*0.25, DMG_MORTAR, 0, 0, 0, 0, FX_BIOHAZARDMISSILE_DETONATE);
		SetThink(&CBaseEntity::SUB_Remove);
	}
	else if (pOther->IsPushable() || pOther->IsBreakable())
	{
		CGrenade::Impact(pOther, FALSE, 0, 0, TRUE, RANDOM_LONG(DECAL_MDSCORCH1,DECAL_MDSCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 0, 0);
		pev->velocity = pev->velocity*0.33;
		pev->gravity = 0.5;
		pev->avelocity.y = -1024.0;
		pev->body = PROJ_BODY_BIO_MISSILE_BROKEN;

		SetTouch(&CBioMissile::BounceTouch);
		SetThink(&CBioMissile::PoisonThink);
	}
	else
	{
		pev->velocity = pev->velocity*0.33;
		pev->gravity = 0.5;
		pev->avelocity.y = -1024.0;
		pev->body = PROJ_BODY_BIO_MISSILE_BROKEN;

		SetTouch(&CBioMissile::BounceTouch);
		SetThink(&CBioMissile::PoisonThink);
	}
	pev->nextthink = gpGlobals->time;
}

void CBioMissile::BounceTouch(CBaseEntity *pOther)
{
	if (pOther->edict() == pev->owner)
		return;

	if (pev->impacttime < gpGlobals->time && pev->velocity.Length() > 100)
	{
		CGrenade::Impact(pOther, FALSE, 0, 0, FALSE, 0, pev->dmg*0.15, DMG_MORTAR, 0, 0, 0, 0.0, 0);
		pev->impacttime = gpGlobals->time + 1.0f; // debounce
	}

	Vector vecTestVelocity(pev->velocity);
	vecTestVelocity.z *= 0.45f;

	if (pev->flags & FL_ONGROUND)
		pev->velocity = pev->velocity * 0.75f;
	else
		CGrenade::BounceSound();
}


void CBioMissile::PoisonThink(void)
{
	if (!IsInWorld())
	{
		FX_Trail( pev->origin, entindex(), FX_REMOVE );
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}

	if (pev->dmgtime <= gpGlobals->time)
	{
		if (pev->impulse > 0)
		{
			SetThink(&CBioMissile::PoisonThinkEnd);
		}
		else
		{
			pev->impulse = 1;
			pev->ltime = gpGlobals->time;// start stinkin'!
			pev->dmgtime = gpGlobals->time + 5;
			FX_Trail(pev->origin, entindex(), FX_BIOHAZARDMISSILE_SMOKE);
		}
	}
	if (pev->impulse > 0 && pev->ltime <= gpGlobals->time)
	{
		CGrenade::Impact(NULL, FALSE, 0, 0, FALSE, 0, 0, 0, pev->dmg*0.15, pev->dmg*3.0, DMG_POISON, 0.0, 0);
		pev->ltime = gpGlobals->time + 0.25f;
	}
	if ( gpGlobals->time >= pev->framerate )
	{
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/biohazardmissile_smoke.wav", VOL_NORM, ATTN_NORM);
		pev->framerate = gpGlobals->time + 1.1;
	}
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CBioMissile::PoisonThinkEnd(void)
{
	Vector vecSpot(pev->origin);
	if (pev->flags & FL_ONGROUND)// XDM: for RadiusDamage
		vecSpot.z += 16.0f;

	CGrenade::Blast(0, 0, pev->dmg*0.75, pev->dmg*1.5, DMG_POISON, FX_BIOHAZARDMISSILE_FINAL_BLAST);
}

void CBioMissile::Fly(void)
{
	if ( gpGlobals->time >= pev->framerate )
	{
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/fly_biohazardmissile.wav", VOL_NORM, ATTN_NORM);
		pev->framerate = gpGlobals->time + 1.5;
	}
	if (pev->frags == 0)
	{
		FX_Trail(pev->origin, entindex(), FX_BIOHAZARDMISSILE);
		pev->velocity = pev->velocity * 1.25;
		pev->frags = 1;
	}
	CGrenade::SearchTarget();
	CGrenade::SpeedUp();
	pev->nextthink = gpGlobals->time + 0.1;
}