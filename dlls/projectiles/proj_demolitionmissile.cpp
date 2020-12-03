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

LINK_ENTITY_TO_CLASS(DemolitionMissile, CDemolitionMissile);

CDemolitionMissile *CDemolitionMissile::ShootDemolitionMissile(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CDemolitionMissile *pNew = GetClassPtr((CDemolitionMissile *)NULL, "DemolitionMissile");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_DEMOLITION_MISSILE + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}

void CDemolitionMissile::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_DEMOLITION_MISSILE;
	pev->takedamage = DAMAGE_YES;
	pev->health = 15;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 3.0f);
	pev->impulse = 0;

	pev->nextthink = gpGlobals->time+0.15;
	SetTouch(&CDemolitionMissile::ExplodeTouch);
	SetThink(&CDemolitionMissile::Fly);
}

int CDemolitionMissile::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_DEMOLITIONMISSILE_BLAST);
	return 1;
}

void CDemolitionMissile::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.25, pev->dmg*1.5, TRUE, RANDOM_LONG(DECAL_BLASTSCORCH1,DECAL_BLASTSCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 32.0, FX_DEMOLITIONMISSILE_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CDemolitionMissile::Fly(void)
{
	if ( gpGlobals->time >= pev->framerate )
	{
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/fly_demolitionmissile.wav", VOL_NORM, ATTN_NORM);
		pev->framerate = gpGlobals->time + 1.7;
	}
	if (pev->impulse == 0)
	{
		FX_Trail(pev->origin, entindex(), FX_DEMOLITIONMISSILE );
		pev->velocity = pev->velocity * 1.25;
		pev->impulse = 1;
	}
	pev->origin.z = pev->origin.z + sin( pev->velocity.z + gpGlobals->time * 20 ) * 3;

	CGrenade::SearchTarget();
	CGrenade::SpeedUp();
	pev->nextthink = gpGlobals->time + 0.01;
}