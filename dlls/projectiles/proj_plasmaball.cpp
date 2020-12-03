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
#include "sound.h"

LINK_ENTITY_TO_CLASS(PlasmaBall, CPlasmaBall);

void CPlasmaBall::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_PLASMA;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxFullBright;
	pev->renderamt = 255;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	UTIL_SetSize(this, 3.0f);
	pev->avelocity.z = -1024.0;
	pev->nextthink = gpGlobals->time;

	SetTouch(&CPlasmaBall::ExplodeTouch);
	SetThink(&CPlasmaBall::Fly);
}

CPlasmaBall *CPlasmaBall::ShootPlasmaBall(CBaseEntity *pOwner, const Vector &vecSrc, float spread, float dmg)
{
	CPlasmaBall *pNew = GetClassPtr((CPlasmaBall *)NULL, "PlasmaBall");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecSrc;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_PLASMA_BALL + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_PLASMA);
	}
	return pNew;
}

int CPlasmaBall::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

		CGrenade::Blast(0, 0, 0, 0, 0, FX_PLASMA_BLAST);
	return 1;
}

void CPlasmaBall::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, 0, 0, FALSE, 0, 0, 0, pev->dmg, pev->dmg*1.5, DMG_PLASMA | DMG_RADIUS_MAX, 0.0, 0);

	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin + pev->velocity.Normalize() * 64, dont_ignore_monsters, ENT(pev), &tr );	
	FX_FireBeam( pev->origin, tr.vecEndPos, tr.vecPlaneNormal, CHAR_TEX_CONCRETE, BULLET_PLASMABALL, FALSE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CPlasmaBall::Fly(void)
{
	if (POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, 0, 0, 0, FX_PLASMA_BLAST);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}