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

LINK_ENTITY_TO_CLASS(HellHounder, CHellHounder);

CHellHounder *CHellHounder::ShootHellHounder(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CHellHounder *pNew = GetClassPtr((CHellHounder *)NULL, "HellHounder");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_HELLHOUNDER + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
	}
	return pNew;
}

void CHellHounder::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_HELLHOUNDER;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->movetype = MOVETYPE_FLYMISSILE;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 3.0f);
	pev->impulse = 0;

	pev->nextthink = gpGlobals->time+0.15;
	SetTouch(&CHellHounder::ExplodeTouch);
	SetThink(&CHellHounder::Fly);
}

int CHellHounder::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_BLAST, FX_HELLHOUNDER_BLAST);
	return 1;
}

void CHellHounder::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*1.5, TRUE, RANDOM_LONG(DECAL_SCORCH1,DECAL_SCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 24.0, FX_HELLHOUNDER_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CHellHounder::Fly(void)
{
	if ( gpGlobals->time >= pev->framerate )
	{
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/fly_hellhounder.wav", VOL_NORM, ATTN_NORM);
		pev->framerate = gpGlobals->time + 1.8;
	}
	if (pev->impulse == 0)
	{
		FX_Trail(pev->origin, entindex(), FX_HELLHOUNDER );
		pev->velocity = pev->velocity * 1.25;
		pev->impulse = 1;
	}
	pev->origin.z = pev->origin.z + sin( pev->velocity.z + gpGlobals->time * 15 ) * 3;
	pev->origin.x = pev->origin.x + sin( pev->velocity.x + gpGlobals->time * 15 ) * 3;
	pev->origin.y = pev->origin.y + sin( pev->velocity.y + gpGlobals->time * 15 ) * 3;

	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.05;
}