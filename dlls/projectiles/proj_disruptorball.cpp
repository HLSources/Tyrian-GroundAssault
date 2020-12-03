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

LINK_ENTITY_TO_CLASS(DisruptorBall, CDisruptorBall);

void CDisruptorBall::Spawn(void)
{
	SET_MODEL(ENT(pev), "sprites/animglow01.spr");
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 25;
	pev->health = 10;
	pev->scale = 0.1;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	UTIL_SetSize(this, 0.0);

	SetTouch(&CDisruptorBall::ExplodeTouch);
	SetThink(&CDisruptorBall::Fly);
	pev->nextthink = gpGlobals->time+ 0.01;
}

CDisruptorBall *CDisruptorBall::ShootDisruptorBall(CBaseEntity *pOwner, const Vector &vecStart, int spin, float dmg)
{
	CDisruptorBall *pNew = GetClassPtr((CDisruptorBall *)NULL, "DisruptorBall");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_DISRUPTOR_BALL;
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->impulse = spin;
		pNew->pev->button = 0;

		if (spin == 0)
			pNew->pev->dmg = dmg;
		else
			pNew->pev->dmg = dmg*0.5;
	}
	return pNew;
}

int CDisruptorBall::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(0, 0, pev->dmg*0.75, pev->dmg*1.5, DMG_ENERGYBLAST, FX_DISRUPTOR_BLAST);
	return 1;
}

void CDisruptorBall::ExplodeTouch(CBaseEntity *pOther)
{
	if (pev->impulse == 0) //main charge
		CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*2.0, TRUE, RANDOM_LONG(DECAL_GLUONSCORCH1,DECAL_GLUONSCORCH3), 0, 0, pev->dmg, pev->dmg*2.5, DMG_ENERGYBLAST, 24.0, FX_DISRUPTOR_DETONATE);
	else
		CGrenade::Impact(pOther, TRUE, 0, 0, TRUE, RANDOM_LONG(DECAL_SMALLSCORCH1,DECAL_SMALLSCORCH3), 0, 0, pev->dmg, pev->dmg*2.5, DMG_ENERGYBLAST, 20.0, FX_DISRUPTOR_BLAST);
	
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}


void CDisruptorBall::Fly( void )
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_ENERGYBLAST, FX_DISRUPTOR_BLAST);
		return;
	}

	if (pev->button == 0)
	{
		if (pev->impulse == 0)
			FX_Trail(pev->origin, entindex(), FX_DISRUPTOR_MAIN);
		else
			FX_Trail(pev->origin, entindex(), FX_DISRUPTOR_BABY);

		pev->button = 1;
	}


	CGrenade::SearchTarget();

	if (pev->impulse == 1)
	{
		pev->origin.z = pev->origin.z + sin( pev->velocity.z + gpGlobals->time * 20 ) * 5;
		pev->origin.x = pev->origin.x + sin( pev->velocity.x + gpGlobals->time * 20 ) * 5;
		pev->origin.y = pev->origin.y + sin( pev->velocity.y + gpGlobals->time * 20 ) * 5;
	}
	else if (pev->impulse == 2)
	{
		pev->origin.z = pev->origin.z - sin( pev->velocity.z + gpGlobals->time * 20 ) * 5;
		pev->origin.x = pev->origin.x - sin( pev->velocity.x + gpGlobals->time * 20 ) * 5;
		pev->origin.y = pev->origin.y - sin( pev->velocity.y + gpGlobals->time * 20 ) * 5;
	}
	pev->nextthink = gpGlobals->time + 0.01;
}