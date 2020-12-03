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

LINK_ENTITY_TO_CLASS(MicroMissile, CMicroMissile);

CMicroMissile *CMicroMissile::ShootMicroMissile(CBaseEntity *pOwner, const Vector &vecStart, CBaseEntity *pEnemy, float spread, float dmg)
{
	CMicroMissile *pNew = GetClassPtr((CMicroMissile *)NULL, "MicroMissile");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->m_hEnemy = pEnemy;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_MICRO_MISSILE + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_MICROMISSILE );
	}
	return pNew;
}


void CMicroMissile::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_MICRO_MISSILE;
	pev->takedamage = DAMAGE_YES;
	pev->health = 12;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 2.0f);

	pev->nextthink = gpGlobals->time+0.15;
	SetTouch(&CMicroMissile::ExplodeTouch);
	SetThink(&CMicroMissile::Fly);
}

int CMicroMissile::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, 0, 0, 0, FX_MICROMISSILE_BLAST);
	return 1;
}

void CMicroMissile::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.33, pev->dmg*2.0, TRUE, RANDOM_LONG(DECAL_EXPSCORCH1,DECAL_EXPSCORCH3), pev->dmg*0.25, DMG_MORTAR, pev->dmg, pev->dmg*2.0, DMG_BLAST, 20.0, FX_MICROMISSILE_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CMicroMissile::Fly(void)
{
	if (m_hEnemy != NULL)
	{
		if (m_hEnemy->IsAlive())
			CGrenade::MovetoTarget(m_hEnemy->Center(), SPEED_MICRO_MISSILE);
		else
			m_hEnemy = NULL;
	}
	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->nextthink = gpGlobals->time + 0.05;
}

