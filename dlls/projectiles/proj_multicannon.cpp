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

LINK_ENTITY_TO_CLASS(MultiCannon, CMultiCannon);

void CMultiCannon::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->takedamage = DAMAGE_YES;
	pev->body = PROJ_BODY_MULTICANNON;
	pev->health = 2;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxFullBright;
	pev->renderamt = 100;
	UTIL_SetSize(this, 3.0f);
	pev->nextthink = gpGlobals->time;

	SetTouch(&CMultiCannon::ExplodeTouch);
	SetThink(&CMultiCannon::Fly);
}

CMultiCannon *CMultiCannon::ShootMultiCannon(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CMultiCannon *pNew = GetClassPtr((CMultiCannon *)NULL, "MultiCannon");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_MULTI_CANNON + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_MULTICANNON );
	}
	return pNew;
}

int CMultiCannon::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

		CGrenade::Blast(0, 0, 0, 0, 0, FX_MULTICANNON_BLAST);
	return 1;
}

void CMultiCannon::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, 0, 0, TRUE, RANDOM_LONG(DECAL_OFSCORCH1,DECAL_OFSCORCH3), pev->dmg, DMG_ENERGYBEAM, 0, 0, 0, 16.0, FX_MULTICANNON_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CMultiCannon::Fly(void)
{
	if (POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		CGrenade::Blast(0, 0, 0, 0, 0, FX_MULTICANNON_DETONATE);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}