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

LINK_ENTITY_TO_CLASS(FrostBall, CFrostBall);

void CFrostBall::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->rendermode = kRenderTransTexture;
	pev->renderamt = 200;
	pev->body = PROJ_BODY_FROSTBALL;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->avelocity.z = 1024.0;
	UTIL_SetSize(this, 2.0f);
	pev->nextthink = gpGlobals->time;

	SetTouch(&CFrostBall::ExplodeTouch);
	SetThink(&CFrostBall::Fly);
}

CFrostBall *CFrostBall::ShootFrostBall(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CFrostBall *pNew = GetClassPtr((CFrostBall *)NULL, "FrostBall");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_FROST_BALL + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_FROSTBALL );
	}
	return pNew;
}


int CFrostBall::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	CGrenade::Blast(0, 0, 0, 0, 0, FX_FROSTBALL_BLAST);
	return 1;
}

void CFrostBall::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, 0, 0, TRUE, RANDOM_LONG(DECAL_FROSTSCORCH1,DECAL_FROSTSCORCH3), 0, 0, pev->dmg, pev->dmg*2.5, DMG_FREEZE, 20.0, FX_FROSTBALL_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CFrostBall::Fly( void )
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, 0, 0, 0, FX_FROSTBALL_BLAST);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}