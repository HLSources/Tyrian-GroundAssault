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

LINK_ENTITY_TO_CLASS(ShockLaser, CShockLaser);

void CShockLaser::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_SHOCK_LASER;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->rendermode = kRenderTransTexture;
	pev->renderamt = 200;
	pev->renderfx = kRenderFxFullBright;
	UTIL_SetSize(this, 2.0f);
	pev->nextthink = gpGlobals->time;

	SetTouch(&CShockLaser::ExplodeTouch);
	SetThink(&CShockLaser::Fly);
}

CShockLaser *CShockLaser::ShootShockLaser(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CShockLaser *pNew = GetClassPtr((CShockLaser *)NULL, "ShockLaser");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_SHOCK_LASER + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_SHOCK_LASER );
	}
	return pNew;
}

int CShockLaser::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	CGrenade::Blast(0, 0, 0, 0, 0, FX_SHOCK_LASER_BLAST);
	return 1;
}

void CShockLaser::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, 0, 0, TRUE, RANDOM_LONG(DECAL_SMALLSCORCH1,DECAL_SMALLSCORCH3), pev->dmg*0.5, DMG_SHOCK, pev->dmg*0.5, pev->dmg*2.0, DMG_SHOCK | DMG_RADIUS_MAX, 12.0, FX_SHOCK_LASER_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CShockLaser::Fly(void)
{
	if (POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, pev->dmg*2.0, pev->dmg*4.0, DMG_SHOCK, FX_SHOCK_LASER_BLAST);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}