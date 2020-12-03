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

LINK_ENTITY_TO_CLASS(GluonBall, CGluonBall);

void CGluonBall::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_SPHERE;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxFullBright;
	pev->renderamt = 25;
	pev->health = 10;
	pev->scale = 0.5;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	UTIL_SetSize(this, 3.0);
	pev->nextthink = gpGlobals->time;

	SetTouch(&CGluonBall::ExplodeTouch);
	SetThink(&CGluonBall::Fly);
}

CGluonBall *CGluonBall::ShootGluonBall(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CGluonBall *pNew = GetClassPtr((CGluonBall *)NULL, "GluonBall");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_GLUON_BALL  + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
//		pNew->pev->velocity = gpGlobals->v_forward * 450;

		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_GLUON );
	}
	return pNew;
}

int CGluonBall::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

	CGrenade::Blast(0, 0, pev->dmg*0.75, pev->dmg*1.25, DMG_ENERGYBLAST, FX_GLUON_BLAST);
	return 1;
}

void CGluonBall::ExplodeTouch(CBaseEntity *pOther)
{
	if (pOther->IsProjectile())
	{
		if (FClassnameIs(pOther->pev, "GluonBall") || FClassnameIs(pOther->pev, "DisruptorBall") || FClassnameIs(pOther->pev, "Teleporter") || FClassnameIs(pOther->pev, "LightningBall") || FClassnameIs(pOther->pev, "Scorcher") || FClassnameIs(pOther->pev, "PlasmaBall"))
		{
			CGrenade::Blast(pev->dmg*0.8, pev->dmg*4.0, pev->dmg*4.0, pev->dmg*6.0, DMG_ENERGYBLAST, FX_GLUON_COLLIDE);
			return;
		}
	}
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.2, pev->dmg*2.0, TRUE, RANDOM_LONG(DECAL_GLUONSCORCH1,DECAL_GLUONSCORCH3), 0, 0, pev->dmg, pev->dmg*2.5, DMG_ENERGYBLAST, 24.0, FX_GLUON_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CGluonBall::Fly( void )
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, pev->dmg*0.5, pev->dmg, DMG_ENERGYBLAST, FX_GLUON_BLAST);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}